#include <Runtime/LegacyDisplaySettingsPlatform.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <algorithm>

namespace KaGuYa::Start::Runtime
{
BOOL Win32LegacyDisplayModeApi::Enumerate(const char* name, DWORD index, DEVMODEA& mode)
{
    return ::EnumDisplaySettingsA(name, index, &mode);
}
LONG Win32LegacyDisplayModeApi::Change(const char* name, DEVMODEA& mode, DWORD flags)
{
    return ::ChangeDisplaySettingsExA(name, &mode, nullptr, flags, nullptr);
}
void RefreshLegacyDisplayViewport(WindowModeView mode, IGraphicsWindowModeApi& graphics,
    IWindowModePlatformApi& platform, HWND window, const DEVMODEA& current,
    int logicalWidth, int logicalHeight, int scaling, UINT additionalFlags)
{
    // 01:4177C0/4177F0，03:419D10/419D40：窗口态不改变视口。
    if (mode.ModeAt04() == 0) return;
    const auto* viewport = graphics.ResizeScreenIfLoaded(current.dmPelsWidth, current.dmPelsHeight, scaling);
    auto& body = mode.NativeBody();
    if (viewport) std::copy_n(viewport, 4, body.viewportAt0C.begin());
    else body.viewportAt0C = {0, 0, logicalWidth, logicalHeight};
    static_cast<void>(platform.PositionWindow(window, nullptr, current.dmPosition.x, current.dmPosition.y,
        current.dmPelsWidth, current.dmPelsHeight, SWP_SHOWWINDOW | additionalFlags));
    // 现代位置同步所需的运行态，不落盘；避免随后按旧监视器矩形重置视口。
    body.savedMonitorRectAt1C = {current.dmPosition.x, current.dmPosition.y,
        current.dmPosition.x + static_cast<LONG>(current.dmPelsWidth),
        current.dmPosition.y + static_cast<LONG>(current.dmPelsHeight)};
    graphics.ResetRendererIfLoaded();
}
}
