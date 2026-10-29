#pragma once
#include <Runtime/CgDisplayResourceNative.h>
#include <Runtime/CgDisplayWindowNative.h>
#include <Runtime/CgOperationWindowNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RuntimeCStringFolderNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/RuntimeCStringException.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <cstring>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::CgDisplayResourceNativeDetail
{
using Scene = CgSceneNativeBodyB4h;
using Slot = YtlReferenceSlotRuntime;
using Array = RuntimeCStringArrayReference;
using Sprite = SpriteNativeBodyA0h;
using Scanner = RuntimeCStringFolderNativeBody8h;
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T& Field(void* p, unsigned offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
template<class T> T* Advance(T* p, int i) noexcept
{ return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + std::uint32_t(i) * sizeof(T)); }
inline int Add(int a, int b) noexcept { return std::bit_cast<int>(std::uint32_t(a) + std::uint32_t(b)); }
template<class T> T* Resolve(const Slot& r, int i = 0) noexcept
{
    const auto base = Read(r.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(r.biasAt04) + std::uint32_t(i)) * stride);
}
inline int Count(const Slot& r) noexcept
{
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return c ? std::bit_cast<int>(Read(c->countAt10)) : 0;
}
template<class T> int Distance(T* first, T* last) noexcept
{ return std::bit_cast<int>(std::uint32_t(reinterpret_cast<std::uintptr_t>(last) - reinterpret_cast<std::uintptr_t>(first))) / int(sizeof(T)); }
inline const char* Chars(const RuntimeCString& s) noexcept
{ return Read(*reinterpret_cast<const char* const*>(&s)); }
inline int Length(const RuntimeCString& s) noexcept
{ return Read(*(reinterpret_cast<const int*>(Chars(s)) - 3)); }
inline CgDisplayWindowNativeBody60h* Display(Scene& scene) noexcept
{ return Resolve<CgDisplayWindowNativeBody60h>(scene.displayWindowAt9C); }
inline Sprite* At(Scene& scene, int i) noexcept { return Resolve<Sprite>(Display(scene)->spritesAt24, i); }
inline RuntimeCString& Name(Array& a, int i) noexcept { return *Resolve<RuntimeCString>(a.RawSlot(), i); }
inline const ParamsPatternNative* Pattern(CgDisplayResourceNativeServices& s, unsigned site)
{ return s.source.CaptureParamsAt418480(site)->Pattern().get(); }
inline std::uint8_t Initialization(CgDisplayResourceNativeServices& s, unsigned site)
{
    auto* root = s.source.CaptureEnvironmentAt4180C0(site);
    auto* archive = ResolveEnvironmentArchiveAt417990(*root, 1);
    return Read(Field<std::uint8_t>(archive, 0x6D));
}
inline void ReleaseCached(YtlReferenceControlRuntime* c)
{
    if (c && --c->referenceCountAt04 == 0)
    { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
}
inline void ReleaseRetained(Slot& r)
{
    auto* c = Read(r.controlAt08);
    if (c && --c->referenceCountAt04 == 0)
    {
        if (c->owningAt08) c->DestroyPayloadAtVslot00();
        if (auto* current = Read(r.controlAt08)) current->DeleteControlAtVslot04(1);
    }
}
void ConstructScanner(Scanner&, const char*, IRuntimeStringManagerSource&);
void ReleaseScanner(Scanner&);
void DestroyScanner(Scanner&) noexcept;
struct UnwindEntry final { int previous; unsigned action; };
extern const UnwindEntry unwind[14];
extern const char readFailure[];
extern const char loadFailure[];
extern const char missingResource[];
}
