#include "LegacyCharacterSelection.Internal.h"
#include <algorithm>

namespace KaGuYa::Start::Runtime::LegacyCharacterSelectionDetail
{
void Navigation(Body& b)
{
    ClearGuiNavigationReferencesAt4D80B0(b.common.navigationAt24);
    const auto& p=b.services->profile;
    const int view=p.kind==LegacyCharacterSelectionKind::TwoPage?(b.selected>=0?-2:b.page):0;
    for(const auto& n:p.navigation)if(n.view==view||n.view==-1)
        CreateAndAppendGuiNavigation(b.common.navigationAt24,n.input,n.pairs.data());
}
void Hotkeys(Body& b)
{
    if(b.selected>=0)return;
    for(const auto& k:b.services->profile.hotkeys)
        if(k.view==b.page&&b.common.vptrAt00->pressedAt70(&b.common,k.input))
            if(auto* c=Find(b,k.event);c&&c->visible&&c->enabled)
                b.common.vptrAt00->setActiveAt30(&b.common,c->body);
}
void RefreshPages(Body& b)
{
    const auto& p=b.services->profile;bool out=false,back=false;
    for(unsigned i=0;i<p.animations.size();++i)
    {
        const auto& a=p.animations[i];const int location=Read(b,a.variable);
        Visible(b,a.button.common.eventAt00,location>=0&&location<13);
        if(location>=13)out=true;
        if(location>=0&&location<13)
        {
            const auto at=p.animationPositions[i+5*location];
            auto* c=Find(b,a.button.common.eventAt00);c->body->xAt34=at.x;c->body->yAt38=at.y;
            b.animations[i]->SetOrigin(at.x,at.y);b.animations[i]->RestartAnimation(true);
        }
    }
    Visible(b,12,out);
    for(const auto& r:p.hoverRows)
    {const auto location=Read(b,r.variable);Enabled(b,r.event,location>=13);if(location>=0&&location<13)back=true;Visible(b,r.overlay,false);}
    Visible(b,13,back);
    for(const auto& e:p.effects){Visible(b,e.icon,false);Visible(b,e.background,false);}
}
void Hover(Body& b)
{
    const auto& p=b.services->profile;
    if(p.kind==LegacyCharacterSelectionKind::TwoPage)
    {
        if(b.page==0)
        {
            for(unsigned i=0;i<p.animations.size();++i)
            {auto* c=Find(b,p.animations[i].button.common.eventAt00);if(c&&c->visible){(void)b.animations[i]->AdvanceAnimation();if(c->body->vptrAt00->resourceStateAt38(c->body)==0)Dirty(b);}}
        }
        else
        {
            std::vector<int> values;
            for(const auto& r:p.hoverRows)
            {auto* c=Find(b,r.event);const auto state=c?c->body->vptrAt00->resourceStateAt38(c->body):0;const int value=state>=3&&state<=5?Read(b,r.variable):-1;Visible(b,r.overlay,value!=-1);values.push_back(value);}
            for(const auto& e:p.effects){const bool show=std::find(values.begin(),values.end(),e.value)!=values.end();Visible(b,e.icon,show);Visible(b,e.background,show);}
        }
    }
    if(p.kind==LegacyCharacterSelectionKind::ResourcePair)
    {
        int picture=0;
        for(int event:{6,7})if(auto* c=Find(b,event))
        {const int state=c->body->vptrAt00->resourceStateAt38(c->body);if(state>=3&&state<=4)picture=1;else if(state==5)picture=2;}
        Visible(b,3,picture==1);Visible(b,4,picture==2);
    }
    Apply(b);
}
void Transition(Body& b)
{
    auto& s=*b.services;auto& source=s.source;
    if(b.transitionStage==0)
    {
        b.operation={};b.operation.opcode=0;
        if(b.phase==LegacyCharacterSelectionPhase::Opening)
        {
            b.common.vptrAt00->compositeAt5C(&b.common,Copy(b.image.Slot()),nullptr);b.operation.source=b.image.Get();
            unsigned time=s.profile.bgm.size()>1?unsigned(Read(b,"time")):0;
            if(time>=s.profile.bgm.size())time=0;
            source.PlayBgmAndStopEffects(s.profile.bgm[time]);
        }
        else b.operation.source=b.black.Get();
        source.UnSave().ModeAt04()=0;b.transitionStage=1;return;
    }
    if(b.transitionStage==1)
    {
        if(b.operation.current>=b.operation.target)b.transitionStage=2;
        if(source.AccelerateTransition())b.operation.current=b.operation.target;
        static_cast<void>(source.Transition().RenderAt449DD0(b.operation,s.transitionGraphics));
        constexpr int steps[]{4,8,16,32};const auto speed=source.SaveData().valueAt70;
        b.operation.current=(std::min)(b.operation.target,Add(b.operation.current,steps[speed<4?speed:0]));
        source.Present();return;
    }
    b.phase=b.phase==LegacyCharacterSelectionPhase::Opening?LegacyCharacterSelectionPhase::Ready:LegacyCharacterSelectionPhase::Finished;
    source.UnSave().ModeAt04()=1;
}
void Begin(Gui* gui,void*)
{
    auto& b=*reinterpret_cast<Body*>(gui);const auto& p=b.services->profile;
    gui->vptrAt00->writeEnabledAt24(gui,1);gui->vptrAt00->resetInputAt3C(gui);
    ClearGuiClickedEventFlagsAt532200(b.events);b.selected=-1;b.phase=LegacyCharacterSelectionPhase::Opening;b.transitionStage=0;
    for(auto& c:b.controls){c.visible=c.enabled=true;c.body->vptrAt00->resetAt08(c.body);}
    for(const auto& r:p.disableRules)Enabled(b,r.event,Read(b,r.variable)<r.threshold);
    for(int event:p.disabledEvents)Enabled(b,event,false);
    Enabled(b,p.skipEvent,false);
    for(int event:{p.autoEvent,p.skipEvent})if(auto* c=Find(b,event))
        reinterpret_cast<GuiCheckBoxNativeBody60h*>(c->body)->checkedAt5C=((event==p.autoEvent?b.services->source.UnSave().FlagsAt18():b.services->source.UnSave().FlagsAt14())&2)!=0;
    if(p.kind==LegacyCharacterSelectionKind::TwoPage)
    {
        RefreshPages(b);b.page=Read(b,"selected_page");
        if(b.page!=0&&b.page!=1)b.page=(!Find(b,12)->visible||Find(b,13)->visible)?0:1;
    }
    if(p.kind==LegacyCharacterSelectionKind::TabbedSix){RefreshTabPictures(b);RefreshTabs(b,0);}
    if(p.kind==LegacyCharacterSelectionKind::ResourcePair){Visible(b,3,false);Visible(b,4,false);}
    Apply(b);Navigation(b);Dirty(b);
}
int Poll(Gui* gui,void*)
{
    auto& b=*reinterpret_cast<Body*>(gui);auto& s=*b.services;const auto& p=s.profile;
    if(b.phase==LegacyCharacterSelectionPhase::Opening||b.phase==LegacyCharacterSelectionPhase::Closing){Transition(b);return 1;}
    if(b.phase==LegacyCharacterSelectionPhase::Finished)return 0;
    if(b.phase==LegacyCharacterSelectionPhase::Confirming)return 1;
    if(b.phase==LegacyCharacterSelectionPhase::Hold)
    {if(b.hold>0)--b.hold;else{b.phase=LegacyCharacterSelectionPhase::Closing;b.transitionStage=0;}return 1;}
    Hotkeys(b);
    if(p.kind==LegacyCharacterSelectionKind::TwoPage&&b.selected>=0)
    {
        // 02只让确认页收输入；底层页面继续按原状态绘制，不改成整页灰色。
        struct InputMask final
        {
            std::vector<std::pair<GuiControlNativeBody50h*,int>> saved;
            explicit InputMask(Body& b)
            {saved.reserve(b.controls.size());for(auto& c:b.controls)if(c.view!=-2){saved.emplace_back(c.body,c.body->enabledAt18);c.body->enabledAt18=0;}}
            ~InputMask(){for(auto [control,enabled]:saved)control->enabledAt18=enabled;}
        };
        {InputMask mask(b);ProcessGuiDialogInputAt518C30(*gui);}
        const int event=PollGuiClickedEventManagerNativeAt50FDB0(&b.events,1);
        if(event==33){b.phase=LegacyCharacterSelectionPhase::Closing;b.transitionStage=0;}
        else if(event==34||s.source.CancelPressed())
        {b.selected=-1;gui->vptrAt00->resetInputAt3C(gui);Apply(b);Navigation(b);Dirty(b);}
        return 1;
    }
    ProcessGuiDialogInputAt518C30(*gui);
    Hover(b);
    const int event=PollGuiClickedEventManagerNativeAt50FDB0(&b.events,1);
    auto* control=Find(b,event);
    if(event<0||!control||!control->body->vptrAt00->enabledAt1C(control->body))return 1;
    if(event==p.autoEvent)
    {auto& flags=s.source.UnSave().FlagsAt18();flags^=2u;reinterpret_cast<GuiCheckBoxNativeBody60h*>(control->body)->checkedAt5C=(flags&2)!=0;Dirty(b);return 1;}
    for(const auto& command:p.commands)if(event==command.event)
    {control->body->vptrAt00->resetAt08(control->body);s.source.SetCommand(command.command);return 1;}
    if(p.kind==LegacyCharacterSelectionKind::TwoPage&&(event==27||event==32))
    {
        // 02:4F8050/4F8120只启用并重置目标页，不重跑Begin或重启动画。
        b.page=event==27?1:0;Apply(b);Navigation(b);gui->vptrAt00->resetInputAt3C(gui);Dirty(b);
        Write(b,"selected_page",b.page);return 1;
    }
    if(p.kind==LegacyCharacterSelectionKind::TabbedSix&&event>=7&&event<=12)
    {RefreshTabs(b,event-7);Apply(b);Dirty(b);return 1;}
    for(const auto& choice:p.choices)
    {
        if(choice.event!=event||(choice.view>=0&&choice.view!=(p.kind==LegacyCharacterSelectionKind::TwoPage?b.page:b.mode)))continue;
        b.selected=choice.result;
        if(p.kind==LegacyCharacterSelectionKind::TwoPage)
        {for(int id=7;id<=11;++id)Visible(b,id,id-7==b.selected);Apply(b);Navigation(b);gui->vptrAt00->resetInputAt3C(gui);Dirty(b);}
        else if(p.kind==LegacyCharacterSelectionKind::ConfirmedPair||p.kind==LegacyCharacterSelectionKind::ResourcePair)
        {
            if(p.kind==LegacyCharacterSelectionKind::ConfirmedPair)Enabled(b,event,false);
            if(p.kind==LegacyCharacterSelectionKind::ResourcePair)
            {Enabled(b,event==6?7:6,false);control->body->vptrAt00->resetAt08(control->body);s.source.PlaySelectionCue(p.selectionCue);}
            Apply(b);Dirty(b);b.phase=LegacyCharacterSelectionPhase::Confirming;
        }
        else{b.phase=LegacyCharacterSelectionPhase::Closing;b.transitionStage=0;}
        break;
    }
    return 1;
}
int End(Gui* gui,void*)
{auto& b=*reinterpret_cast<Body*>(gui);gui->vptrAt00->writeEnabledAt24(gui,0);Write(b,b.services->profile.resultVariable,b.selected);return 1;}
}
namespace KaGuYa::Start::Runtime
{
void ResolveLegacyCharacterSelectionConfirmation(LegacyCharacterSelectionDialog& b,int result)
{
    using namespace LegacyCharacterSelectionDetail;
    if(b.phase!=LegacyCharacterSelectionPhase::Confirming)throw std::logic_error("Character selection: no pending confirmation");
    const auto& p=b.services->profile;
    if(result==6)
    {
        if(p.kind==LegacyCharacterSelectionKind::ResourcePair)
        {
            b.hold=30;b.phase=LegacyCharacterSelectionPhase::Hold;
            const int cost=p.costs[6*b.mode+2*b.option+b.selected];
            Write(b,p.costVariable,Sub(Read(b,p.costVariable),cost));
            b.services->source.CaptureMasoGauge(b.gauge);Dirty(b);
        }
        else {b.phase=LegacyCharacterSelectionPhase::Closing;b.transitionStage=0;}
    }
    else if(result==7||result==0)
    {
        b.selected=-1;b.phase=LegacyCharacterSelectionPhase::Ready;
        for(const auto& c:p.choices){Enabled(b,c.event,true);if(auto* control=Find(b,c.event))control->body->vptrAt00->resetAt08(control->body);}
        b.common.vptrAt00->resetInputAt3C(&b.common);Apply(b);Dirty(b);
    }
    else throw std::runtime_error("Character selection: unexpected confirmation result");
}
}
