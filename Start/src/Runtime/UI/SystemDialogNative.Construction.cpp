#include "SystemDialogNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SystemDialogNativeDetail;
void Populate(Body& b, const GuiButtonDescriptorNative2Ch& descriptor, SystemDialogNativeServices& s)
{
    FreshReference child;
    CreateGuiButtonAt5041D0(child.slot, &b.common, descriptor, s.loader, s.surfaces, s.strings);
    LocalReference childLifetime{child.slot}; // EH state1：工厂成功返回才有资格。
    FreshReference event;
    // 4F4266后才重解child并读+04；new18失败不得替原回收孤立new0C正文。
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    LocalReference eventLifetime{event.slot}; // state2。
    auto* capturedEvent = event.slot.controlAt08;
    Slot binding{event.slot.baseAt00, 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        LocalReference bindingLifetime{binding}; // state3 -> state2，正常state4终止。
        auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
        auto& target = selected->eventAt3C;
        if (&target != &binding)
        {
            ReleaseWithoutClearing(target);
            target.baseAt00 = event.slot.baseAt00; // 4F4365：旧release返回之后才取base。
            target.biasAt04 = 0;
            target.controlAt08 = capturedEvent;
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
        }
        selected->eventEnabledAt48 = 1;
        bindingLifetime.Captured(capturedEvent);
    }
    Slot upcast = Copy(child.slot);
    {
        LocalReference upcastLifetime{upcast}; // state5 -> state2；state6正常终止。
        auto* captured = upcast.controlAt08;
        if (b.common.childrenAt0C.end == b.common.childrenAt0C.capacity)
        {
            GrowGuiControlChildrenAt4D93B0(b.common.childrenAt0C, b.common.childrenAt0C.end, upcast);
            captured = upcast.controlAt08;
        }
        else AppendGuiControlChildReference(b.common.childrenAt0C, upcast);
        upcastLifetime.Captured(captured);
    }
    // 快路使用原ESI且bias=0；增长路4D9620结束才更新ESI。state7 -> state1。
    AppendPopupClickedEventReferenceAt4EA97F(b.eventsAt74, event.slot, capturedEvent);
    eventLifetime.Captured(capturedEvent);
    childLifetime.Reload(); // state8 -> state0：重读child槽control。
}
}
SystemDialogNativeBody88h* ConstructSystemDialogNativeAt4F41A0(void* storage)
{
    auto& s = RequireSystemDialogNativeServices();
    auto* b = ::new(storage) SystemDialogNativeBody88h;
    ConstructGuiDialogAt4E0BE0(&b->common, nullptr, -1);
    b->backgroundAt5C = {};
    b->eventsAt74 = {};
    struct Unwind final
    { Body* p; bool active = true; ~Unwind() noexcept { if (active) DestroySystemDialogNativeAt4F80D0(p); } } unwind{b};
    b->common.vptrAt00 = &SystemDialogNativeVtable().common;
    b->fieldAt80 = 1;
    b->suppressOnceAt84 = 0;
    FreshReference frame;
    // 原工厂直接写按值参数槽；4FF020负责消费，不为caller添一份临时Ref保护。
    CreateGuiStaticAt505800(frame.slot, &b->common, SystemDialogNativeStaticSample18(),
        s.loader, s.surfaces, s.strings);
    GuiDialogBaseVtable().appendChildAt10(&b->common, frame.slot);
    for (const auto& d : SystemDialogNativeButtonsSample18()) Populate(*b, d, s);
    // 原在四组按钮全部完成之后才以GameInput替换NullInput。
    AdoptGuiInputAt536EA0(b->common.inputAt18, NewGameGuiInputAt4EAAED());
    unwind.active = false;
    return b;
}
GuiDialogNativeBody5Ch* ConstructSystemDialogAt4F41A0(void* storage)
{ return &ConstructSystemDialogNativeAt4F41A0(storage)->common; }
}
