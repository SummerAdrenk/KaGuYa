#include "SettingPage04Native.Internal.h"
#include <string>

namespace KaGuYa::Start::Runtime::SettingPage04NativeDetail
{
namespace
{
int Choice(Common* p,int id)
{auto* radio=FindRadio(p,id);return radio?ReadRadio(radio):-1;}
void InvalidateControl(Common* p,Control* c)
{RECT out;auto* r=ControlTable(c).rectAt3C(c,&out);Read(p->vptrAt00)->invalidateAt64(p,*r);}
bool ToggleCheck(Common* p,Control* c)
{
    if(!c||!ControlTable(c).readEnabledAt50(c)||!ControlTable(c).readStateAt48(c))return false;
    Field<int>(c,0x5C)=Read(Field<int>(c,0x5C))==0;
    InvalidateControl(p,c);return true;
}
RECT NameRectangle(Common* p)
{
    auto* surface=Resolve<Surface>(Field<Slot>(p,0x7C));
    if(!surface)return {};
    const auto anchor=CurrentSettingOtherProfile().name.center;
    const auto width=Read(surface->widthAt08),height=Read(surface->heightAt0C);
    const int x=anchor.x-(width>>1),y=anchor.y-(height>>1);
    return {x,y,x+width,y+height};
}
void Gate(Common* p,const SettingOtherBlockProfile& profile,int blocked,int redraw)
{
    for(int id:profile.gateRadios)if(auto* radio=FindRadio(p,id))
    {
        Read(radio->vptrAt00)->resetChildrenAt68(radio);
        Read(radio->vptrAt00)->writeEnabledAt24(radio,blocked==0);
        if(redraw)Read(p->vptrAt00)->invalidateAt64(p,Field<RECT>(p,0x6C));
    }
    for(int id:profile.gateControls)if(auto* c=FindControl(p,id))
    {
        ControlTable(c).resetAt08(c);ControlTable(c).writeStateAt44(c,blocked==0);
        if(redraw)InvalidateControl(p,c);
    }
}
}
void __fastcall CheckboxBegin(Common* p,void*)
{
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p,0x74)));
    Read(p->vptrAt00)->resetChildrenAt68(p);
}
int __fastcall CheckboxPoll(Common* p,void*)
{
    Read(p->vptrAt00)->processInputAt54(p);
    const auto id=PollManager(Field<Slot>(p,0x74));
    if(id<0)return 1;
    auto* parent=Read(p->parentAt04);
    if(ToggleCheck(parent,FindControl(parent,id)))
        for(const auto& b:ProfileFor(parent).booleans)
            if(b.controlOrRadioId==id&&b.commit==SettingOtherCommit::Immediate)
                WriteBool(b.field,BoolSelection(parent,b));
    return 1;
}
void __fastcall Begin01(Common* p,void*)
{
    GuiDialogIntoBaseVtable().common.beginAt44(p);
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p,0x88)));
    Read(p->vptrAt00)->writeEnabledAt24(p,1);Read(p->vptrAt00)->resetChildrenAt68(p);CTable(p).loadAt98(p);
    ApplyNameDisabled(p,CurrentSettingOtherProfile().name.disabledControlsAtBegin);
    BuildNameTextAt512200(p,1,0);
}
int __fastcall Poll01(Common* p,void*)
{
    Field<int>(Read(p->parentAt04),0x84)=-1;
    GuiDialogIntoBaseVtable().common.pollAt48(p);Read(p->vptrAt00)->processInputAt54(p);
    const auto& name=CurrentSettingOtherProfile().name;
    if(PollManager(Field<Slot>(p,0x88))!=name.buttonEvent)return 1;
    if(auto* button=FindControl(p,name.buttonEvent))ControlTable(button).resetAt08(button);
    switch(name.entry)
    {
    case SettingOtherNameEntry::ParentResult:
        Field<int>(Read(p->parentAt04),0x84)=name.parentResult;break;
    case SettingOtherNameEntry::LocalResource112:
    {
        const auto result=RequireSettingPage04NativeServices().source.RunLocalNamePrompt();
        switch(result.status)
        {
        case GameResourceNamePromptStatus::Cancelled:return 1;
        case GameResourceNamePromptStatus::Error:
            if(result.exception)std::rethrow_exception(result.exception);
            throw std::runtime_error("Page04 local name prompt failed; Win32 error "+std::to_string(result.win32Error));
        case GameResourceNamePromptStatus::Accepted:
            // 本地资源框已提交姓名和skip；此处仅刷新，不能再次写入状态。
            break;
        }
        // 02-10只刷新姓名，不重载原有checkbox/radio选择；09/10的End仍按原选择提交。
        RECT dirty=NameRectangle(p);BuildNameTextAt512200(p,1,0);
        const RECT current=NameRectangle(p);::UnionRect(&dirty,&dirty,&current);
        Read(p->vptrAt00)->invalidateAt64(p,dirty);break;
    }
    case SettingOtherNameEntry::None:throw std::logic_error("Other name action has no entry");
    }
    return 1;
}
void __fastcall BeginManaged(Common* p,void*)
{
    GuiDialogIntoBaseVtable().common.beginAt44(p);
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p,0x7C)));
    Read(p->vptrAt00)->writeEnabledAt24(p,1);Read(p->vptrAt00)->resetChildrenAt68(p);CTable(p).loadAt98(p);
}
int __fastcall Poll02(Common* p,void*)
{
    const auto& binding=ProfileFor(p).booleans.front();
    const bool old=BoolSelection(p,binding);
    GuiDialogIntoBaseVtable().common.pollAt48(p);Read(p->vptrAt00)->processInputAt54(p);
    const bool now=BoolSelection(p,binding);
    if(now!=old)MTable(p).gateAtA0(p,!now,1);
    return 1;
}
int __fastcall Poll05(Common* p,void*)
{
    const auto& block=ProfileFor(p);
    const auto& position=block.words.front();
    const auto old=Choice(p,position.radioId);
    GuiDialogIntoBaseVtable().common.pollAt48(p);Read(p->vptrAt00)->processInputAt54(p);
    const auto id=PollManager(Field<Slot>(p,0x7C));
    bool checkEvent=false;
    for(const auto& check:block.checks)if(check.eventAt00==id)
    {ToggleCheck(p,FindControl(p,id));checkEvent=true;break;}
    if(!checkEvent)
    {
        const auto now=Choice(p,position.radioId);
        if(now!=old)MTable(p).gateAtA0(p,now==position.otherReadEvent,1);
    }
    return 1;
}
void __fastcall Gate02(Common* p,void*,int blocked,int redraw)
{
    const auto& block=ProfileFor(p);
    // Old effect gate also sets the inverted check, without committing the setting.
    if(block.radios.empty())for(const auto& check:block.checks)if(auto* c=FindControl(p,check.eventAt00))
    {
        ControlTable(c).resetAt08(c);Field<int>(c,0x5C)=blocked!=0;
        if(redraw)InvalidateControl(p,c);
    }
    Gate(p,block,blocked,redraw);
}
void __fastcall Gate05(Common* p,void*,int blocked,int redraw)
{Gate(p,ProfileFor(p),blocked,redraw);}
}
