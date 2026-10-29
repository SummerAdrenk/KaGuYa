#include "SaveLoadPageNative.Internal.h"
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageNativeDetail;
void AppendStatic(Body& body,Slot& child)
{
    LocalReference lifetime{child};
    GuiDialogBaseVtable().appendChildAt10(&body.common,Copy(child));
    lifetime.Reload(false); // 背景与页码Static正常尾内联释放，保持局部位模式。
}
void AppendButton(Body& body,Slot& child)
{
    LocalReference childLifetime{child};
    Slot event;CreateToolClickedEventAt4E2F5A(event,child);
    LocalReference eventLifetime{event};auto* cachedEvent=event.controlAt08;
    Slot binding{event.baseAt00,0,cachedEvent};if(cachedEvent)++cachedEvent->referenceCountAt04;
    {
        LocalReference bindingLifetime{binding};
        auto* selected=Resolve<GuiControlNativeBody50h>(child);
        auto& target=selected->eventAt3C;
        if(&target!=&binding)
        {
            ReleaseUncleared(target);
            target={event.baseAt00,0,cachedEvent};if(cachedEvent)++cachedEvent->referenceCountAt04;
        }
        selected->eventEnabledAt48=1;
        bindingLifetime.Captured(cachedEvent);
    }
    Slot copied=Copy(child);
    {
        LocalReference lifetime{copied};auto* cached=copied.controlAt08;
        if(body.common.childrenAt0C.end==body.common.childrenAt0C.capacity)
        {GrowGuiControlChildrenAt4D93B0(body.common.childrenAt0C,body.common.childrenAt0C.end,copied);cached=copied.controlAt08;}
        else AppendGuiControlChildReference(body.common.childrenAt0C,copied);
        lifetime.Captured(cached);
    }
    Slot managerArgument{event.baseAt00,0,cachedEvent};if(cachedEvent)++cachedEvent->referenceCountAt04;
    {
        LocalReference lifetime{managerArgument};auto* cached=cachedEvent;
        auto* manager=Resolve<GuiClickedEventReferences>(body.managerAt70);
        AppendPopupClickedEventReferenceAt4EA97F(*manager,managerArgument,cached);
        lifetime.Captured(cached);
    }
    eventLifetime.Captured(cachedEvent);
    childLifetime.Reload();
}
void MoveButton(Slot& child,std::int32_t x,std::int32_t y)
{
    auto* control=Resolve<GuiControlNativeBody50h>(child);
    Slot group=Copy(*control->resourcesAt28.begin);LocalReference lifetime{group};
    // 缓存原资源group的base/bias/control；5086C0后不以新的child首group代替。
    const auto selected=group;auto* const captured=group.controlAt08;
    MoveGuiControlResourceAt5086C0(Copy(group),x,y);
    auto* resource=Resolve<GuiControlResourceNativeBody38h>(selected);
    control=Resolve<GuiControlNativeBody50h>(child);
    control->xAt34=resource->combinedRectAt28.left;control->yAt38=resource->combinedRectAt28.top;
    lifetime.Captured(captured);
}
void CreatePageNumberCanvas(Body& body,SaveLoadPageNativeServices& services)
{
    RECT rect;std::memcpy(&rect,&Parent(&body.common).pageBoundsAt104,sizeof(rect));
    AdoptGraphicsSurfaceAt407A80(body.canvasAt80,NewGraphicsSurfaceAt40C2E0(services.common.surfaces,services.common.strings));
    auto* canvas=Resolve<GraphicsSurfaceNativeBody>(body.canvasAt80);
    if(!canvas->CreateAt415010(services.common.surfaces,Sub(rect.right,rect.left),Sub(rect.bottom,rect.top),1,1))throw 0;
    canvas=Resolve<GraphicsSurfaceNativeBody>(body.canvasAt80);canvas->originYAt14=rect.top;canvas->originXAt10=rect.left;
    Slot child;CreateGuiStaticFromSurfaceAt505570(child,&body.common,49,Copy(body.canvasAt80),services.common.surfaces);
    AppendStatic(body,child);
}
void CreatePreviewCanvases(Body& body,SaveLoadPageNativeServices& services)
{
    auto* const oldEnd=body.surfacesAt8C.end;
    ReleaseGuiControlSurfaceRangeAt43EA10(body.surfacesAt8C.begin,oldEnd);
    body.surfacesAt8C.end=body.surfacesAt8C.begin;
    for(std::int32_t slot=0;slot<CurrentSaveLoadResourceProfile().catalog.slotsPerPage;++slot)
    {
        Slot found;GuiDialogBaseVtable().findChildAt14(&body.common,&found,slot+25);
        LocalReference foundLifetime{found};auto* const foundControl=found.controlAt08;
        if(found.baseAt00)
        {
            RECT rect;auto* child=Resolve<GuiControlNativeBody50h>(found);
            ::CopyRect(&rect,child->vptrAt00->rectAt3C(child,&rect));
            Slot surface{};LocalReference surfaceLifetime{surface};
            auto* image=NewGraphicsSurfaceAt40C2E0(services.common.surfaces,services.common.strings);
            ConstructGraphicsSurfaceReferenceAt451500(surface,image,1);auto* cached=surface.controlAt08;
            image=Resolve<GraphicsSurfaceNativeBody>(surface);
            if(image->CreateAt415010(services.common.surfaces,Sub(rect.right,rect.left),Sub(rect.bottom,rect.top),1,1))
            {
                image=Resolve<GraphicsSurfaceNativeBody>(surface);image->originXAt10=rect.left;image->originYAt14=rect.top;
                foundLifetime.Captured(foundControl);
                Slot preview;CreateGuiStaticFromSurfaceAt505570(preview,&body.common,slot+33,Copy(surface),services.common.surfaces);
                LocalReference previewLifetime{preview};
                Slot copied=Copy(preview);
                {
                    LocalReference copiedLifetime{copied};auto* control=copied.controlAt08;
                    if(body.common.childrenAt0C.end==body.common.childrenAt0C.capacity)
                    {GrowGuiControlChildrenAt4D93B0(body.common.childrenAt0C,body.common.childrenAt0C.end,copied);control=copied.controlAt08;}
                    else AppendGuiControlChildReference(body.common.childrenAt0C,copied);
                    copiedLifetime.Captured(control);
                }
                if(body.surfacesAt8C.end==body.surfacesAt8C.capacity)
                {GrowGuiControlSurfaceReferencesAt4540A0(body.surfacesAt8C,body.surfacesAt8C.end,surface);cached=surface.controlAt08;}
                else AppendGuiControlSurfaceReference(body.surfacesAt8C,{surface.baseAt00,0,cached});
                previewLifetime.Reload();
            }
            else foundLifetime.Captured(foundControl);
            // 原Create FALSE释放本项但继续，不能抛整页失败或追加一个空占位。
            surfaceLifetime.Captured(cached);
        }
        else foundLifetime.Reload(false);
    }
}
void MakeTabPaintGroup(Slot& child,SaveLoadPageNativeServices& services)
{
    auto* control=Resolve<GuiControlNativeBody50h>(child);
    Slot base=Copy(*control->resourcesAt28.begin);LocalReference baseLife{base};
    Slot paint;ConstructGuiControlResourceReferenceAt4D76A0(paint,NewGuiControlResourceAt5047DD(),1);
    LocalReference paintLife{paint};
    auto* source=Resolve<GuiControlResourceNativeBody38h>(base);
    auto* target=Resolve<GuiControlResourceNativeBody38h>(paint);
    target->combinedRectAt28=source->combinedRectAt28;
    // 底图只读保存在第二组；四种字形状态独立surface，不能因底图同名合并绘制状态。
    for(const int state:{0,3,5,1})
    {
        const auto index=source->slotIndicesAt00[state];
        if(index<0)continue;
        auto* original=Resolve<GraphicsSurfaceNativeBody>(source->surfacesAt1C.begin[index]);
        Slot image{};LocalReference imageLife{image};
        AdoptGraphicsSurfaceAt407A80(image,NewGraphicsSurfaceAt40C2E0(services.common.surfaces,services.common.strings));
        auto* surface=Resolve<GraphicsSurfaceNativeBody>(image);
        if(!surface->CreateAt415010(services.common.surfaces,original->widthAt08,original->heightAt0C,1,1))throw 1;
        surface->originXAt10=original->originXAt10;surface->originYAt14=original->originYAt14;
        const auto next=GuiControlSurfaceReferenceCount(target->surfacesAt1C);
        AppendGuiControlSurfaceReference(target->surfacesAt1C,image);
        target->slotIndicesAt00[state]=next;
        if(state==3)target->slotIndicesAt00[4]=next;
    }
    AppendGuiControlResourceReference(control->resourcesAt28,base);
    ReleaseYtlReference(control->resourcesAt28.begin[0]);
    CopyGuiControlResourceReference(control->resourcesAt28.begin[0],paint);
}
void CreateTabs(Body& body,bool load,SaveLoadPageNativeServices& services)
{
    const auto& profile=CurrentSaveLoadResourceProfile();
    for(int i=0;i<profile.catalog.visiblePageTabs;++i)
    {
        auto descriptor=profile.page.tabButtons[load?1:0];
        descriptor.common.eventAt00=SaveLoadSelectTabFirstEvent+i;
        Slot tab;CreateGuiButtonAt5041D0(tab,&body.common,descriptor,services.common.loader,services.common.surfaces,services.common.strings);
        LocalReference tabLife{tab};MakeTabPaintGroup(tab,services);
        const auto position=profile.page.tabPositions[i];MoveButton(tab,position.x,position.y);
        tabLife.armed=false;AppendButton(body,tab);
        descriptor=profile.page.tabDeleteButtons[load?1:0];
        descriptor.common.eventAt00=SaveLoadDeleteTabFirstEvent+i;
        const auto offset=*descriptor.common.positionAt20;
        Slot deletion;CreateGuiButtonAt5041D0(deletion,&body.common,descriptor,services.common.loader,services.common.surfaces,services.common.strings);
        LocalReference deletionLife{deletion};MoveButton(deletion,position.x+offset.x,position.y+offset.y);
        deletionLife.armed=false;AppendButton(body,deletion);
    }
}
void CreateHover(Body& body,SaveLoadPageNativeServices& services)
{
    const auto& skin=CurrentSaveLoadResourceProfile().page.slots;
    if(skin.labelKind!=SaveLoadLabelKind::SharedHover)return;
    AdoptGraphicsSurfaceAt407A80(body.hoverSurface,NewGraphicsSurfaceAt40C2E0(services.common.surfaces,services.common.strings));
    auto* surface=Resolve<GraphicsSurfaceNativeBody>(body.hoverSurface);
    const auto rectangle=skin.hoverBounds;
    if(!surface->CreateAt415010(services.common.surfaces,rectangle.right-rectangle.left,rectangle.bottom-rectangle.top,1,1))throw 1;
    surface->originXAt10=rectangle.left;surface->originYAt14=rectangle.top;
    Slot child;CreateGuiStaticFromSurfaceAt505570(child,&body.common,1002,Copy(body.hoverSurface),services.common.surfaces);
    AppendStatic(body,child);
}
Common* Construct(void* raw,Common* parent,std::int32_t id,bool load,SaveLoadPageNativeServices& services)
{
    SaveLoadPageNativeScope scope(services);
    auto* common=ConstructSaveLoadPageSecondNativeAt4EF5E0(raw,parent,id);auto& body=Page(common);
    struct Unwind{Body& body;bool armed=true;~Unwind() noexcept{if(armed)DestroySaveLoadPageSecondNativeAt4F9470(&body);}} unwind{body};
    common->vptrAt00=&(load?SaveLoadPage01NativeVtable():SaveLoadPage00NativeVtable()).inheritedGui35;
    auto* managerRaw=::operator new(0x0C);
    auto* manager=managerRaw?::new(managerRaw)GuiClickedEventReferences{}:nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(body.managerAt70,manager);
    RECT bounds{};
    const auto& profile=CurrentSaveLoadResourceProfile();
    const auto& skin=profile.page.slots;
    Slot background;CreateGuiStaticAt505800(background,common,profile.page.backgrounds[load?id-2:0],
        services.common.loader,services.common.surfaces,services.common.strings);
    AppendStatic(body,background);
    // 原Start.exe 4EE493 cmp esi,3 / 4EE496 jne 4EE91A同时跳过页码49和顶部按钮50..52。
    if(!load||id==3)
    {
        if(profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)CreateTabs(body,load,services);
        else CreatePageNumberCanvas(body,services);
    }
        for(const auto& original:profile.page.page[load?1:0].buttons)
        {
            auto descriptor=original;
            descriptor.common.eventAt00=SaveLoadConsumerEvent(SaveLoadEventScope::Page,descriptor.common.eventAt00);
            const bool rolling=load&&id!=3;
            if(rolling!=(descriptor.common.eventAt00==12))continue;
            Slot child;CreateGuiButtonAt5041D0(child,common,descriptor,services.common.loader,services.common.surfaces,services.common.strings);
            AppendButton(body,child);
        }
    const auto positions=skin.positions;
    for(std::int32_t slot=0;slot<profile.catalog.slotsPerPage;++slot)
    {
        auto descriptor=skin.fileButtons[load?1:0];
        if(profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)
        {
            // 旧根先铺每槽底图，再画独立target；目标图透明区不能丢掉原底图。
            const auto* normal=descriptor.common.namesAt04[0];
            GuiStaticDescriptorNative24h base{2000+slot,{normal,nullptr,nullptr,normal,normal,nullptr,nullptr},&positions[slot]};
            Slot image;CreateGuiStaticAt505800(image,common,base,services.common.loader,services.common.surfaces,services.common.strings);
            AppendStatic(body,image);descriptor.common.namesAt04[0]=nullptr;
        }
        Slot child;CreateGuiButtonAt5041D0(child,common,descriptor,
            services.common.loader,services.common.surfaces,services.common.strings);
        LocalReference lifetime{child};Resolve<GuiControlNativeBody50h>(child)->eventAt04=slot+25;
        MoveButton(child,positions[slot].x,positions[slot].y);
        RECT rect;auto* selected=Resolve<GuiControlNativeBody50h>(child);
        ::UnionRect(&bounds,&bounds,selected->vptrAt00->rectAt3C(selected,&rect));
        lifetime.armed=false;AppendButton(body,child);
    }
    CreateHover(body,services);
    CreatePreviewCanvases(body,services);
    // 原4EF156 cmp [ebp-8Ch],3 / 4EF15D jne 4EF4BF：Quick/Auto不创建或注册删除按钮41..48。
    if(!load||id==3)
    {
        for(std::int32_t slot=0;slot<profile.catalog.slotsPerPage;++slot)
        {
            Slot child;CreateGuiButtonAt5041D0(child,common,skin.deleteButtons[load?1:0],
                services.common.loader,services.common.surfaces,services.common.strings);
            LocalReference lifetime{child};Resolve<GuiControlNativeBody50h>(child)->eventAt04=slot+41;
            MoveButton(child,Add(positions[slot].x,skin.deleteOffset.x),Add(positions[slot].y,skin.deleteOffset.y));
            lifetime.armed=false;AppendButton(body,child);
        }
    }
    for(const auto& navigation:profile.page.page[load?1:0].navigation)
    {
        std::vector<GuiDialogNavigationMapping> pairs(navigation.pairs.begin(),navigation.pairs.end());
        for(auto& pair:pairs)
        {
            // 12/13只在子控件表接入时转换，父事件循环继续使用现代动作编号。
            if(pair.sourceEventValue>=0)pair.sourceEventValue=SaveLoadConsumerEvent(SaveLoadEventScope::Page,pair.sourceEventValue);
            pair.targetEventValue=SaveLoadConsumerEvent(SaveLoadEventScope::Page,pair.targetEventValue);
        }
        CreateAndAppendGuiNavigation(common->navigationAt24,navigation.input,pairs.data());
    }
    body.boundsAt60=bounds;
    if(!load)body.pageAt7C=CaptureSave(services)->regularSelectedPageAt0C;
    else if(id==3)body.pageAt7C=CaptureSave(services)->regularDisplayedPageAt10;
    else body.pageAt7C=0;
    unwind.armed=false;return common;
}
}
GuiDialogNativeBody5Ch* ConstructSaveLoadPage00NativeAt4ED190(void* raw,Common* parent,std::int32_t id,SaveLoadPageNativeServices& services)
{return Construct(raw,parent,id,false,services);}
GuiDialogNativeBody5Ch* ConstructSaveLoadPage01NativeAt4EE390(void* raw,Common* parent,std::int32_t id,SaveLoadPageNativeServices& services)
{return Construct(raw,parent,id,true,services);}
}
