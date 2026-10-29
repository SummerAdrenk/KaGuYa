#include <Runtime/ScrVmMenuHandler.h>

#include <Runtime/ScrVmControlHandlers.h>

#include <cstring>
#include <bit>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::string_view GetValueOverNum = "GetValue over num";
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
            "SCR VM menu operand is truncated");
    }
}

std::int32_t ReadDwordAt(
    const ScrInstructionView& instruction,
    const std::size_t offset)
{
    RequireOperand(instruction, offset + sizeof(std::int32_t));
    std::int32_t value = 0;
    std::memcpy(&value, instruction.body.data() + offset, sizeof(value));
    return value;
}
}

ScrVmMenuState::ScrVmMenuState() : ScrVmMenuState(CreateScrVmParamsGenerationSlot()) {}
ScrVmMenuState::ScrVmMenuState(ScrVmParamsGenerationBinding generation) : generation_(std::move(generation))
{
    if (!generation_) throw std::invalid_argument("MENU candidate view requires a params slot");
}
ScriptDwordVectorStorage& ScrVmMenuState::CandidateStorage() { return generation_->Current().menuCandidatesAt64; }
const ScriptDwordVectorStorage& ScrVmMenuState::CandidateStorage() const { return generation_->Current().menuCandidatesAt64; }
std::uint32_t ScrVmMenuState::CandidateCount() const { return CandidateStorage().Size(); }
std::int32_t ScrVmMenuState::CandidateAt(std::size_t ordinal) const
{
    const auto& candidates = CandidateStorage();
    if (ordinal >= candidates.Size()) throw std::out_of_range("MENU candidate outside storage (modern safety boundary)");
    return std::bit_cast<std::int32_t>(candidates.storage.begin[ordinal]);
}
bool ScrVmMenuState::UsesParamsGenerationSlot(const ScrVmParamsGenerationBinding& generation) const noexcept
{ return generation_ == generation; }

ScrVmMenuCommandException::ScrVmMenuCommandException(std::string detail)
    : std::runtime_error(detail), detail_(std::move(detail))
{
}

std::string_view ScrVmMenuCommandException::Detail() const noexcept
{
    return detail_;
}

ScrMenuInstructionHandler::ScrMenuInstructionHandler(
    ScrVmValueTable& values,
    const ScrVmExecutableProfile& executableProfile,
    ScrVmScriptCommandState& scriptState,
    ScrVmMenuState& menuState,
    ScrMusicInstructionHandler& musicController,
    IScrVmMenuRuntimeBoundary& menuRuntime,
    IScrVmMenuCommandErrorSink& errorSink) noexcept
    : offline_(OfflineDependencies{values, executableProfile, menuState, menuRuntime, errorSink}),
      scriptState_(scriptState),
      musicController_(musicController),
      layers_(menuRuntime)
{
}

