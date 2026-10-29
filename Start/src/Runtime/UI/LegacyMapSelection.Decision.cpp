#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
namespace
{
int __fastcall PopupKeyboard(GuiControlNativeBody50h* c,void*)
{
    if(c->childIdAt0C<0||!c->vptrAt00->enabledAt1C(c))return 0;
    auto& latch=GuiControlNativeEntries::ButtonField<0x58>(*c);
    auto* parent=static_cast<Gui*>(c->parentAt4C);
    if(latch)
    {
        if(parent->vptrAt00->movedAt7C(parent)||parent->vptrAt00->pressedAt70(parent,c->childIdAt0C))
        {latch=0;c->hoverAt1C=0;return 0;}
    }
    else
    {
        if(!parent->vptrAt00->pressedAt70(parent,c->childIdAt0C))return 0;
        latch=1;c->hoverAt1C=1;
    }
    return 2;
}
int __fastcall PopupMouse(GuiControlNativeBody50h* c,void*)
{
    if(!c->vptrAt00->enabledAt1C(c))return 0;
    auto* parent=static_cast<Gui*>(c->parentAt4C);POINT point{};c->hoverAt1C=0;
    parent->vptrAt00->pointerAt74(parent,&point);
    if(c->vptrAt00->hitAt40(c,point.x,point.y))
    {c->hoverAt1C=1;if(!c->focusAt20)parent->vptrAt00->setActiveAt30(parent,c);}
    return 0;
}
const GuiButtonNativeVtable29& PopupMethods()
{
    static const auto methods=[]
    {
        auto t=GuiButtonVtable();
        t.common.keyboardAt14=reinterpret_cast<decltype(t.common.keyboardAt14)>(&PopupKeyboard);
        t.common.mouseAt18=reinterpret_cast<decltype(t.common.mouseAt18)>(&PopupMouse);return t;
    }();return methods;
}
void BuildPopup(Body& b,const LegacyMapSelectionDecisionActor& actor,GraphicsSurfaceReference& symbol)
{
    const auto& p=b.services->profile.decision;
    LegacyMapSelectionWidget w{1,{actor.popup,actor.popupStates,nullptr},{},6,1};
    AddWidget(b,w,0,false);auto* c=Find(b,0,actor.popup)->body;ConfigurePopup(*c);
    auto& group=RequireGuiControlResource(c->resourcesAt28.begin[0]);
    const int slot=group.slotIndicesAt00[4];
    if(slot<0)return;
    GraphicsSurfaceReference image(group.surfacesAt1C.begin[slot],b.services->surfaces);
    for(const auto [variable,points]:{std::pair{actor.firstCounter,p.firstPoints},std::pair{actor.secondCounter,p.secondPoints}})
    {
        if(!variable)continue;
        const int count=Read(b,variable);
        for(int i=0;i<count&&unsigned(i)<points.size();++i)Blit(b,image,symbol,points[i],false);
    }
    image->originXAt10=p.origin.x;image->originYAt14=p.origin.y;
    if(!actor.extraImage)return;
    GraphicsSurfaceReference extra(b.services->surfaces);Load(b,extra,actor.extraImage);
    RECT a{image->originXAt10,image->originYAt14,Add(image->originXAt10,image->widthAt08),Add(image->originYAt14,image->heightAt0C)};
    RECT e{extra->originXAt10,extra->originYAt14,Add(extra->originXAt10,extra->widthAt08),Add(extra->originYAt14,extra->heightAt0C)},combined{};
    ::UnionRect(&combined,&a,&e);
    GraphicsSurfaceReference merged(b.services->surfaces);merged.ConstructAt40C2E0();
    if(!merged->CreateAt415010(b.services->surfaces,combined.right-combined.left,combined.bottom-combined.top,1,1))throw 1;
    merged->originXAt10=combined.left;merged->originYAt14=combined.top;
    Blit(b,merged,image,{a.left-combined.left,a.top-combined.top},false);
    Blit(b,merged,extra,{e.left-combined.left,e.top-combined.top},false);
    AppendGuiControlSurfaceArgumentAt538350(group,4,Copy(merged.Slot()));
}
}
void ConfigurePopup(GuiControlNativeBody50h& body){body.vptrAt00=&PopupMethods().common;}
void ConstructDecision(Body& b)
{
    const auto& d=b.services->profile.decision;
    GraphicsSurfaceReference symbol(b.services->surfaces);Load(b,symbol,d.counterSymbol);
    for(const auto& actor:d.actors)
    {
        if(actor.popup>=0)BuildPopup(b,actor,symbol);
        const int stage=Read(b,actor.route);if(stage<0)continue;
        if(unsigned(stage)>=actor.stages.size())throw std::out_of_range("Map selection: route stage outside proven data");
        b.canvases.emplace_back(2,actor.decision,b.services->surfaces);auto& image=b.canvases.back().surface;
        Load(b,image,actor.stages[stage].name);AddCanvas(b,image,2,actor.decision);
    }
}
void SelectDecisionActor(Body& b,int selected)
{
    const auto& d=b.services->profile.decision;
    if(unsigned(selected)>=d.actors.size())throw std::out_of_range("Map selection: decision actor");
    b.page=selected;
    for(unsigned i=0;i<d.actors.size();++i)
    {
        const auto& actor=d.actors[i];const bool active=int(i)==selected;
        Visible(b,0,actor.background,active);Visible(b,0,actor.popup,active);
        if(auto* c=Find(b,0,actor.popup))c->body->vptrAt00->resetAt08(c->body);
        // 09:4F8220切换进度图用state，原静态图的disabled资源为空。
        Enabled(b,2,actor.decision,active);
        Visible(b,1,15+int(i),i!=4||Read(b,actor.route)>=0);Enabled(b,1,15+int(i),!active);
    }
    for(int event:{25,26,27})Enabled(b,2,event,false);
    const auto& actor=d.actors[selected];const int stage=Read(b,actor.route);
    if(stage>=0)
    {
        if(unsigned(stage)>=actor.stages.size())throw std::out_of_range("Map selection: decision stage");
        for(int event:{actor.stages[stage].firstEvent,actor.stages[stage].secondEvent})if(event>=0)
        {Enabled(b,2,event,true);if(auto* c=Find(b,2,event))c->body->vptrAt00->resetAt08(c->body);}
    }
    Apply(b);Dirty(b);
}
void PollDecision(Body& b)
{
    // 09:4FCC70先读决定项，再读角色切换，不能同帧换角色后误提交新角色。
    static_cast<void>(Input(b,0));const int decision=Input(b,2);
    if(decision>=25&&decision<=27)
    {b.services->source.PlayCue(b.services->profile.decision.decisionCue);Close(b,b.page,decision==27?1:0);return;}
    const int character=Input(b,1);
    if(character>=15&&character<=19)
    {b.services->source.PlayCue(b.services->profile.decision.characterCue);SelectDecisionActor(b,character-15);}
}
}
