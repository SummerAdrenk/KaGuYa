#pragma once

#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/CgOperationWindowRuntime.h>

#include <Windows.h>

#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x4DB3D0/0x499C10 的强所有权边界：Buttons、Popup、Dialog、CGOpeWin
// 依次建立，外层只需持有最终 Window 即可保住整棵对象图。
class CgOperationObjectGraphRuntime final
{
public:
    CgOperationObjectGraphRuntime(
        LayerList* windowRegistrationLayer,
        ICgOperationButtonsServices& buttonServices,
        ICgOperationPopupServices& popupServices,
        ICgOperationDialogServices& dialogServices,
        ICgOperationWindowServices& windowServices,
        CgOperationWindowLayerRef windowLayer,
        IGraphicsSurfaceCompositionApi& graphics,
        std::shared_ptr<CgOperationButtonsSurfaceResourcesRuntime>
            buttonSurfaceResources,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dialogResources,
        CgOperationButtonsVisualLayout buttonVisualLayout,
        std::vector<CgOperationChildControlRef> buttonChildren,
        const RECT& buttonRect,
        std::vector<CgOperationChildControlRef> popupChildren,
        std::vector<CgOperationPopupPageResourceRef> popupPageResources,
        const RECT& popupRect,
        const RECT& dialogRect,
        std::shared_ptr<RuntimeGraphicsSurface> popupIndependentSurfaceAt7C,
        CgOperationPopupPageProducerRef popupPageProducer);

    [[nodiscard]] const std::shared_ptr<CgOperationWindowRuntime>&
        Window() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationDialogRuntime>&
        Dialog() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationButtonsRuntime>&
        Buttons() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationPopupRuntime>&
        Popup() const noexcept;

private:
    std::shared_ptr<CgOperationButtonsRuntime> buttons_;
    std::shared_ptr<CgOperationPopupRuntime> popup_;
    std::shared_ptr<CgOperationDialogRuntime> dialog_;
    std::shared_ptr<CgOperationWindowRuntime> window_;
};

// 0x4DB3D0/0x499C10 的生产装配 owner。测试可注入构造仍保留在上方，真实装配
// 统一从固定资源表创建控件，避免外部以同 ID 探针替代原 Static/Button 子树。
class CgOperationProductionObjectGraphRuntime final
{
public:
    CgOperationProductionObjectGraphRuntime(
        LayerList* windowRegistrationLayer,
        ICgOperationButtonsServices& buttonServices,
        ICgOperationPopupServices& popupServices,
        ICgOperationDialogServices& dialogServices,
        ICgOperationWindowServices& windowServices,
        CgOperationWindowLayerRef windowLayer,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationSurfaceFactory& surfaceFactory,
        CgOperationButtonsVisualLayout buttonVisualLayout,
        const RECT& buttonSurfaceTemplateRect);

    [[nodiscard]] const std::shared_ptr<CgOperationObjectGraphRuntime>&
        Graph() const noexcept;

private:
    std::shared_ptr<CgOperationObjectGraphRuntime> graph_;
};
}
