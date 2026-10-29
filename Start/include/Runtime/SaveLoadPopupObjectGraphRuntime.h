#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/SaveLoadDialogRuntime.h>

#include <Windows.h>

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ISaveLoadPopupInheritedServices
{
public:
    virtual ~ISaveLoadPopupInheritedServices() = default;

    [[nodiscard]] virtual bool ReadCancelRequestAt52FDA0() = 0;
    virtual void UpdateGuiDialogAtVslot54() = 0;
    virtual void OpenGuiDialogAtVslot3C() = 0;
    [[nodiscard]] virtual std::int32_t
        ReadPageStepDirectionAtVslot78() = 0;
    [[nodiscard]] virtual std::int32_t
        ReadFallbackPageAt53DDE0Vslot60() = 0;
    [[nodiscard]] virtual const SaveFileCatalog&
        SaveFileCatalogAt4180C0() const = 0;
    virtual void SubmitDirtyRectAtVslot64(const RECT& rect) = 0;
};

struct SaveLoadPopupNumberRuntime final
{
    std::int32_t pageValueAt00 = 0;
    POINT originAt08{};
    std::array<std::shared_ptr<RuntimeGraphicsSurface>, 3> surfacesAt10{};
};

// GuiSaveLoadDialog::SaveLoadPopup，构造入口 0x4EF660。styleSlot 只能取
// 原调用点证明的 6（Save）或 7（Load）；两套资源不得互相回退。
class SaveLoadPopupObjectGraphRuntime final : public ISaveLoadDialogChild
{
public:
    SaveLoadPopupObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationButtonInputServices& buttonInputServices,
        ICgOperationSliderInputServices& sliderInputServices,
        ISaveLoadPopupInheritedServices& inheritedServices,
        std::int32_t styleSlot);

    [[nodiscard]] std::int32_t StateAtVslot20() const override;
    void WriteEnabledAtVslot24(std::int32_t value) override;
    [[nodiscard]] std::int32_t GateAtVslot28() const override;
    void UpdateAtVslot48() override;
    void EndAtVslot4C() override;
    [[nodiscard]] std::int32_t EventAtVslot50() const override;
    [[nodiscard]] SaveLoadDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) override;

    void OpenAtVslot44() override;
    void RefreshAt52B390(std::int32_t firstPage, bool submitDirty);
    void DrawAt524A90(const RECT* destinationClip);
    void CompositeAt5249C0(
        GraphicsSurfaceHandle destination,
        const RECT* destinationClip);

    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        EventChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<SaveLoadPopupNumberRuntime>&
        PopupNumbersAtA0() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        DigitResourcesAt88() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        MarkerResourcesAt94() const noexcept;
    [[nodiscard]] const std::vector<GuiDialogNavigationGroupRef>&
        NavigationGroupsAt24() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
        ClickedEventManagerAt70() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationSliderRuntime>&
        SliderAt57() const noexcept;
    [[nodiscard]] const RECT& CombinedRectAt60() const noexcept;
    [[nodiscard]] std::int32_t SelectedPageAt7C() const noexcept;
    [[nodiscard]] std::int32_t DisplayedFirstPageAt80() const noexcept;
    [[nodiscard]] std::int32_t PendingSelectedPageAt84() const noexcept;
    void WriteSelectedPageAt7C(std::int32_t value) noexcept;
    void WriteStateAtVslot20(std::int32_t value) noexcept;

private:
    void AddChild(const CgOperationChildControlRef& child);
    [[nodiscard]] CgOperationChildControlRef ChildByEventValue(
        std::int32_t event) const;
    [[nodiscard]] std::shared_ptr<CgOperationChildControlRuntime>
        ConcreteChildByEventValue(std::int32_t event) const;
    void DrawWhole(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source,
        const POINT& point);

    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    ISaveLoadPopupInheritedServices* inheritedServices_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<CgOperationChildControlRef> eventChildren_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime>
        clickedEventManagerAt70_;
    std::shared_ptr<CgOperationSliderRuntime> sliderAt57_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digitResourcesAt88_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> markerResourcesAt94_;
    std::vector<SaveLoadPopupNumberRuntime> popupNumbersAtA0_;
    RECT combinedRectAt60_{};
    std::int32_t stateAt34_ = 1;
    std::int32_t gateAt38_ = 1;
    bool eventAt5CInitialized_ = false;
    std::int32_t eventAt5C_ = -1;
    std::int32_t selectedPageAt7C_ = 0;
    std::int32_t displayedFirstPageAt80_ = -1;
    std::int32_t pendingSelectedPageAt84_ = 0;
    std::int32_t valueAtB0_ = -1;
};
}
