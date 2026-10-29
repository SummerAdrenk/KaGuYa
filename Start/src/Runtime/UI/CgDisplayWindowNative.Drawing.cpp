#include "CgDisplayWindowNative.Internal.h"
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/SpriteAnimationNativeExecution.h>

namespace KaGuYa::Start::Runtime
{
namespace CgDisplayWindowNativeDetail
{
std::uint32_t Render(Body& body, GraphicsSurfaceNativeBody* target, const RECT* clip,
    IGraphicsWindowSurfaceApi* drawing, bool primary)
{
    std::uint32_t index = 0;
    for (; index < CgDisplayWindowSpriteCountAt4ACF90(body); ++index)
    {
        auto* sprite = ResolveCgDisplayWindowSpriteNative(body, index);
        if (!sprite->logicalNameAt0C.GetLength()) continue;
        const auto savedX = sprite->xAt2C;
        const auto savedY = sprite->yAt30;
        sprite->xAt2C = Add(savedX, X(body));
        sprite->yAt30 = Add(savedY, Y(body));
        // 原静态0/1、动态2和未知模式分支由Sprite Native原核执行。
        // 异常直接外传，不用guard恢复临时坐标，保留原部分提交。
        if (sprite->modeAt04 <= 2u)
        {
            auto& graphics = drawing ? *drawing : Services().drawing;
            if (primary) (void)DrawSpriteNativeAt4768D0(*sprite, clip, graphics);
            else (void)CompositeSpriteNativeAt46B390(*sprite, target, clip, graphics);
        }
        sprite = ResolveCgDisplayWindowSpriteNative(body, index);
        sprite->xAt2C = savedX;
        sprite->yAt30 = savedY;
    }
    return CgDisplayWindowSpriteCountAt4ACF90(body);
}
}
std::uint32_t DrawCgDisplayWindowNativeAt4B5300(CgDisplayWindowNativeBody60h& body,
    const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{ return CgDisplayWindowNativeDetail::Render(body, nullptr, clip, &drawing, true); }
std::uint32_t CompositeCgDisplayWindowNativeAt4B5040(CgDisplayWindowNativeBody60h& body,
    GraphicsSurfaceNativeBody* target, const RECT* clip, IGraphicsWindowSurfaceApi& drawing)
{ return CgDisplayWindowNativeDetail::Render(body, target, clip, &drawing, false); }
void AdvanceCgDisplayWindowNativeAt4A7470(CgDisplayWindowNativeBody60h& body)
{
    using namespace CgDisplayWindowNativeDetail;
    if (!body.window.layer.enabledAt0C) return;
    for (std::uint32_t index = 0; index < CgDisplayWindowSpriteCountAt4ACF90(body); ++index)
    {
        auto before = SpriteBounds(*ResolveCgDisplayWindowSpriteNative(body, index));
        auto* const sprite = ResolveCgDisplayWindowSpriteNative(body, index);
        if (!sprite->logicalNameAt0C.GetLength() || sprite->modeAt04 != 2u) continue;
        if (AdvanceSpriteAnimationNativeAt443740(sprite->animationAt34) != 0) continue;
        ::OffsetRect(&before, X(body), Y(body));
        RECT clipped{};
        ::CopyRect(&clipped, &before);
        ::IntersectRect(&clipped, &clipped, &body.window.boundsAt14);
        if (!::IsRectEmpty(&clipped)) AddParentDirty(body, &clipped);
        // dirty虚调用后必须重新解析数组/坐标，不沿用旧Sprite receiver。
        auto after = SpriteBounds(*ResolveCgDisplayWindowSpriteNative(body, index));
        ::OffsetRect(&after, X(body), Y(body));
        ::CopyRect(&clipped, &after);
        ::IntersectRect(&clipped, &clipped, &body.window.boundsAt14);
        if (!::IsRectEmpty(&clipped)) AddParentDirty(body, &clipped);
    }
}
}
