#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
namespace
{
void Create(Slot& out, Common* parent, SettingPage03ControlKind kind, const void* d)
{
    auto& s = RequireSettingPage03NativeServices();
    switch (kind)
    {
    case SettingPage03ControlKind::Static:
        CreateGuiStaticAt505800(out, parent, *static_cast<const GuiStaticDescriptorNative24h*>(d), s.loader, s.surfaces, s.strings); break;
    case SettingPage03ControlKind::Button:
        CreateGuiButtonAt5041D0(out, parent, *static_cast<const GuiButtonDescriptorNative2Ch*>(d), s.loader, s.surfaces, s.strings); break;
    case SettingPage03ControlKind::Slider:
        CreateGuiSliderAt5052C0(out, parent, *static_cast<const GuiSliderDescriptorNative6Ch*>(d), s.loader, s.surfaces, s.strings); break;
    case SettingPage03ControlKind::CheckBox:
    {
        // 正式5043B0内核，仅复用工厂，不读取SaveLoad业务/modern模型。
        SaveLoadMessageBoxNativeChildFactoriesRuntime factory({s.loader, s.surfaces, s.strings});
        factory.CreateAt5043B0(&out, parent, d); break;
    }
    default: throw 1;
    }
}
}
void Populate(Common& parent, Slot& manager, SettingPage03ControlKind kind, const void* d, RECT& bounds, bool generic)
{
    Fresh child;
    if (generic)
    {
        child.slot = {}; RefLife output{child.slot}; Fresh typed;
        Create(typed.slot, &parent, kind, d); RefLife temporary{typed.slot};
        ReleaseYtlReference(child.slot); CopyGuiClickedEventReference(child.slot, typed.slot);
        temporary.Normal(); output.active = false;
    }
    else Create(child.slot, &parent, kind, d);
    RefLife childLife{child.slot};
    auto* selected = Resolve<Control>(child.slot);
    RECT rectangle; auto* returned = CTable(selected).rectAt3C(selected, &rectangle);
    ::UnionRect(&bounds, &bounds, returned);
    Fresh event;
    CreateToolClickedEventAt4E2F5A(event.slot, child.slot);
    RefLife eventLife{event.slot}; auto* capturedEvent = Read(event.slot.controlAt08);
    Slot binding{Read(event.slot.baseAt00), 0, capturedEvent};
    if (capturedEvent) ++capturedEvent->referenceCountAt04;
    {
        RefLife life{binding}; selected = Resolve<Control>(child.slot);
        auto& target = selected->eventAt3C;
        if (&target != &binding)
        {
            ReleaseOld(target); target.baseAt00 = Read(event.slot.baseAt00); target.biasAt04 = 0; target.controlAt08 = capturedEvent;
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
        AppendPopupClickedEventReferenceAt4EA97F(*Resolve<GuiClickedEventReferences>(manager), argument, captured);
        life.Captured(captured);
    }
    eventLife.Captured(capturedEvent); childLife.Normal(true);
}
std::int32_t PollManager(Slot& slot)
{
    auto* manager = Resolve<GuiClickedEventReferences>(slot);
    auto* p = Read(manager->begin); auto* end = Read(manager->end);
    for (; p != end; ++p)
    {
        const auto base = Read(p->baseAt00);
        if (!Read(ResolveBase<GuiClickedEventNativeBody>(*p, base)->clickedFlagAt04)) continue;
        const auto id = Read(ResolveBase<GuiClickedEventNativeBody>(*p, base)->eventValueAt08);
        auto* event = ResolveBase<GuiClickedEventNativeBody>(*p, base);
        Read(event->vptrAt00)->clearAt04(event); return id;
    }
    return -1;
}
void __fastcall BeginBlock(Common* p, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(Field<Slot>(p, 0x74)));
    Read(p->vptrAt00)->resetChildrenAt68(p); Table(p).loadAt90(p);
}
std::int32_t __fastcall EndBlock(Common* p, void*) { Table(p).storeAt94(p); return 1; }
RECT* __fastcall BlockBounds(Common* p, void*, RECT* out) { *out = Field<RECT>(p, 0x64); return out; }
}
