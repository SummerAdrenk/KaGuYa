#include <Runtime/DemoSpriteRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t IntegerPart(const std::uint32_t fixed) noexcept
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(fixed >> 16));
}

std::uint32_t ReplaceIntegerPart(
    const std::uint32_t fixed, const std::int32_t value) noexcept
{
    // 4A7796/4A77E9 只写高 WORD，越过目标时仍保留累加器的小数部分。
    return (fixed & 0xFFFFu) | (static_cast<std::uint32_t>(value) << 16);
}

std::uint32_t ComputeStep(
    const std::int32_t larger, const std::int32_t smaller,
    const std::int32_t duration)
{
    const auto distance = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(larger) - static_cast<std::uint32_t>(smaller));
    // 4BFD7D..4BFDE9：DWORD 相减后符号扩展，64 位除法，只提交商的低 DWORD。
    // 原版 __alldiv 的零除数进入 x86 DIV 异常；显式保留，避免 C++ 除零未定义行为。
    if (duration == 0)
    {
        __asm
        {
            xor eax, eax
            xor edx, edx
            xor ecx, ecx
            div ecx
        }
    }
    return static_cast<std::uint32_t>(
        (static_cast<std::int64_t>(distance) * 65536) / duration);
}

std::int32_t AddWrapping(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
}

DemoSpriteRuntime::DemoSpriteRuntime(IGraphicsSurfaceApi& graphicsApi) noexcept
    : sprite_(graphicsApi)
{
}

SpriteRuntime& DemoSpriteRuntime::Sprite() noexcept { return sprite_; }
const SpriteRuntime& DemoSpriteRuntime::Sprite() const noexcept { return sprite_; }
const DemoSpriteMotionState& DemoSpriteRuntime::Motion() const noexcept { return motion_; }
std::uint8_t DemoSpriteRuntime::VisibleByte() const noexcept { return sprite_.ActiveByte(); }
void DemoSpriteRuntime::SetVisibleByte(const std::uint8_t value) noexcept
{
    sprite_.SetActiveByte(value);
}
std::int32_t DemoSpriteRuntime::PositionX() const noexcept
{
    return std::bit_cast<std::int32_t>(sprite_.OpaqueValueAt2C());
}
std::int32_t DemoSpriteRuntime::PositionY() const noexcept
{
    return std::bit_cast<std::int32_t>(sprite_.OpaqueValueAt30());
}

RECT DemoSpriteRuntime::Bounds() const noexcept
{
    // 4AD080：名称为空或 mode>2 时矩形为空；不依赖可见标志。
    if (sprite_.LogicalName().empty() ||
        static_cast<std::uint32_t>(sprite_.LoadMode()) > 2u)
        return {};
    return {PositionX(), PositionY(),
        AddWrapping(PositionX(), sprite_.Width()),
        AddWrapping(PositionY(), sprite_.Height())};
}

void DemoSpriteRuntime::SetPosition(const std::int32_t x, const std::int32_t y) noexcept
{
    sprite_.SetOpaqueValuesAt2CAnd30(
        static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y));
}

void DemoSpriteRuntime::BeginMotion(
    const std::int32_t duration,
    const std::int32_t startX, const std::int32_t startY,
    const std::int32_t endX, const std::int32_t endY,
    DirtyRectCollection& dirtyRects)
{
    const RECT previous = Bounds();
    if (startX == endX && startY == endY)
        throw 1;

    CommitMotionAt4BFD18(duration, startX, startY, endX, endY);
    dirtyRects.Add(previous);
}

void DemoSpriteRuntime::CommitMotionAt4BFD18(
    const std::int32_t duration,
    const std::int32_t startX, const std::int32_t startY,
    const std::int32_t endX, const std::int32_t endY)
{
    motion_.direction = 0;
    motion_.fixedY = 0;
    motion_.fixedX = 0;
    motion_.stepY = 0;
    motion_.stepX = 0;
    if (startX < endX)
    {
        motion_.direction = 1;
        motion_.stepX = ComputeStep(endX, startX, duration);
    }
    else if (startX > endX)
    {
        motion_.direction = 2;
        motion_.stepX = ComputeStep(startX, endX, duration);
    }
    if (startY < endY)
    {
        motion_.direction |= 4;
        motion_.stepY = ComputeStep(endY, startY, duration);
    }
    else if (startY > endY)
    {
        motion_.direction |= 8;
        motion_.stepY = ComputeStep(startY, endY, duration);
    }

    motion_.fixedY = ReplaceIntegerPart(motion_.fixedY, startY);
    motion_.targetX = endX;
    motion_.fixedX = ReplaceIntegerPart(motion_.fixedX, startX);
    SetPosition(startX, startY);
    motion_.targetY = endY;
}

void DemoSpriteRuntime::Update(
    const RECT& sequenceBounds, DirtyRectCollection& dirtyRects)
{
    if (!sprite_.Active())
        return;

    if (motion_.direction != 0)
    {
        if ((motion_.direction & 2u) != 0)
        {
            motion_.fixedX -= motion_.stepX;
            if (IntegerPart(motion_.fixedX) < motion_.targetX)
                motion_.fixedX = ReplaceIntegerPart(motion_.fixedX, motion_.targetX);
        }
        else if ((motion_.direction & 1u) != 0)
        {
            motion_.fixedX += motion_.stepX;
            if (IntegerPart(motion_.fixedX) > motion_.targetX)
                motion_.fixedX = ReplaceIntegerPart(motion_.fixedX, motion_.targetX);
        }
        if ((motion_.direction & 8u) != 0)
        {
            motion_.fixedY -= motion_.stepY;
            if (IntegerPart(motion_.fixedY) < motion_.targetY)
                motion_.fixedY = ReplaceIntegerPart(motion_.fixedY, motion_.targetY);
        }
        else if ((motion_.direction & 4u) != 0)
        {
            motion_.fixedY += motion_.stepY;
            if (IntegerPart(motion_.fixedY) > motion_.targetY)
                motion_.fixedY = ReplaceIntegerPart(motion_.fixedY, motion_.targetY);
        }

        const std::int32_t x = IntegerPart(motion_.fixedX);
        const std::int32_t y = IntegerPart(motion_.fixedY);
        if (x == motion_.targetX)
            motion_.direction &= 0xCu;
        if (y == motion_.targetY)
            motion_.direction &= 3u;
        if (x != PositionX() || y != PositionY())
        {
            const RECT previous = Bounds();
            SetPosition(x, y);
            const RECT current = Bounds();
            RECT clipped{};
            IntersectRect(&clipped, &previous, &sequenceBounds);
            dirtyRects.Add(clipped);
            IntersectRect(&clipped, &current, &sequenceBounds);
            dirtyRects.Add(clipped);
        }
    }

    // 4A7916..4A79E4：运动更新之后推进 AN；只有解释器返回 0 才追加动画矩形。
    if (!sprite_.LogicalName().empty() &&
        sprite_.LoadMode() == SpriteLoadMode::Animation &&
        sprite_.AdvanceAnimation() == 0)
        dirtyRects.Add(Bounds());
}
}
