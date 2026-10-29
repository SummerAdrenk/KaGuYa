#pragma once
#include <Runtime/EditWindowNative.h>

namespace KaGuYa::Start::Runtime
{
// 只组织原Win32叶，不保存窗口或另一份正文状态。
class IEditWindowNativeMapApi : public IEditWindowNativeMessageApi,public IEditWindowNativeTextApi
{
public:
    virtual BOOL GetWindowRect(HWND,RECT*)=0;
    virtual BOOL GetClientRect(HWND,RECT*)=0;
    virtual BOOL EqualRect(const RECT*,const RECT*)=0;
    virtual BOOL MoveWindow(HWND,int,int,int,int,BOOL)=0;
    virtual LRESULT SendMessageA(HWND,UINT,WPARAM,LPARAM)=0;
    virtual int GetObjectA(HGDIOBJ,int,void*)=0;
    virtual BOOL DeleteObject(HGDIOBJ)=0;
    virtual HFONT CreateFontIndirectA(const LOGFONTA*)=0;
    virtual HGDIOBJ SelectObject(HDC,HGDIOBJ)=0;
    virtual BOOL PatBlt(HDC,int,int,int,int,DWORD)=0;
    virtual COLORREF SetTextColor(HDC,COLORREF)=0;
    virtual int SetBkMode(HDC,int)=0;
    virtual HMENU GetMenu(HWND)=0;
    virtual BOOL SetWindowTextA(HWND,const char*)=0;
    virtual BOOL DestroyWindow(HWND)=0;
    virtual int IsMbLead(std::uint32_t)=0;
};
IEditWindowNativeMapApi& DefaultEditWindowNativeMapApi() noexcept;
// WM_CREATE仍强制提供真实创建链，未实现不得默返回NULL或建立modern窗口替身。
class IEditControlNativeCreationSource
{
public:
    virtual ~IEditControlNativeCreationSource()=default;
    virtual void CreateControlAt485628(EditControlNativeBody28h&,HWND,DWORD)=0;
};
struct EditWindowNativeMapServices final
{
    IEditWindowNativeMapApi& api;
    IEditControlNativeCreationSource& creation;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
// 48CF30只写有证据的字段及face字符串，不清face尾部未写字节。
void FillEditControlNativeFontAt48CF30(LOGFONTA&,std::int32_t height);
BOOL ProcessEditControlNativeAt485480(EditControlNativeBody28h&,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    IEditWindowNativeMapApi& = DefaultEditWindowNativeMapApi());
BOOL ProcessEditWindowNativeAt4855D0(EditWindowNativeBody98h&,UINT,WPARAM,LPARAM,LRESULT*,DWORD,
    EditWindowNativeMapServices&);
// 原第四个BOOL*参数不读不写；返回值恒0，但通知可修改文本及选择位置。
LRESULT ApplyEditWindowNativeByteLimitAt4824A0(EditWindowNativeBody98h&,UINT,WPARAM,LPARAM,BOOL*,
    IEditWindowNativeMapApi& = DefaultEditWindowNativeMapApi(),
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
void ReplaceEditWindowNativeMarginsAt469F70(YtlReferenceSlotRuntime&);
void ApplyEditWindowNativeLayoutAt4880F0(EditWindowNativeBody98h&,const RECT*,const RECT*,
    IEditWindowNativeMapApi& = DefaultEditWindowNativeMapApi());
}
