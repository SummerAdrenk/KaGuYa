#include <Runtime/ScrVmProgramHandler.h>

#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/ScriptProgramMessageRuntime.h>
#include <Runtime/GameBuildProfile.h>

#include <cstring>
#include <utility>

namespace KaGuYa::Start::Runtime
{
void IScrVmProgramRuntimeBoundary::RequestGauge(bool, bool)
{ throw std::logic_error("Program gauge requests are not bound"); }
namespace
{
constexpr std::string_view SyntaxError = "syntax error";

// 原错误对象携带的 CP932 文本：ゲームパラメータの取得 / 設定。
constexpr std::string_view GetGameParameter =
    "\x83\x51\x81\x5B\x83\x80\x83\x70\x83\x89\x83\x81\x81\x5B"
    "\x83\x5E\x82\xCC\x8E\xE6\x93\xBE";
constexpr std::string_view SetGameParameter =
    "\x83\x51\x81\x5B\x83\x80\x83\x70\x83\x89\x83\x81\x81\x5B"
    "\x83\x5E\x82\xCC\x90\xDD\x92\xE8";

[[noreturn]] void ThrowProgram(
    const ScrVmProgramError error,
    const std::string_view detail)
{
    throw ScrVmProgramCommandException(error, std::string(detail));
}

std::uint16_t ReadU16(
    const ScrInstructionView& instruction,
    const std::size_t offset)
{
    if (instruction.body.size() < offset + sizeof(std::uint16_t))
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM program operand is truncated");
    }

    std::uint16_t value = 0;
    std::memcpy(&value, instruction.body.data() + offset, sizeof(value));
    return value;
}

ScrVmProgramCommand ResolveCommand(
    const ScrVmProgramRecord& record,
    const IScrVmProgramRuntimeBoundary& runtime)
{
    const std::int32_t rawCommand =
        record.operandMode == ScrVmProgramOperandMode::ValueTableIndex
        ? runtime.ReadValueAt(record.rawOperand)
        : static_cast<std::int32_t>(record.rawOperand);
    if (rawCommand < 0 || rawCommand > 23)
        ThrowProgram(ScrVmProgramError::InvalidCommand, SyntaxError);
    return static_cast<ScrVmProgramCommand>(rawCommand);
}

std::uint32_t RequireParameterWordIndex(
    const std::int32_t index,
    const std::string_view operation, bool native)
{
    // 原版仅拒绝 index >= 32，负数会访问 registry[7]+0x0C 之前的内存。
    // 强类型运行时明确拒绝负索引；这是有意避免越界写读的安全差异。
    if (index < 0 || index >= 32)
    {
        if (native) throw operation.data();
        ThrowProgram(
            ScrVmProgramError::ParameterWordIndexOutOfRange,
            operation);
    }
    return static_cast<std::uint32_t>(index);
}
std::int32_t NativeRemainder(std::int32_t random, std::int32_t divisor)
{
    // 4D00EA..4D00F0的cdq/idiv：保留x86除法异常，不把INT_MIN/-1扩宽后静默变成0。
    std::int32_t result;
    __asm {
        mov eax, random
        cdq
        idiv divisor
        mov result, edx
    }
    return result;
}
}

ScrVmProgramCommandException::ScrVmProgramCommandException(
    const ScrVmProgramError error,
    std::string detail)
    : std::runtime_error(detail),
      error_(error),
      detail_(std::move(detail))
{
}

ScrVmProgramError ScrVmProgramCommandException::Error() const noexcept
{
    return error_;
}

std::string_view ScrVmProgramCommandException::Detail() const noexcept
{
    return detail_;
}

ScrVmProgramRecord DecodeScrVmProgramRecord(
    const ScrInstructionView& instruction)
{
    if (instruction.body.size() < 3u)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM program record is truncated");
    }

    const std::uint8_t flags =
        std::to_integer<std::uint8_t>(instruction.body[0]);
    ScrVmProgramOperandMode mode{};
    if ((flags & 0x02u) != 0u)
    {
        // 与 0x4CFC5B 一致：bit 1 比 bit 0 优先。
        mode = ScrVmProgramOperandMode::ValueTableIndex;
    }
    else if ((flags & 0x01u) != 0u)
    {
        mode = ScrVmProgramOperandMode::Immediate;
    }
    else
    {
        ThrowProgram(ScrVmProgramError::InvalidFlags, SyntaxError);
    }

    return ScrVmProgramRecord{flags, mode, ReadU16(instruction, 1u)};
}

ScrProgramInstructionHandler::ScrProgramInstructionHandler(
    ScrVmScriptCommandState& scriptState,
    IScrVmProgramRuntimeBoundary& programRuntime,
    IScrVmProgramRandSource& randSource,
    IScrVmProgramCommandErrorSink& errorSink) noexcept
    : scriptState_(scriptState),
      programRuntime_(programRuntime),
      randSource_(randSource),
      offlineErrors_(&errorSink)
{
}

