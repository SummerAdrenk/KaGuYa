#include <Runtime/CgUiButtonRuntime.h>

#include <limits>
#include <mbstring.h>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] constexpr std::int32_t WrapAdd32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(left) +
        static_cast<std::uint32_t>(right));
}
}

CgUiButtonRuntime::CgUiButtonRuntime(
    ICgUiButtonRuntimeServices& services,
    IGraphicsSurfaceCompositionApi& graphics) noexcept
    : services_(&services),
      graphics_(&graphics)
{
}

std::int32_t CgUiButtonRuntime::ProcessAt4A98E0(const bool forced)
{
    // 0x4A990C：没有资源引用时不进入任何输入状态。
    if (ownedSurfaces_.empty())
        return 0;
    if (stateAt04_ < 0 || stateAt04_ == 3 ||
        (stateAt04_ == 4 && configuration_.inputModeAt1C == 2))
    {
        return 0;
    }
    if (stateAt04_ > 6)
        throw std::out_of_range("CGUIButton +0x04 state exceeds seven surface slots");

    if (!inputLatchAt20_)
    {
        if (services_->HasTimelineActivityAt47F2A0(
                configuration_.timelineIndexAt68))
        {
            stateAt04_ = 2;
            inputLatchAt20_ = true;
            return 1;
        }
    }
    else
    {
        if (services_->HasTimelineActivityAt47F2A0(
                configuration_.timelineIndexAt68))
        {
            return 1;
        }
        stateAt04_ = 0;
        inputLatchAt20_ = false;
        return 2;
    }

    const std::int32_t stateBeforeHitTest = stateAt04_;
    stateAt04_ = configuration_.inputModeAt1C != 0 && stateBeforeHitTest >= 4
        ? 4
        : 0;

    if (!forced)
    {
        const POINT pointer = services_->ReadPointerAt47B850();
        const RECT naturalRect = VisibleRect();
        const RECT& configuredRect = configuration_.explicitHitRectAt0C;
        const bool configuredRectEmpty =
            configuredRect.left >= configuredRect.right ||
            configuredRect.top >= configuredRect.bottom;
        const RECT hitRect = configuration_.pixelHitTestAt08 || configuredRectEmpty
            ? naturalRect
            : configuredRect;
        if (PtInRect(&hitRect, pointer) == FALSE)
            return 0;

        if (configuration_.pixelHitTestAt08)
        {
            // 0x4A9A64 使用归一化前保存的状态来选择资源槽。
            auto surface = ResolveSurfaceSlotAt4ADED0(
                configuration_.stateSurfaceSlotIndicesAt40[
                    static_cast<std::size_t>(stateBeforeHitTest)]);
            // 0x4A9A78：alpha 命中路径才允许回退到资源槽 0。
            if (!surface)
                surface = ResolveSurfaceSlotAt4ADED0(0);
            if (!surface)
            {
                throw std::runtime_error(
                    "CGUIButton alpha hit test could not resolve state slot or slot 0");
            }

            if (surface->hasAlpha)
            {
                const POINT position = DrawPosition(*surface);
                const std::uint32_t pixel =
                    services_->ReadSurfacePixelAtGetSurfacePixel(
                        surface->handle,
                        pointer.x - position.x,
                        pointer.y - position.y);
                if ((pixel & 0xFF000000u) == 0u)
                    return 0;
            }
        }
    }

    // 0x4A9B94：命中后恢复 hit-test 前的状态，再按 0..6 转移。
    stateAt04_ = stateBeforeHitTest;
    switch (stateBeforeHitTest)
    {
    case 0:
    case 4:
        ++stateAt04_;
        return 0;

    case 1:
    case 5:
        if (services_->CompletePressAt47F310())
            ++stateAt04_;
        return 0;

    case 2:
    case 6:
        if (!services_->CompleteReleaseAt47F350())
            return 0;
        switch (configuration_.inputModeAt1C)
        {
        case 0:
            stateAt04_ = 0;
            return 3;
        case 1:
            if (stateBeforeHitTest == 6)
            {
                stateAt04_ = 0;
                return 4;
            }
            stateAt04_ = 4;
            return 3;
        case 2:
            stateAt04_ = 4;
            return 3;
        default:
            return 0;
        }

    default:
        return 0;
    }
}

