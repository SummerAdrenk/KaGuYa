#pragma once
#include <Runtime/SettingPage01Native.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SettingTextDisplayResourceProfile.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
using Common = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
using Parent = SettingPage02NativeBody84h;
using Container = SettingPageRadioContainerNativeBody7Ch;
using Choice = SettingPageChoiceNativeBody80h;
using Mode = SettingPage02ModeNativeBody7Ch;
template<class T> T Read(const T& v) noexcept { return *static_cast<const volatile T*>(&v); }
template<class T> T& Field(void* p, std::uint32_t offset) noexcept
{ return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p) + offset); }
template<class T> T* ResolveBase(const Slot& r, std::uintptr_t base) noexcept
{
    if (!base) return nullptr;
    auto* c = static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    const auto stride = c ? Read(c->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(r.biasAt04) * stride);
}
template<class T> T* Resolve(const Slot& r) noexcept { return ResolveBase<T>(r, Read(r.baseAt00)); }
struct Fresh final { union { Slot slot; }; Fresh() noexcept {} };
inline Slot Copy(const Slot& r) noexcept { Fresh out; CopyGuiClickedEventReference(out.slot, r); return out.slot; }
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
    void Normal(bool clear = false) noexcept { active = false; if (clear) ReleaseYtlReference(slot); else ReleaseOld(slot); }
    void Captured(YtlReferenceControlRuntime* c) noexcept
    { active = false; if (!c) return; if (--c->referenceCountAt04 == 0) { if (c->owningAt08) c->DestroyPayloadAtVslot00(); c->DeleteControlAtVslot04(1); } }
};
inline const GuiDialogIntoNativeVtable37& Into(Common* p) noexcept
{ return *reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(p->vptrAt00)); }
inline const SettingPageRadioContainerNativeVtable40& CTable(Common* p) noexcept
{ return *reinterpret_cast<const SettingPageRadioContainerNativeVtable40*>(Read(p->vptrAt00)); }
inline const SettingPageChoiceNativeVtable38& QTable(Common* p) noexcept
{ return *reinterpret_cast<const SettingPageChoiceNativeVtable38*>(Read(p->vptrAt00)); }
inline void* Registry1()
{ return ResolveEnvironmentArchiveAt417990(*RequireSettingPage02NativeServices().source.CaptureEnvironmentAt4180C0(), 1); }
void ConstructDialogRef(Slot&, Common*, std::uint32_t stride);
void AssignRef(Slot&, const Slot&);
void PopulateButton(Common&, Slot& manager, const GuiButtonDescriptorNative2Ch&, RECT&, bool genericFactory);
void PopulateStatic(Common&, Slot& manager, const GuiStaticDescriptorNative24h&, RECT&);
void PopulateCheckbox(Common&, Slot& manager, const SaveLoadMessageBoxCheckBoxDescriptorNative48h&, RECT&);
std::int32_t PollChoiceManager(Slot&);
const SettingPageChoiceNativeVtable38& SettingCheckboxRowTable() noexcept;
Choice* ConstructBooleanRow(void*, Common*, const SettingTextBoolRow&);
Choice* ConstructBoundChoice(void*, Common*, const SettingTextChoiceRow&);
std::span<const SettingTextBoolRow> BooleanRows(SettingPageRadioContainerKind);
bool ReadBooleanRow(Common&, const SettingTextBoolRow&);
void WriteBooleanRow(Common&, const SettingTextBoolRow&, bool);
bool ReadSettingTextBool(SettingTextBoolField);
void WriteSettingTextBool(SettingTextBoolField, bool);
int ReadSettingTextChoice(SettingTextChoiceField);
void WriteSettingTextChoice(SettingTextChoiceField, int);
std::uint32_t ReadSettingTextSlider(unsigned index);
void WriteSettingTextSlider(unsigned index, std::uint32_t);
std::uint32_t ReadSettingTextAlpha(unsigned index);
void WriteSettingTextAlpha(unsigned index, std::uint32_t);
Container* ConstructContainer(void*, Common*, std::int32_t, SettingPageRadioContainerKind);
void DestroyChoice(Choice*) noexcept;
void DestroyMode(Mode*) noexcept;
const SettingPageChoiceNativeVtable38& ChoiceBaseTable() noexcept;
void __fastcall BeginContainer(Common*, void*);
std::int32_t __fastcall EndContainer(Common*, void*);
RECT* __fastcall ContainerBounds(Common*, void*, RECT*);
void __fastcall BeginChoice(Common*, void*);
std::int32_t __fastcall PollChoice(Common*, void*);
std::int32_t __fastcall EndChoice(Common*, void*);
RECT* __fastcall ChoiceBounds(Common*, void*, RECT*);
void __fastcall Load01B0(Common*, void*); void __fastcall Store01B0(Common*, void*);
void __fastcall Load01B1(Common*, void*); void __fastcall Store01B1(Common*, void*);
void __fastcall Load02B0(Common*, void*); void __fastcall Store02B0(Common*, void*);
void __fastcall Load02B3(Common*, void*); void __fastcall Store02B3(Common*, void*);
std::int32_t __fastcall Poll02B0(Common*, void*);
void __fastcall Load02B1(Common*, void*); void __fastcall Store02B1(Common*, void*);
void __fastcall LoadSceneChoices(Common*, void*); void __fastcall StoreSceneChoices(Common*, void*);
void __fastcall BeginMode(Common*, void*); std::int32_t __fastcall PollMode(Common*, void*);
void __fastcall ParentDraw(Common*, void*, const RECT*);
void __fastcall ParentComposite(Common*, void*, Slot, const RECT*);
RECT* __fastcall ParentBounds(Common*, void*, RECT*);
void __fastcall Reload01(Common*, void*); void __fastcall Reload02(Common*, void*);
Parent* ConstructParent(void*, Common*, std::int32_t, bool page01, ISettingPage01Block02NativeConstruction*);
}
