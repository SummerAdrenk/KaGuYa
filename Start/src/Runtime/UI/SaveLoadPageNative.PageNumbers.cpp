#include "SaveLoadPageNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveFileCatalog.h>
#include <algorithm>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::SaveLoadPageNativeDetail
{
namespace
{
std::uint32_t Count(const GuiControlSurfaceReferences& values) noexcept
{
    return std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(
        reinterpret_cast<std::uintptr_t>(values.end)-reinterpret_cast<std::uintptr_t>(values.begin)))/12);
}
std::uint32_t PositionCount(const SaveLoadPagePositionsNative& values) noexcept
{return std::uint32_t(reinterpret_cast<std::uintptr_t>(values.end)-reinterpret_cast<std::uintptr_t>(values.begin))/8u;}
void DrawNumber(Body& body,std::int32_t number,std::uint32_t position,SaveLoadPageNativeServices& services)
{
    for(std::uint32_t digit=0;digit<3;++digit)
    {
        const auto remainder=std::uint32_t(number%10);number/=10;
        auto& parent=Parent(&body.common);
        Slot selected{};
        if(remainder<Count(parent.pageDigitsAtF8))selected=Copy(parent.pageDigitsAtF8.begin[remainder]);
        LocalReference lifetime{selected};auto* const cached=selected.controlAt08;
        if(selected.baseAt00)
        {
            POINT coordinates{};auto& positions=Parent(&body.common).pagePositionsAt114;
            if(position+digit<PositionCount(positions))coordinates=positions.begin[position+digit];
            auto* source=Resolve<GraphicsSurfaceNativeBody>(selected);
            auto* target=Resolve<GraphicsSurfaceNativeBody>(body.canvasAt80);
            const auto destination=target->handleAt04;
            if(destination)
            {
                const auto image=source->handleAt04;
                if(image)(void)services.drawing.SurfaceBlitRaw(destination,image,coordinates.x,coordinates.y,nullptr,nullptr,nullptr,0);
            }
        }
        lifetime.Captured(cached);
    }
}
void DrawTabs(Body& body,int page,SaveLoadPageNativeServices& services)
{
    const auto& profile=CurrentSaveLoadResourceProfile();
    auto* catalog=CaptureSave(services);
    const auto count=static_cast<int>(catalog->RegularPages().size());
    const auto visible=profile.catalog.visiblePageTabs;
    if(page<body.firstTabIndex)body.firstTabIndex=page;
    else if(page>=body.firstTabIndex+visible)body.firstTabIndex=page-visible+1;
    body.firstTabIndex=std::clamp(body.firstTabIndex,0,std::max(0,count-visible));
    const auto load=body.common.vptrAt00->idAt0C(&body.common)!=2;
    for(int i=0;i<visible;++i)
    {
        const auto index=body.firstTabIndex+i;
        Slot reference;body.common.vptrAt00->findChildAt14(&body.common,&reference,SaveLoadSelectTabFirstEvent+i);
        LocalReference referenceLife{reference};
        auto* child=Resolve<GuiControlNativeBody50h>(reference);
        child->vptrAt00->writeEnabledAt4C(child,index<count);
        if(index<count)
        {
            const auto number=SaveFilePhysicalPageNumber(*catalog,index);
            auto* paint=Resolve<GuiControlResourceNativeBody38h>(child->resourcesAt28.begin[0]);
            auto* base=Resolve<GuiControlResourceNativeBody38h>(child->resourcesAt28.begin[1]);
            for(const int state:{0,3,5,1})
            {
                const auto targetIndex=paint->slotIndicesAt00[state];
                const auto sourceIndex=base->slotIndicesAt00[state];
                if(targetIndex<0||sourceIndex<0)continue;
                auto* destination=Resolve<GraphicsSurfaceNativeBody>(paint->surfacesAt1C.begin[targetIndex]);
                auto* background=Resolve<GraphicsSurfaceNativeBody>(base->surfacesAt1C.begin[sourceIndex]);
                const auto handle=destination->handleAt04;
                if(!handle)continue;
                services.drawing.SurfaceClearRaw(handle,0,nullptr);
                services.drawing.SurfaceBlitRaw(handle,background->handleAt04,0,0,nullptr,nullptr,nullptr,0);
                const auto digitState=state==3?1:state==5?2:state==1&&profile.page.digitStates==4?3:0;
                int divisor=1000;
                for(const auto point:profile.page.digitPositions)
                {
                    const auto digit=number/divisor%10;divisor/=10;
                    const auto glyphIndex=(load?profile.page.digitStates*10:0)+digitState*10+digit;
                    const auto& vector=Parent(&body.common).pageDigitsAtF8;
                    if(glyphIndex<0||glyphIndex>=GuiControlSurfaceReferenceCount(vector))throw std::logic_error("tab digit outside profile table");
                    auto* glyph=Resolve<GraphicsSurfaceNativeBody>(vector.begin[glyphIndex]);
                    services.drawing.SurfaceBlitRaw(handle,glyph->handleAt04,point.x,point.y,nullptr,nullptr,nullptr,0);
                }
            }
            child->vptrAt00->writeStateAt44(child,index!=page);
        }
        Slot deletion;body.common.vptrAt00->findChildAt14(&body.common,&deletion,SaveLoadDeleteTabFirstEvent+i);
        LocalReference deletionLife{deletion};
        auto* close=Resolve<GuiControlNativeBody50h>(deletion);
        close->vptrAt00->writeEnabledAt4C(close,index<count);
    }
}
}
void DrawSaveLoadPageNumbers(Body& body,std::int32_t page,SaveLoadPageNativeServices& services)
{
    if(CurrentSaveLoadResourceProfile().catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)
    {DrawTabs(body,page,services);return;}
    auto* canvas=Resolve<GraphicsSurfaceNativeBody>(body.canvasAt80);
    const auto handle=canvas->handleAt04;
    if(handle)services.drawing.SurfaceClearRaw(handle,canvas->clearColorAt20,nullptr);
    DrawNumber(body,SaveFilePhysicalPageNumber(*CaptureSave(services),page),0,services);
    DrawNumber(body,CurrentSaveLoadResourceProfile().catalog.physicalPageLimit,3,services);
}
void WriteSaveLoadPageButtons(Common& body,std::int32_t page)
{
    const auto& profile=CurrentSaveLoadResourceProfile();
    const auto count=static_cast<int>(CaptureSave(RequireSaveLoadPageNativeServices())->RegularPages().size());
    const auto first=profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages?Page(&body).firstTabIndex:page;
    const auto width=profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages?profile.catalog.visiblePageTabs:1;
    Slot previous;body.vptrAt00->findChildAt14(&body,&previous,50);LocalReference previousLifetime{previous};
    if(previous.baseAt00)
    {
        auto* selected=Resolve<GuiControlNativeBody50h>(previous);selected->vptrAt00->writeStateAt44(selected,first>0);
    }
    previousLifetime.Reload(false);
    Slot next;body.vptrAt00->findChildAt14(&body,&next,51);LocalReference nextLifetime{next};
    if(next.baseAt00)
    {
        auto* selected=Resolve<GuiControlNativeBody50h>(next);selected->vptrAt00->writeStateAt44(selected,first+width<count);
    }
    nextLifetime.Reload();
}
void NotifySaveLoadPage(Common& body,SaveLoadPageNativeServices& services)
{
    auto& source=services.common.source;
    const auto height=source.CaptureEnvironmentAt4180C0()->heightAt1C;
    const auto width=source.CaptureEnvironmentAt4180C0()->widthAt18;
    // height和width来自两次环境捕获，RECT按值进入+64，不能改用page bounds。
    body.vptrAt00->invalidateAt64(&body,{0,0,std::bit_cast<LONG>(width),std::bit_cast<LONG>(height)});
}
}
