#pragma once
#include <Runtime/SaveLoadMessageBoxNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SaveLoadMessageBoxNativeDetail
{
using Body = SaveLoadMessageBoxNativeBodyA8h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
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
template<class T> T* Indexed(const Slot& r, std::int32_t index) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(r.biasAt04) + std::uint32_t(index)) * stride);
}
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
struct FreshReference final { union { Slot slot; }; FreshReference() noexcept {} };
inline Slot Copy(const Slot& r) noexcept
{ FreshReference out; CopyGuiClickedEventReference(out.slot, r); return out.slot; }

// 展开时重读槽；正常尾按原寄存器资格分别缓存control/重读control，尾异常直接终止。
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(Control* c, bool clear = true) noexcept
    {
        active = false;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
        if (clear) { slot.baseAt00 = 0; slot.controlAt08 = nullptr; }
    }
    void Current(bool clear = true) noexcept
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
inline void ReleaseWithoutClearing(Slot& r)
{
    auto* c = r.controlAt08;
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (r.controlAt08) r.controlAt08->DeleteControlAtVslot04(1);
    }
}
void DestroyCompletedMembers(Body&) noexcept;
}
