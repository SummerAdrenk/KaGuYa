#pragma once
#include <Runtime/MainFrameNative.h>
#include <Runtime/WindowClassNative.h>
#include <CommCtrl.h>

namespace KaGuYa::Start::Runtime
{
// 原生结构仍在F/U正文内；服务只借用，不增加虚表、所有权或默认业务状态。
static_assert(sizeof(BOOL)==4 && sizeof(wchar_t)==2 && sizeof(LRESULT)==4);
static_assert(sizeof(MENUITEMINFOA)==0x30 && offsetof(MENUITEMINFOA,dwTypeData)==0x24);
static_assert(sizeof(OSVERSIONINFOEXW)==0x11C);
static_assert(offsetof(NMHDR,code)==8);
static_assert(offsetof(NMTTDISPINFOA,szText)==0x10 && offsetof(NMTTDISPINFOA,uFlags)==0x64);
static_assert(offsetof(NMTTDISPINFOW,szText)==0x10 && offsetof(NMTTDISPINFOW,uFlags)==0xB4);
static_assert(sizeof(NMTTDISPINFOA::szText)==0x50 && sizeof(NMTTDISPINFOW::szText)==0xA0);
static_assert(sizeof(NMTTDISPINFOA)==0x6C && sizeof(NMTTDISPINFOW)==0xBC);
static_assert(offsetof(MainFrameNativeBody90h,opaqueAt24)==0x24);
static_assert(offsetof(MainFrameNativeBody90h,updateAt3C)+offsetof(UpdateUiNativeBody18h,byteAt16)==0x52);
static_assert(offsetof(WindowClassModuleNativeBody38h,resourceInstanceAt08)==8);

// 注入叶可抛；三个业务叶无本地catch/rollback，不能收窄成noexcept。
class IMainFrameUiNativeApi
{
protected:
    ~IMainFrameUiNativeApi()=default;
public:
    virtual BOOL SetMenuDefaultItem(HMENU,UINT,UINT)=0;
    virtual BOOL GetMenuItemInfoA(HMENU,UINT,BOOL,MENUITEMINFOA*)=0;
    virtual BOOL SetMenuItemInfoA(HMENU,UINT,BOOL,const MENUITEMINFOA*)=0;
    virtual BOOL CheckMenuRadioItem(HMENU,UINT,UINT,UINT,UINT)=0;
    virtual ULONGLONG VerSetConditionMask(ULONGLONG,DWORD,BYTE)=0;
    virtual BOOL VerifyVersionInfoW(OSVERSIONINFOEXW*,DWORD,DWORDLONG)=0;
    virtual int LoadStringA(HINSTANCE,UINT,LPSTR,int)=0;
    virtual int LoadStringW(HINSTANCE,UINT,LPWSTR,int)=0;
    virtual LRESULT SendMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual HWND SetFocus(HWND)=0;
    virtual LONG GetWindowLongA(HWND,int)=0;
    virtual void PostQuitMessage(int)=0;
    virtual BOOL GetClientRect(HWND,RECT*)=0;
    virtual BOOL InvalidateRect(HWND,const RECT*,BOOL)=0;
    virtual BOOL GetWindowRect(HWND,RECT*)=0;
    virtual BOOL SetWindowPos(HWND,HWND,int,int,int,int,UINT)=0;
    virtual int StrncpyAt54B740(char*,std::size_t,const char*,std::size_t)=0;
    virtual int WcsncpyAt54B540(wchar_t*,std::size_t,const wchar_t*,std::size_t)=0;
    [[noreturn]] virtual void RangeCheckFailureAt540148()=0;
};
IMainFrameUiNativeApi& DefaultMainFrameUiNativeApi()noexcept;
struct MainFrameUiNativeServices final
{
    IMainFrameUiNativeApi& api;
    WindowClassModuleNativeBody38h& nativeBase;
};
// 默认来源为实际ProcessAtlBaseModuleNative；每次LoadString前晚读+08，不借instanceAt04。
MainFrameUiNativeServices DefaultMainFrameUiNativeServices()noexcept;
BYTE IsMainFrameOldWindowsNativeAt4137B0(IMainFrameUiNativeApi& = DefaultMainFrameUiNativeApi());
int CheckMainFrameUiErrnoNativeAt4136D0(int);

// 原ECX=U=F+3C，ret10；返回LRESULT，与四字节handled输出分离。
LRESULT UpdateMainFrameMenuNativeAt428430(UpdateUiNativeBody18h&,UINT,HMENU,LPARAM,BOOL*,
    MainFrameUiNativeServices = DefaultMainFrameUiNativeServices());
// 原ECX=F，ret18；未命中/mapId非零保留result，不调用default WNDPROC。
BOOL ProcessFrameBaseNativeAt42DC50(MainFrameNativeBody90h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    MainFrameUiNativeServices = DefaultMainFrameUiNativeServices());
// 原ECX=F，ret4；resizeBars非零才发bar WM_SIZE，最终client始终晚读。
void ResizeMainFrameUiNativeAt439100(MainFrameNativeBody90h&,BOOL,
    MainFrameUiNativeServices = DefaultMainFrameUiNativeServices());
}
