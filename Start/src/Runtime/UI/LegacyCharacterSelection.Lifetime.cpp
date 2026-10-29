#include "LegacyCharacterSelection.Internal.h"
#include <algorithm>
#include <memory>

namespace KaGuYa::Start::Runtime
{
using namespace LegacyCharacterSelectionDetail;
LegacyCharacterSelectionDialog::LegacyCharacterSelectionDialog(LegacyCharacterSelectionServices& s)
    :services(&s),image(s.surfaces),black(s.surfaces),gauge(s.surfaces){}
LegacyCharacterSelectionScope::LegacyCharacterSelectionScope(LegacyCharacterSelectionServices& s) noexcept
    :surfaces_(s.surfaces),graphics_(s.graphics),input_(s.input),screen_(s.screen){}
namespace LegacyCharacterSelectionDetail
{
int Read(Body& b,const char* name)
{auto& s=*b.services;return ReadScriptValueAt4C73A0(s.source.Params(),ResolveScriptNamedValueNativeAt498020(name,s.names,s.strings),s.errors);}
void Write(Body& b,const char* name,int value)
{auto& s=*b.services;WriteScriptValueAt4C8220(s.source.Params(),ResolveScriptNamedValueNativeAt498020(name,s.names,s.strings),value,s.errors);}
LegacyCharacterSelectionControl* Find(Body& b,int event)
{for(auto& c:b.controls)if(c.body->eventAt04==event)return &c;return nullptr;}
void Visible(Body& b,int event,bool value){if(auto* c=Find(b,event))c->visible=value;}
void Enabled(Body& b,int event,bool value){if(auto* c=Find(b,event))c->enabled=value;}
void Apply(Body& b)
{
    const auto kind=b.services->profile.kind;
    const int view=kind==LegacyCharacterSelectionKind::TwoPage?b.page:b.mode;
    const bool modal=kind==LegacyCharacterSelectionKind::TwoPage&&b.selected>=0;
    for(auto& c:b.controls)
    {
        const bool shown=c.visible&&(c.view==-1||c.view==view||c.view==100+b.option||(c.view==-2&&modal));
        c.body->vptrAt00->writeEnabledAt4C(c.body,shown);
        c.body->vptrAt00->writeStateAt44(c.body,c.enabled);
    }
}
void Dirty(Body& b)
{const auto size=b.services->source.ScreenSize();RECT r{0,0,size.cx,size.cy};auto* d=b.common.vptrAt00->dirtyAt00(&b.common);d->vptrAt00->addRectAt08(d,&r);}
void Load(Body& b,GraphicsSurfaceReference& out,const char* name)
{
    auto& s=*b.services;out.ConstructAt40C2E0();
    if(!s.loader.LoadIntoAt4800A0(s.loader.CaptureLoadReceiverAt413BE0(),*out,name))throw 1;
}
void CreateCanvas(Body& b,GraphicsSurfaceReference& out,int alpha)
{out.ConstructAt40C2E0();const auto size=b.services->source.ScreenSize();if(!out->CreateAt415010(b.services->surfaces,size.cx,size.cy,alpha,1))throw 1;}
void AddWidget(Body& b,const LegacyCharacterSelectionWidget& w,int view)
{
    auto& s=*b.services;Ref control;
    if(w.kind==0)CreateGuiStaticAt505800(control.slot,&b.common,w.common,s.loader,s.surfaces,s.strings);
    else if(w.kind==1)
    {GuiButtonDescriptorNative2Ch d{w.common,w.hitSlot,w.childId};CreateGuiButtonAt5041D0(control.slot,&b.common,d,s.loader,s.surfaces,s.strings);}
    else
    {
        SaveLoadMessageBoxCheckBoxDescriptorNative48h d{w.common.eventAt00,w.common.namesAt04,w.checked,w.common.positionAt20,w.hitSlot,w.childId};
        SaveLoadMessageBoxNativeChildFactoriesRuntime factory{{s.loader,s.surfaces,s.strings}};
        factory.CreateAt5043B0(&control.slot,&b.common,&d);
    }
    auto* child=ResolveGuiControl(control.slot);
    Ref event;CreatePopupClickedEventAt4EA7B3(event.slot,child->eventAt04);
    // 4EA856从lateSource取事件base；不能把控件base与事件control拼成错误Ref。
    BindPopupGuiControlEventAt4EA856(*child,Copy(event.slot),event.slot);
    AppendGuiClickedEventReference(b.events,event.slot);
    GuiDialogBaseVtable().appendChildAt10(&b.common,Copy(control.slot));
    b.controls.push_back({child,view});
}
void AddCanvas(Body& b,GraphicsSurfaceReference& surface,int event,int view)
{
    Ref control;CreateGuiStaticFromSurfaceAt505570(control.slot,&b.common,event,Copy(surface.Slot()),b.services->surfaces);
    auto* child=ResolveGuiControl(control.slot);
    GuiDialogBaseVtable().appendChildAt10(&b.common,Copy(control.slot));b.controls.push_back({child,view});
}
void Destroy(Body& b) noexcept
{DestroyGuiClickedEventReferencesAt4F6D10(b.events);DestroyGuiDialogAt4F7AC0(&b.common);}
namespace
{
void __fastcall BeginThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b.services);Begin(g,nullptr);}
int __fastcall PollThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b.services);return Poll(g,nullptr);}
int __fastcall EndThunk(Gui* g,void*){auto& b=*reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b.services);return End(g,nullptr);}
Gui* __fastcall DeleteThunk(Gui* g,void*,unsigned flags) noexcept
{auto* b=reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b->services);Destroy(*b);b->~Body();if(flags&1)::operator delete(b,sizeof(Body));return g;}
void __fastcall DrawThunk(Gui* g,void*,const RECT* clip)
{
    auto& b=*reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b.services);if(!g->enabledAt38)return;
    bool sprites=false;
    for(auto& c:b.controls)
    {
        if(!sprites&&c.view!=0){RenderAnimations(b,clip,nullptr);sprites=true;}
        c.body->vptrAt00->drawAt0C(c.body,clip);
    }
    if(!sprites)RenderAnimations(b,clip,nullptr);
}
void __fastcall CompositeThunk(Gui* g,void*,Slot owned,const RECT* clip)
{
    auto& b=*reinterpret_cast<Body*>(g);LegacyCharacterSelectionScope scope(*b.services);Ref lifetime;lifetime.slot=owned;
    if(!g->enabledAt38)return;bool sprites=false;
    for(auto& c:b.controls)
    {
        if(!sprites&&c.view!=0){RenderAnimations(b,clip,&owned);sprites=true;}
        c.body->vptrAt00->compositeAt10(c.body,Copy(owned),clip);
    }
    if(!sprites)RenderAnimations(b,clip,&owned);
}
}
}
const GuiDialogNativeVtable35& LegacyCharacterSelectionDialogMethods() noexcept
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
Gui* NewLegacyCharacterSelectionDialog(LegacyCharacterSelectionServices& s)
{
    ValidateLegacyCharacterSelectionProfile(s.profile);
    if(s.profile.kind==LegacyCharacterSelectionKind::None)throw std::logic_error("Character selection: no scene in this game");
    LegacyCharacterSelectionScope scope(s);auto b=std::make_unique<Body>(s);
    ConstructGuiDialogAt4E0BE0(&b->common,nullptr,-1);b->common.vptrAt00=&LegacyCharacterSelectionDialogMethods();
    try
    {
        const auto& p=s.profile;
        if(p.modeVariable)b->mode=Read(*b,p.modeVariable);
        if(p.optionVariable)b->option=Read(*b,p.optionVariable);
        if(b->mode<0||b->mode>=p.modeCount||b->option<0||b->option>=p.optionCount)
            throw std::out_of_range("Character selection: mode/option outside original resource table");
        if(p.kind==LegacyCharacterSelectionKind::TabbedSix)
        {
            b->canvases.reserve(7);
            for(int i=0;i<7;++i){b->canvases.emplace_back(s.surfaces);CreateCanvas(*b,b->canvases.back());AddCanvas(*b,b->canvases.back(),i+23,0);}
        }
        for(const auto& view:p.views)
        {
            const bool active=view.id<0||view.id==b->mode||view.id==100+b->option||p.kind==LegacyCharacterSelectionKind::TwoPage;
            if(active)for(const auto& w:view.widgets)AddWidget(*b,w,view.id);
            if(view.id==0)for(const auto& a:p.animations)
            {AddWidget(*b,a.button,0);auto sprite=std::make_unique<SpriteRuntime>(s.surfaces);s.source.LoadAnimation(*sprite,a.commands,a.pictures);b->animations.push_back(std::move(sprite));}
        }
        if(p.kind==LegacyCharacterSelectionKind::ResourcePair){s.source.CaptureMasoGauge(b->gauge);AddCanvas(*b,b->gauge,19,-1);}
        Load(*b,b->black,"bg_black.bgd");CreateCanvas(*b,b->image,0);
        AdoptGuiInputAt536EA0(b->common.inputAt18,NewGameGuiInputAt4EAAED());
        return &b.release()->common;
    }
    catch(...){Destroy(*b);throw;}
}
}
