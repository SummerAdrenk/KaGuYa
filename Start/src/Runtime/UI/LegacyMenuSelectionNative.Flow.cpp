#include "LegacyMenuSelectionNative.Internal.h"

namespace KaGuYa::Start::Runtime::LegacyMenuSelectionDetail
{
namespace
{
GuiControlNativeBody50h& Child(Panel& p, int event)
{
    Ref r;
    p.common.vptrAt00->findChildAt14(&p.common, &r.slot, event);
    auto* child = ResolveGuiControl(r.slot);
    if (!child) throw std::logic_error("Required menu control is missing");
    return *child;
}
int& Checked(Panel& p, int event)
{ return reinterpret_cast<GuiCheckBoxNativeBody60h&>(Child(p, event)).checkedAt5C; }
Base* __fastcall DeletePanel(Base* base, void*, unsigned flags) noexcept
{
    auto& p = *reinterpret_cast<Panel*>(base);
    DestroyPanel(p);
    if (flags & 1u) ::operator delete(&p, sizeof(Panel));
    return base;
}
void __fastcall ResetInput(Base* base, void*)
{
    auto& p = *reinterpret_cast<Panel*>(base);
    p.confirmLatch = 0;
    GuiDialogBaseVtable().resetInputAt3C(base);
}
int __fastcall Confirm(Base* base, void*)
{
    auto& p = *reinterpret_cast<Panel*>(base);
    if (p.system || !base->activeAt4C || base->activeAt4C->typeAt08 != 0) return 0;
    const auto previous = p.confirmLatch;
    if (!previous)
    {
        if (!base->vptrAt00->pressedAt70(base, 5)) return 0;
        p.confirmLatch = 1;
        GuiButtonVtable().setPressedAt70(base->activeAt4C, 1);
        return 1;
    }
    if (!base->vptrAt00->heldAt6C(base, 5))
    {
        Ref event(Copy(base->activeAt4C->eventAt3C));
        if (auto* e = ResolveGuiClickedEvent(event.slot)) e->vptrAt00->invokeAt00(e);
        p.confirmLatch = 0;
    }
    return previous;
}
void __fastcall BeginPanel(Base* base, void*)
{
    auto& p = *reinterpret_cast<Panel*>(base);
    Leaves leaves(Services(p));
    ClearGuiClickedEventFlagsAt532200(p.events);
    base->vptrAt00->writeEnabledAt24(base, 1);
    base->vptrAt00->resetInputAt3C(base);
    base->vptrAt00->resetChildrenAt68(base);
    p.result = -1;
    if (p.system) SystemBegin(p); else RenderLabels(p);
    base->vptrAt00->invalidateAt64(base, p.bounds);
}
int __fastcall PollPanel(Base* base, void*)
{
    auto& p = *reinterpret_cast<Panel*>(base);
    Leaves leaves(Services(p));
    if (!p.system) p.result = -1;
    base->vptrAt00->processInputAt54(base);
    const int event = PollGuiClickedEventManagerNativeAt50FDB0(&p.events, 1);
    if (p.system) { if (event >= 0) SystemEvent(p, event); }
    else p.result = event;
    return 1;
}
int __fastcall EndPanel(Base* base, void*)
{
    auto& p = *reinterpret_cast<Panel*>(base);
    if (!p.system && p.result >= 13 && p.result <= 15)
    {
        // 原0x4FE4C0：End重读命名值，再置正文已读byte；不是普通choice表。
        auto& source = Services(p).source;
        source.MarkLabelRead(source.ReadValue(Profile(p).labelVariables[static_cast<std::size_t>(p.result-13)]));
    }
    return 1;
}
Base* __fastcall DeleteTop(Base* base, void*, unsigned flags) noexcept
{
    auto* top = reinterpret_cast<Top*>(base);
    Leaves leaves(*top->services);
    DestroyGuiDialogIntoNativeAt4F7B80(base);
    if (flags & 1u) ::operator delete(top, sizeof(Top));
    return base;
}
void __fastcall BeginTop(Base* base, void*)
{
    auto& b = *reinterpret_cast<Top*>(base);
    Leaves leaves(*b.services);
    base->vptrAt00->writeEnabledAt24(base, 1);
    base->vptrAt00->resetInputAt3C(base);
    b.main->common.vptrAt00->beginAt44(&b.main->common);
    b.system->common.vptrAt00->beginAt44(&b.system->common);
    b.phase = 3; b.result = -1;
}
int __fastcall PollTop(Base* base, void*)
{
    auto& b = *reinterpret_cast<Top*>(base);
    Leaves leaves(*b.services);
    if (b.phase == 3)
    {
        // 原0x4B6F90：先system、后main；本帧系统命令在下一次场景Update处理。
        auto poll = [](Panel& p) { if (p.common.vptrAt00->readStateAt20(&p.common)) p.common.vptrAt00->pollAt48(&p.common); };
        poll(*b.system); poll(*b.main);
        if (b.main->result != -1) { b.result = b.main->result; b.phase = 4; }
        return 1;
    }
    if (b.phase == 4)
    {
        auto* dirty=base->vptrAt00->dirtyAt00(base);
        dirty->vptrAt00->addRectAt08(dirty, &b.bounds);
        b.phase = -1;
        return 1;
    }
    return 0;
}
int __fastcall EndTop(Base* base, void*)
{
    auto& b = *reinterpret_cast<Top*>(base);
    Leaves leaves(*b.services);
    base->vptrAt00->writeEnabledAt24(base, 0);
    // 原0x4EAE40先提交menu_result，之后按main/system顺序End。异常不回滚结果。
    b.services->source.WriteValue(RequireLegacyMenuSelectionProfile(b.services->profile).resultVariable, b.result-13);
    b.main->common.vptrAt00->endAt4C(&b.main->common);
    b.system->common.vptrAt00->endAt4C(&b.system->common);
    return 1;
}
}
void SystemBegin(Panel& p)
{
    auto disable = [&](int event) { auto& c=Child(p,event); c.vptrAt00->writeStateAt44(&c,0); };
    // 原0x4EB330：Voice/透明度/Skip均不可用；auto的false分支不强清已有勾选。
    disable(2); disable(4);
    if (Services(p).source.UnSave().FlagsAt18() & 2u) Checked(p,7)=1;
    Checked(p,8) = (Services(p).source.UnSave().FlagsAt14() & 2u) != 0;
    disable(8);
}
void SystemEvent(Panel& p, int event)
{
    if (event == 7)
    {
        Checked(p,7) = !Checked(p,7);
        Services(p).source.UnSave().FlagsAt18() ^= 2u;
        return;
    }
    for (const auto& row : Profile(p).routes)
        if (event == row.event)
        {
            auto& child = Child(p,event);
            child.vptrAt00->resetAt08(&child);
            Services(p).source.SetCommand(row.command);
            return;
        }
    // 原0x4EB680其余event无业务，包括禁用的8；不发明设置/skip替代入口。
}
void ReopenSystem(Panel& p)
{
    // 原0x4EB880只刷新skip，不清auto或重置其它控件。
    Checked(p,8)=(Services(p).source.UnSave().FlagsAt14()&2u)!=0;
    auto& skip=Child(p,8);
    skip.vptrAt00->writeStateAt44(&skip,0);
}
const GuiDialogNativeVtable35& PanelMethods() noexcept
{
    static const auto table = [] {
        auto t = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define MENU_SLOT(name, fn) t.name = reinterpret_cast<decltype(t.name)>(&fn)
        MENU_SLOT(deletingDestructorAt04, DeletePanel);
        MENU_SLOT(activeInputAt38, Confirm); MENU_SLOT(resetInputAt3C, ResetInput);
        MENU_SLOT(beginAt44, BeginPanel); MENU_SLOT(pollAt48, PollPanel); MENU_SLOT(endAt4C, EndPanel);
#undef MENU_SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
const GuiDialogIntoNativeVtable37& TopMethods() noexcept
{
    static const auto table = [] {
        auto t = GuiDialogIntoBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define MENU_SLOT(name, fn) t.common.name = reinterpret_cast<decltype(t.common.name)>(&fn)
        MENU_SLOT(deletingDestructorAt04, DeleteTop); MENU_SLOT(beginAt44, BeginTop);
        MENU_SLOT(pollAt48, PollTop); MENU_SLOT(endAt4C, EndTop);
#undef MENU_SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
}
namespace KaGuYa::Start::Runtime
{
void ReopenLegacyMenuSelectionSystem(GuiDialogNativeBody5Ch& base)
{
    using namespace LegacyMenuSelectionDetail;
    auto& b = reinterpret_cast<Top&>(base);
    Leaves leaves(*b.services);
    // 原0x4FDB60/0x4EB880：只刷新system，不重渲染main、不提交结果。
    ReopenSystem(*b.system);
    base.vptrAt00->invalidateAt64(&base, b.bounds);
    base.vptrAt00->flushDirtyAt58(&base);
}
}
