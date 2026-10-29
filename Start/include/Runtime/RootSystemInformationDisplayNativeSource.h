#pragma once
#include <Runtime/SystemDialogNative.h>
#include <Runtime/SystemInformationDisplayNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentRootOwner;
class NativeGraphicsRuntimeLibrary;
class RootLogicalRectangleRuntime;
struct RootSystemInformationDisplayNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    NativeGraphicsRuntimeLibrary& graphics;
    RootLogicalRectangleRuntime& rectangle;
    IModalDialogRunnerServices& modal;
    decltype(&::IsIconic) iconic=&::IsIconic;
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
};
// 同根14h父/88h子共用的非拥有来源；构造/getter不提前选根，不保存任何业务镜像。
// SystemDialogNativeScope由长寿宿主安装，覆盖父构造/Run/EH及最后child Ref释放。
class RootSystemInformationDisplayNativeSource final:
    public ISystemDialogNativeSource,public ISystemInformationDisplayNativeSource
{
public:
    explicit RootSystemInformationDisplayNativeSource(RootSystemInformationDisplayNativeDependencies d) noexcept:dependencies_(d){}
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& RootWindowModeSlotAt0C(void*) override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void*) override;
    std::int32_t& RootModalFlagAt1B8(void*) override;
    std::uint8_t RootMenuStateAt219(void*) override;
    RECT* LogicalRectangleAt478E50(void*,RECT*) override;
    YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void*,YtlReferenceSlotRuntime*) override;
    void* SystemAt4180C0() override;
    HWND SystemWindowAt2C(void*) override;
    void DrawBackgroundAt52687F(std::int32_t,const RECT*) override;
    BOOL IsIconic(HWND) override;
    BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM) override;
    GuiDialogNativeBody5Ch* ConstructSystemDialogAt4F41A0(void*) override;
    std::uint32_t ReadRootModalGateAfter413BE0() override;
    std::int32_t TriggerSystemDialogAt5104A0(GuiDialogNativeBody5Ch*) override;
    IModalDialogRunnerServices& ModalServices() override{return dependencies_.modal;}
private:
    RootSystemInformationDisplayNativeDependencies dependencies_;
};
}
