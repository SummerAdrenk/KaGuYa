#include <Runtime/VoiceBatchNative.h>
#include <Runtime/ScrVmVoiceHandler.h>
#include <Runtime/ScriptDispatchNative.h>

#include <bit>
#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredStateControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredStateControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役后的base control按14h删除。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
class StateControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit StateControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    // 原5AE3D0=43BAA0/410B40；无cookie、无逐元素析构。
    void DestroyPayloadAtVslot00() override { ::operator delete[](payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 410B40：flags=0也先退役vptr，其余字段不变；本次派生释放实参仍为18h。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredStateControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(StateControl) == 24);
struct StateReference
{
    YtlReferenceSlotRuntime slot{};
    ~StateReference() noexcept { ReleaseYtlReference(slot); }
    void Build(std::int32_t count)
    {
        // 4127B0：signed非正数不分配；乘法溢出饱和，正文不清零。
        ReleaseYtlReference(slot);
        if (count <= 0) return;
        const auto n = static_cast<std::uint32_t>(count);
        const auto bytes = n > 0x3FFFFFFFu ? 0xFFFFFFFFu : n * 4u;
        void* body = ::operator new[](bytes);
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0;
        slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* raw = ::operator new(24);
        auto* control = raw ? ::new (raw) StateControl(body) : nullptr;
        slot.controlAt08 = control;
        if (!control) throw std::bad_alloc{}; // 原null写入的显式安全拒绝，保留孤立正文。
        control->referenceCountAt04 = 1;
        slot.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = n;
    }
    std::uint32_t& At(std::uint32_t index, YtlReferenceControlRuntime* captured)
    {
        if (!slot.baseAt00) throw std::logic_error("VOICE null state array (modern safety boundary)");
        auto* control = static_cast<EnvironmentObjectReferenceControl*>(captured);
        const auto offset = (slot.biasAt04 + index) * (control ? control->strideAt0C : 0u);
        return *reinterpret_cast<std::uint32_t*>(slot.baseAt00 + offset);
    }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        slot = {};
        YtlReferenceSlotRuntime normal{0, 0, captured};
        ReleaseScriptDispatchCommandAt4D40C6(normal);
    }
};
union TextSlot { RuntimeCString value; TextSlot() noexcept {} ~TextSlot() noexcept {} };
struct TextUnwind
{
    RuntimeCString* value;
    ~TextUnwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
};
std::int32_t Count(const RuntimeCStringArrayReference& names) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(names.Size())); }
}
bool PlayVoiceBatchAt42C4F0(IScrVmVoiceRuntimeBoundary& voice, const RuntimeCStringArrayReference& names)
{
    if (!voice.HasPlaybackSlots() || !voice.IsBatchObjectReadyAtA4()) return true;
    const auto count = Count(names); // 42C562：先捕获数量，再读当前配置。
    if (count > voice.VoiceBatchCapacityAt42C599()) return false;
    StateReference states;
    states.Build(Count(names)); // 42C5C9重读；容量回调可替换输入control。
    voice.StopAndClearAllVoiceSlots();
    auto* const captured = states.slot.controlAt08; // 42C5EA在Stop之后捕获。
    for (std::int32_t i = 0; i < Count(names); ++i)
    {
        states.At(i, captured) = 0;
        if (!voice.PrepareSlotAtA8(i, names.ResourceNameAt(i)))
        { states.At(i, captured) = 1; continue; }
        auto value = voice.ReadSlotValueAtAC(i, names.ResourceNameAt(i));
        if (value < 0)
        {
            // +6C没有name/index形参；不为此窄catch额外解址CString。
            try { value = voice.ReadFallbackValueAt6C(i, {}); }
            catch (...) { continue; }
            if (value < 0) continue;
        }
        TextSlot held;
        const char* chars = names.At(i).GetString();
        auto* header = CloneRuntimeCStringDataAt414540(
            reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1);
        chars = reinterpret_cast<const char*>(header + 1);
        // Clone引用只写入空槽，保留Unwind和缓存header的既有释放责任。
        std::memcpy(static_cast<void*>(&held.value), &chars, sizeof(chars));
        TextUnwind unwind{&held.value};
        if (voice.OpenSlot(i, RuntimeStringView(held.value), value))
            // 42C782通过Ref重新读取control，与第一遍其他状态寻址的captured不同。
            states.At(i, states.slot.controlAt08) = 3;
        unwind.value = nullptr; // 42C78D先撤state3，再释放已捕获header。
        ReleaseRuntimeCStringHeader(header);
    }
    bool success = true;
    // 第二遍循环长度取状态control，而非输入数组；每次条件重读count。
    for (std::int32_t i = 0; i < (captured ? std::bit_cast<std::int32_t>(
        static_cast<EnvironmentObjectReferenceControl*>(captured)->countAt10) : 0); ++i)
    {
        if (states.At(i, captured) == 3)
        {
            voice.PlayVoiceSlot(i);
            voice.StoreVoiceSlotResourceName(i, names.ResourceNameAt(i));
        }
        success &= (states.At(i, captured) & 1u) != 0;
    }
    states.Finish(captured); // 42C9FC state7：正常释放失败直接terminate。
    return success;
}
}
