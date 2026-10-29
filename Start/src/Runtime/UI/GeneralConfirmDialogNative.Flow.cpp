#include "GeneralConfirmDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::GeneralConfirmNativeDetail
{
namespace
{
const GeneralConfirmDialogNativeVtable38& Table(Common* p) noexcept
{ return *reinterpret_cast<const GeneralConfirmDialogNativeVtable38*>(p->vptrAt00); }
void AddCapturedRect(Common* p)
{
    auto& slot = Dialog(p).capturedAt5C;
    const auto width = Surface(slot)->widthAt08;
    const auto height = Surface(slot)->heightAt0C;
    auto* surface = Surface(slot);
    const auto x = surface->originXAt10;
    const auto y = surface->originYAt14;
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}
}
void __fastcall BeginHook(Common*, void*) {} // 5086B0原nullsub，不是未实现占位。
void __fastcall EndHook(Common*, void*) {} // 51C100原nullsub，与E4的51C110分开。
std::int32_t __fastcall Result(Common* p, void*) { return Dialog(p).resultAt68; }
void __fastcall ResetInput(Common* p, void*)
{
    Dialog(p).actionFiveAt70 = 0;
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
std::int32_t __fastcall ActiveInput(Common* p, void*)
{
    if (p->activeAt4C->typeAt08 != 0) return 0;
    auto& body = Dialog(p);
    const auto previous = body.actionFiveAt70;
    if (!previous)
    {
        if (!p->vptrAt00->pressedAt70(p, 5)) return 0;
        auto* child = p->activeAt4C;
        body.actionFiveAt70 = 1;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot event;
        Copy(event, p->activeAt4C->eventAt3C);
        RefLifetime lifetime{event};
        if (event.baseAt00)
        {
            auto* selected = ResolveGuiClickedEvent(event);
            selected->vptrAt00->invokeAt00(selected);
        }
        body.actionFiveAt70 = 0;
        // 510C61直接调用4F64D0；正常与EH均重读槽，不用先前缓存control。
        lifetime.armed = false;
        ReleaseGuiClickedEventReferenceAt4F64D0(event);
    }
    return previous;
}
void __fastcall Begin(Common* p, void*)
{
    auto& body = Dialog(p);
    auto& deps = RequireGeneralConfirmDialogNativeServices();
    if (!body.capturedAt5C.baseAt00)
    {
        auto returned = deps.root.CaptureScreenAt413BE0();
        auto temporary = returned.Detach();
        RefLifetime lifetime{temporary};
        CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt5C, temporary);
        lifetime.Finish(); // 50CFCC..CFF5正常尾不清临时槽；Free异常终止。
    }
    p->vptrAt00->writeEnabledAt24(p, 1);
    ClearGuiClickedEventFlagsAt532200(body.eventsAt74);
    body.phaseAt6C = 0;
    p->vptrAt00->resetInputAt3C(p);
    p->vptrAt00->resetChildrenAt68(p);
    p->activeAt4C = nullptr;
    Table(p).beginHookAt8C(p);
    deps.root.ModalGateAt413BE0() = 1;
}
std::int32_t __fastcall ReadEvents(Common* p, void*)
{
    auto& body = Dialog(p);
    const auto& profile = Resources();
    auto* const end = body.eventsAt74.end;
    for (auto* at = body.eventsAt74.begin; at != end; ++at)
    {
        const auto base = at->baseAt00;
        Slot selected{base, at->biasAt04, at->controlAt08};
        if (!ResolveGuiClickedEvent(selected)->clickedFlagAt04) continue;
        selected = {base, at->biasAt04, at->controlAt08};
        const auto result = GeneralConfirmResultForEvent(profile, ResolveGuiClickedEvent(selected)->eventValueAt08);
        if (!result) return 0;
        // 只写原结果6/7/1；不在renderer提交业务，也不把Ack1当Yes6。
        body.resultAt68 = *result;
        return 1;
    }
    return 0; // 524E60不Clear；首个flag的未知值阻断后续事件。
}
std::int32_t __fastcall Poll(Common* p, void*)
{
    auto& body = Dialog(p);
    switch (body.phaseAt6C)
    {
    case 0: AddCapturedRect(p); body.phaseAt6C = 3; break;
    case 3:
        p->vptrAt00->processInputAt54(p);
        if (Table(p).readEventsAt90(p)) body.phaseAt6C = 4;
        break;
    case 4:
        AddCapturedRect(p);
        p->vptrAt00->writeEnabledAt24(p, 0);
        Table(p).endHookAt94(p);
        body.phaseAt6C = -1;
        break;
    default: return 0;
    }
    p->vptrAt00->flushDirtyAt58(p);
    return 1;
}
std::int32_t __fastcall End(Common* p, void*)
{
    // 508E80非noexcept业务：payload异常须外传，gate保持旧值，不能补清。
    auto& slot = Dialog(p).capturedAt5C;
    if (slot.controlAt08)
    {
        ReleaseUncleared(slot);
        slot.controlAt08 = nullptr;
        slot.baseAt00 = 0;
    }
    RequireGeneralConfirmDialogNativeServices().root.ModalGateAt413BE0() = 0;
    return 1;
}
void __fastcall Flush(Common* p, void*)
{
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = p->vptrAt00->dirtyAt00(p);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = p->vptrAt00->dirtyAt00(p);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t i = 0; i < count; ++i)
    {
        auto* rectangle = rectangles + i;
        const auto handle = Surface(Dialog(p).capturedAt5C)->handleAt04;
        if (handle)
            (void)RequireGeneralConfirmDialogNativeServices().drawing.SurfaceDrawRaw(
                handle, 0, 0, nullptr, nullptr, rectangle, 0);
        p->vptrAt00->drawAt60(p, rectangle);
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangle);
    }
    // 任一绘制/InvalidatedRect异常都保留dirty；count<0原仍走clear。
    dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->clearAt00(dirty);
}
}

namespace KaGuYa::Start::Runtime
{
void AssignGeneralConfirmDialogNativeCaptureAt48CA30(GuiDialogNativeBody5Ch* p, YtlReferenceSlotRuntime& argument)
{ AssignGraphicsSurfaceReferenceAt48CA30(GeneralConfirmNativeDetail::Dialog(p).capturedAt5C, argument); }
void ConsumeRootGeneralConfirmDialogNativeCaptureAt477C60(GuiDialogNativeBody5Ch* p)
{
    auto& slot = RequireGeneralConfirmDialogNativeServices().root.CaptureFrameSlotAt413BE0();
    YtlReferenceSlotRuntime argument;
    ConsumeGraphicsSurfaceReferenceAt477C60(argument, slot);
    AssignGeneralConfirmDialogNativeCaptureAt48CA30(p, argument);
}
}
