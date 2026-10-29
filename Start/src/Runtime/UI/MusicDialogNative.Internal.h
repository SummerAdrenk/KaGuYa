#pragma once

#include <Runtime/MusicDialogNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
using Body = MusicDialogNativeBody84h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = GuiControlNativeBody50h;
using Descriptor = SaveLoadMessageBoxCheckBoxDescriptorNative48h;
using Pair = GuiDialogNavigationMapping;
template<class T> T Read(const T& value) noexcept { return *reinterpret_cast<const volatile T*>(&value); }
template<class T> T* Resolve(const Slot& r) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* c = Read(r.controlAt08);
    const auto stride = c ? Read(static_cast<EnvironmentObjectReferenceControl*>(c)->strideAt0C) : 0;
    return reinterpret_cast<T*>(base + static_cast<std::uint32_t>(Read(r.biasAt04)) * static_cast<std::uint32_t>(stride));
}
inline Slot Copy(const Slot& r) noexcept
{
    Slot copy{Read(r.baseAt00), Read(r.biasAt04), Read(r.controlAt08)};
    if (copy.controlAt08) ++copy.controlAt08->referenceCountAt04;
    return copy;
}
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
// EH读取当前槽；已确认的正常terminate状态按原站点使用缓存ESI/EDI。
struct RefLife final
{
    Slot& slot;
    bool active = true;
    ~RefLife() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* c, bool clear = true) noexcept
    {
        active = false;
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        {
            if (c->owningAt08) c->DestroyPayloadAtVslot00();
            c->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Reload(bool clear = true) noexcept
    {
        active = false;
        auto* c = Read(slot.controlAt08);
        if (!c) return;
        if (--c->referenceCountAt04 == 0)
        {
            if (c->owningAt08) c->DestroyPayloadAtVslot00();
            if (auto* now = Read(slot.controlAt08)) now->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
};
struct RawLife final
{
    void* raw;
    std::size_t size;
    bool active = true;
    ~RawLife() noexcept { if (active) ::operator delete(raw, size); }
};
struct NavigationEx24h final
{
    GuiNavigationNativeBody common;
    std::int32_t countdownAt14;
    std::int32_t repeatedAt18;
    std::int32_t firstDelayAt1C;
    std::int32_t repeatDelayAt20;
};
static_assert(sizeof(NavigationEx24h) == 0x24);
static_assert(offsetof(NavigationEx24h, countdownAt14) == 0x14);
static_assert(offsetof(NavigationEx24h, repeatDelayAt20) == 0x20);
using OperationRecord = MusicNavigationRecord;
static_assert(sizeof(OperationRecord) == 0x10);
const POINT* Positions();
const char* BackgroundName(std::uint32_t);
const Descriptor& CheckTemplate();
std::span<const OperationRecord> Operations();
std::int32_t ItemCount();
void CreateNavigationExAt504CE0(Slot&, std::int32_t input, const Pair* pairs);
void CreateDescriptorReference(Slot&);
std::int32_t __fastcall UpperInputAt51EB70(Common*, void*);
std::int32_t __fastcall ActiveInputAt510C80(Common*, void*);
void __fastcall ResetInputAt5106F0(Common*, void*);
std::int32_t FirstClickedAt50FDB0(GuiClickedEventReferences&, std::int32_t clear);
}
