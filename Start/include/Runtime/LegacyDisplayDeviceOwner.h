#pragma once
#include <Runtime/LegacyDisplayDeviceApi.h>
#include <Runtime/LegacyDisplaySettingsRuntime.h>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
// 只给启动 owner 扩展数量维护；210 receiver 继续使用原来的窄 Store 接口。
class ILegacyDisplayDeviceStore : public ILegacyDisplaySettingsStore
{
public:
    virtual std::uint32_t DeviceRecordCount() = 0;
    virtual void ClearDeviceRecords() = 0;
    virtual void AppendDeviceRecord(LegacyDisplayMode) = 0;
};

struct LegacyDisplayDevice final
{
    std::string name; // 空串仅表示原版明确的 NULL-device 分支。
    std::vector<DEVMODEA> modes;
    bool usable = true;
};
enum class LegacyDisplayOwnershipResult
{ NoChange, Acquired, MissingSavedMode, ApplyFailed, Released, RestoreFailed };

// 正文外运行态：不复制原 2Ch 布局，不序列化设备名、枚举号或 active。
// API/store 必须覆盖 owner 寿命；所有调用由主线程串行执行。
class LegacyDisplayDeviceOwner final
{
public:
    LegacyDisplayDeviceOwner(ILegacyDisplayDeviceApi& api, ILegacyDisplayDeviceStore& store) noexcept
        : api_(api), store_(store) {}
    ~LegacyDisplayDeviceOwner() = default; // 销毁前必须显式 Shutdown 并处理结果。
    LegacyDisplayDeviceOwner(const LegacyDisplayDeviceOwner&) = delete;
    LegacyDisplayDeviceOwner& operator=(const LegacyDisplayDeviceOwner&) = delete;

    void Initialize(std::uint32_t logicalWidth, std::uint32_t logicalHeight);
    [[nodiscard]] bool Initialized() const noexcept { return initialized_; }
    [[nodiscard]] const std::vector<LegacyDisplayDevice>& Devices() const noexcept { return devices_; }
    [[nodiscard]] std::uint32_t SelectedIndex() const;
    [[nodiscard]] std::optional<std::uint32_t> ActiveIndex() const noexcept { return active_; }

    // 01:417C00 / 03:41A150：单设备不查 monitor；多设备无匹配先取目录0，
    // 不可用时按原418020重新查 primary，继而查首个可用项。这不是记录固定index0。
    void SelectForFullscreen(std::optional<std::string_view> windowMonitorDevice);
    [[nodiscard]] LegacyDisplayOwnershipResult SetExclusive(bool acquire);
    [[nodiscard]] LegacyDisplayOwnershipResult Shutdown();
    [[nodiscard]] LegacyDisplaySettingsContext CaptureDisplayContext() const;
    [[nodiscard]] DEVMODEA ReadSelectedCurrentMode();
    // 离开全屏的原失败分支确实返回零位置/零尺寸；不拿另一设备替代。
    [[nodiscard]] DEVMODEA ReadSelectedRegistryModeOrZero();

    // 复用现代 WindowMode/Graphics/platform；不调用共享18号的边框全屏选择器。
    [[nodiscard]] LegacyDisplayOwnershipResult SetWindowMode(WindowModeView,
        IGraphicsWindowModeApi&, IWindowModePlatformApi&, HWND,
        bool fullscreen, LONG style, LONG exStyle);
    void RefreshViewport(WindowModeView, IGraphicsWindowModeApi&, IWindowModePlatformApi&,
        HWND, std::uint32_t scaling, UINT additionalFlags = 0);

private:
    struct Seed { LegacyDisplayMode value; bool supported; };
    void RequireInitialized() const;
    Seed DefaultRecord(std::uint32_t);
    std::uint32_t FindUsablePrimaryOrFirst();
    LegacyDisplayOwnershipResult Acquire();
    LegacyDisplayOwnershipResult Release();
    ILegacyDisplayDeviceApi& api_;
    ILegacyDisplayDeviceStore& store_;
    std::vector<LegacyDisplayDevice> devices_;
    std::uint32_t selected_ = 0, width_ = 0, height_ = 0;
    std::optional<std::uint32_t> active_;
    bool initialized_ = false;
};

enum class LegacyDisplayActivationAction { None, MinimizeThenRelease, AcquireThenRestorePlacement };
[[nodiscard]] LegacyDisplayActivationAction DecideLegacyDisplayActivation(
    bool fullscreen, bool activating, bool iconic, bool minimizeGate) noexcept;
}
