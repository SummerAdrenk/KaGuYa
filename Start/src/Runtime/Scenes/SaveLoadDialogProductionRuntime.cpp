#include <Runtime/SaveLoadDialogProductionRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
SaveLoadDialogSurfaceFrameRuntime::SaveLoadDialogSurfaceFrameRuntime(
    std::shared_ptr<RuntimeGraphicsSurface> surface)
    : surface_(std::move(surface))
{
    if (!surface_)
        throw std::invalid_argument("SaveLoad dialog frame surface is null");
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadDialogSurfaceFrameRuntime::SurfaceRef() const noexcept
{
    return surface_;
}

const RuntimeGraphicsSurface&
SaveLoadDialogSurfaceFrameRuntime::Surface() const noexcept
{
    return *surface_;
}

SaveLoadDialogProductionRuntime::SaveLoadDialogProductionRuntime(
    SaveLoadDialogProductionDependencies dependencies)
{
    dialogBoundsAt78_ =
        RECT{0, 0, dependencies.screenWidth, dependencies.screenHeight};

    // 0x4EB950：六组共享视觉资源严格早于 +0x94/+0xA0 Surface。
    childObjectGraph_ =
        std::make_unique<SaveLoadDialogChildObjectGraphRuntime>(
            SaveLoadDialogChildObjectGraphDependencies{
                dependencies.surfaceFactory,
                dependencies.graphics,
                dependencies.inputServices,
                dependencies.sliderInputServices,
                dependencies.pageServices,
                dependencies.systemServices,
                dependencies.popupServices,
                dialogBoundsAt78_});

    auto transitionSurfaceAt94 = dependencies.surfaceFactory.CreateBlank(
        dependencies.screenWidth,
        dependencies.screenHeight,
        false,
        true);
    if (!transitionSurfaceAt94)
        throw 1;
    transitionFrameAt94_ =
        std::make_shared<SaveLoadDialogSurfaceFrameRuntime>(
            std::move(transitionSurfaceAt94));

    auto backgroundSurfaceAtA0 =
        dependencies.surfaceFactory.LoadStatic("bg_black.bgd");
    if (!backgroundSurfaceAtA0)
        throw 1;
    backgroundFrameAtA0_ =
        std::make_shared<SaveLoadDialogSurfaceFrameRuntime>(
            std::move(backgroundSurfaceAtA0));

    // 0x525480 的截图关闭分支固定使用 +0x144 第一项“无地.vrm”。
    savePreviewFrameAt144_ =
        std::make_shared<SaveLoadDialogSurfaceFrameRuntime>(
            childObjectGraph_->VisualResources().PreviewFallbackRef(true));

    // 原构造顺序是 +0x174 槽确认框在先，+0x168 删除确认框在后。
    auto slotConfirmationAt174 =
        std::make_shared<SaveLoadSlotConfirmationModalRuntime>(
            dependencies.surfaceFactory,
            dependencies.graphics,
            dependencies.inputServices,
            dependencies.modalServices,
            dependencies.slotRenderServices,
            dependencies.modalRunner);
    auto deleteConfirmationAt168 =
        std::make_shared<SaveLoadMessageBoxModalRuntime>(
            dependencies.surfaceFactory,
            dependencies.graphics,
            dependencies.copyGraphics,
            dependencies.inputServices,
            dependencies.modalServices,
            dependencies.modalRunner);

    const std::int32_t initialPage =
        dependencies.mode == SaveLoadSceneMode::Save
            ? 0
            : static_cast<std::int32_t>(
                dependencies.pageServices.SaveFileCatalogAt4180C0()
                    .regularPageStepAt14);
    auto dialogRuntime = std::make_shared<SaveLoadDialogRuntime>(
        dependencies.dialogServices,
        *childObjectGraph_,
        initialPage,
        transitionFrameAt94_,
        backgroundFrameAtA0_,
        savePreviewFrameAt144_,
        std::move(deleteConfirmationAt168),
        std::move(slotConfirmationAt174),
        dialogBoundsAt78_);
    dialogRuntime_ = dialogRuntime.get();
    dialog_ = std::move(dialogRuntime);
}

SaveLoadDialogProductionRuntime::~SaveLoadDialogProductionRuntime()
{
    if (dialogRuntime_ != nullptr)
    {
        // 0x4F9030 横跨 Dialog 与拆出的资源 owner，必须分阶段复刻。
        dialogRuntime_->ReleaseConfirmationsAt4F9030();
        dialogRuntime_->ReleasePreviewReferenceAt4F9030();
    }
    savePreviewFrameAt144_.reset();

    if (childObjectGraph_)
        childObjectGraph_->ReleaseVisualResourcesAt4F9030();

    if (dialogRuntime_ != nullptr)
        dialogRuntime_->ReleaseSurfaceFramesAt4F9030();
    backgroundFrameAtA0_.reset();
    transitionFrameAt94_.reset();

    if (childObjectGraph_)
        childObjectGraph_->ReleaseChildrenAt4F6DC0();

    dialogRuntime_ = nullptr;
    dialog_.reset();
    childObjectGraph_.reset();
}

const SaveLoadDialogRef& SaveLoadDialogProductionRuntime::Dialog() const noexcept
{
    return dialog_;
}

SaveLoadDialogChildObjectGraphRuntime&
SaveLoadDialogProductionRuntime::ChildObjectGraph() noexcept
{
    return *childObjectGraph_;
}

const SaveLoadDialogChildObjectGraphRuntime&
SaveLoadDialogProductionRuntime::ChildObjectGraph() const noexcept
{
    return *childObjectGraph_;
}

const SaveLoadDialogFrameRef&
SaveLoadDialogProductionRuntime::TransitionFrameAt94() const noexcept
{
    return transitionFrameAt94_;
}

const SaveLoadDialogFrameRef&
SaveLoadDialogProductionRuntime::BackgroundFrameAtA0() const noexcept
{
    return backgroundFrameAtA0_;
}

const RECT& SaveLoadDialogProductionRuntime::DialogBoundsAt78() const noexcept
{
    return dialogBoundsAt78_;
}
}
