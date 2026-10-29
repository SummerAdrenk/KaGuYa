#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
void CreateCanvas(Body& b,GraphicsSurfaceReference& out,int alpha)
{
    out.ConstructAt40C2E0();const auto size=b.services->source.ScreenSize();
    if(!out->CreateAt415010(b.services->surfaces,size.cx,size.cy,alpha,1))throw 1;
    b.services->drawing.SurfaceClearRaw(out->handleAt04,out->clearColorAt20,nullptr);
}
void Load(Body& b,GraphicsSurfaceReference& out,const char* name)
{
    // 12:4FE5E0的time1指针为NULL；不能拿另一时段替代。
    if(!name)throw std::invalid_argument("Map selection: null required resource");
    out.ConstructAt40C2E0();auto& s=*b.services;
    if(!s.loader.LoadIntoAt4800A0(s.loader.CaptureLoadReceiverAt413BE0(),*out,name))throw 1;
}
void Blit(Body& b,GraphicsSurfaceReference& destination,const GraphicsSurfaceReference& source,POINT at,bool checked)
{
    const bool success=destination->handleAt04&&source->handleAt04&&b.services->graphics.CompositionAt4182F0().BlitSurface(
        destination->handleAt04,source->handleAt04,at.x,at.y,nullptr,nullptr,nullptr,0);
    if(checked&&!success)throw 1;
}
void Natural(Body& b,GraphicsSurfaceReference& target,const char* name)
{GraphicsSurfaceReference image(b.services->surfaces);Load(b,image,name);Blit(b,target,image,{image->originXAt10,image->originYAt14});}
void Transition(Body& b)
{
    auto& s=*b.services;auto& source=s.source;const auto& p=s.profile;
    if(b.transitionStage==0)
    {
        b.operation={};b.operation.opcode=0;
        if(b.phase==LegacyMapSelectionPhase::Opening)
        {
            b.common.vptrAt00->compositeAt5C(&b.common,Copy(b.image.Slot()),nullptr);b.operation.source=b.image.Get();
            if(p.readTimeForFixedBgm)static_cast<void>(Read(b,"time"));
            unsigned time=unsigned(SelectIndex(b,p.bgmIndex,nullptr));
            // 11:504B80/12:501780音乐越界归0；背景time的拒绝策略独立。
            if(p.bgmIndex==LegacyMapSelectionIndex::Time&&time>=p.bgm.size())time=0;
            if(time>=p.bgm.size())throw std::out_of_range("Map selection: BGM table");
            source.PlayBgmAndStopEffects(p.bgm[time]);
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
    b.phase=b.phase==LegacyMapSelectionPhase::Opening?LegacyMapSelectionPhase::Ready:LegacyMapSelectionPhase::Finished;
    source.UnSave().ModeAt04()=1;
}
void AdvanceAnimations(Body& b)
{
    for(auto& a:b.animations)
    {
        auto* c=Find(b,a.view,a.event);
        if(!View(b,a.view).shown||!c||!c->visible||(!a.normalOnly&&!c->enabled))continue;
        a.sprite->AdvanceAnimation();
    }
}
void RenderAnimations(Body& b,int view,const RECT* clip,const Slot* target)
{
    WaitMarkRendererRuntime renderer(b.services->drawing);
    for(auto& a:b.animations)
    {
        if(a.view!=view)continue;
        auto* c=Find(b,view,a.event);if(!c||!c->visible)continue;
        if(a.normalOnly?c->body->vptrAt00->resourceStateAt38(c->body)!=0:!c->enabled)continue;
        if(target)
        {auto* surface=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*target);static_cast<void>(renderer.DrawToSurface(*a.sprite,GraphicsSurfaceDestination::FromNative(*surface),clip));}
        else
        {const auto size=b.services->source.ScreenSize();const RECT whole{0,0,size.cx,size.cy};static_cast<void>(renderer.DrawToScreen(*a.sprite,clip?*clip:whole));}
    }
}
}
