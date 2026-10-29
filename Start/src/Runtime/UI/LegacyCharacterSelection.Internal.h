#pragma once
#include <Runtime/LegacyCharacterSelectionNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/WaitMarkServicesRuntime.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::LegacyCharacterSelectionDetail
{
using Body=LegacyCharacterSelectionDialog;
using Gui=GuiDialogNativeBody5Ch;
using Slot=YtlReferenceSlotRuntime;
struct Ref final
{
    Slot slot{};
    ~Ref(){ReleaseYtlReference(slot);}
    Slot Take(){auto result=slot;slot={};return result;}
};
inline Slot Copy(Slot s){if(s.controlAt08)++s.controlAt08->referenceCountAt04;return s;}
inline int Add(int a,int b){return std::bit_cast<int>(unsigned(a)+unsigned(b));}
inline int Sub(int a,int b){return std::bit_cast<int>(unsigned(a)-unsigned(b));}
int Read(Body&,const char*);
void Write(Body&,const char*,int);
LegacyCharacterSelectionControl* Find(Body&,int);
void Visible(Body&,int,bool);
void Enabled(Body&,int,bool);
void Apply(Body&);
void Navigation(Body&);
void Hotkeys(Body&);
void Dirty(Body&);
void AddWidget(Body&,const LegacyCharacterSelectionWidget&,int view);
void AddCanvas(Body&,GraphicsSurfaceReference&,int event,int view);
void Load(Body&,GraphicsSurfaceReference&,const char*);
void CreateCanvas(Body&,GraphicsSurfaceReference&,int alpha=1);
void RefreshTabs(Body&,int);
void RefreshTabPictures(Body&);
void RefreshPages(Body&);
void Hover(Body&);
void RenderAnimations(Body&,const RECT*,const Slot*);
void Transition(Body&);
void Begin(Gui*,void*);
int Poll(Gui*,void*);
int End(Gui*,void*);
void Destroy(Body&) noexcept;
}
