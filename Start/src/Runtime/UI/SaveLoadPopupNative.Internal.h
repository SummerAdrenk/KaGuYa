#pragma once
#include <Runtime/SaveLoadPopupNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveFileCatalog.h>
#include <algorithm>
#include <bit>

namespace KaGuYa::Start::Runtime::SaveLoadPopupNativeDetail
{
using Body = SaveLoadPopupNativeBodyC0h;
using Number = SaveLoadPopupNumberNativeBody34h;
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;
using Vector = EnvironmentVectorStorage<Slot>;
inline Body& Popup(Common* p) noexcept { return *reinterpret_cast<Body*>(p); }
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T* ResolveWithBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * (c ? Read(c->strideAt0C) : 0u));
}
template<class T> T* Resolve(const Slot& r) noexcept { return ResolveWithBase<T>(r, Read(r.baseAt00)); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
inline std::int32_t Count(const Vector& v) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Read(v.end)) - reinterpret_cast<std::uintptr_t>(Read(v.begin)))) / 12; }
inline Slot Copy(const Slot& from) noexcept { Slot r; CopyGuiClickedEventReference(r, from); return r; }
// 正常尾缓存control与payload后重读control是两条原路径，EH始终重读局部Ref。
inline void ReleaseCaptured(Control* c) noexcept
{
    if (c && --c->referenceCountAt04 == 0)
    { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); }
}
inline void ReleaseCurrent(Slot& r) noexcept
{
    auto* c = r.controlAt08;
    if (c && --c->referenceCountAt04 == 0)
    { if (c->owningAt08) c->DestroyPayloadAtVslot00(); if (r.controlAt08) r.controlAt08->DeleteControlAtVslot04(1); }
}
struct Local final
{
    Slot& r; bool active = true;
    ~Local() noexcept { if (active) ReleaseYtlReference(r); }
    void Current(bool clear = false) noexcept
    { active = false; if (clear) ReleaseYtlReference(r); else ReleaseCurrent(r); }
    void Captured(Control* c) noexcept { active = false; ReleaseCaptured(c); }
};
struct Fresh final { union { Slot r; }; Fresh() noexcept {} };
inline GuiClickedEventReferences* Manager(Body& p) noexcept { return Resolve<GuiClickedEventReferences>(p.managerAt70); }
using CheckDescriptor=SaveLoadPopupCheckResource;
static_assert(sizeof(CheckDescriptor) == 0x48);
struct Data final
{
    const GuiStaticDescriptorNative24h& background;
    const char* bar;
    const GuiSliderDescriptorNative6Ch& slider;
    const GuiButtonDescriptorNative2Ch* arrows;
    const CheckDescriptor& checkbox;
    const char* const* digits;
    const char* const* markers;
};
Data DataForId(std::int32_t);
std::span<const POINT> DigitPositions();
POINT LatestPosition();
POINT UsedPosition();
const GuiDialogNavigationMapping* NavigationPairs(unsigned);
void CreateCheckBoxAt5043B0(Slot&, Common*, const CheckDescriptor&, SaveLoadPopupNativeServices&);
void NewNumberReference(Slot&);
void AppendNumberAt4D8AC0(Vector&, const Slot&);
void DestroyNumbersAt4F7080(Vector&) noexcept;
void DestroyConstructedMembers(Body&) noexcept;
void SelectSurfaceAt50B1F0(Slot&, GuiControlResourceNativeBody38h*, std::int32_t);
void BindEventAt536DB0(GuiControlNativeBody50h*, Slot owned);
void AppendEventAt4FEF80(GuiClickedEventReferences*, Slot owned);
std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences*);
void Blit(GraphicsSurfaceNativeBody* target, GraphicsSurfaceNativeBody* source, POINT);
void Clear(GraphicsSurfaceNativeBody*);
SaveFileStateNativeArchive* Save(EnvironmentRootStorage*) noexcept;
int PageCount();
inline int VisibleRows(int pages)
{ return std::min(pages, CurrentSaveLoadResourceProfile().popup.visibleRows); }
inline int MaximumFirstPage(int pages)
{ return std::max(0, pages - CurrentSaveLoadResourceProfile().popup.visibleRows); }
void SetSliderRange(GuiControlNativeBody50h*, int maximum);
void __fastcall Begin(Common*, void*);
std::int32_t __fastcall Poll(Common*, void*);
std::int32_t __fastcall UpperInput(Common*, void*);
std::int32_t __fastcall ActiveInput(Common*, void*);
void __fastcall ResetInput(Common*, void*);
std::int32_t __fastcall PriorityInput(Common*, void*);
void __fastcall ProcessInput(Common*, void*);
void __fastcall Draw(Common*, void*, const RECT*);
}
