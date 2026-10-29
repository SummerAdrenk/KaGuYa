#pragma once
#include <Runtime/PropertyPageNative.h>
#include <Runtime/GraphicsRootNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 40B860：共同54h之后没有Ctor默认值；union禁止Ref的DMI提前清写。
struct PropertyPageTextNativeBody68h final
{
    union { PropertyPageNativeBody54h pageAt00; };
    BYTE shadowAt54,antialiasAt55,eraseVoiceAt56,skipUnreadAt57;
    BYTE continueSkipAt58,skipVoiceAt59,skipEffectAt5A,unwrittenAt5B;
    DWORD colorIndexAt5C;
    COLORREF colorsAt60[2];
    PropertyPageTextNativeBody68h()noexcept{}
};
struct PropertyPageImageStaticNativeBody48h final
{
    union { EditWindowEndpointNativeBody24h endpointAt00; };
    union { YtlReferenceSlotRuntime backgroundAt24; };
    union { YtlReferenceSlotRuntime colorSurfaceAt30; };
    union { YtlReferenceSlotRuntime dibAt3C; };
    PropertyPageImageStaticNativeBody48h()noexcept{}
};
struct PropertyPageMessWinNativeBody9Ch final
{
    union { PropertyPageNativeBody54h pageAt00; };
    union { PropertyPageImageStaticNativeBody48h imageAt54; };
    PropertyPageMessWinNativeBody9Ch()noexcept{}
};
static_assert(sizeof(PropertyPageTextNativeBody68h)==0x68);
static_assert(offsetof(PropertyPageTextNativeBody68h,shadowAt54)==0x54);
static_assert(offsetof(PropertyPageTextNativeBody68h,colorIndexAt5C)==0x5C);
static_assert(offsetof(PropertyPageTextNativeBody68h,colorsAt60)==0x60);
static_assert(sizeof(PropertyPageImageStaticNativeBody48h)==0x48);
static_assert(offsetof(PropertyPageImageStaticNativeBody48h,backgroundAt24)==0x24);
static_assert(offsetof(PropertyPageImageStaticNativeBody48h,colorSurfaceAt30)==0x30);
static_assert(offsetof(PropertyPageImageStaticNativeBody48h,dibAt3C)==0x3C);
static_assert(sizeof(PropertyPageMessWinNativeBody9Ch)==0x9C);
static_assert(offsetof(PropertyPageMessWinNativeBody9Ch,imageAt54)==0x54);
static_assert(std::is_standard_layout_v<PropertyPageTextNativeBody68h>);
static_assert(std::is_trivially_destructible_v<PropertyPageMessWinNativeBody9Ch>);

// 只暴露两种实际singleton查询；每站点重新捕获，不用游戏内Setting源或快照。
class IPropertyPageTextNativeSource
{
public:
    virtual ~IPropertyPageTextNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0()=0;
    virtual GraphicsRootNativeBody1Ch* CaptureGraphicsAt4182F0()=0;
};
class RootPropertyPageTextNativeSource final:public IPropertyPageTextNativeSource
{
public:
    RootPropertyPageTextNativeSource(EnvironmentRootOwner&,GraphicsRootOwner&)noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0()override;
    GraphicsRootNativeBody1Ch* CaptureGraphicsAt4182F0()override;
private:
    EnvironmentRootOwner& environment_;
    GraphicsRootOwner& graphics_;
};

