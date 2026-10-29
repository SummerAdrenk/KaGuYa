#pragma once

#include <Runtime/ProgramSystemDialogNativeScope.h>
#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::ProgramSystemNativeDetail
{
using Body = ProgramSystemDialogNativeBody68h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
template<class T> T& Field(void* p, std::size_t offset) noexcept
{ return *reinterpret_cast<T*>(static_cast<std::byte*>(p) + offset); }
inline Body& Dialog(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(slot.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + std::uint32_t(Read(slot.biasAt04) * stride));
}
inline Slot CopyParts(std::uintptr_t base, std::uint32_t bias, Control* control) noexcept
{
    if (control) ++control->referenceCountAt04;
    return {base, bias, control};
}
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
// 原正常尾的terminate态与4F6460展开资格分离；payload后按各调用点决定重取control。
inline void ReleaseCurrent(Slot& slot, bool clear)
{
    auto* control = Read(slot.controlAt08);
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
inline void ReleaseCaptured(Slot& slot, Control* control) noexcept
{
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr; slot.baseAt00 = 0;
}
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish() noexcept { active = false; ReleaseCurrent(slot, false); }
};
struct Leaves final
{
    ProgramSystemDialogNativeServices& services;
    ProgramSelectDialogNativeScope parent;
    ProgramSystemDialogNativeScope scope;
    // ParentScope内的childScopes只绑定叶，不再构造Parent；有限嵌套、无堆分配。
    Leaves() noexcept : services(RequireProgramSystemDialogNativeServices()),
        parent(services.parent), scope(services) {}
};
inline void* Archive2(EnvironmentRootStorage* environment) noexcept
{
    auto* begin = Read(environment->archivesAt30.begin);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end)) -
        reinterpret_cast<std::uintptr_t>(begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    return std::uint32_t(count) > 2 ? Resolve<void>(begin[2]) : nullptr;
}
void Begin(Body&, bool mode01);
std::int32_t Poll(Body&, bool mode01);
Body* Construct(void*, Common*, std::int32_t, bool,
    std::span<const ProgramSystemDialogNativeControlGroup, 4>);
void Destroy(Body&, bool mode01) noexcept;
}
