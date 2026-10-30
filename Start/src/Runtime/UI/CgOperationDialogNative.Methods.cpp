#include "CgOperationDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace CgOperationNativeDetail
{
namespace
{
Common* __fastcall DeleteButtons(Common* common, void*, std::uint32_t flags) noexcept
{
    DestroyButtons(*reinterpret_cast<Buttons*>(common));
    if (flags & 1u) ::operator delete(common, std::size_t{0x94});
    return common;
}
Common* __fastcall DeletePopup(Common* common, void*, std::uint32_t flags) noexcept
{
    DestroyPopup(*reinterpret_cast<Popup*>(common));
    if (flags & 1u) ::operator delete(common, std::size_t{0x9C});
    return common;
}
Common* __fastcall DeleteDialog(Common* common, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteCgOperationDialogNativeAt4FBEE0(reinterpret_cast<Dialog*>(common), flags)); }
std::int32_t __fastcall One(Common*, void*) { return 1; } // 508BE0/508BF0
std::int32_t __fastcall Zero(Common*, void*) { return 0; } // 508C00
std::int32_t __fastcall ButtonsResult(Common* common, void*) { return reinterpret_cast<Buttons*>(common)->resultAt90; }
std::int32_t __fastcall PopupResult(Common* common, void*) { return reinterpret_cast<Popup*>(common)->resultAt78; }
std::int32_t __fastcall DialogResult(Common* common, void*) { return reinterpret_cast<Dialog*>(common)->resultAt78; }
RECT* __fastcall ButtonsBounds(Common* common, void*, RECT* output)
{ *output = reinterpret_cast<Buttons*>(common)->boundsAt80; return output; }
RECT* __fastcall PopupBounds(Common* common, void*, RECT* output)
{ *output = reinterpret_cast<Popup*>(common)->boundsAt68; return output; }
void __fastcall BeginDialog(Common* common, void*)
{ BeginCgOperationDialogNativeAt50BFD0(*reinterpret_cast<Dialog*>(common)); }
std::int32_t __fastcall PollDialog(Common* common, void*)
{ return PollCgOperationDialogNativeAt52C2D0(*reinterpret_cast<Dialog*>(common)); }
}

std::int32_t PollEventsAt50FDB0(GuiClickedEventReferences& events, std::int32_t clear)
{
    auto* const end = events.end;
    for (auto* position = events.begin; position != end; ++position)
    {
        if (!Resolve<GuiClickedEventNativeBody>(*position)->clickedFlagAt04) continue;
        const auto result = Resolve<GuiClickedEventNativeBody>(*position)->eventValueAt08;
        if (clear)
        {
            auto* event = Resolve<GuiClickedEventNativeBody>(*position);
            event->vptrAt00->clearAt04(event);
        }
        return result;
    }
    return -1;
}
void __fastcall BeginButtons(Common* common, void*)
{
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(reinterpret_cast<Buttons*>(common)->eventsAt5C));
    common->vptrAt00->writeEnabledAt24(common, 1);
    common->vptrAt00->resetChildrenAt68(common);
}
std::int32_t __fastcall PollButtons(Common* common, void*)
{
    auto& body = *reinterpret_cast<Buttons*>(common);
    body.resultAt90 = -1;
    common->vptrAt00->processInputAt54(common);
    body.resultAt90 = PollEventsAt50FDB0(*Resolve<GuiClickedEventReferences>(body.eventsAt5C), 1);
    return 1;
}
std::int32_t __fastcall PollPopup(Common* common, void*)
{
    auto& body = *reinterpret_cast<Popup*>(common);
    body.resultAt78 = -1;
    common->vptrAt00->processInputAt54(common);
    body.resultAt78 = PollEventsAt50FDB0(*Resolve<GuiClickedEventReferences>(body.eventsAt5C), 1);
    return 1;
}
void __fastcall ResetPopupInput(Common* common, void*)
{
    reinterpret_cast<Popup*>(common)->keyboardLatchAt98 = 0;
    common->vptrAt00->setActiveAt30(common, nullptr);
    common->inputLatchAt30 = 0;
}
std::int32_t __fastcall ActivePopup(Common* common, void*)
{
    auto& body = *reinterpret_cast<Popup*>(common);
    const auto type = common->activeAt4C->typeAt08;
    if (type != 0 && type != 2) return 0;
    const auto previous = body.keyboardLatchAt98;
    if (!previous)
    {
        if (!common->vptrAt00->pressedAt70(common, 5)) return 0;
        body.keyboardLatchAt98 = 1;
        auto* child = common->activeAt4C;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!common->vptrAt00->heldAt6C(common, 5))
    {
        Slot event = Copy(common->activeAt4C->eventAt3C);
        RefGuard eventGuard{event};
        auto* captured = event.controlAt08;
        if (event.baseAt00)
        {
            auto* receiver = Resolve<GuiClickedEventNativeBody>(event);
            receiver->vptrAt00->invokeAt00(receiver);
        }
        body.keyboardLatchAt98 = 0;
        eventGuard.Captured(captured);
    }
    return previous;
}
void __fastcall BeginPopup(Common* common, void*)
{
    auto& body = *reinterpret_cast<Popup*>(common);
    common->vptrAt00->writeEnabledAt24(common, 0);
    common->vptrAt00->resetInputAt3C(common);
    common->vptrAt00->resetChildrenAt68(common);
    body.pageAt94 = -1;
    for (std::int32_t event = PreviousPageEvent; event != PreviousPageEvent+2; ++event)
    {
        Slot child;
        common->vptrAt00->findChildAt14(common, &child, event);
        RefGuard childGuard{child};
        if (child.baseAt00)
        {
            auto* receiver = Resolve<GuiControlNativeBody50h>(child);
            const auto count = GuiControlSurfaceReferenceCount(body.pagesAt88);
            if(Profile().pagePolicy==CgOperationPagePolicy::ClampedButtonsAndWheel)
            {
                receiver->vptrAt00->writeEnabledAt4C(receiver,count>1);
                receiver->vptrAt00->writeStateAt44(receiver,1);
            }
            else receiver->vptrAt00->writeStateAt44(receiver, static_cast<std::uint32_t>(count) > 1u);
        }
        childGuard.Reload();
    }
}

