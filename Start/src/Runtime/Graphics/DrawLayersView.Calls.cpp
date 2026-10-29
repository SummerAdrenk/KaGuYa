#include <Runtime/DrawLayersView.h>
#include <Runtime/PostWindowNativeDrawScope.h>

#include <optional>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NativeCallScope final
{
public:
    explicit NativeCallScope(const DrawLayersNativeCallServices& services,
        IGraphicsScreenCalls* graphics = nullptr) : gui_(services.gui)
    {
        if (!services.layers && !services.layerRuntime) return;
        if (!services.layers || !services.layerRuntime)
            throw std::logic_error("native Draw call services require both layers and layer runtime");
        scope_.emplace(*services.layers, *services.layerRuntime, graphics ? graphics : services.graphics);
    }
private:
    DrawGuiNativeScope gui_;
    std::optional<PostWindowNativeDrawScope> scope_;
};

PatternLayerNativeRenderServices* PatternServices(const DrawLayersNativeCallServices& calls,
    PatternLayerNativeRenderServices* explicitServices) noexcept
{
    return explicitServices ? explicitServices : calls.layerRuntime ? &calls.layerRuntime->pattern : nullptr;
}
}

void DrawPatternView::InvalidateAllAt443650() const
{
    const NativeCallScope scope(callServices_);
    methods_->invalidateAll(body_);
}
void DrawPatternView::RefreshAt443B00(PatternLayerNativeRenderServices* services) const
{
    const NativeCallScope scope(callServices_);
    methods_->refresh(body_, PatternServices(callServices_, services));
}
void DrawWindowView::AdvanceEffectAndQueueDirtyAt4B5AC5() const
{
    const NativeCallScope scope(callServices_);
    methods_->advanceEffect(body_, callServices_.windowEffects ? callServices_.windowEffects : services_);
}
void DrawWindowView::PropagateDirtyRectsAt4C4250() const
{
    const NativeCallScope scope(callServices_);
    methods_->propagateDirty(body_);
}
DrawPatternView DrawWindowView::PatternAt4AD2A0(std::uint32_t index) const
{
    return methods_->pattern(body_, index).WithNativeCallServices(callServices_);
}
void DrawWindowView::RestorePatternsAt4C4330(IGraphicsWindowSurfaceApi& graphics,
    PatternLayerNativeRenderServices* native) const
{
    const NativeCallScope scope(callServices_);
    methods_->restore(body_, graphics, PatternServices(callServices_, native));
}
void DrawWindowView::RestoreSavedPatternsAt4C14E0(IGraphicsWindowSurfaceApi& graphics,
    PatternLayerNativeRenderServices* native) const
{
    const NativeCallScope scope(callServices_);
    methods_->restoreSaved(body_, graphics, PatternServices(callServices_, native));
}
bool DrawTextView::RebuildAt4C1650(std::uint32_t refreshUnread, DrawTextNativeRebuildServices* native,
    DrawTextControlledRebuildServices* controlled) const
{
    const NativeCallScope scope(callServices_);
    return methods_->rebuild(body_, refreshUnread, native, controlled);
}
void DrawTextView::ResetTextAt46D170(TextWindowLayerNativeServices* native) const
{
    const NativeCallScope scope(callServices_);
    methods_->reset(body_, native ? native : callServices_.layers ? &callServices_.layers->text : nullptr);
}
void DrawLayerView::CompositeToSurface(GraphicsSurfaceDestination target, const RECT* clip) const
{
    const NativeCallScope scope(callServices_);
    methods_->composite(body_, target, clip);
}
void RegisteredLayersView::CompositeToSurface(GraphicsSurfaceDestination target, const RECT* clip) const
{
    const NativeCallScope scope(callServices_);
    methods_->composite(body_, target, clip);
}
void RegisteredLayersView::RenderDirtyRects(IGraphicsScreenCalls& graphics) const
{
    const NativeCallScope scope(callServices_, &graphics);
    // 原生方法表内部另建LayerList scope，显式绘制screen与同源memory必须继续传入。
    auto& memory = callServices_.layers ? callServices_.layers->memory : *memory_;
    methods_->drawDirty(body_, graphics, memory);
}
void RegisteredLayersView::AddDirtyRect(const RECT* rect) const
{
    const NativeCallScope scope(callServices_);
    methods_->addDirty(body_, rect);
}
DrawLayerView DrawLayersView::LayerAt418C10(std::uint32_t index) const
{
    return methods_->layer(body_, index).WithNativeCallServices(callServices_);
}
RegisteredLayersView DrawLayersView::RegisteredLayers() const
{
    return methods_->registered(body_, *memory_).WithNativeCallServices(callServices_);
}
void DrawLayersView::AdvanceFramesAt46B240() const
{
    const NativeCallScope scope(callServices_);
    methods_->advance(body_);
}
void DrawLayersView::ResetOwnedLayersAt47E940(IGraphicsWindowSurfaceApi& graphics) const
{
    const NativeCallScope scope(callServices_);
    methods_->reset(body_, graphics);
}
std::uint8_t DrawLayersView::RequestOwnedLayerAtVslot10(std::uint32_t index,
    std::uint32_t value, std::int32_t second) const
{
    const NativeCallScope scope(callServices_);
    return methods_->request(body_, index, value, second);
}
DrawWindowView DrawLayersView::WindowZero() const
{
    auto* effects = callServices_.windowEffects ? callServices_.windowEffects : windowServices_;
    return methods_->window0(body_, effects).WithNativeCallServices(callServices_);
}
DrawTextView DrawLayersView::TextAtSlotThree() const
{
    return methods_->text3(body_).WithNativeCallServices(callServices_);
}
DrawToolView DrawLayersView::ToolAtSlotTwo(ToolWindowNativeInteractionServices* native,
    IToolWindowInteractionServices* controlled) const
{
    return methods_->tool2(body_,native,controlled).WithNativeGuiServices(callServices_.gui);
}
DrawSelectionView DrawLayersView::SelectionAtSlotFive(SelectWindowNativeWorkflowServices* native,
    DrawSelectionControlledServices controlled) const
{
    return methods_->selection5(body_,native,controlled).WithNativeGuiServices(callServices_.gui);
}
DrawOpacityView DrawLayersView::OpacityAtSlotSix(IOpacityWindowInputSource* native,
    DrawOpacityControlledServices controlled) const
{
    return methods_->opacity6(body_,native,controlled).WithNativeGuiServices(callServices_.gui);
}
}
