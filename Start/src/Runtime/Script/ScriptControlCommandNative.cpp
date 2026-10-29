#include <Runtime/ScriptControlCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmCallHandlers.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScrVmScriptStorage.h>
#include <Runtime/ScrVmSceneHandler.h>

#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<ScriptControlCommandKind Kind>
class CommandControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit CommandControl(ScriptControlCommandNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    // flag:5B6158；jump:5B61A0；if:5B61D0；wait:5B6188。均为4-byte sized delete。
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{4}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Flag>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::LabelJump>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Conditional>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Wait>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Assign>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Compare>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Title>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Scene>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::FileJump>) == 0x18);
static_assert(sizeof(CommandControl<ScriptControlCommandKind::Update>) == 0x18);
struct CommandRead
{
    YtlReferenceSlotRuntime reference;
    std::uintptr_t allocationBase = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
};
void DeleteConstructingCommand(IEnvironmentRootMemory& memory, void* body) noexcept
{ memory.DeleteBody(body, 4); }
CommandRead CreateCommand(ScrContainer& parser, IScriptControlCommandSource& source,
    ScriptControlCommandKind kind, IEnvironmentRootMemory& memory)
{
    void* allocation = memory.AllocateBody(4);
    auto* body = allocation ? ::new (allocation) ScriptControlCommandNative : nullptr;
    CommandRead result;
    if (body)
    {
        try
        {
            const auto root = source.CaptureAfterEnsureAt413BE0();
            const auto pc = root.readPcAt47BDB0(root.object);
            body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
            const auto& bytes = parser.bytesAt04.RawSlot();
            result.allocationBase = bytes.baseAt00;
            if (bytes.controlAt08)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.controlAt08);
                result.extent = std::uint64_t{control->countAt10} * control->strideAt0C;
                result.knownExtent = true;
            }
        }
        catch (...) { DeleteConstructingCommand(memory, body); throw; }
    }
    // 原入口均在control分配前撤销state0；control失败不回收孤立body。
    CreateScriptControlCommandReference(result.reference, body, kind, memory);
    return result;
}
class CommandUnwind final
{
public:
    explicit CommandUnwind(YtlReferenceSlotRuntime& reference) noexcept : reference_(&reference) {}
    ~CommandUnwind() noexcept
    { if (reference_) UnwindScriptControlCommand(*reference_); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        reference_ = nullptr;
        YtlReferenceSlotRuntime normal{0, 0, captured};
        ReleaseScriptDispatchCommandAt4D40C6(normal);
    }
private:
    YtlReferenceSlotRuntime* reference_;
};
template<class T> T ReadOperand(const CommandRead& command, std::uint32_t field = 4)
{
    const auto* body = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
    if (!body || !body->source)
        throw std::logic_error("CScript control null command/source (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(pointer - command.allocationBase);
    if (command.knownExtent && std::uint64_t{offset} + field + sizeof(T) > command.extent)
        throw std::out_of_range("CScript control operand outside allocation (modern safety boundary)");
    T value;
    std::memcpy(&value, reinterpret_cast<const void*>(pointer + field), sizeof(value));
    return value;
}
std::int32_t ExecuteFlag(ScrContainer& parser, IScriptControlCommandSource& source,
    std::int32_t value, IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(parser, source, ScriptControlCommandKind::Flag, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    const auto index = ReadOperand<std::uint16_t>(command);
    const auto target = source.CaptureAfterEnsureAt413BE0();
    target.writeValueAt47BDB0(target.object, index, value);
    unwind.Finish(captured);
    return 0;
}
std::int32_t ExecuteConditional(ScrContainer& parser, IScriptControlCommandSource& source,
    bool ifTrue, IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(parser, source, ScriptControlCommandKind::Conditional, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    const auto flags = ReadOperand<std::uint8_t>(command);
    const auto raw = ReadOperand<std::uint16_t>(command, 5);
    std::int32_t condition = raw;
    if (flags & 2u)
    {
        const auto root = source.CaptureAfterEnsureAt413BE0();
        condition = root.readValueAt47BDB0(root.object, raw);
    }
    if ((condition != 0) == ifTrue)
    {
        // 原4CEBFA/4CEA46只在命中分支后读取target，不能提前校验整个操作数。
        const auto target = ReadOperand<std::int32_t>(command, 7);
        ApplyScriptProgramCounterTargetAt4D6E90(parser, target, memory);
    }
    unwind.Finish(captured);
    return 0;
}
}

void CreateScriptControlCommandReference(YtlReferenceSlotRuntime& slot,
    ScriptControlCommandNative* body, ScriptControlCommandKind kind, IEnvironmentRootMemory& memory)
{
    if (kind != ScriptControlCommandKind::Flag && kind != ScriptControlCommandKind::LabelJump &&
        kind != ScriptControlCommandKind::Conditional && kind != ScriptControlCommandKind::Wait &&
        kind != ScriptControlCommandKind::Assign && kind != ScriptControlCommandKind::Compare &&
        kind != ScriptControlCommandKind::Title && kind != ScriptControlCommandKind::Scene &&
        kind != ScriptControlCommandKind::FileJump && kind != ScriptControlCommandKind::Update &&
        kind != ScriptControlCommandKind::Music && kind != ScriptControlCommandKind::Sound &&
        kind != ScriptControlCommandKind::Voice && kind != ScriptControlCommandKind::Text &&
        kind != ScriptControlCommandKind::Menu && kind != ScriptControlCommandKind::Program)
        throw std::invalid_argument("CScript control factory kind is not registered");
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* allocation = memory.AllocateBody(0x18);
    EnvironmentObjectReferenceControl* control = nullptr;
    if (allocation)
    {
        if (kind == ScriptControlCommandKind::Flag)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Flag>(body);
        else if (kind == ScriptControlCommandKind::LabelJump)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::LabelJump>(body);
        else if (kind == ScriptControlCommandKind::Conditional)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Conditional>(body);
        else if (kind == ScriptControlCommandKind::Wait)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Wait>(body);
        else if (kind == ScriptControlCommandKind::Assign)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Assign>(body);
        else if (kind == ScriptControlCommandKind::Compare)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Compare>(body);
        else if (kind == ScriptControlCommandKind::Title)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Title>(body);
        else if (kind == ScriptControlCommandKind::Scene)
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Scene>(body);
        else if (kind == ScriptControlCommandKind::Update)
            // 5B60B0：4D7630删除4-byte正文，4CAC80退化后按flags删除18h。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Update>(body);
        else if (kind == ScriptControlCommandKind::Music)
            // 5B6110：4D75A0/4CAAD0。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Music>(body);
        else if (kind == ScriptControlCommandKind::Sound)
            // 5B60F8：4D7600/4CABF0。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Sound>(body);
        else if (kind == ScriptControlCommandKind::Voice)
            // 5B6128：4D7650/4CACE0。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Voice>(body);
        else if (kind == ScriptControlCommandKind::Text)
            // 4C8500 / 5B60C8：4D7610 / 4CAC20，发布时序走下面先写slot的分支。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Text>(body);
        else if (kind == ScriptControlCommandKind::Menu)
            // 4CEE87 / 5B60E0：4D7590/4CAAA0，正常退出捕获EDI。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Menu>(body);
        else if (kind == ScriptControlCommandKind::Program)
            // 4CFC22 / 5B6230：4D75B0/4CAB00，与MENU相同的先写字段再发布顺序。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::Program>(body);
        else
            // 4C82D0/5B6170：file_jump_cmd正文同为4字节，但保留独立control动态类型。
            control = ::new (allocation) CommandControl<ScriptControlCommandKind::FileJump>(body);
    }
    if (kind == ScriptControlCommandKind::Wait || kind == ScriptControlCommandKind::Assign ||
        kind == ScriptControlCommandKind::Compare || kind == ScriptControlCommandKind::Title ||
        kind == ScriptControlCommandKind::Scene || kind == ScriptControlCommandKind::Update ||
        kind == ScriptControlCommandKind::Music || kind == ScriptControlCommandKind::Sound ||
        kind == ScriptControlCommandKind::Voice || kind == ScriptControlCommandKind::Menu ||
        kind == ScriptControlCommandKind::Program)
    {
        // 4D12B7/4D1028/4CD725/4D0E12：先写refs/owning/stride，之后才发布control。
        if (!control) throw std::bad_alloc{};
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 4;
        slot.controlAt08 = control;
        control->countAt10 = 1;
        return;
    }
    slot.controlAt08 = control;
    if (!control) throw std::bad_alloc{}; // 原空control写入的明确安全拒绝。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
// 原4C9560/4C9640/4C95D0/4C9950/4C9870的FunctionInfo均EHFlags=5。
void UnwindScriptControlCommand(YtlReferenceSlotRuntime& slot) noexcept
{ ReleaseYtlReference(slot); }
std::int32_t ExecuteScriptSetFlagAt4CE2A0(ScrContainer& p, IScriptControlCommandSource& s,
    IEnvironmentRootMemory& m) { return ExecuteFlag(p, s, 1, m); }
std::int32_t ExecuteScriptClearFlagAt4CE160(ScrContainer& p, IScriptControlCommandSource& s,
    IEnvironmentRootMemory& m) { return ExecuteFlag(p, s, 0, m); }
std::int32_t ExecuteScriptJumpAt4CE7B0(ScrContainer& parser, IScriptControlCommandSource& source,
    IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(parser, source, ScriptControlCommandKind::LabelJump, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    const auto target = ReadOperand<std::int32_t>(command);
    ApplyScriptProgramCounterTargetAt4D6E90(parser, target, memory);
    unwind.Finish(captured);
    return 0;
}
std::int32_t ExecuteScriptCallAt4CEC40(ScrContainer& parser, IScriptControlCommandSource& source,
    IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(parser, source, ScriptControlCommandKind::LabelJump, memory);
    CommandUnwind unwind(command.reference);
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const auto returnPc = parser.stagedPcAt10; // 4CED02，Ensure之后、Params getter之前。
    root.pushReturnAt47BDB0(root.object, returnPc);
    auto* const captured = command.reference.controlAt08; // 4CED36：压栈之后才捕获。
    const auto target = ReadOperand<std::int32_t>(command);
    ApplyScriptProgramCounterTargetAt4D6E90(parser, target, memory);
    unwind.Finish(captured);
    return 0;
}
std::int32_t ExecuteScriptReturnAt4D0A90(ScrContainer& parser, IScriptControlCommandSource& source)
{
    const auto root = source.CaptureAfterEnsureAt413BE0();
    const auto target = root.popReturnAt47BDB0(root.object);
    parser.stagedPcAt10 = target; // 不经过4D6E90；负DWORD照样写入，不读命令正文。
    return 0;
}
std::int32_t ExecuteScriptIfTrueAt4CEA90(ScrContainer& p, IScriptControlCommandSource& s,
    IEnvironmentRootMemory& m) { return ExecuteConditional(p, s, true, m); }
std::int32_t ExecuteScriptIfFalseAt4CE8E0(ScrContainer& p, IScriptControlCommandSource& s,
    IEnvironmentRootMemory& m) { return ExecuteConditional(p, s, false, m); }

std::int32_t ExecuteScriptWaitAt4D11D0(ScrContainer& parser, IScriptControlCommandSource& source,
    ScrVmScriptStorage& storage, IEnvironmentRootMemory& memory)
{
    auto command = CreateCommand(parser, source, ScriptControlCommandKind::Wait, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    // 4D12FB早于第一次操作数读取；短正文失败不能撤销这次写入。
    storage.WriteWaitValue(storage.WaitState(), 2, 0);
    if (ReadOperand<std::uint8_t>(command) & 0x40u)
        storage.WriteWaitValue(storage.WaitState(), 0, 1);
    else if (ReadOperand<std::uint8_t>(command) & 0x80u)
        storage.WriteWaitValue(storage.WaitState(), 0, 3);
    else if (ReadOperand<std::uint8_t>(command) & 0x20u)
        storage.WriteWaitValue(storage.WaitState(), 0, 4);
    else if (ReadOperand<std::uint8_t>(command) & 0x10u)
    {
        storage.WriteWaitValue(storage.WaitState(), 0, 5);
        const auto auxiliary = ReadOperand<std::uint16_t>(command, 7);
        storage.WriteWaitValue(storage.WaitState(), 2, auxiliary);
    }
    else
        storage.WriteWaitValue(storage.WaitState(), 0, 2);

    if (ReadOperand<std::uint8_t>(command) & 2u)
    {
        const auto index = ReadOperand<std::uint16_t>(command, 5);
        const auto root = source.CaptureAfterEnsureAt413BE0();
        // 4D1474捕获发生在Ensure之后，但在Params getter/GetValue之前。
        auto& target = storage.WaitState();
        const auto value = root.readValueAt47BDB0(root.object, index);
        storage.WriteWaitValue(target, 1, value);
    }
    else
    {
        const auto value = ReadOperand<std::uint16_t>(command, 5);
        storage.WriteWaitValue(storage.WaitState(), 1, value);
    }
    unwind.Finish(captured);
    return 7;
}

std::int32_t ExecuteScriptSceneAt4D0CA0(ScrContainer& parser,
    IScriptControlCommandSource& pcSource, IScrVmSceneReplaySource& replay,
    IScrVmSceneSource& scene, IScrVmSceneCommandErrorSink& errors, IEnvironmentRootMemory& memory)
{
    // 4D0CCA..4D0D40先查询replay；此分支不分配命令、不读取PC或SCR。
    if (replay.ReadReplayValue() != 0) return 6;
    auto command = CreateCommand(parser, pcSource, ScriptControlCommandKind::Scene, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    const auto label = ReadOperand<std::uint16_t>(command);
    const auto result = ApplyScriptSceneLabelAt4D0E39(label, scene, errors);
    // 4D0F05进入terminate边界；两虚槽均使用4D0DF9捕获的control。
    unwind.Finish(captured);
    return result;
}

ScriptControlRootReceiver RootScriptControlCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* p) {
        return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().programCounter;
    }, [](void* p, std::uint16_t index, std::int32_t value) {
        auto& slot = static_cast<RootRuntimePayload*>(p)->ParamsSlotAt1C0();
        ScrVmValueTable values(ScrVmParamsGenerationBinding::Borrowed(slot));
        values.Write(index, value);
    }, [](void* p, std::uint32_t value) {
        auto& slot = static_cast<RootRuntimePayload*>(p)->ParamsSlotAt1C0();
        ScrVmCallStack stack(ScrVmParamsGenerationBinding::Borrowed(slot));
        stack.Push(value);
    }, [](void* p) {
        auto& slot = static_cast<RootRuntimePayload*>(p)->ParamsSlotAt1C0();
        ScrVmCallStack stack(ScrVmParamsGenerationBinding::Borrowed(slot));
        return stack.Pop();
    }, [](void* p, std::uint16_t index) {
        auto& slot = static_cast<RootRuntimePayload*>(p)->ParamsSlotAt1C0();
        ScrVmValueTable values(ScrVmParamsGenerationBinding::Borrowed(slot));
        return values.Read(index);
    }};
}
ScriptControlRootReceiver BoundScriptControlCommandSource::CaptureAfterEnsureAt413BE0()
{
    return {this, [](void* p) { return static_cast<BoundScriptControlCommandSource*>(p)->runtime_.ProgramCounter(); },
    [](void* p, std::uint16_t index, std::int32_t value) {
        auto& self = *static_cast<BoundScriptControlCommandSource*>(p);
        self.values_->Write(index, value);
    }, [](void* p, std::uint32_t value) {
        static_cast<BoundScriptControlCommandSource*>(p)->stack_->Push(value);
    }, [](void* p) { return static_cast<BoundScriptControlCommandSource*>(p)->stack_->Pop(); },
    [](void* p, std::uint16_t index) {
        return static_cast<BoundScriptControlCommandSource*>(p)->readValues_->Read(index);
    }};
}
}
