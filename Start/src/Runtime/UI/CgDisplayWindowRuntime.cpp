#include <Runtime/CgDisplayWindowRuntime.h>

#include <algorithm>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::int32_t AddWrapping(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) +
        std::bit_cast<std::uint32_t>(right));
}

[[nodiscard]] std::int32_t SubWrapping(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) -
        std::bit_cast<std::uint32_t>(right));
}

[[nodiscard]] std::int32_t NegateWrapping(
    const std::int32_t value) noexcept
{
    return std::bit_cast<std::int32_t>(
        0u - std::bit_cast<std::uint32_t>(value));
}

[[nodiscard]] const RuntimeGraphicsSurface& SurfaceAt443580(
    const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount()) return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        const auto count = sprite.AuxiliarySurfaceCount(group);
        if (index < count) return sprite.AuxiliarySurfaceAt(group, index);
        index -= count;
    }
    throw std::out_of_range("CCGDispWin 443580 surface reference is missing");
}
}

CgDisplayWindowRuntime::CgDisplayWindowRuntime(
    const std::size_t spriteSlotCount,
    IGraphicsSurfaceApi& surfaceApi,
    LayerList* const layerList,
    ICgDisplayWindowServices& services,
    IGraphicsSurfaceCompositionApi& graphics,
    const std::int32_t screenWidth,
    const std::int32_t screenHeight)
    : LayerItem(layerList),
      services_(&services),
      graphics_(&graphics),
      clipRectAt14_{0, 0, screenWidth, screenHeight}
{
    // 0x499A20 的数组构造发生在 LayerItem 注册之后。reserve/emplace 的异常
    // 会先销毁已构造 Sprite，再由基类析构撤销注册，保持原 EH 顺序。
    sprites_.reserve(spriteSlotCount);
    for (std::size_t index = 0; index < spriteSlotCount; ++index)
        sprites_.emplace_back(surfaceApi);
}

void CgDisplayWindowRuntime::Draw(const RECT& dirtyRect)
{
    static_assert(sizeof(void*) == sizeof(std::int32_t),
        "CCGDispWin runtime requires the original x86 ABI");
    static_cast<void>(DrawAtVslot00(static_cast<std::int32_t>(
        reinterpret_cast<std::intptr_t>(&dirtyRect))));
}

void CgDisplayWindowRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    // 仅在既有地址命名接口边界保留 x86 的 clip 整数载体。
    static_assert(sizeof(void*) == sizeof(std::int32_t),
        "CCGDispWin clip adapter requires x86 pointers");
    static_cast<void>(BlitAtVslot04(destination, static_cast<std::int32_t>(
        reinterpret_cast<std::intptr_t>(destinationClip))));
}

std::size_t CgDisplayWindowRuntime::DrawAtVslot00(
    const std::int32_t argument)
{
    return ComposeSprites(0, argument, true);
}

std::size_t CgDisplayWindowRuntime::BlitAtVslot04(
    const GraphicsSurfaceDestination destination,
    const std::int32_t argument)
{
    return ComposeSprites(destination, argument, false);
}

RECT* CgDisplayWindowRuntime::CopyClipRectAtVslot0C(RECT* const output) const
{
    *output = clipRectAt14_;
    return output;
}

bool CgDisplayWindowRuntime::QueryAtVslot10(
    const std::uint32_t first,
    const std::uint32_t second) const noexcept
{
    static_cast<void>(first);
    static_cast<void>(second);
    return false;
}

void CgDisplayWindowRuntime::DeactivateAtVslot18() noexcept
{
    SetEnabled(false);
}

