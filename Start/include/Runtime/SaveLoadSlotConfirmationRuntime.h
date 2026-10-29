#pragma once

#include <Runtime/SaveLoadMessageBoxObjectGraphRuntime.h>

#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x526DA0 依赖的外部绘制边界。预览帧和捕获帧在 SaveLoadDialog 中
// 仍是独立对象，不能把它们强制转换成 RuntimeGraphicsSurface。
class ISaveLoadSlotConfirmationRenderServices
{
public:
    virtual ~ISaveLoadSlotConfirmationRenderServices() = default;

    virtual void DrawSceneLabelAt53ACA0(
        GraphicsSurfaceHandle destination,
        const std::string& sceneLabelCp932,
        std::int32_t x,
        std::int32_t y,
        std::int32_t height,
        std::int32_t width,
        std::int32_t valueAt18,
        std::uint8_t valueAt20,
        std::uint32_t color) = 0;
    [[nodiscard]] virtual std::span<const RECT>
        ReadDirtyRectsAtVslot00Then18And1C() = 0;
    virtual void FinishDirtyRectsAtVslot00() = 0;
    virtual void DrawCapturedFrameAtSurfaceDraw(
        const SaveLoadMessageBoxCapturedFrameRef& frame,
        const RECT& clip) = 0;
    virtual void DrawPreviewFrameAt441980(
        const SaveLoadDialogFrameRef& frame,
        std::int32_t x,
        std::int32_t y,
        const RECT& clip) = 0;
    virtual void InvalidatedRectAt526DA0(const RECT& rect) = 0;
};

// GuiModalSaveLoadConfirm::SaveLoadConfirmDialog(0x4EACA0) 的派生对象图。
// 四组资源表保留原始空槽，避免 file/Lfile 两组索引发生位移。
class SaveLoadSlotConfirmationObjectGraphRuntime final
{
public:
    SaveLoadSlotConfirmationObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationButtonInputServices& buttonInputServices,
        ISaveLoadSlotConfirmationRenderServices& renderServices);

    void WriteModeAt80(std::int32_t value) noexcept;
    void ApplyModeChildEnablementAt50E7E0();
    void ConfigureAt535FF0(
        const std::string& pageLabel,
        const std::string& slotLabel,
        const std::string& timestampLabel,
        const std::string& sceneLabel,
        bool isSaveMode);
    void AssignPreviewFrameAt538430(const SaveLoadDialogFrameRef& frame);
    [[nodiscard]] std::int32_t PollClickedResultAt525300() noexcept;

    [[nodiscard]] std::int32_t ModeAt80() const noexcept;
    [[nodiscard]] std::int32_t ResultAt68() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<CgOperationClickedEventRef>&
        ClickedEventsInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<GuiDialogNavigationGroupRef>&
        NavigationGroupsAt24() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        OverlaySurfaceAt84() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        CompositeSurfaceAt90() const noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef&
        PreviewFrameAt9C() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        SlotDigitResourcesAtA8() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        PageDigitResourcesAtB4() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        DateResourcesAtC0() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        TimeResourcesAtCC() const noexcept;
    [[nodiscard]] std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
        QuickAutoResourcesAtD8() const noexcept;

private:
    void DrawPageLabelAt508110(const std::string& value, bool isSaveMode);
    void DrawSlotLabelAt507EB0(const std::string& value, bool isSaveMode);
    void DrawTimestampAt5075C0(const std::string& value, bool isSaveMode);
    void CopyResourceAt(
        const std::shared_ptr<RuntimeGraphicsSurface>& source,
        std::int32_t x,
        std::int32_t y);

    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    ISaveLoadSlotConfirmationRenderServices* renderServices_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    std::vector<CgOperationClickedEventRef> clickedEventsAt74_;
    std::int32_t resultAt68_ = 0;
    std::int32_t modeAt80_ = 0;
    std::shared_ptr<RuntimeGraphicsSurface> overlaySurfaceAt84_;
    std::shared_ptr<RuntimeGraphicsSurface> compositeSurfaceAt90_;
    SaveLoadDialogFrameRef previewFrameAt9C_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> slotDigitsAtA8_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageDigitsAtB4_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dateResourcesAtC0_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> timeResourcesAtCC_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> quickAutoAtD8_;
};

// SaveLoadDialog+0x174 的完整生产确认框。它复用公共 modal 协议，
// 但保留 0x526DA0 的派生绘制和 0x5093D0 的预览释放顺序。
class SaveLoadSlotConfirmationModalRuntime final
    : public ISaveLoadSlotConfirmation,
      public IModalDialogRuntimeTarget,
      public IModalDialogRuntimeTargetReference,
      private ICgOperationButtonInputServices
{
public:
    SaveLoadSlotConfirmationModalRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ISaveLoadMessageBoxInputServices& inputServices,
        ISaveLoadMessageBoxModalServices& modalServices,
        ISaveLoadSlotConfirmationRenderServices& renderServices,
        ModalDialogRunnerRuntime& runner);

    void WriteModeAtVslot98(std::int32_t value) override;
    void ConfigureAt535FF0(
        const std::string& pageLabel,
        const std::string& slotLabel,
        const std::string& timestampLabel,
        const std::string& sceneLabel,
        bool isSaveMode) override;
    void AssignPreviewFrameAt538430(
        const SaveLoadDialogFrameRef& frame) override;
    [[nodiscard]] std::int32_t RunAt5071D0() override;

    void BeginAtVslot44() override;
    [[nodiscard]] bool PollAtVslot48() override;
    [[nodiscard]] bool EndAtVslot4C() override;
    [[nodiscard]] std::int32_t ResultAtVslot50() const override;
    [[nodiscard]] IModalDialogRuntimeTarget* ResolveAt4A5330() override;

    void DrawAt526DA0();

    [[nodiscard]] const SaveLoadSlotConfirmationObjectGraphRuntime&
        ObjectGraph() const noexcept;
    [[nodiscard]] std::int32_t PhaseAt6C() const noexcept;
    [[nodiscard]] const SaveLoadMessageBoxCapturedFrameRef&
        CapturedFrameAt5C() const noexcept;

private:
    void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) override;
    [[nodiscard]] bool ReadInputAtVslot6C(std::int32_t eventValue) override;
    [[nodiscard]] bool ReadInputAtVslot70(std::int32_t eventValue) override;
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
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    ISaveLoadMessageBoxInputServices* inputServices_ = nullptr;
    ISaveLoadMessageBoxModalServices* modalServices_ = nullptr;
    ISaveLoadSlotConfirmationRenderServices* renderServices_ = nullptr;
    ModalDialogRunnerRuntime* runner_ = nullptr;
    SaveLoadSlotConfirmationObjectGraphRuntime objectGraph_;
    SaveLoadMessageBoxCapturedFrameRef capturedFrameAt5C_;
    std::int32_t phaseAt6C_ = 0;
    std::int32_t inputLatchAt30_ = 0;
    std::int32_t actionFiveLatchAt70_ = 0;
    CgOperationChildControlRef activeControlAt4C_;
};
}
