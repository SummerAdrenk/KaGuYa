#include "SettingPage02Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
template<class Create> void Populate(Common& parent, Slot& manager, RECT& combined,
    bool upcastFactory, Create&& create)
{
    Fresh child;
    if (upcastFactory)
    {
        child.slot = {}; RefLife output{child.slot}; Fresh typed;
        create(typed.slot); RefLife temporary{typed.slot};
        ReleaseYtlReference(child.slot); CopyGuiClickedEventReference(child.slot, typed.slot);
        temporary.Normal(); output.active = false;
    }
    else create(child.slot);
    RefLife childLife{child.slot};
    auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
    RECT rectangle;
    auto* returned = Read(selected->vptrAt00)->rectAt3C(selected, &rectangle);
    ::UnionRect(&combined, &combined, returned);
    Fresh event;
    // 所有原构造均在new0C之后重解child+04。失败的new18不取得事件Ref资格。
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    RefLife eventLife{event.slot};
    auto* capturedEvent = Read(event.slot.controlAt08);
    Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        RefLife life{binding}; selected = Resolve<GuiControlNativeBody50h>(child.slot);
        auto& target = selected->eventAt3C;
        if (&target != &binding)
        {
            ReleaseOld(target);
            target.baseAt00 = Read(event.slot.baseAt00); target.biasAt04 = 0; target.controlAt08 = capturedEvent;
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
        }
        selected->eventEnabledAt48 = 1; life.Captured(capturedEvent);
    }
    Slot upcast = Copy(child.slot);
    {
        RefLife life{upcast}; auto* captured = Read(upcast.controlAt08);
        auto& children = parent.childrenAt0C;
        if (children.end == children.capacity)
        { GrowGuiControlChildrenAt4D93B0(children, children.end, upcast); captured = Read(upcast.controlAt08); }
        else AppendGuiControlChildReference(children, upcast);
        life.Captured(captured);
    }
    Slot argument{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        RefLife life{argument}; auto* captured = capturedEvent;
        AppendPopupClickedEventReferenceAt4EA97F(*Resolve<GuiClickedEventReferences>(manager), argument, captured); life.Captured(captured);
    }
    eventLife.Captured(capturedEvent); childLife.Normal(true);
}
}
void PopulateButton(Common& parent, Slot& manager, const GuiButtonDescriptorNative2Ch& d,
    RECT& combined, bool genericFactory)
{
    auto& s = RequireSettingPage02NativeServices();
    Populate(parent, manager, combined, genericFactory, [&](Slot& out) {
        CreateGuiButtonAt5041D0(out, &parent, d, s.loader, s.surfaces, s.strings);
    });
}
void PopulateStatic(Common& parent, Slot& manager, const GuiStaticDescriptorNative24h& d, RECT& combined)
{
    auto& s = RequireSettingPage02NativeServices();
    Populate(parent, manager, combined, true, [&](Slot& out) {
        CreateGuiStaticAt505800(out, &parent, d, s.loader, s.surfaces, s.strings);
    });
}
void PopulateCheckbox(Common& parent, Slot& manager, const SaveLoadMessageBoxCheckBoxDescriptorNative48h& d, RECT& combined)
{
    auto& s = RequireSettingPage02NativeServices();
    Populate(parent, manager, combined, true, [&](Slot& out) {
        SaveLoadMessageBoxNativeChildFactoriesRuntime factory({s.loader, s.surfaces, s.strings});
        factory.CreateAt5043B0(&out, &parent, &d);
    });
}
}
