#pragma once
#include <Runtime/GraffitiCountDialogNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::GraffitiCountDialogNativeDetail
{
using Dialog = GuiDialogNativeBody5Ch;
using Common = GraffitiCountDialogNativeBody90h;
using Likable = GraffitiLikableCountDialogNativeBodyB4h;
using Weight = GraffitiWeightCountDialogNativeBodyB8h;
using Slot = YtlReferenceSlotRuntime;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
template<class T> T* Resolve(const Slot& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * (control ? Read(control->strideAt0C) : 0u));
}
inline Slot Copy(const Slot& source) noexcept
{
    Slot result{Read(source.baseAt00), Read(source.biasAt04), Read(source.controlAt08)};
    if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
    return result;
}
// 正常尾缓存control，EH重读槽。正常释放抛出对应原terminate状态。
void FinishCached(YtlReferenceControlRuntime*) noexcept;
void FinishCurrentWithoutClearing(Slot&) noexcept;
struct RefUnwind final
{
    Slot& slot;
    bool armed = true;
    ~RefUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
    void Finish(YtlReferenceControlRuntime* control) noexcept
    { armed = false; FinishCached(control); }
};
std::int32_t ReadNamed(const char*, bool rootFirst = false);
void WriteNamedLate(const char*, const std::int32_t&);
void AddFullScreen(Common&);
void AddDialogRectangle(Common&);
void PlayCue(std::uint32_t);
enum class SkipReason { None, Latched, Action5 };
SkipReason SkipRequested();
void ConstructCountPrefix(Common&, bool weight);
void ConstructCountWidgets(Common&, RECT&);
const GuiStaticDescriptorNative24h& WeightTrainingDescriptor() noexcept;
const char* BackgroundName(bool weight, std::uint32_t index) noexcept;
const char* DigitName(std::uint32_t index) noexcept;

namespace Entries
{
GuiDirtyRectNativeBody80h* __fastcall Dirty(Dialog*, void*);
Dialog* __fastcall DeleteBase(Dialog*, void*, std::uint32_t) noexcept;
Dialog* __fastcall DeleteLikable(Dialog*, void*, std::uint32_t) noexcept;
Dialog* __fastcall DeleteWeight(Dialog*, void*, std::uint32_t) noexcept;
void __fastcall BeginLikable(Dialog*, void*);
void __fastcall BeginWeight(Dialog*, void*);
std::int32_t __fastcall PollLikable(Dialog*, void*);
std::int32_t __fastcall PollWeight(Dialog*, void*);
std::int32_t __fastcall EndLikable(Dialog*, void*);
std::int32_t __fastcall EndWeight(Dialog*, void*);
void __fastcall InputLikable(Dialog*, void*);
void __fastcall InputWeight(Dialog*, void*);
}
}
