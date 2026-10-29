#pragma once
#include <Runtime/MainFrameUiNative.h>

namespace KaGuYa::Start::Runtime
{
// 42D230使用原50h REBARBANDINFOA，不能采用新版SDK的sizeof。
struct MainFrameChevronBandInfo50h final
{
    UINT cbSize;
    UINT fMask;
    DWORD opaqueAt08[6];
    HWND childAt20;
    DWORD opaqueAt24[11];
};
struct MainFrameChevronMenuInfo0Ch final
{
    HMENU menuAt00;
    NMREBARCHEVRON* notificationAt04;
    BYTE commandBarAt08;
    BYTE unwrittenAt09[3];
};
struct MainFrameChevronPopup18h final
{
    DWORD cbSize;
    HMENU menu;
    UINT flags;
    int x;
    int y;
    TPMPARAMS* parameters;
};
static_assert(sizeof(WPARAM)==4 && sizeof(LPARAM)==4 && sizeof(BOOL)==4);
static_assert(sizeof(MainFrameChevronBandInfo50h)==0x50 && offsetof(MainFrameChevronBandInfo50h,childAt20)==0x20);
static_assert(sizeof(MainFrameChevronMenuInfo0Ch)==0x0C && offsetof(MainFrameChevronMenuInfo0Ch,notificationAt04)==4);
static_assert(offsetof(MainFrameChevronMenuInfo0Ch,commandBarAt08)==8);
static_assert(sizeof(MainFrameChevronPopup18h)==0x18 && offsetof(MainFrameChevronPopup18h,parameters)==0x14);
static_assert(offsetof(NMREBARCHEVRON,uBand)==0x0C && offsetof(NMREBARCHEVRON,rc)==0x18);
static_assert(sizeof(NMREBARCHEVRON)==0x2C && sizeof(TBBUTTON)==0x14 && offsetof(TBBUTTON,fsState)==8);
static_assert(offsetof(TBBUTTON,fsStyle)==9 && sizeof(TBBUTTONINFOA)==0x20);
static_assert(offsetof(TBBUTTONINFOA,pszText)==0x18 && sizeof(TPMPARAMS)==0x14);
static_assert(sizeof(MSG)==0x1C && offsetof(MSG,pt)==0x14 && sizeof(MONITORINFO)==0x28);

// 只借原Win32叶和既有4137B0协议；可注入叶允许抛出，本链没有catch或展开清理资格。
class IMainFrameChevronNativeApi
{
protected:
    ~IMainFrameChevronNativeApi()=default;
public:
    virtual LRESULT SendMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual HMENU CreatePopupMenu()=0;
    virtual BOOL GetClientRect(HWND,RECT*)=0;
    virtual int GetMenuItemCount(HMENU)=0;
    virtual BOOL AppendMenuA(HMENU,UINT,UINT_PTR,LPCSTR)=0;
    virtual BYTE IsOldWindowsAt4137B0()=0;
    virtual BOOL GetMenuItemInfoA(HMENU,UINT,BOOL,MENUITEMINFOA*)=0;
    virtual int LoadStringA(HINSTANCE,UINT,LPSTR,int)=0;
    virtual BOOL DestroyMenu(HMENU)=0;
    virtual BOOL MessageBeep(UINT)=0;
    virtual int MapWindowPoints(HWND,HWND,POINT*,UINT)=0;
    virtual BOOL IsWindow(HWND)=0;
    virtual HMONITOR MonitorFromPoint(POINT,DWORD)=0;
    virtual BOOL GetMonitorInfoA(HMONITOR,MONITORINFO*)=0;
    virtual BOOL TrackPopupMenuEx(HMENU,UINT,int,int,HWND,TPMPARAMS*)=0;
    virtual BOOL RemoveMenu(HMENU,UINT,UINT)=0;
    virtual BOOL PeekMessageA(MSG*,HWND,UINT,UINT,UINT)=0;
    virtual BOOL PtInRect(const RECT*,POINT)=0;
};
IMainFrameChevronNativeApi& DefaultMainFrameChevronNativeApi()noexcept;
struct MainFrameChevronNativeServices final
{
    IMainFrameChevronNativeApi& api;
    WindowClassModuleNativeBody38h& nativeBase;
};
MainFrameChevronNativeServices DefaultMainFrameChevronNativeServices()noexcept;

// 原stdcall(context*)/ret4，仅AL有业务含义；services不进入原正文。
BYTE BuildMainFrameChevronMenuNativeAt42D230(MainFrameChevronMenuInfo0Ch*,
    MainFrameChevronNativeServices = DefaultMainFrameChevronNativeServices());
// 原cdecl按值POINT，caller清8，仅返回修正后的x。
int AdjustMainFrameChevronXNativeAt43A680(POINT,
    IMainFrameChevronNativeApi& = DefaultMainFrameChevronNativeApi());
// 原ECX=完整F/ret0C；成功保留handled，失败写DWORD 0。
LRESULT ChevronMainFrameNativeAt423550(MainFrameNativeBody90h&,WPARAM,NMREBARCHEVRON*,BOOL*,
    MainFrameChevronNativeServices = DefaultMainFrameChevronNativeServices());

// 由生产Popup集合委托此叶；428C10是独立业务链，不互相替代。
class MainFrameChevronNativeRuntime final
{
public:
    explicit MainFrameChevronNativeRuntime(MainFrameChevronNativeServices = DefaultMainFrameChevronNativeServices())noexcept;
    LRESULT ChevronAt423550(MainFrameNativeBody90h&,WPARAM,LPARAM,BOOL*);
private:
    MainFrameChevronNativeServices services_;
};
}
