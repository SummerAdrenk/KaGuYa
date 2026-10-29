#pragma once
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ReadScriptSceneNativeBodyBCh;
class IReadScriptReturnTitleNativeSource
{
public:
    virtual ~IReadScriptReturnTitleNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t caller)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t caller)=0;
    virtual void* RootAt6198C4(std::uint32_t caller)=0;
    virtual std::uint8_t ReadWaitMarkAt418C10(void* capturedRoot,std::uint32_t caller)=0;
    virtual void WriteWaitMarkAt418C10(void* capturedRoot,std::uint8_t,std::uint32_t caller)=0;
    virtual void CaptureToolAt4C01F0(void* capturedRoot,std::uint32_t caller)=0;
    virtual void SetToolCaptureModeAt4C2300(void* capturedRoot,std::uint8_t,std::uint32_t caller)=0;
    virtual void RestoreToolAt4BF4E0(void* capturedRoot,std::uint32_t caller)=0;
    virtual DrawLayersView LayersAt4175A0(void* capturedRoot,std::uint32_t caller)=0;
    virtual IModalDialogRunnerServices& ModalAt4AD230(void* capturedRoot,std::uint32_t caller)=0;
    virtual BOOL PostMessage(HWND,UINT,WPARAM,LPARAM,std::uint32_t caller)=0;
};
struct ReadScriptReturnTitleNativeServices final
{
    IReadScriptReturnTitleNativeSource& source;
    GeneralConfirmDialogNativeServices& confirmation;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenSizeApi& sizes;
    IEnvironmentRootMemory& memory;
};
// 必需的正文外元数据；生产provider须持有同Root服务，不能在缺失时改用旧确认框。
class IReadScriptReturnTitleNativeServicesSource
{
public:
    virtual ~IReadScriptReturnTitleNativeServicesSource()=default;
    virtual ReadScriptReturnTitleNativeServices& ReturnTitleNativeServices()=0;
};
enum class ReadScriptReturnTitleCaller{Text,Menu};
// 整个原case19。无finally恢复；normal40D8A0和EH40D8A0都属于flags5终止域。
bool RunReadScriptReturnTitleNative(ReadScriptSceneNativeBodyBCh&,
    ReadScriptReturnTitleNativeServices&,ReadScriptReturnTitleCaller);
}
