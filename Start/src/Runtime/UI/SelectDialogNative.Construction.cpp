#include <Runtime/SelectDialogNative.h>
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
    void Finish() noexcept
    {
        active = false;
        auto* control = slot.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        }
        slot.controlAt08 = nullptr; slot.baseAt00 = 0;
    }
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
};
struct ConstructionLifetime final
{
    SelectDialogNativeBody88h& body;
    bool active = true;
    ~ConstructionLifetime() noexcept
    {
        if (!active) return;
        DestroyGuiClickedEventReferencesAt4F6D10(body.eventsAt7C);
        DestroyGuiDialogAt4F7AC0(&body.common);
    }
};
}
SelectDialogNativeBody88h* ConstructSelectDialogNativeAt4F17E0(SelectDialogNativeBody88h& body,
    const SelectDialogNativeProfile& profile, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope surfaceScope(surfaces);
    ConstructGuiDialogAt4E0BE0(&body.common, nullptr, -1);
    body.common.vptrAt00 = &SelectDialogNativeVtable();
    body.boundsAt6C = {};
    body.eventsAt7C = {};
    ConstructionLifetime construction{body};
    void* raw = ::operator new(1);
    Slot factory;
    ConstructGuiFactoryReferenceAt4D7860(factory, raw, 1);
    RefLifetime factoryLifetime{factory};
    const auto* placement = profile.fiveButtonsAt607098;
    for (std::size_t ordinal = 0; ordinal != profile.placementCount; ++ordinal, ++placement)
    {
        Slot child;
        CreateGuiButtonAt5041D0(child, &body.common, *placement->descriptorAt00, loader, surfaces, strings);
        RefLifetime childLifetime{child};
        ResolveGuiControl(child)->eventAt04 = placement->idAt0C;
        auto* receiver = ResolveGuiControl(child);
        const auto y = placement->yAt08;
        const auto x = placement->xAt04;
        receiver->xAt34 = x; receiver->yAt38 = y;
        Slot event;
        // 4F1935事件保存循环序号ESI，不是placement.id或child+04。
        const auto eventValue = static_cast<std::int32_t>(ordinal);
        CreatePopupClickedEventAt4EA7B3(event, eventValue);
        RefLifetime eventLifetime{event};
        auto* eventControl = event.controlAt08;
        Slot binding{event.baseAt00, 0, eventControl};
        if (eventControl) ++eventControl->referenceCountAt04;
        BindPopupGuiControlEventAt4EA856(*ResolveGuiControl(child), binding, event);
        Slot upcast;
        CopyGuiClickedEventReference(upcast, child);
        RefLifetime upcastLifetime{upcast};
        AppendGuiControlChildReference(body.common.childrenAt0C, upcast);
        upcastLifetime.Captured(upcast.controlAt08);
        AppendPopupClickedEventReferenceAt4EA97F(body.eventsAt7C, event, eventControl);
        eventLifetime.Captured(eventControl);
        childLifetime.Finish();
    }
    AdoptGuiInputAt536EA0(body.common.inputAt18, NewGameGuiInputAt4EAAED());
    factoryLifetime.Captured(factory.controlAt08, false);
    construction.active = false;
    return &body;
}
}
