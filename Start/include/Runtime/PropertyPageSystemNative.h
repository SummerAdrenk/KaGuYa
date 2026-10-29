#pragma once
#include <Runtime/PropertyPageNative.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;
class ParamsRuntimeRootOwner;
struct EnvironmentRootStorage;

// 40B860内嵌页：复用唯一54h共同存储，不包含游戏内GUI、服务指针或配置镜像。
struct PropertyPageSaveLoadNativeBody5Ch final
{
    union { PropertyPageNativeBody54h pageAt00; };
    BYTE thumbnailAt54, autoSaveAt55, quickSaveConfirmAt56, quickLoadConfirmAt57, historyAt58;
    BYTE unwrittenAt59[3];
    PropertyPageSaveLoadNativeBody5Ch() noexcept {}
};
struct PropertyPageSystem1NativeBody60h final
{
    union { PropertyPageNativeBody54h pageAt00; };
    BYTE skipStartupAt54, stopAnimationAt55, disableEffectAt56, stopSelectionAnimationAt57;
    union { RuntimeCString rendererTextAt58; };
    DWORD rightClickModeAt5C;
    PropertyPageSystem1NativeBody60h() noexcept {}
    ~PropertyPageSystem1NativeBody60h() noexcept {} // 仅原非deleting入口减持，C++析构不重复释放union。
};
struct PropertyPageSystem2NativeBody68h final
{
    union { PropertyPageNativeBody54h pageAt00; };
    DWORD firstChoiceAt54, secondChoiceAt58;
    union { RuntimeCString givenNameAt5C; };
    union { RuntimeCString familyNameAt60; };
    BYTE skipNameDialogAt64, unwrittenAt65[3];
    PropertyPageSystem2NativeBody68h() noexcept {}
    ~PropertyPageSystem2NativeBody68h() noexcept {} // 保留原阶段资格，包括第二GetNil失败时不自动回滚。
};
static_assert(sizeof(PropertyPageSaveLoadNativeBody5Ch) == 0x5C);
static_assert(sizeof(PropertyPageSystem1NativeBody60h) == 0x60);
static_assert(sizeof(PropertyPageSystem2NativeBody68h) == 0x68);
static_assert(offsetof(PropertyPageSaveLoadNativeBody5Ch, thumbnailAt54) == 0x54);
static_assert(offsetof(PropertyPageSaveLoadNativeBody5Ch, historyAt58) == 0x58);
static_assert(offsetof(PropertyPageSystem1NativeBody60h, rendererTextAt58) == 0x58);
static_assert(offsetof(PropertyPageSystem1NativeBody60h, rightClickModeAt5C) == 0x5C);
static_assert(offsetof(PropertyPageSystem2NativeBody68h, firstChoiceAt54) == 0x54);
static_assert(offsetof(PropertyPageSystem2NativeBody68h, givenNameAt5C) == 0x5C);
static_assert(offsetof(PropertyPageSystem2NativeBody68h, familyNameAt60) == 0x60);
static_assert(offsetof(PropertyPageSystem2NativeBody68h, skipNameDialogAt64) == 0x64);

// 仅提供原捕获/借用叶；inline注册槽解析由caller保留原读序，417990另有自己的读序。
class IPropertyPageSystemNativeSource
{
public:
    virtual ~IPropertyPageSystemNativeSource() = default;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0() = 0;
    virtual void* RegistryAt417990(EnvironmentRootStorage*, DWORD index) = 0;
    virtual void* ParamsAt418480() = 0;
};
class RootPropertyPageSystemNativeSource final : public IPropertyPageSystemNativeSource
{
public:
    RootPropertyPageSystemNativeSource(EnvironmentRootOwner&, ParamsRuntimeRootOwner&) noexcept;
    EnvironmentRootStorage* EnvironmentAt4180C0() override;
    void* RegistryAt417990(EnvironmentRootStorage*, DWORD) override;
    void* ParamsAt418480() override;
private:
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
};

