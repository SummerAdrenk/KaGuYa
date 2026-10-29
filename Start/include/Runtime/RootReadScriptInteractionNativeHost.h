#pragma once
#include <Runtime/ReadScriptInteractionNativeHost.h>
#include <Runtime/ReadScriptQuickCaptureNative.h>
#include <Runtime/ReadScriptSoundRequestsRuntime.h>
#include <Runtime/RootReadScriptSaveCaptureNative.h>
#include <Runtime/QuickOperationsNative.h>
#include <Runtime/AutoSaveOperationsNative.h>
#include <Runtime/SceneManagerView.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
struct RootReadScriptInteractionNativeHostDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    ToolWindowNativeInteractionServices& tool;
    ReadScriptSoundRequestsRuntime& sound;
    IReadScriptSaveCaptureNativeSource& saveCapture;
    QuickOperationsNativeServices& quick;
    AutoSaveOperationsNativeServices& automatic;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 原生叶没有固定Scenes/CParams/HWND/Root字段引用，不拥有第二份业务状态。
class RootReadScriptInteractionNativeHost final:public IReadScriptInteractionNativeHost,
    private IReadScriptQuickCaptureNativeSource
{
public:
    explicit RootReadScriptInteractionNativeHost(RootReadScriptInteractionNativeHostDependencies)noexcept;
    void ClearModalResultAt10()override;
    void ProcessSoundRequestsAt4A9090()override;
    bool ScriptHasWaitMarkerAt5C()override;
    void BackupBeforeSaveLoadAt4B01A0()override;
    void CaptureQuickOperationAt4C1C60()override;
    void QuickSaveAt485FC0(std::int32_t)override;
    bool QuickLoadAt485CE0()override;
    void ClearScenesAt4C1460()override;
    void QueueSceneAt47F920(const char*)override;
    void ActivateSceneAt4A9030(const char*)override;
    void MarkRootSaveLoadAt1AF()override;
    void ShowPublicDialogAt4C1760()override;
    void AutoSaveAt46B280(std::uint8_t)override;
    void CaptureToolAt4C01F0()override;
    void SetToolCaptureModeAt4C2300(bool)override;
    void RestoreToolAt4BF4E0()override;
    std::uint8_t ReadWaitMarkEnabledAt0C()override;
    void WriteWaitMarkEnabledAt0C(std::uint8_t)override;
private:
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    DrawLayersView LayersAt4175A0(void*)override;
    YtlReferenceSlotRuntime& FrameAt1F0(void*)override;
    RootRuntimePayload& CurrentRoot();
    SceneManagerView CurrentScenes();
    void ResolveUnusedSceneReceiver();
    RootReadScriptInteractionNativeHostDependencies dependencies_;
    ReadScriptQuickCaptureNativeServices capture_;
};
struct RootReadScriptInteractionNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    ToolWindowNativeInteractionServices& tool;
    ISavePreviewCaptureServices& preview;
    QuickOperationsNativeServices& quick;
    AutoSaveOperationsNativeServices& automatic;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
// 只拥有长寿服务适配器；body/Ref/文件/图层仍属于真正Root。
class RootReadScriptInteractionNativeRuntime final
{
public:
    explicit RootReadScriptInteractionNativeRuntime(RootReadScriptInteractionNativeDependencies)noexcept;
    RootReadScriptInteractionNativeHost& Host()noexcept{return host_;}
private:
    EnvironmentReadScriptSoundRequestsSource soundSource_;
    ReadScriptSoundRequestsRuntime sound_;
    RootReadScriptSaveCaptureNativeSource saveCapture_;
    RootReadScriptInteractionNativeHost host_;
};
}
