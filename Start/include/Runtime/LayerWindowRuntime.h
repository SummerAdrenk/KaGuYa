#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/LayerWindowDrawStateRuntime.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct PatternLayerScreenSize final
{
    std::int32_t width = 0;
    std::int32_t height = 0;

    bool operator==(const PatternLayerScreenSize&) const = default;
};

// 459709 在基类注册后查询 Graphics；459766/45988E 分别重新取得 Params。
// count 与 byte 必须各自重取当前 GameSystem，不能跨调用缓存 span/共享指针。
// 仅在构造期间借用；原 startup 字节数组控制块为空时 count=0，比较采用有符号 DWORD。
class ILayerWindowConstructionSource
{
public:
    virtual ~ILayerWindowConstructionSource() = default;
    [[nodiscard]] virtual PatternLayerScreenSize ReadScreenSizeAt47CC00() = 0;
    [[nodiscard]] virtual std::int32_t ReadStartupConfigCountAt45983C() = 0;
    [[nodiscard]] virtual std::uint8_t ReadStartupConfigByteAt45998B(std::size_t index) = 0;
};

class ILayerWindowPatternLayer : public LayerItem
{
public:
    explicit ILayerWindowPatternLayer(LayerList* internalLayerList)
        : LayerItem(internalLayerList)
    {
    }
    ~ILayerWindowPatternLayer() override = default;
    virtual void AdvanceFrameAt4423C0() = 0;
    virtual void DrawAtOffsetAt482C00(
        std::int32_t x, std::int32_t y, const RECT& dirtyRect) = 0;
};

class ILayerWindowPatternLayerFactory
{
public:
    virtual ~ILayerWindowPatternLayerFactory() = default;

    // 对应 0x459650 对 0x45B120 的逐项调用。返回空指针也会作为一条空引用记录保留。
    [[nodiscard]] virtual std::unique_ptr<ILayerWindowPatternLayer> Create(
        std::size_t index,
        LayerList& internalLayerList,
        PatternLayerScreenSize screenSize,
        bool nonZeroIndexFlag,
        std::uint8_t spriteSlotCount) = 0;
};

// 对应 CLayerWin 的基类注册、内部 CLayerList 与 +0x30 PatternLayer 引用数组。
class LayerWindowRuntime final : public FrameLayerItem, public ILayerResetTarget,
    private ILayerWindowEffectBoundary
{
public:
    // 生产入口：注册成功后才读取 source；构造 catch 保留已提交成员及父注册。
    LayerWindowRuntime(
        LayerList* outerLayerList,
        ILayerWindowConstructionSource& source,
        ILayerWindowPatternLayerFactory& factory,
        ILayerWindowDrawState& drawState,
        ILayerWindowEffectServices& effectServices);
    // 离线/受控入口：尺寸与数组身份/长度已由调用方固定，不模拟全局重取时机。
    // span 存储必须覆盖本次构造且不得失效；构造内异常仍采用 459A80 的保留语义。
    LayerWindowRuntime(
        LayerList* outerLayerList,
        std::span<const std::byte> startupConfigBytes,
        PatternLayerScreenSize screenSize,
        ILayerWindowPatternLayerFactory& factory,
        ILayerWindowDrawState& drawState,
        ILayerWindowEffectServices& effectServices);
    ~LayerWindowRuntime() override;

    void CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* destinationClip) override;
    void Draw(const RECT& dirtyRect) override;
    void AdvanceFrameAt46B180();
    void AdvanceOwnedFrame() override;
    void PropagateDirtyRectsAt4C4250();
    // 4C4330 同时供 FreeScroll 退出和 ReadScript 转场准备使用。
    void RestorePatternsAt4C4330(IGraphicsWindowSurfaceApi& drawing);
    // LoadAfter 的 4C14E0，和转场/FreeScroll 的 4C4330 不同。
    void RestoreSavedPatternsAt4C14E0(IGraphicsWindowSurfaceApi& drawing);
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi& drawing) override;
    // 4B5AC5..4B5B44：独立于普通帧推进，效果存在时才读取停止条件。
    void AdvanceEffectAndQueueDirtyAt4B5AC5();
    [[nodiscard]] LayerWindowEffectController& Effects() noexcept;
    [[nodiscard]] const LayerWindowEffectController& Effects() const noexcept;

    LayerWindowRuntime(const LayerWindowRuntime&) = delete;
    LayerWindowRuntime& operator=(const LayerWindowRuntime&) = delete;

    [[nodiscard]] LayerList& InternalLayerList() noexcept;
    [[nodiscard]] const LayerList& InternalLayerList() const noexcept;
    [[nodiscard]] RECT BoundsAt479DB0() const noexcept { return bounds_; }
    [[nodiscard]] std::size_t PatternLayerCount() const noexcept;
    [[nodiscard]] ILayerWindowPatternLayer* PatternLayerAt(std::size_t index);
    [[nodiscard]] const ILayerWindowPatternLayer* PatternLayerAt(
        std::size_t index) const;

private:
    void ConstructAt4596FA(ILayerWindowConstructionSource& source,
        ILayerWindowPatternLayerFactory& factory) noexcept;
    void ReleasePatternLayersForward() noexcept;
    void InvalidateWindowFullScreen() override;

    RECT bounds_{};
    LayerList internalLayerList_;
    std::vector<std::unique_ptr<ILayerWindowPatternLayer>> patternLayers_;
    ILayerWindowDrawState& drawState_;
    ILayerWindowEffectServices& effectServices_;
    LayerWindowEffectController effects_;
};
}