class IPropertyPageSystemNativeApi
{
public:
    virtual ~IPropertyPageSystemNativeApi() = default;
    virtual HWND GetParent(HWND) = 0;
    virtual HWND GetDlgItem(HWND, int) = 0;
    virtual LRESULT SendMessageA(HWND, UINT, WPARAM, LPARAM) = 0;
    virtual HWND GetWindow(HWND, UINT) = 0;
    virtual LONG GetWindowLongA(HWND, int) = 0;
    virtual LONG SetWindowLongA(HWND, int, LONG) = 0;
    virtual BOOL SetWindowTextA(HWND, LPCSTR) = 0;
    virtual BOOL SetDlgItemTextA(HWND, int, LPCSTR) = 0;
    virtual BOOL EnableWindow(HWND, BOOL) = 0;
    virtual int MessageBoxA(HWND, LPCSTR, LPCSTR, UINT) = 0;
    virtual BOOL MessageBeep(UINT) = 0;
    virtual HWND SetFocus(HWND) = 0;
    virtual int GetWindowTextLengthA(HWND) = 0;
    virtual int GetWindowTextA(HWND, LPSTR, int) = 0;
    virtual int GetObjectA(HGDIOBJ, int, void*) = 0;
    virtual HFONT CreateFontIndirectA(const LOGFONTA*) = 0;
    virtual COLORREF SetTextColor(HDC, COLORREF) = 0;
    virtual int SetBkMode(HDC, int) = 0;
    virtual HGDIOBJ GetStockObject(int) = 0;
    // 424CB3的movsx必须保留；使用当前CRT多字节表，不猜固定CP932范围。
    virtual int IsMbLead(int signedByte) = 0;
};
IPropertyPageSystemNativeApi& DefaultPropertyPageSystemNativeApi() noexcept;
struct PropertyPageSystemNativeServices final
{
    IPropertyPageSystemNativeSource& source;
    IPropertyPageSystemNativeApi& api;
    DialogEndpointNativeServices& endpoint;
    IRuntimeStringManagerSource& strings;
    // 主线40B860在六页构造后清零的唯一6197EC槽；不在每页/每服务内另存副本。
    DWORD& sheetButtonsAt6197EC;
};
class PropertyPageSystemNativeScope final
{
public:
    explicit PropertyPageSystemNativeScope(PropertyPageSystemNativeServices&) noexcept;
    ~PropertyPageSystemNativeScope() noexcept;
    PropertyPageSystemNativeScope(const PropertyPageSystemNativeScope&) = delete;
    PropertyPageSystemNativeScope& operator=(const PropertyPageSystemNativeScope&) = delete;
private:
    PropertyPageSystemNativeServices* previous_;
    DialogEndpointNativeScope endpoint_;
};
PropertyPageSystemNativeServices& RequirePropertyPageSystemNativeServices();

const DialogEndpointNativeVtable4& PropertyPageSaveLoadNativeVtable() noexcept;
const DialogEndpointNativeVtable4& PropertyPageSystem1NativeVtable() noexcept;
const DialogEndpointNativeVtable4& PropertyPageSystem2NativeVtable() noexcept;
void InitializePropertyPageSaveLoadNativeAt40BBE6(PropertyPageSaveLoadNativeBody5Ch&, WindowClassModuleNativeBody38h&) noexcept;
void InitializePropertyPageSystem1NativeAt40BC89(PropertyPageSystem1NativeBody60h&, WindowClassModuleNativeBody38h&,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
// 第二GetNil失败时不清第一串；40B860此时仍state5，尚未取得System2析构资格。
void InitializePropertyPageSystem2NativeAt40BD0F(PropertyPageSystem2NativeBody68h&, WindowClassModuleNativeBody38h&,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
void DestroyPropertyPageSaveLoadNativeAt40EB00(PropertyPageSaveLoadNativeBody5Ch&, DialogEndpointNativeServices&);
void DestroyPropertyPageSystem1NativeAt40EB90(PropertyPageSystem1NativeBody60h&, DialogEndpointNativeServices&);
void DestroyPropertyPageSystem2NativeAt40EBF0(PropertyPageSystem2NativeBody68h&, DialogEndpointNativeServices&);

INT_PTR CALLBACK BoundPropertyPageSystemNativeAt415710(HWND, UINT, WPARAM, LPARAM);
BOOL ProcessPropertyPageSaveLoadNativeAt42FDE0(PropertyPageSaveLoadNativeBody5Ch&, HWND, UINT, WPARAM, LPARAM, LRESULT*, DWORD, PropertyPageSystemNativeServices&);
BOOL ProcessPropertyPageSystem1NativeAt430490(PropertyPageSystem1NativeBody60h&, HWND, UINT, WPARAM, LPARAM, LRESULT*, DWORD, PropertyPageSystemNativeServices&);
BOOL ProcessPropertyPageSystem2NativeAt4306D0(PropertyPageSystem2NativeBody68h&, HWND, UINT, WPARAM, LPARAM, LRESULT*, DWORD, PropertyPageSystemNativeServices&);
LRESULT InitializePropertyPageSaveLoadNativeAt4253C0(PropertyPageSaveLoadNativeBody5Ch&, PropertyPageSystemNativeServices&);
BOOL ApplyPropertyPageSaveLoadNativeAt421450(PropertyPageSaveLoadNativeBody5Ch&, PropertyPageSystemNativeServices&);
LRESULT InitializePropertyPageSystem1NativeAt4274E0(PropertyPageSystem1NativeBody60h&, PropertyPageSystemNativeServices&);
BOOL DdxPropertyPageSystem1NativeAt415D80(PropertyPageSystem1NativeBody60h&, BOOL save, DWORD control, PropertyPageSystemNativeServices&);
BOOL ApplyPropertyPageSystem1NativeAt422940(PropertyPageSystem1NativeBody60h&, PropertyPageSystemNativeServices&);
LRESULT InitializePropertyPageSystem2NativeAt427A40(PropertyPageSystem2NativeBody68h&, PropertyPageSystemNativeServices&);
BOOL ApplyPropertyPageSystem2NativeAt422B90(PropertyPageSystem2NativeBody68h&, PropertyPageSystemNativeServices&);
LRESULT DefaultNamesPropertyPageSystem2NativeAt423E20(PropertyPageSystem2NativeBody68h&, PropertyPageSystemNativeServices&);
LRESULT LimitNamePropertyPageSystem2NativeAt424BC0(PropertyPageSystem2NativeBody68h&, WORD control, PropertyPageSystemNativeServices&);
}
