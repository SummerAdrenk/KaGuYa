#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/SaveLoadDialogRuntime.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// GuiControl::GuiSaveLoadDialog::SaveLoadMessageBox(0x4EC8A0) 的样本 18
// 生产对象图。本类只暴露已闭环的派生对象行为；ModalDialog 的通用
// 0x5071D0 状态机在其独立边界完成前不在这里伪造。
class SaveLoadMessageBoxObjectGraphRuntime final
{
public:
    SaveLoadMessageBoxObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        IGraphicsSurfaceCopyApi& copyGraphics,
        ICgOperationButtonInputServices& buttonInputServices);

    void WriteModeAt80(std::int32_t value) noexcept;

    // 0x50EAA0 在进入通用 ModalDialog::Begin 前执行的派生控件门控。
    void ApplyModeChildEnablementAt50EAA0();
    void PreparePageAt502950(std::int32_t pageNumber);
    void PreparePageAndSlotAt502B10(
        std::int32_t pageNumber,
        std::int32_t slotNumber);
    [[nodiscard]] std::int32_t PollClickedResultAt5253D0() noexcept;

    [[nodiscard]] std::int32_t ModeAt80() const noexcept;
    [[nodiscard]] std::int32_t ResultAt68() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<CgOperationClickedEventRef>&
        ClickedEventsInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<GuiDialogNavigationGroupRef>&
        NavigationGroupsAt24() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        DigitResourcesAt9C() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        PageAndSlotBaseAt84() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        PageOnlyBaseAt90() const noexcept;

private:
    [[nodiscard]] std::shared_ptr<CgOperationChildControlRuntime>
        ResolveChildAt509D20(
        std::int32_t eventValue) const;
    void EnsureDigitResourcesAt529FD0();
    void RestoreAndDrawNumber(
        const std::shared_ptr<CgOperationChildControlRuntime>& child,
        std::shared_ptr<RuntimeGraphicsSurface>& cachedBase,
        std::int32_t number,
        bool threeDigits);
    void DrawDigitAt529FD0(
        RuntimeGraphicsSurface& destination,
        std::int32_t digit,
        std::int32_t absoluteX,
        std::int32_t absoluteY);

    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    IGraphicsSurfaceCopyApi* copyGraphics_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<CgOperationClickedEventRef> clickedEvents_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    std::int32_t resultAt68_ = 0;
    std::int32_t modeAt80_ = 0;
    std::shared_ptr<RuntimeGraphicsSurface> pageAndSlotBaseAt84_;
    std::shared_ptr<RuntimeGraphicsSurface> pageOnlyBaseAt90_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digitResourcesAt9C_;
};

class ISaveLoadMessageBoxCapturedFrame
{
public:
    virtual ~ISaveLoadMessageBoxCapturedFrame() = default;

    // 捕获帧对象 +0x08..+0x14 保存提交给合成器虚槽 +0x10 的四个边界值。
    [[nodiscard]] virtual RECT ReadBoundsAt08() const = 0;
};

using SaveLoadMessageBoxCapturedFrameRef =
    std::shared_ptr<ISaveLoadMessageBoxCapturedFrame>;

class ISaveLoadMessageBoxInputServices
    : public ICgOperationButtonInputServices
{
public:
    virtual ~ISaveLoadMessageBoxInputServices() = default;

    [[nodiscard]] virtual bool ReadInputAtVslot7C() = 0;
};

class ISaveLoadMessageBoxModalServices
{
public:
    virtual ~ISaveLoadMessageBoxModalServices() = default;

    [[nodiscard]] virtual SaveLoadMessageBoxCapturedFrameRef
        CapturePrimaryFrameAt4ACDD0() = 0;
    virtual void CompositeCapturedFrameAtVslot10(
        const SaveLoadMessageBoxCapturedFrameRef& frame,
        const RECT& bounds) = 0;
    virtual void SynchronizeRuntimeAt413BE0() = 0;
    virtual void WriteRootModalGateAt1B8(std::int32_t value) = 0;
    virtual void SubmitControlRectAt50FC80(const RECT& rect) = 0;
    virtual void FlushDirtyRectsAt5267E0() = 0;
};

