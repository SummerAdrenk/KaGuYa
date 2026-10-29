#include <Runtime/CgDisplayPublicControlsRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept
{ return *reinterpret_cast<const volatile T*>(&value); }
LONG Add(LONG left, LONG right) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(left) + std::uint32_t(right)); }
LONG Sub(LONG left, LONG right) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(left) - std::uint32_t(right)); }
GraphicsSurfaceNativeBody* RequireBody(GraphicsSurfaceDestination destination)
{
    if (!destination.Body())
        throw std::invalid_argument("CG Native controls require the actual Surface24h target");
    return const_cast<GraphicsSurfaceNativeBody*>(destination.Body());
}
}

CgDisplayPublicModeButtonsRuntime::CgDisplayPublicModeButtonsRuntime(
    const YtlReferenceSlotRuntime* borrowed, ModeSelectButtonNativeServices& services) noexcept
    : storage_(std::in_place_index<0>, borrowed, services) {}
CgDisplayPublicModeButtonsRuntime::CgDisplayPublicModeButtonsRuntime(ControlledButtons buttons) noexcept
    : storage_(std::in_place_index<1>, std::move(buttons)) {}
bool CgDisplayPublicModeButtonsRuntime::IsNative() const noexcept { return storage_.index() == 0; }
const SceneManagerModeButtonsNativeReference*
CgDisplayPublicModeButtonsRuntime::NativeReference() const noexcept
{ return IsNative() ? &std::get<0>(storage_).reference : nullptr; }
std::int32_t CgDisplayPublicModeButtonsRuntime::Count() const noexcept
{
    if (IsNative()) return std::get<0>(storage_).reference.Count();
    return static_cast<std::int32_t>(std::get<1>(storage_).size());
}
bool CgDisplayPublicModeButtonsRuntime::HasButton(std::int32_t index) const
{
    // 原循环不跳过空数组元素，也不在失败后切换所有权域。
    return IsNative() || static_cast<bool>(std::get<1>(storage_).at(index));
}
std::int32_t CgDisplayPublicModeButtonsRuntime::StateAt04(std::int32_t index) const
{
    if (IsNative()) return Read(std::get<0>(storage_).reference.BorrowAt(index).Get()->stateAt04);
    return std::get<1>(storage_).at(index)->StateAt04();
}
RECT CgDisplayPublicModeButtonsRuntime::VisibleRect(std::int32_t index) const
{
    if (!IsNative()) return std::get<1>(storage_).at(index)->VisibleRect();
    const auto* body = std::get<0>(storage_).reference.BorrowAt(index).Get();
    const auto* position = Read(body->useNaturalAt2C) ? &body->naturalAt30 : &body->positionAt24;
    RECT rect{0, 0, Read(body->extentAt38.cx), Read(body->extentAt38.cy)};
    ::OffsetRect(&rect, Read(position->x), Read(position->y));
    return rect;
}
std::int32_t CgDisplayPublicModeButtonsRuntime::ProcessAt4A98E0(std::int32_t index, bool forced)
{
    if (IsNative()) return std::get<0>(storage_).reference.BorrowAt(index).ProcessAt4A98E0(forced);
    return std::get<1>(storage_).at(index)->ProcessAt4A98E0(forced);
}
std::uint8_t CgDisplayPublicModeButtonsRuntime::DrawToScreenAt441980(
    std::int32_t index, const RECT* clip)
{
    if (!IsNative()) return std::get<1>(storage_).at(index)->DrawToScreenAt441980(clip);
    auto& native = std::get<0>(storage_);
    ModeSelectButtonNativeScope scope(*native.services);
    auto* body = native.reference.BorrowAt(index).Get();
    // 4AC1AD..4AC208内联解析CGUIButton当前绘制槽，再进入CSurface的441980包装。
    const auto state = Read(body->stateAt04);
    if (state < 0) return std::uint8_t(state);
    const auto slotAddress = reinterpret_cast<std::uintptr_t>(body) + 0x40u + std::uint32_t(state) * 4u;
    const auto slot = Read(*reinterpret_cast<const std::int32_t*>(slotAddress));
    if (slot < 0) return std::uint8_t(slot);
    auto* source = ResolveCgUiButtonSurfaceNativeAt4ADED0(*body, slot);
    if (!source) return 0;
    const auto dx = Sub(Read(source->originXAt10), Read(body->naturalAt30.x));
    const auto dy = Sub(Read(source->originYAt14), Read(body->naturalAt30.y));
    const auto* position = Read(body->useNaturalAt2C) ? &body->naturalAt30 : &body->positionAt24;
    const auto x = Add(dx, Read(position->x));
    const auto y = Add(dy, Read(position->y));
    // 4419A6在4182F0之前读取+04；441A75保留DLL返回的AL，不按整个EAX归一化。
    const auto handle = Read(source->handleAt04);
    if (!handle) return 0;
    return std::uint8_t(native.services->button.graphics.SurfaceDrawRaw(
        handle, x, y, nullptr, nullptr, clip, 0));
}
std::uint8_t CgDisplayPublicModeButtonsRuntime::DrawIntoAt4B5290(
    std::int32_t index, GraphicsSurfaceDestination destination, const RECT* clip)
{
    if (IsNative())
        return std::get<0>(storage_).reference.BorrowAt(index).DrawIntoAt4B5290(RequireBody(destination), clip);
    return std::uint8_t(std::get<1>(storage_).at(index)->DrawIntoAt4B5290(destination, clip));
}
void CgDisplayPublicModeButtonsRuntime::Release()
{
    if (IsNative()) std::get<0>(storage_).reference.Reset();
    else std::get<1>(storage_).clear();
}

