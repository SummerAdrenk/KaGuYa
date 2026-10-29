#pragma once

#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class ITitleDialogInputNotifications
{
public:
    virtual ~ITitleDialogInputNotifications() = default;
    virtual bool ReadPointerMovedAtVslot7C() = 0;
    virtual void SubmitChildRectAt50FC80(const RECT& rect) = 0;
    virtual void NotifyChildChangedAt518AE0(CgOperationChildControlRef child) = 0;
};

// TitleDialog 的输入状态与已确认的导航/视觉差异循环。
// +4C 是指向 child vector 所有对象的非拥有指针，不能额外延长 child 寿命。
class TitleDialogInputRuntime final : public ICgOperationButtonInputServices
{
public:
    explicit TitleDialogInputRuntime(ICgOperationButtonInputServices& input) noexcept;
    void ResetAt510840();
    void SubmitActiveAt532170(ICgOperationChildControl* control);
    std::int32_t ProcessConfirmAt511500();
    std::int32_t ProcessNavigationAt51E740(std::span<const CgOperationChildControlRef> children,
        std::span<const GuiDialogNavigationGroupRef> navigation, ITitleDialogInputNotifications& notifications);
    void UpdateAt518C30(std::int32_t modeAt34, std::int32_t enabledAt38,
        std::span<const CgOperationChildControlRef> children,
        std::span<const GuiDialogNavigationGroupRef> navigation, ITitleDialogInputNotifications& notifications);
    ICgOperationChildControl* ActiveAt4C() const noexcept { return active_; }
    std::optional<std::int32_t> ConfirmLatchAtC4() const noexcept { return confirmLatch_; }
    std::int32_t NavigationLatchAt30() const noexcept { return navigationLatch_; }
    void WriteNavigationLatchAt30(std::int32_t value) noexcept { navigationLatch_ = value; }

    void SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control) override;
    bool ReadInputAtVslot6C(std::int32_t eventValue) override;
    bool ReadInputAtVslot70(std::int32_t eventValue) override;
    POINT ReadPointerAtVslot74() override;
    bool ReadPressAtVslot80() override;
    bool ReadReleaseAtVslot84() override;
    std::uint32_t ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle handle,
        std::int32_t x, std::int32_t y) override;

private:
    ICgOperationButtonInputServices* input_;
    ICgOperationChildControl* active_ = nullptr;
    std::optional<std::int32_t> confirmLatch_;
    std::int32_t navigationLatch_ = 0;
};
}
