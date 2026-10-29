#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/PatternResourceExecutor.h>
#include <Runtime/PatternPixelFilterRuntime.h>
#include <Runtime/PatternFilterResourceRuntime.h>
#include <Runtime/ScrResultLayerApply.h>
#include <Runtime/SpriteRuntime.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 4CB030 调用 480650/4B0840 时使用的强类型资源边界。
// 栈 action 跨越 factory；服务不预取 Sprite，不统一翻译正文 false/异常。
class IPatternLayerResourceLoader
{
public:
    virtual ~IPatternLayerResourceLoader() = default;

    virtual void ExecuteLoad(const PatternResourceLoadRequest& request,
        const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
        IRuntimeStringManagerSource& strings) = 0;
};

// 对应 Graphics::CPatternLayer 的资源状态子集。主画布与 Sprite 槽均由对象自身持有。
class PatternLayerRuntime final
    : public ILayerWindowPatternLayer,
      public IPatternResourceExecutionTarget,
      public IScrResultPatternLayer
{
public:
    PatternLayerRuntime(
        LayerList* internalLayerList,
        PatternLayerScreenSize screenSize,
        bool surfaceHasAlpha,
        std::uint8_t spriteSlotCount,
        IGraphicsSurfaceApi& graphicsApi,
        IGraphicsSurfaceCompositionApi& composition,
        IGraphicsWindowSurfaceApi& drawing,
        IPatternLayerResourceLoader& resourceLoader,
        IGraphicsPatternFilterApi& filters,
        IPatternFilterResourceLoader& filterResources);
    ~PatternLayerRuntime() override = default;

    PatternLayerRuntime(const PatternLayerRuntime&) = delete;
    PatternLayerRuntime& operator=(const PatternLayerRuntime&) = delete;

    void Draw(const RECT& dirtyRect) override;
    void DrawAtOffsetAt482C00(std::int32_t x, std::int32_t y, const RECT& dirtyRect) override;
    void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip) override;
    void RefreshMainAndPropagateAt443B00();
    void AdvanceFrameAt4423C0() override;
    void SetMainClearColor(std::uint32_t color) noexcept;
    // 47EC71..47ED3E：重置单个 Pattern，成功后才隐藏，不提交父 dirty。
    void ResetForRootAt47EC71(IGraphicsWindowSurfaceApi& drawing);

    [[nodiscard]] const PatternLayerScreenSize& ScreenSize() const noexcept;
    [[nodiscard]] bool SurfaceHasAlpha() const noexcept;
    [[nodiscard]] GraphicsSurfaceHandle SurfaceHandle() const noexcept;
    // 4AE39F / 4AE574：FreeScroll 复制实际画布尺寸、alpha 与 processed 来源。
    [[nodiscard]] const RuntimeGraphicsSurface& MainSurfaceAt18() const noexcept { return surface_; }
    [[nodiscard]] const RuntimeGraphicsSurface* ProcessedSurfaceAt64() const noexcept { return processedSurface_.get(); }
    // 46F268..46F281：局部 Filter 复制 +54 强引用，随后装载可能修改同一 Surface。
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> CopyTemporaryFilterSurfaceAt54() const noexcept
        { return temporaryFilterSurface_; }
    [[nodiscard]] SpriteRuntime& SpriteAt(std::size_t index);
    [[nodiscard]] const SpriteRuntime& SpriteAt(std::size_t index) const;
    [[nodiscard]] DirtyRectCollection& DirtyRects() noexcept;
    [[nodiscard]] const DirtyRectCollection& DirtyRects() const noexcept;
    void SetLayerPosition(std::int32_t x, std::int32_t y) noexcept;
    [[nodiscard]] bool FilterRefreshPending() const noexcept;
    [[nodiscard]] std::uint8_t ResultValueAt10() const noexcept;
    [[nodiscard]] bool HasPatternItem() const noexcept;

    [[nodiscard]] std::size_t SpriteSlotCount() const noexcept override;
    [[nodiscard]] std::int32_t LayerX() const noexcept override;
    [[nodiscard]] std::int32_t LayerY() const noexcept override;
    [[nodiscard]] const char* LogicalNameCharacters(
        std::size_t spriteSlotIndex) const override;
    [[nodiscard]] std::int32_t LogicalNameLength(
        std::size_t spriteSlotIndex) const override;
    [[nodiscard]] PatternSpriteRectResult LocalDrawableRect(
        std::size_t spriteSlotIndex) const override;
    void ClearSlot(std::size_t spriteSlotIndex) override;
    [[nodiscard]] PatternSpriteReceiver CaptureSpriteReceiver(std::uint32_t slotIndex) const override;
    void LoadResource(std::uint32_t slotIndex, PatternSpriteReceiver capturedReceiver,
        const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
        IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings) override;
    [[nodiscard]] const char* ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const override;
    [[nodiscard]] PatternResourceNamesBorrow ReceiverAuxiliaryNames(
        PatternSpriteReceiver receiver) const override;
    void PrepareReceiverFileGroup(PatternSpriteReceiver receiver, FileGroupChange changes) override;
    void SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source) override;
    void SetReceiverOrigin(PatternSpriteReceiver receiver, std::int32_t x, std::int32_t y) override;
    void SetReceiverActive(PatternSpriteReceiver receiver, bool active) override;
    [[nodiscard]] PatternSpriteRectResult ReceiverDrawableRect(PatternSpriteReceiver receiver) const override;

    [[nodiscard]] std::int32_t CurrentPatternItemIndex() const noexcept override;
    [[nodiscard]] bool DrawingEnabled() const noexcept override;
    [[nodiscard]] RECT ComputePatternUnionRect() const override;
    void ClearAllPatternItems() override;
    void SetCurrentPatternItemIndex(std::int32_t index) noexcept override;
    [[nodiscard]] std::int32_t FilterMode() const noexcept override;
    [[nodiscard]] std::span<const std::uint32_t> FilterArguments() const noexcept override;
    void ResetFilterBeforePatternLoad() override;
    void SetFilterParameters(
        std::int32_t mode,
        std::span<const std::uint32_t> arguments) override;
    void MarkFilterRefreshPending() noexcept override;
    void ApplyFilterCommand() override;
    [[nodiscard]] std::int32_t X() const noexcept override;
    [[nodiscard]] std::int32_t Y() const noexcept override;
    void SetPosition(std::int32_t x, std::int32_t y) noexcept override;
    void SetResultValueAt10(std::uint8_t value) noexcept override;
    void SetHasPatternItem(bool hasPatternItem) noexcept override;
    void Invalidate(const RECT& rect) override;
    // Move 截图使用同一 Pattern 的滤镜参数及 +54 临时资源，不借用 processed 画布。
    void FilterMoveSnapshotAt49D00C(std::unique_ptr<RuntimeGraphicsSurface>& snapshot);

