#include <Runtime/ReadScriptTextServicesRuntime.h>
#include <Runtime/TextWindowSoundSource.h>

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrResourceNameSource.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/TextWindowFormatRuntime.h>
#include <Runtime/TextWindowPlaybackRuntime.h>
#include <Runtime/TextWindowAdvanceRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/WaitMarkRuntime.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptTextServicesRuntime::ReadScriptTextServicesRuntime(TextWindowFormatRuntime& format,
    TextWindowPlaybackRuntime& playback, TextWindowAdvanceRuntime& advance,
    ITextWindowSoundSource& sound, WaitMarkRuntime& waitMark,
    Platform::KeyboardView keyboard, Platform::MouseView mouse,
    const ScrScriptRuntimeState& script, const IScrResourceNameSource& names) noexcept
    : format_(format), playback_(playback), advance_(advance), sound_(sound),
      waitMark_(waitMark), input_(mouse, keyboard), mouse_(mouse), script_(script), names_(names) {}

void ReadScriptTextServicesRuntime::SelectFormatAt4C2740(std::int32_t index, bool initialize)
{
    static_cast<void>(format_.SetFormatAt4C2740(index, initialize));
}
std::int32_t ReadScriptTextServicesRuntime::TickTextAt4BB530(bool suppressAdvance)
{
    return playback_.TickAt4BB530(suppressAdvance);
}
bool ReadScriptTextServicesRuntime::AdvanceTextAt4BC260(std::uint8_t inputMask)
{
    return advance_.TickAt4BC260(inputMask);
}
bool ReadScriptTextServicesRuntime::VoiceArrayAllowedAtSoundParams60()
{
    const auto parameters = sound_.CaptureParametersAt4188C0();
    const auto* voices = sound_.CaptureVoicesSlotAt3C();
    return parameters.Allowed(voices);
}
bool ReadScriptTextServicesRuntime::AnyVoicePlayingAt4196C0()
{
    return sound_.CaptureRootAfter413BE0().Resolve().effects.AnyVoiceSlotPlaying();
}
void ReadScriptTextServicesRuntime::StopVoiceAt4384D0()
{
    sound_.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots();
}
void ReadScriptTextServicesRuntime::StopEffectsAt413270()
{
    // 4B3B52/4B3BE7/4B3FDD 都压入 nullptr，不应清除稍后恢复用的 UnSave 映射。
    sound_.CaptureRootAfter413BE0().Resolve().effects.StopEffectsKeepingMappingsAt413270();
}
bool ReadScriptTextServicesRuntime::ReplayVoiceAt42C4F0()
{
    // 4B35C0固定root，35D9取得voices地址，35DF才解先前root的当前+24。
    const auto root = sound_.CaptureRootAfter413BE0();
    const auto* voices = sound_.CaptureVoicesSlotAt3C();
    return root.Resolve().voice.PlayBatchRawAt42C4F0(voices);
}
void ReadScriptTextServicesRuntime::AdjustEffectsAt4C2410()
{
    sound_.CaptureRootAfter413BE0().Resolve().volume.Adjust();
}
void ReadScriptTextServicesRuntime::RefreshEffectsAt433F60()
{
    sound_.CaptureRootAfter413BE0().Resolve().volume.Refresh();
}
std::uint8_t ReadScriptTextServicesRuntime::ReadWaitMarkEnabledAt0C()
{
    return waitMark_.EnabledByteAt0C();
}
void ReadScriptTextServicesRuntime::WriteWaitMarkEnabledAt0C(std::uint8_t value)
{
    // 原快存/读/退出仅写原 byte，不调用可见性函数的额外动画/dirty 副作用。
    waitMark_.SetEnabledByteAt0C(value);
}
bool ReadScriptTextServicesRuntime::PollKeyboardAt47F4E0(std::uint32_t action)
{
    return input_.ReadPressedAtVslot04(static_cast<std::int32_t>(action));
}
bool ReadScriptTextServicesRuntime::PollMouseAt4B0740()
{
    return mouse_.PreviousPressed(1) == 0 && mouse_.Pressed(1) != 0 && mouse_.ReleaseSuppressionCountdown() == 0;
}
const char* ReadScriptTextServicesRuntime::ReadCurrentScriptNameForError()
{ return ReadCurrentScriptNameForErrorAt4B4397(script_,names_); }
const char* ReadCurrentScriptNameForErrorAt4B4397(const ScrScriptRuntimeState& script,const IScrResourceNameSource& source)
{
    // 4B4397/439E先取CParams及文件ID，43A4..43B3才取Pattern并查询；负ID也要取源。
    const auto id = script.EnvironmentScriptId();
    const auto names = source.CapturePatternNames();
    // 4B43B8直接传raw给417130；不复制、测长或延长Pattern寿命，保留null。
    return names.FindNameAt4AD1B0(id);
}
}
