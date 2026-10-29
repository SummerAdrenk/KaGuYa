#include <Runtime/LegacyDisplaySettingsWin32.h>
#include <CommCtrl.h>
#include <array>
#include <exception>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::wstring Resolution(LegacyDisplayMode mode)
{
    return std::to_wstring(mode.width) + L" X " + std::to_wstring(mode.height) + L" \u30d4\u30af\u30bb\u30eb";
}
std::wstring Frequency(LegacyDisplayMode mode)
{
    return std::to_wstring(mode.frequency) + L" \u30d8\u30eb\u30c4";
}
void CountdownText(HWND window, unsigned seconds)
{
    const auto text = std::to_wstring(seconds) + L" \u79d2\u3067\u5143\u306e\u8a2d\u5b9a\u306b\u623b\u308a\u307e\u3059\u3002";
    ::SetDlgItemTextW(window, 1602, text.c_str());
}
void Center(HWND window, HWND parent)
{
    RECT bounds{}, rectangle{};
    if (!::GetWindowRect(parent, &bounds) || !::GetWindowRect(window, &rectangle))
        throw std::runtime_error("Cannot center legacy display dialog");
    ::SetWindowPos(window, nullptr, bounds.left + ((bounds.right - bounds.left) - (rectangle.right - rectangle.left)) / 2,
        bounds.top + ((bounds.bottom - bounds.top) - (rectangle.bottom - rectangle.top)) / 2,
        0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
struct Confirmation final
{
    Confirmation(LegacyDisplayMode value, HWND owner) noexcept : mode(value), parent(owner) {}
    LegacyDisplayMode mode;
    HWND parent;
    LegacyDisplayConfirmationCountdown countdown;
    UINT_PTR timer = 0;
    HFONT bold = nullptr;
    ~Confirmation() { if (bold) ::DeleteObject(bold); }
    static INT_PTR CALLBACK Procedure(HWND window, UINT message, WPARAM w, LPARAM l) noexcept
    {
        auto* self = reinterpret_cast<Confirmation*>(::GetWindowLongPtrW(window, DWLP_USER));
        try
        {
            if (message == WM_INITDIALOG)
            {
                self = reinterpret_cast<Confirmation*>(l);
                ::SetWindowLongPtrW(window, DWLP_USER, l);
                ::SetDlgItemTextW(window, 1600, Resolution(self->mode).c_str());
                ::SetDlgItemTextW(window, 1601, Frequency(self->mode).c_str());
                CountdownText(window, self->countdown.Seconds());
                self->timer = ::SetTimer(window, 1, 1000, nullptr);
                // 原 SetTimer 失败会失去自动回退；安全域不得继续无超时确认。
                if (!self->timer) { ::EndDialog(window, -1); return TRUE; }
                LOGFONTW font{};
                const auto original = reinterpret_cast<HFONT>(::SendMessageW(window, WM_GETFONT, 0, 0));
                if (::GetObjectW(original, sizeof(font), &font) == sizeof(font))
                {
                    font.lfWeight = FW_BOLD;
                    self->bold = ::CreateFontIndirectW(&font);
                    if (self->bold)
                        for (int id : {1600, 1601}) ::SendDlgItemMessageW(window, id, WM_SETFONT,
                            reinterpret_cast<WPARAM>(self->bold), TRUE);
                }
                Center(window, self->parent);
                return TRUE;
            }
            if (!self) return FALSE;
            if (message == WM_TIMER && w == self->timer)
            {
                const bool expired = self->countdown.Tick();
                CountdownText(window, self->countdown.Seconds());
                if (expired)
                {
                    ::KillTimer(window, self->timer);
                    self->timer = 0;
                    ::PostMessageW(window, WM_COMMAND, IDCANCEL, 0);
                }
                return TRUE;
            }
            if (message == WM_COMMAND && (LOWORD(w) == IDOK || LOWORD(w) == IDCANCEL))
            { ::EndDialog(window, LOWORD(w)); return TRUE; }
            if (message == WM_CLOSE) { ::EndDialog(window, IDCANCEL); return TRUE; }
            if (message == WM_DESTROY && self->timer)
            { ::KillTimer(window, self->timer); self->timer = 0; }
        }
        catch (...) { ::EndDialog(window, -1); }
        return FALSE;
    }
};
}
struct Win32LegacyDisplaySettingsUi::Impl final
{
    Impl(const HINSTANCE& module, const HWND& owner) noexcept : resources(module), parent(owner) {}
    const HINSTANCE& resources;
    const HWND& parent;
    HWND dialog = nullptr;
    LegacyDisplaySettingsRuntime* runtime = nullptr;
    std::exception_ptr failure;

    static INT_PTR CALLBACK Procedure(HWND window, UINT message, WPARAM w, LPARAM l) noexcept
    {
        auto* self = reinterpret_cast<Impl*>(::GetWindowLongPtrW(window, DWLP_USER));
        if (message == WM_INITDIALOG)
        {
            self = reinterpret_cast<Impl*>(l);
            ::SetWindowLongPtrW(window, DWLP_USER, l);
            self->dialog = window;
            return TRUE;
        }
        if (!self || !self->runtime || self->failure) return FALSE;
        try
        {
            if (message == WM_HSCROLL && reinterpret_cast<HWND>(l) == ::GetDlgItem(window, 1500))
            {
                self->runtime->SelectResolution(static_cast<std::size_t>(::SendDlgItemMessageW(window, 1500, TBM_GETPOS, 0, 0)));
                return TRUE;
            }
            if (message == WM_CLOSE) { self->runtime->RequestClose(false); return TRUE; }
            if (message != WM_COMMAND) return FALSE;
            const WORD id = LOWORD(w);
            if (id == IDOK || id == IDCANCEL) self->runtime->RequestClose(id == IDOK);
            else if (id >= 1300 && id <= 1302) self->runtime->RequestScaling();
            else if (id == 1100) self->runtime->RequestPreview();
            else if (id == 1900 && HIWORD(w) == CBN_SELCHANGE)
                self->runtime->SelectRate(static_cast<std::size_t>(::SendDlgItemMessageW(window, 1900, CB_GETCURSEL, 0, 0)));
            else return FALSE;
            return TRUE;
        }
        catch (...) { self->failure = std::current_exception(); return TRUE; }
    }
};
Win32LegacyDisplaySettingsUi::Win32LegacyDisplaySettingsUi(const HINSTANCE& resources, const HWND& parent)
    : impl_(std::make_unique<Impl>(resources, parent)) {}
Win32LegacyDisplaySettingsUi::~Win32LegacyDisplaySettingsUi() { Close(); }
void Win32LegacyDisplaySettingsUi::Open(LegacyDisplaySettingsRuntime& runtime)
{
    if (impl_->dialog) throw std::logic_error("Legacy display dialog already exists");
    impl_->runtime = &runtime;
    impl_->failure = nullptr;
    const INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_BAR_CLASSES};
    if (!::InitCommonControlsEx(&controls)) throw std::runtime_error("Cannot initialize legacy display trackbar");
    impl_->dialog = ::CreateDialogParamW(impl_->resources, MAKEINTRESOURCEW(210), impl_->parent,
        &Impl::Procedure, reinterpret_cast<LPARAM>(impl_.get()));
    if (!impl_->dialog) throw std::runtime_error("Cannot create legacy display dialog 210");
    Center(impl_->dialog, impl_->parent);
}
void Win32LegacyDisplaySettingsUi::Show(bool visible) { ::ShowWindow(impl_->dialog, visible ? SW_SHOW : SW_HIDE); }
void Win32LegacyDisplaySettingsUi::Close() noexcept
{
    if (impl_->dialog) ::DestroyWindow(impl_->dialog);
    impl_->dialog = nullptr;
    impl_->runtime = nullptr;
}
void Win32LegacyDisplaySettingsUi::Synchronize(const LegacyDisplaySettingsModel& model)
{
    const HWND window = impl_->dialog;
    ::SendDlgItemMessageW(window, 1500, TBM_SETRANGEMIN, FALSE, 0);
    ::SendDlgItemMessageW(window, 1500, TBM_SETRANGEMAX, FALSE, model.Resolutions().size() - 1);
    ::SendDlgItemMessageW(window, 1500, TBM_SETPOS, TRUE, model.SelectedResolution());
    const auto& resolution = model.Resolutions().at(model.SelectedResolution());
    ::SetDlgItemTextW(window, 1601, Resolution(model.Modes().at(resolution.modeIndices.front())).c_str());
    ::SendDlgItemMessageW(window, 1900, CB_RESETCONTENT, 0, 0);
    for (auto index : resolution.modeIndices)
    {
        const auto text = Frequency(model.Modes().at(index));
        const auto added = ::SendDlgItemMessageW(window, 1900, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
        if (added == CB_ERR || added == CB_ERRSPACE) throw std::runtime_error("Cannot populate legacy refresh rates");
    }
    ::SendDlgItemMessageW(window, 1900, CB_SETCURSEL, resolution.selectedRate, 0);
    const auto label = L"[ " + std::to_wstring(model.displayed.width) + L" X " +
        std::to_wstring(model.displayed.height) + L", " + Frequency(model.displayed) + L" ]";
    ::SetDlgItemTextW(window, 1600, label.c_str());
    ::CheckRadioButton(window, 1300, 1302, model.scaling <= 2 ? 1300 + model.scaling : 0);
}
std::uint32_t Win32LegacyDisplaySettingsUi::ReadScaling()
{
    std::array<unsigned, 3> checks;
    for (std::uint32_t i = 0; i != 3; ++i)
        checks[i] = ::IsDlgButtonChecked(impl_->dialog, 1300 + i);
    // 01:45336D，02:455550，03:40F4D0；无勾选读回 -1，不能保留旧值或猜 0。
    return ReadLegacyDisplayRadio(checks);
}
LegacyDisplayConfirmResult Win32LegacyDisplaySettingsUi::Confirm(LegacyDisplayMode mode)
{
    Confirmation state{mode, impl_->parent};
    const auto result = ::DialogBoxParamW(impl_->resources, MAKEINTRESOURCEW(211), impl_->parent,
        &Confirmation::Procedure, reinterpret_cast<LPARAM>(&state));
    if (result == IDOK) return LegacyDisplayConfirmResult::Keep;
    if (result == IDCANCEL) return LegacyDisplayConfirmResult::Revert;
    return LegacyDisplayConfirmResult::Failed;
}
void Win32LegacyDisplaySettingsUi::RecenterAndRedraw()
{
    Center(impl_->dialog, impl_->parent);
    Redraw();
}
void Win32LegacyDisplaySettingsUi::Redraw()
{
    ::RedrawWindow(impl_->dialog, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}
void Win32LegacyDisplaySettingsUi::ReportPreviewRestoreFailure()
{
    ::MessageBoxW(impl_->parent, L"\u5143\u306e\u753b\u9762\u8a2d\u5b9a\u306b\u623b\u305b\u307e\u305b\u3093\u3067\u3057\u305f\u3002",
        nullptr, MB_ICONERROR);
}
void Win32LegacyDisplaySettingsUi::RethrowPendingFailure()
{
    if (impl_->failure) std::rethrow_exception(impl_->failure);
}
bool Win32LegacyDisplaySettingsUi::PreTranslate(MSG& message)
{
    return impl_->dialog && ::IsDialogMessageW(impl_->dialog, &message);
}
}
