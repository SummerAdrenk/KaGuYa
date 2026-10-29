#include "SaveLoadSystemPageNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadSystemPageNativeDetail;
// 5045C0在本页两组的compiled kind均为1；保留fresh返回槽、上转Ref与临时释放。
void CreateChild(Slot& out, Common& parent, const GuiButtonDescriptorNative2Ch& d,
    SaveLoadSystemPageNativeServices& s)
{
    out = {};
    LocalReference result{out};
    FreshReference button;
    CreateGuiButtonAt5041D0(button.slot, &parent, d, s.loader, s.surfaces, s.strings);
    LocalReference temporary{button.slot};
    CopyGuiClickedEventReference(out, button.slot); // 4FA010的旧out在入口已清。
    temporary.Reload();
    result.active = false;
}
void Populate(Body& b, const GuiButtonDescriptorNative2Ch& d,
    SaveLoadSystemPageNativeServices& s, RECT& combined)
{
    FreshReference child;
    CreateChild(child.slot, b.common, d, s);
    LocalReference childLifetime{child.slot}; // EH state1，工厂成功才有资格。
    auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
    RECT rectangle;
    const auto* returned = selected->vptrAt00->rectAt3C(selected, &rectangle);
    ::UnionRect(&combined, &combined, returned);

    FreshReference event;
    // 4F0D99..4F0E2D：正文分配之后才解析child并读+04；复用同序原核。
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    LocalReference eventLifetime{event.slot}; // state2。
    auto* capturedEvent = event.slot.controlAt08;
    Slot binding{event.slot.baseAt00, 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        LocalReference bindingLifetime{binding}; // state3 -> 正常state4。
        selected = Resolve<GuiControlNativeBody50h>(child.slot);
        auto& target = selected->eventAt3C;
        if (&target != &binding)
        {
            ReleaseWithoutClearing(target);
            target.baseAt00 = event.slot.baseAt00; // 旧payload/control回调后再取base。
            target.biasAt04 = 0;
            target.controlAt08 = capturedEvent;
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
        }
        selected->eventEnabledAt48 = 1;
        bindingLifetime.Captured(capturedEvent);
    }
    Slot upcast = Copy(child.slot);
    {
        LocalReference upcastLifetime{upcast}; // state5 -> 正常state6。
        auto* captured = upcast.controlAt08;
        auto& children = b.common.childrenAt0C;
        if (children.end == children.capacity)
        {
            GrowGuiControlChildrenAt4D93B0(children, children.end, upcast);
            captured = upcast.controlAt08;
        }
        else AppendGuiControlChildReference(children, upcast);
        upcastLifetime.Captured(captured);
    }
    // state7是独立的事件上转临时Ref，不能省成直接借用event调用vector追加。
    Slot managerArgument{event.slot.baseAt00, 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        LocalReference managerLifetime{managerArgument};
        auto* captured = capturedEvent;
        AppendPopupClickedEventReferenceAt4EA97F(*Manager(b), managerArgument, captured);
        managerLifetime.Captured(captured); // state8。
    }
    eventLifetime.Captured(capturedEvent); // state9，使用原ESI。
    childLifetime.Reload(); // state10，payload后重读局部child.control。
}
}
GuiDialogNativeBody5Ch* ConstructSaveLoadSystemPageNativeAt4F0C80(void* raw80,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, SaveLoadSystemPageNativeServices& s)
{
    SaveLoadSystemPageNativeScope constructionScope{s};
    auto* b = ::new(raw80) SaveLoadSystemPageNativeBody80h;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->managerAt70 = {};
    struct Unwind final
    {
        Body* p; bool active = true;
        ~Unwind() noexcept { if (active) DestroySaveLoadSystemPageNativeAt4F93F0(p); }
    } unwind{b};
    b->common.vptrAt00 = &SaveLoadSystemPageNativeVtable();
    auto* rawManager = ::operator new(0x0C);
    auto* manager = rawManager ? ::new(rawManager) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(b->managerAt70, manager);
    RECT combined{};
    const auto& group = CurrentSaveLoadResourceProfile().page.system[id==0?0:1];
    for (const auto& original : group.buttons)
    {
        auto descriptor=original;
        descriptor.common.eventAt00=SaveLoadConsumerEvent(SaveLoadEventScope::System,descriptor.common.eventAt00);
        Populate(*b, descriptor, s, combined);
    }
    b->boundsAt60 = combined;
    unwind.active = false;
    return &b->common;
}
}
