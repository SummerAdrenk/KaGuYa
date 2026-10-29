#include <Runtime/LegacyDisplaySettingsPlatform.h>
#include <algorithm>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 只选择像素尺寸、色深、刷新率；不是写注册表，不含设备位置。
constexpr DWORD fields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;
static_assert(fields == 0x5C0000);
DEVMODEA EmptyMode() noexcept
{
    DEVMODEA mode{};
    mode.dmSize = sizeof(mode);
    return mode;
}
const char* Name(const std::string& name) noexcept { return name.empty() ? nullptr : name.c_str(); }
}
LegacyDisplayMode DescribeLegacyDisplayMode(const DEVMODEA& mode) noexcept
{
    return {mode.dmPelsWidth, mode.dmPelsHeight, mode.dmBitsPerPel, mode.dmDisplayFrequency};
}
DEVMODEA ReadLegacyDisplayMode(ILegacyDisplayModeApi& api, const std::string& device, DWORD index)
{
    auto current = EmptyMode();
    if (!api.Enumerate(Name(device), index, current))
        throw std::runtime_error("Cannot query legacy display mode");
    return current;
}
std::vector<DEVMODEA> EnumerateLegacyDisplayModes(ILegacyDisplayModeApi& api, const std::string& device,
    std::uint32_t depth, std::uint32_t width, std::uint32_t height)
{
    auto eligible = [&](const DEVMODEA& m)
        { return m.dmBitsPerPel == depth && m.dmPelsWidth >= width && m.dmPelsHeight >= height; };
    std::vector<DEVMODEA> result;
    for (DWORD index = 0;; ++index)
    {
        auto mode = EmptyMode();
        if (!api.Enumerate(Name(device), index, mode)) break;
        if (!eligible(mode)) continue;
        mode.dmFields = fields;
        if (api.Change(Name(device), mode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL) result.push_back(mode);
    }
    auto current = EmptyMode();
    if (api.Enumerate(Name(device), ENUM_CURRENT_SETTINGS, current) && eligible(current) &&
        std::none_of(result.begin(), result.end(), [&](const auto& mode)
            { return SameLegacyDisplaySizeAndRate(DescribeLegacyDisplayMode(mode), DescribeLegacyDisplayMode(current)); }))
        result.push_back(current);
    return result;
}
bool ApplyLegacyDisplayMode(ILegacyDisplayModeApi& api, const std::string& device, DEVMODEA mode)
{
    mode.dmFields = fields;
    return api.Change(Name(device), mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}
}
