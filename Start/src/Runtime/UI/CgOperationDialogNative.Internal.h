#pragma once

#include <Runtime/CgOperationDialogNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/CgOperationResourceProfile.h>

#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::CgOperationNativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Dialog = CgOperationDialogNativeBodyA8h;
using Slot = YtlReferenceSlotRuntime;
using Surface = GraphicsSurfaceNativeBody;
using Surfaces = GuiControlSurfaceReferences;
inline const CgOperationResourceProfile& Profile() { return CurrentCgOperationResourceProfile(); }
inline bool NativeVisuals() { return Profile().encoding==CgOperationVisualEncoding::NativeSevenState; }
inline constexpr int ItemEventBase=12, PreviousPageEvent=10, NextPageEvent=11;

struct Buttons final
{
    union { Common common; };
    union { Slot eventsAt5C; };
    union { Slot surfacesAt68[2]; };
    RECT boundsAt80;
    std::int32_t resultAt90;
    Buttons() noexcept {}
};
struct Popup final
{
    union { Common common; };
    union { Slot eventsAt5C; };
    RECT boundsAt68;
    std::int32_t resultAt78;
    union { Slot backgroundAt7C; };
    union { Surfaces pagesAt88; };
    std::int32_t pageAt94;
    std::int32_t keyboardLatchAt98;
    Popup() noexcept {}
};
struct Vtable36 final
{
    GuiDialogNativeVtable35 common;
    RECT* (__thiscall* boundsAt8C)(Common*, RECT*);
};
static_assert(sizeof(Buttons) == 0x94 && alignof(Buttons) == 4);
static_assert(offsetof(Buttons, eventsAt5C) == 0x5C);
static_assert(offsetof(Buttons, surfacesAt68) == 0x68);
static_assert(offsetof(Buttons, boundsAt80) == 0x80);
static_assert(offsetof(Buttons, resultAt90) == 0x90);
static_assert(sizeof(Popup) == 0x9C && alignof(Popup) == 4);
static_assert(offsetof(Popup, boundsAt68) == 0x68);
static_assert(offsetof(Popup, resultAt78) == 0x78);
static_assert(offsetof(Popup, backgroundAt7C) == 0x7C);
static_assert(offsetof(Popup, pagesAt88) == 0x88);
static_assert(offsetof(Popup, pageAt94) == 0x94);
static_assert(offsetof(Popup, keyboardLatchAt98) == 0x98);
static_assert(sizeof(Vtable36) == 0x90 && offsetof(Vtable36, boundsAt8C) == 0x8C);
static_assert(std::is_standard_layout_v<Buttons> && std::is_trivially_destructible_v<Buttons>);
static_assert(std::is_standard_layout_v<Popup> && std::is_trivially_destructible_v<Popup>);

inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b)); }
inline std::int32_t Mul(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b)); }
template<class T> T* Resolve(const Slot& s) noexcept { return ResolveEnvironmentObjectReference<T>(s); }
inline Slot Copy(const Slot& s) noexcept
{
    Slot out;
    CopyGuiControlResourceReference(out, s);
    return out;
}

// 展开4F64xx和正常terminate态分开；normal可选择缓存或重读control，不改bias。
struct RefGuard final
{
    Slot& slot;
    bool active = true;
    ~RefGuard() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* control, bool clear = false) noexcept;
    void Reload(bool clear = false, bool checkAfterPayload = true) noexcept;
};
struct RawGuard final
{
    void* raw;
    std::size_t bytes;
    bool active = true;
    ~RawGuard() noexcept { if (active) ::operator delete(raw, bytes); }
};
CgOperationDialogNativeServices& Services();
const Vtable36& ButtonsMethods() noexcept;
const Vtable36& PopupMethods() noexcept;
Buttons* ConstructButtonsAt4DAC80(Buttons&, Common* parent, std::int32_t id);
Popup* ConstructPopupAt4DB860(Popup&, Common* parent, std::int32_t id);
void DestroyButtons(Buttons&) noexcept;
void DestroyPopup(Popup&) noexcept;
void ConstructButtonsReference(Slot& freshOutput, Buttons*);
void ConstructPopupReference(Slot& freshOutput, Popup*);
void AppendAvailability(EnvironmentVectorStorage<std::int32_t>&, const std::int32_t&);
void DestroyAvailability(EnvironmentVectorStorage<std::int32_t>&) noexcept;
Slot* GrowSurfacesAt4D83D0(Surfaces&, Slot* position, const Slot& source);
void AppendSurface(Surfaces&, const Slot&);
void AssignReference(Slot& destination, const Slot& source, bool clearControl);
Slot* CompatibleSurfaceAt444470(Slot& freshOutput, Surface& source);
void ApplySelectionAt537010(Buttons&, std::int32_t selection);
void AppendPopupItemAt4FEAE0(Popup&, std::int32_t index, std::int32_t available);
void SelectPageAt527500(Popup&, std::int32_t page);
void ResetPopupAt5021C0(Popup&);
void ClearSurfaceAt4143A0(Surface*);
void BlitAt4425C0(Surface* destination, Surface* source, std::int32_t x,
    std::int32_t y, const RECT* clip = nullptr);
Slot DigitFor(Dialog&, std::int32_t digit, CgOperationDigitUse);
std::int32_t PollEventsAt50FDB0(GuiClickedEventReferences&, std::int32_t clear);
const GuiButtonNativeVtable29& NativeOperationButtonMethods() noexcept;
void SyncPopupControls(Popup&);
int DispatchAction(Dialog&, CgOperationAction);

void __fastcall DrawButtons(Common*, void*, const RECT*);
void __fastcall CompositeButtons(Common*, void*, Slot, const RECT*);
std::int32_t __fastcall ActivePopup(Common*, void*);
void __fastcall ResetPopupInput(Common*, void*);
void __fastcall BeginPopup(Common*, void*);
void __fastcall BeginButtons(Common*, void*);
std::int32_t __fastcall PollButtons(Common*, void*);
std::int32_t __fastcall PollPopup(Common*, void*);
}
