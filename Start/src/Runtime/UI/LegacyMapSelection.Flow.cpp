#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
void Navigation(Body& b,Panel& p)
{
    ClearGuiNavigationReferencesAt4D80B0(p.common.navigationAt24);
    for(const auto& n:b.services->profile.navigation)if(n.view==p.id)
        CreateAndAppendGuiNavigation(p.common.navigationAt24,n.input,n.pairs.data());
}
int Input(Body& b,int view)
{
    auto& p=View(b,view);if(!p.shown||!p.interactive)return -1;
    for(const auto& k:b.services->profile.hotkeys)
        if(k.view==view&&p.common.vptrAt00->pressedAt70(&p.common,k.input))
            if(auto* c=Find(b,view,k.event);c&&c->visible&&c->enabled)
                p.common.vptrAt00->setActiveAt30(&p.common,c->body);
    ProcessGuiDialogInputAt518C30(p.common);
    const int event=PollGuiClickedEventManagerNativeAt50FDB0(&p.events,1);
    auto* c=Find(b,view,event);
    return c&&c->visible&&c->enabled?event:-1;
}
void Close(Body& b,int result,int secondary)
{b.selected=result;b.secondary=secondary;b.phase=LegacyMapSelectionPhase::Closing;b.transitionStage=0;}
void Begin(Gui* gui)
{
    auto& b=*reinterpret_cast<Body*>(gui);const auto& p=b.services->profile;
    gui->vptrAt00->writeEnabledAt24(gui,1);
    b.phase=LegacyMapSelectionPhase::Opening;b.transitionStage=0;b.selected=b.secondary=-1;
    for(auto& v:b.panels)
    {
        v->shown=v->interactive=true;v->dimmed=false;
        for(auto& c:v->controls){c.visible=c.enabled=true;c.body->vptrAt00->resetAt08(c.body);}
        Reset(b,v->id);
    }
    for(int id:p.system.disabled)Enabled(b,-1,id,false);
    Enabled(b,-1,p.system.skipEvent,false);
    for(int id:{p.system.autoEvent,p.system.skipEvent})if(auto* c=Find(b,-1,id))
        reinterpret_cast<GuiCheckBoxNativeBody60h*>(c->body)->checkedAt5C=
            ((id==p.system.autoEvent?b.services->source.UnSave().FlagsAt18():b.services->source.UnSave().FlagsAt14())&2)!=0;
    switch(p.kind)
    {
    case LegacyMapSelectionKind::AnimatedActors:BeginAnimated(b);break;
    case LegacyMapSelectionKind::AreaActors:BeginArea(b);break;
    case LegacyMapSelectionKind::ProgressBook:BeginBook(b);break;
    case LegacyMapSelectionKind::CharacterDecision:SelectDecisionActor(b,0);break;
    default:throw std::logic_error("Map selection: no Begin contract");
    }
    Apply(b);Dirty(b);
}
int Poll(Gui* gui)
{
    auto& b=*reinterpret_cast<Body*>(gui);auto& s=*b.services;const auto& p=s.profile;
    if(b.phase==LegacyMapSelectionPhase::Opening||b.phase==LegacyMapSelectionPhase::Closing){Transition(b);return 1;}
    if(b.phase==LegacyMapSelectionPhase::Finished)return 0;
    if(p.kind!=LegacyMapSelectionKind::ProgressBook)
    {
        const int event=Input(b,-1);
        if(event>=0)
        {
            if(event==p.system.autoEvent)
            {
                auto& flags=s.source.UnSave().FlagsAt18();flags^=2u;
                reinterpret_cast<GuiCheckBoxNativeBody60h*>(Find(b,-1,event)->body)->checkedAt5C=(flags&2)!=0;
            }
            for(const auto& c:p.system.commands)if(event==c.event)
            {auto* control=Find(b,-1,event)->body;control->vptrAt00->resetAt08(control);s.source.SetCommand(c.name);break;}
        }
    }
    switch(p.kind)
    {
    case LegacyMapSelectionKind::AnimatedActors:PollAnimated(b);break;
    case LegacyMapSelectionKind::AreaActors:PollArea(b);break;
    case LegacyMapSelectionKind::ProgressBook:PollBook(b);break;
    case LegacyMapSelectionKind::CharacterDecision:PollDecision(b);break;
    default:throw std::logic_error("Map selection: no Poll contract");
    }
    AdvanceAnimations(b);Apply(b);Dirty(b);
    // 十款原Scene.Draw为空；05:4D2F30等由GUI Ready分支刷新画面。
    gui->vptrAt00->flushDirtyAt58(gui);
    return 1;
}
int End(Gui* gui)
{
    auto& b=*reinterpret_cast<Body*>(gui);gui->vptrAt00->writeEnabledAt24(gui,0);
    Write(b,b.services->profile.resultVariable,b.selected);
    // 09:4F5C40先提交主结果；第二写失败不得撤销已经写入的prog_result。
    if(b.services->profile.secondResultVariable)Write(b,b.services->profile.secondResultVariable,b.secondary);
    return 1;
}
void ConstructAnimated(Body& b)
{
    for(const auto& a:b.services->profile.animatedActors)
    {
        const auto form=unsigned(Read(b,a.form));
        if(form>=a.variants.size()||!a.variants[form].button)
            throw std::out_of_range("Map selection: absent animated form");
        const auto& v=a.variants[form];AddWidget(b,*v.button,0);
        auto sprite=std::make_unique<SpriteRuntime>(b.services->surfaces);
        b.services->source.LoadAnimation(*sprite,v.animation);
        b.animations.push_back({0,v.button->common.eventAt00,true,std::move(sprite)});
    }
}
void BeginAnimated(Body& b)
{
    const auto& p=b.services->profile;
    for(unsigned i=0;i<p.animatedActors.size();++i)
    {
        const int location=Read(b,p.animatedActors[i].location);auto& animation=b.animations[i];
        const bool valid=unsigned(location)<13;Visible(b,0,animation.event,valid);
        if(valid)
        {
            const auto point=p.animationPositions[location*p.animatedActors.size()+i];
            auto* c=Find(b,0,animation.event)->body;c->xAt34=point.x;c->yAt38=point.y;
            animation.sprite->SetOrigin(point.x,point.y);animation.sprite->RestartAnimation(true);
        }
    }
}
void PollAnimated(Body& b)
{const int event=Input(b,0);for(const auto& r:b.services->profile.results)if(r.event==event){Close(b,r.value);return;}}
}
namespace KaGuYa::Start::Runtime
{
void ResumeLegacyMapSelectionSettings(LegacyMapSelectionDialog& b)
{
    using namespace LegacyMapSelectionDetail;
    if(auto* c=Find(b,-1,b.services->profile.system.skipEvent))
        reinterpret_cast<GuiCheckBoxNativeBody60h*>(c->body)->checkedAt5C=(b.services->source.UnSave().FlagsAt14()&2)!=0;
    for(auto& v:b.panels)Reset(b,v->id);Dirty(b);
}
}
