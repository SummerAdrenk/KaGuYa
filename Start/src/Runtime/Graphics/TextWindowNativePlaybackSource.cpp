#include <Runtime/TextWindowNativePlaybackSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileInfoProgressRuntime.h>

namespace KaGuYa::Start::Runtime
{
void FormatTextVoiceFailureAt4C315D(RuntimeCString& text, YtlReferenceSlotRuntime names)
{
    static_assert(sizeof(names) == 12 && std::is_trivially_copyable_v<YtlReferenceSlotRuntime>);
    // PE5B4924 CP932常量；4C3140..315D以struct按值压12字节，%s读取第一个DWORD。
    static constexpr char format[] = "\x89\xB9\x90\xBA\x82\xCC\x8D\xC4\x90\xB6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : %s";
    FormatRuntimeCStringAt417130(text, format, names);
}
EnvironmentTextWindowNativePlaybackSource::EnvironmentTextWindowNativePlaybackSource(ITextWindowSoundSource& sound,
    RootRuntimeOwner& root, IFileInfoProgressConfirmation& messages) noexcept
    : sound_(sound), root_(root), messages_(messages) {}
TextWindowNativeSoundReceiver EnvironmentTextWindowNativePlaybackSource::CaptureSoundAfter413BE0()
{ return TextWindowNativeSoundReceiver(sound_.CaptureRootAfter413BE0().Resolve()); }
void EnvironmentTextWindowNativePlaybackSource::FormatVoiceFailureAt417130(RuntimeCString& text, YtlReferenceSlotRuntime names)
{ FormatTextVoiceFailureAt4C315D(text, names); }
int EnvironmentTextWindowNativePlaybackSource::ShowMessageBoxA(HWND window, const char* text, const char* caption, UINT flags)
{ return messages_.Confirm(window, text, caption, flags); }
void EnvironmentTextWindowNativePlaybackSource::ClearRootUpdateEnabledAt1B0After413BE0()
{
    root_.EnsureCreatedAt413BE0();
    root_.RequirePublished().Frame().FrameWaitByteAt1B0() = 0;
}
}