// Win32薄叶：默认值是真API；测试仅替换这些外部叶，不创建窗口或执行原PE。
struct PropertyPageTextNativeWin32Api final
{
    decltype(&::GetDlgItem) getDlgItem=&::GetDlgItem;
    decltype(&::SendMessageA) sendMessage=&::SendMessageA;
    decltype(&::GetParent) getParent=&::GetParent;
    decltype(&::GetWindow) getWindow=&::GetWindow;
    decltype(&::GetWindowLongA) getWindowLong=&::GetWindowLongA;
    decltype(&::SetWindowLongA) setWindowLong=&::SetWindowLongA;
    decltype(&::CheckDlgButton) checkDlgButton=&::CheckDlgButton;
    decltype(&::GetWindowTextLengthA) getWindowTextLength=&::GetWindowTextLengthA;
    decltype(&::GetWindowTextA) getWindowText=&::GetWindowTextA;
    decltype(&::SetWindowTextA) setWindowText=&::SetWindowTextA;
    decltype(&::GetWindowRect) getWindowRect=&::GetWindowRect;
    decltype(&::GetClientRect) getClientRect=&::GetClientRect;
    decltype(&::ScreenToClient) screenToClient=&::ScreenToClient;
    decltype(&::InvalidateRect) invalidateRect=&::InvalidateRect;
    decltype(&::GetDC) getDC=&::GetDC;
    decltype(&::DeleteDC) deleteDC=&::DeleteDC;
    decltype(&::CreatePen) createPen=&::CreatePen;
    decltype(&::CreateSolidBrush) createSolidBrush=&::CreateSolidBrush;
    decltype(&::SelectObject) selectObject=&::SelectObject;
    decltype(&::DeleteObject) deleteObject=&::DeleteObject;
    decltype(&::Rectangle) rectangle=&::Rectangle;
    decltype(&::BeginPaint) beginPaint=&::BeginPaint;
    decltype(&::EndPaint) endPaint=&::EndPaint;
    decltype(&::CopyRect) copyRect=&::CopyRect;
    decltype(&::IsRectEmpty) isRectEmpty=&::IsRectEmpty;
    decltype(&::BitBlt) bitBlt=&::BitBlt;
    decltype(&::FindResourceA) findResource=&::FindResourceA;
    decltype(&::LoadResource) loadResource=&::LoadResource;
    decltype(&::SizeofResource) sizeofResource=&::SizeofResource;
    decltype(&::LockResource) lockResource=&::LockResource;
    decltype(&::FreeResource) freeResource=&::FreeResource;
};
struct PropertyPageTextNativeServices final
{
    IPropertyPageTextNativeSource& source;
    PropertyPageTextNativeWin32Api& win32;
    DialogEndpointNativeServices& endpoints;
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
    IGraphicsModuleNativeWin32Api& exports;
    // 原hModule只供CC资源；resourceInstance另由共同页Ctor取得，不混为同一全局。
    HMODULE bitmapResourceModule;
    // 原6197EC由整个33Ch Ctor清零；单页Ctor绝不重置该共享一次性标志。
    DWORD& sheetButtonsAt6197EC;
    const HMODULE* nativeBitmapResourceModuleSlot = nullptr;
};
class PropertyPageTextNativeScope final
{
public:
    explicit PropertyPageTextNativeScope(PropertyPageTextNativeServices&)noexcept;
    ~PropertyPageTextNativeScope()noexcept;
    PropertyPageTextNativeScope(const PropertyPageTextNativeScope&)=delete;
    PropertyPageTextNativeScope& operator=(const PropertyPageTextNativeScope&)=delete;
private:
    PropertyPageTextNativeServices* previous_;
    DialogEndpointNativeScope endpoints_;
    GraphicsSurfaceNativeScope surfaces_;
};
PropertyPageTextNativeServices& RequirePropertyPageTextNativeServices();
const DialogEndpointNativeVtable4& PropertyPageTextNativeMethods()noexcept;
const DialogEndpointNativeVtable4& PropertyPageMessWinNativeMethods()noexcept;
const EditWindowEndpointNativeVtable4& PropertyPageImageStaticNativeMethods()noexcept;
void InitializePropertyPageTextNativeAt40B91C(PropertyPageTextNativeBody68h&,WindowClassModuleNativeBody38h&)noexcept;
void InitializePropertyPageMessWinNativeAt40B9C0(PropertyPageMessWinNativeBody9Ch&,WindowClassModuleNativeBody38h&)noexcept;
void DestroyPropertyPageTextNativeAt411F10(PropertyPageTextNativeBody68h&,PropertyPageTextNativeServices&);
void DestroyPropertyPageMessWinNativeAt411D60(PropertyPageMessWinNativeBody9Ch&,PropertyPageTextNativeServices&);
void DestroyPropertyPageImageStaticNativeAt40E990(PropertyPageImageStaticNativeBody48h&,PropertyPageTextNativeServices&)noexcept;
BOOL ProcessPropertyPageTextNativeAt430850(PropertyPageTextNativeBody68h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,PropertyPageTextNativeServices&);
BOOL ProcessPropertyPageMessWinNativeAt42FB20(PropertyPageMessWinNativeBody9Ch&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,PropertyPageTextNativeServices&);
BOOL ProcessPropertyPageImageStaticNativeAt42E700(PropertyPageImageStaticNativeBody48h&,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD,PropertyPageTextNativeServices&);
BOOL ExchangePropertyPageTextNativeAt415FF0(PropertyPageTextNativeBody68h&,BOOL,DWORD,PropertyPageTextNativeServices&);
BOOL ApplyPropertyPageTextNativeAt422D60(PropertyPageTextNativeBody68h&,PropertyPageTextNativeServices&);
BOOL ApplyPropertyPageMessWinNativeAt4212A0(PropertyPageMessWinNativeBody9Ch&,PropertyPageTextNativeServices&);
void InitializePropertyPageImageStaticNativeAt414660(PropertyPageImageStaticNativeBody48h&,PropertyPageTextNativeServices&);
void UpdatePropertyPageImageStaticNativeAt438D60(PropertyPageImageStaticNativeBody48h&,DWORD,PropertyPageTextNativeServices&);
// 原资源仅静态字节；宿主决定如何安装66/68，不能宣称header ID已在工程中接通。
std::span<const BYTE> PropertyPageTextNativeTemplate66()noexcept;
std::span<const BYTE> PropertyPageMessWinNativeTemplate68()noexcept;
}
