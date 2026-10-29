#pragma once

#include <Runtime/LayerItemNative.h>
#include <Runtime/PatternFilterNative.h>
#include <Runtime/SpriteNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsPatternFilterCalls;
class IScrResourceNameRootSource;
struct PatternLayerNativeVtable final
{
    LayerItemNativeVtable base;
    void (__thiscall* drawOffsetAt0C)(LayerItemNativeBody14h*, std::int32_t, std::int32_t, const RECT*);
};
static_assert(sizeof(PatternLayerNativeVtable) == 0x10);
static_assert(offsetof(PatternLayerNativeVtable, drawOffsetAt0C) == 0x0C);

// 45B120/468FC0：唯一80h正文，四个槽，不是带Bounds/Advance的七槽窗口。
struct PatternLayerNativeBody80h final
{
    LayerItemNativeBody14h layer;
    std::int32_t itemIndexAt14;
    union { YtlReferenceSlotRuntime mainAt18; };
    RECT boundsAt24;
    union { YtlReferenceSlotRuntime spritesAt34; };
    union { PatternFilterNativeBody20h filterAt40; };
    std::uint32_t pendingAt60;
    union { YtlReferenceSlotRuntime processedAt64; };
    union { YtlReferenceSlotRuntime dirtyAt70; };
    std::uint8_t hasItemAt7C;
    std::uint8_t resultAt7D;
    std::uint8_t unwrittenAt7E[2];
    PatternLayerNativeBody80h() noexcept {}
    ~PatternLayerNativeBody80h() noexcept {}
    PatternLayerNativeBody80h(const PatternLayerNativeBody80h&) = delete;
    PatternLayerNativeBody80h& operator=(const PatternLayerNativeBody80h&) = delete;
};
static_assert(sizeof(PatternLayerNativeBody80h) == 0x80 && alignof(PatternLayerNativeBody80h) == 4);
static_assert(offsetof(PatternLayerNativeBody80h, layer) == 0);
static_assert(offsetof(PatternLayerNativeBody80h, itemIndexAt14) == 0x14);
static_assert(offsetof(PatternLayerNativeBody80h, mainAt18) == 0x18);
static_assert(offsetof(PatternLayerNativeBody80h, boundsAt24) == 0x24);
static_assert(offsetof(PatternLayerNativeBody80h, spritesAt34) == 0x34);
static_assert(offsetof(PatternLayerNativeBody80h, filterAt40) == 0x40);
static_assert(offsetof(PatternLayerNativeBody80h, pendingAt60) == 0x60);
static_assert(offsetof(PatternLayerNativeBody80h, processedAt64) == 0x64);
static_assert(offsetof(PatternLayerNativeBody80h, dirtyAt70) == 0x70);
static_assert(offsetof(PatternLayerNativeBody80h, hasItemAt7C) == 0x7C);
static_assert(offsetof(PatternLayerNativeBody80h, resultAt7D) == 0x7D);

const PatternLayerNativeVtable& PatternLayerNativeMethods() noexcept;
PatternLayerNativeBody80h* ConstructPatternLayerNativeAt45B120(PatternLayerNativeBody80h& body,
    LayerListNativeBody18h* parent, SIZE size, std::int32_t alpha, std::uint32_t spriteCount,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DestroyPatternLayerNativeAt4636E0(PatternLayerNativeBody80h& body) noexcept;
PatternLayerNativeBody80h* DeletePatternLayerNativeAt468FC0(PatternLayerNativeBody80h& body,
    std::uint32_t flags) noexcept;
void ConstructPatternLayerReferenceAt4599B5(YtlReferenceSlotRuntime& out, PatternLayerNativeBody80h* body);

void DrawPatternLayerNativeAt482C00(PatternLayerNativeBody80h& body, std::int32_t x, std::int32_t y,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing);
void CompositePatternLayerNativeAt481BD0(PatternLayerNativeBody80h& body,
    GraphicsSurfaceNativeBody* destination, const RECT* clip, IGraphicsWindowSurfaceApi& drawing);
// 444230每轮重读Sprite数量/引用，临时坐标只有正常绘制后恢复。
void RefreshPatternMainNativeAt444230(PatternLayerNativeBody80h& body,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing);
GraphicsSurfaceNativeBody* ResolveSpriteSurfaceNativeAt443580(
    const SpriteNativeBodyA0h& body, std::int32_t index) noexcept;

// 全为调用期间的借用；不进入80h正文、Ref控制块或另一个资源owner。
struct PatternLayerNativeRenderServices final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsPatternFilterCalls& filters;
    const IScrResourceNameRootSource& names;
    IGuiControlResourceLoadServices& resources;
    IRuntimeStringManagerSource& strings;
};
void RefreshPatternProcessedNativeAt443B00(PatternLayerNativeBody80h& body,
    PatternLayerNativeRenderServices& services);
std::int32_t ClearPatternSpritesNativeAt442700(PatternLayerNativeBody80h& body, std::int32_t index);
void ResetPatternLayerNativeAt47EC71(PatternLayerNativeBody80h& body, IGraphicsWindowSurfaceApi& drawing);
void AdvancePatternLayerNativeAt4423C0(PatternLayerNativeBody80h& body, PatternLayerNativeRenderServices& services);
// 4D3490只汇总有效模式；空名字或unsigned mode>2贡献零矩形，不走资源提交的错误分支。
RECT* ReadPatternBoundsNativeAt4D3490(PatternLayerNativeBody80h& body, RECT* out);
// 4CAF10按当前filter参数的原始DWORD对执行；非零命令要求具名mode2，否则抛int 10。
void ApplyPatternAnimationCommandsNativeAt4CAF10(PatternLayerNativeBody80h& body);

// 原槽没有服务参数；宿主提供非拥有的模块调用边界，不在80h正文增加API指针。
class PatternLayerNativeGraphicsScope final
{
public:
    explicit PatternLayerNativeGraphicsScope(IGraphicsWindowSurfaceApi& drawing) noexcept;
    ~PatternLayerNativeGraphicsScope() noexcept;
    PatternLayerNativeGraphicsScope(const PatternLayerNativeGraphicsScope&) = delete;
    PatternLayerNativeGraphicsScope& operator=(const PatternLayerNativeGraphicsScope&) = delete;
private:
    IGraphicsWindowSurfaceApi* previous_;
};
}
