#pragma once
#include <Runtime/SaveLoadPageSecondNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::SaveLoadPageSecondNativeDetail
{
using Body = SaveLoadPageSecondNativeBodyACh;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
inline Body& Page(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
template<class T> T* ResolveWithBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& r) noexcept { return ResolveWithBase<T>(r, Read(r.baseAt00)); }
inline std::int32_t Add(std::int32_t value, std::int32_t delta) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(value) + static_cast<std::uint32_t>(delta)); }
inline GuiClickedEventReferences* Manager(Body& b) noexcept { return Resolve<GuiClickedEventReferences>(b.managerAt70); }
inline const SaveLoadPageSecondNativeVtable36* Table(Common* p) noexcept
{ return reinterpret_cast<const SaveLoadPageSecondNativeVtable36*>(Read(p->vptrAt00)); }
struct FreshReference final { union { Slot slot; }; FreshReference() noexcept {} };
inline Slot Copy(const Slot& source) noexcept
{ FreshReference r; CopyGuiClickedEventReference(r.slot, source); return r.slot; }
// 正常内联尾无清写；payload后重读control，和缓存寄存器的尾不能合并。
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
    void Reload(bool clear = true) noexcept
    { active = false; if (clear) ReleaseYtlReference(slot); else ReleaseWithoutClearing(slot); }
    void Captured(YtlReferenceControlRuntime* c, bool clear = true) noexcept
    {
        active = false;
        if (clear) { ReleasePopupClickedEventTemporaryAt4EA9BC(slot, c); return; }
        if (c && --c->referenceCountAt04 == 0)
        {
            if (c->owningAt08) c->DestroyPayloadAtVslot00();
            c->DeleteControlAtVslot04(1); // 5112E3仍用EDI，无清写局部Ref。
        }
    }
};
inline void SetPressed(GuiControlNativeBody50h* child)
{ reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1); }
inline void Invoke(const Slot& r)
{
    if (Read(r.baseAt00))
    {
        auto* e = Resolve<GuiClickedEventNativeBody>(r);
        e->vptrAt00->invokeAt00(e);
    }
}
}
