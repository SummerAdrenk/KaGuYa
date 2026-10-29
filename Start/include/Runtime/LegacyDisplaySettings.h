#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 仅用于显式接线；Disabled 不接管 18 号或其它游戏的窗口/全屏命令。
enum class LegacyDisplaySettingsFamily { Disabled, Baku, Namaiki, ChuPet };

// 语义值，不是新的文件 wire；落盘使用已有 SaveDataRecordVector。
struct LegacyDisplayMode final
{
    std::uint32_t width = 0, height = 0, bitsPerPixel = 0, frequency = 0;
    bool operator==(const LegacyDisplayMode&) const = default;
};
bool SameLegacyDisplaySizeAndRate(const LegacyDisplayMode&, const LegacyDisplayMode&) noexcept;
// 原 DDX_Radio 读回先置 -1，再采用最后一个非零选中项。
std::uint32_t ReadLegacyDisplayRadio(std::span<const unsigned> checks) noexcept;

struct LegacyDisplayResolution final
{
    std::uint32_t width, height;
    std::vector<std::size_t> modeIndices;
    std::size_t selectedRate = 0;
};

// 01:409730/409D60/409D90，02:40AA10，03:40A8B0。
// 保留枚举下标，按宽/高和刷新率排序；重复枚举项不能静默去重。
class LegacyDisplaySettingsModel final
{
public:
    LegacyDisplaySettingsModel(std::span<const LegacyDisplayMode>, LegacyDisplayMode current,
        std::uint32_t scaling);
    void SelectResolution(std::size_t);
    void SelectRate(std::size_t);
    [[nodiscard]] std::size_t SelectedModeIndex() const;
    [[nodiscard]] std::optional<std::size_t> FindMode(LegacyDisplayMode, bool includeDepth) const;
    [[nodiscard]] const std::vector<LegacyDisplayResolution>& Resolutions() const noexcept { return resolutions_; }
    [[nodiscard]] const std::vector<LegacyDisplayMode>& Modes() const noexcept { return modes_; }
    [[nodiscard]] std::size_t SelectedResolution() const noexcept { return selectedResolution_; }
    // 原 +38 是最后请求预览的枚举下标，失败后也不回滚成当前显示模式。
    std::size_t displayedModeIndex = 0;
    LegacyDisplayMode displayed;
    std::uint32_t scaling;
private:
    std::vector<LegacyDisplayMode> modes_;
    std::vector<LegacyDisplayResolution> resolutions_;
    std::size_t selectedResolution_ = 0;
};

// 01:40B210/40B360/40B270：15 次 1000ms tick 后取消，不依赖真实计时器。
class LegacyDisplayConfirmationCountdown final
{
public:
    [[nodiscard]] unsigned Seconds() const noexcept { return seconds_; }
    [[nodiscard]] bool Tick() noexcept;
private:
    unsigned seconds_ = 15;
};

class ILegacyDisplaySettingsStore
{
public:
    virtual ~ILegacyDisplaySettingsStore() = default;
    virtual std::uint32_t ReadScaling() = 0;
    virtual void WriteScaling(std::uint32_t) = 0;
    virtual LegacyDisplayMode ReadDeviceMode(std::uint32_t deviceRecordIndex) = 0;
    virtual void WriteDeviceMode(std::uint32_t deviceRecordIndex, LegacyDisplayMode) = 0;
};
}
