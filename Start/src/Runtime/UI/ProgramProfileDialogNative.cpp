#include "ProgramProfileDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace { thread_local const ProgramModeProfile* selectedMode = nullptr; }
const ProgramModeProfile* SelectedProgramDialogProfile() noexcept { return selectedMode; }
namespace ProgramProfileDetail
{
ModeScope::ModeScope(const ProgramModeProfile& mode) noexcept : previous_(selectedMode) { selectedMode = &mode; }
ModeScope::~ModeScope() noexcept { selectedMode = previous_; }
namespace
{
Common* __fastcall DeletePanel(Common* p, void*, unsigned flags) noexcept
{
    Leaves leaves;
    ProgramMain01NativeDetail::DestroyMembers(AsPanel(p).base);
    if (flags & 1u) ::operator delete(p, sizeof(Panel));
    return p;
}
void __fastcall PanelBegin(Common* p, void*) { Leaves leaves; BeginPanel(AsPanel(p)); }
int __fastcall PanelPoll(Common* p, void*) { Leaves leaves; return PollPanel(AsPanel(p)); }
int __fastcall PanelEnd(Common*, void*) { return 1; }
void __fastcall PanelDraw(Common* p, void*, const RECT* clip) { Leaves leaves; DrawPanel(AsPanel(p), clip); }
void __fastcall PanelComposite(Common* p, void*, Slot owned, const RECT* clip)
{ Leaves leaves; CompositePanel(AsPanel(p), owned, clip); }
Common* __fastcall DeleteTop(Common* p, void*, unsigned flags) noexcept
{
    Leaves leaves; DestroyTop(AsTop(p));
    if (flags & 1u) ::operator delete(p, sizeof(Top));
    return p;
}
void __fastcall Update(Common* p, void*) { Leaves leaves; UpdateTop(AsTop(p)); }
void __fastcall Advance(Common* p, void*)
{
    Leaves leaves; auto& b = AsTop(p); ModeScope mode(*b.mode);
    AdvanceProgramSelectDialogNativeAt51C4E0(b.base);
}
void __fastcall ReopenEntry(Common* p, void*) { Leaves leaves; Reopen(AsTop(p)); }
void __fastcall Draw(Common* p, void*, const RECT* clip)
{
    Leaves leaves;
    auto& b = AsTop(p);
    ProgramSelectDialogNativeTop01Vtable().inherited.common.drawAt60(p, clip);
    if (b.modal)
    {
        auto* modal = Resolve<Common>(b.confirmation);
        modal->vptrAt00->drawAt60(modal, clip);
    }
}
void __fastcall Composite(Common* p, void*, Slot owned, const RECT* clip)
{
    Leaves leaves; Ref target; target.slot = owned;
    auto& b = AsTop(p);
    ProgramSelectDialogNativeTop01Vtable().inherited.common.compositeAt5C(p, Copy(target.slot), clip);
    if (b.modal)
    {
        auto* modal = Resolve<Common>(b.confirmation);
        modal->vptrAt00->compositeAt5C(modal, Copy(target.slot), clip);
    }
}
}
const GuiDialogNativeVtable35& PanelTable() noexcept
{
    static const auto table = []
    {
        auto t = ProgramMain01DialogNativeVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define PSLOT(field, fn) t.field = reinterpret_cast<decltype(t.field)>(&fn)
        PSLOT(deletingDestructorAt04, DeletePanel); PSLOT(beginAt44, PanelBegin);
        PSLOT(pollAt48, PanelPoll); PSLOT(endAt4C, PanelEnd);
        PSLOT(drawAt60, PanelDraw); PSLOT(compositeAt5C, PanelComposite);
#undef PSLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
const ProgramSelectDialogNativeVtable42& TopTable() noexcept
{
    static const auto table = []
    {
        auto t = ProgramSelectDialogNativeTop01Vtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.inherited.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.inherited.common.deletingDestructorAt04)>(&DeleteTop);
        t.inherited.common.drawAt60 = reinterpret_cast<decltype(t.inherited.common.drawAt60)>(&Draw);
        t.inherited.common.compositeAt5C = reinterpret_cast<decltype(t.inherited.common.compositeAt5C)>(&Composite);
        t.updateChildrenAt94 = reinterpret_cast<decltype(t.updateChildrenAt94)>(&Update);
        t.advanceTransitionAt98 = reinterpret_cast<decltype(t.advanceTransitionAt98)>(&Advance);
        t.reopenAtA4 = reinterpret_cast<decltype(t.reopenAtA4)>(&ReopenEntry);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
void Redraw(Top& b) { RedrawProgramSelectDialogNativeAt530E40(b.base); }
}
std::size_t ProgramProfileDialogStorageSize() noexcept { return sizeof(ProgramProfileDetail::Top); }
bool IsProgramProfileDialog(const GuiDialogNativeBody5Ch* p) noexcept
{ return p && p->vptrAt00 == &ProgramProfileDetail::TopTable().inherited.common; }
bool ProgramProfileDialogHasModal(const GuiDialogNativeBody5Ch* p) noexcept
{ return IsProgramProfileDialog(p) && reinterpret_cast<const ProgramProfileDetail::Top*>(p)->modal; }
}
