#include "LogDialogNative.Internal.h"
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/PopupNativePopulation.h>
#include <Runtime/LogPageResourceProfile.h>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
void ReleaseRetain(Slot& r)
{
    auto* c=Read(r.controlAt08);if(c&&--c->referenceCountAt04==0)
    {if(c->owningAt08)c->DestroyPayloadAtVslot00();if(auto* now=Read(r.controlAt08))now->DeleteControlAtVslot04(1);}
}
void BindOwned(Control& p,Slot argument)
{
    RefLife life{argument};
    if(&p.eventAt3C!=&argument)
    {
        ReleaseRetain(p.eventAt3C);
        p.eventAt3C={Read(argument.baseAt00),Read(argument.biasAt04),Read(argument.controlAt08)};
        if(p.eventAt3C.controlAt08)++p.eventAt3C.controlAt08->referenceCountAt04;
    }
    p.eventEnabledAt48=1;life.TerminateNormal(false);
}
void AppendEventOwned(Slot& manager,Slot argument)
{
    RefLife life{argument};AppendGuiClickedEventReference(*Resolve<GuiClickedEventReferences>(manager),argument);
    life.TerminateNormal(false);
}
void Manager(Slot& out)
{
    void* raw=::operator new(0xC);
    auto* created=raw?::new(raw) GuiClickedEventReferences{}:nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(out,created);
}
void UnionControl(RECT& bounds,const Slot& child)
{auto* c=Resolve<Control>(child);RECT out;auto* r=Read(c->vptrAt00)->rectAt3C(c,&out);::UnionRect(&bounds,&bounds,r);}
void NewChild(Slot& out,bool slider,Common* parent,const void* descriptor)
{
    auto& s=RequireLogDialogNativeServices();
    if(slider)CreateGuiSliderAt5052C0(out,parent,*static_cast<const GuiSliderDescriptorNative6Ch*>(descriptor),s.pictures,s.surfaces,s.strings);
    else CreateGuiButtonAt5041D0(out,parent,*static_cast<const GuiButtonDescriptorNative2Ch*>(descriptor),s.pictures,s.surfaces,s.strings);
}
}
void BindEvent(Common& owner,Slot& child,Slot& manager,int event,bool inlineControl)
{
    Fresh clicked;
    if(inlineControl)CreatePopupClickedEventAt4EA7B3(clicked.slot,event);
    else CreateToolClickedEventAt4E2F5A(clicked.slot,child);
    RefLife eventLife{clicked.slot};auto* captured=Read(clicked.slot.controlAt08);
    if(inlineControl)
    {
        Slot binding{Read(clicked.slot.baseAt00),0,captured};if(captured)++captured->referenceCountAt04;
        BindPopupGuiControlEventAt4EA856(*Resolve<Control>(child),binding,clicked.slot);
    }
    else BindOwned(*Resolve<Control>(child),Copy(clicked.slot));
    if(inlineControl)
    {
        Slot copied=Copy(child);RefLife life{copied};AppendGuiControlChildReference(owner.childrenAt0C,copied);
        life.TerminateCaptured(Read(copied.controlAt08));
        Slot eventCopy{Read(clicked.slot.baseAt00),0,captured};if(captured)++captured->referenceCountAt04;
        RefLife copiedLife{eventCopy};AppendGuiClickedEventReference(*Resolve<GuiClickedEventReferences>(manager),eventCopy);
        copiedLife.TerminateCaptured(Read(eventCopy.controlAt08));
    }
    else
    {
        GuiDialogBaseVtable().appendChildAt10(&owner,Copy(child));
        AppendEventOwned(manager,Copy(clicked.slot));
    }
    eventLife.TerminateCaptured(captured);
}
Row* NewRow()
{
    auto& s=RequireLogDialogNativeServices();auto* raw=s.memory.AllocateBody(0x30);Raw allocation{raw,0x30};
    if(!raw){allocation.active=false;return nullptr;}
    auto* p=::new(raw) Row;p->vptrAt00=&LogTextNativeVtable();p->xAt04=p->yAt08=0;
    p->voicesAt18={};p->surfaceAt24={};
    struct Prefix final
    {
        Row* p;bool active=true;
        ~Prefix() noexcept{if(active){ReleaseYtlReference(p->surfaceAt24);DestroyRuntimeCStringVectorAt40DD20(p->voicesAt18);}}
    }prefix{p};
    AdoptGraphicsSurfaceAt407A80(p->surfaceAt24,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
    const auto size=CurrentLogPageResourceProfile().rowSurface;
    if(!Resolve<GraphicsSurfaceNativeBody>(p->surfaceAt24)->CreateAt415010(s.surfaces,size.cx,size.cy,1,1))throw 0;
    p->formatAt10=p->eventAt14=p->historyAt0C=-1;prefix.active=false;allocation.active=false;return p;
}
}

