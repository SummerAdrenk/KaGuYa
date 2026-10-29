#pragma once

#include <Runtime/LegacyDisplaySettings.h>
#include <Windows.h>
#include <string>

namespace KaGuYa::Start::Runtime
{
class ILegacyDisplayModeApi
{
public:
    virtual ~ILegacyDisplayModeApi() = default;
    virtual BOOL Enumerate(const char* deviceName, DWORD index, DEVMODEA&) = 0;
    virtual LONG Change(const char* deviceName, DEVMODEA&, DWORD flags) = 0;
};
class Win32LegacyDisplayModeApi final : public ILegacyDisplayModeApi
{
public:
    BOOL Enumerate(const char*, DWORD, DEVMODEA&) override;
    LONG Change(const char*, DEVMODEA&, DWORD) override;
};
LegacyDisplayMode DescribeLegacyDisplayMode(const DEVMODEA&) noexcept;
DEVMODEA ReadLegacyDisplayMode(ILegacyDisplayModeApi&, const std::string&, DWORD index = ENUM_CURRENT_SETTINGS);
// 01:4687A0、02:46D3C0、03:46CBA0；仅调用注入 API，纯数据测试不得选 Win32 实现。
std::vector<DEVMODEA> EnumerateLegacyDisplayModes(ILegacyDisplayModeApi&, const std::string&,
    std::uint32_t bitsPerPixel, std::uint32_t logicalWidth, std::uint32_t logicalHeight);
bool ApplyLegacyDisplayMode(ILegacyDisplayModeApi&, const std::string&, DEVMODEA);

class WindowModeView;
class IGraphicsWindowModeApi;
class IWindowModePlatformApi;
// 共用现代 WindowMode 的视口和 Graphics；不改 SetModeAt438AD0 或 18 的行为。
// 01:417953：进入全屏附加SWP_FRAMECHANGED，预览与缩放仍传0。
void RefreshLegacyDisplayViewport(WindowModeView, IGraphicsWindowModeApi&, IWindowModePlatformApi&,
    HWND, const DEVMODEA& current, int logicalWidth, int logicalHeight, int scaling, UINT additionalFlags = 0);
}
