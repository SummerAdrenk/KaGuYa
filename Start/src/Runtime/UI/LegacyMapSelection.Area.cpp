#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
namespace
{
using R=LegacyMapSelectionAreaRule;
bool Cancel(Body& b){return b.services->source.EscapePressed()||b.services->source.RightPressed();}
void Move(Body& b,int event,POINT point)
{if(auto* c=Find(b,0,event)){c->body->xAt34=point.x;c->body->yAt38=point.y;}}
int __fastcall MarkerState(GuiControlNativeBody50h* c,void*)
{return !c->enabledAt18?2:!c->stateAt14?1:c->unknownAt24?5:c->hoverAt1C?4:c->focusAt20?3:0;}
const GuiControlNativeVtable23& MarkerMethods()
{
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
    static const auto methods=[]{auto m=GuiStaticVtable();m.resourceStateAt38=reinterpret_cast<decltype(m.resourceStateAt38)>(&MarkerState);return m;}();return methods;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
}
void ConstructArea(Body& b)
{
    const auto& p=b.services->profile;const auto& a=p.area;
    for(const auto& actor:p.actors)
    {
        if(actor.variants.empty()||(actor.route&&Read(b,actor.route)<0))continue;
        const unsigned type=unsigned(Read(b,actor.eventType));
        // 08:505881、10:4F4DBD、11:4F5C46、12:4EE30D均为unsigned ja。
        if(type<actor.variants.size()&&actor.variants[type])AddWidget(b,*actor.variants[type],1);
    }
    for(const auto* name:a.badgeNames)
    {b.badges.emplace_back(b.services->surfaces);if(name)Load(b,b.badges.back(),name);}
    if(a.animation)
    {
        auto sprite=std::make_unique<SpriteRuntime>(b.services->surfaces);
        b.services->source.LoadAnimation(*sprite,a.animation);sprite->SetOrigin(a.animationPosition.x,a.animationPosition.y);
        b.animations.push_back({1,a.animationEvent,false,std::move(sprite)});
    }
    if(a.rule==R::PriorityMarkers)
    {
        std::vector<int> types(a.count,-1);
        for(const auto& actor:p.actors)
        {
            const unsigned location=unsigned(Read(b,actor.location));
            if(location<types.size())
            {const int type=Read(b,actor.eventType);if(types[location]!=0)types[location]=type;}
        }
        for(int i=0;i<a.count;++i)if(types[i]>=0)
        {
            const int kind=types[i]!=0;
            LegacyMapSelectionWidget w{0,{i+3,a.markerStates[kind],&a.markerPositions[2*i+kind]},{},0,0};
            AddWidget(b,w,0,false);
            View(b,0).controls.back().body->vptrAt00=&MarkerMethods();
            // 05:4F6240标记与按钮event可能重复；同步保留控件身份，不用全局Find混查。
            b.synchronizedMarkers.emplace_back(a.firstPlace+i,View(b,0).controls.back().body);
        }
    }
    if(a.rule==R::SpecialEvents)
    {
        for(unsigned i=0;i<p.special.types.size();++i)
        {
            const unsigned value=unsigned(Read(b,p.special.types[i]));
            if(value<=2)AddWidget(b,p.special.typeWidgets[3*i+value],1);
        }
        for(unsigned i=0;i<p.special.flags.size();++i)
            if(Read(b,p.special.flags[i])&2)AddWidget(b,p.special.flagWidgets[i],1);
        for(const auto* variable:p.special.otherLocations)if(Read(b,variable)!=-1)
        {AddWidget(b,p.special.flagWidgets.back(),1);break;}
    }
    for(const auto [event,gauge]:{std::pair{a.dateEvent,false},std::pair{a.gaugeEvent,true}})if(event>=0)
    {
        b.canvases.emplace_back(2,event,b.services->surfaces);auto& surface=b.canvases.back().surface;
        if(gauge)b.services->source.CaptureMasoGauge(surface);else b.services->source.CaptureDate(surface);
        AddCanvas(b,surface,2,event);
    }
}
void BeginArea(Body& b)
{
    // 08:504EA0/10:4F4310按占用更新；11:4F4F20、12:4EDA30另保留堆叠/固定标记。
    const auto& p=b.services->profile;const auto& a=p.area;
    b.content=0;b.area=-1;View(b,0).shown=true;View(b,1).shown=false;View(b,-1).interactive=true;View(b,-1).dimmed=false;
    if(a.hintArea>=0)View(b,3).shown=false;
    if(a.parentOverlay>=0)Enabled(b,-2,a.parentOverlay,true);
    std::vector<int> occupancy(a.count,0);
    for(unsigned i=0;i<p.actors.size();++i)
    {
        const auto location=unsigned(Read(b,p.actors[i].location));const bool valid=location<occupancy.size();
        if(a.rule==R::StackedFive||a.rule==R::StackedThree)
        {
            Visible(b,0,a.firstMarker+int(i),valid);
            if(valid&&!(a.rule==R::StackedThree&&i==3))
            {
                const auto slot=(a.rule==R::StackedFive?5:3)*location+occupancy[location];
                if(slot>=a.markerPositions.size())throw std::out_of_range("Map selection: marker position table");
                Move(b,a.firstMarker+int(i),a.markerPositions[slot]);++occupancy[location];
            }
        }
        else if(valid)++occupancy[location];
    }
    if(a.rule==R::SpecialEvents)
    {
        for(const char* variable:p.special.types)if(Read(b,variable)!=-1)occupancy[6]=1;
        for(const char* variable:p.special.otherLocations)
        {const unsigned location=unsigned(Read(b,variable));if(location<occupancy.size())occupancy[location]=1;}
        occupancy[a.hintArea]=1;
    }
    const bool restrictExploration=a.explorationVariable&&Read(b,a.explorationVariable)==0;
    for(int i=0;i<a.count;++i)
    {
        const bool occupied=occupancy[i]!=0;
        if(a.rule==R::HideEmpty)
        {Visible(b,0,a.firstPlace+i,occupied);Visible(b,0,a.firstMarker+i,occupied);}
        if(a.rule==R::RouteVisibility)Visible(b,0,a.firstMarker+i,occupied);
        if(a.rule==R::StackedFive&&restrictExploration)Enabled(b,0,a.firstPlace+i,occupied);
        if(a.rule==R::SpecialEvents)
        {Enabled(b,0,a.firstPlace+i,occupied);if(i<8)Visible(b,0,a.firstMarker+i,occupied);}
    }
    Reset(b,0);Apply(b);Dirty(b);
}
void SelectArea(Body& b,int area)
{
    // 05:4F6DA0、07:4FB9C0与08..15角色Begin分别由明确策略选择，不按类名合并。
    const auto& p=b.services->profile;const auto& a=p.area;
    if(area<0||area>=a.count)throw std::out_of_range("Map selection: region");
    b.area=area;View(b,0).shown=false;View(b,-1).interactive=false;View(b,-1).dimmed=true;
    if(a.parentOverlay>=0)Enabled(b,-2,a.parentOverlay,false);
    if(area==a.hintArea){b.content=3;View(b,3).shown=true;Reset(b,3);Apply(b);Dirty(b);return;}
    b.content=1;View(b,1).shown=true;
    if(a.firstBackground>=0)
    {
        const bool single=a.rule==R::HideEmpty||a.rule==R::StackedThree;
        for(int i=0;i<(single?1:(a.rule==R::SpecialEvents?8:a.count));++i)
            Visible(b,1,a.firstBackground+i,single||i==area);
    }
    for(const auto& actor:p.actors)
    {
        const bool route=!actor.route||Read(b,actor.route)>=0;
        const bool here=Read(b,actor.location)==area;
        Enabled(b,1,actor.event,route&&here);
        if(a.rule==R::RouteVisibility||a.rule==R::HideEmpty)Visible(b,1,actor.event,route);
        if(actor.label>=0)Visible(b,1,actor.label,here);
    }
    if(a.freeEvent>=0)
    {
        bool enabled=true;
        if(a.rule==R::PriorityMarkers||a.rule==R::TimeMarkers)
            enabled=!Find(b,1,p.actors.back().event)->enabled;
        if(a.rule==R::StackedFive)enabled=Read(b,a.explorationVariable)!=0;
        if(a.rule==R::StackedThree)enabled=area!=0&&area!=4&&area!=7&&area!=10;
        Enabled(b,1,a.freeEvent,enabled);
    }
    if(a.rule==R::RouteVisibility)Visible(b,1,47,Read(b,a.extraAreaVariables[area])!=0);
    if(a.rule==R::SpecialEvents)
    {
        // 15:4EA7E0的area6与普通区用两组可见ID；bond先命中，TrueRoute最后处理。
        const bool special=area==6;
        for(unsigned i=0;i<p.special.visibilitySets.size();++i)
            if((i<11)!=special)Visible(b,1,p.special.visibilitySets[i],false);
        if(special)
        {
            for(int i=0;i<4;++i)
            {
                const bool available=Read(b,p.special.types[i])!=-1;
                Visible(b,1,22+i,true);Enabled(b,1,22+i,available);Visible(b,1,45+i,available);
            }
            const bool available=Read(b,p.special.types[3])!=-1;
            for(int event:{49,50,51}){Visible(b,1,event,available);Enabled(b,1,event,available);}
        }
        else
        {
            for(const auto& actor:p.actors)Visible(b,1,actor.event,true);
            for(int event:{32,33,34,35})Visible(b,1,event,false);
            bool found=false;
            for(int i=0;i<3;++i)if(Read(b,p.special.otherLocations[i])==area)
            {Visible(b,1,32+i,true);found=true;break;}
            const bool here=Read(b,p.special.otherLocations[3])==area;
            Visible(b,1,35,!found);Enabled(b,1,35,!found&&here);Visible(b,1,52,found||here);
        }
    }
    for(auto& animation:b.animations)if(animation.view==1&&Find(b,1,animation.event)->enabled)animation.sprite->RestartAnimation(true);
    Reset(b,1);Apply(b);Dirty(b);
}
void PollArea(Body& b)
{
    const auto& p=b.services->profile;const auto& a=p.area;
    if(b.content==0)
    {
        const int event=Input(b,0);
        for(auto [peer,marker]:b.synchronizedMarkers)
        {
            const auto* c=Find(b,0,peer);const int state=c->body->vptrAt00->resourceStateAt38(c->body);
            marker->focusAt20=state==3;marker->hoverAt1C=state==4;marker->unknownAt24=state==5;
        }
        if(event>=a.firstPlace&&event<a.firstPlace+a.count)
        {if(a.placeCue.name)b.services->source.PlayCue(a.placeCue);SelectArea(b,event-a.firstPlace);}
        return;
    }
    const int event=Input(b,b.content);
    if(Cancel(b)||event==(b.content==3?a.hintCancel:a.cancelEvent))
    {View(b,b.content).shown=false;BeginArea(b);return;}
    if(b.content==3)return;
    if(event==a.freeEvent&&event>=0)
    {if(a.actorCue.name)b.services->source.PlayCue(a.actorCue);Close(b,b.area+a.freeResultBase);return;}
    for(const auto& r:p.results)if(r.event==event)
    {if(a.actorCue.name)b.services->source.PlayCue(a.actorCue);Close(b,r.value);return;}
}
void RenderArea(Body& b,int view,const RECT* clip,const Slot* target)
{
    if(view!=1||b.services->profile.kind!=LegacyMapSelectionKind::AreaActors)return;
    const auto& p=b.services->profile;
    for(const auto& badge:p.area.badges)
    {
        auto* c=Find(b,1,badge.event);
        // 05:4F7290/07:4FBE80检查readState(+48)，不是鼠标hover。
        if(!c||!c->body->vptrAt00->readStateAt48(c->body))continue;
        const int value=Read(b,p.actors[badge.actor].eventType);if(value<0)continue;
        if(unsigned(value)>=b.badges.size())
        {if(p.area.rule==R::PriorityMarkers)throw std::out_of_range("Map selection: badge table");continue;}
        const auto& image=b.badges[value];if(!image)continue;
        if(target)
        {
            auto* dst=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*target);
            static_cast<void>(b.services->drawing.SurfaceBlitRaw(dst->handleAt04,image->handleAt04,badge.position.x,badge.position.y,nullptr,nullptr,clip,0));
        }
        else static_cast<void>(b.services->drawing.SurfaceDrawRaw(image->handleAt04,badge.position.x,badge.position.y,nullptr,nullptr,clip,0));
    }
}
}
