#pragma once
#include <Runtime/TextWindowNativePlaybackServices.h>

namespace KaGuYa::Start::Runtime
{
class IFileInfoProgressConfirmation;
// 已确认的4C315D原Ref12 varargs；不增持、不把array解析成声音名字列表。
void FormatTextVoiceFailureAt4C315D(RuntimeCString&, YtlReferenceSlotRuntime names);

class EnvironmentTextWindowNativePlaybackSource final : public ITextWindowNativePlaybackSource
{
public:
    EnvironmentTextWindowNativePlaybackSource(ITextWindowSoundSource&, RootRuntimeOwner&,
        IFileInfoProgressConfirmation&) noexcept;
    TextWindowNativeSoundReceiver CaptureSoundAfter413BE0() override;
    void FormatVoiceFailureAt417130(RuntimeCString&, YtlReferenceSlotRuntime) override;
    int ShowMessageBoxA(HWND, const char*, const char*, UINT) override;
    void ClearRootUpdateEnabledAt1B0After413BE0() override;
private:
    ITextWindowSoundSource& sound_;
    RootRuntimeOwner& root_;
    IFileInfoProgressConfirmation& messages_;
};
}
