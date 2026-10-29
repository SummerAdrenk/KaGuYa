#include "LegacyItemAcquisition.Internal.h"
namespace KaGuYa::Start::Runtime::LegacyItemAcquisitionDetail
{
namespace
{
int& Checked(Panel& p,int event){return reinterpret_cast<GuiCheckBoxNativeBody60h&>(Child(p.common,event)).checkedAt5C;}
void Disable(Panel& p,int event){auto& c=Child(p.common,event);c.vptrAt00->writeStateAt44(&c,0);}
Base* __fastcall DeletePanel(Base* base,void*,unsigned f) noexcept
{auto* p=reinterpret_cast<Panel*>(base);DestroyPanel(*p);if(f&1u)::operator delete(p,sizeof(Panel));return base;}
Base* __fastcall DeleteTop(Base* base,void*,unsigned f) noexcept
{auto* b=reinterpret_cast<Top*>(base);DestroyTop(*b);if(f&1u)::operator delete(b,sizeof(Top));return base;}
void __fastcall ResetInput(Base* base,void*)
{reinterpret_cast<Panel*>(base)->latch=0;GuiDialogBaseVtable().resetInputAt3C(base);}
int __fastcall ConfirmKey(Base* base,void*)
{
    auto& p=*reinterpret_cast<Panel*>(base);auto* c=base->activeAt4C;
    if(p.role!=Role::Select||!c||(c->typeAt08!=0&&c->typeAt08!=2))return 0;
    const auto previous=p.latch;
    if(!previous)
    {if(!base->vptrAt00->pressedAt70(base,5))return 0;p.latch=1;GuiButtonVtable().setPressedAt70(c,1);return 1;}
    if(!base->vptrAt00->heldAt6C(base,5))
    {Ref event(Copy(c->eventAt3C));if(auto* e=ResolveGuiClickedEvent(event.slot))e->vptrAt00->invokeAt00(e);p.latch=0;}
    return previous;
}
void __fastcall BeginPanel(Base* base,void*)
{
    auto& p=*reinterpret_cast<Panel*>(base);Leaves leaves(Services(p));
    base->vptrAt00->writeEnabledAt24(base,1);ClearGuiClickedEventFlagsAt532200(p.events);
    base->vptrAt00->resetInputAt3C(base);base->vptrAt00->resetChildrenAt68(base);p.selected=-1;
    if(p.role==Role::Select)RefreshAvailability(p);else if(p.role==Role::System)SystemBegin(p);
}
void SystemEvent(Panel& p,int event)
{
    if(event==20){Checked(p,20)=!Checked(p,20);Services(p).source.UnSave().FlagsAt18()^=2u;return;}
    for(const auto& r:Profile(*p.owner).routes)if(r.event==event)
    {auto& c=Child(p.common,event);c.vptrAt00->resetAt08(&c);Services(p).source.Command(r.command);return;}
}
int __fastcall PollPanel(Base* base,void*)
{
    auto& p=*reinterpret_cast<Panel*>(base);Leaves leaves(Services(p));
    if(p.role==Role::Gauge||p.role==Role::Date)return 0;
    base->vptrAt00->processInputAt54(base);const int event=PollGuiClickedEventManagerNativeAt50FDB0(&p.events,1);
    if(p.role==Role::System){if(event>=0)SystemEvent(p,event);return 1;}
    if(event<6||event>13)return 0;
    p.selected=event-6;
    // 原0x50A5B0在余额检查及确认之前播放取得音；不足/取消也不能挪到提交后才响。
    Services(p).source.SelectionCue(Profile(*p.owner).selectionCue);return 1;
}
int __fastcall EndPanel(Base*,void*){return 1;}
void __fastcall WritePanelState(Base* base,void*,int value)
{
    auto& p=*reinterpret_cast<Panel*>(base);
    // 原0x4DB2D0的System setter只转写event22，不改变整个dialog.state。
    if(p.role==Role::System){auto& c=Child(*base,22);c.vptrAt00->writeStateAt44(&c,value);}
    else GuiDialogBaseVtable().writeStateAt1C(base,value);
}
void __fastcall BeginTop(Base* base,void*)
{
    auto& b=*reinterpret_cast<Top*>(base);Leaves leaves(*b.services);
    ClearGuiClickedEventFlagsAt532200(b.events);base->vptrAt00->writeEnabledAt24(base,1);
    base->vptrAt00->resetInputAt3C(base);base->vptrAt00->resetChildrenAt68(base);
    for(auto* p:{b.select,b.gauge,b.system,b.date})p->common.vptrAt00->beginAt44(&p->common);
    b.phase=0;b.transition=0;b.selected=-1;b.prompt=-1;
}
void __fastcall DrawTop(Base* base,void*,const RECT* clip)
{
    // 原0x4DE0C0先画top静态/退出，再画四个dialog；共享Into本身不画top.controls。
    GuiDialogBaseVtable().drawAt60(base,clip);GuiDialogIntoBaseVtable().common.drawAt60(base,clip);
}
void __fastcall Composite(Base* base,void*,Slot owned,const RECT* clip)
{CompositeTop(*reinterpret_cast<Top*>(base),owned,clip);}
void AdvanceTransition(Top& b)
{
    auto& source=b.services->source;
    if(b.transition==0)
    {
        if(b.phase==1){CompositeTop(b,Copy(b.canvas),nullptr);source.StartAudio(Profile(b).bgm);}
        b.operation={};b.operation.opcode=0;
        b.operation.source=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(b.phase==1?b.canvas:b.black);
        b.pendingPhase=b.phase==1?3:4;source.UnSave().ModeAt04()=0;b.step=source.TransitionStep();b.transition=1;return;
    }
    if(b.transition==1)
    {
        if(b.operation.opcode==-1||b.operation.current>=b.operation.target)b.transition=2;
        if(source.AccelerateTransition())b.operation.current=std::min(Add(b.operation.current,256),b.operation.target);
        source.RenderTransition(b.operation);
        b.operation.current=std::min(Add(b.operation.current,b.step),b.operation.target);return;
    }
    if(b.transition==2){b.phase=b.pendingPhase;source.UnSave().ModeAt04()=1;}
}
void ProcessPrompt(Top& b)
{
    const int mode=b.prompt;auto& s=*b.services;
    Ref capture;AdoptGraphicsSurfaceAt407A80(capture.slot,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
    auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(capture.slot);
    if(!image->CreateAt415010(s.surfaces,b.bounds.right,b.bounds.bottom,0,0))throw 1;
    CompositeTop(b,Copy(capture.slot),nullptr);
    const int answer=s.source.Confirm(mode,capture.slot);
    if(mode==10)
    {if(answer==6){b.phase=4;b.transition=0;}b.prompt=-1;return;}
    ResetSelection(*b.select);
    if(mode==12&&answer==6&&static_cast<unsigned>(b.selected)<Profile(b).products.size())
    {
        const auto& product=Profile(b).products[static_cast<std::size_t>(b.selected)];
        // 原0x510FF4..0x5110F1：先道具=1，再晚读余额扣除，再晚读累计量加价；异常不回滚。
        s.source.Write(product.variable,1);
        s.source.Write(Profile(b).balance,Sub(s.source.Read(Profile(b).balance),product.cost));
        s.source.Write(Profile(b).spent,Add(s.source.Read(Profile(b).spent),product.cost));
        RefreshSnapshot(*b.gauge);RefreshAvailability(*b.select);ResetSelection(*b.select);
    }
    b.prompt=-1;
}
int __fastcall PollTop(Base* base,void*)
{
    auto& b=*reinterpret_cast<Top*>(base);Leaves leaves(*b.services);
    if(b.phase==0){b.phase=1;b.transition=0;}
    if(b.phase==1||b.phase==2){AdvanceTransition(b);return 1;}
    if(b.phase==4){b.phase=-1;return 0;}
    if(b.phase!=3)return 0;
    if(b.prompt>=0){ProcessPrompt(b);base->vptrAt00->flushDirtyAt58(base);return 1;}
    base->vptrAt00->processInputAt54(base);
    if(b.system->common.vptrAt00->readStateAt20(&b.system->common))b.system->common.vptrAt00->pollAt48(&b.system->common);
    const int event=PollGuiClickedEventManagerNativeAt50FDB0(&b.events,1);
    if(event==5)
    {Child(*base,5).vptrAt00->resetAt08(&Child(*base,5));b.prompt=10;}
    else if(b.select->common.vptrAt00->readEnabledAt28(&b.select->common)
        &&b.select->common.vptrAt00->pollAt48(&b.select->common))
    {
        const int balance=b.services->source.Read(Profile(b).balance);b.selected=b.select->selected;
        if(static_cast<unsigned>(b.selected)<8u)
        {
            auto& c=Child(b.select->common,Profile(b).products[static_cast<std::size_t>(b.selected)].event);
            GuiButtonVtable().setPressedAt70(&c,1);
            // 原0x5114A0 setle为有符号比较；确认返回后不再二次检查余额/拥有标志。
            b.prompt=Profile(b).products[static_cast<std::size_t>(b.selected)].cost<=balance?12:11;
        }
    }
    base->vptrAt00->flushDirtyAt58(base);return 1;
}
int __fastcall EndTop(Base* base,void*)
{base->vptrAt00->writeEnabledAt24(base,0);return 1;}
}
void CompositeTop(Top& b,Slot owned,const RECT* clip)
{
    Ref target(owned);auto* base=&Common(b);
    GuiDialogBaseVtable().compositeAt5C(base,Copy(target.slot),clip);
    GuiDialogIntoBaseVtable().common.compositeAt5C(base,Copy(target.slot),clip);
}
void ResetSelection(Panel& p)
{
    p.common.vptrAt00->resetChildrenAt68(&p.common);
    // 原0x4D3890在确认返回后显式invalidate整个Select bounds，不能只清pressed而留下旧像素。
    p.common.vptrAt00->invalidateAt64(&p.common,p.bounds);
}
void RefreshAvailability(Panel& p)
{
    for(const auto& item:Profile(*p.owner).products)
    {auto& c=Child(p.common,item.event);c.vptrAt00->writeStateAt44(&c,Services(p).source.Read(item.variable)==0);}
}
void RefreshSnapshot(Panel& p)
{
    auto& s=Services(p);auto* image=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(p.canvas);
    s.graphics.CompositionAt4182F0().ClearSurface(image->handleAt04,image->clearColorAt20,nullptr);
    s.source.SnapshotLayer(p.role==Role::Gauge,*image);
    const RECT rect{image->originXAt10,image->originYAt14,image->originXAt10+image->widthAt08,image->originYAt14+image->heightAt0C};
    p.common.vptrAt00->invalidateAt64(&p.common,rect);
}
void SystemBegin(Panel& p)
{
    // 原0x509C20：15声音、17透明、21skip、22好感度常禁用，20auto可切换。
    Disable(p,15);Disable(p,17);Disable(p,21);Disable(p,22);
    if(Services(p).source.UnSave().FlagsAt18()&2u)Checked(p,20)=1;
    Checked(p,21)=(Services(p).source.UnSave().FlagsAt14()&2u)!=0;
}
const GuiDialogNativeVtable35& PanelMethods() noexcept
{
    static const auto table=[] {auto t=GuiDialogBaseVtable();
#define ITEM_SLOT(n,f) t.n=reinterpret_cast<decltype(t.n)>(&f)
        ITEM_SLOT(deletingDestructorAt04,DeletePanel);ITEM_SLOT(resetInputAt3C,ResetInput);ITEM_SLOT(activeInputAt38,ConfirmKey);
        ITEM_SLOT(writeStateAt1C,WritePanelState);
        ITEM_SLOT(beginAt44,BeginPanel);ITEM_SLOT(pollAt48,PollPanel);ITEM_SLOT(endAt4C,EndPanel);
#undef ITEM_SLOT
        return t;}();return table;
}
const GuiDialogIntoNativeVtable37& TopMethods() noexcept
{
    static const auto table=[] {auto t=GuiDialogIntoBaseVtable();
#define ITEM_SLOT(n,f) t.common.n=reinterpret_cast<decltype(t.common.n)>(&f)
        ITEM_SLOT(deletingDestructorAt04,DeleteTop);ITEM_SLOT(beginAt44,BeginTop);ITEM_SLOT(pollAt48,PollTop);
        ITEM_SLOT(endAt4C,EndTop);ITEM_SLOT(drawAt60,DrawTop);ITEM_SLOT(compositeAt5C,Composite);
#undef ITEM_SLOT
        return t;}();return table;
}
}
namespace KaGuYa::Start::Runtime
{
void ReopenLegacyItemAcquisitionSystem(GuiDialogNativeBody5Ch& base)
{
    using namespace LegacyItemAcquisitionDetail;auto& b=reinterpret_cast<Top&>(base);Leaves leaves(*b.services);
    auto& skip=Child(b.system->common,21);reinterpret_cast<GuiCheckBoxNativeBody60h&>(skip).checkedAt5C=(b.services->source.UnSave().FlagsAt14()&2u)!=0;
    skip.vptrAt00->writeStateAt44(&skip,0);base.vptrAt00->invalidateAt64(&base,b.bounds);base.vptrAt00->flushDirtyAt58(&base);
}
void BeginLegacyItemAcquisitionExitTransition(GuiDialogNativeBody5Ch& base)
{auto& b=reinterpret_cast<LegacyItemAcquisitionDetail::Top&>(base);b.phase=2;b.transition=0;}
}