const Vtable36& ButtonsMethods() noexcept
{
    static const auto table = []() noexcept
    {
        Vtable36 result{GuiDialogBaseVtable(), {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define CG_SLOT(name, entry) result.common.name = reinterpret_cast<decltype(result.common.name)>(&entry)
        CG_SLOT(deletingDestructorAt04, DeleteButtons);
        CG_SLOT(beginAt44, BeginButtons);
        CG_SLOT(pollAt48, PollButtons);
        CG_SLOT(endAt4C, One);
        CG_SLOT(resultAt50, ButtonsResult);
        CG_SLOT(compositeAt5C, CompositeButtons);
        CG_SLOT(drawAt60, DrawButtons);
#undef CG_SLOT
        result.boundsAt8C = reinterpret_cast<decltype(result.boundsAt8C)>(&ButtonsBounds);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return result;
    }();
    return table;
}
const Vtable36& PopupMethods() noexcept
{
    static const auto table = []() noexcept
    {
        Vtable36 result{GuiDialogBaseVtable(), {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define CG_SLOT(name, entry) result.common.name = reinterpret_cast<decltype(result.common.name)>(&entry)
        CG_SLOT(deletingDestructorAt04, DeletePopup);
        CG_SLOT(activeInputAt38, ActivePopup);
        CG_SLOT(resetInputAt3C, ResetPopupInput);
        CG_SLOT(beginAt44, BeginPopup);
        CG_SLOT(pollAt48, PollPopup);
        CG_SLOT(endAt4C, Zero);
        CG_SLOT(resultAt50, PopupResult);
#undef CG_SLOT
        result.boundsAt8C = reinterpret_cast<decltype(result.boundsAt8C)>(&PopupBounds);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return result;
    }();
    return table;
}
}

const GuiDialogIntoNativeVtable37& CgOperationDialogNativeMethods() noexcept
{
    using namespace CgOperationNativeDetail;
    static const auto table = []() noexcept
    {
        auto result = GuiDialogIntoBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define CG_SLOT(name, entry) result.common.name = reinterpret_cast<decltype(result.common.name)>(&entry)
        CG_SLOT(deletingDestructorAt04, DeleteDialog);
        CG_SLOT(beginAt44, BeginDialog);
        CG_SLOT(pollAt48, PollDialog);
        CG_SLOT(endAt4C, One);
        CG_SLOT(resultAt50, DialogResult);
#undef CG_SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return result;
    }();
    return table;
}
void BeginCgOperationDialogNativeAt50BFD0(CgOperationDialogNativeBodyA8h& body)
{
    using namespace CgOperationNativeDetail;
    auto& common = body.into.common;
    common.vptrAt00->writeEnabledAt24(&common, 1);
    for (auto* position = body.into.dialogsAt5C.begin; position != body.into.dialogsAt5C.end; ++position)
    {
        auto* child = Resolve<Common>(*position);
        child->vptrAt00->beginAt44(child);
    }
    Slot primary;
    reinterpret_cast<const GuiDialogIntoNativeVtable37*>(common.vptrAt00)->findDialogAt90(&common, &primary, 0);
    RefGuard primaryGuard{primary};
    if (primary.baseAt00) ApplySelectionAt537010(*Resolve<Buttons>(primary), body.selectionAt7C);
    primaryGuard.Reload();
}
void ResetCgOperationDialogNativeAt502110(CgOperationDialogNativeBodyA8h& body)
{
    using namespace CgOperationNativeDetail;
    body.availabilityAt80.end = body.availabilityAt80.begin;
    auto& common = body.into.common;
    Slot popup;
    reinterpret_cast<const GuiDialogIntoNativeVtable37*>(common.vptrAt00)->findDialogAt90(&common, &popup, 1);
    RefGuard popupGuard{popup};
    if (popup.baseAt00) ResetPopupAt5021C0(*Resolve<Popup>(popup));
    popupGuard.Reload();
}
void AppendCgOperationAvailabilityNativeAt4FEA00(CgOperationDialogNativeBodyA8h& body, std::int32_t value)
{
    using namespace CgOperationNativeDetail;
    AppendAvailability(body.availabilityAt80, value);
    auto& common = body.into.common;
    Slot popup;
    reinterpret_cast<const GuiDialogIntoNativeVtable37*>(common.vptrAt00)->findDialogAt90(&common, &popup, 1);
    RefGuard popupGuard{popup};
    if (popup.baseAt00)
    {
        const auto count = Sub(static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(body.availabilityAt80.end)),
            static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(body.availabilityAt80.begin))) / 4;
        AppendPopupItemAt4FEAE0(*Resolve<Popup>(popup), Add(count, -1), value);
    }
    popupGuard.Reload();
}
}
