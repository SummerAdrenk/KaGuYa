#pragma once
#include <Runtime/LegacyDisplaySettingsPlatform.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class LegacyDisplaySettingsRuntime;
class IGraphicsPresentNativeApi;

// 运行态设备目录的快照，不是 SaveData 副本，也不序列化设备名/DEVMODE。
struct LegacyDisplaySettingsContext final
{
    std::string deviceName;
    std::uint32_t deviceRecordIndex = 0;
    std::vector<DEVMODEA> modes;
};
enum class LegacyDisplayConfirmResult { Keep, Revert, Failed };
enum class LegacyDisplayCloseResult { None, Registered, Cancelled, RestoreFailed };

class ILegacyDisplaySettingsHost
{
public:
    virtual ~ILegacyDisplaySettingsHost() = default;
    // 必须来自真实设备枚举/当前全屏设备，不能硬填设备 0 或临时扩容 recordsAt10。
    virtual LegacyDisplaySettingsContext CaptureDisplayContext() = 0;
    // 对当前 Root 的 WindowMode 调用 RefreshLegacyDisplayViewport；再呈现由控制器负责。
    virtual void RefreshGameViewport(std::uint32_t scaling) = 0;
    // 原 Root+19A 的共享语义为 Frame().WriteMinimizeByteAt1B2(value)。
    virtual void SetMinimizeGate(bool enabled) = 0;
    virtual void PostDisplayCommand(WORD command) = 0;
};

class ILegacyDisplaySettingsUi
{
public:
    virtual ~ILegacyDisplaySettingsUi() = default;
    virtual void Open(LegacyDisplaySettingsRuntime&) = 0;
    virtual void Show(bool) = 0;
    virtual void Close() noexcept = 0;
    virtual void Synchronize(const LegacyDisplaySettingsModel&) = 0;
    virtual std::uint32_t ReadScaling() = 0;
    virtual LegacyDisplayConfirmResult Confirm(LegacyDisplayMode) = 0;
    virtual void Redraw() = 0;
    virtual void RecenterAndRedraw() = 0;
    virtual void ReportPreviewRestoreFailure() = 0;
    // 原生回调不得跨 USER32 栈抛异常；在正常 C++ 消费点重新抛出。
    virtual void RethrowPendingFailure() = 0;
};

class LegacyDisplaySettingsRuntime final
{
public:
    LegacyDisplaySettingsRuntime(LegacyDisplaySettingsFamily, ILegacyDisplaySettingsHost&,
        ILegacyDisplaySettingsStore&, ILegacyDisplayModeApi&, ILegacyDisplaySettingsUi&,
        IGraphicsPresentNativeApi&) noexcept;
    ~LegacyDisplaySettingsRuntime();
    LegacyDisplaySettingsRuntime(const LegacyDisplaySettingsRuntime&) = delete;
    LegacyDisplaySettingsRuntime& operator=(const LegacyDisplaySettingsRuntime&) = delete;

    // 直接接入 MainFrame 的 WM_COMMAND 接收分支；返回 false 时不改 result。
    bool Dispatch(unsigned map, UINT message, WPARAM, LPARAM, LRESULT& result);
    // 对应 01:40B530/40B580。入口有 210 时返回 true，包括关闭的那一帧；宿主跳过 Root tick。
    bool Poll();
    [[nodiscard]] bool Active() const noexcept { return model_ != nullptr; }
    [[nodiscard]] const LegacyDisplaySettingsModel& Model() const;
    [[nodiscard]] LegacyDisplayCloseResult LastCloseResult() const noexcept { return lastClose_; }
    void SelectResolution(std::size_t);
    void SelectRate(std::size_t);
    void RequestPreview();
    void RequestScaling();
    void RequestClose(bool registerSettings);

private:
    void Open();
    void ApplyScaling();
    void Preview();
    void Refresh(std::uint32_t);
    void Finish();
    LegacyDisplaySettingsFamily family_;
    ILegacyDisplaySettingsHost& host_;
    ILegacyDisplaySettingsStore& store_;
    ILegacyDisplayModeApi& modes_;
    ILegacyDisplaySettingsUi& ui_;
    IGraphicsPresentNativeApi& present_;
    LegacyDisplaySettingsContext context_;
    std::unique_ptr<LegacyDisplaySettingsModel> model_;
    unsigned closeRequest_ = 0;
    LegacyDisplayCloseResult lastClose_ = LegacyDisplayCloseResult::None;
};
}
