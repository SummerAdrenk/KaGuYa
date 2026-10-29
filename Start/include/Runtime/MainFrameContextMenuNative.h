#pragma once
#include <Runtime/MainFrameNative.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/WindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class IMainFrameContextMenuNativeSource
{
public:
    virtual ~IMainFrameContextMenuNativeSource()=default;
    virtual HINSTANCE ResourceInstanceAt61A6A0()=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual const YtlReferenceSlotRuntime& ModeAt0C(void*)=0;
    virtual const YtlReferenceSlotRuntime& DrawAt18(void*)=0;
    virtual void WritePopupAt1B1(void*,BYTE)=0;
    virtual InputRootNativeBody24h* InputAt418210()=0;
};
class IMainFrameContextMenuNativeApi
{
public:
    virtual ~IMainFrameContextMenuNativeApi()=default;
    virtual HMENU LoadMenuA(HINSTANCE,LPCSTR)=0;
    virtual HMENU GetSubMenu(HMENU,int)=0;
    virtual BOOL IsMenu(HMENU)=0;
    virtual BOOL SetMenuItemInfoA(HMENU,UINT,BOOL,const MENUITEMINFOA*)=0;
    virtual UINT EnableMenuItem(HMENU,UINT,UINT)=0;
    virtual BOOL ClientToScreen(HWND,POINT*)=0;
    virtual HMONITOR MonitorFromPoint(POINT,DWORD)=0;
    virtual BOOL GetMonitorInfoA(HMONITOR,MONITORINFO*)=0;
    virtual BOOL TrackPopupMenu(HMENU,UINT,int,int,int,HWND,const RECT*)=0;
    virtual BOOL DestroyMenu(HMENU)=0;
};
IMainFrameContextMenuNativeApi& DefaultMainFrameContextMenuNativeApi()noexcept;
// 只借同一生产Root/Environment/Input及原Base.resourceInstance槽；构造不取根。
class RootMainFrameContextMenuNativeSource final:public IMainFrameContextMenuNativeSource
{
public:
    RootMainFrameContextMenuNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,InputRootOwner&,
        WindowClassModuleNativeBody38h&)noexcept;
    HINSTANCE ResourceInstanceAt61A6A0()override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    const YtlReferenceSlotRuntime& ModeAt0C(void*)override;
    const YtlReferenceSlotRuntime& DrawAt18(void*)override;
    void WritePopupAt1B1(void*,BYTE)override;
    InputRootNativeBody24h* InputAt418210()override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;InputRootOwner& input_;
    WindowClassModuleNativeBody38h& nativeBase_;
};
// 413F30只设置原分支命中的条目；不先清全表、不以Win32返回值门控后续取源。
void PrepareMainFrameContextMenuNativeAt413F30(HMENU*,IMainFrameContextMenuNativeSource&,
    IMainFrameContextMenuNativeApi& = DefaultMainFrameContextMenuNativeApi());
int ClampMainFramePopupXNativeAt43A680(POINT,IMainFrameContextMenuNativeApi& = DefaultMainFrameContextMenuNativeApi());
LRESULT ShowMainFrameContextMenuNativeAt428C10(MainFrameNativeBody90h&,DWORD,DWORD,LPARAM,BOOL*,
    IMainFrameContextMenuNativeSource&,IMainFrameContextMenuNativeApi& = DefaultMainFrameContextMenuNativeApi());
}
