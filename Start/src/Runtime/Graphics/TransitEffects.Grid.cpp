#include "TransitEffects.h"

namespace KaGuYa::Start::Runtime::TransitEffects
{
std::uint8_t TileRevealAt44DDA0(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::int32_t step, const std::int32_t size)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    for (std::int32_t y = 0; y < height; y = Add(y, step))
        for (std::int32_t x = 0; x < width; x = Add(x, step))
        {
            const auto left = x >= 0 ? x : 0;
            const auto top = y >= 0 ? y : 0;
            const auto w = Add(left, size) > width ? Sub(width, left) : size;
            const auto h = Add(top, size) > height ? Sub(height, top) : size;
            // 44DE80..44DF79：尺寸为零也提交；先遍历横向，再进入下一行。
            DrawRect(p, graphics, RECT{left, top, Add(left, w), Add(top, h)});
        }
    return 1;
}

std::uint8_t SlidingCopy(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const bool vertical, const bool negative)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto length = vertical ? Source(p).heightAt0C : Source(p).widthAt08;
    const auto delta = Sub(length, Mul(p.current, length) >> 8);
    const auto position = negative ? Sub(0, delta) : delta;
    // 44E5B8/44E6A5：运动轴直接取计算坐标，不叠加传入x/y；完整帧才用原坐标。
    const auto x = vertical ? p.x : position;
    const auto y = vertical ? position : p.y;
    if (p.targetKind == 0)
        static_cast<void>(graphics.TransitCopyDraw(Source(p).handleAt04, x, y, nullptr, nullptr, p.clip, 0));
    else if (p.targetKind == 1)
        static_cast<void>(graphics.TransitCopyBlit(p.destination, Source(p).handleAt04, x, y,
            nullptr, nullptr, p.clip, 0));
    return 1;
}

std::uint8_t StaggeredBands(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const bool vertical, const bool forward, const std::int32_t size)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    const auto length = vertical ? height : width;
    const auto count = Add(Add(length, 1), size) / size;
    // 44E93F/44EB0C先修正负数再SAR8，对应有符号除256，不能直接算术右移。
    const auto progress = Mul(p.current, Add(count, size)) / 256;
    std::int32_t start = 0, end = length, remaining = progress;
    for (std::int32_t index = 0; index < count && index < progress; index = Add(index, 1))
    {
        const auto extent = remaining <= size ? remaining : size;
        auto first = forward ? start : Sub(end, extent);
        auto last = forward ? Add(start, extent) : end;
        // 原代码仅在 >length 时取 length-1，且保留空/反向矩形。
        if (last > length) last = Sub(length, 1);
        if (first < 0) first = 0;
        if (p.targetKind == 0 || p.targetKind == 1)
            DrawRect(p, graphics, vertical ? RECT{0, first, width, last} : RECT{first, 0, last, height});
        start = Add(start, size);
        end = Sub(end, size);
        remaining = Sub(remaining, 1);
    }
    return 1;
}
}
