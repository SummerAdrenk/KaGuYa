#include <Runtime/LayerWindowNative.h>
#include <Runtime/LayerWindowDrawStateRuntime.h>
#include <Runtime/LayerListNative.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerWindowNativeBody4Ch;
PatternLayerNativeBody80h& Pattern(const YtlReferenceSlotRuntime& slot)
{
    auto* const body = ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(slot);
    return *body;
}
LayerListNativeBody18h& Internal(Body& body)
{
    auto* const list = ResolveLayerListReference(body.internalAt24);
    return *list;
}
GuiDirtyRectNativeBody80h& Dirty(Body& body)
{
    auto& list = Internal(body);
    auto* const dirty = list.vptrAt00->dirtyAt14(&list);
    return *dirty;
}
}
void DrawLayerWindowNativeAt482900(Body& body, const RECT* clip, const ILayerWindowDrawState& state)
{
    if (!state.AnyActionSixCurrentState() && (state.UnSaveFlagsAt14() & 2u) == 0 && state.SaveByteAt6F())
    {
        if (auto* const effect = ResolveLayerWindowEffectReference(body.effectAt40))
        {
            effect->vptrAt00->drawAt00(effect, clip, &body.patternsAt30);
            return;
        }
    }
    // 4829FD读取当前end；不能用range-for缓存末端，也不以Window.enabled门控。
    for (auto* slot = body.patternsAt30.begin; slot != body.patternsAt30.end; ++slot)
    {
        auto& pattern = Pattern(*slot);
        if (pattern.layer.enabledAt0C)
            pattern.layer.vptrAt00->drawAt00(&pattern.layer, clip);
    }
}
void CompositeLayerWindowNativeAt481990(Body& body, GraphicsSurfaceNativeBody* destination, const RECT* clip)
{
    auto& list = Internal(body);
    list.vptrAt00->compositeAt1C(&list, destination, clip);
}
void AdvanceLayerWindowNativeAt46B180(Body& body, PatternLayerNativeRenderServices& services)
{
    if (!body.window.layer.enabledAt0C) return;
    for (auto* slot = body.patternsAt30.begin; slot != body.patternsAt30.end; ++slot)
        AdvancePatternLayerNativeAt4423C0(Pattern(*slot), services);
}
void ResetLayerWindowNativeAt47EC40(Body& body, IGraphicsWindowSurfaceApi& drawing)
{
    for (auto* slot = body.patternsAt30.begin; slot != body.patternsAt30.end; ++slot)
        ResetPatternLayerNativeAt47EC71(Pattern(*slot), drawing);
    body.window.layer.enabledAt0C = 1;
    body.selectorAt3C = -1;
    ReleaseYtlReference(body.effectAt40);
}
void PropagateLayerWindowDirtyNativeAt4C4250(Body& body)
{
    auto* dirty = &Dirty(body);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = &Dirty(body);
    const auto count = dirty->vptrAt00->countAt18(dirty);
    if (count)
    {
        dirty = &Dirty(body);
        const auto* const rects = dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t i = 0; i < std::bit_cast<std::int32_t>(count); ++i)
        {
            auto* const parent = body.window.layer.parentAt10;
            parent->vptrAt00->addDirtyAt20(parent, &rects[i]);
        }
    }
    dirty = &Dirty(body);
    dirty->vptrAt00->clearAt00(dirty);
}
}
