#pragma once

#include <Runtime/ProgramSelectDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::ProgramSelectNativeDetail
{
using Body = ProgramSelectDialogNativeBodyECh;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
using Source = IProgramSelectDialogNativeSource;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T& Field(void* body, std::size_t offset) noexcept
{ return *reinterpret_cast<T*>(static_cast<std::byte*>(body) + offset); }
inline Body& Dialog(Common* body) noexcept { return *reinterpret_cast<Body*>(body); }
inline Common& Base(Body& body) noexcept { return body.into.common; }
inline const ProgramSelectDialogNativeVtable42& Methods(Body& body) noexcept
{ return *reinterpret_cast<const ProgramSelectDialogNativeVtable42*>(Read(Base(body).vptrAt00)); }
template<class T = void> T* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(slot.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + std::uint32_t(Read(slot.biasAt04) * stride));
}
inline Slot Copy(const Slot& from) noexcept
{
    Slot result{Read(from.baseAt00), Read(from.biasAt04), Read(from.controlAt08)};
    if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
    return result;
}
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
struct LocalReference final
{
    Slot& slot;
    bool active = true;
    ~LocalReference() noexcept { if (active) ReleaseYtlReference(slot); }
    void Finish(bool clear = false) noexcept
    {
        active = false;
        auto* control = Read(slot.controlAt08);
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (auto* current = Read(slot.controlAt08)) current->DeleteControlAtVslot04(1);
        }
        if (clear && control) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
};
struct UnwrittenReference final
{
    union { Slot slot; };
    UnwrittenReference() noexcept {}
};
inline void ReleaseCaptured(Control* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
// Scope公头不依赖完整services；各实际表入口覆盖整次调用绑定这些叶，
// 包括回调、展开和递归删除，不申请隐藏scope存储或额外正文owner。
struct LeafScope final
{
    ProgramSelectDialogNativeServices& services;
    GraphicsSurfaceNativeScope surfaces;
    GuiControlGraphicsScope graphics;
    GuiInputNativeScope input;
    GuiDialogNativeScope screen;
    TransitGraphicsScope transit;
    LeafScope() noexcept
        : services(RequireProgramSelectDialogNativeServices()), surfaces(services.surfaces),
          graphics(services.graphics), input(services.input), screen(services.screen),
          transit(services.transit) {}
};
inline void* Root(Source& source, std::uint32_t ensureSite, std::uint32_t readSite)
{ source.EnsureRootAt413BE0(ensureSite); return source.RootAt6198C4(readSite); }
inline void* Registry(Source& source, std::uint32_t index,
    std::uint32_t environmentSite, std::uint32_t registrySite)
{ return source.RegistryAt417990(source.CaptureEnvironmentAt4180C0(environmentSite), index, registrySite); }
inline void* Archive(EnvironmentRootStorage* environment, std::uint32_t index) noexcept
{
    const auto begin = Read(environment->archivesAt30.begin);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end)) -
        reinterpret_cast<std::uintptr_t>(begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    return std::uint32_t(count) > index ? Resolve(begin[index]) : nullptr;
}
inline void ResetOperation(TransitOperation& op) noexcept
{
    op.destination = 0; op.previous = 0; op.source = nullptr; op.mask = 0;
    op.y = 0; op.x = 0; op.clip = nullptr; op.opcode = -1;
    op.targetKind = 0; op.current = 0; op.target = 255; op.range = 0;
}
void Composite(Common*, Slot ownedTarget, const RECT*);
void Draw(Common*, const RECT*);
void UpdateChildren(Body&, bool mode01);
void Reopen(Body&, bool mode01);
void Advance(Body&, bool mode01);
Slot* CaptureWithSystem(Body&, Slot*, std::int32_t includeSystem);
}
