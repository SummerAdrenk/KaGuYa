#pragma once

#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/MusicAppreciationButtonRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IMusicAppreciationModeSoundCheckbox
{
public:
    virtual ~IMusicAppreciationModeSoundCheckbox() = default;

    [[nodiscard]] virtual std::int32_t ControlIdAt04() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t CheckedStateAt5C() const noexcept = 0;
    virtual void WriteCheckedStateAt5C(std::int32_t value) noexcept = 0;
    [[nodiscard]] virtual RECT ReadRectAtVslot3C() const = 0;
    virtual void ClearInteractionStateAt04And20() = 0;
};

using MusicAppreciationModeSoundCheckboxRef =
    std::shared_ptr<IMusicAppreciationModeSoundCheckbox>;

// CheckBox（0x50A110/0x50B280）的生产状态。原对象固定持有 Off/On 两个
// SurfaceGroup；+0x5C 只决定当前使用哪一组，不改变 +0x3C 返回的命中矩形。
class MusicAppreciationModeSoundCheckboxRuntime final
    : public IMusicAppreciationModeSoundCheckbox
{
public:
    MusicAppreciationModeSoundCheckboxRuntime(
        std::int32_t controlIdAt04,
        std::shared_ptr<CgOperationControlResourceGroupRuntime> offResources,
        std::shared_ptr<CgOperationControlResourceGroupRuntime> onResources,
        CgOperationClickedEventRef clickedEvent);

    [[nodiscard]] std::int32_t ControlIdAt04() const noexcept override;
    [[nodiscard]] std::int32_t CheckedStateAt5C() const noexcept override;
    void WriteCheckedStateAt5C(std::int32_t value) noexcept override;
    [[nodiscard]] RECT ReadRectAtVslot3C() const override;
    void ClearInteractionStateAt04And20() override;

    [[nodiscard]] const CgOperationControlResourceGroupRuntime&
        CurrentResourceGroupAt50B280() const noexcept;
    [[nodiscard]] const CgOperationClickedEventRef&
        ClickedEventAt3C() const noexcept;
    [[nodiscard]] std::int32_t FocusStateAt20() const noexcept;
    [[nodiscard]] std::int32_t VisualSlotAt54() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationChildControlRuntime>&
        Control() const noexcept;

private:
    std::int32_t controlIdAt04_ = -1;
    std::shared_ptr<CgOperationChildControlRuntime> control_;
    CgOperationClickedEventRef clickedEventAt3C_;
};

class IMusicAppreciationModeSoundDialogServices
{
public:
    virtual ~IMusicAppreciationModeSoundDialogServices() = default;

    virtual void WriteDialogEnabledAtVslot24(std::int32_t value) = 0;
    virtual void InitializeChildrenAtVslot3C() = 0;
    virtual void UpdateGuiDialogAtVslot54() = 0;
    [[nodiscard]] virtual std::int32_t PollClickedEventAt50FDB0(
        std::int32_t clear) = 0;
    virtual void InvalidateRootRect(const RECT& rect) = 0;
    virtual void AddOwnDirtyRectAtVslot64(const RECT& rect) = 0;
    virtual void SubmitOwnDirtyRectsAtVslot58() = 0;
    [[nodiscard]] virtual std::span<const RECT>
        EventDirtyRectsAtVslots00_18_1C() const = 0;
    virtual void DrawChildrenAtVslot60(const RECT& clip) = 0;
};

// ModeSoundDialog 从 GuiDialog 基类继承的外部边界。子控件协调、焦点、
// ClickedEvent 和 dirty rect 提交均由生产 owner 自己复刻，不允许上层替代。
class IMusicAppreciationModeSoundDialogInheritedServices
{
public:
    virtual ~IMusicAppreciationModeSoundDialogInheritedServices() = default;

