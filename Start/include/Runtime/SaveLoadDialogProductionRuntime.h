#pragma once

#include <Runtime/SaveLoadPageCoordinatorRuntime.h>
#include <Runtime/SaveLoadSlotConfirmationRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
// SaveLoadDialog 中以 CSurface 引用存在的 +0x94/+0xA0/+0x144 适配层。
// 它只建立强所有权和已证明的 Surface 身份，不复制像素或改变资源属性。
class SaveLoadDialogSurfaceFrameRuntime final : public ISaveLoadDialogFrame
{
public:
    explicit SaveLoadDialogSurfaceFrameRuntime(
        std::shared_ptr<RuntimeGraphicsSurface> surface);

    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        SurfaceRef() const noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface& Surface() const noexcept;

private:
    std::shared_ptr<RuntimeGraphicsSurface> surface_;
};

struct SaveLoadDialogProductionDependencies final
{
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceCompositionApi& graphics;
    IGraphicsSurfaceCopyApi& copyGraphics;
    ISaveLoadMessageBoxInputServices& inputServices;
    ICgOperationSliderInputServices& sliderInputServices;
    ISaveLoadPageInheritedServices& pageServices;
    ISaveLoadSystemInheritedServices& systemServices;
    ISaveLoadPopupInheritedServices& popupServices;
    ISaveLoadMessageBoxModalServices& modalServices;
    ISaveLoadSlotConfirmationRenderServices& slotRenderServices;
    ModalDialogRunnerRuntime& modalRunner;
    ISaveLoadDialogRuntimeServices& dialogServices;
    SaveLoadSceneMode mode = SaveLoadSceneMode::Load;
    std::int32_t screenWidth = 0;
    std::int32_t screenHeight = 0;
};

// GuiSaveLoadDialog::SaveLoadDialog(0x4EB950) 的完整生产强所有权边界。
// 八个派生 child 仍只在 Begin->0x5330A0 首次到达时按原顺序创建。
class SaveLoadDialogProductionRuntime final
{
public:
    explicit SaveLoadDialogProductionRuntime(
        SaveLoadDialogProductionDependencies dependencies);
    ~SaveLoadDialogProductionRuntime();

    SaveLoadDialogProductionRuntime(
        const SaveLoadDialogProductionRuntime&) = delete;
    SaveLoadDialogProductionRuntime& operator=(
        const SaveLoadDialogProductionRuntime&) = delete;

    [[nodiscard]] const SaveLoadDialogRef& Dialog() const noexcept;
    [[nodiscard]] SaveLoadDialogChildObjectGraphRuntime&
        ChildObjectGraph() noexcept;
    [[nodiscard]] const SaveLoadDialogChildObjectGraphRuntime&
        ChildObjectGraph() const noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef&
        TransitionFrameAt94() const noexcept;
    [[nodiscard]] const SaveLoadDialogFrameRef&
        BackgroundFrameAtA0() const noexcept;
    [[nodiscard]] const RECT& DialogBoundsAt78() const noexcept;

private:
    RECT dialogBoundsAt78_{};
    std::unique_ptr<SaveLoadDialogChildObjectGraphRuntime> childObjectGraph_;
    SaveLoadDialogFrameRef transitionFrameAt94_;
    SaveLoadDialogFrameRef backgroundFrameAtA0_;
    SaveLoadDialogFrameRef savePreviewFrameAt144_;
    SaveLoadDialogRuntime* dialogRuntime_ = nullptr;
    SaveLoadDialogRef dialog_;
};
}
