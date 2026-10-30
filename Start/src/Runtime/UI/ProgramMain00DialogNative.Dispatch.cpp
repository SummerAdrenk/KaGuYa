#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
auto __fastcall Dirty(Common* p, void*) -> decltype(GuiDialogBaseVtable().dirtyAt00(p))
{ LeafScope leaves; return GuiDialogBaseVtable().dirtyAt00(p); }
auto __fastcall ClearChildren(Common* p, void*) -> decltype(GuiDialogBaseVtable().clearChildrenAt08(p))
{ LeafScope leaves; return GuiDialogBaseVtable().clearChildrenAt08(p); }
auto __fastcall Id(Common* p, void*) -> decltype(GuiDialogBaseVtable().idAt0C(p))
{ LeafScope leaves; return GuiDialogBaseVtable().idAt0C(p); }
auto __fastcall State(Common* p, void*) -> decltype(GuiDialogBaseVtable().readStateAt20(p))
{ LeafScope leaves; return GuiDialogBaseVtable().readStateAt20(p); }
auto __fastcall Enabled(Common* p, void*) -> decltype(GuiDialogBaseVtable().readEnabledAt28(p))
{ LeafScope leaves; return GuiDialogBaseVtable().readEnabledAt28(p); }
auto __fastcall IsEnabled(Common* p, void*) -> decltype(GuiDialogBaseVtable().enabledAt2C(p))
{ LeafScope leaves; return GuiDialogBaseVtable().enabledAt2C(p); }
auto __fastcall UpperInput(Common* p, void*) -> decltype(GuiDialogBaseVtable().upperInputAt34(p))
{ LeafScope leaves; return GuiDialogBaseVtable().upperInputAt34(p); }
auto __fastcall ActiveInput(Common* p, void*) -> decltype(GuiDialogBaseVtable().activeInputAt38(p))
{
    LeafScope leaves;
    // 510970是派生焦点确认路径；base +38为空，Button自身的快捷键不能代替它。
    const auto type=Read(Read(p->activeAt4C)->typeAt08);
    if(type!=0&&type!=2)return 0;
    auto& body=Dialog(p);
    const auto latch=Read(body.keyboardLatchAt94);
    const auto* methods=Read(p->vptrAt00);
    if(!latch)
    {
        if(!methods->pressedAt70(p,5))return 0;
        auto* child=Read(p->activeAt4C);
        body.keyboardLatchAt94=1;
        const auto* button=reinterpret_cast<const GuiButtonNativeVtable29*>(Read(child->vptrAt00));
        button->setPressedAt70(child,1);
        return 1;
    }
    if(!methods->heldAt6C(p,5))
    {
        auto* child=Read(p->activeAt4C);
        auto* control=Read(child->eventAt3C.controlAt08);
        Slot event{Read(child->eventAt3C.baseAt00),Read(child->eventAt3C.biasAt04),control};
        if(control)++control->referenceCountAt04;
        RefFrame lifetime{event};
        if(event.baseAt00)
        {
            auto* receiver=Resolve<GuiClickedEventNativeBody>(event);
            Read(receiver->vptrAt00)->invokeAt00(receiver);
        }
        body.keyboardLatchAt94=0;
        // 510A50正常尾使用缓存ESI；事件抛出时由RefFrame按原EH重读局部槽。
        lifetime.Captured(control,false);
    }
    return latch;
}
auto __fastcall ResetInput(Common* p, void*) -> decltype(GuiDialogBaseVtable().resetInputAt3C(p))
{
    LeafScope leaves;
    const auto* methods=Read(p->vptrAt00);
    Dialog(p).keyboardLatchAt94=0;
    methods->setActiveAt30(p,nullptr);
    p->inputLatchAt30=0;
}
auto __fastcall PriorityInput(Common* p, void*) -> decltype(GuiDialogBaseVtable().priorityInputAt40(p))
{ LeafScope leaves; return GuiDialogBaseVtable().priorityInputAt40(p); }
auto __fastcall Result(Common* p, void*) -> decltype(GuiDialogBaseVtable().resultAt50(p))
{ LeafScope leaves; return GuiDialogBaseVtable().resultAt50(p); }
auto __fastcall Input(Common* p, void*) -> decltype(GuiDialogBaseVtable().processInputAt54(p))
{ LeafScope leaves; return GuiDialogBaseVtable().processInputAt54(p); }
auto __fastcall Flush(Common* p, void*) -> decltype(GuiDialogBaseVtable().flushDirtyAt58(p))
{ LeafScope leaves; return GuiDialogBaseVtable().flushDirtyAt58(p); }
auto __fastcall ResetChildren(Common* p, void*) -> decltype(GuiDialogBaseVtable().resetChildrenAt68(p))
{ LeafScope leaves; return GuiDialogBaseVtable().resetChildrenAt68(p); }
auto __fastcall Wheel(Common* p, void*) -> decltype(GuiDialogBaseVtable().wheelAt78(p))
{ LeafScope leaves; return GuiDialogBaseVtable().wheelAt78(p); }
auto __fastcall Moved(Common* p, void*) -> decltype(GuiDialogBaseVtable().movedAt7C(p))
{ LeafScope leaves; return GuiDialogBaseVtable().movedAt7C(p); }
auto __fastcall MousePress(Common* p, void*) -> decltype(GuiDialogBaseVtable().mousePressAt80(p))
{ LeafScope leaves; return GuiDialogBaseVtable().mousePressAt80(p); }
auto __fastcall MouseRelease(Common* p, void*) -> decltype(GuiDialogBaseVtable().mouseReleaseAt84(p))
{ LeafScope leaves; return GuiDialogBaseVtable().mouseReleaseAt84(p); }
auto __fastcall Append(Common* p, void*, Slot v) -> decltype(GuiDialogBaseVtable().appendChildAt10(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().appendChildAt10(p, v); }
auto __fastcall SetDirty(Common* p, void*, Slot v) -> decltype(GuiDialogBaseVtable().setDirtyAt18(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().setDirtyAt18(p, v); }
auto __fastcall SetState(Common* p, void*, std::int32_t v) -> decltype(GuiDialogBaseVtable().writeStateAt1C(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().writeStateAt1C(p, v); }
auto __fastcall SetEnabled(Common* p, void*, std::int32_t v) -> decltype(GuiDialogBaseVtable().writeEnabledAt24(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().writeEnabledAt24(p, v); }
auto __fastcall SetActive(Common* p, void*, GuiControlNativeBody50h* v) -> decltype(GuiDialogBaseVtable().setActiveAt30(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().setActiveAt30(p, v); }
auto __fastcall Invalidate(Common* p, void*, RECT v) -> decltype(GuiDialogBaseVtable().invalidateAt64(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().invalidateAt64(p, v); }
auto __fastcall Held(Common* p, void*, std::int32_t v) -> decltype(GuiDialogBaseVtable().heldAt6C(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().heldAt6C(p, v); }
auto __fastcall Pressed(Common* p, void*, std::int32_t v) -> decltype(GuiDialogBaseVtable().pressedAt70(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().pressedAt70(p, v); }
auto __fastcall Pointer(Common* p, void*, POINT* v) -> decltype(GuiDialogBaseVtable().pointerAt74(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().pointerAt74(p, v); }
auto __fastcall Draw(Common* p, void*, const RECT* v) -> decltype(GuiDialogBaseVtable().drawAt60(p, v))
{ LeafScope leaves; return GuiDialogBaseVtable().drawAt60(p, v); }
Slot* __fastcall Find(Common* p, void*, Slot* out, std::int32_t id)
{ LeafScope leaves; return GuiDialogBaseVtable().findChildAt14(p, out, id); }
void __fastcall Composite(Common* p, void*, Slot target, const RECT* clip)
{ LeafScope leaves; GuiDialogBaseVtable().compositeAt5C(p, target, clip); }
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ return reinterpret_cast<Common*>(DeleteProgramMain00DialogNativeAt4FC110(&Dialog(p), flags)); }
void __fastcall Begin(Common* p, void*) { BeginProgramMain00DialogNativeAt50C230(Dialog(p)); }
std::int32_t __fastcall Poll(Common* p, void*) { return PollProgramMain00DialogNativeAt52C990(Dialog(p)); }
std::int32_t __fastcall End(Common* p, void*) { return EndProgramMain00DialogNativeAt508C10(Dialog(p)); }
void __fastcall Changed(Common* p, void*, Slot child) { ChangeProgramMain00DialogNativeAt518820(Dialog(p), child); }
GuiDialogNativeVtable35 Make() noexcept
{
    GuiDialogNativeVtable35 table{};
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
    // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节、RECT 为 16 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(member, target) table.member = reinterpret_cast<decltype(table.member)>(&target)
    SLOT(dirtyAt00, Dirty);
    SLOT(clearChildrenAt08, ClearChildren);
    SLOT(idAt0C, Id);
    SLOT(readStateAt20, State);
    SLOT(readEnabledAt28, Enabled);
    SLOT(enabledAt2C, IsEnabled);
    SLOT(upperInputAt34, UpperInput);
    SLOT(activeInputAt38, ActiveInput);
    SLOT(resetInputAt3C, ResetInput);
    SLOT(priorityInputAt40, PriorityInput);
    SLOT(resultAt50, Result);
    SLOT(processInputAt54, Input);
    SLOT(flushDirtyAt58, Flush);
    SLOT(resetChildrenAt68, ResetChildren);
    SLOT(wheelAt78, Wheel);
    SLOT(movedAt7C, Moved);
    SLOT(mousePressAt80, MousePress);
    SLOT(mouseReleaseAt84, MouseRelease);
    SLOT(appendChildAt10, Append);
    SLOT(setDirtyAt18, SetDirty);
    SLOT(writeStateAt1C, SetState);
    SLOT(writeEnabledAt24, SetEnabled);
    SLOT(setActiveAt30, SetActive);
    SLOT(invalidateAt64, Invalidate);
    SLOT(heldAt6C, Held);
    SLOT(pressedAt70, Pressed);
    SLOT(pointerAt74, Pointer);
    SLOT(drawAt60, Draw);
    SLOT(findChildAt14, Find); SLOT(compositeAt5C, Composite);
    SLOT(deletingDestructorAt04, Delete); SLOT(beginAt44, Begin);
    SLOT(pollAt48, Poll); SLOT(endAt4C, End); SLOT(childChangedAt88, Changed);
#undef SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return table;
}
}
const GuiDialogNativeVtable35& ProgramMain00DialogNativeVtable() noexcept
{ static const auto table = Make(); return table; }
}

