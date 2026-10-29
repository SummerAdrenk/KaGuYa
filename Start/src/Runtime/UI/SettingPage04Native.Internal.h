#pragma once
#include <Runtime/SettingPage04Native.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/SettingOtherResourceProfile.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
using Common=GuiDialogNativeBody5Ch;
using Control=GuiControlNativeBody50h;
using Slot=YtlReferenceSlotRuntime;
using Surface=GraphicsSurfaceNativeBody;
using Parent=SettingPage04NativeBody88h;
using Container=SettingPage04ContainerNativeBody7Ch;
using Radio=SettingPage04RadioNativeBody80h;
using TextBlock=SettingPage04Block03NativeBody8Ch;
template<class T>T Read(const T& x) noexcept {return *static_cast<const volatile T*>(&x);}
template<class T>T& Field(void* p,unsigned o) noexcept {return *reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(p)+o);}
template<class T>T* ResolveBase(const Slot& r,std::uintptr_t base) noexcept
{
    if(!base)return nullptr;
    auto* c=static_cast<EnvironmentObjectReferenceControl*>(Read(r.controlAt08));
    return reinterpret_cast<T*>(base+Read(r.biasAt04)*(c?Read(c->strideAt0C):0u));
}
template<class T>T* Resolve(const Slot& r) noexcept {return ResolveBase<T>(r,Read(r.baseAt00));}
struct Fresh final {union {Slot slot;};Fresh() noexcept {}};
inline Slot Copy(const Slot& r) noexcept {Fresh f;CopyGuiClickedEventReference(f.slot,r);return f.slot;}
inline void ReleaseOld(Slot& r)
{
    auto* c=Read(r.controlAt08);
    if(c && --c->referenceCountAt04==0)
    {if(c->owningAt08)c->DestroyPayloadAtVslot00();if(auto* current=Read(r.controlAt08))current->DeleteControlAtVslot04(1);}
}
struct RefLife final
{
    Slot& r;bool active=true;
    ~RefLife() noexcept {if(active)ReleaseYtlReference(r);}
    void Normal(bool clear=false) noexcept {active=false;if(clear)ReleaseYtlReference(r);else ReleaseOld(r);}
    void Captured(YtlReferenceControlRuntime* c,bool clear=false) noexcept
    {
        active=false;
        if(c && --c->referenceCountAt04==0){if(c->owningAt08)c->DestroyPayloadAtVslot00();c->DeleteControlAtVslot04(1);}
        if(clear && c){r.controlAt08=nullptr;r.baseAt00=0;}
    }
};
struct Text final
{
    union {RuntimeCString value;};bool active=false;
    Text() noexcept {} ~Text() noexcept {if(active)DestroyRuntimeCStringAt40CD40(&value);}
    void Normal(){active=false;DestroyRuntimeCStringAt40CD40(&value);}
};
inline const GuiDialogIntoNativeVtable37& Into(Common* p) noexcept
{return *reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(p->vptrAt00));}
inline const SettingPage04ContainerNativeVtable40& CTable(Common* p) noexcept
{return *reinterpret_cast<const SettingPage04ContainerNativeVtable40*>(Read(p->vptrAt00));}
inline const SettingPage04ManagedNativeVtable41& MTable(Common* p) noexcept
{return *reinterpret_cast<const SettingPage04ManagedNativeVtable41*>(Read(p->vptrAt00));}
inline const SettingPage04RadioNativeVtable38& RTable(Common* p) noexcept
{return *reinterpret_cast<const SettingPage04RadioNativeVtable38*>(Read(p->vptrAt00));}
inline const GuiControlNativeVtable23& ControlTable(Control* p) noexcept {return *Read(p->vptrAt00);}
inline const GuiSliderNativeVtable31& SliderTable(Control* p) noexcept
{return *reinterpret_cast<const GuiSliderNativeVtable31*>(Read(p->vptrAt00));}
void* Registry(unsigned index);
const SettingOtherBlockProfile& ProfileFor(Common*);
Control* FindControl(Common*,int);
Common* FindRadio(Common*,int);
bool ReadBool(SettingOtherBool);
void WriteBool(SettingOtherBool,bool);
bool BoolSelection(Common*,const SettingOtherBoolBinding&);
bool NameAvailable();
void ApplyNameDisabled(Common*,std::span<const int>);
void Populate(Container&,const SettingOtherBlockProfile&,Slot*,RECT&);
void AppendCheckbox(Container&,const SaveLoadMessageBoxCheckBoxDescriptorNative48h&,RECT&);
void __fastcall CheckboxBegin(Common*,void*);
int __fastcall CheckboxPoll(Common*,void*);
const SettingPage04RadioNativeVtable38& CheckboxTable() noexcept;
void ConstructDialogRef(Slot&,Common*,unsigned kind,unsigned stride);
Radio* ConstructRadio(void*,Common*,int,std::span<const GuiButtonDescriptorNative2Ch>);
void AppendRadio(Container&,const SettingPage04RadioGroup&,RECT&,bool virtualAppend);
void AppendStaticInline(Common&,const GuiStaticDescriptorNative24h&,RECT&);
void AppendEventControl(Common&,Slot& manager,Slot& child,RECT* bounds,bool capturedBinding);
void CreateGeneric(Slot&,Common*,unsigned kind,const void* descriptor);
void NewManager(Slot&);
std::int32_t PollManager(Slot&);
int ReadRadio(Common*);void WriteRadio(Common*,int);
const SettingPage04RadioNativeVtable38& RadioTable() noexcept;
const SettingPage04RadioNativeVtable38& RadioBaseTable() noexcept;
const SettingPage04NativeVtable39& ParentBaseTable() noexcept;
void DestroyBlock(Common*,unsigned kind) noexcept;
void DestroyRadio(Common*) noexcept;
void __fastcall RadioBegin(Common*,void*);int __fastcall RadioPoll(Common*,void*);
RECT* __fastcall RadioBounds(Common*,void*,RECT*);
void __fastcall ContainerBegin(Common*,void*);int __fastcall ContainerEnd(Common*,void*);
RECT* __fastcall ContainerBounds(Common*,void*,RECT*);
void __fastcall Begin01(Common*,void*);int __fastcall Poll01(Common*,void*);
void __fastcall BeginManaged(Common*,void*);int __fastcall Poll02(Common*,void*);int __fastcall Poll05(Common*,void*);
void __fastcall Gate02(Common*,void*,int,int);void __fastcall Gate05(Common*,void*,int,int);
void __fastcall Load00(Common*,void*);void __fastcall Store00(Common*,void*);
void __fastcall Load01(Common*,void*);void __fastcall Store01(Common*,void*);
void __fastcall Load02(Common*,void*);void __fastcall Store02(Common*,void*);
void __fastcall Load04(Common*,void*);void __fastcall Store04(Common*,void*);
void __fastcall Load05(Common*,void*);void __fastcall Store05(Common*,void*);
void __fastcall ParentBegin(Common*,void*);int __fastcall ParentResult(Common*,void*);
void __fastcall ParentReload(Common*,void*);RECT* __fastcall ParentBounds(Common*,void*,RECT*);
void __fastcall ParentDraw(Common*,void*,const RECT*);void __fastcall ParentComposite(Common*,void*,Slot,const RECT*);
void __fastcall CombinedDraw(Common*,void*,const RECT*);void __fastcall CombinedComposite(Common*,void*,Slot,const RECT*);
void __fastcall NameDraw(Common*,void*,const RECT*);void __fastcall NameComposite(Common*,void*,Slot,const RECT*);
void __fastcall TextDraw(Common*,void*,const RECT*);void __fastcall TextComposite(Common*,void*,Slot,const RECT*);
void BuildNameTextAt512200(Common*,int mode,int shadow);
bool BuildBlock03Text(TextBlock&,RECT&);
}
