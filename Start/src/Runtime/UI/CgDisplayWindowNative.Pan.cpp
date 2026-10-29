#include "CgDisplayWindowNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
void ConfigureCgDisplayWindowPanNativeAt4C17F0(CgDisplayWindowNativeBody60h& body, std::int32_t step)
{
    using namespace CgDisplayWindowNativeDetail;
    RECT united{};
    auto y = Y(body);
    const auto oldX = X(body);
    body.stepAt34 = step;
    for (std::uint32_t i = 0; i < CgDisplayWindowSpriteCountAt4ACF90(body); ++i)
    {
        const auto rect = SpriteBounds(*ResolveCgDisplayWindowSpriteNative(body, i));
        ::UnionRect(&united, &united, &rect);
    }
    const auto width = Sub(body.window.boundsAt14.right, body.window.boundsAt14.left);
    const auto height = Sub(body.window.boundsAt14.bottom, body.window.boundsAt14.top);
    body.axesAt30 = 0;
    if (Sub(united.right, united.left) > width) body.axesAt30 = 1;
    if (Sub(united.bottom, united.top) > height) body.axesAt30 |= 2u;
    body.draggingAt54 = 0;
    body.maximumXAt38 = 0;
    body.maximumYAt3C = 0;
    body.minimumXAt40 = 0;
    body.minimumYAt44 = 0;
    std::int32_t x;
    if (body.axesAt30)
    {
        body.maximumXAt38 = Sub(0, united.left);
        body.maximumYAt3C = Sub(0, united.top);
        body.minimumXAt40 = Sub(width, united.right);
        body.minimumYAt44 = Sub(height, united.bottom);
        // 按原先上界后下界，即使另一轴min>max也不交换或修正范围。
        x = oldX;
        if (x > body.maximumXAt38) x = body.maximumXAt38;
        if (x < body.minimumXAt40) x = body.minimumXAt40;
        if (y > body.maximumYAt3C) y = body.maximumYAt3C;
        if (y < body.minimumYAt44) y = body.minimumYAt44;
    }
    else
    {
        x = Sub(0, united.left);
        y = Sub(0, united.top);
    }
    body.window.layer.valueAt08 = std::uint32_t(y);
    body.window.layer.valueAt04 = std::uint32_t(x);
}
}
