#include <Runtime/RootProgramSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/SceneManagerView.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
SceneManagerView Scenes(void* capturedRoot,void* manager)
{
    auto* services=static_cast<RootRuntimePayload*>(capturedRoot)->PostWindow().BorrowSceneManagerNativeServices();
    if(!services)throw std::logic_error("Captured Program Root has no native SceneManager services");
    return SceneManagerView(static_cast<SceneManagerNativeBody48h*>(manager),*services);
}
}
YtlReferenceSlotRuntime* RootProgramSceneNativeSource::CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime* output,unsigned)
{
    // 原4ACDD0不消费Draw ECX；screen/尺寸/像素仍由同一个Graphics库取得。
    CaptureGraphicsScreenAt4ACDD0(*output,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);
    return output;
}
void RootProgramSceneNativeSource::SubmitPreviewAt48CFF0(void* capturedRoot,const YtlReferenceSlotRuntime* image,unsigned)
{
    auto& root=*static_cast<RootRuntimePayload*>(capturedRoot);
    auto preview=root.CreateSavePreviewService(dependencies_.graphics,dependencies_.graphics,
        dependencies_.graphics,dependencies_.root.Files(),dependencies_.previewCapture);
    preview.SetCaptureAt48CFF0(*image);
}
void RootProgramSceneNativeSource::SetTransitionAt4C1C60(void* root,YtlReferenceSlotRuntime owned,unsigned)
{AssignGraphicsSurfaceReferenceAt4C1C60(static_cast<RootRuntimePayload*>(root)->TransitionFrameSlotAt1F0(),owned);}
void RootProgramSceneNativeSource::ResetDrawAt47E940(void* capturedRoot,void* draw,unsigned)
{
    const auto services=static_cast<RootRuntimePayload*>(capturedRoot)->PostWindow().BorrowNativeDrawCallServices();
    if(!services.layers||!services.layerRuntime)
        throw std::logic_error("Captured Program Root has no native Draw layer services");
    // 47E944固定传入Draw的begin；只借该Root的服务，不重读Root.Draw或借ambient图层。
    DrawLayersView(static_cast<DrawManagerNativeBody2Ch*>(draw),dependencies_.memory)
        .WithNativeCallServices(services).ResetOwnedLayersAt47E940(dependencies_.graphics);
}
void RootProgramSceneNativeSource::AdvanceScenesAt4C1760(void* root,void* scenes,unsigned)
{Scenes(root,scenes).PopThenActivatePendingAt4C1760();}
void RootProgramSceneNativeSource::PushSceneAt4A9030(void* root,void* scenes,const char* name,unsigned)
{Scenes(root,scenes).PushActiveScene(name);}
void RootProgramSceneNativeSource::ClearScenesAt4C1460(void* root,void* scenes,unsigned)
{Scenes(root,scenes).ClearScenes();}
void RootProgramSceneNativeSource::QueueSceneAt47F920(void* root,void* scenes,const char* name,unsigned)
{Scenes(root,scenes).QueueScene(name);}
void RootProgramSceneNativeSource::WriteRootByteAt1AF(void* root,std::uint8_t value,unsigned)
{static_cast<RootRuntimePayload*>(root)->WriteByteAt1AF(value);}
void RootProgramSceneNativeSource::QuickSaveAt485FC0(void*,std::uint8_t value,unsigned)
{QuickOperationsNativeScope scope(dependencies_.quick);QuickSaveNativeAt485FC0(value);}
std::uint8_t RootProgramSceneNativeSource::QuickLoadAt485CE0(void*,unsigned)
{QuickOperationsNativeScope scope(dependencies_.quick);return QuickLoadNativeAt485CE0();}
GuiDialogNativeBody5Ch* RootProgramSceneNativeSource::ConstructConfirmAt4E3330(void* raw,int mode)
{
    GeneralConfirmDialogNativeScope scope(dependencies_.confirmation);
    return ConstructGeneralConfirmDialogNativeAt4E3330(raw,mode);
}
void RootProgramSceneNativeSource::AssignConfirmAt48CA30(GuiDialogNativeBody5Ch* target,YtlReferenceSlotRuntime owned,unsigned)
{AssignGeneralConfirmDialogNativeCaptureAt48CA30(target,owned);}
int RootProgramSceneNativeSource::RunModalAt5071D0(void*,YtlReferenceSlotRuntime* borrowed,unsigned)
{return RunNativeGuiModalReferenceAt5071D0(borrowed,dependencies_.confirmation.modal);}
BOOL RootProgramSceneNativeSource::PostMessage(HWND window,UINT message,WPARAM w,LPARAM l,unsigned)
{return dependencies_.postMessage(window,message,w,l);}
}
