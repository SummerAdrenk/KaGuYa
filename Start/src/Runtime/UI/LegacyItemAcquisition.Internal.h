#pragma once
#include <Runtime/LegacyItemAcquisitionNative.h>
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <memory>
#include <stdexcept>
#include <bit>
#include <algorithm>
namespace KaGuYa::Start::Runtime::LegacyItemAcquisitionDetail
{
using Base=GuiDialogNativeBody5Ch;
using Slot=YtlReferenceSlotRuntime;
struct Top;
enum class Role { Select,System,Gauge,Date };
struct Panel final
{
    Base common;
    Top* owner;
    Role role;
    GuiClickedEventReferences events{};
    Slot canvas{};
    RECT bounds{};
    int selected=-1,latch=0;
};
struct Top final
{
    GuiDialogIntoNativeBody68h into;
    LegacyItemAcquisitionServices* services;
    Panel* select=nullptr;
    Panel* system=nullptr;
    Panel* gauge=nullptr;
    Panel* date=nullptr;
    GuiClickedEventReferences events{};
    Slot black{},canvas{};
    RECT bounds{};
    TransitOperation operation{};
    int phase=0,transition=0,pendingPhase=3,step=4,selected=-1,prompt=-1;
};
struct Ref final
{
    Slot slot{};
    Ref()=default;
    explicit Ref(Slot value):slot(value){}
    ~Ref() noexcept {ReleaseYtlReference(slot);}
    Ref(const Ref&)=delete;
};
inline Slot Copy(const Slot& s) noexcept {auto r=s;if(r.controlAt08)++r.controlAt08->referenceCountAt04;return r;}
struct Leaves final
{
    GraphicsSurfaceNativeScope surfaces;
    GuiControlGraphicsScope graphics;
    GuiInputNativeScope input;
    GuiDialogNativeScope screen;
    explicit Leaves(LegacyItemAcquisitionServices& s):surfaces(s.surfaces),graphics(s.graphics),input(s.input),screen(s.screen){}
};
inline auto& Services(Panel& p){return *p.owner->services;}
inline auto& Profile(Top& b){return RequireLegacyItemAcquisitionProfile(b.services->profile);}
inline Base& Common(Top& b){return b.into.common;}
inline int Add(int a,int b) noexcept {return std::bit_cast<int>(unsigned(a)+unsigned(b));}
inline int Sub(int a,int b) noexcept {return std::bit_cast<int>(unsigned(a)-unsigned(b));}
const GuiDialogNativeVtable35& PanelMethods() noexcept;
const GuiDialogIntoNativeVtable37& TopMethods() noexcept;
void NewPanel(Slot&,Top&,Role,int);
void DestroyPanel(Panel&) noexcept;
void DestroyTop(Top&) noexcept;
void AppendControl(Base&,GuiClickedEventReferences*,Ref&);
void AppendChecks(Panel&,std::span<const LegacyItemAcquisitionCheck>);
void RefreshAvailability(Panel&);
void RefreshSnapshot(Panel&);
GuiControlNativeBody50h& Child(Base&,int);
void SystemBegin(Panel&);
void ResetSelection(Panel&);
void CompositeTop(Top&,Slot owned,const RECT*);
}
