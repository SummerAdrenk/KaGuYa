#include <Runtime/LegacyDisplayDeviceOwner.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
int Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
int Sub(int a, int b) noexcept { return Signed(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
int Add(int a, int b) noexcept { return Signed(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
}
void LegacyDisplayDeviceOwner::RefreshViewport(WindowModeView view, IGraphicsWindowModeApi& graphics,
    IWindowModePlatformApi& platform, HWND window, std::uint32_t scaling, UINT flags)
{
    if (!view.ModeAt04()) return;
    const auto context = CaptureDisplayContext();
    const auto current = ReadLegacyDisplayMode(api_, context.deviceName);
    RefreshLegacyDisplayViewport(view, graphics, platform, window, current,
        Signed(width_), Signed(height_), Signed(scaling), flags);
}
LegacyDisplayOwnershipResult LegacyDisplayDeviceOwner::SetWindowMode(WindowModeView view,
    IGraphicsWindowModeApi& graphics, IWindowModePlatformApi& platform, HWND window,
    bool fullscreen, LONG style, LONG exStyle)
{
    RequireInitialized();
    if ((view.ModeAt04() != 0) == fullscreen) return LegacyDisplayOwnershipResult::NoChange;
    if (fullscreen)
    {
        MONITORINFOEXA info{}; info.cbSize = sizeof(info);
        std::optional<std::string_view> name;
        if (devices_.size() != 1)
        {
            const auto monitor = platform.FindWindowMonitor(window, MONITOR_DEFAULTTONULL);
            if (monitor && platform.ReadMonitorInfo(monitor, info)) name = info.szDevice;
        }
        SelectForFullscreen(name);
        const auto& selected = devices_.at(selected_).name;
        SetRuntimeCStringAt437680(view.NativeBody().deviceNameAt08, selected.c_str(), static_cast<int>(selected.size()));
        const auto result = SetExclusive(true);
        // 原417C00在申请失败后仍翻转byte并可能以active=-1索引；这是明确安全差异。
        // 失败不伪造成功全屏，保留结果；不得另选设备/模式兜底。
        if (result != LegacyDisplayOwnershipResult::Acquired) return result;
        view.NativeBody().modeAt04 = 1;
        static_cast<void>(platform.WriteWindowLong(window, GWL_STYLE, style));
        static_cast<void>(platform.WriteWindowLong(window, GWL_EXSTYLE, exStyle));
        RefreshViewport(view, graphics, platform, window, store_.ReadScaling(), SWP_FRAMECHANGED);
        return result;
    }
    const auto result = SetExclusive(false);
    // 恢复失败仍保留active责任；原离开流程继续设置窗口样式，主线必须报告result。
    view.NativeBody().modeAt04 = 0;
    static_cast<void>(platform.WriteWindowLong(window, GWL_STYLE, style));
    static_cast<void>(platform.WriteWindowLong(window, GWL_EXSTYLE, exStyle));
    const auto desktop = ReadSelectedRegistryModeOrZero();
    const auto logicalWidth = Signed(width_), logicalHeight = Signed(height_);
    static_cast<void>(graphics.ResizeScreenIfLoaded(logicalWidth, logicalHeight, -1));
    RECT adjusted{0, 0, logicalWidth, logicalHeight};
    static_cast<void>(platform.AdjustFrameRect(adjusted, static_cast<DWORD>(style), FALSE, static_cast<DWORD>(exStyle)));
    const auto width = Sub(adjusted.right, adjusted.left), height = Sub(adjusted.bottom, adjusted.top);
    const auto desktopWidth = Signed(desktop.dmPelsWidth), desktopHeight = Signed(desktop.dmPelsHeight);
    const auto x = desktopWidth <= logicalWidth ? desktop.dmPosition.x :
        Add(Sub(desktopWidth, width) / 2, desktop.dmPosition.x);
    const auto y = desktopHeight <= logicalHeight ? desktop.dmPosition.y :
        Add(Sub(desktopHeight, height) / 2, desktop.dmPosition.y);
    static_cast<void>(platform.PositionWindow(window, HWND_NOTOPMOST, x, y, width, height,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW));
    static_cast<void>(platform.InvalidateWindowRect(nullptr, nullptr, FALSE));
    graphics.ResetRendererIfLoaded();
    view.NativeBody().savedMonitorRectAt1C = {};
    return result;
}
}
