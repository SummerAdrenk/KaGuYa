#pragma once
#include <Runtime/SystemDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SystemDialogNativeDetail
{
using Body = SystemDialogNativeBody88h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline const SystemDialogNativeVtable38& Table(Common* p) noexcept
{ return *reinterpret_cast<const SystemDialogNativeVtable38*>(p->vptrAt00); }
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T* ResolveWithBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& r) noexcept
{ return ResolveWithBase<T>(r, Read(r.baseAt00)); }
struct FreshReference final { union { Slot slot; }; FreshReference() noexcept {} };
inline Slot Copy(const Slot& r) noexcept
{ FreshReference out; CopyGuiClickedEventReference(out.slot, r); return out.slot; }
// 原正常尾区是terminate态。展开重读槽，正常尾明确区分缓存control和重读control。
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
        if (clear) { slot.baseAt00 = 0; slot.controlAt08 = nullptr; }
    }
    void Reload(bool clear = true) noexcept
    {
        active = false;
        auto* c = slot.controlAt08;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        {
            if (c->owningAt08) c->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.baseAt00 = 0; slot.controlAt08 = nullptr; }
    }
};
// 赋值旧槽释放处允许抛出，不能把正常尾terminate边界加在整段业务上。
inline void ReleaseWithoutClearing(Slot& r)
{
    auto* c = r.controlAt08;
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (r.controlAt08) r.controlAt08->DeleteControlAtVslot04(1);
    }
}
inline void* Root(ISystemDialogNativeSource& s)
{ s.EnsureRootAt413BE0(); return s.RootAt6198C4(); }
void BeginModalAt50CF50(Common*);
std::int32_t EndModalAt508E80(Common*);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
void __fastcall ProcessInput(Common*, void*);
void __fastcall FlushDirty(Common*, void*);
std::int32_t __fastcall PollExtra(Common*, void*);
void __fastcall Noop(Common*, void*);
}