    virtual void WriteDialogEnabledAt538660(std::int32_t value) = 0;
    [[nodiscard]] virtual bool ReadInputHeldAtVslot6C(
        std::int32_t action) = 0;
    [[nodiscard]] virtual bool ReadInputPressedAtVslot70(
        std::int32_t action) = 0;
    [[nodiscard]] virtual POINT ReadPointerAtVslot74() = 0;
    [[nodiscard]] virtual bool ReadCancelAtVslot7C() = 0;
    [[nodiscard]] virtual bool ReadPointerPressAtVslot80() = 0;
    [[nodiscard]] virtual bool ReadPointerReleaseAtVslot84() = 0;
    [[nodiscard]] virtual std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) = 0;
    [[nodiscard]] virtual ICgDisplayButtonGroupDirtyRects&
        OwnDirtyRectsAtVslot00() = 0;
    virtual void InvalidateRootRect(const RECT& rect) = 0;
};

// GuiControl::GuiModeSoundDialog::ModeSoundDialog 的状态核心。
// 资源树由 0x4E3900 建立；本类型只接收已完成构造的 18 个 CheckBox，严格复刻
// 0x50D090/0x52DD40/0x50AB80 已证明的状态和调用次序。
class MusicAppreciationModeSoundDialogRuntime final
    : public IMusicAppreciationModeSoundDialog
{
public:
    MusicAppreciationModeSoundDialogRuntime(
        IMusicAppreciationModeSoundDialogServices& services,
        std::vector<MusicAppreciationModeSoundCheckboxRef> checkboxes,
        std::int32_t firstControlId,
        const RECT& dialogRectAt64);

    void InitializeAtVslot44() override;
    [[nodiscard]] std::int32_t UpdateAtVslot48() override;
    [[nodiscard]] std::int32_t SelectionAtVslot50() override;
    [[nodiscard]] std::span<const RECT>
        EventDirtyRectsAtVslots00_18_1C() const override;
    void DrawAtVslot60(const RECT* clip) override;
    [[nodiscard]] std::int32_t ItemCount() const noexcept override;
    void ClearItemStateAt04And20(std::int32_t itemIndex) override;

    [[nodiscard]] std::int32_t StateAt5C() const noexcept;
    void WriteStateAt5C(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t FieldAt4C() const noexcept;
    [[nodiscard]] const RECT& DialogRectAt64() const noexcept;

private:
    [[nodiscard]] IMusicAppreciationModeSoundCheckbox&
        CheckboxForControlId(std::int32_t controlId) const;
    [[nodiscard]] IMusicAppreciationModeSoundCheckbox&
        CheckboxForItemIndex(std::int32_t itemIndex) const;
    void ClearAllSelectionsAt532340(bool invalidate);

    IMusicAppreciationModeSoundDialogServices* services_ = nullptr;
    std::vector<MusicAppreciationModeSoundCheckboxRef> checkboxes_;
    std::int32_t firstControlId_ = 2;
    std::int32_t fieldAt4C_ = 0;
    std::int32_t stateAt5C_ = -1;
    RECT dialogRectAt64_{};
    std::int32_t selectionAt80_ = -1;
};

// 0x4E3900 的生产 owner：三张基础 Surface、18 次边框合成、标题合成、
// 18 个双 SurfaceGroup CheckBox 及 ClickedEvent 均在同一强所有权边界内建立。
class MusicAppreciationModeSoundDialogObjectGraphRuntime final
    : public IMusicAppreciationModeSoundDialog,
      private IMusicAppreciationModeSoundDialogServices,
      private ICgOperationButtonInputServices
{
public:
    MusicAppreciationModeSoundDialogObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        IMusicAppreciationModeSoundDialogInheritedServices& inheritedServices,
        const MusicAppreciationButtonStaticData& staticData);

    void InitializeAtVslot44() override;
    [[nodiscard]] std::int32_t UpdateAtVslot48() override;
    [[nodiscard]] std::int32_t SelectionAtVslot50() override;
    [[nodiscard]] std::span<const RECT>
        EventDirtyRectsAtVslots00_18_1C() const override;
    void DrawAtVslot60(const RECT* clip) override;
    [[nodiscard]] std::int32_t ItemCount() const noexcept override;
    void ClearItemStateAt04And20(std::int32_t itemIndex) override;

    [[nodiscard]] const std::shared_ptr<
        MusicAppreciationModeSoundDialogRuntime>& Dialog() const noexcept;
    [[nodiscard]] const std::array<std::shared_ptr<RuntimeGraphicsSurface>, 3>&
        BaseSurfacesInLoadOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationChildControlRuntime>&
        StaticBackgroundChild() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<
        MusicAppreciationModeSoundCheckboxRuntime>>& Checkboxes() const noexcept;
    [[nodiscard]] const std::vector<CgOperationClickedEventRef>&
        ClickedEvents() const noexcept;
    [[nodiscard]] std::span<const MusicAppreciationOperationKeyStaticData>
        OperationKeys() const noexcept;
    [[nodiscard]] const CgOperationChildControlRef&
        ActiveControlAt4C() const noexcept;
    [[nodiscard]] std::int32_t NavigationStateAt30() const noexcept;
    [[nodiscard]] std::int32_t ActionLatchAt60() const noexcept;
    [[nodiscard]] std::int32_t OperationTimer(std::size_t index) const;
    [[nodiscard]] std::int32_t OperationRepeatPhase(std::size_t index) const;

private:
    struct OperationKeyRuntimeState final
    {
        const MusicAppreciationOperationKeyStaticData* data = nullptr;
        std::int32_t timerAt14 = 0;
        std::int32_t repeatPhaseAt18 = 0;
    };

    void WriteDialogEnabledAtVslot24(std::int32_t value) override;
    void InitializeChildrenAtVslot3C() override;
    void UpdateGuiDialogAtVslot54() override;
    [[nodiscard]] std::int32_t PollClickedEventAt50FDB0(
        std::int32_t clear) override;
    void InvalidateRootRect(const RECT& rect) override;
    void AddOwnDirtyRectAtVslot64(const RECT& rect) override;
    void SubmitOwnDirtyRectsAtVslot58() override;
    void DrawChildrenAtVslot60(const RECT& clip) override;

    void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) override;
    [[nodiscard]] bool ReadInputAtVslot6C(std::int32_t action) override;
    [[nodiscard]] bool ReadInputAtVslot70(std::int32_t action) override;
    [[nodiscard]] POINT ReadPointerAtVslot74() override;
    [[nodiscard]] bool ReadPressAtVslot80() override;
    [[nodiscard]] bool ReadReleaseAtVslot84() override;
    [[nodiscard]] std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) override;

    void SubmitActiveControlAt532170(CgOperationChildControlRef control);
    void ResetInputAt5106F0();
    void ResetChildrenAt522E90();
    [[nodiscard]] CgOperationChildControlRef ResolveChildAt509D20(
        std::int32_t eventValue) const;
    [[nodiscard]] bool TryActivateCandidateAt51EB70(
        std::int32_t eventValue,
        bool resetChildrenFirst);
    [[nodiscard]] std::int32_t ProcessUpperInputAt51EB70();
    [[nodiscard]] std::int32_t ProcessActiveCheckBoxAt510C80();

    IMusicAppreciationModeSoundDialogInheritedServices* inheritedServices_ = nullptr;
    CgOperationDialogRenderHostRuntime renderHost_;
    std::array<std::shared_ptr<RuntimeGraphicsSurface>, 3> baseSurfaces_{};
    std::shared_ptr<CgOperationChildControlRuntime> staticBackgroundChild_;
    std::vector<std::shared_ptr<MusicAppreciationModeSoundCheckboxRuntime>>
        checkboxes_;
    std::vector<CgOperationClickedEventRef> clickedEvents_;
    std::vector<CgOperationChildControlRef> childRefs_;
    std::span<const MusicAppreciationOperationKeyStaticData> operationKeys_;
    std::vector<OperationKeyRuntimeState> operationKeyStates_;
    CgOperationChildControlRef activeControlAt4C_;
    std::int32_t navigationStateAt30_ = 0;
    std::int32_t actionLatchAt60_ = 0;
    std::shared_ptr<MusicAppreciationModeSoundDialogRuntime> dialog_;
};
}
