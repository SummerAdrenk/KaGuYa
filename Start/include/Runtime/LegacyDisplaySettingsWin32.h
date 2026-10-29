#pragma once
#include <Runtime/LegacyDisplaySettingsRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 使用当前游戏已编译的 210/211，不复制模板；02/03 的 211 原图标为 206，01 为 205。
// 所有 USER32 行为隔离在此适配器，纯数据探针只注入 ILegacyDisplaySettingsUi。
class Win32LegacyDisplaySettingsUi final : public ILegacyDisplaySettingsUi
{
public:
    Win32LegacyDisplaySettingsUi(const HINSTANCE& resources, const HWND& parent);
    ~Win32LegacyDisplaySettingsUi() override;
    Win32LegacyDisplaySettingsUi(const Win32LegacyDisplaySettingsUi&) = delete;
    Win32LegacyDisplaySettingsUi& operator=(const Win32LegacyDisplaySettingsUi&) = delete;
    void Open(LegacyDisplaySettingsRuntime&) override;
    void Show(bool) override;
    void Close() noexcept override;
    void Synchronize(const LegacyDisplaySettingsModel&) override;
    std::uint32_t ReadScaling() override;
    LegacyDisplayConfirmResult Confirm(LegacyDisplayMode) override;
    void Redraw() override;
    void RecenterAndRedraw() override;
    void ReportPreviewRestoreFailure() override;
    void RethrowPendingFailure() override;
    // 在 MainFrame 的 accelerator 前调用，保留 Tab/Enter/Escape 的非模态对话框语义。
    bool PreTranslate(MSG&);
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
