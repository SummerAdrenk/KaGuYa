#pragma once
#include <Runtime/MainFrameNative.h>
#include <Runtime/IdleLoopNative.h>
#include <Runtime/EditWindowNativeViewport.h>
#include <Runtime/RootControlNativeState.h>
#include <Platform/Input.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;class EnvironmentRootOwner;class GraphicsRootOwner;
class WindowModeNative2Ch;
class IGraphicsModuleNativeWin32Api;
class IMainFrameNativeRootSource
{
public:
    virtual ~IMainFrameNativeRootSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual YtlReferenceSlotRuntime& ModeAt0C(void*)=0;
    virtual RootControlNativeState& ControlAt1AC(void*)=0;
    virtual YtlReferenceSlotRuntime& InformationAt3C(void*)=0;
    virtual void SynchronizeModeAt42116C(void*,WindowModeNative2Ch&)=0;
    virtual DWORD MovieBaseAt48(void*)=0;
    virtual void ResizeMovieAt48(void*)=0;
    virtual void ReleaseWindowObjectsAt4826A0(void*)=0;
    virtual void PaintRootAt483B70(void*,PAINTSTRUCT)=0;
    virtual void UpdateEditViewportAt4875F0(void*)=0;
};
// 全部getter只投影同一已选Root，Env使用原生nullable捕获，不借旧MainWindow安全接口。
class RootMainFrameNativeSource final:public IMainFrameNativeRootSource
{
public:
    RootMainFrameNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,GraphicsRootOwner&,
        IRuntimeStringManagerSource&,IGraphicsModuleNativeWin32Api&,EditWindowNativeViewportServices&,
        EditWindowNativeMapServices&,WindowThunkNativeServices&)noexcept;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    YtlReferenceSlotRuntime& ModeAt0C(void*)override;
    RootControlNativeState& ControlAt1AC(void*)override;
    YtlReferenceSlotRuntime& InformationAt3C(void*)override;
    void SynchronizeModeAt42116C(void*,WindowModeNative2Ch&)override;
    DWORD MovieBaseAt48(void*)override;
    void ResizeMovieAt48(void*)override;
    void ReleaseWindowObjectsAt4826A0(void*)override;
    void PaintRootAt483B70(void*,PAINTSTRUCT)override;
    void UpdateEditViewportAt4875F0(void*)override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;GraphicsRootOwner& graphics_;
    IRuntimeStringManagerSource& strings_;IGraphicsModuleNativeWin32Api& graphicsWindows_;
    EditWindowNativeViewportServices& viewport_;EditWindowNativeMapServices& maps_;WindowThunkNativeServices& thunks_;
};
class IMainFrameNativeWindowApi
{
public:
    virtual ~IMainFrameNativeWindowApi()=default;
    virtual HDC BeginPaint(HWND,PAINTSTRUCT*)=0;
    virtual BOOL EndPaint(HWND,const PAINTSTRUCT*)=0;
    virtual BOOL ValidateRect(HWND,const RECT*)=0;
    virtual BOOL AdjustWindowRectEx(RECT*,DWORD,BOOL,DWORD)=0;
    virtual BOOL IsWindowVisible(HWND)=0;
    virtual BOOL IsIconic(HWND)=0;
    virtual BOOL OpenIcon(HWND)=0;
    virtual BOOL CloseWindow(HWND)=0;
    virtual BOOL GetWindowPlacement(HWND,WINDOWPLACEMENT*)=0;
    virtual BOOL SetWindowPlacement(HWND,const WINDOWPLACEMENT*)=0;
    virtual int MessageBoxA(HWND,const char*,const char*,UINT)=0;
    virtual BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual LRESULT CallWindowProcA(WNDPROC,HWND,UINT,WPARAM,LPARAM)=0;
};
IMainFrameNativeWindowApi& DefaultMainFrameNativeWindowApi()noexcept;
// 每个原叶必须有实际提供者；没有默认false/stub或按地址猜测的通用分派。
class IMainFrameNativeTerminalHandlers
{
public:
    virtual ~IMainFrameNativeTerminalHandlers()=default;
    virtual LRESULT CloseAt423750(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)=0;
    virtual LRESULT DisplayChangeAt424230(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)=0;
    virtual LRESULT ActivateAt420F90(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)=0;
    virtual LRESULT DeviceChangeAt424070(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,BOOL*)=0;
    virtual LRESULT CommandAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*)=0;
    virtual LRESULT UpdateMenuAt428430(UpdateUiNativeBody18h&,UINT,HMENU,LPARAM,BOOL*)=0;
    virtual LRESULT NotifyAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*)=0;
    virtual void UpdateUiAt439100(MainFrameNativeBody90h&,BOOL)=0;
    virtual BOOL FrameBaseAt42DC50(MainFrameNativeBody90h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD)=0;
};
struct MainFrameNativeDispatchServices final
{
    IMainFrameNativeRootSource& roots;
    IMainFrameNativeWindowApi& windows;
    IMainFrameNativeTerminalHandlers& terminals;
    Platform::IWindowHookMessageSource& hooks;
    AppModuleNativeBody3Ch& module;
    AppModuleNativeBody3Ch*& publication;
    AppModuleMessageLoopsNativeServices& loops;
};
BOOL ProcessMainFrameNativeAt42E810(MainFrameNativeBody90h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    MainFrameNativeDispatchServices&);
}
