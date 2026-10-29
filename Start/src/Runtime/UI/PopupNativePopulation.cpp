#include <Runtime/PopupNativePopulation.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
struct RefUnwind final
{
    Slot& value;
    bool active = true;
    ~RefUnwind() noexcept { if (active) ReleaseYtlReference(value); }
    void FinishCurrent() noexcept { active = false; ReleaseYtlReference(value); }
    void FinishCaptured(YtlReferenceControlRuntime* control, bool clear = true) noexcept
    {
        active = false;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
        if (clear)
        {
            value.controlAt08 = nullptr;
            value.baseAt00 = 0;
        }
    }
};
struct RawCheckUnwind final
{
    void* body;
    bool active = true;
    ~RawCheckUnwind() noexcept { if (active) ::operator delete(body, std::size_t{0x60}); }
};
Slot Copy(const Slot& source) noexcept
{
    Slot value;
    CopyGuiControlResourceReference(value, source);
    return value;
}
void StampResources(Slot& checked, const Slot& normalStamp, const Slot& offStamp,
    POINT position, IGuiControlGraphicsSource& graphics)
{
    // 4EA2D6每轮从当前组重新取长度，4EA314找第一个映射到唯一资源的状态槽。
    for (std::int32_t index = 0;
        index < GuiControlSurfaceReferenceCount(RequireGuiControlResource(checked).surfacesAt1C); ++index)
    {
        for (int state = 0; state != 7; ++state)
        {
            if (RequireGuiControlResource(checked).slotIndicesAt00[state] != index) continue;
            Slot destination = Copy(RequireGuiControlResource(checked).surfacesAt1C.begin[index]);
            RefUnwind destinationLifetime{destination};
            auto* const capturedControl = destination.controlAt08;
            if (destination.baseAt00)
            {
                auto* source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(
                    state == 1 ? offStamp : normalStamp);
                auto* target = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(destination);
                const auto targetHandle = target->handleAt04;
                if (targetHandle)
                {
                    if (!source) throw std::logic_error("4EA3FB null Popup stamp body");
                    const auto sourceHandle = source->handleAt04;
                    if (sourceHandle)
                        static_cast<void>(graphics.CompositionAt4182F0().BlitSurface(
                            targetHandle, sourceHandle, position.x, position.y,
                            nullptr, nullptr, nullptr, 0));
                }
            }
            destinationLifetime.FinishCaptured(capturedControl);
            break;
        }
    }
}
void CreateCheckBox(void* parent, GuiControlChildrenStorage& children, GuiClickedEventReferences& events,
    const GuiCheckBoxDescriptorNative2Ch& descriptor, const Slot& normalStamp, const Slot& offStamp,
    POINT stampPosition, IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IGraphicsSurfaceCopyApi& copy, IGuiControlGraphicsSource& graphics, IRuntimeStringManagerSource& strings)
{
    Slot normal;
    LoadGuiControlResourceAt5047A0(normal, descriptor.common.namesAt04.data(), loader, surfaces, strings);
    RefUnwind normalLifetime{normal};
    Slot checked;
    CopyGuiControlResourceAt502630(checked, RequireGuiControlResource(normal), surfaces, copy, strings);
    RefUnwind checkedLifetime{checked};
    StampResources(checked, normalStamp, offStamp, stampPosition, graphics);
    const auto normalBase = normal.baseAt00;
    auto* const normalControl = normal.controlAt08;
    if (const auto* point = descriptor.common.positionAt20)
    {
        const auto y = point->y;
        const auto x = point->x;
        Slot argument{normalBase, normal.biasAt04, normalControl};
        if (normalControl) ++normalControl->referenceCountAt04;
        MoveGuiControlResourceAt5086C0(argument, x, y);
        point = descriptor.common.positionAt20; // 4EA678重新读取描述表指针。
        if (!point) throw std::logic_error("4EA67E null Popup position after first move");
        const auto secondY = point->y;
        const auto secondX = point->x;
        MoveGuiControlResourceAt5086C0(Copy(checked), secondX, secondY);
    }
    void* allocation = ::operator new(0x60);
    RawCheckUnwind rawLifetime{allocation};
    GuiCheckBoxNativeBody60h* childBody = nullptr;
    if (allocation)
    {
        // 4EA6E1先增持checked，再用仍在ESI/EBX中的normal base/control。
        Slot checkedArgument = Copy(checked);
        Slot normalArgument{normalBase, normal.biasAt04, normalControl};
        if (normalControl) ++normalControl->referenceCountAt04;
        childBody = ConstructGuiCheckBoxAt4E0520(static_cast<GuiCheckBoxNativeBody60h*>(allocation),
            parent, descriptor.common.eventAt00, normalArgument, checkedArgument);
    }
    rawLifetime.active = false;
    Slot child;
    ConstructPopupCheckBoxReferenceAt4EA73C(child, childBody);
    auto* const childControl = child.controlAt08;
    if (!childBody) throw std::logic_error("4EA7A1 null Popup CheckBox body");
    childBody->common.childIdAt0C = descriptor.childIdAt28;
    RefUnwind childLifetime{child};
    const auto* table = reinterpret_cast<const GuiCheckBoxNativeVtable30*>(childBody->common.vptrAt00);
    table->setHitSlotAt60(&childBody->common, descriptor.hitSlotAt24);

    Slot event;
    CreatePopupClickedEventAt4EA7B3(event, childBody->common.eventAt04);
    RefUnwind eventLifetime{event};
    auto* eventControl = event.controlAt08;
    Slot binding{event.baseAt00, 0, eventControl};
    if (eventControl) ++eventControl->referenceCountAt04;
    BindPopupGuiControlEventAt4EA856(*reinterpret_cast<GuiControlNativeBody50h*>(child.baseAt00), binding, event);

    Slot upcast{child.baseAt00, 0, childControl};
    if (childControl) ++childControl->referenceCountAt04;
    RefUnwind upcastLifetime{upcast};
    auto* upcastControl = childControl;
    if (children.end != children.capacity)
        AppendGuiControlChildReference(children, upcast);
    else
    {
        GrowGuiControlChildrenAt4D93B0(children, children.end, upcast);
        upcastControl = upcast.controlAt08;
    }
    upcastLifetime.FinishCaptured(upcastControl);
    AppendPopupClickedEventReferenceAt4EA97F(events, event, eventControl);
    eventLifetime.FinishCaptured(eventControl);
    childLifetime.FinishCaptured(childControl);
    checkedLifetime.FinishCurrent();
    normalLifetime.FinishCaptured(normal.controlAt08);
}
}
bool LoadPopupStampAt480000(Slot& output, void* receiver, const char* name,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces, IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    if (!output.baseAt00)
        AdoptGraphicsSurfaceAt407A80(output, NewGraphicsSurfaceAt40C2E0(surfaces, strings));
    auto* body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(output);
    if (!body) throw std::logic_error("480087 null Popup Surface body");
    return loader.LoadIntoAt4800A0(receiver, *body, name);
}
void PopulatePopupNativeControlsAt4EA0D1(void* parent,
    GuiControlChildrenStorage& children, GuiClickedEventReferences& events,
    std::span<const GuiStaticDescriptorNative24h, 12> statics,
    std::span<const GuiCheckBoxDescriptorNative2Ch, 9> checkBoxes,
    const PopupMenuCheckStampData& stamps, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces, IGraphicsSurfaceCopyApi& copy, IGuiControlGraphicsSource& graphics,
    IRuntimeStringManagerSource& strings)
{
    GraphicsSurfaceNativeScope scope(surfaces);
    for (const auto& descriptor : statics)
    {
        Slot child;
        CreateGuiStaticAt505800(child, parent, descriptor, loader, surfaces, strings);
        RefUnwind childLifetime{child};
        AppendGuiControlChildReference(children, child);
        childLifetime.FinishCurrent();
    }
    Slot normalStamp{};
    RefUnwind normalStampLifetime{normalStamp};
    void* receiver = loader.CaptureLoadReceiverAt413BE0();
    if (!LoadPopupStampAt480000(normalStamp, receiver, stamps.normalNameCp932.data(), loader, surfaces, strings))
        throw 1;
    Slot offStamp{};
    RefUnwind offStampLifetime{offStamp};
    receiver = loader.CaptureLoadReceiverAt413BE0();
    if (!LoadPopupStampAt480000(offStamp, receiver, stamps.slot1NameCp932.data(), loader, surfaces, strings))
        throw 1;
    for (const auto& descriptor : checkBoxes)
        CreateCheckBox(parent, children, events, descriptor, normalStamp, offStamp, stamps.origin,
            loader, surfaces, copy, graphics, strings);
    offStampLifetime.FinishCaptured(offStamp.controlAt08, false);
    normalStampLifetime.FinishCaptured(normalStamp.controlAt08, false);
}
}
