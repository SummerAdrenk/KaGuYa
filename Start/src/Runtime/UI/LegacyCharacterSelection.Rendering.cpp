#include "LegacyCharacterSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyCharacterSelectionDetail
{
namespace
{
void Blit(Body& b,GraphicsSurfaceReference& target,const GraphicsSurfaceReference& source,POINT at)
{
    if(!target->handleAt04||!source->handleAt04||!b.services->graphics.CompositionAt4182F0().BlitSurface(
        target->handleAt04,source->handleAt04,at.x,at.y,nullptr,nullptr,nullptr,0))throw 1;
}
}
void RefreshTabPictures(Body& b)
{
    const auto& p=b.services->profile;const int time=Read(b,"time");
    // 13:4E9630只在unsigned location<28时更新，越界保留原画布。
    for(unsigned i=0;i<6;++i)
    {
        const unsigned location=unsigned(Read(b,p.tabs[i].location));
        if(location<p.portraits.size())
        {GraphicsSurfaceReference image(b.services->surfaces);Load(b,image,p.portraits[location]);Blit(b,b.canvases[i],image,{0,0});}
    }
    GraphicsSurfaceReference base(b.services->surfaces);Load(b,base,p.dateBase);
    Blit(b,b.canvases[6],base,{base->originXAt10,base->originYAt14});
    const int date=Add(Read(b,"days"),1),ones=date%10,tens=(date/10)%10;
    if(ones<0||tens<0)throw std::out_of_range("Character selection: negative date digit index");
    GraphicsSurfaceReference digit(b.services->surfaces);Load(b,digit,p.dateDigits[ones]);
    Blit(b,b.canvases[6],digit,p.datePositions[0]);
    if(tens){Load(b,digit,p.dateDigits[tens]);Blit(b,b.canvases[6],digit,p.datePositions[1]);}
    if(time<0||static_cast<unsigned>(time)>=p.timeImages.size())throw std::out_of_range("Character selection: time image outside original table");
    if(p.timeImages[time])
    {GraphicsSurfaceReference image(b.services->surfaces);Load(b,image,p.timeImages[time]);Blit(b,b.canvases[6],image,{image->originXAt10,image->originYAt14});}
}
void RefreshTabs(Body& b,int selected)
{
    const auto& p=b.services->profile;
    if(selected<0||static_cast<unsigned>(selected)>=p.tabs.size())throw std::out_of_range("Character selection: invalid tab");
    b.activeTab=selected;
    const int type=p.tabs[selected].eventType?Read(b,p.tabs[selected].eventType):-1;
    for(unsigned i=0;i<p.tabs.size();++i)
    {
        const auto& tab=p.tabs[i];const bool active=int(i)==selected;
        Visible(b,tab.canvas,active);if(tab.background>=0)Visible(b,tab.background,active);
        Visible(b,tab.tab,!tab.route||Read(b,tab.route)>=0);Enabled(b,tab.tab,!active);
        Visible(b,tab.select,active);if(active)Enabled(b,tab.select,type>=0||i==5);
        if(auto* c=Find(b,tab.tab))c->body->vptrAt00->resetAt08(c->body);
        if(auto* c=Find(b,tab.select))c->body->vptrAt00->resetAt08(c->body);
    }
    for(int i=0;i<4;++i)Visible(b,19+i,i==type);
}
void RenderAnimations(Body& b,const RECT* clip,const Slot* target)
{
    if(b.animations.empty()||b.page!=0||!b.common.enabledAt38)return;
    WaitMarkRendererRuntime renderer(b.services->drawing);
    for(unsigned i=0;i<b.animations.size();++i)
    {
        auto* c=Find(b,b.services->profile.animations[i].button.common.eventAt00);
        if(!c||!c->visible||c->body->vptrAt00->resourceStateAt38(c->body)!=0)continue;
        if(target)
        {auto* surface=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*target);static_cast<void>(renderer.DrawToSurface(*b.animations[i],GraphicsSurfaceDestination::FromNative(*surface),clip));}
        else {const auto size=b.services->source.ScreenSize();const RECT whole{0,0,size.cx,size.cy};static_cast<void>(renderer.DrawToScreen(*b.animations[i],clip?*clip:whole));}
    }
}
}
