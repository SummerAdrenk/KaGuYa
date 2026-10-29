#pragma once

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <Windows.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ICgOperationSurfaceBatch
{
public:
    virtual ~ICgOperationSurfaceBatch() = default;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface> LoadStatic(
        const std::string& resourceNameCp932) = 0;
};

class ICgOperationSurfaceFactory
{
public:
    virtual ~ICgOperationSurfaceFactory() = default;

    // 480930：一次调用的七槽共用一个批次；返回非空对象，依赖须活过批次。
    [[nodiscard]] virtual std::unique_ptr<ICgOperationSurfaceBatch> CreateBatch() = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface> LoadStatic(
        const std::string& resourceNameCp932) = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface> CreateBlank(
        std::int32_t width,
        std::int32_t height,
        bool hasAlpha,
        bool clearOnCreate) = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface>
        CreateCompatible(
            const RuntimeGraphicsSurface& source,
            bool copyPixels) = 0;
};

struct CgOperationControlResourceDescriptor final
{
    std::array<std::string, 7> resourceNamesAt04{};
    std::optional<POINT> absoluteOriginAt20;
};

// Start.exe:5047A0。七个槽先按 CP932 原始字节去重，再装载唯一 Surface；
// 各槽保存唯一资源索引，联合 RECT 与可选平移严格在同一资源组内生效。
class CgOperationControlResourceGroupRuntime final
{
public:
    CgOperationControlResourceGroupRuntime(
        ICgOperationSurfaceFactory& factory,
        const CgOperationControlResourceDescriptor& descriptor);
    // 502630复制唯一Surface，保留槽索引、空复制结果与联合矩形。
    CgOperationControlResourceGroupRuntime(
        ICgOperationSurfaceFactory& factory,
        const CgOperationControlResourceGroupRuntime& source);
    CgOperationControlResourceGroupRuntime(
        std::shared_ptr<RuntimeGraphicsSurface> surface,
        std::size_t slot);

    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        ResourceForSlot(std::size_t slot) const;
    [[nodiscard]] std::int32_t ResourceIndexForSlot(
        std::size_t slot) const;
    [[nodiscard]] const RECT& CombinedRect() const noexcept;
    // 4E06DF/4E0714：CheckBox 将 union 同时写回 Off/On，不改变图片原点。
    void WriteCombinedRectForConstruction(const RECT& rect) noexcept { combinedRect_ = rect; }
    [[nodiscard]] std::size_t UniqueResourceCount() const noexcept;

private:
    std::array<std::int32_t, 7> slotIndices_{};
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> uniqueResources_;
    RECT combinedRect_{};
};

struct CgOperationChildDescriptor final
{
    CgOperationControlKind kind = CgOperationControlKind::Static;
    std::int32_t eventValueAt04 = 0;
    CgOperationControlResourceDescriptor resources;
    std::int32_t initialResourceSlotAt24 = 7;
    std::int32_t childIdAt0C = -1;
};

// Start.exe:5052C0 的 108-byte Slider 描述记录。字段顺序按原表保留，
// sharedOriginAt3C 会在创建两组 SurfaceGroup 后分别应用到两组资源。
struct CgOperationSliderDescriptor final
{
    std::int32_t eventValueAt00 = 0;
    std::array<CgOperationControlResourceDescriptor, 2> resourcesAt04And20;
    std::optional<POINT> sharedOriginAt3C;
    std::int32_t hitTestResourceSlotAt40 = 7;
    std::int32_t orientationAt44 = 0;
    std::int32_t reverseAt48 = 0;
    RECT rectAt4C{};
    RECT offsetRectAt5C{};
};

[[nodiscard]] std::shared_ptr<CgOperationSliderRuntime>
CreateCgOperationSliderAt5052C0(
    ICgOperationSurfaceFactory& factory,
    ICgOperationSliderInputServices& inputServices,
    const CgOperationSliderDescriptor& descriptor);

// 样本 18 的 OpacityDialog 固定表，逐 DWORD 对应原 PE 的
// 0x606FE4（Static）与 0x607008（Slider）。
[[nodiscard]] const CgOperationChildDescriptor&
OpacityDialogStaticDescriptorSample18At606FE4();
[[nodiscard]] const CgOperationSliderDescriptor&
OpacityDialogSliderDescriptorSample18At607008();

[[nodiscard]] const std::array<CgOperationChildDescriptor, 6>&
CgOperationButtonsChildDescriptorsAt604240();

// 0x608440 的 type/count/records 顺序：Static 2，随后 Button 3..7。
[[nodiscard]] std::vector<CgOperationChildControlRef>
CreateCgOperationButtonsChildrenAt608440(
    ICgOperationSurfaceFactory& factory);

class CgOperationPopupPageSurfaceRuntime;

// 0x4DB860 中从 Static 8 到 Button 11 的完整子树。该 owner 保留初始页、
// Static 9 共用页、独立数值按钮 Surface 与 24 个控件之间的强所有权关系。
class CgOperationPopupChildTreeRuntime final
{
public:
    CgOperationPopupChildTreeRuntime(
        ICgOperationSurfaceFactory& factory,
        IGraphicsSurfaceCompositionApi& graphics);

    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        IndependentSurfaceAt7C() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationPopupPageSurfaceRuntime>&
        InitialPage() const noexcept;
    [[nodiscard]] const std::vector<CgOperationPopupPageResourceRef>&
        PageResourcesAt88() const noexcept;
    [[nodiscard]] const RECT& CombinedChildRectAt68() const noexcept;

private:
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<RuntimeGraphicsSurface> independentSurfaceAt7C_;
    std::shared_ptr<CgOperationPopupPageSurfaceRuntime> initialPage_;
    std::vector<CgOperationPopupPageResourceRef> pageResourcesAt88_;
    RECT combinedChildRectAt68_{};
};

