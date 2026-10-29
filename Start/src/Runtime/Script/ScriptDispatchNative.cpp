#include <Runtime/ScriptDispatchNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootRuntimeOwner.h>

#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class SingleCommandControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SingleCommandControl(ScriptSingleCommandNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 原5B5C74[0]=4D75E0：只作4-byte sized delete，不释放source脚本。
        ::operator delete(payloadAt14, std::size_t{4});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SingleCommandControl) == 0x18);

struct ReadWitness final
{
    std::uintptr_t base = 0;
    std::uint64_t extent = 0;
    bool known = false;
};
struct CommandRead final
{
    YtlReferenceSlotRuntime reference;
    ReadWitness witness;
    // 此处不能RAII回收：4D4094..BF/4D415B..87尚未注册原EH清理状态。
};

CommandRead CreateCommand(ScrContainer& parser, const std::uint32_t* fixedPc,
    IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(4);
    auto* body = storage ? ::new (storage) ScriptSingleCommandNative : nullptr;
    ReadWitness witness;
    if (body)
    {
        // 4D4051/4D410D在分配返回后才读+A4。第一次重读+B0，第二次用原EBX。
        const auto pc = fixedPc ? *fixedPc : parser.stagedPcAt10;
        body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
        const auto& bytes = parser.bytesAt04.RawSlot();
        witness.base = bytes.baseAt00;
        if (bytes.controlAt08)
        {
            const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.controlAt08);
            witness.extent = std::uint64_t{control->countAt10} * control->strideAt0C;
            witness.known = true;
        }
    }
    CommandRead result{{}, witness};
    CreateScriptSingleCommandReferenceAt4C8490(result.reference, body, 1, memory);
    return result;
}

std::uint16_t ReadWord(const CommandRead& command, std::uint32_t field)
{
    const auto* body = ResolveEnvironmentObjectReference<ScriptSingleCommandNative>(command.reference);
    if (!body || !body->source)
        throw std::logic_error("CScript native null command read (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
    if (command.witness.known)
    {
        const auto offset = static_cast<std::uint32_t>(pointer - command.witness.base);
        const auto required = std::uint64_t{offset} + field + 2;
        if (required > command.witness.extent)
            throw std::out_of_range("CScript native command word outside allocation (modern safety boundary)");
    }
    // 只读当前WORD，不预检整条指令长度、code段或下个PC；DWORD地址加法按x86回绕。
    const auto address = static_cast<std::uintptr_t>(pointer + field);
    std::uint16_t result;
    std::memcpy(&result, reinterpret_cast<const void*>(address), sizeof(result));
    return result;
}
}

void CreateScriptSingleCommandReferenceAt4C8490(YtlReferenceSlotRuntime& slot,
    ScriptSingleCommandNative* body, std::uint8_t owning, IEnvironmentRootMemory& memory)
{
    slot = {};
    if (!body) return;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new (storage) SingleCommandControl(body) : nullptr;
    slot.controlAt08 = control;
    if (!control) throw std::bad_alloc{}; // 原空control解引用的显式安全边界。
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = owning;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

void ReleaseScriptDispatchCommandAt4D40C6(YtlReferenceSlotRuntime& slot) noexcept
{
    auto* const captured = slot.controlAt08;
    if (!captured) return;
    if (--captured->referenceCountAt04 == 0)
    {
        if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
        captured->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

ScriptExecutionRootReceiver ScriptExecutionRootSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* object) -> std::uint32_t& {
        return static_cast<RootRuntimePayload*>(object)->ParamsAt47BDB0().programCounter;
    }};
}

void ApplyScriptProgramCounterTargetAt4D6E90(ScrContainer& parser, std::int32_t target,
    IEnvironmentRootMemory& memory)
{
    if (target >= 0)
    {
        parser.stagedPcAt10 = static_cast<std::uint32_t>(target);
        return;
    }
    auto command = CreateCommand(parser, nullptr, memory);
    const auto length = ReadWord(command, 2);
    parser.stagedPcAt10 += length;
    // 5F1670/5F1668只有{-1,541E55}：此正常释放也必须按捕获control终止异常。
    ReleaseScriptDispatchCommandAt4D40C6(command.reference);
}

std::int32_t ExecuteScriptLoopAt4D3FF0(ScrContainer& parser,
    IScriptExecutionRootSource& root, IScriptNativeCommandDispatch& commands,
    IEnvironmentRootMemory& memory)
{
    for (;;)
    {
        auto receiver = root.CaptureAfterEnsureAt413BE0();
        const auto pc = receiver.programCounterAt47BDB0(receiver.object);
        parser.stagedPcAt10 = pc;
        auto lengthCommand = CreateCommand(parser, nullptr, memory);
        const auto length = ReadWord(lengthCommand, 2);
        parser.stagedPcAt10 += length;
        ReleaseScriptDispatchCommandAt4D40C6(lengthCommand.reference);

        auto opcodeCommand = CreateCommand(parser, &pc, memory);
        const auto opcode = ReadWord(opcodeCommand, 0);
        const auto index = static_cast<std::uint32_t>(opcode - 1u) <= 27u ? opcode : 0u;
        ReleaseScriptDispatchCommandAt4D40C6(opcodeCommand.reference);
        const auto result = commands.DispatchAt4D4200(index);

        receiver = root.CaptureAfterEnsureAt413BE0();
        const auto staged = parser.stagedPcAt10;
        receiver.programCounterAt47BDB0(receiver.object) = staged;
        if (result != 0) return result;
    }
}
}
