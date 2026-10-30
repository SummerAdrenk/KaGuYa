#include "ProgramMain01DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain01NativeDetail;
// 每个实际入口绑定整个调用，覆盖回调和EH；算法仍用现有35槽原生叶。
#define BOUND0(name, member) \
    auto __fastcall name(Common* p, void*) -> decltype(GuiDialogBaseVtable().member(p)) \
    { LeafScope leaves; return GuiDialogBaseVtable().member(p); }
#define BOUND1(name, member, type, arg) \
    auto __fastcall name(Common* p, void*, type arg) -> decltype(GuiDialogBaseVtable().member(p, arg)) \
    { LeafScope leaves; return GuiDialogBaseVtable().member(p, arg); }
#define BOUND2(name, member, type1, arg1, type2, arg2) \
    auto __fastcall name(Common* p, void*, type1 arg1, type2 arg2) -> decltype(GuiDialogBaseVtable().member(p, arg1, arg2)) \
    { LeafScope leaves; return GuiDialogBaseVtable().member(p, arg1, arg2); }
BOUND0(Dirty, dirtyAt00)
BOUND0(Clear, clearChildrenAt08)
BOUND0(Id, idAt0C)
BOUND1(Append, appendChildAt10, Slot, owned)
BOUND2(Find, findChildAt14, Slot*, output, std::int32_t, id)
BOUND1(SetDirty, setDirtyAt18, Slot, owned)
BOUND1(SetState, writeStateAt1C, std::int32_t, value)
BOUND0(State, readStateAt20)
BOUND1(SetEnabled, writeEnabledAt24, std::int32_t, value)
BOUND0(Enabled, readEnabledAt28)
BOUND0(IsEnabled, enabledAt2C)
BOUND1(SetActive, setActiveAt30, Child*, child)
BOUND0(UpperInput, upperInputAt34)
BOUND0(PriorityInput, priorityInputAt40)
BOUND0(Result, resultAt50)
BOUND0(ProcessInput, processInputAt54)
BOUND0(FlushDirty, flushDirtyAt58)
BOUND2(Composite, compositeAt5C, Slot, ownedTarget, const RECT*, clip)
BOUND1(Draw, drawAt60, const RECT*, clip)
BOUND1(Invalidate, invalidateAt64, RECT, rect)
BOUND0(ResetChildren, resetChildrenAt68)
BOUND1(Held, heldAt6C, std::int32_t, action)
BOUND1(Pressed, pressedAt70, std::int32_t, action)
BOUND1(Pointer, pointerAt74, POINT*, output)
BOUND0(Wheel, wheelAt78)
BOUND0(Moved, movedAt7C)
BOUND0(MousePress, mousePressAt80)
BOUND0(MouseRelease, mouseReleaseAt84)
BOUND1(Changed, childChangedAt88, Slot, owned)
#undef BOUND0
#undef BOUND1
#undef BOUND2
std::int32_t __fastcall ActiveInput(Common* p, void*)
{
    LeafScope leaves;
    auto* active = Read(p->activeAt4C);
    const auto type = Read(active->typeAt08);
    if (type != 0 && type != 2) return 0;
    const auto latch = Read(Dialog(p).confirmLatchAt84);
    if (!latch)
    {
        if (!Read(p->vptrAt00)->pressedAt70(p, 5)) return 0;
        active = Read(p->activeAt4C);
        Dialog(p).confirmLatchAt84 = 1;
        const auto* table = reinterpret_cast<const GuiButtonNativeVtable29*>(Read(active->vptrAt00));
        table->setPressedAt70(active, 1);
        return 1;
    }
    if (Read(p->vptrAt00)->heldAt6C(p, 5)) return latch;
    active = Read(p->activeAt4C);
    auto* captured = Read(active->eventAt3C.controlAt08);
    Slot event{Read(active->eventAt3C.baseAt00), Read(active->eventAt3C.biasAt04), captured};
    if (captured) ++captured->referenceCountAt04;
    RefLife life{event}; // 510A80 FI0/5A65A0；回调抛错只展开这个局部Ref。
    if (Read(event.baseAt00))
    {
        auto* receiver = Resolve<GuiClickedEventNativeBody>(event);
        Read(receiver->vptrAt00)->invokeAt00(receiver);
    }
    Dialog(p).confirmLatchAt84 = 0;
    life.Captured(captured); // FI1/541E55；return保留进入时EBX，不归一化。
    return latch;
}
void __fastcall ResetInput(Common* p, void*)
{
    LeafScope leaves;
    const auto* table = Read(p->vptrAt00);
    Dialog(p).confirmLatchAt84 = 0;
    table->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramMain01DialogNativeAt4FC170(&Dialog(p), flags)); }
void __fastcall Begin(Common* p, void*) { BeginProgramMain01DialogNativeAt50C2A0(Dialog(p)); }
std::int32_t __fastcall Poll(Common* p, void*) { return PollProgramMain01DialogNativeAt52D040(Dialog(p)); }
std::int32_t __fastcall End(Common*, void*) { LeafScope leaves; return 1; } // 原508CB0，不关闭或写档。
GuiDialogNativeVtable35 MakeTable() noexcept
{
    GuiDialogNativeVtable35 t;
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
    // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节、RECT 为 16 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(member, target) t.member = reinterpret_cast<decltype(t.member)>(&target)
    SLOT(dirtyAt00, Dirty); SLOT(deletingDestructorAt04, Delete); SLOT(clearChildrenAt08, Clear);
    SLOT(idAt0C, Id); SLOT(appendChildAt10, Append); SLOT(findChildAt14, Find);
    SLOT(setDirtyAt18, SetDirty); SLOT(writeStateAt1C, SetState); SLOT(readStateAt20, State);
    SLOT(writeEnabledAt24, SetEnabled); SLOT(readEnabledAt28, Enabled); SLOT(enabledAt2C, IsEnabled);
    SLOT(setActiveAt30, SetActive); SLOT(upperInputAt34, UpperInput); SLOT(activeInputAt38, ActiveInput);
    SLOT(resetInputAt3C, ResetInput); SLOT(priorityInputAt40, PriorityInput);
    SLOT(beginAt44, Begin); SLOT(pollAt48, Poll); SLOT(endAt4C, End); SLOT(resultAt50, Result);
    SLOT(processInputAt54, ProcessInput); SLOT(flushDirtyAt58, FlushDirty);
    SLOT(compositeAt5C, Composite); SLOT(drawAt60, Draw); SLOT(invalidateAt64, Invalidate);
    SLOT(resetChildrenAt68, ResetChildren); SLOT(heldAt6C, Held); SLOT(pressedAt70, Pressed);
    SLOT(pointerAt74, Pointer); SLOT(wheelAt78, Wheel); SLOT(movedAt7C, Moved);
    SLOT(mousePressAt80, MousePress); SLOT(mouseReleaseAt84, MouseRelease); SLOT(childChangedAt88, Changed);
#undef SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return t;
}
}
const GuiDialogNativeVtable35& ProgramMain01DialogNativeVtable() noexcept
{ static const auto table = MakeTable(); return table; }
}
