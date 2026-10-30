#include "ProgramSystemDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSystemNativeDetail;
#define BOUND0(name, member) \
    auto __fastcall name(Common* p, void*) -> decltype(GuiDialogBaseVtable().member(p)) \
    { Leaves leaves; return GuiDialogBaseVtable().member(p); }
#define BOUND1(name, member, type, arg) \
    auto __fastcall name(Common* p, void*, type arg) -> decltype(GuiDialogBaseVtable().member(p, arg)) \
    { Leaves leaves; return GuiDialogBaseVtable().member(p, arg); }
#define BOUND2(name, member, type1, arg1, type2, arg2) \
    auto __fastcall name(Common* p, void*, type1 arg1, type2 arg2) -> decltype(GuiDialogBaseVtable().member(p, arg1, arg2)) \
    { Leaves leaves; return GuiDialogBaseVtable().member(p, arg1, arg2); }
BOUND0(Dirty, dirtyAt00)
BOUND0(Clear, clearChildrenAt08)
BOUND0(Id, idAt0C)
BOUND1(Append, appendChildAt10, Slot, owned)
BOUND2(Find, findChildAt14, Slot*, out, std::int32_t, id)
BOUND1(SetDirty, setDirtyAt18, Slot, owned)
BOUND0(State, readStateAt20)
BOUND1(SetEnabled, writeEnabledAt24, std::int32_t, value)
BOUND0(Enabled, readEnabledAt28)
BOUND0(IsEnabled, enabledAt2C)
BOUND1(SetActive, setActiveAt30, GuiControlNativeBody50h*, child)
BOUND0(UpperInput, upperInputAt34)
BOUND0(ActiveInput, activeInputAt38)
BOUND0(ResetInput, resetInputAt3C)
BOUND0(PriorityInput, priorityInputAt40)
BOUND0(Result, resultAt50)
BOUND0(ProcessInput, processInputAt54)
BOUND0(FlushDirty, flushDirtyAt58)
BOUND2(Composite, compositeAt5C, Slot, owned, const RECT*, clip)
BOUND1(Draw, drawAt60, const RECT*, clip)
BOUND1(Invalidate, invalidateAt64, RECT, rectangle)
BOUND0(ResetChildren, resetChildrenAt68)
BOUND1(Held, heldAt6C, std::int32_t, key)
BOUND1(Pressed, pressedAt70, std::int32_t, key)
BOUND1(Pointer, pointerAt74, POINT*, output)
BOUND0(Wheel, wheelAt78)
BOUND0(Moved, movedAt7C)
BOUND0(MousePress, mousePressAt80)
BOUND0(MouseRelease, mouseReleaseAt84)
BOUND1(Changed, childChangedAt88, Slot, owned)
#undef BOUND0
#undef BOUND1
#undef BOUND2
Common* __fastcall Delete00(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramSystem00DialogNativeAt4FC2B0(&Dialog(p), flags)); }
Common* __fastcall Delete01(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramSystem01DialogNativeAt4FC350(&Dialog(p), flags)); }
void __fastcall State00(Common* p, void*, std::int32_t value)
{ Leaves leaves; p->stateAt34 = value; } // 5364D0，不能复用536500的递归子控件写态。
void __fastcall State01(Common* p, void*, std::int32_t value)
{ Leaves leaves; p->stateAt34 = value; } // 5364E0，独立原目标。
void __fastcall Begin00(Common* p, void*) { BeginProgramSystem00DialogNativeAt50C300(Dialog(p)); }
void __fastcall Begin01(Common* p, void*) { BeginProgramSystem01DialogNativeAt50C5F0(Dialog(p)); }
std::int32_t __fastcall Poll00(Common* p, void*) { return PollProgramSystem00DialogNativeAt52D090(Dialog(p)); }
std::int32_t __fastcall Poll01(Common* p, void*) { return PollProgramSystem01DialogNativeAt52D270(Dialog(p)); }
std::int32_t __fastcall End00(Common*, void*) { Leaves leaves; return 1; } // 508CC0不关闭。
std::int32_t __fastcall End01(Common*, void*) { Leaves leaves; return 1; } // 508CD0不关闭。
template<bool Mode01> GuiDialogNativeVtable35 Make() noexcept
{
    GuiDialogNativeVtable35 t{};
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
    // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节、RECT 为 16 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(member, target) t.member = reinterpret_cast<decltype(t.member)>(&target)
    SLOT(dirtyAt00, Dirty); SLOT(clearChildrenAt08, Clear); SLOT(idAt0C, Id);
    SLOT(appendChildAt10, Append); SLOT(findChildAt14, Find); SLOT(setDirtyAt18, SetDirty);
    SLOT(readStateAt20, State); SLOT(writeEnabledAt24, SetEnabled); SLOT(readEnabledAt28, Enabled);
    SLOT(enabledAt2C, IsEnabled); SLOT(setActiveAt30, SetActive); SLOT(upperInputAt34, UpperInput);
    SLOT(activeInputAt38, ActiveInput); SLOT(resetInputAt3C, ResetInput); SLOT(priorityInputAt40, PriorityInput);
    SLOT(resultAt50, Result); SLOT(processInputAt54, ProcessInput); SLOT(flushDirtyAt58, FlushDirty);
    SLOT(compositeAt5C, Composite); SLOT(drawAt60, Draw); SLOT(invalidateAt64, Invalidate);
    SLOT(resetChildrenAt68, ResetChildren); SLOT(heldAt6C, Held); SLOT(pressedAt70, Pressed);
    SLOT(pointerAt74, Pointer); SLOT(wheelAt78, Wheel); SLOT(movedAt7C, Moved);
    SLOT(mousePressAt80, MousePress); SLOT(mouseReleaseAt84, MouseRelease); SLOT(childChangedAt88, Changed);
    if constexpr (Mode01)
    {
        SLOT(deletingDestructorAt04, Delete01); SLOT(writeStateAt1C, State01);
        SLOT(beginAt44, Begin01); SLOT(pollAt48, Poll01); SLOT(endAt4C, End01);
    }
    else
    {
        SLOT(deletingDestructorAt04, Delete00); SLOT(writeStateAt1C, State00);
        SLOT(beginAt44, Begin00); SLOT(pollAt48, Poll00); SLOT(endAt4C, End00);
    }
#undef SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return t;
}
}
const GuiDialogNativeVtable35& ProgramSystem00DialogNativeVtable() noexcept
{ static const auto table = Make<false>(); return table; }
const GuiDialogNativeVtable35& ProgramSystem01DialogNativeVtable() noexcept
{ static const auto table = Make<true>(); return table; }
}
