#pragma once

#include <Runtime/GuiPanelNative.h>
#include <Runtime/GuiDialogNative.h>

namespace KaGuYa::Start::Runtime::GuiPanelNativeDetail
{
using Body = GuiPanelNativeBody6Ch;
using Common = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;

inline Body& Panel(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline GuiDialogNativeBody5Ch* Parent(Body& b) noexcept
{ return static_cast<GuiDialogNativeBody5Ch*>(b.common.parentAt4C); }

// 原未构造callee输出槽不能先执行Ref默认构造。
struct UnwrittenReference final
{
    union { Slot slot; };
    UnwrittenReference() noexcept {}
};
inline Slot CopyWithControl(const Slot& source, Control* control) noexcept
{
    Slot result{source.baseAt00, source.biasAt04, control};
    if (control) ++control->referenceCountAt04;
    return result;
}
inline void FinishCaptured(Control* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
inline void FinishCurrent(Slot& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(Control* control) noexcept { active = false; FinishCaptured(control); }
    void Current(bool clear) noexcept
    {
        active = false;
        if (clear) ReleaseYtlReference(slot);
        else FinishCurrent(slot);
    }
};
void DestroyIds(GuiPanelIdStorage&) noexcept;

namespace Entries
{
Common* __fastcall Delete(Common*, void*, std::uint32_t) noexcept;
std::int32_t __fastcall Keyboard(Common*, void*);
void __fastcall WriteState(Common*, void*, std::int32_t);
void __fastcall WriteEnabled(Common*, void*, std::int32_t);
void __fastcall AddId(Common*, void*, std::int32_t);
void __fastcall AddNavigation(Common*, void*, Slot);
void __fastcall Select(Common*, void*, std::int32_t);
}
}
