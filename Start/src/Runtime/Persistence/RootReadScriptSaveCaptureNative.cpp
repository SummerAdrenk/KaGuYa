#include <Runtime/RootReadScriptSaveCaptureNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/RootSavePreviewRuntime.h>
#include <Runtime/ToolWindowNativeInteraction.h>

namespace KaGuYa::Start::Runtime
{
void RootReadScriptSaveCaptureNativeSource::EnsureRootAt413BE0(std::uint32_t){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootReadScriptSaveCaptureNativeSource::RootAt6198C4(std::uint32_t){return dependencies_.root.ReadPublishedAt6198C4();}
DrawManagerNativeBody2Ch* RootReadScriptSaveCaptureNativeSource::DrawAt4B01D6(void* root)
{return ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18());}
YtlReferenceSlotRuntime* RootReadScriptSaveCaptureNativeSource::SelectToolAt417B50(ToolWindowNativeBody40h* tool,YtlReferenceSlotRuntime& out)
{return SelectToolDialogNativeAt417B50(*tool,out,dependencies_.tool.window.replay);}
void RootReadScriptSaveCaptureNativeSource::CapturePreviewAt48DBE0(void* root)
{
    auto& captured=*static_cast<RootRuntimePayload*>(root);auto& g=dependencies_.graphics;
    auto preview=captured.CreateSavePreviewService(g,g,g,dependencies_.root.Files(),dependencies_.preview);
    preview.CaptureForSaveAt48DBE0();
}
}
