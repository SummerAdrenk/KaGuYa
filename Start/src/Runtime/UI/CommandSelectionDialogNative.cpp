#include <Runtime/CommandSelectionNative.h>
#include <Runtime/PopupNativeLifetime.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = CommandSelectionDialogNativeBody;
using Base = GuiDialogNativeBody5Ch;
using Slot = YtlReferenceSlotRuntime;
struct LocalReference final
{
    Slot slot{};
    ~LocalReference() noexcept { ReleaseYtlReference(slot); }
};

void RefreshAvailability(Body& body)
{
    const auto apply = [&](int id, int value)
    {
        LocalReference child;
        body.common.vptrAt00->findChildAt14(&body.common, &child.slot, id);
        if (auto* control = ResolveGuiControl(child.slot))
            control->vptrAt00->writeStateAt44(control, value);
    };
    switch (body.profile->availability)
    {
    case CommandSelectionAvailability::All: return;
    case CommandSelectionAvailability::BitMask:
    {
        const auto mask = static_cast<std::uint32_t>(body.source->ReadAvailabilityParameter());
        for (int id = 0; id < 4; ++id) apply(id, static_cast<int>(mask & (1u << id)));
        return;
    }
    case CommandSelectionAvailability::LayoutTable:
    {
        const auto index = body.source->ReadAvailabilityParameter();
        if (index < 0 || static_cast<std::size_t>(index) >= body.profile->layouts.size())
            throw std::out_of_range("Command selection layout index outside audited table");
        for (const auto& state : body.profile->layouts[static_cast<std::size_t>(index)].controls)
            apply(state.event, state.state);
        return;
    }
    }
    throw std::logic_error("Unknown command selection availability strategy");
}
Base* __fastcall Delete(Base* base, void*, std::uint32_t flags) noexcept
{
    auto* body = reinterpret_cast<Body*>(base);
    DestroyCommandSelectionDialog(*body);
    if (flags & 1u) ::operator delete(body, sizeof(Body));
    return base;
}
void __fastcall ResetInput(Base* base, void*)
{
    reinterpret_cast<Body*>(base)->confirmLatch = 0;
    base->vptrAt00->setActiveAt30(base, nullptr);
    base->inputLatchAt30 = 0;
}
std::int32_t __fastcall Confirm(Base* base, void*)
{
    if (base->activeAt4C->typeAt08 != 0) return 0;
    auto& body = *reinterpret_cast<Body*>(base);
    const auto previous = body.confirmLatch;
    if (!previous)
    {
        if (!base->vptrAt00->pressedAt70(base, 5)) return 0;
        body.confirmLatch = 1;
        auto* child = base->activeAt4C;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!base->vptrAt00->heldAt6C(base, 5))
    {
        LocalReference event;
        CopyGuiClickedEventReference(event.slot, base->activeAt4C->eventAt3C);
        if (auto* value = ResolveGuiClickedEvent(event.slot)) value->vptrAt00->invokeAt00(value);
        body.confirmLatch = 0;
    }
    return previous;
}
void __fastcall Begin(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    base->vptrAt00->resetInputAt3C(base);
    base->activeAt4C = nullptr;
    ClearGuiClickedEventFlagsAt532200(body.events);
    base->vptrAt00->writeEnabledAt24(base, 1);
    RefreshAvailability(body);
    body.phase = 3;
    body.result = -1;
}
std::int32_t __fastcall Poll(Base* base, void*)
{
    auto& body = *reinterpret_cast<Body*>(base);
    switch (body.phase)
    {
    case 0: case 1: case 2: return 1;
    case 3:
    {
        base->vptrAt00->processInputAt54(base);
        const auto result = PollGuiClickedEventManagerNativeAt50FDB0(&body.events, 1);
        const auto count = body.profile->buttons.size() + body.profile->multiButtons.size();
        if (result >= 0 && static_cast<std::size_t>(result) < count)
        {
            body.result = body.profile->results.empty() ? result : body.profile->results[static_cast<std::size_t>(result)];
            body.phase = 4;
            body.source->PlayCommandCue(true);
        }
        return 1;
    }
    case 4:
    {
        auto* dirty = base->vptrAt00->dirtyAt00(base);
        dirty->vptrAt00->addRectAt08(dirty, &body.bounds);
        ClearGuiClickedEventFlagsAt532200(body.events);
        for (auto* entry = base->childrenAt0C.begin; entry != base->childrenAt0C.end; ++entry)
        {
            auto* child = ResolveGuiControl(*entry);
            child->vptrAt00->resetAt08(child);
        }
        RefreshAvailability(body);
        body.phase = -1;
        return 1;
    }
    default: return 0;
    }
}
std::int32_t __fastcall End(Base* base, void*)
{ base->vptrAt00->writeEnabledAt24(base, 0); return 1; }
void __fastcall ChildChanged(Base* base, void*, Slot ownedChild)
{
    LocalReference child{ownedChild};
    auto& body = *reinterpret_cast<Body*>(base);
    auto* value = ResolveGuiControl(child.slot);
    const auto count = body.profile->buttons.size() + body.profile->multiButtons.size();
    if (value->eventAt04 < 0 || static_cast<std::size_t>(value->eventAt04) >= count) return;
    const auto state = value->vptrAt00->resourceStateAt38(value);
    if ((state == 3 || state == 4)
        && value->eventAt04 != PollGuiClickedEventManagerNativeAt50FDB0(&body.events, 0))
        body.source->PlayCommandCue(false);
}
}

