#include "LogDialogNative.Internal.h"
#include <Runtime/LogPageResourceProfile.h>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
KeyboardManagerNativeBody438h* Keys(std::uint32_t site)
{auto* input=RequireLogDialogNativeServices().source.CaptureInputAt418210(site);return Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);}
bool Held(KeyboardManagerNativeBody438h& keys,int action)
{
    auto* at=Read(keys.actionsAt14[action].begin);auto* end=Read(keys.actionsAt14[action].end);
    if(at==end)return false;const auto buffer=Read(keys.bufferAt10);
    for(;at!=end;++at)if(Read(at->states[buffer]))return true;return false;
}
bool Pressed(KeyboardManagerNativeBody438h& keys,int action)
{
    auto* at=Read(keys.actionsAt14[action].begin);auto* end=Read(keys.actionsAt14[action].end);
    if(at==end)return false;const auto buffer=Read(keys.bufferAt10);
    for(;at!=end;++at)if(Read(at->states[buffer])&&!Read(at->states[1u-buffer]))return true;return false;
}
void FireManager(Scroll& p,int id,int counter,bool first)
{
    Fresh f;f.slot={};auto* manager=Resolve<GuiClickedEventReferences>(p.eventsAt74);
    for(auto* at=Read(manager->begin);at!=Read(manager->end);++at)
        if(Read(Resolve<GuiClickedEventNativeBody>(*at)->eventValueAt08)==id){f.slot=Copy(*at);break;}
    RefLife life{f.slot};auto* captured=Read(f.slot.controlAt08);if(Read(f.slot.baseAt00))
    {auto* e=Resolve<GuiClickedEventNativeBody>(f.slot);Read(e->vptrAt00)->invokeAt00(e);}
    p.repeatAt6C=counter;if(first)p.heldEventAt70=id;
    if(first&&id==5)life.TerminateCaptured(captured,false);else life.TerminateNormal();
}
bool RepeatKey(Scroll& p,int id)
{
    if(Read(p.heldEventAt70)==-1){FireManager(p,id,30,true);return true;}
    if(Read(p.heldEventAt70)!=id)return false;
    if(Read(p.repeatAt6C))p.repeatAt6C=Sub(Read(p.repeatAt6C),1);
    else FireManager(p,id,2,false);
    return true;
}
void FireButton(Scroll& p,Slot& button,int id,int counter,bool first)
{
    Slot event=Copy(Resolve<Control>(button)->eventAt3C);RefLife life{event};
    if(Read(event.baseAt00)){auto* e=Resolve<GuiClickedEventNativeBody>(event);Read(e->vptrAt00)->invokeAt00(e);}
    p.repeatAt6C=counter;if(first)p.heldEventAt70=id;
    life.TerminateNormal();
}
int State(Slot& r)
{auto* c=Resolve<Control>(r);return Read(c->vptrAt00)->readStateAt48(c);}
void SetState(Slot& r,int state)
{auto* c=Resolve<Control>(r);Read(c->vptrAt00)->writeStateAt44(c,state);}
int Style(Slot& r)
{if(!State(r))return 7;auto* c=Resolve<Control>(r);return Read(c->vptrAt00)->resourceStateAt38(c);}
void RepeatButton(Scroll& p,Slot& button,int id,int before,int now)
{
    if(before!=now)
    {
        if(now==5&&Read(p.heldEventAt70)==-1)FireButton(p,button,id,30,true);
    }
    else if(Read(p.heldEventAt70)==id)
    {
        if(before!=5)p.heldEventAt70=-1;
        else if(Read(p.repeatAt6C))p.repeatAt6C=Sub(Read(p.repeatAt6C),1);
        else FireButton(p,button,id,2,false);
    }
}
}
void __fastcall BeginScroll(Common* p,void*)
{
    auto& b=*reinterpret_cast<Scroll*>(p);
    for(auto* at=Read(p->childrenAt0C.begin);at!=Read(p->childrenAt0C.end);++at)
    {auto* c=Resolve<Control>(*at);Read(c->vptrAt00)->resetAt08(c);}
    auto* params=Parameters(0x50EDF3);
    const auto count=std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(Read(params->historyAt3C.storage.end))-
        reinterpret_cast<std::uintptr_t>(Read(params->historyAt3C.storage.begin)))/4;
    if(count<=4)
    {
        for(int id:{2,3,4}){Fresh f;Find(p,f.slot,id);RefLife life{f.slot};SetState(f.slot,0);life.TerminateNormal();}
    }
    else
    {
        Fresh f;Find(p,f.slot,2);RefLife life{f.slot};auto* c=Resolve<Control>(f.slot);
        Slider(c).setRangeAt5C(c,0,Sub(count,4));c=Resolve<Control>(f.slot);Slider(c).setValueAt64(c,Sub(count,4));life.TerminateNormal(false);
    }
    b.heldEventAt70=-1;
}
int ScrollPosition(Common* p)
{
    Fresh f;Find(p,f.slot,2);RefLife life{f.slot};int value=0;
    if(Read(f.slot.baseAt00)&&State(f.slot)){auto* c=Resolve<Control>(f.slot);value=Slider(c).valueAt60(c);}
    life.TerminateNormal(false);return value;
}
void MoveScroll(Common* p,int delta)
{
    Fresh f;Find(p,f.slot,2);RefLife life{f.slot};
    if(Read(f.slot.baseAt00)&&State(f.slot))
    {
        auto* c=Resolve<Control>(f.slot);const auto value=Slider(c).valueAt60(c);
        c=Resolve<Control>(f.slot);const bool reverse=Slider(c).reverseAt70(c)!=0;
        c=Resolve<Control>(f.slot);Slider(c).setValueAt64(c,Add(value,reverse?Sub(0,delta):delta));
    }
    life.TerminateNormal(false);
}
int __fastcall UpperScroll(Common* p,void*)
{
    // 01–09原上层输入委托公共GUI；不注入10以后才有的9/10整页和1/2重复入口。
    if(CurrentLogPageResourceProfile().scrollPolicy==LogPageScrollPolicy::SingleStep)
        return GuiDialogBaseVtable().upperInputAt34(p);
    auto& b=*reinterpret_cast<Scroll*>(p);auto* keys=Keys(0x5206C9);
    if(Held(*keys,9)){if(RepeatKey(b,5))return 1;}
    else
    {
        keys=Keys(0x520710);
        if(Held(*keys,10)){if(RepeatKey(b,6))return 1;}
        else
        {
            int id=0;
            if(Pressed(*keys,1))id=3;
            else if(Pressed(*Keys(0x5209E7),2))id=4;
            if(id)
            {
                Fresh f;Find(p,f.slot,id);RefLife life{f.slot};
                if(State(f.slot))
                {auto* c=Resolve<Control>(f.slot);reinterpret_cast<const GuiButtonNativeVtable29*>(Read(c->vptrAt00))->setPressedAt70(c,1);life.TerminateNormal();return 1;}
                life.TerminateNormal();
            }
        }
    }
    if(Read(b.heldEventAt70)==5||Read(b.heldEventAt70)==6)b.heldEventAt70=-1;
    return GuiDialogBaseVtable().upperInputAt34(p);
}
void __fastcall InputScroll(Common* p,void*)
{
    if(CurrentLogPageResourceProfile().scrollPolicy==LogPageScrollPolicy::SingleStep)
    {
        // 与构造保留eventEnabled配套：普通鼠标click由共享GUI触发，不能同时重复派发。
        GuiDialogBaseVtable().processInputAt54(p);return;
    }
    auto& b=*reinterpret_cast<Scroll*>(p);Fresh first;Find(p,first.slot,3);RefLife lifeFirst{first.slot};const auto before3=Style(first.slot);
    Fresh second;Find(p,second.slot,4);RefLife lifeSecond{second.slot};const auto before4=Style(second.slot);
    GuiDialogBaseVtable().processInputAt54(p);const auto now3=Style(first.slot),now4=Style(second.slot);
    RepeatButton(b,first.slot,3,before3,now3);RepeatButton(b,second.slot,4,before4,now4);
    lifeSecond.TerminateNormal();lifeFirst.TerminateNormal(false);
}
int __fastcall PollScroll(Common* p,void*)
{
    auto& b=*reinterpret_cast<Scroll*>(p);Fresh slider;Find(p,slider.slot,2);RefLife life{slider.slot};
    auto* c=Resolve<Control>(slider.slot);const auto before=Slider(c).valueAt60(c);
    c=Resolve<Control>(slider.slot);const auto changed=Slider(c).changedAt6C(c);int state3=0,state4=0;
    if(changed)
    {
        Fresh f;Find(p,f.slot,3);RefLife guard{f.slot};state3=State(f.slot);SetState(f.slot,0);guard.TerminateNormal();
        Fresh g;Find(p,g.slot,4);RefLife other{g.slot};state4=State(g.slot);SetState(g.slot,0);other.TerminateNormal();
    }
    Read(p->vptrAt00)->processInputAt54(p);const auto event=PollEvents(b.eventsAt74);
    const bool repeat=CurrentLogPageResourceProfile().scrollPolicy==LogPageScrollPolicy::PageKeysAndHoldRepeat;
    if(repeat&&event==5)MoveScroll(p,-4);else if(repeat&&event==6)MoveScroll(p,4);
    else if(event==3)MoveScroll(p,-1);else if(event==4)MoveScroll(p,1);
    else{const auto wheel=Read(p->vptrAt00)->wheelAt78(p);if(wheel>0)MoveScroll(p,-1);else if(wheel<0)MoveScroll(p,1);}
    c=Resolve<Control>(slider.slot);
    if(Slider(c).valueAt60(c)!=before)
    {c=Resolve<Control>(slider.slot);auto* table=Read(p->vptrAt00);RECT out;auto* r=Read(c->vptrAt00)->rectAt3C(c,&out);table->invalidateAt64(p,*r);}
    if(changed)
    {
        Fresh f;Find(p,f.slot,3);RefLife guard{f.slot};SetState(f.slot,state3);guard.TerminateNormal();
        Fresh g;Find(p,g.slot,4);RefLife other{g.slot};SetState(g.slot,state4);other.TerminateNormal();
    }
    life.TerminateNormal(false);return 1;
}
}
