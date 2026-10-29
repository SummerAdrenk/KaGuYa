#include <Runtime/CgOperationObjectGraphRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
CgOperationObjectGraphRuntime::CgOperationObjectGraphRuntime(
    LayerList* const windowRegistrationLayer,
    ICgOperationButtonsServices& buttonServices,
    ICgOperationPopupServices& popupServices,
    ICgOperationDialogServices& dialogServices,
    ICgOperationWindowServices& windowServices,
    CgOperationWindowLayerRef windowLayer,
    IGraphicsSurfaceCompositionApi& graphics,
    std::shared_ptr<CgOperationButtonsSurfaceResourcesRuntime>
        buttonSurfaceResources,
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dialogResources,
    const CgOperationButtonsVisualLayout buttonVisualLayout,
    std::vector<CgOperationChildControlRef> buttonChildren,
    const RECT& buttonRect,
    std::vector<CgOperationChildControlRef> popupChildren,
    std::vector<CgOperationPopupPageResourceRef> popupPageResources,
    const RECT& popupRect,
    const RECT& dialogRect,
    std::shared_ptr<RuntimeGraphicsSurface> popupIndependentSurfaceAt7C,
    CgOperationPopupPageProducerRef popupPageProducer)
{
    if (!buttonSurfaceResources)
        throw std::invalid_argument("CGOperationButtons surface resources are null");
    // 4DB3D0 固定先创建 Buttons，再创建 Popup；499C10 随后把完整 Dialog
    // 放入 CGOpeWin。shared_ptr 链保持同一正向构造、反向释放关系。
    auto buttonVisuals = std::make_shared<CgOperationButtonsVisualRuntime>(
        graphics,
        buttonSurfaceResources->Target0At68(),
        buttonSurfaceResources->Target1At74(),
        std::move(dialogResources),
        buttonVisualLayout);
    auto buttonEventManager =
        CgOperationClickedEventManagerRuntime::CreateForButtonsAt4DAC80(
            buttonChildren);
    RECT completeButtonRect{};
    ::UnionRect(
        &completeButtonRect,
        &buttonRect,
        &buttonSurfaceResources->CombinedTargetRect());
    buttons_ = std::make_shared<CgOperationButtonsRuntime>(
        buttonServices,
        std::move(buttonEventManager),
        std::move(buttonVisuals),
        std::move(buttonChildren),
        completeButtonRect);
    auto popupEventManager =
        CgOperationClickedEventManagerRuntime::CreateForPopupAt4DB860(
            popupChildren);
    popup_ = std::make_shared<CgOperationPopupRuntime>(
        popupServices,
        std::move(popupEventManager),
        std::move(popupChildren),
        std::move(popupPageResources),
        popupRect,
        std::move(popupIndependentSurfaceAt7C),
        std::move(popupPageProducer));
    dialog_ = std::make_shared<CgOperationDialogRuntime>(
        dialogServices,
        buttons_,
        popup_,
        dialogRect);
    window_ = std::make_shared<CgOperationWindowRuntime>(
        windowRegistrationLayer,
        windowServices,
        std::move(windowLayer),
        dialog_,
        graphics);
}

const std::shared_ptr<CgOperationWindowRuntime>&
CgOperationObjectGraphRuntime::Window() const noexcept
{
    return window_;
}

const std::shared_ptr<CgOperationDialogRuntime>&
CgOperationObjectGraphRuntime::Dialog() const noexcept
{
    return dialog_;
}

const std::shared_ptr<CgOperationButtonsRuntime>&
CgOperationObjectGraphRuntime::Buttons() const noexcept
{
    return buttons_;
}

const std::shared_ptr<CgOperationPopupRuntime>&
CgOperationObjectGraphRuntime::Popup() const noexcept
{
    return popup_;
}

namespace
{
[[nodiscard]] RECT ConcreteChildrenRect(
    const std::vector<CgOperationChildControlRef>& children)
{
    RECT result{};
    for (const auto& child : children)
    {
        const auto concrete =
            std::dynamic_pointer_cast<CgOperationChildControlRuntime>(child);
        if (!concrete)
        {
            throw std::invalid_argument(
                "CGOperation production child is not a concrete Static/Button");
        }
        RECT combined{};
        const RECT& childRect = concrete->RectAtVslot3C();
        ::UnionRect(&combined, &result, &childRect);
        result = combined;
    }
    return result;
}
}

CgOperationProductionObjectGraphRuntime::
CgOperationProductionObjectGraphRuntime(
    LayerList* const windowRegistrationLayer,
    ICgOperationButtonsServices& buttonServices,
    ICgOperationPopupServices& popupServices,
    ICgOperationDialogServices& dialogServices,
    ICgOperationWindowServices& windowServices,
    CgOperationWindowLayerRef windowLayer,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationSurfaceFactory& surfaceFactory,
    const CgOperationButtonsVisualLayout buttonVisualLayout,
    const RECT& buttonSurfaceTemplateRect)
{
    // 原 4DB3D0 先装载 Dialog 的 20 张数字资源，之后才进入 Buttons/Popup。
    auto dialogVisualResources =
        std::make_shared<CgOperationDialogVisualResourcesRuntime>(surfaceFactory);

    auto buttonChildren =
        CreateCgOperationButtonsChildrenAt608440(surfaceFactory);
    const RECT buttonChildRect = ConcreteChildrenRect(buttonChildren);
    auto buttonSurfaceResources =
        std::make_shared<CgOperationButtonsSurfaceResourcesRuntime>(
            surfaceFactory,
            buttonSurfaceTemplateRect,
            buttonVisualLayout);

    // Popup owner 内部继续保持 Static 8、初始页、Static 9、独立 Surface 的顺序。
    auto popupChildTree = std::make_shared<CgOperationPopupChildTreeRuntime>(
        surfaceFactory, graphics);
    auto popupPageProducer =
        std::make_shared<CgOperationPopupPageProducerRuntime>(
            surfaceFactory,
            graphics,
            std::vector<std::shared_ptr<RuntimeGraphicsSurface>>(
                dialogVisualResources->Resources().begin(),
                dialogVisualResources->Resources().end()));

    RECT completeButtonRect{};
    const RECT& buttonTargets =
        buttonSurfaceResources->CombinedTargetRect();
    ::UnionRect(&completeButtonRect, &buttonChildRect, &buttonTargets);
    RECT dialogRect{};
    const RECT& popupRect = popupChildTree->CombinedChildRectAt68();
    ::UnionRect(&dialogRect, &completeButtonRect, &popupRect);

    graph_ = std::make_shared<CgOperationObjectGraphRuntime>(
        windowRegistrationLayer,
        buttonServices,
        popupServices,
        dialogServices,
        windowServices,
        std::move(windowLayer),
        graphics,
        buttonSurfaceResources,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>>(
            dialogVisualResources->Resources().begin(),
            dialogVisualResources->Resources().end()),
        buttonVisualLayout,
        std::move(buttonChildren),
        buttonChildRect,
        popupChildTree->ChildrenInInsertionOrder(),
        popupChildTree->PageResourcesAt88(),
        popupRect,
        dialogRect,
        popupChildTree->IndependentSurfaceAt7C(),
        std::move(popupPageProducer));
}

const std::shared_ptr<CgOperationObjectGraphRuntime>&
CgOperationProductionObjectGraphRuntime::Graph() const noexcept
{
    return graph_;
}

}
