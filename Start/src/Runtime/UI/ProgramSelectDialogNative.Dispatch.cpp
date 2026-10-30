#include "ProgramSelectDialogNative.Internal.h"

extern "C" int __cdecl _purecall();

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSelectNativeDetail;
#define BOUND0(name, table, member) \
    auto __fastcall name(Common* p, void*) -> decltype((table).member(p)) \
    { LeafScope leaves; return (table).member(p); }
#define BOUND1(name, table, member, type, arg) \
    auto __fastcall name(Common* p, void*, type arg) -> decltype((table).member(p, arg)) \
    { LeafScope leaves; return (table).member(p, arg); }
#define BOUND2(name, table, member, type1, arg1, type2, arg2) \
    auto __fastcall name(Common* p, void*, type1 arg1, type2 arg2) -> decltype((table).member(p, arg1, arg2)) \
    { LeafScope leaves; return (table).member(p, arg1, arg2); }
BOUND0(Dirty, GuiDialogBaseVtable(), dirtyAt00)
BOUND0(Clear, GuiDialogBaseVtable(), clearChildrenAt08)
BOUND0(Id, GuiDialogBaseVtable(), idAt0C)
BOUND1(AppendControl, GuiDialogBaseVtable(), appendChildAt10, Slot, owned)
BOUND2(FindControl, GuiDialogBaseVtable(), findChildAt14, Slot*, output, std::int32_t, id)
BOUND1(SetDirty, GuiDialogIntoBaseVtable().common, setDirtyAt18, Slot, owned)
BOUND1(SetState, GuiDialogBaseVtable(), writeStateAt1C, std::int32_t, value)
BOUND0(State, GuiDialogBaseVtable(), readStateAt20)
BOUND1(SetEnabled, GuiDialogBaseVtable(), writeEnabledAt24, std::int32_t, value)
BOUND0(Enabled, GuiDialogBaseVtable(), readEnabledAt28)
BOUND0(IsEnabled, GuiDialogBaseVtable(), enabledAt2C)
BOUND1(SetActive, GuiDialogBaseVtable(), setActiveAt30, GuiControlNativeBody50h*, child)
BOUND0(UpperInput, GuiDialogBaseVtable(), upperInputAt34)
BOUND0(ActiveInput, GuiDialogBaseVtable(), activeInputAt38)
BOUND0(ResetInput, GuiDialogBaseVtable(), resetInputAt3C)
BOUND0(PriorityInput, GuiDialogBaseVtable(), priorityInputAt40)
BOUND0(Result, GuiDialogBaseVtable(), resultAt50)
BOUND0(ProcessInput, GuiDialogBaseVtable(), processInputAt54)
BOUND0(FlushDirty, GuiDialogBaseVtable(), flushDirtyAt58)
BOUND1(Invalidate, GuiDialogBaseVtable(), invalidateAt64, RECT, rectangle)
BOUND0(ResetChildren, GuiDialogBaseVtable(), resetChildrenAt68)
BOUND1(Held, GuiDialogBaseVtable(), heldAt6C, std::int32_t, key)
BOUND1(Pressed, GuiDialogBaseVtable(), pressedAt70, std::int32_t, key)
BOUND1(Pointer, GuiDialogBaseVtable(), pointerAt74, POINT*, output)
BOUND0(Wheel, GuiDialogBaseVtable(), wheelAt78)
BOUND0(Moved, GuiDialogBaseVtable(), movedAt7C)
BOUND0(MousePress, GuiDialogBaseVtable(), mousePressAt80)
BOUND0(MouseRelease, GuiDialogBaseVtable(), mouseReleaseAt84)
BOUND1(Changed, GuiDialogBaseVtable(), childChangedAt88, Slot, owned)
BOUND1(AppendDialog, GuiDialogIntoBaseVtable(), appendDialogAt8C, Slot, owned)
BOUND2(FindDialog, GuiDialogIntoBaseVtable(), findDialogAt90, Slot*, output, std::int32_t, id)
#undef BOUND0
#undef BOUND1
#undef BOUND2
Common* __fastcall DeleteBase(Common* p, void*, std::uint32_t flags) noexcept
{
    LeafScope leaves;
    DestroyProgramSelectDialogNativeAt4F8D50(Dialog(p));
    if (flags & 1u) ::operator delete(p, std::size_t{0xEC});
    return p;
}
Common* __fastcall Delete00(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramSelectDialogNativeAt4FC140(&Dialog(p), flags)); }
Common* __fastcall Delete01(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramSelectDialogNativeAt4FC280(&Dialog(p), flags)); }
void __fastcall Begin(Common* p, void*) { BeginProgramSelectDialogNativeAt50E770(Dialog(p)); }
std::int32_t __fastcall Poll(Common* p, void*) { return PollProgramSelectDialogNativeAt52F6E0(Dialog(p)); }
std::int32_t __fastcall End(Common* p, void*) { return EndProgramSelectDialogNativeAt509340(Dialog(p)); }
void __fastcall CompositeEntry(Common* p, void*, Slot target, const RECT* clip)
{ LeafScope leaves; Composite(p, target, clip); }
void __fastcall DrawEntry(Common* p, void*, const RECT* clip)
{ LeafScope leaves; Draw(p, clip); }
void __fastcall Pure(Common*, void*) { _purecall(); std::terminate(); }
void __fastcall Update00(Common* p, void*) { UpdateProgramSelectDialogNativeAt521FD0(Dialog(p)); }
void __fastcall Update01(Common* p, void*) { UpdateProgramSelectDialogNativeAt5221A0(Dialog(p)); }
void __fastcall Advance00(Common* p, void*) { AdvanceProgramSelectDialogNativeAt51C120(Dialog(p)); }
void __fastcall Advance01(Common* p, void*) { AdvanceProgramSelectDialogNativeAt51C4E0(Dialog(p)); }
Slot* __fastcall CaptureBase(Common* p, void*, Slot* out, std::int32_t value)
{ return CaptureProgramSelectDialogNativeAt503AA0(Dialog(p), out, value); }
Slot* __fastcall Capture00(Common* p, void*, Slot* out, std::int32_t value)
{ return CaptureProgramSelectDialogNativeAt5038A0(Dialog(p), out, value); }
Slot* __fastcall Capture01(Common* p, void*, Slot* out, std::int32_t value)
{ return CaptureProgramSelectDialogNativeAt5039A0(Dialog(p), out, value); }
void __fastcall Redraw(Common* p, void*) { RedrawProgramSelectDialogNativeAt530E40(Dialog(p)); }
void __fastcall Reopen00(Common* p, void*) { ReopenProgramSelectDialogNativeAt530E80(Dialog(p)); }
void __fastcall Reopen01(Common* p, void*) { ReopenProgramSelectDialogNativeAt531010(Dialog(p)); }
ProgramSelectDialogNativeVtable42 MakeBase() noexcept
{
    ProgramSelectDialogNativeVtable42 table{};
    auto& common = table.inherited.common;
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
    // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节、RECT 为 16 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(member, target) common.member = reinterpret_cast<decltype(common.member)>(&target)
    SLOT(dirtyAt00, Dirty); SLOT(deletingDestructorAt04, DeleteBase);
    SLOT(clearChildrenAt08, Clear); SLOT(idAt0C, Id);
    SLOT(appendChildAt10, AppendControl); SLOT(findChildAt14, FindControl);
    SLOT(setDirtyAt18, SetDirty); SLOT(writeStateAt1C, SetState); SLOT(readStateAt20, State);
    SLOT(writeEnabledAt24, SetEnabled); SLOT(readEnabledAt28, Enabled); SLOT(enabledAt2C, IsEnabled);
    SLOT(setActiveAt30, SetActive); SLOT(upperInputAt34, UpperInput); SLOT(activeInputAt38, ActiveInput);
    SLOT(resetInputAt3C, ResetInput); SLOT(priorityInputAt40, PriorityInput);
    SLOT(beginAt44, Begin); SLOT(pollAt48, Poll); SLOT(endAt4C, End); SLOT(resultAt50, Result);
    SLOT(processInputAt54, ProcessInput); SLOT(flushDirtyAt58, FlushDirty);
    SLOT(compositeAt5C, CompositeEntry); SLOT(drawAt60, DrawEntry); SLOT(invalidateAt64, Invalidate);
    SLOT(resetChildrenAt68, ResetChildren); SLOT(heldAt6C, Held); SLOT(pressedAt70, Pressed);
    SLOT(pointerAt74, Pointer); SLOT(wheelAt78, Wheel); SLOT(movedAt7C, Moved);
    SLOT(mousePressAt80, MousePress); SLOT(mouseReleaseAt84, MouseRelease); SLOT(childChangedAt88, Changed);
#undef SLOT
    table.inherited.appendDialogAt8C = reinterpret_cast<decltype(table.inherited.appendDialogAt8C)>(&AppendDialog);
    table.inherited.findDialogAt90 = reinterpret_cast<decltype(table.inherited.findDialogAt90)>(&FindDialog);
    table.updateChildrenAt94 = reinterpret_cast<decltype(table.updateChildrenAt94)>(&Pure);
    table.advanceTransitionAt98 = reinterpret_cast<decltype(table.advanceTransitionAt98)>(&Pure);
    table.captureAt9C = reinterpret_cast<decltype(table.captureAt9C)>(&CaptureBase);
    table.redrawAtA0 = reinterpret_cast<decltype(table.redrawAtA0)>(&Redraw);
    table.reopenAtA4 = reinterpret_cast<decltype(table.reopenAtA4)>(&Pure);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return table;
}
ProgramSelectDialogNativeVtable42 MakeTop(bool mode01) noexcept
{
    auto table = ProgramSelectDialogNativeBaseVtable();
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
    table.inherited.common.deletingDestructorAt04 = reinterpret_cast<decltype(table.inherited.common.deletingDestructorAt04)>(mode01 ? &Delete01 : &Delete00);
    table.updateChildrenAt94 = reinterpret_cast<decltype(table.updateChildrenAt94)>(mode01 ? &Update01 : &Update00);
    table.advanceTransitionAt98 = reinterpret_cast<decltype(table.advanceTransitionAt98)>(mode01 ? &Advance01 : &Advance00);
    table.captureAt9C = reinterpret_cast<decltype(table.captureAt9C)>(mode01 ? &Capture01 : &Capture00);
    table.reopenAtA4 = reinterpret_cast<decltype(table.reopenAtA4)>(mode01 ? &Reopen01 : &Reopen00);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return table;
}
}
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeBaseVtable() noexcept
{ static const auto table = MakeBase(); return table; }
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeTop00Vtable() noexcept
{ static const auto table = MakeTop(false); return table; }
const ProgramSelectDialogNativeVtable42& ProgramSelectDialogNativeTop01Vtable() noexcept
{ static const auto table = MakeTop(true); return table; }

