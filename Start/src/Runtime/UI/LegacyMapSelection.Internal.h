#pragma once

#include <Runtime/LegacyMapSelectionNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/SaveLoadMessageBoxNativeChildFactoriesRuntime.h>
#include <Runtime/WaitMarkServicesRuntime.h>
#include <algorithm>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
using Body=LegacyMapSelectionDialog;
using Panel=LegacyMapSelectionPanel;
using Gui=GuiDialogNativeBody5Ch;
using Slot=YtlReferenceSlotRuntime;
struct Ref final
{
    Slot slot{};
    ~Ref(){ReleaseYtlReference(slot);}
    Ref()=default;
    Ref(const Ref&)=delete;
    Ref& operator=(const Ref&)=delete;
};
inline Slot Copy(Slot s){if(s.controlAt08)++s.controlAt08->referenceCountAt04;return s;}
inline int Add(int a,int b){return std::bit_cast<int>(unsigned(a)+unsigned(b));}
int Read(Body&,const char*);
void Write(Body&,const char*,int);
Panel& View(Body&,int);
LegacyMapSelectionControl* Find(Body&,int view,int event);
void Visible(Body&,int view,int event,bool);
void Enabled(Body&,int view,int event,bool);
void Apply(Body&);
void Reset(Body&,int view);
void Dirty(Body&);
void AddWidget(Body&,const LegacyMapSelectionWidget&,int view,bool event=true);
void AddCanvas(Body&,GraphicsSurfaceReference&,int view,int event);
GraphicsSurfaceReference& Canvas(Body&,int view,int event);
void CreateCanvas(Body&,GraphicsSurfaceReference&,int alpha=1);
void Load(Body&,GraphicsSurfaceReference&,const char*);
void Blit(Body&,GraphicsSurfaceReference&,const GraphicsSurfaceReference&,POINT,bool checked=true);
void Natural(Body&,GraphicsSurfaceReference&,const char*);
int SelectIndex(Body&,LegacyMapSelectionIndex,const char*);
int Input(Body&,int view);
void Navigation(Body&,Panel&);
void ConstructArea(Body&);
void BeginArea(Body&);
void SelectArea(Body&,int);
void PollArea(Body&);
void RenderArea(Body&,int view,const RECT*,const Slot*);
void ConstructBook(Body&);
void BeginBook(Body&);
void PollBook(Body&);
void ConstructDecision(Body&);
void SelectDecisionActor(Body&,int);
void PollDecision(Body&);
void ConstructAnimated(Body&);
void BeginAnimated(Body&);
void PollAnimated(Body&);
void RenderAnimations(Body&,int,const RECT*,const Slot*);
void AdvanceAnimations(Body&);
void Begin(Gui*);
int Poll(Gui*);
int End(Gui*);
void Close(Body&,int result,int secondary=-1);
void Transition(Body&);
void Destroy(Body&) noexcept;
void ConfigurePopup(GuiControlNativeBody50h&);
}
