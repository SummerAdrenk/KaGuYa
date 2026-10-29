#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveLoadPageSecondNative.h>
#include <Runtime/SaveLoadPopupNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SaveLoadDialogNativeBody184h;using Common=GuiDialogNativeBody5Ch;using Slot=YtlReferenceSlotRuntime;
struct Temporary
{
    union{Slot slot;};bool armed=false;
    Temporary() noexcept{}
    ~Temporary() noexcept{if(armed)ReleaseYtlReference(slot);}
    void Find(Common& body,int id)
    {reinterpret_cast<const SaveLoadDialogNativeVtable37*>(body.vptrAt00)->findDialogAt90(&body,&slot,id);armed=true;}
    void FindControl(Common& body,int id){body.vptrAt00->findChildAt14(&body,&slot,id);armed=true;}
    Common* Dialog(){return ResolveEnvironmentObjectReference<Common>(slot);}
    void Finish() noexcept{armed=false;SaveLoadDialogNativeDetail::ReleaseWithoutClearing(slot);}
};
std::int32_t& Index(const Slot& slot,unsigned offset)
{return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(ResolveEnvironmentObjectReference<Common>(slot))+offset);}
void Invalidate(Body& body)
{auto* dirty=body.common.vptrAt00->dirtyAt00(&body.common);dirty->vptrAt00->addRectAt08(dirty,&body.boundsAt78);}
void ResetAndInvalidateControl(Body& body,Common* parent,int event)
{
    Temporary control;control.FindControl(*parent,event);
    if(control.slot.baseAt00)
    {
        auto* selected=ResolveGuiControl(control.slot);selected->vptrAt00->resetAt08(selected);
        selected=ResolveGuiControl(control.slot);const auto* table=body.common.vptrAt00;
        RECT scratch;auto* rectangle=selected->vptrAt00->rectAt3C(selected,&scratch);
        table->invalidateAt64(&body.common,*rectangle);
    }
    control.Finish();
}
void Rebuild(Body& body,ISaveLoadDialogNativePageFactory& factory)
{
    InitializeSaveLoadDialogPageNativeAt5330A0(body,body.pageModeAtE4,factory);
    OpenSaveLoadDialogChildrenNativeAt532560(body);Invalidate(body);
}
void EnterPopup(Body& body)
{
    auto& parent=body.common;
    {Temporary system;system.Find(parent,body.pageModeAtE4!=0);if(system.slot.baseAt00){auto* d=system.Dialog();d->vptrAt00->writeStateAt1C(d,0);}system.Finish();}
    int index=0,first=0;
    {Temporary page;page.Find(parent,2+(body.pageModeAtE4!=0));if(page.slot.baseAt00){auto* d=page.Dialog();d->vptrAt00->writeStateAt1C(d,0);index=Index(page.slot,0x7C);
        first=reinterpret_cast<SaveLoadPageSecondNativeBodyACh*>(d)->firstTabIndex;}page.Finish();}
    {Temporary popup;popup.Find(parent,6+(body.pageModeAtE4!=0));if(popup.slot.baseAt00){auto* d=popup.Dialog();d->vptrAt00->writeStateAt1C(d,1);
        d=popup.Dialog();d->vptrAt00->writeEnabledAt24(d,1);Index(popup.slot,0x7C)=index;
        if(CurrentSaveLoadResourceProfile().popup.tiledFrame)reinterpret_cast<SaveLoadPopupNativeBodyC0h*>(d)->firstPageAt80=first;
        d=popup.Dialog();d->vptrAt00->beginAt44(d);}popup.Finish();}
    Invalidate(body);
}
void LeavePopup(Body& body,int event)
{
    auto& parent=body.common;
    {Temporary system;system.Find(parent,body.pageModeAtE4!=0);if(system.slot.baseAt00){auto* d=system.Dialog();d->vptrAt00->writeStateAt1C(d,1);}system.Finish();}
    int index=0;
    {Temporary popup;popup.Find(parent,6+(body.pageModeAtE4!=0));if(popup.slot.baseAt00){auto* d=popup.Dialog();d->vptrAt00->writeStateAt1C(d,0);
        d=popup.Dialog();d->vptrAt00->writeEnabledAt24(d,0);if(event==54)index=Index(popup.slot,0x84);}popup.Finish();}
    {Temporary page;page.Find(parent,2+(body.pageModeAtE4!=0));if(page.slot.baseAt00){auto* d=page.Dialog();d->vptrAt00->writeStateAt1C(d,1);
        if(event==54){Index(page.slot,0x7C)=index;d=page.Dialog();d->vptrAt00->beginAt44(d);}}page.Finish();}
    Invalidate(body);
}
}
void ProcessSaveLoadDialogEventsNativeAt522350(Body& body,ISaveLoadDialogNativeCatalogSource& source,
    ISaveLoadDialogNativePageFactory& factory)
{
    for(auto* at=body.dialogsAt5C.begin;at!=body.dialogsAt5C.end;++at)
    {
        auto* child=ResolveEnvironmentObjectReference<Common>(*at);
        if(!child->vptrAt00->readEnabledAt28(child))continue;
        child=ResolveEnvironmentObjectReference<Common>(*at);
        if(!child->vptrAt00->readStateAt20(child))continue;
        child=ResolveEnvironmentObjectReference<Common>(*at);child->vptrAt00->pollAt48(child);
        child=ResolveEnvironmentObjectReference<Common>(*at);const auto event=child->vptrAt00->resultAt50(child);
        const auto& profile=CurrentSaveLoadResourceProfile();
        if(event==SaveLoadCloseApplicationEvent)
        {
            if(!profile.closeApplicationButton)throw std::logic_error("save/load close action is absent in this profile");
            source.RequestCloseApplication();return;
        }
        if(event==SaveLoadAddPageEvent)
        {
            auto* catalog=source.CaptureCatalogAt4180C0();
            const auto added=AddSaveFileCatalogPage(*catalog);
            if(added<0){source.ShowPageLimitMessage();return;}
            if(body.pageModeAtE4==0)catalog->regularSelectedPageAt0C=added;
            else catalog->regularDisplayedPageAt10=added;
            Temporary selected;selected.Find(body.common,2+(body.pageModeAtE4!=0));
            if(!selected.slot.baseAt00)throw std::logic_error("save/load regular page is not constructed");
            const auto* table=reinterpret_cast<const SaveLoadPageSecondNativeVtable36*>(selected.Dialog()->vptrAt00);
            table->changePageAt8C(selected.Dialog(),added,1,1);selected.Finish();return;
        }
        if(event>=SaveLoadDeleteTabFirstEvent&&event<SaveLoadDeleteTabFirstEvent+profile.catalog.visiblePageTabs)
        {
            auto* page=reinterpret_cast<SaveLoadPageSecondNativeBodyACh*>(child);
            // -1仍是当前整页；<=-2只为页签删除编码目标显示下标，不借用读档sourceIndex。
            body.slotIndexAt164=-2-(page->firstTabIndex+event-SaveLoadDeleteTabFirstEvent);
            body.phaseAt6C=6;body.transitionPhaseAt70=0;return;
        }
        if(event==11){body.phaseAt6C=2;body.transitionPhaseAt70=0;return;}
        if(event==10)
        {
            body.phaseAt6C=5;body.transitionPhaseAt70=0;
            ResetAndInvalidateControl(body,ResolveEnvironmentObjectReference<Common>(*at),10);return;
        }
        if(event==12)
        {
            bool allowed;
            if(body.pageModeAtE4==2)allowed=source.CaptureCatalogAt4180C0()->quickLatestItemAt04>=0;
            else if(body.pageModeAtE4==3)allowed=source.CaptureCatalogAt4180C0()->autoLatestItemAt08>=0;
            else
            {
                Temporary page;page.Find(body.common,2+(body.pageModeAtE4!=0));
                const auto index=Index(page.slot,0x7C);
                auto* catalog=source.CaptureCatalogAt4180C0();
                const auto address=reinterpret_cast<std::uintptr_t>(catalog->regularPagesAt20.begin)+std::uint32_t(index)*12u;
                allowed=profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages ||
                    ResolveEnvironmentObjectReference<SaveFileNativePage>(*reinterpret_cast<const Slot*>(address))->availabilityAt00!=0;
                page.Finish();
            }
            if(!allowed)continue;
            body.phaseAt6C=6;body.transitionPhaseAt70=0;body.slotIndexAt164=-1;
            ResetAndInvalidateControl(body,ResolveEnvironmentObjectReference<Common>(*at),12);return;
        }
        if(event>=41&&event<41+profile.catalog.slotsPerPage){body.phaseAt6C=6;body.slotIndexAt164=event-41;body.transitionPhaseAt70=0;return;}
        if(event>=25&&event<25+profile.catalog.slotsPerPage)
        {
            if(!body.pageModeAtE4){body.phaseAt6C=7;body.sourceIndexAt160=-3;}
            else
            {
                if(body.pageModeAtE4==2)body.sourceIndexAt160=-1;
                else if(body.pageModeAtE4==3)body.sourceIndexAt160=-2;
                else
                {
                    Temporary page;page.Find(body.common,3);
                    if(!page.slot.baseAt00){page.Finish();return;}
                    body.sourceIndexAt160=Index(page.slot,0x7C);page.Finish();
                }
                body.phaseAt6C=8;
            }
            body.transitionPhaseAt70=0;
            body.slotIndexAt164=(body.pageModeAtE4==2||body.pageModeAtE4==3)?
                SaveFileRollingSlot(*source.CaptureCatalogAt4180C0(),body.pageModeAtE4==3,event-25):event-25;
            return;
        }
        if(event==9){body.pageModeAtE4=std::bit_cast<std::int32_t>(source.CaptureCatalogAt4180C0()->regularPageStepAt14);Rebuild(body,factory);return;}
        if(event==13){body.pageModeAtE4=0;Rebuild(body,factory);return;}
        if(event>=14&&event<=16)
        {
            body.pageModeAtE4=event-13;
            source.CaptureCatalogAt4180C0()->regularPageStepAt14=std::uint32_t(body.pageModeAtE4);
            Rebuild(body,factory);return;
        }
        if(event==52)EnterPopup(body);
        else if(event==53||event==54)LeavePopup(body,event);
    }
}
}
