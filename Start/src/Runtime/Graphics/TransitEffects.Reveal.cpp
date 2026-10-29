#include "TransitEffects.h"

namespace KaGuYa::Start::Runtime::TransitEffects
{
std::uint8_t CopyWhole(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    const auto source = Source(p).handleAt04;
    if (p.targetKind == 0)
        return graphics.TransitCopyDraw(source, p.x, p.y, nullptr, nullptr, p.clip, 0);
    return graphics.TransitCopyBlit(p.destination, source, p.x, p.y, nullptr, nullptr, p.clip, 0);
}

void DrawRect(const TransitOperation& p, IGraphicsTransitApi& graphics, const RECT& rect)
{
    // 每个原switch分支临调用才读source句柄；未知targetKind不读句柄且不绘制。
    switch (p.targetKind)
    {
    case 0:
        static_cast<void>(graphics.TransitCopyDraw(Source(p).handleAt04, Add(p.x, rect.left),
            Add(p.y, rect.top), nullptr, &rect, p.clip, 0));
        break;
    case 1:
        static_cast<void>(graphics.TransitCopyBlit(p.destination, Source(p).handleAt04, Add(p.x, rect.left),
            Add(p.y, rect.top), nullptr, &rect, p.clip, 0));
        break;
    case 2:
        static_cast<void>(graphics.TransitBlendDraw(Source(p).handleAt04, Add(p.x, rect.left),
            Add(p.y, rect.top), p.current, nullptr, &rect, p.clip, 0));
        break;
    case 3:
        static_cast<void>(graphics.TransitBlendBlit(p.destination, Source(p).handleAt04, Add(p.x, rect.left),
            Add(p.y, rect.top), p.current, nullptr, &rect, p.clip, 0));
        break;
    }
}

namespace
{
std::uint8_t Interlace(const TransitOperation& p, IGraphicsTransitApi& graphics, const bool columns)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);

    const auto& source = Source(p);
    const auto count = columns ? source.widthAt08 : source.heightAt0C;
    const auto length = columns ? source.heightAt0C : source.widthAt08;
    // 两个交错分支使用SAR8，非除255；负乘积仍按x86符号移位。
    const auto scaled = Mul(p.current, length) >> 8;
    if (count <= 0) return 1;
    const auto firstEnd = scaled <= length ? scaled : length;
    const auto candidate = Sub(Sub(length, scaled), 1);
    const auto secondStart = candidate >= 0 ? candidate : 0;
    const auto secondLength = Add(secondStart, length) <= length ? length : Sub(length, secondStart);
    const auto secondEnd = Add(secondStart, secondLength);

    for (std::int32_t index = 0; index < count; index = Add(index, 2))
    {
        const auto first = index >= 0 ? index : 0;
        const auto firstSize = Add(first, 1) > count ? Sub(count, first) : 1;
        const RECT firstRect = columns ? RECT{first, 0, Add(first, firstSize), firstEnd}
                                      : RECT{0, first, firstEnd, Add(first, firstSize)};
        DrawRect(p, graphics, firstRect);
        const auto next = Add(index, 1);
        const auto second = next >= 0 ? next : 0;
        const auto secondSize = Add(second, 1) > count ? Sub(count, second) : 1;
        const RECT secondRect = columns ? RECT{second, secondStart, Add(second, secondSize), secondEnd}
                                       : RECT{secondStart, second, secondEnd, Add(second, secondSize)};
        // 奇数维度的最后一条空矩形也照原样调用，不能自行滤掉。
        DrawRect(p, graphics, secondRect);
    }
    return 1;
}
}

std::uint8_t RevealAt44B3C0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);
    const auto& source = Source(p);
    const auto width = source.widthAt08;
    const auto height = source.heightAt0C;
    const auto half = height / 2;
    const auto candidate = Sub(half, height);
    const auto top = candidate >= 0 ? candidate : 0;
    const auto scaled = Mul(2, Mul(p.current, half) / 255);
    const auto extent = Add(top, scaled) <= height ? scaled : Sub(height, top);
    const RECT rect{0, top, width, Add(top, extent)};
    DrawRect(p, graphics, rect);
    // 中间帧固定AL=1；仅开头的完整复制分支返回DLL的AL。
    return 1;
}

std::uint8_t InterlaceAt44B5A0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    return Interlace(p, graphics, false);
}

