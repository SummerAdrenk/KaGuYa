#pragma once
#include <Runtime/EditWindowNative.h>
#include <Runtime/WindowClassNative.h>
#include <Runtime/WindowThunkNative.h>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentRootMemory;
struct DialogEndpointNativeBody20h;
struct DialogEndpointNativeVtable4 final
{
    BOOL(__thiscall* processAt00)(DialogEndpointNativeBody20h*,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD);
    DialogEndpointNativeBody20h*(__thiscall* deleteAt04)(DialogEndpointNativeBody20h*,DWORD);
    DLGPROC(__thiscall* procedureAt08)(DialogEndpointNativeBody20h*);
    void(__thiscall* finalMessageAt0C)(DialogEndpointNativeBody20h*,HWND);
};
// 原About20h及StartupSetting28h共同前缀；没有普通窗口的+20 previous WNDPROC。
struct DialogEndpointNativeBody20h final
{
    const DialogEndpointNativeVtable4* vptrAt00;
    HWND windowAt04;
    union{Platform::PendingWindowCreateRecord creationAt08;};
    void* thunkAt14;
    void* currentMessageAt18;
    DWORD destroyPendingAt1C;
    DialogEndpointNativeBody20h()noexcept{}
};
static_assert(sizeof(DialogEndpointNativeVtable4)==0x10&&sizeof(DialogEndpointNativeBody20h)==0x20);
static_assert(offsetof(DialogEndpointNativeBody20h,creationAt08)==8&&offsetof(DialogEndpointNativeBody20h,thunkAt14)==0x14);
static_assert(offsetof(DialogEndpointNativeBody20h,currentMessageAt18)==0x18&&offsetof(DialogEndpointNativeBody20h,destroyPendingAt1C)==0x1C);
class IDialogEndpointNativeApi
{
public:
    virtual ~IDialogEndpointNativeApi()=default;
    virtual DWORD GetCurrentThreadId()=0;
    virtual void EnterCriticalSection(LPCRITICAL_SECTION)=0;
    virtual void LeaveCriticalSection(LPCRITICAL_SECTION)=0;
    virtual LONG SetWindowLongA(HWND,int,LONG)=0;
    virtual LONG GetWindowLongA(HWND,int)=0;
    virtual HWND GetParent(HWND)=0;
    virtual HWND GetWindow(HWND,UINT)=0;
    virtual BOOL GetWindowRect(HWND,RECT*)=0;
    virtual BOOL GetClientRect(HWND,RECT*)=0;
    virtual int MapWindowPoints(HWND,HWND,POINT*,UINT)=0;
    virtual HMONITOR MonitorFromWindow(HWND,DWORD)=0;
    virtual BOOL GetMonitorInfoA(HMONITOR,MONITORINFO*)=0;
    virtual BOOL SetWindowPos(HWND,HWND,int,int,int,int,UINT)=0;
};
IDialogEndpointNativeApi& DefaultDialogEndpointNativeApi()noexcept;
struct DialogEndpointNativeServices final
{
    WindowClassWinModuleNativeBody2Ch& winModule;
    IDialogEndpointNativeApi& api;
    WindowThunkNativeServices thunks;
    IEnvironmentRootMemory& memory;
};
DialogEndpointNativeServices DefaultDialogEndpointNativeServices()noexcept;
class DialogEndpointNativeScope final
{
public:
    explicit DialogEndpointNativeScope(DialogEndpointNativeServices&)noexcept;
    ~DialogEndpointNativeScope()noexcept;
    DialogEndpointNativeScope(const DialogEndpointNativeScope&)=delete;
    DialogEndpointNativeScope& operator=(const DialogEndpointNativeScope&)=delete;
private:
    DialogEndpointNativeServices* previous_;
};
DialogEndpointNativeServices& RequireDialogEndpointNativeServices();
void PushDialogCreationNativeAt42F8B9(DialogEndpointNativeBody20h&,DialogEndpointNativeServices&);
void* PopWindowCreationNativeAt416B00(WindowClassWinModuleNativeBody2Ch*,IDialogEndpointNativeApi&);
INT_PTR CALLBACK InitialDialogEndpointNativeAt4382D0(HWND,UINT,WPARAM,LPARAM);
INT_PTR CALLBACK BoundDialogEndpointNativeAt415810(HWND,UINT,WPARAM,LPARAM);
INT_PTR DispatchDialogEndpointNativeAt415810(DialogEndpointNativeBody20h&,UINT,WPARAM,LPARAM,IDialogEndpointNativeApi&);
void ReleaseDialogEndpointThunkNativeAt40E730(DialogEndpointNativeBody20h&,DialogEndpointNativeServices&);
// 原this是HWND槽地址；每个原[esi]位置都晚读，不借按值HWND的现代CenterWindow。
BOOL CenterDialogWindowNativeAt413990(const HWND*,HWND,IDialogEndpointNativeApi&);
}
