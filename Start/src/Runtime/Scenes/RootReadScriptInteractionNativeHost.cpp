#include <Runtime/RootReadScriptInteractionNativeHost.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/ToolWindowNativeInteraction.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootReadScriptInteractionNativeHost::RootReadScriptInteractionNativeHost(RootReadScriptInteractionNativeHostDependencies d)noexcept
    :dependencies_(d),capture_{*this,d.graphics,d.graphics}{}
void RootReadScriptInteractionNativeHost::EnsureRootAt413BE0(){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootReadScriptInteractionNativeHost::RootAt6198C4(){return dependencies_.root.ReadPublishedAt6198C4();}
RootRuntimePayload& RootReadScriptInteractionNativeHost::CurrentRoot()
{EnsureRootAt413BE0();return *static_cast<RootRuntimePayload*>(RootAt6198C4());}
SceneManagerView RootReadScriptInteractionNativeHost::CurrentScenes()
{
    auto& root=CurrentRoot();const auto current=root.PostWindow().BorrowSceneManagerAt30();
    if(!current.IsNative())throw std::logic_error("Native ReadScript Host requires native scene services");
    return SceneManagerView(current.NativeBody(),current.NativeServices());
}
void RootReadScriptInteractionNativeHost::ResolveUnusedSceneReceiver()
{
    // 485CE0/485FC0/46B280不消费传入ECX；caller解Root+30仍执行，空body不提前拒绝。
    auto& root=CurrentRoot();static_cast<void>(ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(root.SceneManagerSlotAt30()));
}
void RootReadScriptInteractionNativeHost::ClearModalResultAt10()
{
    auto* info=ResolveEnvironmentObjectReference<void>(CurrentRoot().InformationDisplaySlotAt3C());
    *reinterpret_cast<std::uint32_t*>(static_cast<std::byte*>(info)+0x10)=0;
}
void RootReadScriptInteractionNativeHost::ProcessSoundRequestsAt4A9090(){dependencies_.sound.ProcessSoundRequestsAt4A9090();}
bool RootReadScriptInteractionNativeHost::ScriptHasWaitMarkerAt5C()
{
    auto* input=dependencies_.input.CaptureAt418210();
    auto* keyboard=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    const volatile auto& action=keyboard->actionsAt14[6];auto* position=action.begin;const auto* end=action.end;
    if(position==end)return false;
    const auto buffer=*static_cast<const volatile std::uint32_t*>(&keyboard->bufferAt10);
    do
    {
        if(*reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(position)+1u+buffer))return true;
        ++position;
    }while(position!=end);
    return false;
}
void RootReadScriptInteractionNativeHost::BackupBeforeSaveLoadAt4B01A0()
{CaptureBeforeSaveNativeAt4B01A0(dependencies_.saveCapture);}
DrawLayersView RootReadScriptInteractionNativeHost::LayersAt4175A0(void* root)
{
    auto& captured=*static_cast<RootRuntimePayload*>(root);
    auto* draw=ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(captured.DrawManagerSlotAt18());
    return DrawLayersView(draw,dependencies_.memory).WithNativeCallServices(captured.PostWindow().BorrowNativeDrawCallServices());
}
YtlReferenceSlotRuntime& RootReadScriptInteractionNativeHost::FrameAt1F0(void* root)
{
    auto& captured=*static_cast<RootRuntimePayload*>(root);captured.BindGraphicsSurfaceApi(dependencies_.graphics);
    return captured.TransitionFrameSlotAt1F0();
}
void RootReadScriptInteractionNativeHost::CaptureQuickOperationAt4C1C60(){CaptureReadScriptQuickNativeAt4C1C60(capture_);}
void RootReadScriptInteractionNativeHost::QuickSaveAt485FC0(std::int32_t argument)
{ResolveUnusedSceneReceiver();QuickOperationsNativeScope scope(dependencies_.quick);QuickSaveNativeAt485FC0(std::uint32_t(argument));}
bool RootReadScriptInteractionNativeHost::QuickLoadAt485CE0()
{ResolveUnusedSceneReceiver();QuickOperationsNativeScope scope(dependencies_.quick);return QuickLoadNativeAt485CE0();}
void RootReadScriptInteractionNativeHost::AutoSaveAt46B280(std::uint8_t argument)
{ResolveUnusedSceneReceiver();AutoSaveOperationsNativeScope scope(dependencies_.automatic);AutoSaveNativeAt46B280(argument);}
void RootReadScriptInteractionNativeHost::ClearScenesAt4C1460(){CurrentScenes().ClearScenes();}
void RootReadScriptInteractionNativeHost::QueueSceneAt47F920(const char* name){CurrentScenes().QueueScene(name);}
void RootReadScriptInteractionNativeHost::ActivateSceneAt4A9030(const char* name){CurrentScenes().PushActiveScene(name);}
void RootReadScriptInteractionNativeHost::ShowPublicDialogAt4C1760(){CurrentScenes().PopThenActivatePendingAt4C1760();}
void RootReadScriptInteractionNativeHost::MarkRootSaveLoadAt1AF(){CurrentRoot().WriteByteAt1AF(1);}
void RootReadScriptInteractionNativeHost::CaptureToolAt4C01F0()
{auto& root=CurrentRoot();LayersAt4175A0(&root).ToolAtSlotTwo(&dependencies_.tool).CaptureAt4C01F0();}
void RootReadScriptInteractionNativeHost::SetToolCaptureModeAt4C2300(bool value)
{auto& root=CurrentRoot();LayersAt4175A0(&root).ToolAtSlotTwo(&dependencies_.tool).SetCaptureModeAt4C2300(std::uint8_t(value));}
void RootReadScriptInteractionNativeHost::RestoreToolAt4BF4E0()
{auto& root=CurrentRoot();LayersAt4175A0(&root).ToolAtSlotTwo(&dependencies_.tool).RestoreAt4BF4E0();}
std::uint8_t RootReadScriptInteractionNativeHost::ReadWaitMarkEnabledAt0C()
{
    auto* draw=ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(CurrentRoot().DrawManagerSlotAt18());
    return DrawLayerAt418C10(*draw,4)->enabledAt0C;
}
void RootReadScriptInteractionNativeHost::WriteWaitMarkEnabledAt0C(std::uint8_t value)
{
    auto* draw=ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(CurrentRoot().DrawManagerSlotAt18());
    DrawLayerAt418C10(*draw,4)->enabledAt0C=value;
}
RootReadScriptInteractionNativeRuntime::RootReadScriptInteractionNativeRuntime(RootReadScriptInteractionNativeDependencies d)noexcept
    :soundSource_(d.environment,d.params,d.input,d.root),sound_(soundSource_),
     saveCapture_({d.root,d.graphics,d.tool,d.preview}),
     host_({d.root,d.environment,d.input,d.graphics,d.tool,sound_,saveCapture_,d.quick,d.automatic,d.memory}){}
}
