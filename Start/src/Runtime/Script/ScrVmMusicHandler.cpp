#include <Runtime/ScrVmMusicHandler.h>
#include <Runtime/ScriptAudioCommandsNative.h>
#include <Runtime/ScriptMusicSource.h>

#include <Runtime/ScrVmControlHandlers.h>

#include <mbstring.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t ReadResourceId(const ScrInstructionView& instruction)
{
    if (instruction.body.size() < sizeof(std::int32_t))
    {
        throw ScrVmOperandException(
            ScrVmOperandError::OperandTruncated,
            instruction.opcode,
            "SCR VM music resource ID is truncated");
    }

    std::int32_t value = 0;
    std::memcpy(&value, instruction.body.data(), sizeof(value));
    return value;
}
}

ScrVmMusicException::ScrVmMusicException(
    const ScrVmMusicError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

ScrVmMusicError ScrVmMusicException::Error() const noexcept
{
    return error_;
}

ScrMusicInstructionHandler::ScrMusicInstructionHandler(
    const IScrResourceNameSource& resourceNames,
    IScrVmMusicNameWriter& musicName,
    IScrVmMusicRuntimeBoundary& musicRuntime) noexcept
    : resourceNames_(resourceNames),
      musicName_(musicName),
      musicRuntime_(&musicRuntime)
{
}

ScrMusicInstructionHandler::ScrMusicInstructionHandler(const IScrResourceNameSource& names,
    IScrVmMusicNameWriter& writer, IScriptFileCommandSource& pc,
    IScriptMusicSource& source) noexcept
    : resourceNames_(names), musicName_(writer), pcSource_(&pc), musicSource_(&source) {}
std::int32_t ScrMusicInstructionHandler::ExecuteRuntimeEntry(ScrScriptRuntimeState& runtime,
    IEnvironmentRootMemory& memory)
{
    if (!pcSource_ || !musicSource_) throw std::logic_error("MUSIC native entry requires PC and music sources");
    return ExecuteScriptMusicAt4CF9E0(runtime, *pcSource_, *this, memory);
}
std::int32_t ScrMusicInstructionHandler::Execute(
    ScrScriptRuntimeState&,
    const ScrInstructionView& instruction)
{
    ApplyResourceId(ReadResourceId(instruction));
    return 0;
}

void ScrMusicInstructionHandler::ApplyNativeResourceId(const std::int32_t resourceId)
{
    if (!musicSource_) throw std::logic_error("MUSIC native resource requires music source");
    ApplyScriptMusicAt4D59E0(resourceId, *musicSource_, resourceNames_, musicName_);
}

void ScrMusicInstructionHandler::ApplyResourceId(const std::int32_t resourceId)
{
    if (!musicRuntime_) throw std::logic_error("MUSIC offline entry requires an explicit offline manager");
    auto& music = *musicRuntime_;
    if (resourceId < 0)
    {
        // 0x4D59EB..0x4D5A3E：先清 Environment 名称，再停止音乐槽。
        musicName_.WriteCurrentMusicNameAt2C({});
        music.StopMusicSlots();
        return;
    }

    // 4D5A7B..4D5A9F先捕获Pattern裸receiver，4D5AD6参数回调后仍用它查询。
    const auto names = resourceNames_.CapturePatternNames();
    const std::int32_t playbackParameter = music.ReadPlaybackParameter();
    const char* const resourceName = names.FindNameAt4AD1B0(resourceId);
    // 4D5AFA..4D5B15：null按零长度清名，非null按首NUL写入；不复制或保活原raw。
    musicName_.WriteCurrentMusicNameAt2C(resourceName);

    if (!music.HasPlaybackSlots() || !music.IsCurrentSlotActive())
        return;

    // 4D5B5D/4D5B8B 只在活动播放器分支拒绝缺失指针；非 null 空串仍进入比较。
    if (resourceName == nullptr)
    {
        throw ScrVmMusicException(
            ScrVmMusicError::MissingActiveResourceName,
            "SCR VM active music resource name is missing");
    }

    const char* const currentName = music.CurrentSlotResourceName();
    if (_mbsicmp(
            reinterpret_cast<const unsigned char*>(currentName),
            reinterpret_cast<const unsigned char*>(resourceName)) != 0)
    {
        // 0x4D5B71..0x4D5B7E 固定 loop=1、deferPlay=0；边界只暴露可观察业务结果。
        // 4D5B7A仍传原raw；此时重新按首NUL形成view，不跨前述回调缓存旧长度。
        music.SwitchCurrentSlot(std::string_view(resourceName), playbackParameter);
    }
}
}
