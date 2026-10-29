#include "ModeButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace ModeButtonNativeDetail;
namespace
{
RECT OffsetChildRect(Child& child, LONG width, LONG height, bool natural)
{
    RECT rect{0, 0, width, height};
    auto* position = natural ? &child.naturalAt30 : &child.positionAt24;
    const auto y = Read(position->y);
    const auto x = Read(position->x);
    ::OffsetRect(&rect, x, y);
    return rect;
}
}
RECT ModeButtonNativeDetail::ChildRect(Child& child)
{
    // 4AD0F0：先读宽高，再依+2C选择+24/+30坐标，不借用Dialog坐标或现代Rect。
    const auto width = Read(child.extentAt38.cx);
    const auto height = Read(child.extentAt38.cy);
    return OffsetChildRect(child, width, height, Read(child.useNaturalAt2C) != 0);
}
CgUiButtonNativeBody6Ch* ResolveModeButtonChildNativeAt4ACF20(Body& body, std::int32_t index) noexcept
{
    if (index < 0) return nullptr;
    auto* first = Read(body.childrenAt14.begin);
    if (index >= Distance(first, Read(body.childrenAt14.end))) return nullptr;
    return Resolve<Child>(*Advance(first, index));
}
std::int32_t InvalidateModeButtonNativeAt4B4570(Body& body, RECT rect)
{
    const auto empty = ::IsRectEmpty(&rect);
    if (empty) return empty;
    auto* dirty = Resolve<GuiDirtyRectNativeBody80h>(body.dirtyAt24);
    const RECT* argument = &rect;
    std::int32_t result;
    // 原08为按值RECT/retn10h；dirty08的既有公头返回void。
    // x86直调后采集真实EAX，不凭空把dirty的寄存器残值改成true/0。
    __asm
    {
        mov ecx, dirty
        push argument
        mov eax, [ecx]
        call dword ptr [eax+8]
        mov result, eax
    }
    return result;
}
void ResetModeButtonChildrenNativeAt4A73F0(Body& body)
{
    for (auto* at = Read(body.childrenAt14.begin); at != Read(body.childrenAt14.end); at = Advance(at, 1))
        if (ResourceCount(*Resolve<Child>(*at))) Resolve<Child>(*at)->stateAt04 = 0;
}
std::int32_t ResetModeButtonChildNativeAt4AACD0(Body& body, std::int32_t index)
{
    auto* child = ResolveModeButtonChildNativeAt4ACF20(body, index);
    if (!child) return 0;
    const auto state = Read(child->stateAt04) < 4 ? 0 : 4;
    const auto width = Read(child->extentAt38.cx);
    const auto natural = Read(child->useNaturalAt2C) != 0;
    const auto height = Read(child->extentAt38.cy);
    child->stateAt04 = state;
    const auto rect = OffsetChildRect(*child, width, height, natural);
    return body.vptrAt00->invalidateAt08(&body, rect);
}
std::int32_t FocusModeButtonChildNativeAt4ACD30(Body& body, std::int32_t index)
{
    auto* child = ResolveModeButtonChildNativeAt4ACF20(body, index);
    if (!child) return 0;
    const auto width = Read(child->extentAt38.cx);
    const auto state = Read(child->stateAt04) < 4 ? 1 : 5;
    const auto height = Read(child->extentAt38.cy);
    const auto natural = Read(child->useNaturalAt2C) != 0;
    child->stateAt04 = state;
    const auto rect = OffsetChildRect(*child, width, height, natural);
    return body.vptrAt00->invalidateAt08(&body, rect);
}
std::int32_t PressModeButtonChildNativeAt4C1980(Body& body, std::int32_t index)
{
    auto* child = ResolveModeButtonChildNativeAt4ACF20(body, index);
    if (!child) return 0;
    const auto width = Read(child->extentAt38.cx);
    const auto state = Read(child->stateAt04) < 4 ? 2 : 6;
    const auto height = Read(child->extentAt38.cy);
    const auto natural = Read(child->useNaturalAt2C) != 0;
    child->stateAt04 = state;
    const auto rect = OffsetChildRect(*child, width, height, natural);
    return body.vptrAt00->invalidateAt08(&body, rect);
}
std::int32_t CommitModeButtonChildNativeAt4B10F0(Body& body, std::int32_t index)
{
    auto* child = ResolveModeButtonChildNativeAt4ACF20(body, index);
    if (!child) return 0;
    switch (Read(child->inputModeAt1C))
    {
    case 0: child->stateAt04 = 0; break;
    case 1: child->stateAt04 = Read(child->stateAt04) < 4 ? 4 : 0; break;
    case 2: child->stateAt04 = 4; break;
    default: break;
    }
    const auto rect = ChildRect(*child);
    return body.vptrAt00->invalidateAt08(&body, rect);
}
std::int32_t SelectModeButtonNativeAt4C2200(Body& body, std::int32_t index) noexcept
{
    body.selectedAt04 = index;
    body.directionAt08 = -1;
    body.confirmLatchAt0C = 0;
    return index;
}
std::int32_t CurrentModeButtonNativeAt4ACFA0(Body& body) noexcept
{
    const auto selected = Read(body.selectedAt04);
    if (selected >= 0) return selected;
    auto* const first = Read(body.childrenAt14.begin);
    auto* const last = Read(body.childrenAt14.end);
    for (auto* at = first; at != last; at = Advance(at, 1))
    {
        if (!ResourceCount(*Resolve<Child>(*at))) continue;
        if (Read(Resolve<Child>(*at)->stateAt04) == 1 || Read(Resolve<Child>(*at)->stateAt04) == 2)
            return Distance(first, at);
    }
    return selected;
}
}
