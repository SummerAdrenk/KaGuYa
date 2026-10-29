#include <Runtime/ScrVmTextHandler.h>

#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/GameBuildProfile.h>

#include <cstring>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::string_view SetValueOverNum = "SetValue over num";
constexpr std::string_view SyntaxError = "syntax error";

void RequireOperand(
    const ScrInstructionView& instruction,
    const std::size_t required)
{
    if (instruction.body.size() < required)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM text operand is truncated");
    }
}

std::int32_t ReadDword(
    const ScrInstructionView& instruction,
    const std::size_t dwordIndex)
{
    const std::size_t offset = dwordIndex * sizeof(std::int32_t);
    RequireOperand(instruction, offset + sizeof(std::int32_t));
    std::int32_t value = 0;
    std::memcpy(&value, instruction.body.data() + offset, sizeof(value));
    return value;
}
}

ScrVmTextCommandException::ScrVmTextCommandException(std::string detail)
    : std::runtime_error(detail), detail_(std::move(detail))
{
}

std::string_view ScrVmTextCommandException::Detail() const noexcept
{
    return detail_;
}

ScrTextInstructionHandler::ScrTextInstructionHandler(
    ScrVmValueTable& values,
    const ScrVmExecutableProfile& executableProfile,
    ScrVmScriptCommandState& scriptState,
    ScrVmTextState&,
    ScrMusicInstructionHandler& musicController,
    IScrVmTextRuntimeBoundary& textRuntime,
    IScrVmTextCommandErrorSink& errorSink) noexcept
    : offline_(OfflineDependencies{values,executableProfile,errorSink}),
      scriptState_(scriptState),
      musicController_(musicController),
      textRuntime_(textRuntime)
{
}

ScrTextInstructionHandler::ScrTextInstructionHandler(ScrVmValueTable& values,
    const ScrVmExecutableProfile& profile, ScrVmScriptCommandState& state, ScrVmTextState& textState,
    ScrMusicInstructionHandler& music, IScrVmTextRuntimeBoundary& text, IScrVmTextCommandErrorSink& errors,
    ScriptTextCommandNativeContext native) noexcept
    : ScrTextInstructionHandler(values, profile, state, textState, music, text, errors)
{
    native_.emplace(native);
}
ScrTextInstructionHandler::ScrTextInstructionHandler(ScrVmScriptCommandState& state,
    ScrMusicInstructionHandler& music,IScrVmTextRuntimeBoundary& text,ScriptTextCommandNativeContext native) noexcept
    :scriptState_(state),musicController_(music),textRuntime_(text),native_(native)
{
}
std::int32_t ScrTextInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime, IEnvironmentRootMemory& memory)
{
    if (!native_) throw std::logic_error("TEXT native entry requires explicit source context");
    return ExecuteScriptTextAt4D1E50(runtime, scriptState_, musicController_, textRuntime_, *native_, memory);
}
std::int32_t ScrTextInstructionHandler::Execute(
    ScrScriptRuntimeState& runtime,
    const ScrInstructionView& instruction)
{
    if(!offline_)throw std::logic_error("TEXT offline Execute requires explicit offline dependencies");
    auto& values_=offline_->values;
    const auto& executableProfile_=offline_->profile;
    auto& errorSink_=offline_->errors;
    const std::size_t firstCount = textRuntime_.FirstStringGroupCountAt50();
    const std::size_t secondCount = textRuntime_.SecondStringGroupCountAt48();
    const std::size_t groupCount = firstCount + secondCount;
    if (groupCount < firstCount ||
        groupCount > (std::numeric_limits<std::size_t>::max() / 4u) - 5u)
    {
        throw std::overflow_error("SCR VM text string group count overflow");
    }
    RequireOperand(instruction, (5u + groupCount) * sizeof(std::int32_t));

    try
    {
        // 0x4D1F05..0x4D1F30：在读取当前 record 字段前保存尚未提交的旧 PC。
        if (CompiledGameProfile().restartPointEnabled)
            values_.WriteSigned(
                executableProfile_.ResolveNamedValue("restart_point"),
                static_cast<std::int32_t>(runtime.ProgramCounter()));

        // 4D1FEF 写入实时 CParams+60，后续 4B2780/4C1650 读取同一字段。
        values_.SetMessageFormatIndexAt60(ReadDword(instruction, 0u));
        const bool reload =
            (scriptState_.ReadFlagsAt04() & 0x02u) != 0u ||
            scriptState_.ReadReloadByteAt08() != 0u;
        if (reload)
        {
            textRuntime_.ApplyResultLayerList(ReadDword(instruction, 1u));
            musicController_.ApplyResourceId(ReadDword(instruction, 2u));

            for (std::size_t index = 0; index < firstCount; ++index)
            {
                textRuntime_.ApplyFirstStringSlot(
                    ReadDword(instruction, 3u + index), index);
            }
            for (std::size_t index = 0; index < secondCount; ++index)
            {
                textRuntime_.ApplySecondStringSlot(
                    ReadDword(instruction, 3u + firstCount + index), index);
            }
            // 4D1FFB/4D2334只读写低字节；完整重载后才清零，保留当前高三字节。
            scriptState_.WriteReloadByteAt08(0u);
        }

        const std::size_t effectIndex = 3u + groupCount;
        textRuntime_.SelectEffect(ReadDword(instruction, effectIndex));
        if ((scriptState_.ReadFlagsAt04() & 0x01u) == 0u)
        {
            // 4D247C 先写 CSaveParams2+08；4D2491 追加历史失败不回滚恢复索引。
            values_.SetInstructionIndexAtSaveParams08(ReadDword(instruction, effectIndex + 1u));
            values_.AppendCurrentMessageToHistoryAt3C();
        }
        return 5;
    }
    catch (const ScrVmValueException&)
    {
        errorSink_.ReportTextFailure(SetValueOverNum);
        return 2;
    }
    catch (const ScrVmMusicException& error)
    {
        errorSink_.ReportTextFailure(error.what());
        return 2;
    }
    catch (const ScrVmTextCommandException& error)
    {
        errorSink_.ReportTextFailure(
            error.Detail().empty() ? SyntaxError : error.Detail());
        return 2;
    }
}
}
