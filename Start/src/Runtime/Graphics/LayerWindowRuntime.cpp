#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/PatternLayerRuntime.h>

#include <utility>
#include <stdexcept>
#include <limits>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FixedLayerWindowConstructionSource final : public ILayerWindowConstructionSource
{
public:
    FixedLayerWindowConstructionSource(std::span<const std::byte> bytes,
        PatternLayerScreenSize screenSize) noexcept : bytes_(bytes), screenSize_(screenSize) {}
    PatternLayerScreenSize ReadScreenSizeAt47CC00() override { return screenSize_; }
    std::int32_t ReadStartupConfigCountAt45983C() override
    {
        if (bytes_.size() > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
            throw std::length_error("LayerWindow fixed config exceeds signed DWORD count");
        return static_cast<std::int32_t>(bytes_.size());
    }
    std::uint8_t ReadStartupConfigByteAt45998B(std::size_t index) override
    {
        return std::to_integer<std::uint8_t>(bytes_[index]);
    }
private:
    std::span<const std::byte> bytes_;
    PatternLayerScreenSize screenSize_;
};
}

void LayerWindowRuntime::AdvanceOwnedFrame()
{
    AdvanceFrameAt46B180();
}

void LayerWindowRuntime::ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi& drawing)
{
    for (std::size_t index = 0; index < patternLayers_.size(); ++index)
    {
        auto* pattern = dynamic_cast<PatternLayerRuntime*>(patternLayers_[index].get());
        if (!pattern) throw std::logic_error("47EC40 requires actual PatternLayer owning references");
        pattern->ResetForRootAt47EC71(drawing);
    }
    // Pattern全部成功后才恢复外层enabled和selector；internal/outer dirty不清。
    SetEnabledByteAt0C(1);
    effects_.ResetForRootAt47ED4C();
}

void LayerWindowRuntime::Draw(const RECT& dirtyRect)
{
    // 482932..482976：严格短路，后续全局对象可能尚不可用，不能预读所有状态。
    const LayerWindowEffectState* effect = nullptr;
    if (!drawState_.AnyActionSixCurrentState() &&
        (drawState_.UnSaveFlagsAt14() & 2u) == 0 && drawState_.SaveByteAt6F() != 0)
        effect = effects_.Effect() ? &*effects_.Effect() : nullptr;

    // 4829A5 / 4D4250：两分支都走 +30 拥有数组，不走 internalLayerList 注册表。
    // 本函数不检查 Window 自身 enabled，也不隐式推进效果/动画或清理 dirty。
    for (const auto& layer : patternLayers_)
    {
        if (!layer)
            throw std::logic_error("LayerWindow draw encountered a null Pattern reference");
        if (!layer->Enabled())
            continue;
        if (effect)
            layer->DrawAtOffsetAt482C00(effect->valueAt10, effect->valueAt14, dirtyRect);
        else
            layer->Draw(dirtyRect);
    }
}

LayerWindowEffectController& LayerWindowRuntime::Effects() noexcept { return effects_; }
const LayerWindowEffectController& LayerWindowRuntime::Effects() const noexcept { return effects_; }

void LayerWindowRuntime::InvalidateWindowFullScreen()
{
    const auto height = effectServices_.ReadProgramHeightAt1C();
    const auto width = effectServices_.ReadProgramWidthAt18();
    internalLayerList_.DirtyRects().Add({0, 0, width, height});
}

void LayerWindowRuntime::AdvanceEffectAndQueueDirtyAt4B5AC5()
{
    if (!effects_.Effect())
        return;
    // 4C3430 与 Draw 同一组状态，含义却是停止/复位参数，不是跳过 Update 的门控。
    const bool stop = drawState_.AnyActionSixCurrentState() ||
        (drawState_.UnSaveFlagsAt14() & 2u) != 0 || drawState_.SaveByteAt6F() == 0;
    RECT dirty{};
    effects_.AdvanceMotionAt4D42D0(dirty, static_cast<std::uint8_t>(stop), effectServices_);
    if (!::IsRectEmpty(&dirty))
        internalLayerList_.DirtyRects().Add(dirty);
}

void LayerWindowRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip)
{
    // 4819B7 尾跳内部 LayerList+1C，保留目标与 clip，不清画布或推进拥有数组。
    internalLayerList_.CompositeToSurface(destination, destinationClip);
}

void LayerWindowRuntime::PropagateDirtyRectsAt4C4250()
{
    auto& dirty = internalLayerList_.DirtyRects();
    dirty.Coalesce();
    const auto count = static_cast<std::uint32_t>(dirty.Count());
    // 4C42B2空集不取指针；非空集先取一次指针，再按signed count传播。
    if (count != 0)
    {
        auto& body = dirty.NativeBody();
        const auto* const rects = body.vptrAt00->rectsAt1C(&body);
        for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
        {
            // 4C42E8每轮读取parent；enabled不门控，也不裁剪矩形。
            auto* const parent = RegisteredLayerList();
            if (!parent)
                throw std::logic_error("LayerWindow dirty propagation requires a parent list");
            parent->DirtyRects().Add(rects[index]);
        }
    }
    // 原末尾才清空，不能在父引用失败时丢失待传播矩形。
    dirty.Clear();
}

void LayerWindowRuntime::RestorePatternsAt4C4330(IGraphicsWindowSurfaceApi& drawing)
{
    for (std::size_t i = 0; i < PatternLayerCount(); ++i)
    {
        auto* pattern = dynamic_cast<PatternLayerRuntime*>(PatternLayerAt(i));
        if (!pattern) throw std::logic_error("LayerWindow restore encountered a missing Pattern");
        if (!pattern->Enabled())
        {
            if (!pattern->HasPatternItem()) continue;
            pattern->SetEnabledByteAt0C(1);
        }
        if (!pattern->HasPatternItem())
        {
            pattern->SetEnabledByteAt0C(0);
            const auto& main = pattern->MainSurfaceAt18();
            // 4C439C..4C43A1：先检查 main+4 handle，再调用全画布 Clear。
            if (main.Handle() != 0)
                drawing.SurfaceClearRaw(main.Handle(), main.ClearColor(), nullptr);
        }
        pattern->RefreshMainAndPropagateAt443B00();
    }
}

void LayerWindowRuntime::RestoreSavedPatternsAt4C14E0(IGraphicsWindowSurfaceApi& drawing)
{
    if (!Enabled())
        return;
    for (const auto& owned : patternLayers_)
    {
        auto* const pattern = dynamic_cast<PatternLayerRuntime*>(owned.get());
        if (!pattern)
            throw std::logic_error("LayerWindow save restore requires a Pattern reference");
        if (!pattern->Enabled())
        {
            if (!pattern->HasPatternItem())
                continue;
            pattern->SetEnabledByteAt0C(1);
        }
        if (!pattern->HasPatternItem())
        {
            pattern->SetEnabledByteAt0C(0);
            const auto& main = pattern->MainSurfaceAt18();
            if (main.Handle() != 0)
                drawing.SurfaceClearRaw(main.Handle(), main.ClearColor(), nullptr);
            // 4C1580 直接到下一层，不刷新 processed、不传播该层旧 dirty。
            continue;
        }
        for (std::size_t index = 0; index < pattern->SpriteSlotCount(); ++index)
            pattern->SpriteAt(index).RestoreAnimationAt4C14E0();
        pattern->RefreshMainAndPropagateAt443B00();
    }
}

void LayerWindowRuntime::AdvanceFrameAt46B180()
{
    if (!Enabled())
        return;
    // 46B18A..46B1BD 按 +30 拥有数组顺序调用，不遍历内部绘制注册表。
    for (const auto& layer : patternLayers_)
    {
        // 原版对空引用继续解引用；现代安全边界明确失败，不静默跳过。
        if (!layer)
            throw std::logic_error("LayerWindow frame update encountered a null Pattern reference");
        layer->AdvanceFrameAt4423C0();
    }
}

