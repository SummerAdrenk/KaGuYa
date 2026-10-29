#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/SaveLoadDialogRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class SaveLoadDialogChildObjectGraphRuntime;

enum class SaveLoadPageKind : std::int32_t
{
    Save = 2,
    Load = 3,
    QuickLoad = 4,
    AutoLoad = 5,
};

// SaveLoadDialog(0x4EB950) 在创建 Page 之前装载的六组共享视觉资源。
// 数量、顺序和 CP932 文件名均直接来自 0x605FA0、0x6060D0、0x606110、
// 0x606050、0x606058 与 0x606060；任一资源失败都按原构造函数抛出整数 1。
class SaveLoadDialogVisualResourcesRuntime final
{
public:
    explicit SaveLoadDialogVisualResourcesRuntime(
        ICgOperationSurfaceFactory& factory);

    [[nodiscard]] const RuntimeGraphicsSurface& PageDigit(
        std::int32_t digit) const;
    [[nodiscard]] const RuntimeGraphicsSurface& FileNumber(
        std::int32_t slotIndex,
        bool saveStyle) const;
    [[nodiscard]] const RuntimeGraphicsSurface& DateSymbol(
        std::int32_t symbol,
        bool saveStyle) const;
    [[nodiscard]] const RuntimeGraphicsSurface& NoData(
        bool saveStyle) const;
    [[nodiscard]] const RuntimeGraphicsSurface& PreviewFallback(
        bool saveStyle) const;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        PreviewFallbackRef(bool saveStyle) const;
    [[nodiscard]] const RuntimeGraphicsSurface& NewMarker(
        bool saveStyle) const;

    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        PageDigitsAtF8() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        FileNumbersAt120() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        DateSymbolsAt12C() const noexcept;

private:
    friend class SaveLoadDialogChildObjectGraphRuntime;

    void ReleaseAt4F9030() noexcept;

    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageDigitsAtF8_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> fileNumbersAt120_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dateSymbolsAt12C_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> noDataAt138_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> previewFallbackAt144_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> newMarkerAt150_;
};

// 0x5339C0/0x5345A0/0x52A470 唯一仍位于 Page 外部的对象来源与副作用。
// 页码、槽遍历、日期拆分、资源选择、预览回退及控件可用性均由 Page 直接实现。
class ISaveLoadPageRefreshServices
{
public:
    virtual ~ISaveLoadPageRefreshServices() = default;

    [[nodiscard]] virtual const SaveFileCatalog&
        SaveFileCatalogAt4180C0() const = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateRegularFactoryAt4DCDB0(std::int32_t pageNumber) = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateQuickFactoryAt4DC5F0() = 0;
    [[nodiscard]] virtual SaveLoadFileFactoryOwner
        CreateAutoFactoryAt4DA870() = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface>
        CreatePreviewSurfaceAt40C2E0() = 0;
    [[nodiscard]] virtual std::string BuildPreviewPathAt4C7210(
        const std::string& fileName) = 0;
    [[nodiscard]] virtual bool TryLoadPreviewAt511D90(
        const std::shared_ptr<RuntimeGraphicsSurface>& destination,
        const std::string& path) = 0;
    virtual void DrawSceneLabelAt528D90(
        RuntimeGraphicsSurface& destination,
        const std::string& label,
        bool saveStyle) = 0;
    virtual void SubmitPageRectAtVslot64(const RECT& rect) = 0;
};

// SaveLoadPage 的 GuiDialog 公共链与页码持久化边界。0x52F8B0 只在进入
// Page 自身事件分派前调用公共更新；0x50EB50 则严格按“清 active control、
// 打开公共 Dialog、强制刷新当前页”的顺序执行。
class ISaveLoadPageInheritedServices : public ISaveLoadPageRefreshServices
{
public:
    virtual ~ISaveLoadPageInheritedServices() = default;

    virtual void UpdateGuiDialogAtVslot54() = 0;
    virtual void ClearActiveControlAtVslot30() = 0;
    virtual void OpenGuiDialogAtVslot3C() = 0;
    virtual void WriteRegularSelectedPageAt0C(std::int32_t value) = 0;
    virtual void WriteRegularDisplayedPageAt10(std::int32_t value) = 0;
};

