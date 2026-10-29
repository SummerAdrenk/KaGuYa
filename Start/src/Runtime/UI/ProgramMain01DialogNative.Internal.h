#pragma once
#include <Runtime/ProgramMain01DialogNative.h>
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::ProgramMain01NativeDetail
{
using Body = ProgramMain01DialogNativeBody88h;
using Common = GuiDialogNativeBody5Ch;
using Child = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T* ResolveCached(const Slot& slot, std::uintptr_t base, Control* control) noexcept
{
    if (!base) return nullptr;
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + std::uint32_t(Read(slot.biasAt04) * stride));
}
template<class T = void> T* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    return ResolveCached<T>(slot, base, Read(slot.controlAt08));
}
inline Slot Copy(const Slot& source) noexcept
{
    Slot result{Read(source.baseAt00), Read(source.biasAt04), Read(source.controlAt08)};
    if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
    return result;
}
inline void ReleaseOld(Slot& slot)
{
    auto* control = Read(slot.controlAt08);
    if (control && --control->referenceCountAt04 == 0)
    {
        if (Read(control->owningAt08)) control->DestroyPayloadAtVslot00();
        if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    }
}
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
// 正常terminate态与EH展开分开；捕获control与payload后重读control不能混为一类。
struct RefLife final
{
    Slot& slot;
    bool active = true;
    ~RefLife() noexcept { if (active) ReleaseYtlReference(slot); }
    void Current(bool clear = false) noexcept
    {
        active = false;
        auto* control = Read(slot.controlAt08);
        ReleaseOld(slot);
        if (clear && control) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Captured(Control* control, bool clear = false, bool reread = false) noexcept
    {
        active = false;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (Read(control->owningAt08)) control->DestroyPayloadAtVslot00();
            if (!reread) control->DeleteControlAtVslot04(1);
            else if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
};
struct LeafScope final
{
    ProgramMain01DialogNativeServices& services;
    GraphicsSurfaceNativeScope surfaces;
    GuiControlGraphicsScope graphics;
    GuiInputNativeScope input;
    GuiDialogNativeScope screen;
    LeafScope() noexcept : services(RequireProgramMain01DialogNativeServices()),
        surfaces(services.shared.surfaces), graphics(services.shared.graphics),
        input(services.shared.input), screen(services.shared.screen) {}
};
inline void* Archive0(EnvironmentRootStorage* environment) noexcept
{
    auto* begin = Read(environment->archivesAt30.begin);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end)) -
        reinterpret_cast<std::uintptr_t>(begin));
    return std::bit_cast<std::int32_t>(bytes) / 12 != 0 ? Resolve(begin[0]) : nullptr;
}
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
void Populate(Body&, const ProgramMain01DialogNativeControlGroup&);
void PopulateNavigation(Body&, const ProgramMain01DialogNativeNavigationGroup&);
void DestroyMembers(Body&) noexcept;
}