LayerWindowRuntime::LayerWindowRuntime(
    LayerList* const outerLayerList,
    ILayerWindowConstructionSource& source,
    ILayerWindowPatternLayerFactory& factory,
    ILayerWindowDrawState& drawState,
    ILayerWindowEffectServices& effectServices)
    : FrameLayerItem(outerLayerList), drawState_(drawState),
      effectServices_(effectServices), effects_(*this)
{
    ConstructAt4596FA(source, factory);
}

LayerWindowRuntime::LayerWindowRuntime(
    LayerList* const outerLayerList,
    const std::span<const std::byte> startupConfigBytes,
    const PatternLayerScreenSize screenSize,
    ILayerWindowPatternLayerFactory& factory,
    ILayerWindowDrawState& drawState,
    ILayerWindowEffectServices& effectServices)
    : FrameLayerItem(outerLayerList), drawState_(drawState),
      effectServices_(effectServices), effects_(*this)
{
    FixedLayerWindowConstructionSource source(startupConfigBytes, screenSize);
    ConstructAt4596FA(source, factory);
}

void LayerWindowRuntime::ConstructAt4596FA(
    ILayerWindowConstructionSource& source, ILayerWindowPatternLayerFactory& factory) noexcept
{
    try
    {
        const auto screenSize = source.ReadScreenSizeAt47CC00();
        bounds_ = {0, 0, screenSize.width, screenSize.height};
        // 459846 JGE：每轮（含终止轮）重取 count；45998B 单独重取本项字节。
        // 保留现代 factory 的分配边界，不模拟原 80h/Ref12 的 OOM ABI。
        for (std::int32_t index = 0; index < source.ReadStartupConfigCountAt45983C(); ++index)
        {
            const auto ordinal = static_cast<std::size_t>(index);
            const auto spriteSlotCount = source.ReadStartupConfigByteAt45998B(ordinal);
            std::unique_ptr<ILayerWindowPatternLayer> layer = factory.Create(
                ordinal,
                internalLayerList_,
                screenSize,
                index != 0,
                spriteSlotCount);
            patternLayers_.push_back(std::move(layer));
        }
    }
    catch (...)
    {
        // 5EA584 -> try map 5EA570 [4,12] -> catch(...) 5EA560 -> 459A80。
        // 只清 +14 RECT 后返回；当前临时对象已展开，已追加 Pattern/父注册均保留。
        // 441270 基类注册及成员初始化在本 try 外，不能由此吞掉它们的异常。
        bounds_ = {};
    }
}

LayerWindowRuntime::~LayerWindowRuntime()
{
    // 468C45..468C82：先释放效果，不标脏；随后正序释放 Pattern，再内部 List/基类。
    effects_.ReleaseEffect();
    ReleasePatternLayersForward();
}

LayerList& LayerWindowRuntime::InternalLayerList() noexcept
{
    return internalLayerList_;
}

const LayerList& LayerWindowRuntime::InternalLayerList() const noexcept
{
    return internalLayerList_;
}

std::size_t LayerWindowRuntime::PatternLayerCount() const noexcept
{
    return patternLayers_.size();
}

ILayerWindowPatternLayer* LayerWindowRuntime::PatternLayerAt(const std::size_t index)
{
    return patternLayers_.at(index).get();
}

const ILayerWindowPatternLayer* LayerWindowRuntime::PatternLayerAt(
    const std::size_t index) const
{
    return patternLayers_.at(index).get();
}

void LayerWindowRuntime::ReleasePatternLayersForward() noexcept
{
    // 452440 逐槽释放时不压缩数组，后续 child 析构仍能观察原 count/index。
    for (auto& layer : patternLayers_)
        layer.reset();
    patternLayers_.clear();
}
}
