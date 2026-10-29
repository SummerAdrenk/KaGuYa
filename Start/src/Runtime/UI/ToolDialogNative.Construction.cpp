#include <Runtime/ToolDialogNative.h>
#include <Runtime/PopupNativeLifetime.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
struct RefLifetime final
{
    Slot& slot;
    bool active = true;
    ~RefLifetime() noexcept { if (active) ReleaseYtlReference(slot); }
    void Captured(YtlReferenceControlRuntime* control, bool clear = true) noexcept
    {
        active = false;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
        if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
    }
    void Reload(bool checkAfterPayload = true) noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (!checkAfterPayload || slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        slot.controlAt08 = nullptr;
        slot.baseAt00 = 0;
    }
};
struct RawLifetime final
{
    void* body;
    std::size_t size;
    bool active = true;
    ~RawLifetime() noexcept { if (active) ::operator delete(body, size); }
};
Slot Copy(const Slot& source) noexcept
{
    Slot result;
    CopyGuiControlResourceReference(result, source);
    return result;
}

void CreateButton(Slot& output, void* parent, const GuiCheckBoxDescriptorNative2Ch& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    Slot group;
    LoadGuiControlResourceAt5047A0(group, descriptor.common.namesAt04.data(), loader, surfaces, strings);
    RefLifetime groupLifetime{group};
    if (!group.baseAt00) throw 1;
    auto* captured = group.controlAt08;
    if (const auto* point = descriptor.common.positionAt20)
    {
        const auto y = point->y;
        const auto x = point->x;
        MoveGuiControlResourceAt5086C0(Copy(group), x, y);
    }
    void* raw = ::operator new(0x5C);
    RawLifetime rawLifetime{raw, 0x5C};
    auto* button = raw ? ConstructGuiButtonAt4DA700(static_cast<GuiButtonNativeBody5Ch*>(raw),
        parent, descriptor.common.eventAt00, Copy(group)) : nullptr;
    rawLifetime.active = false;
    Slot child;
    ConstructToolButtonReferenceAt4E2C61(child, button);
    RefLifetime childLifetime{child};
    auto* control = child.controlAt08;
    button->common.childIdAt0C = descriptor.childIdAt28;
    reinterpret_cast<const GuiButtonNativeVtable29*>(button->common.vptrAt00)->setHitSlotAt60(
        &button->common, descriptor.hitSlotAt24);
    groupLifetime.Captured(captured);
    output = Copy(child);
    childLifetime.Captured(control);
}

void CreateCheckBox(Slot& output, void* parent, const ToolCheckDescriptorNative48h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    Slot normal;
    LoadGuiControlResourceAt5047A0(normal, descriptor.normalAt04.data(), loader, surfaces, strings);
    RefLifetime normalLifetime{normal};
    if (!normal.baseAt00) throw 1;
    Slot checked;
    LoadGuiControlResourceAt5047A0(checked, descriptor.checkedAt20.data(), loader, surfaces, strings);
    RefLifetime checkedLifetime{checked};
    if (!checked.baseAt00) throw 1;
    auto* normalControl = normal.controlAt08; // 4E297D后才捕获normal control。
    if (const auto* point = descriptor.positionAt3C)
    {
        const auto firstY = point->y;
        const auto firstX = point->x;
        MoveGuiControlResourceAt5086C0(Copy(normal), firstX, firstY);
        point = descriptor.positionAt3C;
        const auto secondY = point->y;
        const auto secondX = point->x;
        MoveGuiControlResourceAt5086C0(Copy(checked), secondX, secondY);
    }
    void* raw = ::operator new(0x60);
    RawLifetime rawLifetime{raw, 0x60};
    GuiCheckBoxNativeBody60h* body = nullptr;
    if (raw)
    {
        Slot checkedArgument = Copy(checked);
        Slot normalArgument{normal.baseAt00, normal.biasAt04, normalControl};
        if (normalControl) ++normalControl->referenceCountAt04;
        body = ConstructGuiCheckBoxAt4E0520(static_cast<GuiCheckBoxNativeBody60h*>(raw),
            parent, descriptor.eventAt00, normalArgument, checkedArgument);
    }
    rawLifetime.active = false;
    Slot child;
    ConstructGuiCheckBoxReferenceAt4D7710(child, body, 1);
    RefLifetime childLifetime{child};
    auto* receiver = ResolveGuiControl(child);
    receiver->childIdAt0C = descriptor.childIdAt44;
    receiver = ResolveGuiControl(child);
    reinterpret_cast<const GuiCheckBoxNativeVtable30*>(receiver->vptrAt00)->setHitSlotAt60(
        receiver, descriptor.hitSlotAt40);
    checkedLifetime.Reload(false);
    normalLifetime.Captured(normalControl);
    output = Copy(child);
    childLifetime.Reload();
}