void BeginProgramSelectDialogNativeAt50E770(ProgramSelectDialogNativeBodyECh& body)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    Read(Base(body).vptrAt00)->writeEnabledAt24(&Base(body), 1);
    GuiDialogBaseVtable().resetChildrenAt68(&Base(body));
    for (auto* at = Read(body.into.dialogsAt5C.begin); at != Read(body.into.dialogsAt5C.end); ++at)
    {
        auto* child = Resolve<Common>(*at);
        Read(child->vptrAt00)->beginAt44(child);
    }
    body.resultAt94 = -1;
    body.phaseAt68 = 0;
}
std::int32_t PollProgramSelectDialogNativeAt52F6E0(ProgramSelectDialogNativeBodyECh& body)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    switch (Read(body.phaseAt68))
    {
    case 0: body.phaseAt68 = 1; body.subphaseAt6C = 0; [[fallthrough]];
    case 1: case 2: Methods(body).advanceTransitionAt98(&Base(body)); return 1;
    case 3:
        Methods(body).updateChildrenAt94(&Base(body));
        Read(Base(body).vptrAt00)->flushDirtyAt58(&Base(body));
        return 1;
    case 4: body.phaseAt68 = -1; return 0;
    default: return 0;
    }
}
std::int32_t EndProgramSelectDialogNativeAt509340(ProgramSelectDialogNativeBodyECh& body)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    auto& source = leaves.services.source;
    Read(Base(body).vptrAt00)->writeEnabledAt24(&Base(body), 0);
    auto* archive = Archive(source.CaptureEnvironmentAt4180C0(0x50934C), 0);
    source.EnsureRootAt413BE0(0x509398);
    const auto value = Read(body.resultAt94);
    auto* root = source.RootAt6198C4(0x5093A3);
    const auto index = source.ResolveNamedAt498020(archive, "prog_result", 0x5093B0);
    auto* params = source.ParamsAt47BDB0(root, 0x5093B8);
    source.WriteValueAt4C8220(params, index, value, 0x5093BF);
    return 1;
}
namespace ProgramSelectNativeDetail
{
void Composite(Common* body, Slot ownedTarget, const RECT* clip)
{
    LocalReference argument{ownedTarget};
    GuiDialogBaseVtable().compositeAt5C(body, Copy(ownedTarget), clip);
    GuiDialogIntoBaseVtable().common.compositeAt5C(body, Copy(ownedTarget), clip);
    argument.Finish();
}
void Draw(Common* body, const RECT* clip)
{
    GuiDialogBaseVtable().drawAt60(body, clip);
    GuiDialogIntoBaseVtable().common.drawAt60(body, clip);
}
}
}
