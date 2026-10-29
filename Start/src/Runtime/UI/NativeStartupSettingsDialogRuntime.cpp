#include <Runtime/NativeStartupSettingsDialogRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32StartupSettingsDialogApi final : public INativeStartupSettingsDialogApi
{
public:
    HWND ActiveWindow() override { return ::GetActiveWindow(); }
    DialogEndpointNativeServices EndpointServices() override { return DefaultDialogEndpointNativeServices(); }
    INT_PTR DialogBoxAt489DDF(HINSTANCE instance, LPCSTR resource, HWND parent, DLGPROC procedure, LPARAM parameter) override
    { return ::DialogBoxParamA(instance, resource, parent, procedure, parameter); }
    LONG SetWindowLongValue(HWND window, int index, LONG value) override { return ::SetWindowLongA(window, index, value); }
    HWND Control(HWND dialog, int id) override { return ::GetDlgItem(dialog, id); }
    LRESULT Send(HWND window, UINT message, WPARAM w, LPARAM l) override { return ::SendMessageA(window, message, w, l); }
    HWND NextWindow(HWND window, UINT command) override { return ::GetWindow(window, command); }
    LONG WindowStyle(HWND window) override { return ::GetWindowLongA(window, GWL_STYLE); }
    BOOL Show(HWND window, int command) override { return ::ShowWindow(window, command); }
    BOOL End(HWND dialog, INT_PTR result) override { return ::EndDialog(dialog, result); }
    int FontDescription(HGDIOBJ font, int size, void* output) override { return ::GetObjectA(font, size, output); }
    HFONT MakeFont(const LOGFONTA& description) override { return ::CreateFontIndirectA(&description); }
    BOOL ValidWindow(HWND window) override { return ::IsWindow(window); }
    COLORREF TextColor(HDC dc, COLORREF color) override { return ::SetTextColor(dc, color); }
    int BackgroundMode(HDC dc, int mode) override { return ::SetBkMode(dc, mode); }
    HGDIOBJ StockObject(int index) override { return ::GetStockObject(index); }
};
INativeStartupSettingsDialogApi& DefaultApi() noexcept { static Win32StartupSettingsDialogApi api; return api; }
}
struct NativeStartupSettingsDialogRuntime::State final
{
    StartupSettingsDialogNativeBody28h body;
    HINSTANCE instance;
    DialogEndpointNativeServices endpoint;
    BYTE written = 0;
    StartupSettingsDialogNativeServices services;
    mutable NativeStartupSettingsDialogValues view;
    State(HINSTANCE module, INativeStartupSettingsDialogServices& source, IStartFileApi& files,
        INativeStartupSettingsDialogApi& api)
        : instance(module), endpoint(api.EndpointServices()), services{source, files, api, endpoint,
            DefaultRuntimeStringManagerSource(), &written}
    { InitializeStartupSettingsDialogNativeAt489D15(body); }
    ~State() noexcept { ReleaseStartupSettingsDialogNativeAt489D5F(body, services); }
    const NativeStartupSettingsDialogValues& Values() const noexcept
    {
        // 仅兼容现代观察接口，不为原控件提供默认值或另一份可写设置。
        view.dontShowAgainAt20 = written & 1u ? std::optional<bool>(body.dontShowAt20 != 0) : std::nullopt;
        view.fullscreenAt21 = written & 2u ? std::optional<bool>(body.fullscreenAt21 != 0) : std::nullopt;
        view.renderSelectionAt24 = written & 4u ? std::optional<std::int32_t>(body.renderAt24) : std::nullopt;
        return view;
    }
};
NativeStartupSettingsDialogRuntime::NativeStartupSettingsDialogRuntime(HINSTANCE instance,
    INativeStartupSettingsDialogServices& services, IStartFileApi& files)
    : NativeStartupSettingsDialogRuntime(instance, services, files, DefaultApi()) {}