std::int8_t CgUiButtonRuntime::DrawIntoAt4B5290(
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    if (stateAt04_ < 0)
        return -1;
    if (stateAt04_ > 6)
        throw std::out_of_range("CGUIButton +0x04 state exceeds seven surface slots");

    const std::int32_t slotIndex = configuration_.stateSurfaceSlotIndicesAt40[
        static_cast<std::size_t>(stateAt04_)];
    if (slotIndex < 0)
        return -1;

    // 0x4B52A5：绘制路径只解析当前状态槽，不使用 alpha 命中路径的槽 0 回退。
    const auto surface = ResolveSurfaceSlotAt4ADED0(slotIndex);
    if (!surface)
        return 0;
    const POINT position = DrawPosition(*surface);
    const auto targetHandle = destination.Handle();
    if (targetHandle == 0 || surface->handle == 0) return 0;
    return static_cast<std::int8_t>(graphics_->BlitSurface(
        targetHandle,
        surface->handle,
        position.x,
        position.y,
        nullptr,
        nullptr,
        destinationClip,
        0));
}

bool CgUiButtonRuntime::DrawToScreenAt441980(
    const RECT* const destinationClip)
{
    if (stateAt04_ < 0)
        return false;
    if (stateAt04_ > 6)
        throw std::out_of_range("CGUIButton +0x04 state exceeds seven surface slots");

    const std::int32_t slotIndex = configuration_.stateSurfaceSlotIndicesAt40[
        static_cast<std::size_t>(stateAt04_)];
    if (slotIndex < 0)
        return false;

    const auto surface = ResolveSurfaceSlotAt4ADED0(slotIndex);
    if (!surface)
        return false;
    const POINT position = DrawPosition(*surface);
    return graphics_->DrawSurface(
        surface->handle,
        position.x,
        position.y,
        nullptr,
        nullptr,
        destinationClip,
        0);
}

void CgUiButtonRuntime::ResetInteractionState() noexcept
{
    stateAt04_ = 0;
    inputLatchAt20_ = false;
}

void CgUiButtonRuntime::SetStateAt04(const std::int32_t state)
{
    if (state < -1 || state > 6)
        throw std::out_of_range("CGUIButton +0x04 state exceeds original range");
    stateAt04_ = state;
}

void CgUiButtonRuntime::SetInputLatchAt20(const bool value) noexcept
{
    inputLatchAt20_ = value;
}

void CgUiButtonRuntime::SetPixelHitTestAt08(const bool value) noexcept
{
    configuration_.pixelHitTestAt08 = value;
}

void CgUiButtonRuntime::SetExplicitHitRectAt0C(const RECT& rect) noexcept
{
    configuration_.explicitHitRectAt0C = rect;
}

void CgUiButtonRuntime::SetInputModeAt1C(const std::int32_t value) noexcept
{
    configuration_.inputModeAt1C = value;
}

void CgUiButtonRuntime::SetPositionAt24(const POINT& value) noexcept
{
    configuration_.positionAt24 = value;
}

void CgUiButtonRuntime::SetUsePositionAt30At2C(const bool value) noexcept
{
    configuration_.usePositionAt30At2C = value;
}

void CgUiButtonRuntime::SetPositionAt30(const POINT& value) noexcept
{
    configuration_.positionAt30 = value;
}

void CgUiButtonRuntime::SetExtentAt38(const SIZE& value) noexcept
{
    configuration_.extentAt38 = value;
}

void CgUiButtonRuntime::SetStateSurfaceSlotAt40(
    const std::size_t stateIndex,
    const std::int32_t slotIndex)
{
    if (stateIndex >= configuration_.stateSurfaceSlotIndicesAt40.size())
        throw std::out_of_range("CGUIButton state surface index exceeds seven slots");
    configuration_.stateSurfaceSlotIndicesAt40[stateIndex] = slotIndex;
}

void CgUiButtonRuntime::SetTimelineIndexAt68(const std::int32_t value) noexcept
{
    configuration_.timelineIndexAt68 = value;
}

std::int32_t CgUiButtonRuntime::AppendOwnedSurfaceAt5C(
    std::shared_ptr<RuntimeGraphicsSurface> surface)
{
    if (ownedSurfaces_.size() >=
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::length_error("CGUIButton surface slot index exceeds int32 range");
    }
    const auto slotIndex = static_cast<std::int32_t>(ownedSurfaces_.size());
    ownedSurfaces_.push_back(OwnedSurfaceRecord{"", std::move(surface)});
    return slotIndex;
}