private:
    void RefreshMainRectAt444230(const RECT& dirtyRect);
    void CreateProcessedSurfaceAt444470(bool copyPixels);
    [[nodiscard]] RECT RefreshProcessedRectAt443B00(const RECT& dirtyRect);

    PatternLayerScreenSize screenSize_{};
    bool surfaceHasAlpha_ = false;
    RuntimeGraphicsSurface surface_;
    std::vector<SpriteRuntime> sprites_;
    std::vector<std::uint32_t> filterArguments_;
    // 4636E0/462FC0 逆序析构：dirty、processed、temp、参数、Sprite、main。
    // temp 原 ref 可被其他 Filter 复制；本地解绑不能强制销毁外部仍持有的对象。
    std::shared_ptr<RuntimeGraphicsSurface> temporaryFilterSurface_;
    // 原 +64 processed 引用，以及滤镜状态 +14（Pattern+54）的临时资源引用。
    std::unique_ptr<RuntimeGraphicsSurface> processedSurface_;
    std::int32_t x_ = 0;
    std::int32_t y_ = 0;
    std::int32_t currentPatternItemIndex_ = -1;
    std::int32_t filterMode_ = 0;
    // 原 45B120 未见初始化 +60；false 仅为现代安全初始化，不代表原值。
    bool filterRefreshPending_ = false;
    std::uint8_t resultValueAt10_ = 0;
    bool hasPatternItem_ = false;
    DirtyRectCollection dirtyRects_;
    IGraphicsSurfaceCompositionApi* composition_ = nullptr;
    // 482C00 -> 441980 只借模块门控绘制；Clear/Blit 仍使用 composition。
    IGraphicsWindowSurfaceApi& drawing_;
    IPatternLayerResourceLoader* resourceLoader_ = nullptr;
    IGraphicsSurfaceApi* graphicsApi_ = nullptr;
    IGraphicsPatternFilterApi* filters_ = nullptr;
    IPatternFilterResourceLoader* filterResources_ = nullptr;
};

class PatternLayerRuntimeFactory final : public ILayerWindowPatternLayerFactory
{
public:
    // drawing 为必需借用，生命周期须覆盖工厂及其创建的全部 Pattern。
    PatternLayerRuntimeFactory(
        IGraphicsSurfaceApi& graphicsApi,
        IGraphicsSurfaceCompositionApi& composition,
        IGraphicsWindowSurfaceApi& drawing,
        IPatternLayerResourceLoader& resourceLoader,
        IGraphicsPatternFilterApi& filters,
        IPatternFilterResourceLoader& filterResources) noexcept;

    [[nodiscard]] std::unique_ptr<ILayerWindowPatternLayer> Create(
        std::size_t index,
        LayerList& internalLayerList,
        PatternLayerScreenSize screenSize,
        bool nonZeroIndexFlag,
        std::uint8_t spriteSlotCount) override;

private:
    IGraphicsSurfaceApi* graphicsApi_ = nullptr;
    IGraphicsSurfaceCompositionApi* composition_ = nullptr;
    IGraphicsWindowSurfaceApi& drawing_;
    IPatternLayerResourceLoader* resourceLoader_ = nullptr;
    IGraphicsPatternFilterApi* filters_ = nullptr;
    IPatternFilterResourceLoader* filterResources_ = nullptr;
};
}