std::size_t CgDisplayWindowRuntime::ComposeSprites(
    const GraphicsSurfaceDestination destination,
    const std::int32_t argument,
    const bool drawToPrimary)
{
    static_assert(sizeof(void*) == sizeof(std::int32_t),
        "CCGDispWin runtime requires the original x86 ABI");
    const auto* const destinationClip = reinterpret_cast<const RECT*>(
        static_cast<std::intptr_t>(argument));
    for (std::size_t index = 0; index < sprites_.size(); ++index)
    {
        SpriteRuntime& sprite = sprites_[index];
        if (sprite.LogicalName().empty())
            continue;

        // 4B50B1..4B50F9 / 4B5371..4B53B9：回调必须看到临时偏移后的 Sprite。
        const auto savedX = sprite.X();
        const auto savedY = sprite.Y();
        sprite.SetOrigin(AddWrapping(savedX, xAt04_), AddWrapping(savedY, yAt08_));
        const auto compose = [&](const RuntimeGraphicsSurface& surface,
                                 const std::int32_t x,
                                 const std::int32_t y) {
            if (drawToPrimary)
            {
                const auto sourceHandle = surface.Handle();
                if (sourceHandle != 0)
                    static_cast<void>(graphics_->DrawSurface(
                        sourceHandle, x, y, nullptr, nullptr, destinationClip, 0));
            }
            else
            {
                // 4425E6..4425FC：每次提交重新读取，目标零值先于源零值短路。
                const auto targetHandle = destination.Handle();
                if (targetHandle == 0) return;
                const auto sourceHandle = surface.Handle();
                if (sourceHandle == 0) return;
                static_cast<void>(graphics_->BlitSurface(
                    targetHandle, sourceHandle, x, y,
                    nullptr, nullptr, destinationClip, 0));
            }
        };

        if (sprite.LoadMode() == SpriteLoadMode::StaticWithoutAlpha ||
            sprite.LoadMode() == SpriteLoadMode::StaticWithAlpha)
        {
            compose(SurfaceAt443580(sprite, 0), sprite.X(), sprite.Y());
        }
        else if (sprite.LoadMode() == SpriteLoadMode::Animation)
        {
            // 4B5150..4B5186：DLL 回调可能重建映射，不缓存迭代器或长度。
            for (std::size_t frame = 0; frame < sprite.CurrentFrameMappings().size(); ++frame)
            {
                const auto frameIndex = sprite.CurrentFrameMappings()[frame];
                if (frameIndex < 0) continue;
                const auto& surface = SurfaceAt443580(sprite, static_cast<std::size_t>(frameIndex));
                compose(surface, AddWrapping(sprite.X(), surface.OriginX()),
                    AddWrapping(sprite.Y(), surface.OriginY()));
            }
        }
        // 4B5210..4B5219 / 4B54C3..4B54CC：仅正常返回恢复；异常保留已写字段。
        sprites_[index].SetOrigin(savedX, savedY);
    }
    return sprites_.size();
}

void CgDisplayWindowRuntime::WritePositionAt04And08(
    const std::int32_t x,
    const std::int32_t y)
{
    xAt04_ = x;
    yAt08_ = y;
}

std::uint8_t CgDisplayWindowRuntime::ActiveByteAt54() const
{
    return activeByteAt54_;
}

void CgDisplayWindowRuntime::BeginAt4AA860()
{
    // 4AA860 先清 CCGDispWin+0x54，再进入输入对象虚槽 +0x08。
    activeByteAt54_ = 0;
    services_->BeginPointerAtInputVslot08();
}

void CgDisplayWindowRuntime::InvokeAt4C17F0(const std::int32_t value)
{
    // 4C17F0：先求全部 Sprite 的联合矩形，再据画布尺寸建立滚动边界。
    // 空槽贡献空矩形；不能为“没有资源”另设兜底尺寸。
    RECT unionRect{};
    const std::size_t spriteCount = sprites_.size();
    for (std::size_t index = 0; index < spriteCount; ++index)
    {
        RECT spriteRect{};
        const PatternSpriteRectResult drawable =
            sprites_[index].LocalDrawableRect();
        if (drawable.state == PatternSpriteRectState::Valid)
            spriteRect = drawable.rect;
        UnionRect(&unionRect, &unionRect, &spriteRect);
    }

    panState_.stepAt34 = value;
    panState_.flagsAt30 = 0u;
    const std::int32_t clipWidth =
        SubWrapping(clipRectAt14_.right, clipRectAt14_.left);
    const std::int32_t clipHeight =
        SubWrapping(clipRectAt14_.bottom, clipRectAt14_.top);
    if (SubWrapping(unionRect.right, unionRect.left) > clipWidth)
    {
        panState_.flagsAt30 = 1u;
    }
    if (SubWrapping(unionRect.bottom, unionRect.top) > clipHeight)
    {
        panState_.flagsAt30 |= 2u;
    }

    activeByteAt54_ = 0u;
    panState_.maximumXAt38 = 0;
    panState_.maximumYAt3C = 0;
    panState_.minimumXAt40 = 0;
    panState_.minimumYAt44 = 0;
    if (panState_.flagsAt30 != 0u)
    {
        panState_.maximumXAt38 = NegateWrapping(unionRect.left);
        panState_.maximumYAt3C = NegateWrapping(unionRect.top);
        panState_.minimumXAt40 =
            SubWrapping(clipWidth, unionRect.right);
        panState_.minimumYAt44 =
            SubWrapping(clipHeight, unionRect.bottom);
        xAt04_ = ClampX(xAt04_);
        yAt08_ = ClampY(yAt08_);
    }
    else
    {
        xAt04_ = NegateWrapping(unionRect.left);
        yAt08_ = NegateWrapping(unionRect.top);
    }
}