ScrProgramInstructionHandler::ScrProgramInstructionHandler(ScrVmScriptCommandState& state,
    IScrVmProgramRuntimeBoundary& program, IScrVmProgramRandSource& random,
    IScrVmProgramCommandErrorSink& errors, ScriptProgramCommandNativeContext native) noexcept
    : ScrProgramInstructionHandler(state, program, random, errors) { native_.emplace(native); }
ScrProgramInstructionHandler::ScrProgramInstructionHandler(ScrVmScriptCommandState& state,
    IScrVmProgramRuntimeBoundary& program,IScrVmProgramRandSource& random,ScriptProgramCommandNativeContext native) noexcept
    :scriptState_(state),programRuntime_(program),randSource_(random),native_(native)
{
}

std::int32_t ScrProgramInstructionHandler::ExecuteRuntimeEntry(
    ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    if (!native_) throw std::logic_error("PROGRAM native context is required");
    return ExecuteScriptProgramAt4CFB30(runtime, *this, *native_, memory);
}
std::int32_t ScrProgramInstructionHandler::ExecuteResolvedCommand(ScrVmProgramCommand command,
    ScrScriptRuntimeState& runtime, const ScriptProgramCommandNativeContext* native,
    IEnvironmentRootMemory* memory)
{
        switch (command)
        {
        case ScrVmProgramCommand::Command00:
            if (programRuntime_.ReadNamedValue(ScrVmProgramNamedValue::Replay) != 0)
                return 0;
            {
                const std::int32_t parameter = programRuntime_.ReadNamedValue(
                    ScrVmProgramNamedValue::ProgParam00);
                if (native)
                {
                    struct Current final : IProgramCurrentEnvironment
                    {
                        IScrVmProgramRuntimeBoundary& runtime;
                        explicit Current(IScrVmProgramRuntimeBoundary& r) : runtime(r) {}
                        void WriteStringAt3C(const char* text) override
                        { runtime.WriteCurrentObjectStringAt3C(text); }
                    } current(programRuntime_);
                    ApplyScriptProgramMessageAt4CFD3B(parameter, native->messages, current);
                }
                else
                {
                    const auto value = programRuntime_.ResolveCommand00String(parameter);
                    programRuntime_.WriteCurrentObjectStringAt3C(value.c_str());
                }
            }
            return 8;

        case ScrVmProgramCommand::Command01:
            programRuntime_.WriteRegistrySlot7WordAt0C(0u, 1u);
            return 0;

        case ScrVmProgramCommand::Command02:
            scriptState_.WriteStateByteAt24(
                (scriptState_.ReadFlagsAt04() & 0x02u) != 0u ||
                programRuntime_.ReadNamedValue(ScrVmProgramNamedValue::Replay) != 0
                ? 1u
                : 0u);
            return 9;

        case ScrVmProgramCommand::Command03:
            return 10;

        case ScrVmProgramCommand::Command04:
            programRuntime_.CallRegistrySlot3Vslot18(1);
            programRuntime_.CallRegistrySlot4Vslot18(1);
            return 0;

        case ScrVmProgramCommand::Command05:
            programRuntime_.CallRegistrySlot3Vslot18(0);
            programRuntime_.CallRegistrySlot4Vslot18(0);
            return 0;

        case ScrVmProgramCommand::Command06:
        case ScrVmProgramCommand::Command22:
            return 0;

        case ScrVmProgramCommand::Command07:
            {
                std::int32_t divisor = programRuntime_.ReadNamedValue(
                    ScrVmProgramNamedValue::ProgResult);
                if (divisor == 0)
                    divisor = 1;

                // x86 _rand 返回非负 int。使用 64 位中间值避免 C++ 的
                // INT_MIN % -1 未定义行为，同时保持合法 CRT 返回值的 idiv 余数。
                const std::int64_t randomValue = randSource_.NextRand();
                const std::int32_t remainder = native ? NativeRemainder(static_cast<std::int32_t>(randomValue), divisor) :
                    static_cast<std::int32_t>(randomValue % static_cast<std::int64_t>(divisor));
                programRuntime_.WriteNamedValue(
                    ScrVmProgramNamedValue::ProgResult,
                    remainder);
            }
            return 0;

        case ScrVmProgramCommand::Command08:
            return programRuntime_.ReadRegistrySlot1ByteAt84() != 0u ? 0 : 11;

        case ScrVmProgramCommand::Command09:
            programRuntime_.WriteRegistrySlot2DwordAt14(1);
            return 0;

        case ScrVmProgramCommand::Command10:
            programRuntime_.WriteProgResultFromRegistrySlot1ByteAt6F();
            return 0;

        case ScrVmProgramCommand::Command11:
            return 12;

        case ScrVmProgramCommand::Command12:
            {
                const std::uint32_t index = RequireParameterWordIndex(
                    programRuntime_.ReadNamedValue(ScrVmProgramNamedValue::ProgParam00),
                    GetGameParameter, native != nullptr);
                programRuntime_.WriteNamedValue(
                    ScrVmProgramNamedValue::ProgResult,
                    programRuntime_.ReadRegistrySlot7WordAt0C(index));
            }
            return 0;

        case ScrVmProgramCommand::Command13:
            {
                const std::uint32_t index = RequireParameterWordIndex(
                    programRuntime_.ReadNamedValue(ScrVmProgramNamedValue::ProgParam00),
                    SetGameParameter, native != nullptr);
                programRuntime_.WriteRegistrySlot7WordAt0C(
                    index,
                    static_cast<std::uint16_t>(programRuntime_.ReadNamedValue(
                        ScrVmProgramNamedValue::ProgResult)));
            }
            return 0;

        case ScrVmProgramCommand::Command14:
            return 13;

        case ScrVmProgramCommand::Command15:
            programRuntime_.RequireRegistrySlot4();
            programRuntime_.CallCommand15Nullsub2();
            return 0;

        case ScrVmProgramCommand::Command16:
            {
                // 0x4D040D..0x4D0448 先读 param00，再读 param01。
                const std::int32_t childIndex = programRuntime_.ReadNamedValue(
                    ScrVmProgramNamedValue::ProgParam00);
                const std::int32_t value = programRuntime_.ReadNamedValue(
                    ScrVmProgramNamedValue::ProgParam01);
                programRuntime_.WriteRegistrySlot1ChildVslot84Byte(
                    childIndex,
                    value != 0);
            }
            return 0;

        case ScrVmProgramCommand::Command17:
            programRuntime_.WriteNamedValue(
                ScrVmProgramNamedValue::ProgResult,
                programRuntime_.ReadRegistrySlot1ChildVslot84Byte(
                    programRuntime_.ReadNamedValue(
                        ScrVmProgramNamedValue::ProgParam00))
                    ? 1
                    : 0);
            return 0;

        case ScrVmProgramCommand::Command18:
        {
            const auto value = programRuntime_.ReadNamedValue(ScrVmProgramNamedValue::GagueDisp);
            if (HasPrimaryGauge()) programRuntime_.RequestGauge(false, value != 0);
            return 0;
        }

        case ScrVmProgramCommand::Command19:
            if ((scriptState_.ReadFlagsAt04() & 0x02u) != 0u)
            {
                if (native)
                    RestoreScriptProgramAt4D4F20(runtime, programRuntime_, *native, *memory);
                else
                programRuntime_.RestoreFromRestartPoint(ScrVmProgramRestartPoint{
                    programRuntime_.ReadNamedValue(
                        ScrVmProgramNamedValue::RestartPoint)});
            }
            return 14;

        case ScrVmProgramCommand::Command20:
            return 15;

        case ScrVmProgramCommand::Command21:
        {
            const auto value = programRuntime_.ReadNamedValue(
                ScrVmProgramNamedValue::MasoGaugeDisplayFlag);
            if (CompiledGameProfile().loveHolicGauges) programRuntime_.RequestGauge(true, value != 0);
            return 0;
        }

        case ScrVmProgramCommand::Command23:
            return 16;
        }

        ThrowProgram(ScrVmProgramError::InvalidCommand, SyntaxError);
}

std::int32_t ScrProgramInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime, const ScrInstructionView& instruction)
{
    if(!offlineErrors_)throw std::logic_error("PROGRAM offline Execute requires its error sink");
    auto& errorSink_=*offlineErrors_;
    try
    {
        const auto record = DecodeScrVmProgramRecord(instruction);
        return ExecuteResolvedCommand(ResolveCommand(record, programRuntime_), runtime, nullptr, nullptr);
    }
    catch (const ScrVmProgramCommandException& error)
    {
        errorSink_.ReportProgramFailure(
            error.Detail().empty() ? SyntaxError : error.Detail());
        return 2;
    }
    catch (const char* detail)
    {
        // EH 5F2410的.PAD + HT_IsConst=1允许char*和const char*，typed-null写入空串。
        errorSink_.ReportProgramFailure(detail ? std::string_view(detail) : std::string_view{});
        return 2;
    }
    catch (...)
    {
        // 4D076C..4D077B：包括GetValue/SetValue抛出的int(1)。这不是兼容兜底。
        errorSink_.ReportProgramFailure(SyntaxError);
        return 2;
    }
}
}
