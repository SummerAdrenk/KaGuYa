#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/ScriptSoundGroupSource.h>

#include <Runtime/ScrVmControlHandlers.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

#include <cstring>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::string_view PortNumberError = "\x83\x7C\x81\x5B\x83\x67\x94\xD4\x8D\x86";
constexpr std::string_view SyntaxError = "syntax error";

struct ResolvedSlot final
{
    ScrVmSoundSlotGroup group = ScrVmSoundSlotGroup::Remaining;
    std::size_t index = 0;
};

void RequireOperand(
    const ScrInstructionView& instruction,
    const std::size_t required)
{
    if (instruction.body.size() < required)
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM sound operand is truncated");
    }
}

std::int32_t ReadResourceId(const ScrInstructionView& instruction)
{
    RequireOperand(instruction, 5u);
    std::int32_t value = 0;
    std::memcpy(&value, instruction.body.data() + 1u, sizeof(value));
    return value;
}

ResolvedSlot ResolveSlot(
    const std::size_t port,
    const std::size_t firstCount,
    const std::size_t secondCount)
{
    if (port < firstCount)
        return {ScrVmSoundSlotGroup::FirstAt50, port};

    const std::size_t afterFirst = port - firstCount;
    if (afterFirst < secondCount)
        return {ScrVmSoundSlotGroup::SecondAt48, afterFirst};

    return {ScrVmSoundSlotGroup::Remaining, afterFirst - secondCount};
}
}

ScrVmSoundCommandException::ScrVmSoundCommandException(std::string detail)
    : std::runtime_error(detail), detail_(std::move(detail))
{
}

std::string_view ScrVmSoundCommandException::Detail() const noexcept
{
    return detail_;
}

ScrSoundInstructionHandler::ScrSoundInstructionHandler(
    const IScrResourceNameSource& resourceNames,
    IScrVmSoundRuntimeBoundary& soundRuntime,
    IScrVmSoundCommandErrorSink& errorSink) noexcept
    : resourceNames_(resourceNames),
      offline_(OfflineDependencies{soundRuntime,errorSink}),
      gate_(soundRuntime)
{
}

ScrSoundInstructionHandler::ScrSoundInstructionHandler(const IScrResourceNameSource& names,
    IScrVmSoundRuntimeBoundary& sound, IScrVmSoundCommandErrorSink& errors,
    IScriptFileCommandSource& pc, IScrVmCommandErrorSink& nativeErrors,
    IScriptSoundGroupSource& groups, IScriptSoundLeafSource& leaves) noexcept
    : ScrSoundInstructionHandler(names, sound, errors)
{
    pcSource_ = &pc;
    nativeErrors_ = &nativeErrors;
    groups_ = &groups;
    leaves_ = &leaves;
}
ScrSoundInstructionHandler::ScrSoundInstructionHandler(const IScrResourceNameSource& names,
    IScrVmSoundOpenGate& gate, IScriptFileCommandSource& pc, IScrVmCommandErrorSink& errors,
    IScriptSoundGroupSource& groups, IScriptSoundLeafSource& leaves) noexcept
    :resourceNames_(names),gate_(gate),pcSource_(&pc),nativeErrors_(&errors),groups_(&groups),leaves_(&leaves)
{
}
std::int32_t ScrSoundInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
    IEnvironmentRootMemory& memory)
{
    if (!pcSource_ || !nativeErrors_ || !groups_ || !leaves_)
        throw std::logic_error("SOUND native entry requires PC/error/params sources");
    return ExecuteScriptSoundAt4D1520(runtime, *pcSource_, *this, *nativeErrors_, memory);
}
std::int32_t ScrSoundInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    if(!offline_)throw std::logic_error("SOUND offline Execute requires explicit offline dependencies");
    auto& errorSink_=offline_->errors;
    std::string failure;
    try
    {
        // 4D164A/4D1653 位于 EH states 1..4 内，现代正文截断同样走 catchall。
        RequireOperand(instruction, 1u);
        const std::int8_t port = static_cast<std::int8_t>(
            std::to_integer<std::uint8_t>(instruction.body[0]));
        const std::int32_t resourceId = ReadResourceId(instruction);
        ApplyOperands(port, resourceId);
        return 0;
    }
    catch (const char* detail) { failure.assign(detail ? detail : ""); }
    catch (...) { failure.assign(SyntaxError); }
    errorSink_.ReportSoundFailure(failure);
    return 2;
}
void ScrSoundInstructionHandler::ApplyOperands(const std::int8_t port, const std::int32_t resourceId)
{
        if(!offline_)throw std::logic_error("SOUND offline operands require explicit offline dependencies");
        auto& soundRuntime_=offline_->sound;
        if (resourceId < 0 && port < 0)
        {
            soundRuntime_.StopAndClearAllEffectSlots();
            return;
        }

        if (resourceId >= 0 && port < 0)
            throw PortNumberError.data();

        const std::size_t firstCount = soundRuntime_.FirstSlotGroupCountAt50();
        const std::size_t secondCount = soundRuntime_.SecondSlotGroupCountAt48();
        if (firstCount > std::numeric_limits<std::size_t>::max() - secondCount)
            throw std::overflow_error("SCR VM sound slot group count overflow");

        const ResolvedSlot slot = ResolveSlot(
            static_cast<std::size_t>(static_cast<std::uint8_t>(port)),
            firstCount,
            secondCount);
        const std::size_t absolutePort =
            static_cast<std::size_t>(static_cast<std::uint8_t>(port));

        if (resourceId < 0)
        {
            if (slot.group == ScrVmSoundSlotGroup::Remaining)
            {
                // 0x4D17D3..0x4D1879：第三组只处理 field_20 基址槽，不触碰映射表。
                soundRuntime_.StopAndClearRemainingSlot(slot.index);
            }
            else
            {
                // 0x4D1715..0x4D1778：前两组共用 M[A] 与 field_18+A。
                soundRuntime_.ClearMappedResourceNameAtPort(absolutePort);
                soundRuntime_.StopAndClearMappedSlotAtPort(absolutePort);
            }
            return;
        }

        if (slot.group == ScrVmSoundSlotGroup::Remaining)
        {
            // 0x4D196A..0x4D19BF：公共第三组在解析名称前先执行 gate。
            if (soundRuntime_.ShouldSkipOpening(slot.group, slot.index))
                return;

            // 4D19D5 -> 4D1A31 -> 4D1A72 -> 4D1A8C：两个 receiver 均为裸借用。
            const auto& parameters = soundRuntime_.CaptureEffectParametersAt1C();
            const auto receiver = resourceNames_.CapturePatternNames();
            const std::int32_t fallback = parameters.NormalEffectValueOrThrow();
            const char* const name = receiver.FindNameAt4AD1B0(resourceId);
            if (soundRuntime_.OpenRemainingSlot(slot.index, name, fallback) ==
                ScrVmSoundOpenResult::Failed)
                throw 1; // 4D1BD8：42BC90 返回 false 时抛 int 1。
            return;
        }

        const auto receiver = resourceNames_.CapturePatternNames();
        soundRuntime_.StoreMappedPatternNameAtPort(absolutePort, receiver, resourceId);
        if (soundRuntime_.ShouldSkipOpening(slot.group, slot.index))
            return;
        // 4D67C7/4D650E/4D65F2：gate 后重取 slot2；两个映射组忽略打开失败。
        (void)soundRuntime_.OpenMappedSlotAtPort(slot.group, slot.index, absolutePort);
}
}
