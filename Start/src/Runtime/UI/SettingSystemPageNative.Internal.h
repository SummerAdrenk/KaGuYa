#pragma once
#include <Runtime/SettingSystemPageNative.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SettingNativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
template<class T> T& Field(void* p, std::uint32_t offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
template<class T> T* ResolveWithBase(const Slot& slot, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& slot) noexcept { return ResolveWithBase<T>(slot, Read(slot.baseAt00)); }
struct FreshReference final { union { Slot slot; }; FreshReference() noexcept {} };
inline Slot Copy(const Slot& source) noexcept
{ FreshReference out; CopyGuiClickedEventReference(out.slot, source); return out.slot; }
inline void ReleaseWithoutClearing(Slot& slot)
{
    auto* c = Read(slot.controlAt08);
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    }
}
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* c, bool clear = true) noexcept
    {
        active = false;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Reload(bool clear = true) noexcept
    { active = false; if (clear) ReleaseYtlReference(slot); else ReleaseWithoutClearing(slot); }
};
inline void* Root(ISettingSystemPageNativeSource& s) { s.EnsureRootAt413BE0(); return s.RootAt6198C4(); }
}
namespace KaGuYa::Start::Runtime::SettingSystemPageNativeDetail
{
using namespace SettingNativeDetail;
using Body = SettingSystemPageNativeBody7Ch;
inline Body& Page(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline GuiClickedEventReferences* Manager(Body& b) noexcept { return Resolve<GuiClickedEventReferences>(b.managerAt70); }
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
}
