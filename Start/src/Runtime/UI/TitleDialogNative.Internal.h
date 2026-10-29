#pragma once

#include <Runtime/TitleDialogNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::TitleDialogNativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Body = TitleDialogNativeBodyD8h;
using Slot = YtlReferenceSlotRuntime;
inline Body& Dialog(Common* body) noexcept { return *reinterpret_cast<Body*>(body); }
template<class T> T& Field(void* body, std::size_t offset) noexcept
{ return *reinterpret_cast<T*>(static_cast<std::byte*>(body) + offset); }
inline Slot& RefField(void* body, std::size_t offset) noexcept { return Field<Slot>(body, offset); }
inline void* Resolve(const Slot& slot) noexcept { return ResolveEnvironmentObjectReference<void>(slot); }
inline GuiClickedEventReferences* Manager(const Body& body) noexcept
{ return ResolveEnvironmentObjectReference<GuiClickedEventReferences>(body.clickedManagerAtC8); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
struct UnwrittenReference final
{
    union { Slot slot; };
    UnwrittenReference() noexcept {}
};
inline Slot Copy(const Slot& source) noexcept
{
    UnwrittenReference out;
    CopyGuiClickedEventReference(out.slot, source);
    return out.slot;
}
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
inline void ResetOperation(TransitOperation& op) noexcept
{
    op.destination = 0; op.previous = 0; op.source = nullptr; op.mask = 0;
    op.y = 0; op.x = 0; op.clip = nullptr; op.opcode = -1;
    op.targetKind = 0; op.current = 0; op.target = 255; op.range = 0;
}
inline void* Root(ITitleDialogNativeSource& source)
{ source.EnsureRootAt413BE0(); return source.RootAt6198C4(); }
inline void* Registry(ITitleDialogNativeSource& source, std::int32_t index)
{ return source.RegistryAt417990(source.SystemAt4180C0(), index); }

std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences*, std::int32_t clear);
void PlayCue(std::int32_t channel, bool hover);
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common*, void*);
Common* __fastcall Delete(Common*, void*, std::uint32_t) noexcept;
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
void __fastcall ChildChanged(Common*, void*, Slot);
void __fastcall Redraw(Common*, void*);
}
