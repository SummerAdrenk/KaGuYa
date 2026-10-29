#pragma once
#include <Runtime/DialogEndpointNative.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class INativeStartupSettingsDialogServices;
class INativeStartupSettingsDialogApi;
class IStartFileApi;
struct StartupSettingsDialogNativeBody28h final
{
    union { DialogEndpointNativeBody20h dialogAt00; };
    BYTE dontShowAt20, fullscreenAt21, unwrittenAt22[2];
    std::int32_t renderAt24;
    StartupSettingsDialogNativeBody28h() noexcept {}
};
static_assert(sizeof(StartupSettingsDialogNativeBody28h) == 0x28);
static_assert(offsetof(StartupSettingsDialogNativeBody28h, dontShowAt20) == 0x20);
static_assert(offsetof(StartupSettingsDialogNativeBody28h, renderAt24) == 0x24);
struct StartupSettingsDialogNativeServices final
{
    INativeStartupSettingsDialogServices& source;
    IStartFileApi& files;
    INativeStartupSettingsDialogApi& api;
    DialogEndpointNativeServices& endpoint;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    // 仅现代只读Values外观的发布标记，非业务值，不进原28h；原生caller可显式不用。
    BYTE* writtenFields = nullptr; // bit0=20, bit1=21, bit2=24
};
class StartupSettingsDialogNativeScope final
{
public:
    explicit StartupSettingsDialogNativeScope(StartupSettingsDialogNativeServices&) noexcept;
    ~StartupSettingsDialogNativeScope() noexcept;
    StartupSettingsDialogNativeScope(const StartupSettingsDialogNativeScope&) = delete;
    StartupSettingsDialogNativeScope& operator=(const StartupSettingsDialogNativeScope&) = delete;
private:
    StartupSettingsDialogNativeServices* previous_;
    DialogEndpointNativeScope endpoint_;
};
StartupSettingsDialogNativeServices& RequireStartupSettingsDialogNativeServices();
const DialogEndpointNativeVtable4& StartupSettingsDialogNativeVtable() noexcept;
void InitializeStartupSettingsDialogNativeAt489D15(StartupSettingsDialogNativeBody28h&) noexcept;
INT_PTR RunStartupSettingsDialogNativeAt489DDF(StartupSettingsDialogNativeBody28h&, HINSTANCE,
    StartupSettingsDialogNativeServices&);
INT_PTR RunStartupSettingsDialogNativeWithResourceSlotAt489DDF(StartupSettingsDialogNativeBody28h&,
    const HINSTANCE&, StartupSettingsDialogNativeServices&);
void ReleaseStartupSettingsDialogNativeAt489D5F(StartupSettingsDialogNativeBody28h&,
    StartupSettingsDialogNativeServices&);
LRESULT InitializeStartupSettingsDialogNativeAt4837E0(StartupSettingsDialogNativeBody28h&,
    StartupSettingsDialogNativeServices&);
BOOL UpdateStartupSettingsDialogNativeAt476750(StartupSettingsDialogNativeBody28h&, BOOL save, int control,
    StartupSettingsDialogNativeServices&);
BOOL ProcessStartupSettingsDialogNativeAt485A50(StartupSettingsDialogNativeBody28h&, HWND, UINT,
    WPARAM, LPARAM, LRESULT*, DWORD, StartupSettingsDialogNativeServices&);
}
