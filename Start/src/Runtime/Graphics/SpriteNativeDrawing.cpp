#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/PatternLayerNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Count(const YtlReferenceSlotRuntime& reference) noexcept
{ return reference.controlAt08 ? static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 : 0; }
std::int32_t* Mapping(const YtlReferenceSlotRuntime& reference, std::uint32_t index) noexcept
{
    if (!reference.baseAt00) return nullptr;
    const auto stride = reference.controlAt08 ? static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C : 0;
    return reinterpret_cast<std::int32_t*>(reference.baseAt00 + (reference.biasAt04 + index) * stride);
}
std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
template<class Submit>
std::int32_t Render(SpriteNativeBodyA0h& sprite, Submit&& submit)
{
    if (!sprite.logicalNameAt0C.GetLength()) return 1;
    const auto mode = std::bit_cast<std::int32_t>(sprite.modeAt04);
    if (mode >= 0 && mode <= 1)
        submit(ResolveSpriteSurfaceNativeAt443580(sprite, 0), sprite.xAt2C, sprite.yAt30);
    else if (mode == 2)
    {
        for (std::int32_t i = 0; i < std::bit_cast<std::int32_t>(Count(sprite.animationAt34.currentAt18)); ++i)
        {
            const auto mapping = *Mapping(sprite.animationAt34.currentAt18, static_cast<std::uint32_t>(i));
            if (mapping < 0) continue;
            auto* const origin = ResolveSpriteSurfaceNativeAt443580(sprite, mapping);
            const auto x = Add(sprite.xAt2C, origin->originXAt10);
            const auto y = Add(sprite.yAt30, origin->originYAt14);
            // 476962/46B422再次解析原Sprite；不把origin对象当作已捕获的绘制源。
            auto* const frame = ResolveSpriteSurfaceNativeAt443580(sprite, mapping);
            submit(frame, x, y);
        }
    }
    else return 3;
    return 0;
}
}
std::int32_t DrawSpriteNativeAt4768D0(SpriteNativeBodyA0h& sprite, const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    return Render(sprite, [&](GraphicsSurfaceNativeBody* frame, std::int32_t x, std::int32_t y) {
        const auto handle = frame->handleAt04;
        if (handle) (void)drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, clip, 0);
    });
}
std::int32_t CompositeSpriteNativeAt46B390(SpriteNativeBodyA0h& sprite, GraphicsSurfaceNativeBody* target,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    return Render(sprite, [&](GraphicsSurfaceNativeBody* frame, std::int32_t x, std::int32_t y) {
        // 4425C0先读目标；空目标handle不读取尚未存在的源Surface。
        const auto destination = target->handleAt04;
        if (!destination) return;
        const auto source = frame->handleAt04;
        if (source) (void)drawing.SurfaceBlitRaw(destination, source, x, y, nullptr, nullptr, clip, 0);
    });
}
}
