#pragma once
#include <Runtime/LikabilityDialogNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <bit>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime::LikabilityNativeDetail
{
using Body = LikabilityDialogNativeBody94h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
using Surface = GraphicsSurfaceNativeBody;
using Vector = GuiControlSurfaceReferences;
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T* ResolveWithBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * (c ? Read(c->strideAt0C) : 0u));
}
template<class T> T* Resolve(const Slot& r) noexcept { return ResolveWithBase<T>(r, Read(r.baseAt00)); }
template<class T> T* ResolveCaptured(std::uintptr_t base, std::uint32_t bias, Control* control) noexcept
{
    if (!base) return nullptr;
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + bias * stride);
}
inline Body& Derived(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
inline std::int32_t Count(const Vector& v) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Read(v.end)) - reinterpret_cast<std::uintptr_t>(Read(v.begin)))) / 12; }
inline Slot Copy(const Slot& from) noexcept { Slot r; CopyGuiClickedEventReference(r, from); return r; }
// 正常尾分缓存control/当前槽重读；展开始终读局部当前槽。normal本身可抛，由原FI态决定terminate。
void ReleaseCaptured(Control*);
void ReleaseCurrent(Slot&, bool clear = false);
void Assign(Slot&, const Slot&, bool clear);
struct Local final
{
    Slot& r; bool active = true;
    ~Local() noexcept { if (active) ReleaseYtlReference(r); }
    void Current(bool clear = false) noexcept { active = false; ReleaseCurrent(r, clear); }
};
void BorrowArchive0(EnvironmentRootStorage*);
void* CaptureDraw(std::uint32_t ensureSite, std::uint32_t rootSite);
std::int32_t NamedValue(void* root, const char*, std::uint32_t paramsSite);
void BindEventAt536DB0(GuiControlNativeBody50h*, Slot owned);
void AppendEventAt4FEF80(GuiClickedEventReferences*, Slot owned);
std::int32_t PollEventsAt50FDB0(GuiClickedEventReferences*);
void Blit(Body&, Vector&, std::int32_t digit, POINT);
const char* const* VariableNames(unsigned row) noexcept;
const char* const* ExtraNames() noexcept;
const POINT* Positions(unsigned row) noexcept;
const POINT* ExtraPositions() noexcept;
struct UnwindEntry final { std::int32_t previous; std::uint32_t action; };
std::span<const UnwindEntry,14> ConstructorUnwind() noexcept;
std::span<const UnwindEntry,14> RefreshUnwind() noexcept;
}
