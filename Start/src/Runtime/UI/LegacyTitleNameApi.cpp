#include <Runtime/LegacyTitleNameDialog.h>
#include <mbctype.h>

extern "C" __declspec(dllimport) BOOL WINAPI WINNLSEnableIME(HWND, BOOL);

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32NameApi final : public ILegacyTitleNameApi
{
public:
    DialogEndpointNativeServices EndpointServices() override { return DefaultDialogEndpointNativeServices(); }
    HWND ActiveWindow() override { return ::GetActiveWindow(); }
    INT_PTR DialogBoxAt4BB950(HINSTANCE module, LPCSTR resource, HWND parent, DLGPROC proc, LPARAM param) override
    { return ::DialogBoxParamA(module, resource, parent, proc, param); }
    BOOL EnableIme(HWND window, BOOL enabled) override { return ::WINNLSEnableIME(window, enabled); }
    void LastError(DWORD error) override { ::SetLastError(error); }
    HWND Control(HWND window, int id) override { return ::GetDlgItem(window, id); }
    BOOL Enable(HWND window, BOOL enabled) override { return ::EnableWindow(window, enabled); }
    HWND Focus(HWND window) override { return ::SetFocus(window); }
    int TextLength(HWND window) override { return ::GetWindowTextLengthA(window); }
    int GetText(HWND window, char* buffer, int capacity) override { return ::GetWindowTextA(window, buffer, capacity); }
    BOOL SetControlText(HWND window, int id, const char* text) override { return ::SetDlgItemTextA(window, id, text); }
    LRESULT Send(HWND window, UINT message, WPARAM w, LPARAM l) override { return ::SendMessageA(window, message, w, l); }
    BOOL Beep(UINT type) override { return ::MessageBeep(type); }
    int ShowMessage(HWND window, const char* text, const char* caption, UINT type) override
    { return ::MessageBoxA(window, text, caption, type); }
    BOOL End(HWND window, INT_PTR result) override { return ::EndDialog(window, result); }
    int IsMbLead(unsigned int value) override { return ::_ismbblead(value); }
};
}
ILegacyTitleNameApi& DefaultLegacyTitleNameApi() noexcept { static Win32NameApi api; return api; }
}
