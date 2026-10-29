#pragma once
#include <Runtime/MainFrameNative.h>
#include <Runtime/RootMenuPositionNativeState.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class IMainFramePopupPositionNativeSource
{
public:
    virtual ~IMainFramePopupPositionNativeSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual RootMenuPositionNativeState0Ch& PositionAt208(void*)=0;
    virtual const YtlReferenceSlotRuntime& ModeAt0C(void*)=0;
    virtual void* GameSystemAt418480()=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
};
class RootMainFramePopupPositionNativeSource final:public IMainFramePopupPositionNativeSource
{
public:
    RootMainFramePopupPositionNativeSource(RootRuntimeOwner&,ParamsRuntimeRootOwner&,EnvironmentRootOwner&)noexcept;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    RootMenuPositionNativeState0Ch& PositionAt208(void*)override;
    const YtlReferenceSlotRuntime& ModeAt0C(void*)override;
    void* GameSystemAt418480()override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
private:
    RootRuntimeOwner& root_;ParamsRuntimeRootOwner& params_;EnvironmentRootOwner& environment_;
};
class IMainFramePopupPositionNativeApi
{
public:
    virtual ~IMainFramePopupPositionNativeApi()=default;
    virtual LRESULT CallWindowProcA(WNDPROC,HWND,UINT,WPARAM,LPARAM)=0;
    virtual LONG GetWindowLongA(HWND,int)=0;
    virtual LONG SetWindowLongA(HWND,int,LONG)=0;
    virtual BOOL ClientToScreen(HWND,POINT*)=0;
};
IMainFramePopupPositionNativeApi& DefaultMainFramePopupPositionNativeApi()noexcept;
struct MainFramePopupPositionNativeServices final
{
    IMainFramePopupPositionNativeSource& source;
    IMainFramePopupPositionNativeApi& api;
    WindowThunkNativeServices thunks;
    IEnvironmentRootMemory& memory;
};
class MainFramePopupPositionNativeScope final
{
public:
    explicit MainFramePopupPositionNativeScope(MainFramePopupPositionNativeServices&)noexcept;
    ~MainFramePopupPositionNativeScope()noexcept;
    MainFramePopupPositionNativeScope(const MainFramePopupPositionNativeScope&)=delete;
    MainFramePopupPositionNativeScope& operator=(const MainFramePopupPositionNativeScope&)=delete;
private:
    MainFramePopupPositionNativeServices* previous_;
};
const EditWindowEndpointNativeVtable4& MainFramePopupPositionNativeVtable()noexcept;
BOOL InstallMainFramePopupPositionNativeAt4387D0(EditWindowEndpointNativeBody24h&,HWND,MainFramePopupPositionNativeServices&);
BOOL ProcessMainFramePopupPositionNativeAt430CA0(EditWindowEndpointNativeBody24h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    MainFramePopupPositionNativeServices&);
// 四次Params各自捕获；入口Root不随随后Ensure换代，输出仅在最终转换后按X/Y次序提交。
void LocateMainFramePopupNativeAt46B640(void* capturedRoot,DWORD* x,DWORD* y,IMainFramePopupPositionNativeSource&,
    IMainFramePopupPositionNativeApi&);
}