bool CgUiButtonRuntime::AttachNamedSurfaceAt4C1F20(
    const std::size_t stateSurfaceSlot,
    std::string identifierCp932,
    std::shared_ptr<RuntimeGraphicsSurface> surface)
{
    if (stateSurfaceSlot >= configuration_.stateSurfaceSlotIndicesAt40.size())
        throw 1;
    if (!surface)
        return false;

    std::int32_t surfaceIndex = -1;
    for (std::size_t index = 0; index < ownedSurfaces_.size(); ++index)
    {
        const std::string& existing = ownedSurfaces_[index].identifierCp932;
        if (!existing.empty() &&
            _mbsicmp(
                reinterpret_cast<const unsigned char*>(identifierCp932.c_str()),
                reinterpret_cast<const unsigned char*>(existing.c_str())) == 0)
        {
            surfaceIndex = static_cast<std::int32_t>(index);
            break;
        }
    }

    if (surfaceIndex < 0)
    {
        if (ownedSurfaces_.size() >=
            static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        {
            throw std::length_error("CGUIButton surface slot index exceeds int32 range");
        }

        const RECT newRect{
            surface->OriginX(),
            surface->OriginY(),
            WrapAdd32(surface->OriginX(), surface->Width()),
            WrapAdd32(surface->OriginY(), surface->Height())};
        if (ownedSurfaces_.empty())
        {
            configuration_.positionAt30 = POINT{newRect.left, newRect.top};
            configuration_.extentAt38 = SIZE{
                newRect.right - newRect.left,
                newRect.bottom - newRect.top};
        }
        else
        {
            const RECT currentRect{
                configuration_.positionAt30.x,
                configuration_.positionAt30.y,
                WrapAdd32(
                    configuration_.positionAt30.x,
                    configuration_.extentAt38.cx),
                WrapAdd32(
                    configuration_.positionAt30.y,
                    configuration_.extentAt38.cy)};
            RECT combined{};
            ::UnionRect(&combined, &newRect, &currentRect);
            configuration_.positionAt30 = POINT{combined.left, combined.top};
            configuration_.extentAt38 = SIZE{
                combined.right - combined.left,
                combined.bottom - combined.top};
        }

        surfaceIndex = static_cast<std::int32_t>(ownedSurfaces_.size());
        ownedSurfaces_.push_back(
            OwnedSurfaceRecord{std::move(identifierCp932), std::move(surface)});
    }

    configuration_.stateSurfaceSlotIndicesAt40[stateSurfaceSlot] = surfaceIndex;
    return true;
}

std::int32_t CgUiButtonRuntime::StateAt04() const noexcept
{
    return stateAt04_;
}

bool CgUiButtonRuntime::InputLatchAt20() const noexcept
{
    return inputLatchAt20_;
}

RECT CgUiButtonRuntime::VisibleRect() const noexcept
{
    const POINT& base = configuration_.usePositionAt30At2C
        ? configuration_.positionAt30
        : configuration_.positionAt24;
    return RECT{
        base.x,
        base.y,
        base.x + configuration_.extentAt38.cx,
        base.y + configuration_.extentAt38.cy};
}

const CgUiButtonConfiguration&
CgUiButtonRuntime::Configuration() const noexcept
{
    return configuration_;
}

std::size_t CgUiButtonRuntime::OwnedSurfaceCountAt5C() const noexcept
{
    return ownedSurfaces_.size();
}

std::optional<CgUiButtonSurfaceView>
CgUiButtonRuntime::ResolveSurfaceSlotAt4ADED0(
    const std::int32_t slotIndex) const noexcept
{
    if (slotIndex < 0 ||
        static_cast<std::size_t>(slotIndex) >= ownedSurfaces_.size())
    {
        return std::nullopt;
    }
    const auto& surface =
        ownedSurfaces_[static_cast<std::size_t>(slotIndex)].surface;
    if (!surface)
        return std::nullopt;
    return CgUiButtonSurfaceView{
        surface->Handle(),
        surface->OriginX(),
        surface->OriginY(),
        surface->HasAlpha()};
}

RECT CgUiButtonRuntime::NaturalHitRect(
    const CgUiButtonSurfaceView&) const noexcept
{
    return VisibleRect();
}

POINT CgUiButtonRuntime::DrawPosition(
    const CgUiButtonSurfaceView& surface) const noexcept
{
    const POINT& base = configuration_.usePositionAt30At2C
        ? configuration_.positionAt30
        : configuration_.positionAt24;
    return POINT{
        base.x + surface.originX - configuration_.positionAt30.x,
        base.y + surface.originY - configuration_.positionAt30.y};
}
}
