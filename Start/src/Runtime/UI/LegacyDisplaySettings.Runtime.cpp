#include <Runtime/LegacyDisplaySettingsRuntime.h>
#include <Runtime/GraphicsPresentNativeApi.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
LegacyDisplaySettingsRuntime::LegacyDisplaySettingsRuntime(LegacyDisplaySettingsFamily family,
    ILegacyDisplaySettingsHost& host, ILegacyDisplaySettingsStore& store, ILegacyDisplayModeApi& modes,
    ILegacyDisplaySettingsUi& ui, IGraphicsPresentNativeApi& present) noexcept
    : family_(family), host_(host), store_(store), modes_(modes), ui_(ui), present_(present) {}
LegacyDisplaySettingsRuntime::~LegacyDisplaySettingsRuntime()
{
    // 与旧析构相同，不暗中切屏；正常退出应先 RequestClose(false)/Poll。
    if (model_) ui_.Close();
}

const LegacyDisplaySettingsModel& LegacyDisplaySettingsRuntime::Model() const
{
    if (!model_) throw std::logic_error("Legacy display settings are not open");
    return *model_;
}
bool LegacyDisplaySettingsRuntime::Dispatch(unsigned map, UINT message, WPARAM w, LPARAM, LRESULT& result)
{
    if (family_ == LegacyDisplaySettingsFamily::Disabled || map || message != WM_COMMAND) return false;
    switch (LOWORD(w))
    {
    case 0x801D: Open(); break;
    case 0x801E: if (model_) ApplyScaling(); break;
    case 0x801F: if (model_) Preview(); break;
    default: return false;
    }
    result = 0;
    return true;
}
void LegacyDisplaySettingsRuntime::Open()
{
    if (model_) throw std::logic_error("Legacy display settings already have an active owner");
    auto context = host_.CaptureDisplayContext();
    // 先验证真实记录身份；不得把缺少设备目录的接线伪装为可用设置窗口。
    static_cast<void>(store_.ReadDeviceMode(context.deviceRecordIndex));
    std::vector<LegacyDisplayMode> descriptions;
    for (const auto& mode : context.modes) descriptions.push_back(DescribeLegacyDisplayMode(mode));
    auto model = std::make_unique<LegacyDisplaySettingsModel>(descriptions,
        DescribeLegacyDisplayMode(ReadLegacyDisplayMode(modes_, context.deviceName)), store_.ReadScaling());
    context_ = std::move(context);
    model_ = std::move(model);
    closeRequest_ = 0;
    lastClose_ = LegacyDisplayCloseResult::None;
    try
    {
        ui_.Open(*this);
        ui_.Synchronize(*model_);
        ui_.Show(true);
        host_.SetMinimizeGate(false);
    }
    catch (...)
    {
        ui_.Close();
        model_.reset();
        throw;
    }
}
void LegacyDisplaySettingsRuntime::SelectResolution(std::size_t index)
{
    static_cast<void>(Model());
    if (index == model_->SelectedResolution()) return;
    model_->SelectResolution(index);
    ui_.Synchronize(*model_);
}
void LegacyDisplaySettingsRuntime::SelectRate(std::size_t index)
{
    static_cast<void>(Model());
    model_->SelectRate(index);
}
void LegacyDisplaySettingsRuntime::RequestPreview()
{
    static_cast<void>(Model());
    const auto index = model_->SelectedModeIndex();
    if (index == model_->displayedModeIndex) return;
    // 01:40A200 在 PostMessage(801F) 前发布请求/标签，预览失败仍保留。
    model_->displayedModeIndex = index;
    model_->displayed = model_->Modes().at(index);
    ui_.Synchronize(*model_);
    host_.PostDisplayCommand(0x801F);
}
void LegacyDisplaySettingsRuntime::RequestScaling() { host_.PostDisplayCommand(0x801E); }
void LegacyDisplaySettingsRuntime::RequestClose(bool save)
{
    static_cast<void>(Model());
    closeRequest_ = save ? 1 : 2;
}
void LegacyDisplaySettingsRuntime::Refresh(std::uint32_t scale)
{
    host_.RefreshGameViewport(scale);
    present_.RenderScreenIfLoaded();
}
void LegacyDisplaySettingsRuntime::ApplyScaling()
{
    const auto old = model_->scaling;
    model_->scaling = ui_.ReadScaling();
    if (old != model_->scaling)
    {
        Refresh(model_->scaling);
        ui_.Redraw();
    }
}
void LegacyDisplaySettingsRuntime::Preview()
{
    const auto before = ReadLegacyDisplayMode(modes_, context_.deviceName);
    const auto requested = context_.modes.at(model_->displayedModeIndex);
    if (SameLegacyDisplaySizeAndRate(DescribeLegacyDisplayMode(before), DescribeLegacyDisplayMode(requested))) return;
    if (!ApplyLegacyDisplayMode(modes_, context_.deviceName, requested)) return;
    Refresh(model_->scaling);
    ui_.Show(false);
    const auto confirmation = ui_.Confirm(DescribeLegacyDisplayMode(requested));
    if (confirmation != LegacyDisplayConfirmResult::Keep)
    {
        // 01:40A840 只恢复显示请求，不改滑轨或各分辨率的刷新率选择。
        const auto oldIndex = model_->FindMode(DescribeLegacyDisplayMode(before), false);
        if (!oldIndex) throw std::runtime_error("Previous display mode disappeared from the catalogue");
        model_->displayedModeIndex = *oldIndex;
        model_->displayed = DescribeLegacyDisplayMode(before);
        ui_.Synchronize(*model_);
        if (!ApplyLegacyDisplayMode(modes_, context_.deviceName, context_.modes.at(*oldIndex)))
        {
            ui_.ReportPreviewRestoreFailure();
            ui_.Show(true);
            return;
        }
        Refresh(model_->scaling);
    }
    ui_.Show(true);
    ui_.RecenterAndRedraw();
    // 原 DialogBox(-1) 被误当确认；现代安全域回退后显式报错，不能自动确认。
    if (confirmation == LegacyDisplayConfirmResult::Failed)
        throw std::runtime_error("Legacy display confirmation dialog failed");
}
bool LegacyDisplaySettingsRuntime::Poll()
{
    if (!model_) return false;
    ui_.RethrowPendingFailure();
    if (closeRequest_) Finish();
    return true;
}
void LegacyDisplaySettingsRuntime::Finish()
{
    model_->scaling = ui_.ReadScaling();
    if (closeRequest_ == 1)
    {
        // 01:40B5F2..40B624：先写策略，再写最后请求显示的模式；此处不应用滑轨未预览值。
        store_.WriteScaling(model_->scaling);
        store_.WriteDeviceMode(context_.deviceRecordIndex, model_->Modes().at(model_->displayedModeIndex));
        lastClose_ = LegacyDisplayCloseResult::Registered;
    }
    else
    {
        const auto saved = store_.ReadDeviceMode(context_.deviceRecordIndex);
        const auto current = DescribeLegacyDisplayMode(ReadLegacyDisplayMode(modes_, context_.deviceName));
        const bool changed = !SameLegacyDisplaySizeAndRate(saved, current);
        lastClose_ = LegacyDisplayCloseResult::Cancelled;
        if (changed)
        {
            // 01:417B50/4172F0：差异比较忽略色深，但恢复查找必须匹配四个字段。
            const auto index = model_->FindMode(saved, true);
            if (!index || !ApplyLegacyDisplayMode(modes_, context_.deviceName, context_.modes.at(*index)))
                lastClose_ = LegacyDisplayCloseResult::RestoreFailed;
        }
        const auto scale = store_.ReadScaling();
        // 原最终取消只有 Resize/Reset，没有额外 RenderSceen。
        if (changed || scale != model_->scaling) host_.RefreshGameViewport(scale);
    }
    ui_.Close();
    model_.reset();
    closeRequest_ = 0;
    host_.SetMinimizeGate(true);
}
}
