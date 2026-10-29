#pragma once

#include <Runtime/DialogEndpointNative.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// Baku 416C30/416B00：错误框在一次 root 捕获内先读 caption，再读主窗口。
class ILegacyTitleNameRoot
{
public:
    virtual ~ILegacyTitleNameRoot() = default;
    virtual HWND MainWindow() = 0;
    virtual const RuntimeCString& Caption() = 0;
};

// 每次调用重新解析当前 root / GameSystem / SaveData；不缓存 flags 或整份设置。
// 不以原 +60/+64/+68 偏移解释现代 SaveData。所有 owner 端口必须实际装配。
class ILegacyTitleNameServices
{
public:
    virtual ~ILegacyTitleNameServices() = default;
    virtual bool EditableNameBit(unsigned bit) = 0;
    virtual bool SkipNamePrompt() = 0;
    virtual RuntimeCString& SavedGivenName() = 0;
    virtual RuntimeCString& SavedFamilyName() = 0;
    virtual const RuntimeCString& DefaultGivenName() = 0;
    virtual const RuntimeCString& DefaultFamilyName() = 0;
    virtual void WriteSkipNamePrompt(bool value) = 0;
    virtual ILegacyTitleNameRoot& CaptureRoot() = 0;
    virtual void WriteInputGate(bool value) = 0;
    virtual HINSTANCE ResourceInstance() = 0;
};

class ILegacyTitleNameApi
{
public:
    virtual ~ILegacyTitleNameApi() = default;
    virtual DialogEndpointNativeServices EndpointServices() = 0;
    virtual HWND ActiveWindow() = 0;
    virtual INT_PTR DialogBoxAt4BB950(HINSTANCE, LPCSTR, HWND, DLGPROC, LPARAM) = 0;
    virtual BOOL EnableIme(HWND, BOOL) = 0;
    virtual void LastError(DWORD) = 0;
    virtual HWND Control(HWND, int) = 0;
    virtual BOOL Enable(HWND, BOOL) = 0;
    virtual HWND Focus(HWND) = 0;
    virtual int TextLength(HWND) = 0;
    virtual int GetText(HWND, char*, int) = 0;
    virtual BOOL SetControlText(HWND, int, const char*) = 0;
    virtual LRESULT Send(HWND, UINT, WPARAM, LPARAM) = 0;
    virtual BOOL Beep(UINT) = 0;
    virtual int ShowMessage(HWND, const char*, const char*, UINT) = 0;
    virtual BOOL End(HWND, INT_PTR) = 0;
    // 408953 使用 movsx；保留当前 CRT MB 代码页，不固定 Unicode 字符数或 CP932 表。
    virtual int IsMbLead(unsigned int) = 0;
};
ILegacyTitleNameApi& DefaultLegacyTitleNameApi() noexcept;

// 语义对象，不复制原 CNameDlg 的 2Ch 布局。唯一窗口绑定使用现有真实 endpoint。
class LegacyTitleNameDialog final
{
public:
    LegacyTitleNameDialog(ILegacyTitleNameServices&, ILegacyTitleNameApi&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
    ~LegacyTitleNameDialog() noexcept(false);
    LegacyTitleNameDialog(const LegacyTitleNameDialog&) = delete;
    LegacyTitleNameDialog& operator=(const LegacyTitleNameDialog&) = delete;

    // 4BB950：ActiveWindow -> thunk(0,null) -> pending record -> module -> ID112，lParam=0。
    // 普通 -1 原样返回；不移除未消费的 pending record，不重试创建。
    INT_PTR RunModal();
    BOOL ProcessMessage(HWND, UINT, WPARAM, LPARAM, LRESULT&, DWORD mapId = 0);
    // 49ED60 的已证输入域；不提供带未证 void* / type / validate 参数的通用 DDX。
    BOOL UpdateData(bool save, int controlId = -1);
    HWND Window() const noexcept;
    const RuntimeCString& GivenName() const noexcept { return given_.value; }
    const RuntimeCString& FamilyName() const noexcept { return family_.value; }
    // 仅 WM_INITDIALOG 已执行后可观察此值，原49E920不初始化 checkbox。
    bool SkipValue() const noexcept { return skip_; }

private:
    struct Binding final
    {
        DialogEndpointNativeBody20h endpoint;
        LegacyTitleNameDialog* owner;
    } binding_;
    union StringSlot
    {
        StringSlot() noexcept {}
        ~StringSlot() noexcept {}
        RuntimeCString value;
    } given_, family_;
    ILegacyTitleNameServices& source_;
    ILegacyTitleNameApi& api_;
    IRuntimeStringManagerSource& strings_;
    DialogEndpointNativeServices endpoint_;
    bool skip_;
    bool alive_ = false;

    void Release();
    BOOL Initialize();
    void Accept();
    void Defaults();
    void EditChanged(int id);
    BOOL ExchangeText(int id, RuntimeCString&, bool save);
    void RequiredName(bool given);
    static BOOL __fastcall Process(DialogEndpointNativeBody20h*, void*, HWND,
        UINT, WPARAM, LPARAM, LRESULT*, DWORD);
    static DialogEndpointNativeBody20h* __fastcall Delete(DialogEndpointNativeBody20h*, void*, DWORD);
    static DLGPROC __fastcall Procedure(DialogEndpointNativeBody20h*, void*);
    static void __fastcall Final(DialogEndpointNativeBody20h*, void*, HWND);
    static const DialogEndpointNativeVtable4 table_;
};
}
