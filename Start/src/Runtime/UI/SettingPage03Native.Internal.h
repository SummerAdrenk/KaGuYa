#pragma once
#include <Runtime/SettingPage03Native.h>
#include <Runtime/SettingPage03ResourceProfile.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/PopupNativeLifetime.h>
#include <bit>
#include <stdexcept>
#include <new>

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Control = GuiControlNativeBody50h;
using Slot = YtlReferenceSlotRuntime;
using Parent = SettingPage03NativeBody8Ch;
using Block = SettingPage03BlockNativeBody80h;
using Container = SettingPage03Block03NativeBody7Ch;
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T& Field(void* p, std::uint32_t offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
template<class T> T* ResolveBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * (c ? Read(c->strideAt0C) : 0u));
}
template<class T> T* Resolve(const Slot& r) noexcept { return ResolveBase<T>(r, Read(r.baseAt00)); }
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
inline Slot Copy(const Slot& r) noexcept { Fresh f; CopyGuiClickedEventReference(f.slot, r); return f.slot; }
inline void ReleaseOld(Slot& r)
{
    auto* c = Read(r.controlAt08);
    if (c && --c->referenceCountAt04 == 0)
    { if (c->owningAt08) c->DestroyPayloadAtVslot00(); if (auto* current = Read(r.controlAt08)) current->DeleteControlAtVslot04(1); }
}
struct RefLife final
{
    Slot& slot; bool active = true;
    ~RefLife() noexcept { if (active) ReleaseYtlReference(slot); }
    void Normal(bool clear = false) noexcept
    { active = false; if (clear) ReleaseYtlReference(slot); else ReleaseOld(slot); }
    void Captured(YtlReferenceControlRuntime* c) noexcept
    { active = false; if (c && --c->referenceCountAt04 == 0) { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); } }
};
struct Text final
{
    union { RuntimeCString value; }; bool active = false;
    Text() noexcept {}
    ~Text() noexcept { if (active) DestroyRuntimeCStringAt40CD40(&value); }
    void Normal() { active = false; DestroyRuntimeCStringAt40CD40(&value); }
};
inline const GuiDialogIntoNativeVtable37& Into(Common* p) noexcept
{ return *reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(p->vptrAt00)); }
inline const SettingPage03BlockNativeVtable42& Table(Common* p) noexcept
{ return *reinterpret_cast<const SettingPage03BlockNativeVtable42*>(Read(p->vptrAt00)); }
inline const SettingPage03Block03NativeVtable41& ContainerTable(Common* p) noexcept
{ return *reinterpret_cast<const SettingPage03Block03NativeVtable41*>(Read(p->vptrAt00)); }
inline const GuiSliderNativeVtable31& Slider(Control* p) noexcept
{ return *reinterpret_cast<const GuiSliderNativeVtable31*>(Read(p->vptrAt00)); }
inline const GuiControlNativeVtable23& CTable(Control* p) noexcept { return *Read(p->vptrAt00); }
void* Registry1(bool inlineCapture = false);
EnvironmentSoundParamsArchive* Sounds(bool inlineCapture = false);
SoundManagerNativeBody50h* RootSound();
void ConstructDialogRef(Slot&, Common*, std::uint32_t kind, std::uint32_t stride);
void DestroyBlockBase(Common*) noexcept;
struct RetiredBlockVtable38 final
{
    GuiDialogNativeVtable35 common;
    decltype(SettingPage03BlockNativeVtable42::boundsAt8C) boundsAt8C;
    decltype(SettingPage03BlockNativeVtable42::loadAt90) loadAt90, storeAt94;
};
static_assert(sizeof(RetiredBlockVtable38) == 0x98);
const RetiredBlockVtable38& BlockBaseTable() noexcept;
const SettingPage03NativeVtable39& ParentBaseTable() noexcept;
void Populate(Common&, Slot&, SettingPage03ControlKind, const void*, RECT&, bool generic = true);
std::int32_t PollManager(Slot&);
void __fastcall BeginBlock(Common*, void*);
std::int32_t __fastcall EndBlock(Common*, void*);
RECT* __fastcall BlockBounds(Common*, void*, RECT*);
BOOL __fastcall Checked(Common*, void*, std::int32_t);
std::int32_t __fastcall Value(Common*, void*, std::int32_t, std::uint8_t);
void __fastcall SetChecked(Common*, void*, std::int32_t, std::int32_t, std::int32_t);
std::int32_t __fastcall PreviewForSlider(Common*, void*, std::int32_t);
void __fastcall Load00(Common*, void*); void __fastcall Store00(Common*, void*);
void __fastcall Load01(Common*, void*); void __fastcall Store01(Common*, void*);
void __fastcall Load02(Common*, void*); void __fastcall Store02(Common*, void*);
void __fastcall Load04(Common*, void*); void __fastcall Store04(Common*, void*);
std::int32_t __fastcall PollBlock(Common*, void*);
std::int32_t __fastcall Poll04(Common*, void*);
void __fastcall Begin03(Common*, void*);
std::int32_t __fastcall End03(Common*, void*);
void __fastcall Load03(Common*, void*); void __fastcall Store03(Common*, void*);
RECT* __fastcall Bounds03(Common*, void*, RECT*);
void __fastcall Gate03(Common*, void*, std::int32_t, std::int32_t);
void __fastcall Draw03(Common*, void*, const RECT*);
void __fastcall Composite03(Common*, void*, Slot, const RECT*);
void SelectCharacterPageAt503090(Common*, std::int32_t, std::int32_t);
void SelectPageIndicatorAt503320(Common*, std::int32_t, std::int32_t);
void __fastcall ParentBegin(Common*, void*);
std::int32_t __fastcall ParentPoll(Common*, void*);
void __fastcall ParentReload(Common*, void*);
void __fastcall ParentDraw(Common*, void*, const RECT*);
void __fastcall ParentComposite(Common*, void*, Slot, const RECT*);
RECT* __fastcall ParentBounds(Common*, void*, RECT*);
std::int32_t ParentChecked(Common*, std::int32_t id);
std::int32_t ParentValue(Common*, std::int32_t id);
std::int32_t ParentPage(Common*);
void SyncMusicAt532740(Common*);
void StopPreview(Common* parent, bool characters);
void PlayPreview(Common* parent, std::int32_t button, bool characters);
void AdjustPreview(Common* parent, std::int32_t button, bool characters);
std::int32_t ReadRadio(Common*);
void WriteRadio(Common*, std::int32_t);
const GuiDialogNativeVtable35& RadioTable() noexcept;
Block* ConstructRadio(void*, Common*, std::int32_t);
}
