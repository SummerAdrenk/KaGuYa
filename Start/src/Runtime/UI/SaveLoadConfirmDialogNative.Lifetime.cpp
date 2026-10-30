#include "SaveLoadConfirmDialogNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local SaveLoadConfirmDialogNativeServices* services;
GuiDialogNativeBody5Ch* __fastcall Delete(GuiDialogNativeBody5Ch* body, void*, std::uint32_t flags) noexcept
{ return DeleteSaveLoadConfirmDialogNativeAt4FDD40(body, flags); }
}
SaveLoadConfirmDialogNativeScope::SaveLoadConfirmDialogNativeScope(SaveLoadConfirmDialogNativeServices& value) noexcept
    : SaveLoadConfirmDialogNativeScope(&value) {}
SaveLoadConfirmDialogNativeScope::SaveLoadConfirmDialogNativeScope(SaveLoadConfirmDialogNativeServices* value) noexcept
    : previous_(services), surfaces_(value?&value->surfaces:nullptr), graphics_(value?&value->graphics:nullptr),
      input_(value?&value->input:nullptr), screen_(value?&value->screen:nullptr)
{ services = value; }
SaveLoadConfirmDialogNativeScope::~SaveLoadConfirmDialogNativeScope() noexcept { services = previous_; }
SaveLoadConfirmDialogNativeServices& RequireSaveLoadConfirmDialogNativeServices()
{
    if (!services) throw std::logic_error("SaveLoadConfirm native services are not bound");
    return *services;
}
const SaveLoadConfirmDialogNativeVtable39& SaveLoadConfirmDialogNativeVtable() noexcept
{
    using namespace SaveLoadConfirmNativeDetail;
    static const auto table = []
    {
        SaveLoadConfirmDialogNativeVtable39 value{};
        value.common = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, function) value.common.field = reinterpret_cast<decltype(value.common.field)>(&function)
        SLOT(deletingDestructorAt04, Delete);
        SLOT(activeInputAt38, ActiveInput);
        SLOT(resetInputAt3C, ResetInput);
        SLOT(beginAt44, Begin); SLOT(pollAt48, Poll); SLOT(endAt4C, End);
        SLOT(resultAt50, Result); SLOT(flushDirtyAt58, Flush);
#undef SLOT
        value.beginHookAt8C = reinterpret_cast<decltype(value.beginHookAt8C)>(&NullHook);
        value.readEventsAt90 = reinterpret_cast<decltype(value.readEventsAt90)>(&ReadEvents);
        value.endHookAt94 = reinterpret_cast<decltype(value.endHookAt94)>(&NullHook);
        value.writeModeAt98 = reinterpret_cast<decltype(value.writeModeAt98)>(&Mode);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
namespace SaveLoadConfirmNativeDetail
{
void ReleaseUncleared(Slot& slot)
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
void RefLifetime::Finish(bool clear) noexcept
{
    armed = false;
    auto* control = slot.controlAt08;
    if (!control) return;
    ReleaseUncleared(slot);
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void RefLifetime::FinishCaptured(YtlReferenceControlRuntime* control, bool clear) noexcept
{
    armed = false;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void DestroyMembers(Body& body) noexcept
{
    // 5A2439/5A241E..5A23E0、4F8ED5..4F9017；不End，不清root gate。
    DestroyGuiControlSurfaceReferencesAt43EAA0(body.specialPagesAtD8);
    for (int index = 3; index >= 0; --index)
        DestroyGuiControlSurfaceReferencesAt43EAA0(body.digitsAtA8[index]);
    ReleaseYtlReference(body.previewAt9C);
    ReleaseYtlReference(body.labelsAt90);
    ReleaseYtlReference(body.maskAt84);
    body.common.vptrAt00 = &ModalBaseTable();
    DestroyGuiClickedEventReferencesAt4F6D10(body.eventsAt74);
    ReleaseYtlReference(body.capturedAt5C);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
}
void DestroySaveLoadConfirmDialogNativeAt4F8EB0(GuiDialogNativeBody5Ch* common) noexcept
{
    common->vptrAt00 = &SaveLoadConfirmDialogNativeVtable().common;
    SaveLoadConfirmNativeDetail::DestroyMembers(SaveLoadConfirmNativeDetail::Dialog(common));
}
GuiDialogNativeBody5Ch* DeleteSaveLoadConfirmDialogNativeAt4FDD40(
    GuiDialogNativeBody5Ch* common, std::uint32_t flags) noexcept
{
    DestroySaveLoadConfirmDialogNativeAt4F8EB0(common);
    if (flags & 1u) ::operator delete(common, std::size_t{0xE4});
    return common;
}
void WriteSaveLoadConfirmDialogNativeModeAt536140(GuiDialogNativeBody5Ch* common, std::int32_t value) noexcept
{ SaveLoadConfirmNativeDetail::Dialog(common).modeAt80 = value; }

GuiDialogNativeBody5Ch& SaveLoadConfirmDialogNativeModalTarget::Resolve() const
{ return *ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference_); }
IModalDialogRuntimeTarget* SaveLoadConfirmDialogNativeModalTarget::ResolveAt4A5330()
{ return ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference_) ? this : nullptr; }
void SaveLoadConfirmDialogNativeModalTarget::BeginAtVslot44()
{ auto& body = Resolve(); body.vptrAt00->beginAt44(&body); }
bool SaveLoadConfirmDialogNativeModalTarget::PollAtVslot48()
{ auto& body = Resolve(); return body.vptrAt00->pollAt48(&body) != 0; }
bool SaveLoadConfirmDialogNativeModalTarget::EndAtVslot4C()
{ auto& body = Resolve(); return body.vptrAt00->endAt4C(&body) != 0; }
std::int32_t SaveLoadConfirmDialogNativeModalTarget::ResultAtVslot50() const
{ auto& body = Resolve(); return body.vptrAt00->resultAt50(&body); }
std::int32_t RunSaveLoadConfirmDialogNativeAt5071D0(YtlReferenceSlotRuntime& reference)
{
    SaveLoadConfirmDialogNativeModalTarget target(reference);
    ModalDialogRunnerRuntime runner(RequireSaveLoadConfirmDialogNativeServices().modal);
    return runner.RunAt5071D0(target);
}
}
