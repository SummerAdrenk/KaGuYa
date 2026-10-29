#include "LikabilityDialogNative.Profile.h"
#include "LikabilityDialogNative.Internal.h"
#include <Runtime/SpriteRuntime.h>
#include <Runtime/WaitMarkServicesRuntime.h>
#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilityNativeDetail;
struct OwnedReference final
{
    Slot slot{};
    OwnedReference() = default;
    ~OwnedReference() noexcept { ReleaseYtlReference(slot); }
    OwnedReference(const OwnedReference&) = delete;
    OwnedReference& operator=(const OwnedReference&) = delete;
    OwnedReference(OwnedReference&& other) noexcept : slot(std::exchange(other.slot,{})) {}
    OwnedReference& operator=(OwnedReference&& other) noexcept
    { if(this!=&other){ReleaseYtlReference(slot);slot=std::exchange(other.slot,{});}return *this; }
    Surface& SurfaceBody() const { return *Resolve<Surface>(slot); }
};
struct ProfileChild final
{
    const LikabilityControlProfile* data;
    GuiControlNativeBody50h* body; // common.children持有；不是第二个owner。
};
struct ProfileNavigation final
{
    const LikabilityNavigationProfile* data;
    OwnedReference reference;
};
void DirtyCanvas(Body& body)
{ body.common.vptrAt00->invalidateAt64(&body.common,body.boundsAt5C); }
}

