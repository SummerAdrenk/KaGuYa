#include "SaveLoadPopupNative.Internal.h"

namespace KaGuYa::Start::Runtime::SaveLoadPopupNativeDetail
{
namespace
{
void Invoke(const Slot& event)
{
    if (!Read(event.baseAt00)) return;
    auto* e = Resolve<GuiClickedEventNativeBody>(event);
    e->vptrAt00->invokeAt00(e);
}
GuiNavigationNativeBody* Navigation(Common* p, std::int32_t index)
{ return Resolve<GuiNavigationNativeBody>(Read(p->navigationAt24.begin)[index]); }
void TriggerBoundary(Common* p, std::int32_t index, bool repeat)
{
    const auto input = Read(Navigation(p, index)->inputAt04);
    if (input != 1 && input != 2) return;
    Fresh arrow;
    p->vptrAt00->findChildAt14(p, &arrow.r, input == 1 ? 58 : 59);
    Local arrowLife{arrow.r};
    if (Read(arrow.r.baseAt00))
    {
        auto* child = Resolve<GuiControlNativeBody50h>(arrow.r);
        if (child->vptrAt00->readStateAt48(child))
        {
            Slot event = Copy(Resolve<GuiControlNativeBody50h>(arrow.r)->eventAt3C);
            Local eventLife{event};
            auto* captured = event.controlAt08;
            Invoke(event);
            if (repeat && input == 2) eventLife.Current(true); else eventLife.Captured(captured);
        }
    }
    arrowLife.Current();
}
bool Focus(Common* p, std::int32_t event, bool reset)
{
    if (event < 60 || event >= 60 + VisibleRows(PageCount())) return false;
    Fresh child;
    p->vptrAt00->findChildAt14(p, &child.r, event);
    Local life{child.r};
    if (!Read(child.r.baseAt00)) throw 1; // 520685原缺child抛int1，不返回伪成功。
    auto* selected = Resolve<GuiControlNativeBody50h>(child.r);
    const auto state = selected->vptrAt00->resourceStateAt38(selected);
    if (state == 1 || state == 2) { life.Current(true); return false; }
    if (reset) p->vptrAt00->resetChildrenAt68(p);
    selected = Resolve<GuiControlNativeBody50h>(child.r);
    p->vptrAt00->setActiveAt30(p, selected);
    p->inputLatchAt30 = 1;
    life.Current();
    return true;
}
bool Advance(Common* p, std::int32_t index, std::int32_t start)
{
    auto next = start;
    for (;;)
    {
        auto* navigation = Navigation(p, index);
        next = navigation->vptrAt00->nextAt00(navigation, next);
        if (next < 0 || next == start) return false;
        if (Focus(p, next, true)) return true;
    }
}
std::int32_t Visual(const Slot& arrow)
{
    auto* child = Resolve<GuiControlNativeBody50h>(arrow);
    if (!child->vptrAt00->readStateAt48(child)) return 7;
    child = Resolve<GuiControlNativeBody50h>(arrow);
    return child->vptrAt00->resourceStateAt38(child);
}
void RepeatArrow(Common* p, const Slot& arrow, std::int32_t previous, std::int32_t current, std::int32_t event)
{
    auto& b = Popup(p);
    if (previous != current)
    {
        if (current == 5 && Read(b.mouseRepeatEventAtB0) == -1)
        {
            Slot clicked = Copy(Resolve<GuiControlNativeBody50h>(arrow)->eventAt3C);
            Local life{clicked};
            Invoke(clicked);
            b.mouseRepeatCountAtAC = 20;
            b.mouseRepeatEventAtB0 = event;
            life.Current(true);
        }
        return;
    }
    if (Read(b.mouseRepeatEventAtB0) != event) return;
    if (previous != 5) { b.mouseRepeatEventAtB0 = -1; return; }
    const auto count = Read(b.mouseRepeatCountAtAC);
    if (count) { b.mouseRepeatCountAtAC = Add(count, -1); return; }
    Slot clicked = Copy(Resolve<GuiControlNativeBody50h>(arrow)->eventAt3C);
    Local life{clicked};
    Invoke(clicked);
    b.mouseRepeatCountAtAC = 1;
    life.Current(true);
}
}
void __fastcall ResetInput(Common* p, void*)
{
    Popup(p).keyboardLatchAtB4 = 0;
    Popup(p).navigationActionAtB8 = 255;
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
std::int32_t __fastcall PriorityInput(Common*, void*) { return 0; } // 511610真实xor eax,eax;ret。
std::int32_t __fastcall ActiveInput(Common* p, void*)
{
    if (!Read(p->activeAt4C)) return 0;
    const auto event = Read(p->activeAt4C->eventAt04);
    if (event < 60 || event >= 60 + VisibleRows(PageCount()) ||
        !p->activeAt4C->vptrAt00->enabledAt1C(p->activeAt4C)) return 0;
    const auto latch = Read(Popup(p).keyboardLatchAtB4);
    if (!latch)
    {
        if (!p->vptrAt00->pressedAt70(p, 5)) return 0;
        Popup(p).keyboardLatchAtB4 = 1;
        auto* child = Read(p->activeAt4C);
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot clicked = Copy(Read(p->activeAt4C)->eventAt3C);
        Local life{clicked};
        Invoke(clicked);
        Popup(p).keyboardLatchAtB4 = 0;
        life.Current(true);
    }
    return latch; // 原EDI返回进入时的值，而非刚清写的新值。
}
std::int32_t __fastcall UpperInput(Common* p, void*)
{
    if (p->vptrAt00->movedAt7C(p) || !p->vptrAt00->mouseReleaseAt84(p))
    { p->vptrAt00->resetInputAt3C(p); return 0; }
    if (Read(p->activeAt4C) && p->vptrAt00->activeInputAt38(p)) return 1;
    auto result = Read(p->inputLatchAt30) ? 2 : 0;
    auto& b = Popup(p);
    if (Read(b.navigationActionAtB8) != 255 && Read(p->activeAt4C))
    {
        const auto count = Count(p->navigationAt24);
        std::int32_t index = 0;
        for (; index < count; ++index)
            if (Read(Navigation(p,index)->inputAt04) == Read(b.navigationActionAtB8)) break;
        if (index < count)
        {
            if (!p->vptrAt00->heldAt6C(p, Read(b.navigationActionAtB8))) b.navigationActionAtB8 = 255;
            else if (const auto repeat = Read(b.navigationRepeatAtBC)) b.navigationRepeatAtBC = Add(repeat,-1);
            else
            {
                const auto start = Read(Read(p->activeAt4C)->eventAt04);
                if (Advance(p,index,start)) result = 3;
                else TriggerBoundary(p,index,true);
                b.navigationRepeatAtBC = 1;
            }
        }
    }
    for (std::int32_t index = 0; index < Count(p->navigationAt24); ++index)
    {
        const auto action = Read(Navigation(p,index)->inputAt04);
        if (!p->vptrAt00->pressedAt70(p, action)) continue;
        if (!Read(p->activeAt4C))
        {
            for (std::int32_t pair = 0;; ++pair)
            {
                auto* navigation = Navigation(p,index);
                const auto begin = Read(navigation->pairsAt08.begin);
                const auto end = Read(navigation->pairsAt08.end);
                const auto bytes = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
                    reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin)));
                if (pair >= bytes / 8) break;
                const auto event = Read(Read(Navigation(p,index)->pairsAt08.begin)[pair].sourceEventValue);
                if (Focus(p,event,false)) { result = 3; break; }
            }
        }
        else if (Advance(p,index,Read(Read(p->activeAt4C)->eventAt04))) result = 3;
        if (result != 3) TriggerBoundary(p,index,false);
        b.navigationActionAtB8 = Read(Navigation(p,index)->inputAt04);
        b.navigationRepeatAtBC = 20;
        return result;
    }
    return result;
}
void __fastcall ProcessInput(Common* p, void*)
{
    Fresh upper;
    p->vptrAt00->findChildAt14(p, &upper.r, 58);
    Local upperLife{upper.r};
    const auto beforeUpper = Visual(upper.r);
    Fresh lower;
    p->vptrAt00->findChildAt14(p, &lower.r, 59);
    Local lowerLife{lower.r};
    const auto beforeLower = Visual(lower.r);
    ProcessGuiDialogInputAt518C30(*p);
    const auto afterUpper = Visual(upper.r);
    const auto afterLower = Visual(lower.r);
    RepeatArrow(p,upper.r,beforeUpper,afterUpper,58);
    RepeatArrow(p,lower.r,beforeLower,afterLower,59);
    lowerLife.Current(true);
    upperLife.Current();
}
}