CgDisplayPublicSelectorRuntime::CgDisplayPublicSelectorRuntime(
    const YtlReferenceSlotRuntime* borrowed, OptionCharSelectDialogNativeDependencies& services) noexcept
    : storage_(std::in_place_index<0>, borrowed, services) {}
CgDisplayPublicSelectorRuntime::CgDisplayPublicSelectorRuntime(
    std::shared_ptr<ICgDisplayButtonGroupSelector> selector) noexcept
    : storage_(std::in_place_index<1>, std::move(selector)) {}
bool CgDisplayPublicSelectorRuntime::IsNative() const noexcept { return storage_.index() == 0; }
CgDisplayPublicSelectorRuntime::operator bool() const noexcept
{
    return IsNative() ? static_cast<bool>(std::get<0>(storage_)) : static_cast<bool>(std::get<1>(storage_));
}
const SceneManagerOptionCharNativeReference* CgDisplayPublicSelectorRuntime::NativeReference() const noexcept
{ return IsNative() ? &std::get<0>(storage_) : nullptr; }
std::int32_t CgDisplayPublicSelectorRuntime::ProcessAtVslot48()
{
    if (IsNative()) return std::get<0>(storage_).Borrow().ProcessAtVslot48();
    return std::get<1>(storage_)->ProcessAtVslot48();
}
std::int32_t CgDisplayPublicSelectorRuntime::SelectionAtVslot50() const
{
    if (IsNative()) return std::get<0>(storage_).Borrow().SelectionAtVslot50();
    return std::get<1>(storage_)->SelectionAtVslot50();
}
void CgDisplayPublicSelectorRuntime::AttachDirtyRectsAtVslot94(ICgDisplayButtonGroupDirtyRects& dirty)
{
    if (!IsNative()) { std::get<1>(storage_)->AttachDirtyRectsAtVslot94(dirty); return; }
    const auto* borrowed = dirty.BorrowNativeReference();
    if (!borrowed) throw std::invalid_argument("CG Native selector requires the original DIR Ref");
    std::get<0>(storage_).Borrow().AttachDirtyRectsAtVslot94(borrowed);
}
void CgDisplayPublicSelectorRuntime::ResetAtVslot90()
{
    if (IsNative()) std::get<0>(storage_).Borrow().ResetAtVslot90();
    else std::get<1>(storage_)->ResetAtVslot90();
}
void CgDisplayPublicSelectorRuntime::DrawAtVslot60(const RECT& clip)
{
    if (IsNative()) std::get<0>(storage_).Borrow().DrawAtVslot60(&clip);
    else std::get<1>(storage_)->DrawAtVslot60(clip);
}
void CgDisplayPublicSelectorRuntime::CompositeAtVslot5C(
    const std::shared_ptr<ILoadAfterTransitionSource>& target,
    GraphicsSurfaceDestination destination, const RECT* clip)
{
    if (IsNative()) std::get<0>(storage_).Borrow().CompositeSurfaceAtVslot5C(RequireBody(destination), clip);
    else std::get<1>(storage_)->CompositeAtVslot5C(target, clip);
}
void CgDisplayPublicSelectorRuntime::Release()
{
    if (IsNative()) std::get<0>(storage_).Reset();
    else std::get<1>(storage_).reset();
}
}