const GuiDialogNativeVtable35& CommandSelectionDialogMethods() noexcept
{
    static const auto methods = [] {
        auto value = GuiDialogBaseVtable();
#define COMMAND_SLOT(slot, function) value.slot = reinterpret_cast<decltype(value.slot)>(&function)
        COMMAND_SLOT(deletingDestructorAt04, Delete);
        COMMAND_SLOT(activeInputAt38, Confirm);
        COMMAND_SLOT(resetInputAt3C, ResetInput);
        COMMAND_SLOT(beginAt44, Begin);
        COMMAND_SLOT(pollAt48, Poll);
        COMMAND_SLOT(endAt4C, End);
        COMMAND_SLOT(childChangedAt88, ChildChanged);
#undef COMMAND_SLOT
        // 原+50是基类固定-1；结果由CmdSelect的扩展入口读取，不能套普通选项序号换算。
        return value;
    }();
    return methods;
}

CommandSelectionDialogNativeBody* ConstructCommandSelectionDialog(Body& body, CommandSelectionNativeServices& services)
{
    const GraphicsSurfaceNativeScope graphics(services.surfaces);
    ConstructGuiDialogAt4E0BE0(&body.common, nullptr, -1);
    body.common.vptrAt00 = &CommandSelectionDialogMethods();
    body.events = {};
    body.bounds = {};
    body.source = &services.source;
    body.profile = &services.profile;
    body.phase = 0; body.result = -1; body.confirmLatch = 0;
    try
    {
        const auto count = services.profile.buttons.size() + services.profile.multiButtons.size();
        if (!count || (!services.profile.results.empty() && services.profile.results.size() != count)
            || (services.profile.availability == CommandSelectionAvailability::BitMask && count != 4))
            throw std::invalid_argument("Inconsistent command selection resource profile");
        const auto append = [&](const LocalReference& child)
        {
            auto* control = ResolveGuiControl(child.slot);
            RECT bounds;
            const auto* rect = control->vptrAt00->rectAt3C(control, &bounds);
            (void)::UnionRect(&body.bounds, &body.bounds, rect);
            LocalReference event;
            CreatePopupClickedEventAt4EA7B3(event.slot, control->eventAt04);
            Slot binding;
            CopyGuiClickedEventReference(binding, event.slot);
            BindPopupGuiControlEventAt4EA856(*control, binding, event.slot);
            AppendGuiControlChildReference(body.common.childrenAt0C, child.slot);
            AppendGuiClickedEventReference(body.events, event.slot);
        };
        for (const auto& descriptor : services.profile.backgrounds)
        {
            LocalReference child;
            CreateGuiStaticAt505800(child.slot, &body.common, descriptor, services.resources, services.surfaces, services.strings);
            append(child);
        }
        for (const auto& descriptor : services.profile.buttons)
        {
            LocalReference child;
            CreateGuiButtonAt5041D0(child.slot, &body.common, descriptor, services.resources, services.surfaces, services.strings);
            append(child);
        }
        for (const auto& descriptor : services.profile.multiButtons)
        {
            LocalReference child;
            CreateGuiMultiCheckButtonNative(child.slot, &body.common, descriptor, services.resources, services.surfaces, services.strings);
            append(child);
        }
        AdoptGuiInputAt536EA0(body.common.inputAt18, NewGameGuiInputAt4EAAED());
        for (const auto& navigation : services.profile.navigation)
            CreateAndAppendGuiNavigation(body.common.navigationAt24, navigation.inputAt00, navigation.pairsAt04);
    }
    catch (...)
    {
        DestroyCommandSelectionDialog(body);
        throw;
    }
    return &body;
}
void DestroyCommandSelectionDialog(Body& body) noexcept
{
    DestroyGuiClickedEventReferencesAt4F6D10(body.events);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
}
