#include <Runtime/LegacyDisplaySettings.h>
#include <algorithm>
#include <bit>
#include <stdexcept>
#include <tuple>

namespace KaGuYa::Start::Runtime
{
bool SameLegacyDisplaySizeAndRate(const LegacyDisplayMode& a, const LegacyDisplayMode& b) noexcept
{
    return a.width == b.width && a.height == b.height && a.frequency == b.frequency;
}
std::uint32_t ReadLegacyDisplayRadio(std::span<const unsigned> checks) noexcept
{
    std::uint32_t result = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < checks.size(); ++i)
        if (checks[i]) result = static_cast<std::uint32_t>(i);
    return result;
}

LegacyDisplaySettingsModel::LegacyDisplaySettingsModel(std::span<const LegacyDisplayMode> modes,
    LegacyDisplayMode current, std::uint32_t scale)
    : displayed(current), scaling(scale), modes_(modes.begin(), modes.end())
{
    // 原空表/当前模式不在表内会访问无效项；明确拒绝，不选择猜测的模式。
    if (modes_.empty()) throw std::runtime_error("Legacy display mode catalogue is empty");
    for (std::size_t i = 0; i < modes_.size(); ++i)
    {
        const auto& mode = modes_[i];
        auto at = std::find_if(resolutions_.begin(), resolutions_.end(), [&](const auto& r)
            { return r.width == mode.width && r.height == mode.height; });
        if (at == resolutions_.end())
        {
            resolutions_.push_back({mode.width, mode.height, {}, 0});
            at = resolutions_.end() - 1;
        }
        at->modeIndices.push_back(i);
    }
    // ASM 使用 jge/jle/setl，排序是有符号 DWORD；尺寸筛选则是无符号比较。
    const auto signedValue = [](auto value) { return std::bit_cast<std::int32_t>(value); };
    std::sort(resolutions_.begin(), resolutions_.end(), [&](const auto& a, const auto& b)
        { return std::tuple(signedValue(a.width), signedValue(a.height)) <
            std::tuple(signedValue(b.width), signedValue(b.height)); });
    bool found = false;
    for (std::size_t r = 0; r < resolutions_.size(); ++r)
    {
        auto& resolution = resolutions_[r];
        std::sort(resolution.modeIndices.begin(), resolution.modeIndices.end(), [&](auto a, auto b)
            { return signedValue(modes_[a].frequency) < signedValue(modes_[b].frequency); });
        resolution.selectedRate = resolution.modeIndices.size() - 1;
        for (std::size_t f = 0; f < resolution.modeIndices.size(); ++f)
        {
            const auto index = resolution.modeIndices[f];
            if (!found && SameLegacyDisplaySizeAndRate(modes_[index], current))
            {
                resolution.selectedRate = f;
                selectedResolution_ = r;
                displayedModeIndex = index;
                found = true;
            }
        }
    }
    if (!found) throw std::runtime_error("Current legacy display mode is not in the catalogue");
}
void LegacyDisplaySettingsModel::SelectResolution(std::size_t index)
{
    static_cast<void>(resolutions_.at(index));
    selectedResolution_ = index;
}
void LegacyDisplaySettingsModel::SelectRate(std::size_t index)
{
    auto& resolution = resolutions_.at(selectedResolution_);
    static_cast<void>(resolution.modeIndices.at(index));
    resolution.selectedRate = index;
}
std::size_t LegacyDisplaySettingsModel::SelectedModeIndex() const
{
    const auto& resolution = resolutions_.at(selectedResolution_);
    return resolution.modeIndices.at(resolution.selectedRate);
}
std::optional<std::size_t> LegacyDisplaySettingsModel::FindMode(LegacyDisplayMode wanted, bool depth) const
{
    for (std::size_t i = 0; i < modes_.size(); ++i)
        if (SameLegacyDisplaySizeAndRate(modes_[i], wanted) && (!depth || modes_[i].bitsPerPixel == wanted.bitsPerPixel))
            return i;
    return std::nullopt;
}
bool LegacyDisplayConfirmationCountdown::Tick() noexcept
{
    if (seconds_) --seconds_;
    return seconds_ == 0;
}
}
