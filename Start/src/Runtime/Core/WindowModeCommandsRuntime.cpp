#include <Runtime/WindowModeCommandsRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/WindowEnvironmentRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
BOOL Win32WindowModeCommandPlatformApi::PostWindowMessage(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessageA(window, message, wParam, lParam);
}

int Win32WindowModeCommandPlatformApi::ShowMessage(
    HWND window, const char* text, const char* caption, UINT flags)
{
    return ::MessageBoxA(window, text, caption, flags);
}

WindowModeCommandsRuntime::WindowModeCommandsRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, IWindowModeCommandPlatformApi& platform) noexcept
    : environment_(&environment), root_(&root), platform_(platform)
{
}

WindowModeCommandsRuntime::WindowModeCommandsRuntime(WindowEnvironmentRuntime& environment,
    const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
    const std::shared_ptr<SaveDataEnvironmentArchive>& saveSlot,
    IWindowModeCommandPlatformApi& platform) noexcept
    : modernEnvironment_(&environment), modeSlot_(&modeSlot), saveSlot_(&saveSlot), platform_(platform)
{
}

WindowModeView WindowModeCommandsRuntime::CurrentMode() const
{
    if (root_)
    {
        // 先完成413BE0再重读发布根；按值仅固定本次receiver，不增加Ref计数。
        root_->EnsureCreatedAt413BE0();
        return root_->RequirePublished().WindowModeAt0C();
    }
    // 原空槽会非法访问；现代表示明确拒绝缺失依赖，不制造默认模式或静默跳过。
    if (!*modeSlot_) throw std::logic_error("Window mode owning slot is empty.");
    return (*modeSlot_)->Borrow();
}

SaveDataEnvironmentArchive& WindowModeCommandsRuntime::CurrentSave() const
{
    if (environment_) return environment_->Archives().SaveData();
    if (!*saveSlot_) throw std::logic_error("SaveData owning slot is empty.");
    return **saveSlot_;
}

HWND WindowModeCommandsRuntime::CurrentWindow() const
{
    if (environment_)
        return reinterpret_cast<HWND>(environment_->CaptureAt4180C0().windowAt2C);
    return modernEnvironment_->window;
}

bool WindowModeCommandsRuntime::Dispatch(unsigned int messageMapId, UINT message,
    WPARAM wParam, LPARAM, LRESULT& result)
{
    if (messageMapId != 0 || message != WM_COMMAND) return false;
    const WORD command = LOWORD(wParam);
    if (command == 0x800B)
    {
        // 42F4D3/+28先于42F4E0；SetMode内部重入不改变已捕获style和receiver。
        const LONG style = std::bit_cast<LONG>(environment_ ?
            environment_->CaptureAt4180C0().fullscreenStyleAt28 : modernEnvironment_->fullscreenStyle);
        auto receiver = CurrentMode();
        if (receiver.SetModeAt438AD0(1, style, 0) == 0)
        {
            // 原PE 5AE8CC/5AE8DC的CP932字节；Show返回值不影响后续偏好回写。
            static_cast<void>(platform_.ShowMessage(CurrentWindow(),
                "\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93"
                "\x89\xE6\x96\xCA\x82\xC9\x82\xC5\x82\xAB\x82\xDC\x82\xB9"
                "\x82\xF1\x82\xC5\x82\xB5\x82\xBD\x81\x42",
                "\x89\xE6\x96\xCA\x95\xCF\x8D\x58\x82\xCC\x8E\xB8\x94\x73", MB_ICONERROR));
        }
    }
    else if (command == 0x800C)
    {
        // 42F5DF/+24、42F5E7/+20是两次独立4180C0；42F5F4随后固定receiver。
        const LONG exStyle = std::bit_cast<LONG>(environment_ ?
            environment_->CaptureAt4180C0().exStyleAt24 : modernEnvironment_->executableProfile.configuredExStyle);
        const LONG style = std::bit_cast<LONG>(environment_ ?
            environment_->CaptureAt4180C0().styleAt20 : modernEnvironment_->executableProfile.configuredStyle);
        auto receiver = CurrentMode();
        static_cast<void>(receiver.SetModeAt438AD0(0, style, exStyle));
    }
    else return false;

    // 42F539/42F62E在SetMode及MessageBox返回后重新解根；42F564先读mode，42F567才取Save。
    const auto mode = CurrentMode().ModeAt04();
    CurrentSave().bytesAt04[1] = mode == 0 ? 1 : 0;
    result = 0;
    return true;
}

void WindowModeCommandsRuntime::PostInitialWindowModeCommandAt48AE19()
{
    // 48AE19先解当前Save；只有+05为零才在48AE2D重新取Environment+2C，不触碰mode/root。
    if (CurrentSave().bytesAt04[1] == 0)
        static_cast<void>(platform_.PostWindowMessage(CurrentWindow(), WM_COMMAND, 0x800B, 0));
}
}