class LikabilityProfileState final : public ILikabilityPaintTarget
{
public:
    LikabilityProfileState(Body& b,const LikabilityResourceProfile& p,ILikabilityProfileHost& h)
        : body(b),profile(p),host(h),services(RequireLikabilityDialogNativeServices()) {}
    std::int32_t Read(const char* name) override
    {
        if(!name) throw std::logic_error("Likability attempted a null variable lookup");
        services.source.EnsureRootAt413BE0(profile.refreshVa);
        return NamedValue(services.source.RootAt6198C4(profile.refreshVa),name,profile.refreshVa);
    }
    OwnedReference Load(const char* name)
    {
        if(!name) throw std::logic_error("Likability attempted a null image load");
        OwnedReference result;
        if(!LoadGuiControlSurfaceReferenceAt480000(result.slot,
            services.loader.CaptureLoadReceiverAt413BE0(),name,services.loader,services.surfaces,services.strings)) throw 1;
        return result;
    }
    Surface& Image(const char* name)
    {
        if(!name) throw std::logic_error("Likability requested a null image");
        auto found=images.find(name);
        if(found==images.end()) found=images.emplace(name,Load(name)).first;
        return found->second.SurfaceBody();
    }
    LikabilityImageMetrics Metrics(const char* name) override
    { auto& image=Image(name);return {{image.originXAt10,image.originYAt14},{image.widthAt08,image.heightAt0C}}; }
    void Draw(const char* name,POINT at,const RECT* source,const RECT* clip) override
    {
        auto& input=Image(name);
        if(!painting) throw std::logic_error("Likability paint target is not selected");
        RECT localClip{};
        if(clip) localClip={clip->left-painting->originXAt10,clip->top-painting->originYAt14,
            clip->right-painting->originXAt10,clip->bottom-painting->originYAt14};
        if(painting->handleAt04 && input.handleAt04)
            static_cast<void>(services.graphics.CompositionAt4182F0().BlitSurface(
                painting->handleAt04,input.handleAt04,at.x-painting->originXAt10,at.y-painting->originYAt14,
                nullptr,source,clip?&localClip:nullptr,0));
    }
    void Visible(std::int32_t event,bool value) override { visibility[event]=value; }
    OwnedReference NewPage()
    {
        OwnedReference result;
        AdoptGraphicsSurfaceAt407A80(result.slot,NewGraphicsSurfaceAt40C2E0(services.surfaces,services.strings));
        auto& page=result.SurfaceBody();const auto& canvas=*Resolve<Surface>(body.canvasAt84);
        if(!page.CreateAt415010(services.surfaces,canvas.widthAt08,canvas.heightAt0C,canvas.alphaAt18,1)) throw 1;
        page.originXAt10=canvas.originXAt10;page.originYAt14=canvas.originYAt14;
        return result;
    }
    void CopyPage()
    {
        if(selected<0 || static_cast<std::size_t>(selected)>=pages.size()) throw std::out_of_range("Likability selected page has no rendered surface");
        auto& canvas=*Resolve<Surface>(body.canvasAt84);
        host.LikabilityCopyApi().SurfaceCopyBlitRaw(canvas.handleAt04,pages[selected].SurfaceBody().handleAt04,0,0,nullptr,nullptr,nullptr,0);
    }
    void ApplyVisibility()
    {
        for(const auto& child:children)
        {
            const auto& data=*child.data;const auto event=data.descriptor.common.eventAt00;
            bool show=data.page<0 || data.page==selected;
            if(const auto found=visibility.find(event);found!=visibility.end()) show=show && found->second;
            for(const auto& visual:profile.flow.visuals)
                if(visual.event==event) show=show && visual.page==selected && visual.variant==variant;
            const auto tab=std::find(profile.flow.tabs.begin(),profile.flow.tabs.end(),event);
            if(tab!=profile.flow.tabs.end())
            {
                const auto isCurrent=static_cast<std::int32_t>(tab-profile.flow.tabs.begin())==selected;
                if(!canChange) show=show && profile.flow.keepForcedCurrentTab && isCurrent;
                else if(profile.flow.currentTab==LikabilityCurrentTab::Hide) show=show && !isCurrent;
                if(profile.flow.currentTab==LikabilityCurrentTab::Disable)
                    child.body->vptrAt00->writeStateAt44(child.body,!isCurrent);
            }
            child.body->vptrAt00->writeEnabledAt4C(child.body,show);
        }
    }
    void Select(std::int32_t page,bool change)
    {
        if(page<0 || page>=profile.flow.pages) throw std::out_of_range("Likability page outside original profile domain");
        const auto changed=selected!=page;
        selected=page;canChange=change;variant=0;
        if(changed)
        {
            CopyPage();
            ClearGuiNavigationReferencesAt4D80B0(body.common.navigationAt24);
            for(const auto& nav:navigation)
                if(nav.data->page<0 || nav.data->page==selected)
                    AppendGuiNavigationReferenceAt4D9D60(body.common.navigationAt24,nav.reference.slot);
            for(const auto& child:children)
            {
                const auto tab=std::find(profile.flow.tabs.begin(),profile.flow.tabs.end(),child.data->descriptor.common.eventAt00);
                if(tab==profile.flow.tabs.end()) continue;
                // 08只reset新选中tab；其它分页族的原ChangePage逐一reset所有tab。
                if(!std::holds_alternative<LikabilityLoveHolicData>(profile.paint) || tab-profile.flow.tabs.begin()==page)
                    child.body->vptrAt00->resetAt08(child.body);
            }
        }
        ApplyVisibility();
        DirtyCanvas(body);
    }
    void HoverItems()
    {
        const auto* data=std::get_if<LikabilityLoveHolicData>(&profile.paint);
        if(!data || selected!=2) return;
        std::size_t description=0;
        for(std::size_t i=0;i<data->hoverIds.size();++i)
        {
            auto found=std::find_if(children.begin(),children.end(),[&](const auto& child){return child.data->descriptor.common.eventAt00==data->hoverIds[i];});
            if(found==children.end()) throw std::logic_error("Likability08 item control is missing");
            const auto state=found->body->vptrAt00->resourceStateAt38(found->body);
            if(state==5 || state==3 || state==4) {description=i+1;break;}
        }
        if(hoverDescription==static_cast<int>(description)) return;
        for(std::size_t i=0;i<data->descriptionIds.size();++i) Visible(data->descriptionIds[i],i==description);
        hoverDescription=static_cast<int>(description);
        ApplyVisibility();DirtyCanvas(body);
    }
    void LoadAnimations()
    {
        const auto* data=std::get_if<LikabilityAnimatedData>(&profile.paint);
        if(!data) return;
        auto& s=host.LikabilityAnimation();
        if(data->animation.size()!=5) throw std::logic_error("Likability02 needs one ANM and four PLT members");
        ArchiveReaderFactoryRuntime primary(s.archives,s.transaction,s.files,s.readers,s.codecs);
        // 原02明确指定vrm.arc；这里显式转为共享catalog的vrm命名空间，不按扩展名猜档案。
        const auto primaryName=std::string("vrm:\\")+data->animation[0];
        if(!primary.LoadBorrowedAt483D90(primaryName.c_str(),s.key)) throw std::runtime_error("Likability02 ANM reader failed");
        const auto& canvas=*Resolve<Surface>(body.canvasAt84);
        for(std::size_t i=1;i<data->animation.size();++i)
        {
            SpriteRuntime sprite(services.surfaces);
            if(!sprite.LoadArchiveReaderMemberAt4B0840(0,primary.Reader(),1,s.codecs)) throw std::runtime_error("Likability02 ANM member failed");
            ArchiveReaderFactoryRuntime auxiliary(s.archives,s.transaction,s.files,s.readers,s.codecs);
            const auto name=std::string("vrm:\\")+data->animation[i];
            if(!auxiliary.LoadBorrowedAt483D90(name.c_str(),s.key)) throw std::runtime_error("Likability02 PLT reader failed");
            if(!sprite.LoadArchiveReaderMemberAt4B0840(1,auxiliary.Reader(),1,s.codecs)) throw std::runtime_error("Likability02 PLT member failed");
            sprite.SetLogicalName(data->animation[i]);
            sprite.SetOrigin(sprite.X()-canvas.originXAt10,sprite.Y()-canvas.originYAt14);
            sprites.push_back(std::move(sprite));
        }
    }
    void DrawAnimations()
    {
        if(sprites.empty()) return;
        WaitMarkRendererRuntime renderer(host.LikabilityAnimation().graphics);
        auto& canvas=*Resolve<Surface>(body.canvasAt84);
        for(const auto& sprite:sprites)
            if(renderer.DrawToSurface(sprite,GraphicsSurfaceDestination::FromNative(canvas),nullptr)!=0)
                throw std::runtime_error("Likability02 sprite render failed");
    }
    Body& body;
    const LikabilityResourceProfile& profile;
    ILikabilityProfileHost& host;
    LikabilityDialogNativeServices& services;
    std::vector<OwnedReference> pages;
    std::map<std::string,OwnedReference,std::less<>> images;
    std::vector<ProfileChild> children;
    std::vector<ProfileNavigation> navigation;
    std::map<std::int32_t,bool> visibility;
    std::vector<SpriteRuntime> sprites;
    Surface* painting=nullptr;
    std::int32_t selected=-1,variant=0,hoverDescription=-1;
    bool canChange=true;
};