namespace KaGuYa::Start::Runtime
{
LogTextDialogNativeBodyA0h* ConstructLogTextDialogNativeAt4E1B70(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    static_cast<void>(CurrentLogPageResourceProfile());
    using namespace LogDialogNativeDetail;auto& s=RequireLogDialogNativeServices();
    auto* p=::new(raw) Text;ConstructGuiDialogAt4E0BE0(&p->common,parent,id);
    p->common.vptrAt00=&LogTextDialogNativeVtable().common;p->eventsAt5C={};p->rowsAt68={};
    p->voicesAt78={};p->boundsAt84={};p->frameAt94={};
    struct Unwind final
    {
        Text* p;bool active=true;
        ~Unwind() noexcept
        {
            if(!active)return;
            ReleaseYtlReference(p->frameAt94);ReleaseYtlReference(p->voicesAt78);DestroyRows(p->rowsAt68);
            ReleaseYtlReference(p->eventsAt5C);DestroyGuiDialogAt4F7AC0(&p->common);
        }
    }unwind{p};
    auto* fixedRoot=Root(0x4E1C50);auto* draw=Resolve<void>(s.source.RootDrawSlotAt18(fixedRoot));
    if(!s.source.LoadSurfaceAt480000(draw,p->frameAt94,FrameName()))throw 1;
    RECT bounds{};
    for(const auto point:RowPositions())
    {
        auto* row=NewRow();Fresh found;AdoptRow(found.slot,row);RefLife life{found.slot};
        auto* rowControl=Read(found.slot.controlAt08); // 4E1DB5的EDI跨UnionRect及AppendRow保留。
        auto* surface=Resolve<GraphicsSurfaceNativeBody>(Resolve<Row>(found.slot)->surfaceAt24);
        const RECT r{point.x,point.y,Add(point.x,Read(surface->widthAt08)),Add(point.y,Read(surface->heightAt0C))};
        ::UnionRect(&bounds,&bounds,&r);
        Slot copied{Read(found.slot.baseAt00),0,rowControl};if(copied.controlAt08)++copied.controlAt08->referenceCountAt04;
        RefLife copyLife{copied};AppendRow(p->rowsAt68,copied);copyLife.TerminateCaptured(Read(copied.controlAt08));
        life.TerminateCaptured(rowControl);
    }
    Manager(p->eventsAt5C);GuiFactoryConstructionLifetime factory;
    for(int i=0;i<4;++i)
    {
        static_cast<void>(Resolve<void>(factory.Reference()));Fresh child;NewChild(child.slot,false,parent,&VoiceButton());RefLife life{child.slot};
        auto* control=Resolve<Control>(child.slot);control->eventAt04=10+i;
        const auto point=RowPositions()[i],shift=VoiceShift();control->xAt34=Add(point.x,shift.x);control->yAt38=Add(point.y,shift.y);
        UnionControl(bounds,child.slot);BindEvent(p->common,child.slot,p->eventsAt5C,10+i,true);life.TerminateNormal();
    }
    p->boundsAt84=bounds;factory.Finish();unwind.active=false;return p;
}
LogScrollDialogNativeBody80h* ConstructLogScrollDialogNativeAt4F10C0(void* raw,GuiDialogNativeBody5Ch* parent,int id)
{
    const bool repeat=CurrentLogPageResourceProfile().scrollPolicy==LogPageScrollPolicy::PageKeysAndHoldRepeat;
    using namespace LogDialogNativeDetail;auto* p=::new(raw) Scroll;
    ConstructGuiDialogAt4E0BE0(&p->common,parent,id);p->common.vptrAt00=&LogScrollDialogNativeVtable().common;
    p->boundsAt5C={};p->eventsAt74={};
    struct Unwind final
    {
        Scroll* p;bool active=true;
        ~Unwind() noexcept{if(active){ReleaseYtlReference(p->eventsAt74);DestroyGuiDialogAt4F7AC0(&p->common);}}
    }unwind{p};
    Manager(p->eventsAt74);GuiFactoryConstructionLifetime factory;RECT bounds{};
    for(const auto& descriptor:ScrollButtons())
    {
        static_cast<void>(Resolve<void>(factory.Reference()));Fresh child;NewChild(child.slot,false,parent,&descriptor);RefLife life{child.slot};
        UnionControl(bounds,child.slot);BindEvent(p->common,child.slot,p->eventsAt74,descriptor.common.eventAt00,false);
        // 01–09普通click保持启用；10原4E40A0、18原4F1323/4F150B清0后由Scroll重复处理。
        if(repeat)Resolve<Control>(child.slot)->eventEnabledAt48=0;
        life.TerminateNormal();
    }
    {
        static_cast<void>(Resolve<void>(factory.Reference()));Fresh child;NewChild(child.slot,true,parent,&ScrollSlider());RefLife life{child.slot};
        UnionControl(bounds,child.slot);auto* c=Resolve<Control>(child.slot);Slider(c).setStepInputAt74(c,0);
        GuiDialogBaseVtable().appendChildAt10(&p->common,Copy(child.slot));life.TerminateNormal();
    }
    if(repeat)for(int event:{5,6})
    {
        Fresh f;auto* clicked=NewGuiClickedEventAt4EA7B3(event);ConstructGuiClickedEventReferenceAt4D7780(f.slot,clicked,1);
        RefLife life{f.slot};auto* captured=Read(f.slot.controlAt08);AppendEventOwned(p->eventsAt74,Copy(f.slot));life.TerminateCaptured(captured,false);
    }
    p->boundsAt5C=bounds;factory.Finish();unwind.active=false;return p;
}
GuiDialogNativeBody5Ch* ConstructLogDialogNativeAt4E23D0(void* raw)
{
    // 在构造前明确拒绝未绑定配置；独立getter同样允许该错误正常外传。
    static_cast<void>(CurrentLogPageResourceProfile());
    using namespace LogDialogNativeDetail;auto& s=RequireLogDialogNativeServices();auto* p=::new(raw) DC;
    ConstructGuiDialogAt4E0BE0(&p->common,nullptr,-1);p->common.vptrAt00=&LogDialogNativeVtable();p->boundsAt68={};
    p->textAt78={};p->scrollAt84={};p->backgroundAt90={};p->reservedAt9C={};
    p->fieldAtA8=p->fieldAtB0=p->fieldAtAC=p->fieldAtB4=p->fieldAtBC=p->fieldAtB8=p->fieldAtC0=0;
    p->fieldAtC4=-1;p->fieldAtC8=p->fieldAtCC=0;p->fieldAtD0=0xFF;p->fieldAtD4=0;
    struct Unwind final
    {
        DC* p;bool active=true;
        ~Unwind() noexcept
        {
            if(!active)return;
            ReleaseYtlReference(p->reservedAt9C);ReleaseYtlReference(p->backgroundAt90);
            ReleaseYtlReference(p->scrollAt84);ReleaseYtlReference(p->textAt78);DestroyGuiDialogAt4F7AC0(&p->common);
        }
    }unwind{p};
    {
        auto* storage=s.memory.AllocateBody(0xA0);Raw life{storage,0xA0};
        auto* child=storage?ConstructLogTextDialogNativeAt4E1B70(storage,&p->common,0):nullptr;life.active=false;
        AdoptDialog(p->textAt78,child?&child->common:nullptr,0xA0);
    }
    {
        auto* storage=s.memory.AllocateBody(0x80);Raw life{storage,0x80};
        auto* child=storage?ConstructLogScrollDialogNativeAt4F10C0(storage,&p->common,1):nullptr;life.active=false;
        AdoptDialog(p->scrollAt84,child?&child->common:nullptr,0x80);
    }
    GuiFactoryConstructionLifetime factory;static_cast<void>(Resolve<void>(factory.Reference()));
    Fresh child;CreateGuiStaticAt505800(child.slot,&p->common,BackgroundDescriptor(),s.pictures,s.surfaces,s.strings);RefLife life{child.slot};
    GuiDialogBaseVtable().appendChildAt10(&p->common,Copy(child.slot));life.TerminateNormal(); // DC Ctor state9正常尾。
    const auto height=Read(s.source.CaptureEnvironmentAt4180C0(0x4E2765)->heightAt1C);
    const auto width=Read(s.source.CaptureEnvironmentAt4180C0(0x4E276D)->widthAt18);
    p->boundsAt68={0,0,std::bit_cast<int>(width),std::bit_cast<int>(height)};
    AdoptGuiInputAt536EA0(p->common.inputAt18,NewGameGuiInputAt4EAAED());
    factory.Finish();unwind.active=false;return &p->common;
}
}
