#pragma once
#include <Runtime/SaveLoadSystemPageNative.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SaveLoadSystemPageNativeDetail
{
using Body = SaveLoadSystemPageNativeBody80h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
inline Body& Page(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T* ResolveWithBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& r) noexcept
{ return ResolveWithBase<T>(r, Read(r.baseAt00)); }
inline GuiClickedEventReferences* Manager(Body& b) noexcept
{ return Resolve<GuiClickedEventReferences>(b.managerAt70); }
struct FreshReference final { union { Slot slot; }; FreshReference() noexcept {} };
inline Slot Copy(const Slot& r) noexcept
{ FreshReference out; CopyGuiClickedEventReference(out.slot, r); return out.slot; }
// 原赋值旧槽允许抛出；正常局部Ref终止边界不能包住该业务区。
inline void ReleaseWithoutClearing(Slot& r)
{
    auto* c = r.controlAt08;
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (r.controlAt08) r.controlAt08->DeleteControlAtVslot04(1);
    }
}
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* c) noexcept
    {
        active = false;
        ReleasePopupClickedEventTemporaryAt4EA9BC(slot, c);
    }
    void Reload(bool clear = true) noexcept
    {
        active = false;
        if (clear) ReleaseYtlReference(slot);
        else ReleaseWithoutClearing(slot);
    }
};
const GuiDialogNativeVtable35& PageBaseVtable() noexcept;
std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences*, std::int32_t);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall End(Common*, void*);
std::int32_t __fastcall Result(Common*, void*);
void __fastcall ResetInput(Common*, void*);
std::int32_t __fastcall PriorityInput(Common*, void*);
void __fastcall Composite(Common*, void*, Slot, const RECT*);
void __fastcall Draw(Common*, void*, const RECT*);
}
