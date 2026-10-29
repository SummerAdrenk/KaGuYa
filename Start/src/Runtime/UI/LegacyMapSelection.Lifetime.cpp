#include "LegacyMapSelection.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace LegacyMapSelectionDetail;
LegacyMapSelectionDialog::LegacyMapSelectionDialog(LegacyMapSelectionServices& s)
    :services(&s),image(s.surfaces),black(s.surfaces){}
LegacyMapSelectionScope::LegacyMapSelectionScope(LegacyMapSelectionServices& s) noexcept
    :surfaces_(s.surfaces),graphics_(s.graphics),input_(s.input),screen_(s.screen){}
namespace LegacyMapSelectionDetail
{
int Read(Body& b,const char* name)
{auto& s=*b.services;return ReadScriptValueAt4C73A0(s.source.Params(),ResolveScriptNamedValueNativeAt498020(name,s.names,s.strings),s.errors);}
void Write(Body& b,const char* name,int value)
{auto& s=*b.services;WriteScriptValueAt4C8220(s.source.Params(),ResolveScriptNamedValueNativeAt498020(name,s.names,s.strings),value,s.errors);}
Panel& View(Body& b,int id)
{
    for(auto& p:b.panels)if(p->id==id)return *p;
    auto p=std::make_unique<Panel>(id);
    ConstructGuiDialogAt4E0BE0(&p->common,&b.common,id);
    try{b.panels.push_back(std::move(p));}
    catch(...){if(p)DestroyGuiDialogAt4F7AC0(&p->common);throw;}
    return *b.panels.back();
}
LegacyMapSelectionControl* Find(Body& b,int view,int event)
{for(auto& p:b.panels)if(p->id==view)for(auto& c:p->controls)if(c.body->eventAt04==event)return &c;return nullptr;}
void Visible(Body& b,int view,int event,bool value){if(auto* c=Find(b,view,event))c->visible=value;}
void Enabled(Body& b,int view,int event,bool value){if(auto* c=Find(b,view,event))c->enabled=value;}
void Apply(Body& b)
{
    for(auto& p:b.panels)
    {
        p->common.vptrAt00->writeEnabledAt24(&p->common,p->shown);
        p->common.vptrAt00->writeStateAt1C(&p->common,!p->dimmed);
        for(auto& c:p->controls)
        {c.body->vptrAt00->writeEnabledAt4C(c.body,c.visible);c.body->vptrAt00->writeStateAt44(c.body,c.enabled);}
    }
}
void Reset(Body& b,int view)
{auto& p=View(b,view);p.common.vptrAt00->resetInputAt3C(&p.common);ClearGuiClickedEventFlagsAt532200(p.events);Navigation(b,p);}
void Dirty(Body& b)
{
    const auto size=b.services->source.ScreenSize();RECT r{0,0,size.cx,size.cy};
    auto* d=b.common.vptrAt00->dirtyAt00(&b.common);d->vptrAt00->addRectAt08(d,&r);
}
void AddWidget(Body& b,const LegacyMapSelectionWidget& w,int view,bool clicked)
{
    auto& s=*b.services;auto& p=View(b,view);Ref control;
    if(w.kind==0)CreateGuiStaticAt505800(control.slot,&p.common,w.common,s.loader,s.surfaces,s.strings);
    else if(w.kind==1)
    {GuiButtonDescriptorNative2Ch d{w.common,w.hitSlot,w.childId};CreateGuiButtonAt5041D0(control.slot,&p.common,d,s.loader,s.surfaces,s.strings);}
    else
    {
        SaveLoadMessageBoxCheckBoxDescriptorNative48h d{w.common.eventAt00,w.common.namesAt04,w.checked,w.common.positionAt20,w.hitSlot,w.childId};
        SaveLoadMessageBoxNativeChildFactoriesRuntime factory{{s.loader,s.surfaces,s.strings}};
        factory.CreateAt5043B0(&control.slot,&p.common,&d);
    }
    auto* child=ResolveGuiControl(control.slot);
    if(clicked)
    {
        Ref event;CreatePopupClickedEventAt4EA7B3(event.slot,child->eventAt04);
        // 4EA856晚读base来自同一EventRef，不能与ControlRef混配payload/control。
        BindPopupGuiControlEventAt4EA856(*child,Copy(event.slot),event.slot);
        AppendGuiClickedEventReference(p.events,event.slot);
    }
    GuiDialogBaseVtable().appendChildAt10(&p.common,Copy(control.slot));
    p.controls.push_back({child});
}
void AddCanvas(Body& b,GraphicsSurfaceReference& surface,int view,int event)
{
    auto& p=View(b,view);Ref control;
    CreateGuiStaticFromSurfaceAt505570(control.slot,&p.common,event,Copy(surface.Slot()),b.services->surfaces);
    auto* child=ResolveGuiControl(control.slot);
    GuiDialogBaseVtable().appendChildAt10(&p.common,Copy(control.slot));p.controls.push_back({child});
}
GraphicsSurfaceReference& Canvas(Body& b,int view,int event)
{
    for(auto& c:b.canvases)if(c.view==view&&c.event==event)return c.surface;
    b.canvases.emplace_back(view,event,b.services->surfaces);
    auto& c=b.canvases.back();CreateCanvas(b,c.surface);AddCanvas(b,c.surface,view,event);return c.surface;
}
int SelectIndex(Body& b,LegacyMapSelectionIndex mode,const char* variable)
{
    using I=LegacyMapSelectionIndex;
    switch(mode)
    {
    case I::Fixed:return 0;
    case I::Time:return Read(b,"time");
    case I::BooleanTime:return Read(b,"time")!=0;
    case I::FormTwo:return Read(b,variable)==2;
    case I::CharacterDisplay:return b.services->source.CharacterDisplay(4)?0:1;
    }
    throw std::logic_error("Map selection: unknown variant rule");
}
void Destroy(Body& b) noexcept
{
    for(auto& p:b.panels){DestroyGuiClickedEventReferencesAt4F6D10(p->events);DestroyGuiDialogAt4F7AC0(&p->common);}
    b.panels.clear();DestroyGuiDialogAt4F7AC0(&b.common);
}
namespace
{
void __fastcall BeginThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b.services);Begin(g);}
int __fastcall PollThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b.services);return Poll(g);}
int __fastcall EndThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b.services);return End(g);}
Gui* __fastcall DeleteThunk(Gui* g,void*,unsigned flags) noexcept
{auto* b=reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b->services);Destroy(*b);b->~Body();if(flags&1)::operator delete(b,sizeof(Body));return g;}
void Paint(Body& b,const RECT* clip,const Slot* target)
{
    if(!b.common.enabledAt38)return;
    for(auto& p:b.panels)if(p->shown)
    {
        if(target)GuiDialogBaseVtable().compositeAt5C(&p->common,Copy(*target),clip);
        else GuiDialogBaseVtable().drawAt60(&p->common,clip);
        RenderAnimations(b,p->id,clip,target);RenderArea(b,p->id,clip,target);
    }
}
void __fastcall DrawThunk(Gui* g,void*,const RECT* clip)
{auto& b=*reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b.services);Paint(b,clip,nullptr);}
void __fastcall CompositeThunk(Gui* g,void*,Slot owned,const RECT* clip)
{auto& b=*reinterpret_cast<Body*>(g);LegacyMapSelectionScope scope(*b.services);Ref keep;keep.slot=owned;Paint(b,clip,&owned);}
}
}
const GuiDialogNativeVtable35& LegacyMapSelectionDialogMethods() noexcept
{
    static const auto table=[]
    {
        auto t=GuiDialogBaseVtable();
#define BIND(slot,fn) t.slot=reinterpret_cast<decltype(t.slot)>(&fn)
        BIND(beginAt44,BeginThunk);BIND(pollAt48,PollThunk);BIND(endAt4C,EndThunk);
        BIND(deletingDestructorAt04,DeleteThunk);BIND(drawAt60,DrawThunk);BIND(compositeAt5C,CompositeThunk);
#undef BIND
        return t;
    }();return table;
}
Gui* NewLegacyMapSelectionDialog(LegacyMapSelectionServices& s)
{
    ValidateLegacyMapSelectionProfile(s.profile);
    if(s.profile.kind==LegacyMapSelectionKind::None)throw std::logic_error("Map selection: absent in this game");
    LegacyMapSelectionScope scope(s);auto b=std::make_unique<Body>(s);
    ConstructGuiDialogAt4E0BE0(&b->common,nullptr,-1);b->common.vptrAt00=&LegacyMapSelectionDialogMethods();
    try
    {
        const auto& p=s.profile;
        // Gui子页的held/pressed/pointer逐层借父输入；真正GameInput必须装在根。
        AdoptGuiInputAt536EA0(b->common.inputAt18,NewGameGuiInputAt4EAAED());
        if(!p.background.names.empty())
        {
            const auto index=unsigned(SelectIndex(*b,p.background.index,nullptr));
            if(index>=p.background.names.size())throw std::out_of_range("Map selection: background time");
            b->canvases.emplace_back(-2,p.background.event,s.surfaces);auto& bg=b->canvases.back().surface;
            Load(*b,bg,p.background.names[index]);
            if(p.background.composeDateLayer)
            {GraphicsSurfaceReference date(s.surfaces);s.source.CaptureDate(date);Blit(*b,bg,date,{date->originXAt10,date->originYAt14});}
            if(p.background.dateBase)Natural(*b,bg,p.background.dateBase);
            if(!p.background.dateDigits.empty())
            {
                const int date=Add(Read(*b,"days"),1),ones=date%10,tens=(date/10)%10;
                if(ones<0||tens<0)throw std::out_of_range("Map selection: negative date digit");
                GraphicsSurfaceReference digit(s.surfaces);Load(*b,digit,p.background.dateDigits[ones]);Blit(*b,bg,digit,p.background.datePositions[0]);
                if(tens){Load(*b,digit,p.background.dateDigits[tens]);Blit(*b,bg,digit,p.background.datePositions[1]);}
            }
            AddCanvas(*b,bg,-2,p.background.event);
        }
        for(const auto& v:p.views)
        {
            View(*b,v.id);
            const auto index=unsigned(SelectIndex(*b,v.index,v.variable));
            if(index>=v.variants.size())throw std::out_of_range("Map selection: view variant");
            for(const auto& w:v.variants[index].widgets)AddWidget(*b,w,v.id);
        }
        switch(p.kind)
        {
        case LegacyMapSelectionKind::AnimatedActors:ConstructAnimated(*b);break;
        case LegacyMapSelectionKind::AreaActors:ConstructArea(*b);break;
        case LegacyMapSelectionKind::ProgressBook:ConstructBook(*b);break;
        case LegacyMapSelectionKind::CharacterDecision:ConstructDecision(*b);break;
        default:throw std::logic_error("Map selection: invalid kind");
        }
        Load(*b,b->black,"bg_black.bgd");CreateCanvas(*b,b->image,0);
        return &b.release()->common;
    }
    catch(...){Destroy(*b);throw;}
}
}
