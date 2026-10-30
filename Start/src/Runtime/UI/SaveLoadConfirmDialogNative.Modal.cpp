#include "SaveLoadConfirmDialogNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <stdexcept>
#include <cstdlib>
#include <exception>

namespace KaGuYa::Start::Runtime::SaveLoadConfirmNativeDetail
{
namespace
{
struct ModalTable38 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* beginHookAt8C)(Common*);
    std::int32_t (__thiscall* eventsAt90)(Common*);
    void (__thiscall* endHookAt94)(Common*);
};
static_assert(sizeof(ModalTable38) == 0x98);
const ModalTable38& ModalTable() noexcept;
void __fastcall BeginBase(Common* common, void*) { BeginModalBase(common); }
std::int32_t __fastcall EndBase(Common* common, void*) { return EndModalBase(common); }
std::int32_t __fastcall PureEvents(Common*, void*) { _purecall(); std::terminate(); }
void FlushLayers(Common*, bool);
void __fastcall FlushBase(Common* common, void*) { FlushLayers(common, false); }
Common* __fastcall DeleteBase(Common* common, void*, std::uint32_t flags) noexcept
{
    common->vptrAt00 = &ModalTable().common;
    auto& body = Dialog(common);
    DestroyGuiClickedEventReferencesAt4F6D10(body.eventsAt74);
    ReleaseYtlReference(body.capturedAt5C);
    DestroyGuiDialogAt4F7AC0(common);
    if (flags & 1u) ::operator delete(common, std::size_t{0x80});
    return common;
}
const ModalTable38& ModalTable() noexcept
{
    static const auto table = []
    {
        ModalTable38 value{};
        value.common = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 void* 仅占 EDX、不读取其输入。
        // 仅析构 flags 占 4 字节栈实参并由被调方清栈，其余槽无栈实参；各返回类型一致。
        // DeleteBase 的 noexcept 仅向可抛槽类型放宽，其余异常约定不变。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, function) value.common.field = reinterpret_cast<decltype(value.common.field)>(&function)
        SLOT(deletingDestructorAt04, DeleteBase);
        SLOT(activeInputAt38, ActiveInput); SLOT(resetInputAt3C, ResetInput);
        SLOT(beginAt44, BeginBase); SLOT(pollAt48, Poll); SLOT(endAt4C, EndBase);
        SLOT(resultAt50, Result); SLOT(flushDirtyAt58, FlushBase);
#undef SLOT
        value.beginHookAt8C = reinterpret_cast<decltype(value.beginHookAt8C)>(&NullHook);
        value.eventsAt90 = reinterpret_cast<decltype(value.eventsAt90)>(&PureEvents);
        value.endHookAt94 = reinterpret_cast<decltype(value.endHookAt94)>(&NullHook);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
void AddCapturedRect(Common* common)
{
    auto& body = Dialog(common);
    const auto width = Surface(body.capturedAt5C)->widthAt08;
    const auto height = Surface(body.capturedAt5C)->heightAt0C;
    auto* frame = Surface(body.capturedAt5C);
    const auto x = frame->originXAt10;
    const auto y = frame->originYAt14;
    auto* dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}
void DrawRawHandle(std::int32_t handle, std::int32_t x, std::int32_t y, const RECT* clip)
{
    if (handle)
        (void)RequireSaveLoadConfirmDialogNativeServices().drawing.SurfaceDrawRaw(
            handle, x, y, nullptr, nullptr, clip, 0);
}
void FlushLayers(Common* common, bool extra)
{
    auto& body = Dialog(common);
    auto* dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = common->vptrAt00->dirtyAt00(common);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = common->vptrAt00->dirtyAt00(common);
    auto* rectangles = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t index = 0; index < count; ++index)
    {
        const auto* rectangle = rectangles + index;
        DrawRawHandle(Surface(body.capturedAt5C)->handleAt04, 0, 0, rectangle);
        if (extra && common->vptrAt00->readEnabledAt28(common) && body.maskAt84.baseAt00)
            DrawRawHandle(Surface(body.maskAt84)->handleAt04, 0, 0, rectangle);
        common->vptrAt00->drawAt60(common, rectangle);
        if (extra && common->vptrAt00->readEnabledAt28(common))
        {
            if (body.labelsAt90.baseAt00)
            {
                auto* surface = Surface(body.labelsAt90);
                const auto x = surface->originXAt10;
                const auto y = surface->originYAt14;
                DrawRawHandle(Surface(body.labelsAt90)->handleAt04, x, y, rectangle);
            }
            if (body.previewAt9C.baseAt00)
            {
                const auto& profile=CurrentSaveLoadResourceProfile().confirmation;
                auto point=profile.previewAnchor;
                const auto* preview=Surface(body.previewAt9C);
                if (profile.centerPreview)
                { point.x-=preview->widthAt08/2; point.y-=preview->heightAt0C/2; }
                DrawRawHandle(preview->handleAt04, point.x, point.y, rectangle);
            }
        }
        // 同一dirty数组地址沿原InvalidatedRect叶提交；不能把整个绘制委托给source。
        RequireGuiDialogNativeScreenSource().InvalidateRectAt52603A(rectangle);
    }
    dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->clearAt00(dirty);
}
}
const GuiDialogNativeVtable35& ModalBaseTable() noexcept { return ModalTable().common; }
void __fastcall NullHook(Common*, void*) {} // 5086B0/51C110为原nullsub。
void __fastcall Mode(Common* common, void*, std::int32_t value)
{ WriteSaveLoadConfirmDialogNativeModeAt536140(common, value); }
std::int32_t __fastcall Result(Common* common, void*) { return Dialog(common).resultAt68; }
void __fastcall ResetInput(Common* common, void*)
{
    Dialog(common).actionFiveAt70 = 0;
    common->vptrAt00->setActiveAt30(common, nullptr);
    common->inputLatchAt30 = 0;
}
std::int32_t __fastcall ActiveInput(Common* common, void*)
{
    auto& body = Dialog(common);
    if (common->activeAt4C->typeAt08 != 0) return 0;
    const auto previous = body.actionFiveAt70;
    if (!previous)
    {
        if (!common->vptrAt00->pressedAt70(common, 5)) return 0;
        auto* child = common->activeAt4C;
        body.actionFiveAt70 = 1;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!common->vptrAt00->heldAt6C(common, 5))
    {
        Slot event;
        Copy(event, common->activeAt4C->eventAt3C);
        RefLifetime lifetime{event};
        if (event.baseAt00)
        {
            auto* selected = ResolveGuiClickedEvent(event);
            selected->vptrAt00->invokeAt00(selected);
        }
        body.actionFiveAt70 = 0;
        lifetime.Finish(true);
    }
    return previous;
}
void BeginModalBase(Common* common)
{
    auto& body = Dialog(common);
    auto& deps = RequireSaveLoadConfirmDialogNativeServices();
    if (!body.capturedAt5C.baseAt00)
    {
        auto temporary = deps.root.CaptureScreenAt413BE0();
        CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt5C, temporary.Slot());
    }
    common->vptrAt00->writeEnabledAt24(common, 1);
    ClearGuiClickedEventFlagsAt532200(body.eventsAt74);
    body.phaseAt6C = 0;
    common->vptrAt00->resetInputAt3C(common);
    common->vptrAt00->resetChildrenAt68(common);
    common->activeAt4C = nullptr;
    reinterpret_cast<const ModalTable38*>(common->vptrAt00)->beginHookAt8C(common);
    deps.root.ModalGateAt413BE0() = 1;
}
void __fastcall Begin(Common* common, void*)
{
    auto& body = Dialog(common);
    const auto& profile=CurrentSaveLoadResourceProfile().confirmation;
    if (body.modeAt80<0 || body.modeAt80>=static_cast<int>(profile.hiddenEvents.size()))
        throw std::out_of_range("save confirmation mode outside profile");
    for (auto* position = common->childrenAt0C.begin; position != common->childrenAt0C.end; ++position)
    {
        auto* child = ResolveGuiControl(*position);
        child->vptrAt00->writeEnabledAt4C(child, 1);
        const auto base = position->baseAt00;
        for (std::uint32_t index = 0; index != 4; ++index)
        {
            const Slot selected{base, position->biasAt04, position->controlAt08};
            child = ResolveGuiControl(selected);
            if (child->eventAt04 == profile.hiddenEvents[body.modeAt80][index])
            {
                child->vptrAt00->writeEnabledAt4C(child, 0);
                break;
            }
        }
    }
    BeginModalBase(common);
}
std::int32_t __fastcall ReadEvents(Common* common, void*)
{
    auto& body = Dialog(common);
    auto* const end = body.eventsAt74.end;
    for (auto* position = body.eventsAt74.begin; position != end; ++position)
    {
        const auto base = position->baseAt00;
        Slot selected{base, position->biasAt04, position->controlAt08};
        if (!ResolveGuiClickedEvent(selected)->clickedFlagAt04) continue;
        selected = {base, position->biasAt04, position->controlAt08};
        const auto event = ResolveGuiClickedEvent(selected)->eventValueAt08;
        selected = {base, position->biasAt04, position->controlAt08};
        auto* clicked = ResolveGuiClickedEvent(selected);
        clicked->vptrAt00->clearAt04(clicked);
        if (event == 3 || event == 5) { body.resultAt68 = 6; return 1; }
        if (event == 4 || event == 6) { body.resultAt68 = 7; return 1; }
        return 0; // 原首次有flag就停止，不扫描后续有效event。
    }
    return 0;
}
std::int32_t __fastcall Poll(Common* common, void*)
{
    auto& body = Dialog(common);
    switch (body.phaseAt6C)
    {
    case 0:
        AddCapturedRect(common);
        body.phaseAt6C = 3;
        break;
    case 3:
        common->vptrAt00->processInputAt54(common);
        if (reinterpret_cast<const ModalTable38*>(common->vptrAt00)->eventsAt90(common))
            body.phaseAt6C = 4;
        break;
    case 4:
        AddCapturedRect(common);
        common->vptrAt00->writeEnabledAt24(common, 0);
        reinterpret_cast<const ModalTable38*>(common->vptrAt00)->endHookAt94(common);
        body.phaseAt6C = -1;
        break;
    default:
        return 0;
    }
    common->vptrAt00->flushDirtyAt58(common);
    return 1;
}
std::int32_t EndModalBase(Common* common)
{
    ReleaseYtlReference(Dialog(common).capturedAt5C);
    RequireSaveLoadConfirmDialogNativeServices().root.ModalGateAt413BE0() = 0;
    return 1;
}
std::int32_t __fastcall End(Common* common, void*)
{
    ReleaseYtlReference(Dialog(common).previewAt9C);
    return EndModalBase(common);
}
void __fastcall Flush(Common* common, void*) { FlushLayers(common, true); }
}