std::uint8_t InterlaceAt44B8C0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    return Interlace(p, graphics, true);
}

std::uint8_t StripedReveal(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const bool columns, const bool reverse, const std::int32_t phaseMask, const std::int32_t thresholdShift)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    const auto count = columns ? width : height;
    for (std::int32_t index = 0; index < count; index = Add(index, 1))
    {
        const auto phase = index & phaseMask;
        if ((reverse ? phaseMask - phase : phase) > (p.current >> thresholdShift)) continue;
        // 13..20/57..60仅支持Copy目标0/1，不能沿用3/4/5的Blend目标2/3。
        if (p.targetKind != 0 && p.targetKind != 1) continue;
        const RECT rect = columns ? RECT{index, 0, Add(index, 1), height}
                                  : RECT{0, index, width, Add(index, 1)};
        DrawRect(p, graphics, rect);
    }
    return 1;
}

std::uint8_t OpposingLines(const TransitOperation& p, IGraphicsTransitApi& graphics, const bool columns)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);
    const auto& source = Source(p);
    const auto length = columns ? source.widthAt08 : source.heightAt0C;
    const auto breadth = columns ? source.heightAt0C : source.widthAt08;
    const auto count = Mul(p.current, length) >> 8;
    // 44C710/44C9F0：每次正向一行和反向一行，index仍递增2，不合并重叠行。
    for (std::int32_t index = 0; index < count; index = Add(index, 2))
    {
        const auto first = index >= 0 ? index : 0;
        const auto firstSize = Add(first, 1) > length ? Sub(length, first) : 1;
        const RECT firstRect = columns ? RECT{first, 0, Add(first, firstSize), breadth}
                                      : RECT{0, first, breadth, Add(first, firstSize)};
        DrawRect(p, graphics, firstRect);
        const auto candidate = Sub(Sub(length, index), 1);
        const auto second = candidate >= 0 ? candidate : 0;
        const auto secondSize = Add(second, 1) > length ? Sub(length, second) : 1;
        const RECT secondRect = columns ? RECT{second, 0, Add(second, secondSize), breadth}
                                       : RECT{0, second, breadth, Add(second, secondSize)};
        DrawRect(p, graphics, secondRect);
    }
    return 1;
}

std::uint8_t TwoPhaseInterlace(const TransitOperation& p, IGraphicsTransitApi& graphics, const bool columns)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);
    const auto& source = Source(p);
    const auto count = columns ? source.widthAt08 : source.heightAt0C;
    const auto length = columns ? source.heightAt0C : source.widthAt08;
    if (count <= 0) return 1;
    const auto emit = [&](std::int32_t index, std::int32_t start, std::int32_t end)
    {
        const auto line = index >= 0 ? index : 0;
        const auto size = Add(line, 1) > count ? Sub(count, line) : 1;
        const RECT rect = columns ? RECT{line, start, Add(line, size), end}
                                  : RECT{start, line, end, Add(line, size)};
        DrawRect(p, graphics, rect);
    };
    const auto endpoints = [&](std::int32_t scaled)
    {
        const auto firstEnd = scaled <= length ? scaled : length;
        const auto candidate = Sub(Sub(length, scaled), 1);
        const auto start = candidate >= 0 ? candidate : 0;
        const auto extent = Add(start, length) <= length ? length : Sub(length, start);
        return std::array<std::int32_t, 3>{firstEnd, start, Add(start, extent)};
    };
    const auto firstPhase = endpoints(Mul(p.current, length) >> 7);
    for (std::int32_t index = 0; index < count; index = Add(index, 4))
    {
        emit(index, 0, firstPhase[0]);
        emit(Add(index, 2), firstPhase[1], firstPhase[2]);
    }
    if (p.current > 128)
    {
        const auto secondPhase = endpoints(Mul(length, Sub(p.current, 128)) >> 7);
        // 原第二轮计数=(uint32(count-1)>>2)+1；即使末尾1/3行越界仍提交矩形。
        const auto groups = (static_cast<std::uint32_t>(Sub(count, 1)) >> 2) + 1;
        std::int32_t index = 1;
        for (std::uint32_t group = 0; group < groups; ++group, index = Add(index, 4))
        {
            emit(index, 0, secondPhase[0]);
            emit(Add(index, 2), secondPhase[1], secondPhase[2]);
        }
    }
    return 1;
}
}