// SaveLoadDialog+0x168 的完整生产对象：派生资源图、GuiDialog 公共输入、
// 0x50CF50/0x52DBC0/0x508E80 状态机和 0x5071D0 runner 在此汇合。
class SaveLoadMessageBoxModalRuntime final
    : public ISaveLoadDeleteConfirmation,
      public IModalDialogRuntimeTarget,
      public IModalDialogRuntimeTargetReference,
      private ICgOperationButtonInputServices
{
public:
    SaveLoadMessageBoxModalRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        IGraphicsSurfaceCopyApi& copyGraphics,
        ISaveLoadMessageBoxInputServices& inputServices,
        ISaveLoadMessageBoxModalServices& modalServices,
        ModalDialogRunnerRuntime& runner);

    void WriteModeAt80(std::int32_t value) override;
    void PreparePageAt502950(std::int32_t pageNumber) override;
    void PreparePageAndSlotAt502B10(
        std::int32_t pageNumber,
        std::int32_t slotNumber) override;
    [[nodiscard]] std::int32_t RunAt5071D0() override;

    void BeginAtVslot44() override;
    [[nodiscard]] bool PollAtVslot48() override;
    [[nodiscard]] bool EndAtVslot4C() override;
    [[nodiscard]] std::int32_t ResultAtVslot50() const override;
    [[nodiscard]] IModalDialogRuntimeTarget* ResolveAt4A5330() override;

    void DrawAt5230B0(const RECT& clip);
    void CompositeAt5231C0(
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip);

    [[nodiscard]] const SaveLoadMessageBoxObjectGraphRuntime&
        ObjectGraph() const noexcept;
    [[nodiscard]] std::int32_t PhaseAt6C() const noexcept;
    [[nodiscard]] std::int32_t InputLatchAt30() const noexcept;
    [[nodiscard]] std::int32_t ActionFiveLatchAt70() const noexcept;
    [[nodiscard]] const CgOperationChildControlRef&
        ActiveControlAt4C() const noexcept;
    [[nodiscard]] const SaveLoadMessageBoxCapturedFrameRef&
        CapturedFrameAt5C() const noexcept;

private:
    void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) override;
    [[nodiscard]] bool ReadInputAtVslot6C(
        std::int32_t eventValue) override;
    [[nodiscard]] bool ReadInputAtVslot70(
        std::int32_t eventValue) override;
    [[nodiscard]] POINT ReadPointerAtVslot74() override;
    [[nodiscard]] bool ReadPressAtVslot80() override;
    [[nodiscard]] bool ReadReleaseAtVslot84() override;
    [[nodiscard]] std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) override;

    void SubmitActiveControlAt532170(CgOperationChildControlRef control);
    void ResetInputAt5106D0();
    void ResetChildrenAt522E90();
    [[nodiscard]] CgOperationChildControlRef ResolveChildAt509D20(
        std::int32_t eventValue) const;
    [[nodiscard]] bool TryActivateCandidateAt51E740(
        std::int32_t eventValue,
        bool resetChildrenFirst);
    [[nodiscard]] std::int32_t ProcessActionFiveAt510BA0();
    [[nodiscard]] std::int32_t ProcessUpperInputPathAt51E740();
    void ProcessGenericDialogInputAt518C30();

    CgOperationDialogRenderHostRuntime renderHost_;
    ISaveLoadMessageBoxInputServices* inputServices_ = nullptr;
    ISaveLoadMessageBoxModalServices* modalServices_ = nullptr;
    ModalDialogRunnerRuntime* runner_ = nullptr;
    SaveLoadMessageBoxObjectGraphRuntime objectGraph_;
    SaveLoadMessageBoxCapturedFrameRef capturedFrameAt5C_;
    std::int32_t phaseAt6C_ = 0;
    std::int32_t inputLatchAt30_ = 0;
    std::int32_t actionFiveLatchAt70_ = 0;
    CgOperationChildControlRef activeControlAt4C_;
};
}