struct ConstructionLifetime final
{
    ToolDialogNativeBody7Ch& body;
    bool active = true;
    ~ConstructionLifetime() noexcept
    {
        if (!active) return;
        DestroyGuiClickedEventReferencesAt4F6D10(body.eventsAt70);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
}

ToolDialogNativeBody7Ch* ConstructToolDialogNativeAt4E27F0(ToolDialogNativeBody7Ch& body,
    std::int32_t mode, const ToolDialogNativeProfile& profile,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    ConstructGuiDialogAt4E0BE0(&body.common, nullptr, -1);
    body.common.vptrAt00 = &ToolDialogNativeVtable();
    body.eventsAt70 = {};
    ConstructionLifetime construction{body};
    void* factoryBody = ::operator new(1);
    Slot factory;
    ConstructGuiFactoryReferenceAt4D7860(factory, factoryBody, 1);
    RefLifetime factoryLifetime{factory};
    const auto* groups = mode == 0 ? profile.normalGroupsAt606DD4 : profile.replayGroupsAt606FA0;
    RECT bounds{};
    for (int groupIndex = 0; groupIndex != 5; ++groupIndex)
    {
        const auto& group = groups[groupIndex];
        for (std::int32_t index = 0; index < group.countAt04; ++index)
        {
            Slot child{};
            RefLifetime childLifetime{child};
            switch (group.kindAt00)
            {
            case 0:
                CreateGuiStaticAt505800(child, &body.common,
                    static_cast<const GuiStaticDescriptorNative24h*>(group.recordsAt08)[index], loader, surfaces, strings);
                break;
            case 1:
                CreateButton(child, &body.common,
                    static_cast<const GuiCheckBoxDescriptorNative2Ch*>(group.recordsAt08)[index], loader, surfaces, strings);
                break;
            case 2:
                CreateCheckBox(child, &body.common,
                    static_cast<const ToolCheckDescriptorNative48h*>(group.recordsAt08)[index], loader, surfaces, strings);
                break;
            default: throw 1;
            }
            auto* childControl = child.controlAt08;
            auto* receiver = ResolveGuiControl(child);
            RECT output;
            const auto* rect = receiver->vptrAt00->rectAt3C(receiver, &output);
            ::UnionRect(&bounds, &bounds, rect);
            Slot event;
            CreateToolClickedEventAt4E2F5A(event, child);
            RefLifetime eventLifetime{event};
            auto* eventControl = event.controlAt08;
            Slot binding{event.baseAt00, 0, eventControl};
            if (eventControl) ++eventControl->referenceCountAt04;
            BindPopupGuiControlEventAt4EA856(*ResolveGuiControl(child), binding, event);
            Slot upcast = Copy(child);
            RefLifetime upcastLifetime{upcast};
            AppendGuiControlChildReference(body.common.childrenAt0C, upcast);
            upcastLifetime.Reload(false);
            AppendPopupClickedEventReferenceAt4EA97F(body.eventsAt70, event, eventControl);
            eventLifetime.Captured(eventControl);
            childLifetime.Captured(childControl);
        }
    }
    body.boundsAt60 = bounds;
    AdoptGuiInputAt536EA0(body.common.inputAt18, NewGameGuiInputAt4EAAED());
    factoryLifetime.Captured(factory.controlAt08, false);
    construction.active = false;
    return &body;
}
}
