#pragma once
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime::SaveLoadMessageBoxChildFactoriesDetail
{
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
using Body = GuiControlNativeBody50h;
using MultiCheck = SaveLoadMessageBoxMultiCheckNativeBody78h;
using MultiHotkey = SaveLoadMessageBoxMultiHotkeyNativeBody68h;
using Hotkey = SaveLoadMessageBoxHotkeyAndStateNativeBody08h;
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
template<class T> T* Resolve(const Slot& r) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * (c ? Read(c->strideAt0C) : 0u));
}
inline Slot CopyParts(std::uintptr_t base, std::uint32_t bias, Control* control) noexcept
{
    Slot result{base, bias, control};
    if (control) ++control->referenceCountAt04;
    return result;
}
inline Slot Copy(const Slot& r) noexcept
{ return CopyParts(Read(r.baseAt00), Read(r.biasAt04), Read(r.controlAt08)); }
inline void ReleaseCaptured(Control* c) noexcept
{
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        c->DeleteControlAtVslot04(1);
    }
}
inline void ReleaseCurrent(Slot& r, bool clear = false) noexcept
{
    auto* c = Read(r.controlAt08);
    if (!c) return;
    if (--c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (auto* current = Read(r.controlAt08)) current->DeleteControlAtVslot04(1);
    }
    if (clear) { r.controlAt08 = nullptr; r.baseAt00 = 0; }
}
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
struct ReferenceLifetime final
{
    Slot& r;
    bool active = true;
    ~ReferenceLifetime() noexcept { if (active) ReleaseYtlReference(r); }
    void Captured(Control* c, bool clear = false) noexcept
    {
        active = false;
        ReleaseCaptured(c);
        if (clear && c) { r.controlAt08 = nullptr; r.baseAt00 = 0; }
    }
    void Current(bool clear = false) noexcept { active = false; ReleaseCurrent(r, clear); }
};
struct RawLifetime final
{
    void* address;
    std::size_t size;
    bool active = true;
    ~RawLifetime() noexcept { if (active) ::operator delete(address, size); }
};
// Ref控制块的+09..+0B不初始化；只允许原写点发布，失败不得替raw正文补回滚。
class MultiCheckControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit MultiCheckControl(Body* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (auto* p = static_cast<Body*>(payloadAt14)) p->vptrAt00->deletingDestructorAt00(p, 1); }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
class MultiHotkeyControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit MultiHotkeyControl(Body* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (auto* p = static_cast<Body*>(payloadAt14)) p->vptrAt00->deletingDestructorAt00(p, 1); }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
class RadioControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit RadioControl(Body* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    { if (auto* p = static_cast<Body*>(payloadAt14)) p->vptrAt00->deletingDestructorAt00(p, 1); }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
class HotkeyAndStateControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit HotkeyAndStateControl(Hotkey* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{8}); }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
static_assert(sizeof(MultiCheckControl) == 0x18);
static_assert(sizeof(MultiHotkeyControl) == 0x18);
static_assert(sizeof(RadioControl) == 0x18);
static_assert(sizeof(HotkeyAndStateControl) == 0x18);
template<class C, std::uint32_t Size, class T> void ConstructReference(Slot& out, T* p)
{
    out = {};
    if (!p) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(p);
    void* raw = ::operator new(0x18);
    auto* c = raw ? ::new(raw) C(p) : nullptr;
    out.controlAt08 = c;
    c->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = Size;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}
inline void AppendHotkey(EnvironmentVectorStorage<Slot>& v, const Slot& r)
{
    // 4D8850/4D8140与既存4540A0/43EA10逐序同形；payload始终经Ref虚槽，不作Surface强转。
    if (v.end == v.capacity) GrowGuiControlSurfaceReferencesAt4540A0(v, v.end, r);
    else
    {
        auto* position = Read(v.end);
        position->baseAt00 = Read(r.baseAt00);
        position->biasAt04 = 0;
        auto* c = Read(r.controlAt08);
        position->controlAt08 = c;
        if (c) ++c->referenceCountAt04;
        ++v.end;
    }
}
inline std::int32_t Count(const EnvironmentVectorStorage<Slot>& v) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Read(v.end)) - reinterpret_cast<std::uintptr_t>(Read(v.begin)))) / 12;
}
template<class F> F ParentSlot(void* p, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, p, sizeof(table));
    F f;
    std::memcpy(&f, table + offset, sizeof(f));
    return f;
}
using ParentAction = std::int32_t (__thiscall*)(void*, std::int32_t);
using ParentPoint = POINT* (__thiscall*)(void*, POINT*);
using ParentScalar = std::int32_t (__thiscall*)(void*);
using ParentFocus = void (__thiscall*)(void*, Body*);
void ClearHotkeyStatesAt5321A0(MultiHotkey*);
}
