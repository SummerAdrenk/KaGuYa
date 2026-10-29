#include <Runtime/FreeScrollNative.h>
#include <Runtime/FreeScrollNativeDrawing.h>
#include <Runtime/FreeScrollNativeInput.h>
#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/PatternLayerNativeResourceTarget.h>
#include <Runtime/ParamsRuntimeProfile.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
LayerWindowNativeBody4Ch* WindowZero(IDrawManagerNativeRootSource& source)
{
    // 直接读 owning.begin 的第一 Ref；原点没有 LayerAt 的现代下标保护。
    auto* manager = source.CaptureDrawManagerAfter413BE0();
    return ResolveEnvironmentObjectReference<LayerWindowNativeBody4Ch>(*manager->ownedLayersAt14.begin);
}
PatternLayerNativeBody80h* PatternAt(LayerWindowNativeBody4Ch* window, std::uint32_t index)
{
    const auto& array = window->patternsAt30;
    if (static_cast<std::uint32_t>(DrawLayerReferenceCount(array)) < index) return nullptr;
    return ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(array.begin[index]);
}
PatternLayerNativeBody80h* FirstPattern(IDrawManagerNativeRootSource& source)
{
    // 第一项三次重取不进行 count 比较，区别于前面的遍历选择。
    return ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(*WindowZero(source)->patternsAt30.begin);
}
std::int32_t StartupCount(const IStartupConfigGameSystemSource& source)
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        source.CaptureGameSystemAt418480()->startupConfigBytes.size()));
}
}

RECT ReadFreeScrollLocalUnionAt4ADDE0(PatternLayerNativeBody80h& pattern)
{
    RECT result{};
    const auto count = [&]() -> std::uint32_t {
        auto* control = pattern.spritesAt34.controlAt08;
        return control ? static_cast<EnvironmentObjectReferenceControl*>(control)->countAt10 : 0;
    };
    for (std::uint32_t i = 0; i < count(); ++i)
    {
        const auto& ref = pattern.spritesAt34;
        const auto stride = ref.controlAt08 ?
            static_cast<EnvironmentObjectReferenceControl*>(ref.controlAt08)->strideAt0C : 0u;
        auto* sprite = ref.baseAt00 ? reinterpret_cast<SpriteNativeBodyA0h*>(
            ref.baseAt00 + (ref.biasAt04 + i) * stride) : nullptr;
        RECT bounds{};
        if (sprite->logicalNameAt0C.GetLength() && sprite->modeAt04 <= 2u)
        {
            bounds = {0, 0, sprite->widthAt24, sprite->heightAt28};
            ::OffsetRect(&bounds, sprite->xAt2C, sprite->yAt30);
        }
        ::UnionRect(&result, &result, &bounds);
    }
    return result;
}

bool BeginFreeScrollNativeAt4ADF70(FreeScrollNativeBody50h& body,
    IDrawManagerNativeRootSource& draw, const IStartupConfigGameSystemSource& params,
    FreeScrollNativeDrawServices& graphics, IEnvironmentRootMemory& memory)
{
    GraphicsSurfaceNativeScope surfaces(graphics.surfaces);
    body.axesAt0C = 0;
    bool missingFirst = true;
    for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) < StartupCount(params); ++i)
    {
        auto* pattern = PatternAt(WindowZero(draw), i);
        if (!pattern->layer.enabledAt0C) continue;
        if (!i) missingFirst = false;
        AppendFreeScrollEntryNativeAt4AE185(body, pattern, memory);
    }
    if (missingFirst) return false;

    auto* first = FirstPattern(draw);
    body.xAt14 = std::bit_cast<std::int32_t>(first->layer.valueAt04);
    body.yAt18 = std::bit_cast<std::int32_t>(first->layer.valueAt08);
    body.xAt14 = Sub(0, body.xAt14);
    body.yAt18 = Sub(0, body.yAt18);
    const RECT bounds = FirstPattern(draw)->boundsAt24;
    const RECT extent = ReadFreeScrollLocalUnionAt4ADDE0(*FirstPattern(draw));
    const auto width = Sub(bounds.right, bounds.left);
    const auto height = Sub(bounds.bottom, bounds.top);
    if (Sub(extent.right, extent.left) > width) body.axesAt0C |= 1;
    if (Sub(extent.bottom, extent.top) > height) body.axesAt0C |= 2;
    body.draggingAt44 = 0;
    body.limitsAt2C = {};
    if (!body.axesAt0C) return true;
    body.limitsAt2C = {extent.left, extent.top, Sub(extent.right, width), Sub(extent.bottom, height)};
    // 每次 canvas 回调后重读 end；传 Ref 地址，不能提前固定条目正文。
    for (auto* slot = body.entriesAt00.begin; slot != body.entriesAt00.end; ++slot)
        CreateFreeScrollCanvasAt4AE574(body, *slot, graphics);
    return true;
}

void EndFreeScrollNativeAt4AC970(FreeScrollNativeBody50h& body,
    IDrawManagerNativeRootSource& draw, const IStartupConfigGameSystemSource& params,
    PatternLayerNativeRenderServices& graphics, FreeScrollNativeInputServices& input)
{
    GraphicsSurfaceNativeScope surfaces(graphics.surfaces);
    for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) < StartupCount(params); ++i)
        if (auto* pattern = PatternAt(WindowZero(draw), i))
            InvalidatePatternLayerNativeAt443650(*pattern, nullptr);
    RestoreLayerWindowPatternsNativeAt4C4330(*WindowZero(draw), graphics);
    if (body.axesAt0C)
    {
        auto* layers = ResolveDrawLayerListAt47AE80(*draw.CaptureDrawManagerAfter413BE0());
        layers->vptrAt00->addDirtyAt20(layers, &body.viewportAt1C);
    }
    ReleaseDrawLayerRangeAt452440(body.entriesAt00.begin, body.entriesAt00.end);
    body.entriesAt00.end = body.entriesAt00.begin;
    body.axesAt0C = 0;
    auto* cursor = ResolveEnvironmentObjectReference<CursorNativeBody10h>(
        input.input.CaptureAt418210()->cursorAt0C);
    using Restore = std::uint8_t(__thiscall*)(CursorNativeBody10h*);
    Restore restore;
    std::memcpy(&restore, reinterpret_cast<const std::byte*>(cursor->vptrAt00) + 8, sizeof(restore));
    InputNativeScope scope(input.inputServices);
    static_cast<void>(restore(cursor));
}
}
