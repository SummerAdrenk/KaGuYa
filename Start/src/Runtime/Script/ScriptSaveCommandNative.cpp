#include <Runtime/ScriptSaveCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootProgramNamedValueRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmScriptCommandState.h>

#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class SaveCommandControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SaveCommandControl(ScriptSaveCommandNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 原5B6218[0]=4D75C0：仅删除4-byte包装，不持有脚本源Ref。
        ::operator delete(payloadAt14, std::size_t{4});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SaveCommandControl) == 0x18);
void DeleteConstructingSaveCommand(IEnvironmentRootMemory& memory, void* body) noexcept
{
    // 5A00B0是state0展开funclet，回收时的二次异常不能替换原异常继续外传。
    memory.DeleteBody(body, 4);
}
}

void CreateScriptSaveCommandReferenceAt4D0BAE(YtlReferenceSlotRuntime& slot,
    ScriptSaveCommandNative* body, IEnvironmentRootMemory& memory)
{
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new (storage) SaveCommandControl(body) : nullptr;
    if (!control) throw std::bad_alloc{}; // 原4D0BF4空control写入的现代安全边界。
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 4;
    slot.controlAt08 = control;
    control->countAt10 = 1;
}

void UnwindScriptSaveCommandAt4C9800(YtlReferenceSlotRuntime& slot) noexcept
{
    // 原5F2AD4 EHFlags=5：成员清理不可向外抛，但Delete需要重读slot控制块。
    ReleaseYtlReference(slot);
}

std::int32_t ExecuteScriptSaveAt4D0AC0(ScrContainer& parser,
    IScriptSaveCommandSource& source, IEnvironmentRootMemory& memory)
{
    const auto flags = source.ReadFlagsAt04();
    const bool gate = (flags & 2u) == 0 && source.ReadReplayAt4D0AF3() == 0;
    source.WriteGateAt25(gate);

    void* storage = memory.AllocateBody(4);
    auto* body = storage ? ::new (storage) ScriptSaveCommandNative : nullptr;
    std::uintptr_t allocationBase = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
    if (body)
    {
        try
        {
            auto root = source.CaptureAfterEnsureAt413BE0();
            const auto pc = root.readPcAt47BDB0(root.object);
            body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
            const auto& bytes = parser.bytesAt04.RawSlot();
            allocationBase = bytes.baseAt00;
            if (bytes.controlAt08)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.controlAt08);
                extent = std::uint64_t{control->countAt10} * control->strideAt0C;
                knownExtent = true;
            }
        }
        catch (...)
        {
            // state0 -> 5A00B0只回收构造中的4字节。进入control分配前即撤销此资格。
            DeleteConstructingSaveCommand(memory, body);
            throw;
        }
    }

    YtlReferenceSlotRuntime reference;
    CreateScriptSaveCommandReferenceAt4D0BAE(reference, body, memory);
    if (!body)
        throw std::logic_error("CScript SAVE null command (modern safety boundary)");
    const auto* const instruction = body->source; // 4D0C12在state1注册之前读包装。
    try
    {
        if (!instruction)
            throw std::logic_error("CScript SAVE null source (modern safety boundary)");
        const auto pointer = reinterpret_cast<std::uintptr_t>(instruction);
        const auto offset = static_cast<std::uint32_t>(pointer - allocationBase);
        if (knownExtent && std::uint64_t{offset} + 8 > extent)
            throw std::out_of_range("CScript SAVE slot outside allocation (modern safety boundary)");
        std::int32_t index;
        std::memcpy(&index, reinterpret_cast<const void*>(pointer + 4u), sizeof(index));
        // 4D0C20先读DWORD，之后才Ensure根/取新CParams；不跨该调用保留旧代。
        auto root = source.CaptureAfterEnsureAt413BE0();
        root.writeSlotAt47BDB0(root.object, index);
    }
    catch (...)
    {
        UnwindScriptSaveCommandAt4C9800(reference);
        throw;
    }
    // state2 -> terminate；与取指循环相同的捕获control正常释放，不能使用上面的unwind规则。
    ReleaseScriptDispatchCommandAt4D40C6(reference);
    return 0;
}

RootScriptSaveCommandSource::RootScriptSaveCommandSource(RootRuntimeOwner& root,
    RootProgramNamedValueRuntime& named, const ScrVmScriptCommandState& command,
    ScrScriptRuntimeState& state) noexcept
    : root_(root), named_(named), command_(command), state_(state) {}
std::uint32_t RootScriptSaveCommandSource::ReadFlagsAt04() { return command_.ReadFlagsAt04(); }
std::int32_t RootScriptSaveCommandSource::ReadReplayAt4D0AF3()
{
    // 4D0AF3先root，再slot0/498020，最后对捕获root取Params；该叶已有同序生产实现。
    return named_.ReadAfterRootAt4C73A0("replay");
}
void RootScriptSaveCommandSource::WriteGateAt25(bool value) { state_.SetSaveCommandGate(value); }
ScriptSaveRootReceiver RootScriptSaveCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* p) -> std::uint32_t {
        return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().programCounter;
    }, [](void* p, std::int32_t index) {
        static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().SaveParams2At30()
            .instructionIndexAtSaveParams08 = index;
    }};
}
}