void CgDisplayWindowRuntime::InvokeAt4C13A0(const std::int32_t value)
{
    // 4C13A0：只有非零参数且拖拽仍活动时才恢复进入拖拽前的 registry 值。
    if (value != 0 && activeByteAt54_ != 0u)
        services_->WriteRegistrySlot2DwordAt04(savedRegistryValueAt50_);

    const std::uint32_t flags = panState_.flagsAt30;
    activeByteAt54_ = 0u;
    if ((flags & 3u) != 0u)
        services_->ConfigurePointerAtInputVslot04(207, 0, 0);
    else
        services_->BeginPointerAtInputVslot08();
}

std::int32_t CgDisplayWindowRuntime::ClampX(const std::int32_t value) const noexcept
{
    return (std::min)((std::max)(value, panState_.minimumXAt40),
        panState_.maximumXAt38);
}

std::int32_t CgDisplayWindowRuntime::ClampY(const std::int32_t value) const noexcept
{
    return (std::min)((std::max)(value, panState_.minimumYAt44),
        panState_.maximumYAt3C);
}

void CgDisplayWindowRuntime::UpdateAt4BD050()
{
    if ((panState_.flagsAt30 & 3u) == 0u)
        return;

    services_->ConfigurePointerAtInputVslot04(
        activeByteAt54_ != 0u ? 208 : 207, 0, 0);

    const std::int32_t oldX = xAt04_;
    const std::int32_t oldY = yAt08_;
    std::int32_t nextX = oldX;
    std::int32_t nextY = oldY;

    if (activeByteAt54_ != 0u)
    {
        if (!services_->PrimaryPressedAt10() ||
            services_->PrimarySuppressionAt28() != 0u ||
            !services_->PointerIsInsideClient())
        {
            InvokeAt4C13A0(1);
        }

        if (activeByteAt54_ != 0u)
        {
            const POINT pointer = services_->PointerPositionAt18();
            nextX = ClampX(AddWrapping(nextX,
                SubWrapping(pointer.x, previousPointerAt58_.x)));
            nextY = ClampY(AddWrapping(nextY,
                SubWrapping(pointer.y, previousPointerAt58_.y)));
            previousPointerAt58_ = pointer;
        }
    }
    else if (!services_->PrimaryPreviousPressedAt30() &&
             services_->PrimaryPressedAt10() &&
             services_->PrimarySuppressionAt28() == 0u)
    {
        previousPointerAt58_ = services_->PointerPositionAt18();
        activeByteAt54_ = 1u;
        savedRegistryValueAt50_ = services_->ReadRegistrySlot2DwordAt04();
        services_->WriteRegistrySlot2DwordAt04(0u);
    }

    if (oldX == nextX && oldY == nextY)
    {
        if (services_->ReadInputEdgeAt47F4E0(1) ||
            services_->ReadInputEdgeAt47F4E0(2) ||
            services_->ReadInputEdgeAt47F4E0(3) ||
            services_->ReadInputEdgeAt47F4E0(4))
        {
            repeatCounterAt4C_ = 0;
            repeatDelayAt48_ = 10;
            InvokeAt4C13A0(1);
        }

        std::int32_t direction = -1;
        if (services_->ReadInputHeldAt4198D0(1))
            direction = 0;
        else if (services_->ReadInputHeldAt4198D0(2))
            direction = 1;
        else if (services_->ReadInputHeldAt4198D0(3))
            direction = 2;
        else if (services_->ReadInputHeldAt4198D0(4))
            direction = 3;
        else
            repeatCounterAt4C_ = 1;

        if (repeatCounterAt4C_ != 0)
        {
            --repeatCounterAt4C_;
        }
        else
        {
            switch (direction)
            {
            case 0:
                nextY = ClampY(AddWrapping(nextY, panState_.stepAt34));
                break;
            case 1:
                nextY = ClampY(SubWrapping(nextY, panState_.stepAt34));
                break;
            case 2:
                nextX = ClampX(AddWrapping(nextX, panState_.stepAt34));
                break;
            case 3:
                nextX = ClampX(SubWrapping(nextX, panState_.stepAt34));
                break;
            default:
                break;
            }
            repeatCounterAt4C_ = repeatDelayAt48_;
            repeatDelayAt48_ = 0;
        }
    }

    if (oldX == nextX && oldY == nextY)
    {
        if ((panState_.flagsAt30 & 2u) != 0u)
        {
            if (services_->ReadOpaqueAxisStateAt44(
                    CgDisplayOpaqueAxisSource::At418210) == 0)
            {
                const std::int32_t axis = services_->ReadOpaqueAxisValueAt52(
                    CgDisplayOpaqueAxisSource::At418210);
                if (axis < 0)
                {
                    nextY = ClampY(SubWrapping(nextY, panState_.stepAt34));
                    InvokeAt4C13A0(1);
                }
                else if (axis > 0)
                {
                    nextY = ClampY(AddWrapping(nextY, panState_.stepAt34));
                    InvokeAt4C13A0(1);
                }
            }
        }
        else if ((panState_.flagsAt30 & 1u) != 0u &&
                 services_->ReadOpaqueAxisStateAt44(
                     CgDisplayOpaqueAxisSource::At4AD740) == 0)
        {
            const std::int32_t axis = services_->ReadOpaqueAxisValueAt52(
                CgDisplayOpaqueAxisSource::At4AD740);
            if (axis < 0)
            {
                nextX = ClampX(SubWrapping(nextX, panState_.stepAt34));
                InvokeAt4C13A0(1);
            }
            else if (axis > 0)
            {
                nextX = ClampX(AddWrapping(nextX, panState_.stepAt34));
                InvokeAt4C13A0(1);
            }
        }
    }

    if (oldX != nextX || oldY != nextY)
    {
        xAt04_ = nextX;
        yAt08_ = nextY;
        services_->SubmitDirtyRect(clipRectAt14_);
    }
}