NativeStartupSettingsDialogRuntime::NativeStartupSettingsDialogRuntime(HINSTANCE instance,
    INativeStartupSettingsDialogServices& services, IStartFileApi& files, INativeStartupSettingsDialogApi& api)
    : state_(std::make_unique<State>(instance, services, files, api)) {}
NativeStartupSettingsDialogRuntime::~NativeStartupSettingsDialogRuntime() noexcept = default;
INT_PTR NativeStartupSettingsDialogRuntime::RunModalAt489DDF()
{ return RunStartupSettingsDialogNativeAt489DDF(state_->body, state_->instance, state_->services); }
HWND NativeStartupSettingsDialogRuntime::Window() const noexcept { return state_->body.dialogAt00.windowAt04; }
const NativeStartupSettingsDialogValues& NativeStartupSettingsDialogRuntime::Values() const noexcept { return state_->Values(); }
BOOL NativeStartupSettingsDialogRuntime::UpdateDataAt476750(bool save, int control)
{
    StartupSettingsDialogNativeScope scope(state_->services);
    return UpdateStartupSettingsDialogNativeAt476750(state_->body, save ? TRUE : FALSE, control, state_->services);
}
BOOL NativeStartupSettingsDialogRuntime::ProcessWindowMessageAt485A50(HWND window, UINT message,
    WPARAM w, LPARAM l, LRESULT& result, DWORD map)
{
    StartupSettingsDialogNativeScope scope(state_->services);
    return ProcessStartupSettingsDialogNativeAt485A50(state_->body, window, message, w, l, &result, map, state_->services);
}
INT_PTR CALLBACK NativeStartupSettingsDialogRuntime::InitialDialogProcedureAt4382D0(HWND window, UINT message, WPARAM w, LPARAM l)
{ return InitialDialogEndpointNativeAt4382D0(window, message, w, l); }
void ConfirmNativeStartupSettingsAt489C90(HINSTANCE instance, INativeStartupSettingsDialogServices& services, IStartFileApi& files)
{ ConfirmNativeStartupSettingsAt489C90(instance, services, files, DefaultApi()); }
void ConfirmNativeStartupSettingsAt489C90(HINSTANCE instance, INativeStartupSettingsDialogServices& source,
    IStartFileApi& files, INativeStartupSettingsDialogApi& api)
{ ConfirmNativeStartupSettingsWithResourceSlotAt489C90(instance, source, files, api); }
void ConfirmNativeStartupSettingsWithResourceSlotAt489C90(const HINSTANCE& instance,
    INativeStartupSettingsDialogServices& source, IStartFileApi& files)
{ ConfirmNativeStartupSettingsWithResourceSlotAt489C90(instance, source, files, DefaultApi()); }
void ConfirmNativeStartupSettingsWithResourceSlotAt489C90(const HINSTANCE& instance,
    INativeStartupSettingsDialogServices& source, IStartFileApi& files, INativeStartupSettingsDialogApi& api)
{
    if (source.SaveDataAtSlot1().bytesAt04[0] != 0) return;
    auto endpoint = api.EndpointServices();
    StartupSettingsDialogNativeServices services{source, files, api, endpoint};
    StartupSettingsDialogNativeBody28h body;
    InitializeStartupSettingsDialogNativeAt489D15(body);
    struct Unwind final
    {
        StartupSettingsDialogNativeBody28h& body;
        StartupSettingsDialogNativeServices& services;
        bool active = true;
        ~Unwind() noexcept { if (active) ReleaseStartupSettingsDialogNativeAt489D5F(body, services); }
    } unwind{body, services};
    if (RunStartupSettingsDialogNativeWithResourceSlotAt489DDF(body, instance, services) == 2) throw 0;
    unwind.active = false;
    // 489D62先撤栈对象资格，正常Free失败可向489C90外层catch传播，不变成析构terminate。
    ReleaseStartupSettingsDialogNativeAt489D5F(body, services);
}
}
