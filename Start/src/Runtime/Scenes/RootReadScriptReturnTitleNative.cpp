#include <Runtime/RootReadScriptReturnTitleNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/ToolWindowNativeInteraction.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
DrawManagerNativeBody2Ch* Draw(void* root)
{return ResolveEnvironmentObjectReference<DrawManagerNativeBody2Ch>(static_cast<RootRuntimePayload*>(root)->DrawManagerSlotAt18());}
}
EnvironmentRootStorage* RootReadScriptReturnTitleNativeSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return dependencies_.environment.CaptureAt4180C0();}
void RootReadScriptReturnTitleNativeSource::EnsureRootAt413BE0(std::uint32_t){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootReadScriptReturnTitleNativeSource::RootAt6198C4(std::uint32_t){return dependencies_.root.ReadPublishedAt6198C4();}
std::uint8_t RootReadScriptReturnTitleNativeSource::ReadWaitMarkAt418C10(void* root,std::uint32_t)
{return DrawLayerAt418C10(*Draw(root),4)->enabledAt0C;}
void RootReadScriptReturnTitleNativeSource::WriteWaitMarkAt418C10(void* root,std::uint8_t value,std::uint32_t)
{DrawLayerAt418C10(*Draw(root),4)->enabledAt0C=value;}
DrawLayersView RootReadScriptReturnTitleNativeSource::LayersAt4175A0(void* root,std::uint32_t)
{
    auto* captured=static_cast<RootRuntimePayload*>(root);
    return DrawLayersView(Draw(root),dependencies_.memory).WithNativeCallServices(captured->PostWindow().BorrowNativeDrawCallServices());
}
void RootReadScriptReturnTitleNativeSource::CaptureToolAt4C01F0(void* root,std::uint32_t site)
{LayersAt4175A0(root,site).ToolAtSlotTwo(&dependencies_.tool).CaptureAt4C01F0();}
void RootReadScriptReturnTitleNativeSource::SetToolCaptureModeAt4C2300(void* root,std::uint8_t value,std::uint32_t site)
{LayersAt4175A0(root,site).ToolAtSlotTwo(&dependencies_.tool).SetCaptureModeAt4C2300(value);}
void RootReadScriptReturnTitleNativeSource::RestoreToolAt4BF4E0(void* root,std::uint32_t site)
{LayersAt4175A0(root,site).ToolAtSlotTwo(&dependencies_.tool).RestoreAt4BF4E0();}
IModalDialogRunnerServices& RootReadScriptReturnTitleNativeSource::ModalAt4AD230(void* root,std::uint32_t)
{
    // 4AD230仅解固定Root+3C，5071D0不消费ECX；不额外Ensure或增持。
    static_cast<void>(ResolveEnvironmentObjectReference<void>(static_cast<RootRuntimePayload*>(root)->InformationDisplaySlotAt3C()));
    return dependencies_.confirmation.modal;
}
BOOL RootReadScriptReturnTitleNativeSource::PostMessage(HWND window,UINT message,WPARAM w,LPARAM l,std::uint32_t)
{return dependencies_.postMessage(window,message,w,l);}
RootReadScriptReturnTitleNativeRuntime::RootReadScriptReturnTitleNativeRuntime(RootReadScriptReturnTitleNativeDependencies d)noexcept
    :source_(d),services_{source_,d.confirmation,d.graphics,d.graphics,d.memory}{}
}
