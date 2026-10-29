#pragma once

#include <Runtime/OptionCharSelectDialogNative.h>
#include <Runtime/OptionCharResourceProfile.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::OptionCharSelectNativeDetail
{
using Body = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
inline OptionCharSelectDialogNativeBody8Ch& Dialog(Body* body) noexcept
{ return *reinterpret_cast<OptionCharSelectDialogNativeBody8Ch*>(body); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
// fresh返回槽在原调用完成前不写；避免Slot的DMI抢在分配/原helper之前清栈槽。
struct UnwrittenReference final
{
    union { Slot slot; };
    UnwrittenReference() noexcept {}
};
static_assert(sizeof(UnwrittenReference) == 0x0C);
inline Slot Copy(const Slot& source) noexcept
{
    UnwrittenReference output;
    CopyGuiClickedEventReference(output.slot, source);
    return output.slot;
}
inline GuiClickedEventReferences* Manager(const Slot& reference) noexcept
{ return ResolveEnvironmentObjectReference<GuiClickedEventReferences>(reference); }
inline bool Legacy(const OptionCharResourceProfile& profile) noexcept
{ return profile.presentation == OptionCharPresentation::NaturalLegacy; }
inline GuiControlNativeBody50h* FindChild(Body* body, int event)
{
    Slot reference{};
    auto* returned = body->vptrAt00->findChildAt14(body, &reference, event);
    auto* child = ResolveGuiControl(*returned);
    ReleaseGuiControlReferenceAt4F6460(reference);
    return child; // 父children仍持有正文；这里只结束查询的临时增持。
}
inline GuiControlNativeBody50h& RequiredChild(Body* body, int event)
{
    auto* child = FindChild(body, event);
    if (!child) throw std::logic_error("Option character navigation selected a missing child");
    return *child;
}

// 正常内联terminate态与EH展开分开；clear仅清base/control，不改bias。
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* control, bool clear = true) noexcept
    {
        active = false;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Reload(bool clear = true) noexcept
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

GuiDirtyRectNativeBody80h* __fastcall Dirty(Body*, void*);
Body* __fastcall Delete(Body*, void*, std::uint32_t) noexcept;
std::int32_t __fastcall UpperInput(Body*, void*);
std::int32_t __fastcall ActiveInput(Body*, void*);
void __fastcall ResetInput(Body*, void*);
void __fastcall Begin(Body*, void*);
std::int32_t __fastcall Poll(Body*, void*);
std::int32_t __fastcall End(Body*, void*);
std::int32_t __fastcall Result(Body*, void*);
std::int32_t __fastcall Count(Body*, void*);
void __fastcall Reset(Body*, void*);
void __fastcall AttachDirty(Body*, void*, const Slot*);
}
