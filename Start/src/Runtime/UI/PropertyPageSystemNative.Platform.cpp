#include "PropertyPageSystemNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <mbctype.h>

namespace KaGuYa::Start::Runtime
{
RootPropertyPageSystemNativeSource::RootPropertyPageSystemNativeSource(EnvironmentRootOwner& e, ParamsRuntimeRootOwner& p) noexcept
    : environment_(e), params_(p) {}
EnvironmentRootStorage* RootPropertyPageSystemNativeSource::EnvironmentAt4180C0()
{ return environment_.CaptureAt4180C0(); }
void* RootPropertyPageSystemNativeSource::RegistryAt417990(EnvironmentRootStorage* e, DWORD i)
{ return ResolveEnvironmentArchiveAt417990(*e, i); }
void* RootPropertyPageSystemNativeSource::ParamsAt418480() { return params_.CaptureAt418480(); }
namespace
{
class Api final : public IPropertyPageSystemNativeApi
{
public:
    HWND GetParent(HWND w) override { return ::GetParent(w); }
    HWND GetDlgItem(HWND w, int i) override { return ::GetDlgItem(w, i); }
    LRESULT SendMessageA(HWND w, UINT m, WPARAM p, LPARAM l) override { return ::SendMessageA(w, m, p, l); }
    HWND GetWindow(HWND w, UINT c) override { return ::GetWindow(w, c); }
    LONG GetWindowLongA(HWND w, int i) override { return ::GetWindowLongA(w, i); }
    LONG SetWindowLongA(HWND w, int i, LONG l) override { return ::SetWindowLongA(w, i, l); }
    BOOL SetWindowTextA(HWND w, LPCSTR t) override { return ::SetWindowTextA(w, t); }
    BOOL SetDlgItemTextA(HWND w, int i, LPCSTR t) override { return ::SetDlgItemTextA(w, i, t); }
    BOOL EnableWindow(HWND w, BOOL e) override { return ::EnableWindow(w, e); }
    int MessageBoxA(HWND w, LPCSTR t, LPCSTR c, UINT f) override { return ::MessageBoxA(w, t, c, f); }
    BOOL MessageBeep(UINT f) override { return ::MessageBeep(f); }
    HWND SetFocus(HWND w) override { return ::SetFocus(w); }
    int GetWindowTextLengthA(HWND w) override { return ::GetWindowTextLengthA(w); }
    int GetWindowTextA(HWND w, LPSTR t, int n) override { return ::GetWindowTextA(w, t, n); }
    int GetObjectA(HGDIOBJ h, int n, void* p) override { return ::GetObjectA(h, n, p); }
    HFONT CreateFontIndirectA(const LOGFONTA* f) override { return ::CreateFontIndirectA(f); }
    COLORREF SetTextColor(HDC d, COLORREF c) override { return ::SetTextColor(d, c); }
    int SetBkMode(HDC d, int m) override { return ::SetBkMode(d, m); }
    HGDIOBJ GetStockObject(int i) override { return ::GetStockObject(i); }
    int IsMbLead(int c) override { return ::_ismbblead(static_cast<unsigned int>(c)); }
};
}
IPropertyPageSystemNativeApi& DefaultPropertyPageSystemNativeApi() noexcept { static Api api; return api; }
}
