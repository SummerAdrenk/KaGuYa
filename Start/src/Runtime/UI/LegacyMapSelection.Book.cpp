#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMapSelectionDetail
{
namespace
{
void Picture(Body& b,GraphicsSurfaceReference& canvas,const char* name)
{
    GraphicsSurfaceReference image(b.services->surfaces);Load(b,image,name);
    Blit(b,canvas,image,{image->originXAt10,image->originYAt14},false);
}
void RefreshPage(Body& b,const LegacyMapSelectionBookPage& page)
{
    // 06:4D7340节点阈值/状态/形态决定可见与启用；已过节点绘制完成标记。
    auto& canvas=Canvas(b,page.view,page.canvasEvent);
    b.services->drawing.SurfaceClearRaw(canvas->handleAt04,canvas->clearColorAt20,nullptr);
    for(unsigned trackIndex=0;trackIndex<page.tracks.size();++trackIndex)
    {
        const auto& track=page.tracks[trackIndex];
        const int progress=Read(b,track.progress),state=Read(b,track.state),form=Read(b,track.form);
        for(unsigned i=0;i<track.prefixImages.size();++i)
        {
            // 06:4F77D0按有符号高DWORD/无符号低DWORD比较，不改成progress单独比较。
            const auto threshold=track.nodes[i].threshold;
            if(progress<threshold||(progress==threshold&&std::uint32_t(state)==0))break;
            Picture(b,canvas,track.prefixImages[i]);
        }
        // 06:4F8920先画第一组节点/状态图，再画全局图、第二组状态图和节点。
        if(page.view==7&&trackIndex==1&&b.services->profile.book.globalImage&&
            b.services->source.GlobalWord(b.services->profile.book.globalWord))
            Picture(b,canvas,b.services->profile.book.globalImage);
        if(track.stateImage&&state!=0&&!(page.view==7&&trackIndex==0))Picture(b,canvas,track.stateImage);
        for(const auto& node:track.nodes)
        {
            auto* c=Find(b,page.view,node.event);if(!c)continue;
            const bool past=node.threshold<progress,current=node.threshold==progress;
            c->visible=past||(current&&state==2);c->enabled=!past;
            if(!c->visible)continue;
            const int symbol=past?0:form==0?-1:form==1?1:2;
            if(symbol>=0)
            {
                const auto& image=b.badges[symbol];
                Blit(b,canvas,image,{Add(node.position.x,image->originXAt10),Add(node.position.y,image->originYAt14)},false);
            }
        }
        if(track.stateImage&&state!=0&&page.view==7&&trackIndex==0)Picture(b,canvas,track.stateImage);
    }
    for(const auto& overlay:page.overlays)
    {
        const unsigned value=unsigned(Read(b,overlay.variable));
        if(value<overlay.images.size()&&overlay.images[value])Picture(b,canvas,overlay.images[value]);
    }
    Reset(b,page.view);
}
void ShowBook(Body& b)
{
    b.content=1;View(b,0).shown=false;View(b,8).shown=true;
    for(const auto& page:b.services->profile.book.pages)
    {RefreshPage(b,page);View(b,page.view).shown=page.view==b.services->profile.book.pageOrder[b.page];}
    Reset(b,8);Apply(b);Dirty(b);
}
void ShowTool(Body& b)
{
    const int previous=b.content==0?0:8;Reset(b,previous);
    auto& controls=View(b,previous).common;controls.vptrAt00->resetChildrenAt68(&controls);
    b.suspendedContent=b.content;b.content=2;
    View(b,0).interactive=false;View(b,8).interactive=false;
    for(const auto& page:b.services->profile.book.pages)View(b,page.view).interactive=false;
    View(b,-1).shown=true;View(b,-1).interactive=true;Reset(b,-1);
    b.services->source.UnSave().ModeAt04()=0;Apply(b);Dirty(b);
}
void HideTool(Body& b)
{
    View(b,-1).shown=false;b.content=b.suspendedContent;b.toolResume=false;
    View(b,0).interactive=true;View(b,8).interactive=true;
    for(const auto& page:b.services->profile.book.pages)View(b,page.view).interactive=true;
    Reset(b,b.content==0?0:b.services->profile.book.pageOrder[b.page]);Reset(b,8);
    b.services->source.UnSave().ModeAt04()=1;b.services->source.ResetKeyboard();Apply(b);Dirty(b);
}
}
void ConstructBook(Body& b)
{
    for(const auto& symbol:b.services->profile.book.symbols)
    {
        b.badges.emplace_back(b.services->surfaces);auto& image=b.badges.back();Load(b,image,symbol.name);
        image->originXAt10=Add(image->originXAt10,symbol.offset.x);image->originYAt14=Add(image->originYAt14,symbol.offset.y);
    }
    for(const auto& page:b.services->profile.book.pages)static_cast<void>(Canvas(b,page.view,page.canvasEvent));
}
void BeginBook(Body& b)
{
    b.content=0;b.page=0;b.toolResume=false;View(b,0).shown=true;View(b,8).shown=false;View(b,-1).shown=false;
    for(const auto& page:b.services->profile.book.pages)View(b,page.view).shown=false;
    Reset(b,0);
}
void PollBook(Body& b)
{
    const auto& p=b.services->profile;
    if(b.toolResume){HideTool(b);return;}
    if(b.content==2)
    {
        const int event=Input(b,-1);
        if(b.services->source.EscapePressed()||event==15){HideTool(b);return;}
        for(const auto& command:p.system.commands)if(command.event==event)
        {b.services->source.SetCommand(command.name);b.toolResume=true;return;}
        return;
    }
    if(b.content==0)
    {
        const int event=Input(b,0);
        if(event>=17&&event<=21){b.page=p.book.overviewResults[event-17];ShowBook(b);}
        else if(event==22)ShowTool(b);
        return;
    }
    const int nav=b.services->source.RightPressed()?71:Input(b,8);
    if(nav==71)
    {for(const auto& page:p.book.pages)View(b,page.view).shown=false;BeginBook(b);Apply(b);Dirty(b);return;}
    if(nav==72){ShowTool(b);return;}
    if(nav==69||nav==70)
    {
        View(b,p.book.pageOrder[b.page]).shown=false;b.page=(b.page+(nav==69?4:1))%5;
        View(b,p.book.pageOrder[b.page]).shown=true;Reset(b,p.book.pageOrder[b.page]);Apply(b);Dirty(b);return;
    }
    const int view=p.book.pageOrder[b.page],event=Input(b,view);
    const auto it=std::find_if(p.book.pages.begin(),p.book.pages.end(),[&](const auto& x){return x.view==view;});
    for(const auto& track:it->tracks)for(const auto& node:track.nodes)
        if(node.event==event){Close(b,b.page+(view==7&&event==68?1:0));return;}
}
}
