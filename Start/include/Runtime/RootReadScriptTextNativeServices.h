#pragma once
#include <Runtime/ReadScriptTextServicesRuntime.h>
#include <Runtime/TextWindowNativeFormat.h>
#include <Runtime/TextWindowNativeTextServices.h>
#include <Runtime/TextWindowNativePlaybackServices.h>
#include <Runtime/TextWindowNativePlaybackSource.h>
#include <Runtime/TextWindowNativeAdvance.h>
#include <Runtime/TextWindowNativeMaintenance.h>
#include <Runtime/ReadScriptAdvanceCommandGateNative.h>
#include <Runtime/ParamsResourceNameRuntime.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
struct RootReadScriptTextNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IScrVmCommandErrorSink& errors;
    IFileInfoProgressConfirmation& voiceErrors;
    const char* const& fontFaceAt619824;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 只拥有长寿来源/服务；每次捕获E8并构造调用期layout，不保存窗口或CParams镜像。
class RootReadScriptTextNativeServices final:public IReadScriptTextServices
{
public:
    explicit RootReadScriptTextNativeServices(RootReadScriptTextNativeDependencies)noexcept;
    void SelectFormatAt4C2740(std::int32_t,bool)override;
    std::int32_t TickTextAt4BB530(bool)override;
    bool AdvanceTextAt4BC260(std::uint8_t)override;
    bool VoiceArrayAllowedAtSoundParams60()override;
    bool AnyVoicePlayingAt4196C0()override;
    void StopVoiceAt4384D0()override;
    void StopEffectsAt413270()override;
    bool ReplayVoiceAt42C4F0()override;
    void AdjustEffectsAt4C2410()override;
    void RefreshEffectsAt433F60()override;
    std::uint8_t ReadWaitMarkEnabledAt0C()override;
    void WriteWaitMarkEnabledAt0C(std::uint8_t)override;
    bool PollKeyboardAt47F4E0(std::uint32_t)override;
    bool PollMouseAt4B0740()override;
    const char* ReadCurrentScriptNameForError()override;
private:
    struct Invocation{TextWindowLayerNativeBodyE8h& body;TextWindowLayerNativeServices& window;};
    Invocation CaptureText();
    RootReadScriptTextNativeDependencies dependencies_;
    EnvironmentTextWindowSoundSource sound_;
    RootTextWindowNativeFormatSource format_;
    EnvironmentTextWindowNativePlaybackSource playbackSource_;
    TextWindowNativePlaybackServices playback_;
    EnvironmentTextWindowEffectRestoreSource restore_;
    RootReadScriptAdvanceCommandGateNativeSource command_;
    RootTextWindowNativeMaintenanceSource maintenance_;
    RootTextWindowNativeAdvanceSource advanceSource_;
    TextWindowNativeAdvanceServices advance_;
    ParamsResourceNameRuntime names_;
};
}
