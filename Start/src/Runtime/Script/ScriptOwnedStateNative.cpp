#include <Runtime/ScriptOwnedStateNative.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <bit>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 5B6074 -> 4D7230：无cookie/长度读取，正文释放只传一个地址。
class PairControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PairControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete[](payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(PairControl) == 0x18);

class WaitControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit WaitControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{0x0C}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(WaitControl) == 0x18);

template<class Control, class Body>
void Publish(YtlReferenceSlotRuntime& slot, Body* body, std::uint32_t stride,
    std::uint32_t count, IEnvironmentRootMemory& memory)
{
    // 4C8F49/4C907E/4C91C0：重新取control，只删除，不重复释放正文。
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    Control* control = nullptr;
    if constexpr (std::is_same_v<Control, PairControl> || std::is_same_v<Control, WaitControl>)
        control = raw ? ::new (raw) Control(body) : nullptr;
    else
        control = raw ? ::new (raw) Control(body, typename Control::UninitializedFieldsTag{}) : nullptr;
    slot.controlAt08 = control;
    // 原null control随后AV；显式终止边界不补空Ref，也不自动回收已发布body。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = count;
}

template<class T> T& Consume(T* pointer)
{
    if (!pointer) throw std::logic_error("CScript native body is null at consumption (modern safety boundary)");
    return *pointer;
}
}

ScriptEffectNative::~ScriptEffectNative() noexcept
{
    // 4C9FA0：先pairs Ref，后CString；本层EH以terminate收束，不能让Free正常外传。
    ReleaseYtlReference(pairsAt08);
    DestroyRuntimeCStringAt40CD40(&resourceNameAt04);
}

void CreateScriptEffectAt4C8ECC(YtlReferenceSlotRuntime& effect,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x14);
    auto* body = raw ? ::new (raw) ScriptEffectNative(ScriptEffectShellTag{}) : nullptr;
    if (body)
    {
        // GetNil失败时旧effect仍在，本次raw未取得清理资格，不偷偷以RAII回收。
        InitializeEmptyRuntimeCString(body->resourceNameAt04, strings);
        body->pairsAt08 = {};
    }
    ReleaseYtlReference(effect);
    if (body) Publish<EnvironmentObjectControl<ScriptEffectNative>>(effect, body, 0x14, 1, memory);
}

void CreateScriptWaitAt4C917D(YtlReferenceSlotRuntime& wait, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x0C);
    // 无括号default-init：4C917D..4C9211没有对正文三个DWORD的写入。
    auto* body = raw ? ::new (raw) ScriptWaitNative : nullptr;
    ReleaseYtlReference(wait);
    if (body) Publish<WaitControl>(wait, body, 0x0C, 1, memory);
}

void CreateScriptEffectPairsAt4C9025(ScriptEffectNative& effect, std::int32_t count,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(effect.pairsAt08);
    if (count <= 0) return;
    const auto length = static_cast<std::uint32_t>(count);
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    const bool overflow = length > maximum / 8u;
    const auto bytes = overflow ? maximum : length * 8u;
    auto* raw = ::operator new[](bytes);
    if (raw && overflow)
    {
        // 分配叶已见原UINT_MAX请求；原后续遍历越界，现代实现在发布前明确拒绝。
        ::operator delete[](raw);
        throw std::bad_array_new_length{};
    }
    auto* body = static_cast<ScriptUpdatePairNative*>(raw);
    if (body)
        for (std::uint32_t i = 0; i < length; ++i)
        {
            ::new (body + i) ScriptUpdatePairNative;
            body[i].first = 0;
            body[i].second = 0;
        }
    // 4C9077原null数组也会发布control/count，不将它改成跳过或空表。
    Publish<PairControl>(effect.pairsAt08, body, 8, length, memory);
}

std::int32_t ScriptPairCount(const ScriptEffectNative& effect) noexcept
{
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(effect.pairsAt08.controlAt08);
    return control ? std::bit_cast<std::int32_t>(control->countAt10) : 0;
}

ScriptUpdatePairNative* ResolveScriptPair(const ScriptEffectNative& effect, std::uint32_t index) noexcept
{
    const auto& slot = effect.pairsAt08;
    if (!slot.baseAt00) return nullptr;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto offset = (slot.biasAt04 + index) * (control ? control->strideAt0C : 0u);
    return reinterpret_cast<ScriptUpdatePairNative*>(slot.baseAt00 + offset);
}

void ResetScriptEffectAt4CD180(ScriptEffectNative& effect)
{
    const std::uint32_t header = 0xFFFF;
    std::memcpy(&effect.status, &header, sizeof(header));
    SetRuntimeCStringAt437680(effect.resourceNameAt04, "", 0);
    for (std::int32_t i = 0; i < ScriptPairCount(effect); ++i)
    {
        Consume(ResolveScriptPair(effect, static_cast<std::uint32_t>(i))).second = 0;
        Consume(ResolveScriptPair(effect, static_cast<std::uint32_t>(i))).first = 0;
    }
}

std::size_t ScriptEffectNative::PairCount() const noexcept
{
    return static_cast<std::uint32_t>(ScriptPairCount(*this));
}

const ScriptUpdatePairNative& ScriptEffectNative::PairAt(std::size_t index) const
{
    if (index >= PairCount()) throw std::out_of_range("CScript effect pair index (modern safety boundary)");
    return Consume(ResolveScriptPair(*this, static_cast<std::uint32_t>(index)));
}

ScriptUpdatePairNative& ScriptEffectNative::PairAt(std::size_t index)
{
    return const_cast<ScriptUpdatePairNative&>(static_cast<const ScriptEffectNative&>(*this).PairAt(index));
}

void ScriptEffectNative::SetResourceName(const char* name)
{
    // 4D2B04..4D2B31：null与空串都传length=0；不经现代std::string临时分配。
    SetRuntimeCStringAt437680(resourceNameAt04, name, name ? static_cast<int>(std::strlen(name)) : 0);
}

void ScriptEffectNative::Reset() { ResetScriptEffectAt4CD180(*this); }

void InitializeScriptEffectAndWaitAt4C8FA1(YtlReferenceSlotRuntime& effect,
    YtlReferenceSlotRuntime& wait, ParamsRuntimeRootOwner& params, IEnvironmentRootMemory& memory)
{
    const auto& gameSystem = params.GetOrCreateAt418480().GameSystem();
    if (!gameSystem) throw std::logic_error("CScript native initialization GameSystem is null (modern safety boundary)");
    const auto count = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(gameSystem->startupConfigBytes.size()));
    CreateScriptEffectPairsAt4C9025(Consume(ResolveEnvironmentObjectReference<ScriptEffectNative>(effect)), count, memory);
    const auto bytes = static_cast<std::uint32_t>(ScriptPairCount(
        Consume(ResolveEnvironmentObjectReference<ScriptEffectNative>(effect))));
    auto* data = ResolveScriptPair(Consume(ResolveEnvironmentObjectReference<ScriptEffectNative>(effect)), 0);
    if (bytes) std::memset(&Consume(data), 0, bytes); // 原4C914D仅count字节，不是count*8。
    ResetScriptEffectAt4CD180(Consume(ResolveEnvironmentObjectReference<ScriptEffectNative>(effect)));
    CreateScriptWaitAt4C917D(wait, memory);
}
}