// 0x480000/0x4800A0 使用的 archive 静态 Surface 装载边界。
class CgOperationArchiveSurfaceFactory final
    : public ICgOperationSurfaceFactory
{
public:
    CgOperationArchiveSurfaceFactory(
        const ResourceCatalog& catalog,
        const FileTransactionContext* transactionContext,
        IStartFileApi& fileApi,
        const IEnvironmentKeyTableSource& environmentKeyBytes,
        IGraphicsSurfaceApi& graphicsApi, ArchiveCodecServices codecs) noexcept;

    [[nodiscard]] std::unique_ptr<ICgOperationSurfaceBatch> CreateBatch() override;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> LoadStatic(
        const std::string& resourceNameCp932) override;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> CreateBlank(
        std::int32_t width,
        std::int32_t height,
        bool hasAlpha,
        bool clearOnCreate) override;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> CreateCompatible(
        const RuntimeGraphicsSurface& source,
        bool copyPixels) override;

private:
    const ResourceCatalog* catalog_ = nullptr;
    const FileTransactionContext* transactionContext_ = nullptr;
    IStartFileApi* fileApi_ = nullptr;
    const IEnvironmentKeyTableSource* environmentKeyBytes_ = nullptr;
    IGraphicsSurfaceApi* graphicsApi_ = nullptr;
    ArchiveCodecServices codecs_;
};

[[nodiscard]] const std::array<std::string, 20>&
CgOperationDialogVisualResourceNamesAt608480();

// 0x4DB3D0 在创建 Buttons/Popup 之前装载的 20 张固定视觉资源。
class CgOperationDialogVisualResourcesRuntime final
{
public:
    explicit CgOperationDialogVisualResourcesRuntime(
        ICgOperationSurfaceFactory& factory);

    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        Resources() const noexcept;
    [[nodiscard]] const RECT& CombinedRect() const noexcept;

private:
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> resources_;
    RECT combinedRect_{};
};

// CGOperationButtons::CGOperationButtons(0x4DAC80) 创建的两张同规格目标 Surface。
class CgOperationButtonsSurfaceResourcesRuntime final
{
public:
    CgOperationButtonsSurfaceResourcesRuntime(
        ICgOperationSurfaceFactory& factory,
        const RECT& templateRect,
        const CgOperationButtonsVisualLayout& layout);

    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        Target0At68() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        Target1At74() const noexcept;
    [[nodiscard]] const RECT& CombinedTargetRect() const noexcept;

private:
    std::shared_ptr<RuntimeGraphicsSurface> target0At68_;
    std::shared_ptr<RuntimeGraphicsSurface> target1At74_;
    RECT combinedTargetRect_{};
};

[[nodiscard]] const std::string&
CgOperationPopupIndependentResourceNameAt5B9100();

// CSurfaceGroup::Clear(0x4143A0) 只把给定 RECT 指针转发给 SurfaceClear。
class CgOperationPopupPageSurfaceRuntime final
    : public ICgOperationPopupPageResource
{
public:
    CgOperationPopupPageSurfaceRuntime(
        IGraphicsSurfaceCompositionApi& graphics,
        std::shared_ptr<RuntimeGraphicsSurface> surface,
        const RECT& layoutRect);

    void ClearAt4143A0(const RECT* rect) override;

    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        Surface() const noexcept;
    [[nodiscard]] const RECT& LayoutRect() const noexcept;

private:
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    std::shared_ptr<RuntimeGraphicsSurface> surface_;
    RECT layoutRect_{};
};

// CGOperationPopup::4FEAE0。每页固定 20 项；新页只由第 0 页按
// SurfaceCreate 同规格创建，随后复制原点，再按 0x6084E8 的坐标表绘制。
class CgOperationPopupPageProducerRuntime final
    : public ICgOperationPopupPageProducer
{
public:
    CgOperationPopupPageProducerRuntime(
        ICgOperationSurfaceFactory& factory,
        IGraphicsSurfaceCompositionApi& graphics,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digitResources);

    void DrawSelectionAt4FEAE0(
        std::vector<CgOperationPopupPageResourceRef>& pageResources,
        const std::shared_ptr<RuntimeGraphicsSurface>& independentSurfaceAt7C,
        std::int32_t itemIndex,
        std::int32_t available) override;

private:
    [[nodiscard]] const RuntimeGraphicsSurface& DigitResourceAt50A7D0(
        std::int32_t digit,
        std::int32_t available) const;

    ICgOperationSurfaceFactory* factory_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> digitResources_;
};

// CGOperationPopup::CGOperationPopup(0x4DB860) 中独立 Surface 与初始页的强所有权边界。
class CgOperationPopupSurfaceResourcesRuntime final
{
public:
    CgOperationPopupSurfaceResourcesRuntime(
        ICgOperationSurfaceFactory& factory,
        IGraphicsSurfaceCompositionApi& graphics,
        const RECT& static8Rect);

    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        IndependentSurfaceAt7C() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationPopupPageSurfaceRuntime>&
        InitialPage() const noexcept;
    [[nodiscard]] const std::vector<CgOperationPopupPageResourceRef>&
        PageResourcesAt88() const noexcept;

private:
    // 声明顺序与原析构相反：页 vector 先析构，独立 Surface 后析构。
    std::shared_ptr<RuntimeGraphicsSurface> independentSurfaceAt7C_;
    std::shared_ptr<CgOperationPopupPageSurfaceRuntime> initialPage_;
    std::vector<CgOperationPopupPageResourceRef> pageResourcesAt88_;
};
}
