#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/GuiNavigationNative.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class OpacityDialogObjectGraphRuntime;

// 50D857 取 manager owning[1] 的 CMesWin；按其样式索引选择
// Environment slot 1 Save 的 +44/+4C。不是 LayerWindow[0] 的 Pattern。
class IOpacityDialogResetStateServices
{
public:
    virtual ~IOpacityDialogResetStateServices() = default;

    [[nodiscard]] virtual bool
        SelectMessageWindowOpacitySelectorNonZeroAt50D730() const = 0;
    [[nodiscard]] virtual std::int32_t
        ReadSaveSlot1OpacitySelectorZeroOrNonZero(
            bool selectorNonZero) const = 0;
};

// 52DF20 在通用 Dialog 输入前后读取 Slider，按 CMesWin 样式选择
// Save+44/+4C，再刷新同一 CMesWin 的样式面板。
class IOpacityDialogUpdateServices
{
public:
    virtual ~IOpacityDialogUpdateServices() = default;

    [[nodiscard]] virtual bool
        SelectMessageWindowOpacitySelectorNonZeroAt52DF20() const = 0;
    virtual void WriteSaveSlot1OpacitySelectorZeroOrNonZero(
        bool selectorNonZero,
        std::int32_t value) = 0;
    virtual void RefreshMessageWindowStyleAt5325F0(
        std::int32_t selector) = 0;
};

// GuiDialog+0x24 保存的导航组。0x51E740 通过 +0x04 的输入值选择组，
// 再用虚槽 0 或 +0x08 vector 枚举候选控件事件值。
class IGuiDialogNavigationGroup
{
public:
    virtual ~IGuiDialogNavigationGroup() = default;

    [[nodiscard]] virtual std::int32_t InputValueAt04() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t NextAtVslot00(
        std::int32_t currentEventValue) const = 0;
    [[nodiscard]] virtual std::span<const std::int32_t>
        InitialCandidatesAt08() const noexcept = 0;
};

using GuiDialogNavigationGroupRef =
    std::shared_ptr<IGuiDialogNavigationGroup>;

// 0x504CE0 创建的普通 GuiOperationKey。+0x08 vector 保留原 pair 表顺序；
// vslot 0 按 source 查找并返回对应 target，未命中返回 -1。
class GuiDialogNavigationGroupRuntime final
    : public IGuiDialogNavigationGroup
{
public:
    GuiDialogNavigationGroupRuntime(
        std::int32_t inputValueAt04,
        std::vector<GuiDialogNavigationMapping> mappings);

    [[nodiscard]] std::int32_t InputValueAt04() const noexcept override;
    [[nodiscard]] std::int32_t NextAtVslot00(
        std::int32_t currentEventValue) const override;
    [[nodiscard]] std::span<const std::int32_t>
        InitialCandidatesAt08() const noexcept override;

    [[nodiscard]] std::span<const GuiDialogNavigationMapping>
        MappingsAt08() const noexcept;

private:
    std::int32_t inputValueAt04_ = 0;
    std::vector<GuiDialogNavigationMapping> mappingsAt08_;
    std::vector<std::int32_t> initialCandidatesAt08_;
};

// GuiOpacityControl::OpacityDialog(0x4E4780) 的样本 18 生产对象图。
// Static 与 Slider 按原构造顺序共同持有，避免只恢复滑块而丢失背景控件。
class OpacityDialogObjectGraphRuntime final
    : private ICgOperationSliderInputServices
{
public:
    OpacityDialogObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationSliderInputSource& inputServices);
    ~OpacityDialogObjectGraphRuntime();
    OpacityDialogObjectGraphRuntime(const OpacityDialogObjectGraphRuntime&) = delete;
    OpacityDialogObjectGraphRuntime& operator=(const OpacityDialogObjectGraphRuntime&) = delete;

    [[nodiscard]] ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() noexcept;

    void DrawAt5230B0(const RECT& clip);
    // 与原 +5C 相同：消费调用方已经构造的按值 Ref。
    void CompositeAt5231C0(
        YtlReferenceSlotRuntime& consumed,
        const RECT* destinationClip);
    [[nodiscard]] std::int32_t ProcessSliderInputAt521820();
    [[nodiscard]] std::int32_t ProcessUpperInputPathAt51E740();
    void ProcessGenericDialogInputAt518C30();
    [[nodiscard]] std::int32_t UpdateAtVslot48At52DF20(
        IOpacityDialogUpdateServices& services);
    void WriteEnabledAtVslot24(std::int32_t value) noexcept;
    void ResetAtVslot44At50D730(
        const IOpacityDialogResetStateServices& stateServices);

    [[nodiscard]] const std::shared_ptr<CgOperationChildControlRuntime>&
        BackgroundStatic() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationSliderRuntime>&
        Slider() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const RECT& DialogRectAt5C() const noexcept;
    [[nodiscard]] std::int32_t EnabledAt38() const noexcept;
    [[nodiscard]] std::int32_t InputLatchAt30() const noexcept;
    [[nodiscard]] ICgOperationChildControl*
        ActiveControlAt4C() const noexcept;
    void SetNavigationGroupsAt24ForConstruction(
        std::vector<GuiDialogNavigationGroupRef> navigationGroups);

private:
    void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) override;
    [[nodiscard]] bool ReadInputAtVslot70(
        std::int32_t eventValue) override;
    [[nodiscard]] POINT ReadPointerAtVslot74() override;
    [[nodiscard]] std::int32_t ReadStepDirectionAtVslot78() override;
    [[nodiscard]] bool ReadInputAtVslot7C() override;
    [[nodiscard]] bool ReadPressAtVslot80() override;
    [[nodiscard]] bool ReadReleaseAtVslot84() override;
    [[nodiscard]] std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) override;

    void SubmitActiveControlAt532170(ICgOperationChildControl* control);
    void ReleaseChildrenAndDirty() noexcept;
    void ResetInputAt5106B0();
    void ResetChildrenAt522E90();
    [[nodiscard]] CgOperationChildControlRef ResolveChildAt509D20(
        std::int32_t eventValue) const;
    [[nodiscard]] bool TryActivateCandidateAt51E740(
        std::int32_t eventValue,
        bool resetChildrenFirst);

    CgOperationDialogRenderHostRuntime renderHost_;
    ICgOperationSliderInputSource* inputServices_ = nullptr;
    std::shared_ptr<CgOperationChildControlRuntime> backgroundStatic_;
    std::shared_ptr<CgOperationSliderRuntime> slider_;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    std::int32_t inputLatchAt30_ = 0;
    ICgOperationChildControl* activeControlAt4C_ = nullptr;
    RECT dialogRectAt5C_{};
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyAt50_;
};
}
