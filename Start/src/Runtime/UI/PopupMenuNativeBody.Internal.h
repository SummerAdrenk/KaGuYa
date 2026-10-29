#pragma once

#include <Runtime/PopupMenuNativeBody.h>
#include <Runtime/PopupMenuSample18Data.h>
#include <bit>

namespace KaGuYa::Start::Runtime::PopupNativeDetail
{
using Slot = YtlReferenceSlotRuntime;
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
inline PopupMenuNativeBodyA4h& Popup(GuiDialogNativeBody5Ch* common) noexcept
{ return *reinterpret_cast<PopupMenuNativeBodyA4h*>(common); }
// 调用者在find成功返回后才登记EH；正常内联尾和4F6460展开不能合并。
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish(bool clear) noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
};
inline RECT ReadRect(Slot& slot)
{
    auto& child = RequireGuiControl(slot);
    RECT temporary, result;
    CopyRect(&result, child.vptrAt00->rectAt3C(&child, &temporary));
    return result;
}
inline void WriteVisible(Slot& slot, std::int32_t value)
{
    auto& child = RequireGuiControl(slot);
    child.vptrAt00->writeEnabledAt4C(&child, value);
}
inline void WriteState(Slot& slot, std::int32_t value)
{
    auto& child = RequireGuiControl(slot);
    child.vptrAt00->writeStateAt44(&child, value);
}
inline void WriteChecked(Slot& slot, std::int32_t value)
{ reinterpret_cast<GuiCheckBoxNativeBody60h*>(&RequireGuiControl(slot))->checkedAt5C = value; }
inline void InvalidateChild(GuiDialogNativeBody5Ch& body, Slot& slot)
{
    auto& child = RequireGuiControl(slot);
    // 4FFD0A/519C0F在child+3C回调之前选定父表；矩形取返回地址，不假设等于out。
    const auto* table = body.vptrAt00;
    RECT temporary;
    const RECT rectangle = *child.vptrAt00->rectAt3C(&child, &temporary);
    table->invalidateAt64(&body, rectangle);
}
inline std::int32_t HintFor(std::int32_t event) noexcept
{
    for (const auto& pair : PopupMenuHoverMapSample18())
        if (pair.controlEvent == event) return pair.hintEvent;
    return -1;
}
}
