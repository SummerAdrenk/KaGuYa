#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScriptControlCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/ScrScriptRuntime.h>

#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ShowError(IScrVmCommandErrorSink& errors, const char* text)
{
    const auto& environment = errors.CaptureAt4180C0();
    const auto* caption = environment.titleAt10.GetString();
    const auto window = reinterpret_cast<HWND>(environment.windowAt2C);
    static_cast<void>(errors.ShowMessageBoxA(window, text, caption, MB_ICONERROR));
}
[[noreturn]] void ReportIndexError(IScrVmCommandErrorSink& errors, bool write)
{
    ShowError(errors, write ? "SetValue over num" : "GetValue over num");
    throw 1; // 4C7411/4C829A：报告本身异常时不得再替换成整数异常。
}
void DeleteConstructingCommand(IEnvironmentRootMemory& memory, void* body) noexcept
{ memory.DeleteBody(body, 4); }
struct Command
{
    YtlReferenceSlotRuntime reference;
    std::uintptr_t allocationBase = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
};
Command CreateCommand(ScrContainer& parser, IScriptArithmeticCommandSource& source,
    bool assign, IEnvironmentRootMemory& memory)
{
    void* allocation = memory.AllocateBody(4);
    auto* body = allocation ? ::new (allocation) ScriptControlCommandNative : nullptr;
    Command command;
    if (body)
    {
        try
        {
            const auto root = source.CaptureAfterEnsureAt413BE0();
            const auto pc = root.readPcAt47BDB0(root.object);
            body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
            const auto& bytes = parser.bytesAt04.RawSlot();
            command.allocationBase = bytes.baseAt00;
            if (bytes.controlAt08)
            {
                const auto* control = static_cast<EnvironmentObjectReferenceControl*>(bytes.controlAt08);
                command.extent = std::uint64_t{control->countAt10} * control->strideAt0C;
                command.knownExtent = true;
            }
        }
        catch (...) { DeleteConstructingCommand(memory, body); throw; }
    }
    CreateScriptControlCommandReference(command.reference, body, assign ? ScriptControlCommandKind::Assign :
        ScriptControlCommandKind::Compare, memory);
    return command;
}
class CommandUnwind final
{
public:
    explicit CommandUnwind(YtlReferenceSlotRuntime& slot) noexcept : slot_(&slot) {}
    ~CommandUnwind() noexcept { if (slot_) UnwindScriptControlCommand(*slot_); }
    void Finish(YtlReferenceControlRuntime* captured) noexcept
    {
        slot_ = nullptr;
        YtlReferenceSlotRuntime reference{0, 0, captured};
        ReleaseScriptDispatchCommandAt4D40C6(reference);
    }
private:
    YtlReferenceSlotRuntime* slot_;
};
template<class T> T Read(const Command& command, unsigned field)
{
    const auto* body = ResolveEnvironmentObjectReference<ScriptControlCommandNative>(command.reference);
    if (!body || !body->source)
        throw std::logic_error("CScript arithmetic null command/source (modern safety boundary)");
    const auto address = reinterpret_cast<std::uintptr_t>(body->source);
    const auto offset = static_cast<std::uint32_t>(address - command.allocationBase);
    if (command.knownExtent && std::uint64_t{offset} + field + sizeof(T) > command.extent)
        throw std::out_of_range("CScript arithmetic operand outside allocation (modern safety boundary)");
    T value;
    std::memcpy(&value, reinterpret_cast<const void*>(address + field), sizeof(T));
    return value;
}
std::int32_t Variable(IScriptArithmeticCommandSource& source, IScrVmCommandErrorSink& errors, std::int32_t index)
{
    const auto root = source.CaptureAfterEnsureAt413BE0();
    return ReadScriptValueAt4C73A0(root.paramsAt47BDB0(root.object), index, errors);
}
// 4D10F2/4D10FB保留真实x86 #DE；不能用C++未定义除法或现代异常代替硬件分支。
__declspec(noinline) std::int32_t Divide(std::int32_t lhs, std::int32_t rhs, bool remainder)
{
    std::int32_t quotient, residual;
    __asm
    {
        mov eax, lhs
        cdq
        idiv rhs
        mov quotient, eax
        mov residual, edx
    }
    return remainder ? residual : quotient;
}
std::int32_t Calculate(unsigned op, std::int32_t lhs, std::int32_t rhs)
{
    const auto left = std::bit_cast<std::uint32_t>(lhs), right = std::bit_cast<std::uint32_t>(rhs);
    switch (op)
    {
    case 0: return rhs;
    case 1: return std::bit_cast<std::int32_t>(left + right);
    case 2: return std::bit_cast<std::int32_t>(left - right);
    case 3: return std::bit_cast<std::int32_t>(left * right);
    case 4: return Divide(lhs, rhs, false);
    case 5: return Divide(lhs, rhs, true);
    case 6: return std::bit_cast<std::int32_t>(left & right);
    case 7: return std::bit_cast<std::int32_t>(left | right);
    case 8: return std::bit_cast<std::int32_t>(left ^ right);
    case 9: return std::bit_cast<std::int32_t>(~right);
    case 10: return std::bit_cast<std::int32_t>(left << (right & 31u));
    case 11: return lhs >> (right & 31u); // C++20算术右移，与sar edi,cl一致。
    default: throw std::logic_error("CScript arithmetic operation was not dispatched");
    }
}
std::int32_t Compare(unsigned op, std::int32_t lhs, std::int32_t rhs)
{
    switch (op)
    {
    case 0: return lhs == rhs;
    case 1: return lhs != rhs;
    case 2: return lhs < rhs;
    case 3: return lhs <= rhs;
    case 4: return lhs > rhs;
    case 5: return lhs >= rhs;
    case 6: return lhs != 0 && rhs != 0;
    case 7: return lhs != 0 || rhs != 0;
    default: throw std::logic_error("CScript comparison operation was not dispatched");
    }
}
std::int32_t Execute(ScrContainer& parser, IScriptArithmeticCommandSource& source,
    IScrVmCommandErrorSink& errors, IEnvironmentRootMemory& memory, bool assign)
{
    auto command = CreateCommand(parser, source, assign, memory);
    auto* const captured = command.reference.controlAt08;
    CommandUnwind unwind(command.reference);
    std::int32_t lhs;
    if (Read<std::uint8_t>(command, 4) & 2u)
        lhs = Variable(source, errors, Read<std::int32_t>(command, 8));
    else if (assign && (Read<std::uint8_t>(command, 4) & 8u))
        lhs = 0;
    else
        lhs = Read<std::int32_t>(command, 8);
    // 4D1098/4CD77D：不能沿用左值取值前的flags或rhs快照。
    const auto rhs = (Read<std::uint8_t>(command, 4) & 0x20u) ?
        Variable(source, errors, Read<std::int32_t>(command, 12)) : Read<std::int32_t>(command, 12);
    const auto operation = Read<std::uint8_t>(command, 5);
    std::int32_t status = 0;
    if (operation > (assign ? 11u : 7u))
    {
        ReportScriptOperationError(errors, assign);
        status = 2;
    }
    else
    {
        const auto value = assign ? Calculate(operation, lhs, rhs) : Compare(operation, lhs, rhs);
        const auto destination = Read<std::uint16_t>(command, 6);
        const auto root = source.CaptureAfterEnsureAt413BE0();
        WriteScriptValueAt4C8220(root.paramsAt47BDB0(root.object), destination, value, errors);
    }
    unwind.Finish(captured);
    return status;
}
std::uint32_t ValueOffset(std::int32_t index)
{
    // 4C73D0/4C8257：x86的index*4+0C按32位回绕，不是有符号数组下标检查。
    const auto offset = std::bit_cast<std::uint32_t>(index) * 4u + 12u;
    if (offset >= 0x80Cu)
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "CScript value address outside allocation (modern safety boundary)");
    return offset;
}
}
std::int32_t ReadScriptValueAt4C73A0(const ScrVmParamsGeneration& params, std::int32_t index,
    IScrVmCommandErrorSink& errors)
{
    if (index == -1) ReportIndexError(errors, false);
    const auto offset = ValueOffset(index);
    const auto& body = params.SaveParams2At30();
    // 4C73D0直接读回绕后的正文DWORD，包含+00虚表及未初始化的+04/+08。
    std::int32_t value;
    std::memcpy(&value, reinterpret_cast<const std::byte*>(&body) + offset, sizeof(value));
    return value;
}
void WriteScriptValueAt4C8220(ScrVmParamsGeneration& params, std::int32_t index,
    std::int32_t value, IScrVmCommandErrorSink& errors)
{
    if (index == -1) ReportIndexError(errors, true);
    const auto offset = ValueOffset(index);
    auto& body = params.SaveParams2At30();
    // 不用逻辑values[]替代正文地址，也不为+00/+04/+08维护旁路状态。
    std::memcpy(reinterpret_cast<std::byte*>(&body) + offset, &value, sizeof(value));
}
void ReportScriptOperationError(IScrVmCommandErrorSink& errors, bool assign)
{ ShowError(errors, assign ? "CmdSetValue" : "CmdComp"); }
std::int32_t ExecuteScriptAssignAt4D0F40(ScrContainer& p, IScriptArithmeticCommandSource& s,
    IScrVmCommandErrorSink& e, IEnvironmentRootMemory& m) { return Execute(p, s, e, m, true); }
std::int32_t ExecuteScriptCompareAt4CD640(ScrContainer& p, IScriptArithmeticCommandSource& s,
    IScrVmCommandErrorSink& e, IEnvironmentRootMemory& m) { return Execute(p, s, e, m, false); }

ScriptArithmeticRootReceiver RootScriptArithmeticCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* p) {
        return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().programCounter;
    }, [](void* p) -> ScrVmParamsGeneration& {
        return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0();
    }};
}
ScriptArithmeticRootReceiver BoundScriptArithmeticCommandSource::CaptureAfterEnsureAt413BE0()
{
    return {this, [](void* p) { return static_cast<BoundScriptArithmeticCommandSource*>(p)->runtime_.ProgramCounter(); },
        [](void* p) -> ScrVmParamsGeneration& {
            return static_cast<BoundScriptArithmeticCommandSource*>(p)->values_.CurrentParamsGeneration();
        }};
}
}
