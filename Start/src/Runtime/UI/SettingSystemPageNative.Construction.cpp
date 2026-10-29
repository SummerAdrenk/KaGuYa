#include "SettingSystemPageNative.Internal.h"
#include <Runtime/SettingShellResourceProfile.h>
#include <Runtime/PopupNativeLifetime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingSystemPageNativeDetail;
void CreateChild(Slot& out, Common& parent, int index, SettingSystemPageNativeServices& s)
{
    // 共享页框使用本款static/button描述；typed返回后才取得临时Ref展开资格。
    out = {};
    LocalReference output{out};
    FreshReference typed;
    const auto& profile = CurrentSettingShellProfile();
    if (static_cast<std::size_t>(index) < profile.backgrounds.size())
        CreateGuiStaticAt505800(typed.slot, &parent, profile.backgrounds[index], s.loader, s.surfaces, s.strings);
    else
        CreateGuiButtonAt5041D0(typed.slot, &parent, profile.buttons[index - profile.backgrounds.size()], s.loader, s.surfaces, s.strings);
    LocalReference temporary{typed.slot};
    ReleaseYtlReference(out);
    CopyGuiClickedEventReference(out, typed.slot);
    temporary.Reload();
    output.active = false;
}
void Populate(Body& b, int index, SettingSystemPageNativeServices& s, RECT& bounds)
{
    FreshReference child;
    CreateChild(child.slot, b.common, index, s);
    LocalReference childLifetime{child.slot}; // state3，成功工厂返回后。
    auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
    RECT rect;
    auto* returned = Read(selected->vptrAt00)->rectAt3C(selected, &rect);
    ::UnionRect(&bounds, &bounds, returned);
    FreshReference event;
    // 4F389B new0C之后才重解child读+04；new18失败不补销毁孤立event。
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    LocalReference eventLifetime{event.slot}; // state4。
    auto* capturedEvent = Read(event.slot.controlAt08);
    Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        LocalReference bindingLifetime{binding}; // state5，正常state6终止。
        selected = Resolve<GuiControlNativeBody50h>(child.slot);
        auto& target = selected->eventAt3C;
        if (&target != &binding)
        {
            ReleaseWithoutClearing(target);
            target.baseAt00 = Read(event.slot.baseAt00);
            target.biasAt04 = 0; target.controlAt08 = capturedEvent;
            if (capturedEvent) ++capturedEvent->referenceCountAt04;
        }
        selected->eventEnabledAt48 = 1;
        bindingLifetime.Captured(capturedEvent);
    }
    Slot upcast = Copy(child.slot);
    {
        LocalReference lifetime{upcast}; // state7，正常state8终止。
        auto* captured = Read(upcast.controlAt08);
        auto& children = b.common.childrenAt0C;
        if (children.end == children.capacity)
        { GrowGuiControlChildrenAt4D93B0(children, children.end, upcast); captured = Read(upcast.controlAt08); }
        else AppendGuiControlChildReference(children, upcast);
        lifetime.Captured(captured);
    }
    Slot argument{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        LocalReference lifetime{argument}; // state9，正常state10终止。
        auto* captured = capturedEvent;
        AppendPopupClickedEventReferenceAt4EA97F(*Manager(b), argument, captured);
        lifetime.Captured(captured);
    }
    eventLifetime.Captured(capturedEvent); // state11 -> state3。
    childLifetime.Reload(); // state12 -> state2，payload后重读control。
}
}
SettingSystemPageNativeBody7Ch* ConstructSettingSystemPageNativeAt4F3740(void* raw, Common* parent, std::int32_t id)
{
    auto& s = RequireSettingSystemPageNativeServices();
    auto* b = ::new(raw) SettingSystemPageNativeBody7Ch;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->common.vptrAt00 = &SettingSystemPageNativeVtable();
    struct Unwind final
    {
        Body* body; bool manager = false, active = true;
        ~Unwind() noexcept
        { if (active) { if (manager) ReleaseYtlReference(body->managerAt70); DestroyGuiDialogAt4F7AC0(&body->common); } }
    } unwind{b}; // state0后才允许5Ch展开，绝不调用派生删除器。
    b->managerAt70 = {}; unwind.manager = true; // state1。
    auto* managerRaw = ::operator new(0x0C);
    auto* manager = managerRaw ? ::new(managerRaw) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(b->managerAt70, manager);
    GuiFactoryConstructionLifetime factory; // 原1字节factory及18h控制；state2。
    RECT bounds{};
    const auto& profile = CurrentSettingShellProfile();
    const auto count = profile.backgrounds.size() + profile.buttons.size();
    for (std::size_t index = 0; index != count; ++index) Populate(*b, static_cast<int>(index), s, bounds);
    b->boundsAt60 = bounds; // 4F3BB7：全部完成才一次写16字节。
    factory.Finish(); // state13正常缓存control尾。
    unwind.active = false;
    return b;
}
}
