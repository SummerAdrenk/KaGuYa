#include <Runtime/LayerWindowNative.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/LayerWindowDrawStateRuntime.h>
#include <Runtime/LayerListNative.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerWindowNativeBody4Ch;
template<class T> T& Required(T* value)
{
    return *value;
}
std::uint32_t Count(const YtlReferenceSlotRuntime& slot) noexcept
{
    return slot.controlAt08 ? static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 : 0;
}
template<class T> T* Element(const YtlReferenceSlotRuntime& slot, std::uint32_t index) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ? static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0;
    return reinterpret_cast<T*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
void ClearMain(PatternLayerNativeBody80h& body, IGraphicsWindowSurfaceApi& drawing)
{
    auto& main = Required(ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18));
    if (const auto handle = main.handleAt04) drawing.SurfaceClearRaw(handle, main.clearColorAt20, nullptr);
}
class SelectionSource final : public ILayerWindowEffectSelectionSource
{
public:
    SelectionSource(Body& body, ILayerWindowEffectNativeSource& source) noexcept : body_(body), source_(source) {}
    std::int32_t ReadSelectorAt3C() override { return body_.selectorAt3C; }
    void WriteSelectorAt3C(std::int32_t selector) override { body_.selectorAt3C = selector; }
    const YtlReferenceSlotRuntime& CaptureLayerListAt24() override { return body_.internalAt24; }
    void ClearScreenIfLoadedAt4182F0() override { source_.ClearScreenIfLoadedAt4182F0(); }
    std::int32_t ReadProgramHeightAt4180C0() override { return source_.ReadProgramHeightAt4180C0(); }
    std::int32_t ReadProgramWidthAt4180C0() override { return source_.ReadProgramWidthAt4180C0(); }
private:
    Body& body_;
    ILayerWindowEffectNativeSource& source_;
};
}
void RestoreLayerWindowPatternsNativeAt4C4330(Body& body, PatternLayerNativeRenderServices& services)
{
    for (auto* slot = body.patternsAt30.begin; slot != body.patternsAt30.end; ++slot)
    {
        auto& pattern = Required(ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(*slot));
        if (!pattern.layer.enabledAt0C)
        {
            if (!pattern.hasItemAt7C) continue;
            pattern.layer.enabledAt0C = 1;
        }
        if (!pattern.hasItemAt7C)
        {
            pattern.layer.enabledAt0C = 0;
            ClearMain(pattern, services.drawing);
        }
        RefreshPatternProcessedNativeAt443B00(pattern, services);
    }
}
void RestoreSavedLayerWindowPatternsNativeAt4C14E0(Body& body, PatternLayerNativeRenderServices& services)
{
    if (!body.window.layer.enabledAt0C) return;
    for (auto* slot = body.patternsAt30.begin; slot != body.patternsAt30.end; ++slot)
    {
        auto& pattern = Required(ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(*slot));
        if (!pattern.layer.enabledAt0C)
        {
            if (!pattern.hasItemAt7C) continue;
            pattern.layer.enabledAt0C = 1;
        }
        if (!pattern.hasItemAt7C)
        {
            pattern.layer.enabledAt0C = 0;
            ClearMain(pattern, services.drawing);
            continue; // 4C1580不进入443B00，保留该层dirty和processed。
        }
        for (std::uint32_t i = 0; i < Count(pattern.spritesAt34); ++i)
        {
            auto& sprite = Required(Element<SpriteNativeBodyA0h>(pattern.spritesAt34, i));
            if (!sprite.logicalNameAt0C.GetLength() || sprite.modeAt04 != 2 || sprite.animationAt34.repeatAt10)
                continue;
            auto& animation = sprite.animationAt34;
            const auto count = Count(animation.commandsAt3C);
            if (count && Required(Element<SpriteCommandNativeBody10h>(animation.commandsAt3C, count - 1)).opcodeAt00 == 4)
                AdvanceSpriteAnimationToFinalNativeAt4C1D20(animation);
            else
            {
                animation.indexAt08 = 0;
                animation.flagsAt00 = 0;
                CopySpriteFinalMappingsNativeAt4C23A0(animation);
            }
        }
        RefreshPatternProcessedNativeAt443B00(pattern, services);
    }
}
void ReplaceNativeLayerWindowEffectAt4D6860(Body& body, std::int32_t selector, ILayerWindowEffectNativeSource& source)
{
    SelectionSource selection(body, source);
    ReplaceLayerWindowEffectAt4D6860(body.effectAt40, selector, selection);
}
void AdvanceNativeLayerWindowEffectAt4B5AC5(Body& body,
    const ILayerWindowDrawState& state, ILayerWindowEffectNativeSource& source)
{
    auto* const effect = ResolveLayerWindowEffectReference(body.effectAt40);
    if (!effect) return;
    // 4B5AE5固定当前表，再求stop；状态读取期间换effect或vptr也不重定本次调用。
    const auto* const methods = effect->vptrAt00;
    const bool stop = state.AnyActionSixCurrentState() || (state.UnSaveFlagsAt14() & 2u) != 0 || !state.SaveByteAt6F();
    LayerWindowEffectNativeScope scope(source);
    RECT changed;
    (void)methods->motionAt08(effect, &changed, static_cast<std::uint8_t>(stop));
    if (::IsRectEmpty(&changed)) return;
    auto& list = Required(ResolveLayerListReference(body.internalAt24));
    auto& dirty = Required(list.vptrAt00->dirtyAt14(&list));
    dirty.vptrAt00->addRectAt08(&dirty, &changed);
}
}
