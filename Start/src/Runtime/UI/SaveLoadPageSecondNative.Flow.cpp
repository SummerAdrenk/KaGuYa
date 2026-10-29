#include "SaveLoadPageSecondNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/SaveLoadPageNative.h>
#include <Runtime/SaveLoadPageSlotNative.h>
#include <algorithm>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageSecondNativeDetail;
std::int32_t DrainOne(GuiClickedEventReferences* manager)
{
    auto* at = Read(manager->begin);
    auto* const end = Read(manager->end);
    for (; at != end; ++at)
    {
        const auto base = Read(at->baseAt00); // 原ESI在flag/value/clear之间保持base，bias/control重读。
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->clickedFlagAt04)) continue;
        const auto event = Read(ResolveWithBase<GuiClickedEventNativeBody>(*at, base)->eventValueAt08);
        auto* selected = ResolveWithBase<GuiClickedEventNativeBody>(*at, base);
        selected->vptrAt00->clearAt04(selected);
        return event;
    }
    return -1;
}
bool Ready(Common* p, std::int32_t id, std::int32_t index)
{
    Slot page{};
    LocalReference lifetime{page}; // 52FA2F state0；source抛出也释放已形成的空Ref。
    auto& s = RequireSaveLoadPageSecondNativeServices().source;
    const Slot* source;
    if (id == 4 || id == 5)
    {
        auto* environment = s.CaptureEnvironmentAt4180C0();
        auto* save = s.SaveAt417990(environment, 6);
        source = id == 4 ? &save->quickPageAt2C : &save->autoPageAt38;
        index=SaveFileRollingSlot(*save,id==5,index);
    }
    else
    {
        const auto capturedPage = Read(Page(p).pageAt7C); // 52FA64在4180C0/417990之前捕获。
        auto* environment = s.CaptureEnvironmentAt4180C0();
        auto* save = s.SaveAt417990(environment, 6);
        source = Read(save->regularPagesAt20.begin) + capturedPage;
    }
    CopyGuiClickedEventReference(page, *source); // 4FA0D0旧空槽，完整bias增持。
    auto* const cached = page.controlAt08; // 原ESI；正常释放不在payload之后重读local。
    auto* selected = Resolve<SaveFileNativePage>(page);
    const auto& item = Read(selected->itemsAt04.begin)[index];
    const bool ready = Read(Resolve<SaveFileNativeItem>(item)->timeAt00.baseAt00) != 0;
    lifetime.Captured(cached); // 异常展开则LocalReference走原reload释放。
    return ready;
}
}
void BeginSaveLoadPageSecondNativeAt50EB50(Common* p)
{
    ClearGuiClickedEventFlagsAt532200(*Manager(Page(p)));
    p->vptrAt00->resetChildrenAt68(p);
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->vptrAt00->resetInputAt3C(p);
    const auto* table = Table(p);
    table->changePageAt8C(p, Read(Page(p).pageAt7C), 0, 1);
}
std::int32_t PollSaveLoadPageSecondNativeAt52F8B0(Common* p)
{
    Page(p).resultAt5C = -1;
    p->vptrAt00->processInputAt54(p);
    if(CurrentSaveLoadResourceProfile().page.slots.labelKind==SaveLoadLabelKind::SharedHover)
        RefreshSaveLoadPageHoverNative(Page(p),RequireSaveLoadPageNativeServices().slots);
    for (;;)
    {
        const auto event = DrainOne(Manager(Page(p)));
        const auto slots=CurrentSaveLoadResourceProfile().catalog.slotsPerPage;
        if (event >= 25 && event < 25+slots)
        {
            const auto id = p->vptrAt00->idAt0C(p);
            if (id == 2 || Ready(p, id, event - 25)) Page(p).resultAt5C = event;
        }
        else if ((event >= 41 && event < 41+slots) || event == 52 || event==12 ||
            event==SaveLoadAddPageEvent || (event>=SaveLoadDeleteTabFirstEvent &&
            event<SaveLoadDeleteTabFirstEvent+CurrentSaveLoadResourceProfile().catalog.visiblePageTabs)) Page(p).resultAt5C = event;
        else if (event == 50 || event == 51)
        {
            auto target=Add(Read(Page(p).pageAt7C),event==50?-1:1);
            const auto& profile=CurrentSaveLoadResourceProfile();
            auto& source=RequireSaveLoadPageSecondNativeServices().source;
            auto* catalog=source.SaveAt417990(source.CaptureEnvironmentAt4180C0(),6);
            const auto count=static_cast<int>(catalog->RegularPages().size());
            if(profile.catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)
                target=Page(p).firstTabIndex+(event==50?-1:profile.catalog.visiblePageTabs);
            const auto* table = Table(p);
            table->changePageAt8C(p,std::clamp(target,0,count-1),1,0);
        }
        else if(event>=SaveLoadSelectTabFirstEvent&&event<SaveLoadSelectTabFirstEvent+CurrentSaveLoadResourceProfile().catalog.visiblePageTabs)
        {
            const auto target=Page(p).firstTabIndex+event-SaveLoadSelectTabFirstEvent;
            Table(p)->changePageAt8C(p,target,1,0);
        }
        if (event <= -1) return 1; // 负值事件也已clear；-2不是继续扫的事件。
    }
}
std::int32_t EndSaveLoadPageSecondNativeAt5094C0(Common*) noexcept { return 1; }
std::int32_t ResultSaveLoadPageSecondNativeAt50ABC0(Common* p) noexcept { return Read(Page(p).resultAt5C); }
void DrawSaveLoadPageBaseNativeAt524960(Common* p, const RECT* clip)
{
    if (!Read(p->enabledAt38)) return;
    for (auto* at = Read(p->childrenAt0C.begin); at != Read(p->childrenAt0C.end); ++at)
    {
        auto* child = Resolve<GuiControlNativeBody50h>(*at);
        child->vptrAt00->drawAt0C(child, clip);
    }
}
void CompositeSaveLoadPageBaseNativeAt5249C0(Common* p, Slot owned, const RECT* clip)
{
    LocalReference argument{owned};
    if (Read(p->enabledAt38))
        for (auto* at = Read(p->childrenAt0C.begin); at != Read(p->childrenAt0C.end); ++at)
        {
            auto* child = Resolve<GuiControlNativeBody50h>(*at);
            Slot copied = Copy(owned);
            child->vptrAt00->compositeAt10(child, copied, clip);
        }
    argument.Reload(false);
}
}
