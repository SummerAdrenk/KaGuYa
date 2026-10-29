#pragma once

#include <Runtime/LayerWindowNativeBase.h>
#include <Runtime/DrawLayerReferenceArray.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/LayerWindowEffectNative.h>

namespace KaGuYa::Start::Runtime
{
class ILayerWindowConstructionSource;
class ILayerWindowDrawState;

// 459650/468C20：注册表、拥有数组与效果各有唯一原Ref，不持现代窗口镜像。
struct LayerWindowNativeBody4Ch final
{
    LayerWindowNativeBody24h window;
    union { YtlReferenceSlotRuntime internalAt24; };
    DrawLayerReferenceArray patternsAt30;
    std::int32_t selectorAt3C;
    union { YtlReferenceSlotRuntime effectAt40; };
    LayerWindowNativeBody4Ch() noexcept {}
    ~LayerWindowNativeBody4Ch() noexcept {}
    LayerWindowNativeBody4Ch(const LayerWindowNativeBody4Ch&) = delete;
    LayerWindowNativeBody4Ch& operator=(const LayerWindowNativeBody4Ch&) = delete;
};
static_assert(sizeof(LayerWindowNativeBody4Ch) == 0x4C && alignof(LayerWindowNativeBody4Ch) == 4);
static_assert(offsetof(LayerWindowNativeBody4Ch, internalAt24) == 0x24);
static_assert(offsetof(LayerWindowNativeBody4Ch, patternsAt30) == 0x30);
static_assert(offsetof(LayerWindowNativeBody4Ch, selectorAt3C) == 0x3C);
static_assert(offsetof(LayerWindowNativeBody4Ch, effectAt40) == 0x40);

const LayerItemNativeFrameVtable& LayerWindowNativeMethods() noexcept;
LayerWindowNativeBody4Ch* ConstructLayerWindowNativeAt459650(LayerWindowNativeBody4Ch& body,
    LayerListNativeBody18h* parent, ILayerWindowConstructionSource& source,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
LayerWindowNativeBody4Ch* DeleteLayerWindowNativeAt468C20(LayerWindowNativeBody4Ch& body,
    std::uint32_t flags) noexcept;
void DrawLayerWindowNativeAt482900(LayerWindowNativeBody4Ch& body, const RECT* clip,
    const ILayerWindowDrawState& state);
void CompositeLayerWindowNativeAt481990(LayerWindowNativeBody4Ch& body,
    GraphicsSurfaceNativeBody* destination, const RECT* clip);
void AdvanceLayerWindowNativeAt46B180(LayerWindowNativeBody4Ch& body,
    PatternLayerNativeRenderServices& services);
void ResetLayerWindowNativeAt47EC40(LayerWindowNativeBody4Ch& body, IGraphicsWindowSurfaceApi& drawing);
void PropagateLayerWindowDirtyNativeAt4C4250(LayerWindowNativeBody4Ch& body);
void RestoreLayerWindowPatternsNativeAt4C4330(LayerWindowNativeBody4Ch& body,
    PatternLayerNativeRenderServices& services);
void RestoreSavedLayerWindowPatternsNativeAt4C14E0(LayerWindowNativeBody4Ch& body,
    PatternLayerNativeRenderServices& services);
void ReplaceNativeLayerWindowEffectAt4D6860(LayerWindowNativeBody4Ch& body, std::int32_t selector,
    ILayerWindowEffectNativeSource& source);
void AdvanceNativeLayerWindowEffectAt4B5AC5(LayerWindowNativeBody4Ch& body,
    const ILayerWindowDrawState& state, ILayerWindowEffectNativeSource& source);

// 只在调用期提供原全局服务；不进入4Ch/80h或控制块，不另持资源。
struct LayerWindowNativeServices final
{
    PatternLayerNativeRenderServices& pattern;
    const ILayerWindowDrawState& drawState;
};
class LayerWindowNativeScope final
{
public:
    explicit LayerWindowNativeScope(LayerWindowNativeServices& services) noexcept;
    ~LayerWindowNativeScope() noexcept;
    LayerWindowNativeScope(const LayerWindowNativeScope&) = delete;
    LayerWindowNativeScope& operator=(const LayerWindowNativeScope&) = delete;
private:
    LayerWindowNativeServices* previous_;
    PatternLayerNativeGraphicsScope patternDrawing_;
};
}