void CgDisplayWindowRuntime::SubmitClippedSpriteRect(
    const SpriteRuntime& sprite)
{
    const PatternSpriteRectResult drawable = sprite.LocalDrawableRect();
    if (drawable.state != PatternSpriteRectState::Valid)
        return;

    RECT rect = drawable.rect;
    ::OffsetRect(&rect, xAt04_, yAt08_);
    RECT clipped{};
    ::IntersectRect(&clipped, &rect, &clipRectAt14_);
    if (!::IsRectEmpty(&clipped))
        services_->SubmitDirtyRect(clipped);
}

void CgDisplayWindowRuntime::UpdateAnimationAndDirtyRectsAtVslot14()
{
    if (!Enabled())
        return;

    for (std::size_t index = 0;
         index < sprites_.size(); ++index)
    {
        SpriteRuntime& sprite = sprites_[index];
        if (sprite.LogicalName().empty() ||
            sprite.LoadMode() != SpriteLoadMode::Animation)
        {
            continue;
        }

        const PatternSpriteRectResult oldDrawable = sprite.LocalDrawableRect();
        if (sprite.AdvanceAnimation() != 0)
            continue;

        if (oldDrawable.state == PatternSpriteRectState::Valid)
        {
            RECT oldRect = oldDrawable.rect;
            ::OffsetRect(&oldRect, xAt04_, yAt08_);
            RECT clipped{};
            ::IntersectRect(&clipped, &oldRect, &clipRectAt14_);
            if (!::IsRectEmpty(&clipped))
                services_->SubmitDirtyRect(clipped);
        }
        SubmitClippedSpriteRect(sprite);
    }
}

std::size_t CgDisplayWindowRuntime::SpriteSlotCount() const noexcept
{
    return sprites_.size();
}

SpriteRuntime& CgDisplayWindowRuntime::SpriteAt(const std::size_t slotIndex)
{
    return sprites_.at(slotIndex);
}

const SpriteRuntime& CgDisplayWindowRuntime::SpriteAt(
    const std::size_t slotIndex) const
{
    return sprites_.at(slotIndex);
}

void CgDisplayWindowRuntime::SetWindowActiveAt0C(const bool active) noexcept
{
    SetEnabled(active);
}

bool CgDisplayWindowRuntime::WindowActiveAt0C() const noexcept
{
    return Enabled();
}

void CgDisplayWindowRuntime::ConfigurePanStateAt30(
    const CgDisplayWindowPanState& state) noexcept
{
    panState_ = state;
}

const CgDisplayWindowPanState&
CgDisplayWindowRuntime::PanStateAt30() const noexcept
{
    return panState_;
}

std::int32_t CgDisplayWindowRuntime::XAt04() const noexcept { return xAt04_; }
std::int32_t CgDisplayWindowRuntime::YAt08() const noexcept { return yAt08_; }
std::int32_t CgDisplayWindowRuntime::RepeatDelayAt48() const noexcept
{
    return repeatDelayAt48_;
}
std::int32_t CgDisplayWindowRuntime::RepeatCounterAt4C() const noexcept
{
    return repeatCounterAt4C_;
}
std::uint32_t CgDisplayWindowRuntime::SavedRegistryValueAt50() const noexcept
{
    return savedRegistryValueAt50_;
}
POINT CgDisplayWindowRuntime::PreviousPointerAt58() const noexcept
{
    return previousPointerAt58_;
}
}
