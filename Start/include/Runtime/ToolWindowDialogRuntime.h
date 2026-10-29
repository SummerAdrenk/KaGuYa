#pragma once
#include <Platform/InputViews.h>

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>
#include <Runtime/ToolWindowInteractionRuntime.h>

#include <optional>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 606FE0/606FDC 是可写数据；这里只恢复样本初值，不声称已证明全部外部写方。
struct ToolWindowDialogRelationState final
{
    std::int32_t controlIdAt606FE0 = 100;
    std::int32_t relatedIdAt606FDC = 10;
};

struct ToolWindowDialogDependencies final
{
    // 固定子对象入口：保留旧六参数花括号语法；子对象须长于所有使用它的Dialog。
    ToolWindowDialogDependencies(ICgOperationSurfaceFactory& surfaces,
        IGraphicsSurfaceCompositionApi& graphics, IGraphicsSurfacePixelApi& pixels,
        Platform::MouseView mouse, Platform::KeyboardView keyboard,
        const ToolWindowDialogRelationState& relation) noexcept;
    // 4E3250..4E326F仅安装查询对象；允许先建Dialog、后执行4507E0。
    // 只借用稳定aggregate，不在构造时解析子对象；aggregate须长于Dialog。
    ToolWindowDialogDependencies(ICgOperationSurfaceFactory& surfaces,
        IGraphicsSurfaceCompositionApi& graphics, IGraphicsSurfacePixelApi& pixels,
        const Platform::IInputRuntime& input, const ToolWindowDialogRelationState& relation) noexcept;

    ICgOperationSurfaceFactory& surfaces;
    IGraphicsSurfaceCompositionApi& graphics;
    IGraphicsSurfacePixelApi& pixels;
    const ToolWindowDialogRelationState& relation;

private:
    friend class ToolWindowDialogRuntime;
    // 仅由上述重载选择一种来源；复制依赖时保留同一种借用，不提前解析。
    const Platform::IInputRuntime* input_ = nullptr;
    Platform::MouseView mouse_ = nullptr;
    Platform::KeyboardView keyboard_ = nullptr;
};

// 样本 18 的 GuiMessWinButton::MessWinDialog(4E27F0/5BE994)。
// 父 Dialog 固定为空；只恢复原两张构造表，不提供猜测的动态控件插入。
class ToolWindowDialogRuntime final : public IToolWindowDialog,
    public IToolWindowDialogInteraction,
    private ICgOperationButtonInputServices
{
public:
    ToolWindowDialogRuntime(std::int32_t mode,
        ToolWindowDialogDependencies dependencies);
    ~ToolWindowDialogRuntime() override;
    ToolWindowDialogRuntime(const ToolWindowDialogRuntime&) = delete;
    ToolWindowDialogRuntime& operator=(const ToolWindowDialogRuntime&) = delete;

    void ResetControlsAt532480() override;
    // 5325A0：仅enabled门控的逐child+08，不清勾选/事件/active/dirty。
    void ResetChildrenForCaptureAt5325A0();
    void SetEnabledAtVslot24(std::uint8_t value) override;
    [[nodiscard]] const RECT& BoundsAt60() const override;
    void ResetAtVslot44() override;
    void DrawAtVslot60(const RECT& dirty) override;
    void CompositeAtVslot5C(YtlReferenceSlotRuntime& consumed,
        const RECT* clip) override;

    // 交互侧调用独立于六项图层接口；+54 的 EAX 不是业务返回值。
    void UpdateAtVslot54() override;
    [[nodiscard]] std::int32_t ResultAtVslot50() override;
    [[nodiscard]] std::int32_t ScanControlStateAt50FD20() override;
    void SetControlEnabledAt508470(std::int32_t id, std::int32_t enabled) override;
    void SetControlCheckedAt501FA0(std::int32_t id, std::int32_t checked) override;
    void ResetControlAt5312B0(std::int32_t id) override;
    void InvalidateAllControlsAt530DC0() override;
    void ReconcileControlsAt527720() override;
    void ClearActiveControlAtVslot30() override;
    void SetInputByteAtVslot1C(std::uint8_t value) override;
    [[nodiscard]] ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() override;
    [[nodiscard]] const std::optional<std::int32_t>& EventAt5C() const noexcept;
    void WriteModeAtVslot1C(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t ModeAt34() const noexcept;
    [[nodiscard]] std::int32_t EnabledAt38() const noexcept;
    [[nodiscard]] std::shared_ptr<CgOperationChildControlRuntime>
        FindControlAt509D20(std::int32_t eventValue) const;
    [[nodiscard]] std::span<const CgOperationChildControlRef>
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const ICgOperationDialogEventTarget* ActiveControlAt4C() const noexcept;

private:
    void ReleaseOwnedAt4FCA80() noexcept;
    void ProcessGenericInputAt518C30();
    void ResetControlAndBoundEvent(CgOperationChildControlRuntime& control);
    void SubmitActiveControlAt532170(ICgOperationDialogEventTarget* control);
    void SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control) override;
    bool ReadInputAtVslot6C(std::int32_t action) override;
    bool ReadInputAtVslot70(std::int32_t action) override;
    POINT ReadPointerAtVslot74() override;
    bool ReadPressAtVslot80() override;
    bool ReadReleaseAtVslot84() override;
    std::uint32_t ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle handle,
        std::int32_t x, std::int32_t y) override;

    ToolWindowDialogDependencies dependencies_;
    CgOperationDialogRenderHostRuntime renderHost_;
    std::vector<CgOperationChildControlRef> children_;
    std::unique_ptr<IGuiInputStateRuntime> inputAt18_;
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyAt50_;
    std::vector<CgOperationClickedEventRef> eventsAt70_;
    ICgOperationDialogEventTarget* activeAt4C_ = nullptr;
    std::optional<std::int32_t> eventAt5C_;
    RECT boundsAt60_{};
};

// 依赖均借用根 owner，必须长于本工厂产生的所有 Dialog。
class ToolWindowDialogFactoryRuntime final : public IToolWindowDialogFactory
{
public:
    explicit ToolWindowDialogFactoryRuntime(ToolWindowDialogDependencies dependencies);
    [[nodiscard]] std::shared_ptr<IToolWindowDialog>
        CreateDialogAt4E27F0(std::int32_t mode) override;

private:
    ToolWindowDialogDependencies dependencies_;
};
}