// SaveLoadPage00(0x4ED190) 与 SaveLoadPage01(0x4EE390) 的构造期生产对象图。
// 构造对象图与 +0x8C 页面内容刷新均由本类型持有；完整 Dialog child 的其余
// 公共状态机仍在后续 owner 中接入，因此这里不提前冒充 ISaveLoadDialogChild。
class SaveLoadPageObjectGraphRuntime final
{
public:
    SaveLoadPageObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        ICgOperationButtonInputServices& buttonInputServices,
        const RECT& dialogBounds,
        SaveLoadPageKind kind,
        std::int32_t initialPage = 0);

    [[nodiscard]] SaveLoadPageKind Kind() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        EventChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
        ClickedEventManagerAt70() const noexcept;
    [[nodiscard]] const std::vector<GuiDialogNavigationGroupRef>&
        NavigationGroupsAt24() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        FullPageSurfaceAt80() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        DisplaySurfacesAt8C() const noexcept;
    [[nodiscard]] const RECT& CombinedRectAt60() const noexcept;
    [[nodiscard]] std::int32_t CurrentPageAt7C() const noexcept;
    void WriteCurrentPageAt7C(std::int32_t value) noexcept;

    // Page00:0x5339C0 与 Page01:0x5345A0。force 对应原第四参数是否非空；
    // submitDirty 对应原第三参数。
    void RefreshAtVslot8C(
        std::int32_t pageIndex,
        bool submitDirty,
        bool force,
        IGraphicsSurfaceCompositionApi& graphics,
        const SaveLoadDialogVisualResourcesRuntime& visuals,
        ISaveLoadPageRefreshServices& services);

private:
    void AddChild(const CgOperationChildControlRef& child);
    [[nodiscard]] std::shared_ptr<CgOperationChildControlRuntime>
        CreateButton(
            ICgOperationSurfaceFactory& surfaceFactory,
            ICgOperationButtonInputServices& buttonInputServices,
            const CgOperationChildDescriptor& descriptor);
    [[nodiscard]] CgOperationChildControlRef ChildByEventValue(
        std::int32_t eventValue) const;
    void DrawPageNumber(
        IGraphicsSurfaceCompositionApi& graphics,
        const SaveLoadDialogVisualResourcesRuntime& visuals,
        std::int32_t pageNumber,
        std::size_t coordinateBase);
    void DrawSlotAt52A470(
        std::int32_t slotIndex,
        const SaveFileCatalogItem& item,
        ISaveLoadFileFactory& factory,
        bool latest,
        IGraphicsSurfaceCompositionApi& graphics,
        const SaveLoadDialogVisualResourcesRuntime& visuals,
        ISaveLoadPageRefreshServices& services);

    SaveLoadPageKind kind_ = SaveLoadPageKind::Save;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<CgOperationChildControlRef> eventChildren_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime>
        clickedEventManagerAt70_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    std::shared_ptr<RuntimeGraphicsSurface> fullPageSurfaceAt80_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> displaySurfacesAt8C_;
    RECT combinedRectAt60_{};
    std::int32_t currentPageAt7C_ = 0;
};

// 0x4ED190/0x4EE390 构造出的完整 Page child。构造资源仍由上面的对象图
// 持有；本类型补齐 0x50EB50、0x52F8B0、0x509420/0x509490 与
// ISaveLoadDialogChild 所要求的生命周期，避免把尚未接线的构造对象直接
// 塞进 SaveLoadDialog 八槽容器。
class SaveLoadPageDialogChildRuntime final : public ISaveLoadDialogChild
{
public:
    SaveLoadPageDialogChildRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        ICgOperationButtonInputServices& buttonInputServices,
        const RECT& dialogBounds,
        SaveLoadPageKind kind,
        IGraphicsSurfaceCompositionApi& graphics,
        const SaveLoadDialogVisualResourcesRuntime& visuals,
        ISaveLoadPageInheritedServices& inheritedServices);

    [[nodiscard]] std::int32_t StateAtVslot20() const override;
    void WriteEnabledAtVslot24(std::int32_t value) override;
    [[nodiscard]] std::int32_t GateAtVslot28() const override;
    void UpdateAtVslot48() override;
    void EndAtVslot4C() override;
    [[nodiscard]] std::int32_t EventAtVslot50() const override;
    [[nodiscard]] SaveLoadDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) override;

    void OpenAtVslot44() override;
    void WriteStateAtVslot20(std::int32_t value) noexcept;
    void WriteCurrentPageAt7C(std::int32_t value) noexcept;
    void NotifySelectionAtVslot8C(
        std::int32_t previousValue,
        std::int32_t argument1,
        std::int32_t argument2);

    [[nodiscard]] SaveLoadPageObjectGraphRuntime& Graph() noexcept;
    [[nodiscard]] const SaveLoadPageObjectGraphRuntime& Graph() const noexcept;

private:
    [[nodiscard]] bool IsFileSlotAvailable(std::int32_t slotIndex) const;

    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    const SaveLoadDialogVisualResourcesRuntime* visuals_ = nullptr;
    ISaveLoadPageInheritedServices* inheritedServices_ = nullptr;
    SaveLoadPageObjectGraphRuntime graph_;
    std::int32_t stateAt34_ = 1;
    std::int32_t gateAt38_ = 1;
    bool eventAt5CInitialized_ = false;
    std::int32_t eventAt5C_ = -1;
};
}
