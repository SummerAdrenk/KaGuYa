#include <Runtime/PatternLayerNative.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Count(const YtlReferenceSlotRuntime& slot) noexcept
{
    return slot.controlAt08 ? static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 : 0;
}
template<class T> T* Element(const YtlReferenceSlotRuntime& slot, std::uint32_t index) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    return reinterpret_cast<T*>(slot.baseAt00 + (slot.biasAt04 + index) * stride);
}
template<class T> T& Required(T* pointer)
{
    return *pointer;
}
std::int32_t Add(std::int32_t first, std::int32_t second) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second));
}
void Blit(GraphicsSurfaceNativeBody* target, GraphicsSurfaceNativeBody* source,
    std::int32_t x, std::int32_t y, const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    // 4425C0先目标句柄、后源句柄；空目标句柄可短路尚未存在的源正文。
    const auto destination = Required(target).handleAt04;
    if (!destination) return;
    const auto handle = Required(source).handleAt04;
    if (handle) (void)drawing.SurfaceBlitRaw(destination, handle, x, y, nullptr, nullptr, clip, 0);
}
}

GraphicsSurfaceNativeBody* ResolveSpriteSurfaceNativeAt443580(
    const SpriteNativeBodyA0h& body, std::int32_t index) noexcept
{
    std::uint32_t preceding = 0;
    if (body.primarySurfacesAt88.baseAt00)
    {
        preceding = Count(body.primarySurfacesAt88);
        if (index < std::bit_cast<std::int32_t>(preceding))
            return Element<GraphicsSurfaceNativeBody>(body.primarySurfacesAt88, static_cast<std::uint32_t>(index));
    }
    // 4435E3比较指针是否相等；辅助组没有提前过滤base或先用现代vector扁平化。
    const auto* const end = body.auxiliarySurfacesAt94.end;
    for (auto* slot = body.auxiliarySurfacesAt94.begin; slot != end; ++slot)
    {
        const auto count = Count(*slot);
        if (index < std::bit_cast<std::int32_t>(preceding + count))
            return Element<GraphicsSurfaceNativeBody>(*slot, static_cast<std::uint32_t>(index) - preceding);
        preceding += Count(*slot);
    }
    return nullptr;
}

void RefreshPatternMainNativeAt444230(PatternLayerNativeBody80h& body,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{
    const auto& main = Required(ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18));
    const auto handle = main.handleAt04;
    if (handle) drawing.SurfaceClearRaw(handle, main.clearColorAt20, clip);
    // 4172B0首次与每轮尾部重新查询控制块count；循环用JB而非有符号比较。
    for (std::uint32_t index = 0; index < Count(body.spritesAt34); ++index)
    {
        auto* sprite = Element<SpriteNativeBodyA0h>(body.spritesAt34, index);
        if (!Required(sprite).logicalNameAt0C.GetLength()) continue;
        sprite = Element<SpriteNativeBodyA0h>(body.spritesAt34, index);
        const auto oldX = Required(sprite).xAt2C;
        const auto oldY = sprite->yAt30;
        const auto drawX = Add(oldX, std::bit_cast<std::int32_t>(body.layer.valueAt04));
        const auto drawY = Add(oldY, std::bit_cast<std::int32_t>(body.layer.valueAt08));
        sprite = Element<SpriteNativeBodyA0h>(body.spritesAt34, index);
        Required(sprite).xAt2C = drawX;
        sprite->yAt30 = drawY;
        // 44434F/444351固定本次Sprite和main正文；Blt回调换成员后本次循环不换receiver。
        sprite = Element<SpriteNativeBodyA0h>(body.spritesAt34, index);
        auto* const target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.mainAt18);
        if (Required(sprite).logicalNameAt0C.GetLength())
        {
            const auto mode = std::bit_cast<std::int32_t>(sprite->modeAt04);
            if (mode >= 0 && mode <= 1)
            {
                Blit(target, ResolveSpriteSurfaceNativeAt443580(*sprite, 0),
                    sprite->xAt2C, sprite->yAt30, clip, drawing);
            }
            else if (mode == 2)
            {
                for (std::int32_t mappingIndex = 0;
                    mappingIndex < std::bit_cast<std::int32_t>(Count(sprite->animationAt34.currentAt18));
                    ++mappingIndex)
                {
                    const auto mapping = Required(Element<std::int32_t>(sprite->animationAt34.currentAt18,
                        static_cast<std::uint32_t>(mappingIndex)));
                    if (mapping < 0) continue;
                    auto* const origin = ResolveSpriteSurfaceNativeAt443580(*sprite, mapping);
                    const auto x = Add(sprite->xAt2C, Required(origin).originXAt10);
                    const auto y = Add(sprite->yAt30, origin->originYAt14);
                    auto* const source = ResolveSpriteSurfaceNativeAt443580(*sprite, mapping);
                    Blit(target, source, x, y, clip, drawing);
                }
            }
        }
        // 44441D重新解析当前成员；不是scope guard，绘制抛出时不恢复坐标。
        sprite = Element<SpriteNativeBodyA0h>(body.spritesAt34, index);
        Required(sprite).xAt2C = oldX;
        sprite->yAt30 = oldY;
    }
}
}