LikabilityProfileState* ConstructLikabilityProfileGui(Body& body,const LikabilityResourceProfile& profile,ILikabilityProfileHost& host)
{
    RequireUsableLikabilityProfile(profile);
    if(std::holds_alternative<LikabilityBaseline18Data>(profile.paint)) throw std::logic_error("Likability18 construction must retain its baseline path");
    auto& s=RequireLikabilityDialogNativeServices();
    ConstructGuiDialogAt4E0BE0(&body.common,nullptr,-1);
    body.common.vptrAt00=&LikabilityDialogNativeVtable();
    ::new(&body.clickedManagerAt6C) Slot{};
    ::new(&body.backgroundAt78) Slot{};
    ::new(&body.canvasAt84) Slot{};
    struct Unwind final {Body& body;bool armed=true;~Unwind()noexcept{if(armed)DestroyLikabilityDialogNative(body);}} unwind{body};
    auto state=std::make_unique<LikabilityProfileState>(body,profile,host);
    const char* frame=!profile.backgrounds.empty()?profile.backgrounds[0]:profile.pages.empty()?nullptr:profile.pages[0];
    auto canvas=state->Load(frame);Assign(body.canvasAt84,canvas.slot,false);
    AdoptGuiClickedEventManagerAt4D7B90(body.clickedManagerAt6C,new GuiClickedEventReferences{});
    OwnedReference child;
    CreateGuiStaticFromSurfaceAt505570(child.slot,&body.common,0,Copy(body.canvasAt84),s.surfaces);
    GuiDialogBaseVtable().appendChildAt10(&body.common,Copy(child.slot));
    auto& surface=canvas.SurfaceBody();
    body.boundsAt5C={surface.originXAt10,surface.originYAt14,surface.originXAt10+surface.widthAt08,surface.originYAt14+surface.heightAt0C};
    const auto constructorVariant=profile.flow.constructorVariant?state->Read(profile.flow.constructorVariant)!=0:0;
    for(const auto& data:profile.controls)
    {
        if(data.canvasBackground || (data.constructorVariant>=0 && data.constructorVariant!=constructorVariant)) continue;
        OwnedReference item;
        if(data.button) CreateGuiButtonAt5041D0(item.slot,&body.common,data.descriptor,s.loader,s.surfaces,s.strings);
        else CreateGuiStaticAt505800(item.slot,&body.common,data.descriptor.common,s.loader,s.surfaces,s.strings);
        auto* control=Resolve<GuiControlNativeBody50h>(item.slot);
        if(data.button)
        {
            OwnedReference event;
            CreateToolClickedEventAt4E2F5A(event.slot,item.slot);
            BindEventAt536DB0(control,Copy(event.slot));
            AppendEventAt4FEF80(Resolve<GuiClickedEventReferences>(body.clickedManagerAt6C),Copy(event.slot));
        }
        GuiDialogBaseVtable().appendChildAt10(&body.common,Copy(item.slot));
        state->children.push_back({&data,control});
    }
    AdoptGuiInputAt536EA0(body.common.inputAt18,NewGameGuiInputAt4EAAED());
    for(const auto& row:profile.navigation)
    {
        if(row.mappings.empty() || row.mappings.back().sourceEventValue>=0)
            throw std::logic_error("Likability navigation requires the original negative-source terminator");
        OwnedReference nav;
        CreateGuiNavigationAt504CE0(nav.slot,row.action,row.mappings.data());
        state->navigation.push_back({&row,std::move(nav)});
    }
    state->LoadAnimations();
    unwind.armed=false;
    return state.release();
}
void DestroyLikabilityProfileState(LikabilityProfileState* state) noexcept { delete state; }
bool LikabilityProfileHasCancelInput(const LikabilityProfileState& state) noexcept { return state.profile.flow.cancelInput; }
void RefreshLikabilityProfileGui(LikabilityProfileState& state)
{
    state.pages.clear();state.visibility.clear();state.selected=-1;state.hoverDescription=-1;
    struct Painting final { LikabilityProfileState& state;~Painting(){state.painting=nullptr;} } painting{state};
    for(int page=0;page<state.profile.flow.pages;++page)
    {
        auto surface=state.NewPage();state.painting=&surface.SurfaceBody();
        PaintLikabilityPage(state.profile,page,state);
        state.painting=nullptr;state.pages.push_back(std::move(surface));
    }
    const auto& flow=state.profile.flow;
    auto page=flow.readRememberedPage?state.host.ReadLikabilityPage():0;
    bool change=true;
    if(flow.forcedPage) {const auto forced=state.Read(flow.forcedPage);if(forced>=0){page=forced;change=false;}}
    state.Select(page,change);
    // 原12/13的附加显隐在首次选页后查询；不把它混成构造期资源许可。
    for(const auto& condition:flow.conditions)
    {const auto value=state.Read(condition.variable);state.Visible(condition.event,condition.nonnegative?value>=0:value!=0);}
    state.ApplyVisibility();state.DrawAnimations();
}
void TickLikabilityProfileGui(LikabilityProfileState& state)
{
    bool changed=false;
    for(auto& sprite:state.sprites) if(sprite.AdvanceAnimation()==0) changed=true;
    if(changed) {state.CopyPage();state.DrawAnimations();DirtyCanvas(state.body);}
}
void DispatchLikabilityProfileEvent(LikabilityProfileState& state,std::int32_t event)
{
    const auto& flow=state.profile.flow;
    if(event==flow.close) {state.body.phaseAt90=4;return;}
    const auto tab=std::find(flow.tabs.begin(),flow.tabs.end(),event);
    if(tab!=flow.tabs.end()) state.Select(static_cast<int>(tab-flow.tabs.begin()),true);
    else if(flow.next>=0 && event==flow.next) state.Select((state.selected+1)%flow.pages,true);
    else if(flow.previous>=0 && event==flow.previous) state.Select((state.selected+flow.pages-1)%flow.pages,true);
    else if(flow.toggle>=0 && event==flow.toggle) {state.variant^=1;state.ApplyVisibility();DirtyCanvas(state.body);}
    // 08无事件时也扫描hover；未知event不触发关闭，不改脚本变量。
    state.HoverItems();
}
void CommitLikabilityProfileClose(LikabilityProfileState& state)
{
    if(state.profile.flow.storePageOnClose) state.host.StoreLikabilityPage(state.selected);
}
}