ScrMenuInstructionHandler::ScrMenuInstructionHandler(ScrVmValueTable& values,
    const ScrVmExecutableProfile& profile, ScrVmScriptCommandState& state, ScrVmMenuState& menuState,
    ScrMusicInstructionHandler& music, IScrVmMenuRuntimeBoundary& menu,
    IScrVmMenuCommandErrorSink& errors, ScriptMenuCommandNativeContext native) noexcept
    : ScrMenuInstructionHandler(values, profile, state, menuState, music, menu, errors)
{
    native_.emplace(native);
}
ScrMenuInstructionHandler::ScrMenuInstructionHandler(ScrVmScriptCommandState& state,
    ScrMusicInstructionHandler& music, IScrVmMenuLayerBoundary& layers,
    ScriptMenuCommandNativeContext native) noexcept
    : scriptState_(state), musicController_(music), layers_(layers), native_(native)
{
}
std::int32_t ScrMenuInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
    IEnvironmentRootMemory& memory)
{
    if (!native_) throw std::logic_error("MENU native entry requires explicit source context");
    return ExecuteScriptMenuAt4CEDA0(runtime, scriptState_, musicController_, layers_, *native_, memory);
}
std::int32_t ScrMenuInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    if (!offline_) throw std::logic_error("MENU offline Execute requires explicit offline dependencies");
    auto& values_ = offline_->values;
    const auto& executableProfile_ = offline_->profile;
    auto& menuState_ = offline_->state;
    auto& menuRuntime_ = offline_->menu;
    auto& errorSink_ = offline_->errors;
    RequireOperand(instruction, 2u);
    const std::int8_t mode = static_cast<std::int8_t>(
        std::to_integer<std::uint8_t>(instruction.body[0]));
    const std::uint8_t itemCount =
        std::to_integer<std::uint8_t>(instruction.body[1]);
    const std::size_t firstCount = menuRuntime_.FirstStringGroupCountAt50();
    const std::size_t secondCount = menuRuntime_.SecondStringGroupCountAt48();
    const std::size_t groupCount = firstCount + secondCount;
    if (groupCount < firstCount ||
        groupCount > (std::numeric_limits<std::size_t>::max() / 4u) - 5u)
    {
        throw std::overflow_error("SCR VM menu string group count overflow");
    }

    const std::size_t itemBytes = static_cast<std::size_t>(itemCount) * 4u;
    const std::size_t tailOffset = 2u + itemBytes;
    const std::size_t required = tailOffset + (5u + groupCount) * 4u;
    RequireOperand(instruction, required);

    try
    {
        if ((scriptState_.ReadFlagsAt04() & 0x02u) != 0u)
        {
            // 4CEEE6 与 text opcode 共用 CParams+60，不能保存在独立 menu 副本。
            values_.SetMessageFormatIndexAt60(ReadDwordAt(instruction, tailOffset));
            menuRuntime_.ApplyResultLayerList(
                ReadDwordAt(instruction, tailOffset + 4u));
            musicController_.ApplyResourceId(
                ReadDwordAt(instruction, tailOffset + 8u));
            for (std::size_t index = 0; index < firstCount; ++index)
            {
                menuRuntime_.ApplyFirstStringSlot(
                    ReadDwordAt(instruction, tailOffset + (3u + index) * 4u),
                    index);
            }
            for (std::size_t index = 0; index < secondCount; ++index)
            {
                menuRuntime_.ApplySecondStringSlot(
                    ReadDwordAt(
                        instruction,
                        tailOffset + (3u + firstCount + index) * 4u),
                    index);
            }
        }

        const std::size_t effectOffset = tailOffset + (3u + groupCount) * 4u;
        menuRuntime_.SelectEffect(ReadDwordAt(instruction, effectOffset));
        if ((scriptState_.ReadFlagsAt04() & 0x01u) == 0u && mode != 3 && mode != 5)
        {
            // 4CF398 写同一 CSaveParams2+08，先于清候选；此分支不追加 text 历史。
            values_.SetInstructionIndexAtSaveParams08(ReadDwordAt(instruction, effectOffset + 4u));
        }

        menuState_.CandidateStorage().clear();
        for (std::size_t index = 0; index < itemCount; ++index)
            menuState_.CandidateStorage().Push(static_cast<std::uint32_t>(ReadDwordAt(instruction, 2u + index * 4u)));

        // 4CF471先读26，4CF475再写27；恢复分支跳过mode0..4选择，保留selection=-1。
        const bool skipSelection = scriptState_.ReadStateByteAt26() != 0u;
        std::int32_t selection = -1;
        scriptState_.WriteStateByteAt27(1u);
        if (!skipSelection)
        {
            if (mode == 0 || mode == 3)
            {
                // 4CF4B3/4CF4B7：普通提前返回不经过4CF657，不能改写旧28或读取replay。
                scriptState_.WriteStateByteAt24(1u);
                scriptState_.WriteStateByteAt27(0u);
                return 4;
            }
            if (mode == 1)
            {
                selection = menuRuntime_.ReadSelectionWordAt10();
            }
            else if (mode == 2)
            {
                selection = menuRuntime_.ReadSelectionWordAt12();
            }
            else if (mode == 4)
            {
                try
                {
                    selection = values_.ReadSigned(
                        executableProfile_.ResolveNamedValue("prog_result"));
                }
                catch (const ScrVmValueException&)
                {
                    errorSink_.ReportMenuFailure(GetValueOverNum);
                    return 2;
                }
            }
        }

        // 4CF657在selection读取成功后才清28；读取/报告抛出时保留旧28，27已为1。
        scriptState_.WriteStateByteAt28(0u);
        if (mode == 5)
        {
            scriptState_.WriteStateByteAt28(1u);
        }

        if (selection >= 0 &&
            static_cast<std::size_t>(selection) < menuState_.CandidateCount())
        {
            try
            {
                values_.WriteSigned(
                    executableProfile_.ResolveNamedValue("menu_result"), selection);
            }
            catch (const ScrVmValueException&)
            {
                errorSink_.ReportMenuFailure(SetValueOverNum);
                return 2;
            }

            const std::int32_t operand =
                menuState_.CandidateAt(static_cast<std::size_t>(selection));
            const std::optional<bool> oldFlag = menuRuntime_.ReadChoiceFlag(operand);
            if (oldFlag.has_value())
            {
                menuRuntime_.SetChoiceFlag(operand);
                if (!*oldFlag)
                    menuRuntime_.PersistUnreadFlags();
            }
            return 0;
        }

        if ((scriptState_.ReadFlagsAt04() & 0x02u) != 0u)
        {
            scriptState_.WriteStateByteAt24(1u);
        }
        else
        {
            try
            {
                scriptState_.WriteStateByteAt24(values_.ReadSigned(
                    executableProfile_.ResolveNamedValue("replay")) != 0
                    ? 1u
                    : 0u);
            }
            catch (const ScrVmValueException&)
            {
                errorSink_.ReportMenuFailure(GetValueOverNum);
                return 2;
            }
        }
        // 4CF8FD先发布24，4CF927才清恢复路径mode0/3的27；replay读取失败不能提前清。
        if (mode == 0 || mode == 3)
            scriptState_.WriteStateByteAt27(0u);
        return 4;
    }
    catch (const ScrVmMusicException& error)
    {
        errorSink_.ReportMenuFailure(error.what());
        return 2;
    }
    catch (const ScrVmMenuCommandException& error)
    {
        errorSink_.ReportMenuFailure(
            error.Detail().empty() ? SyntaxError : error.Detail());
        return 2;
    }
}
}
