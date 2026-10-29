#include "SaveLoadSystemPageNative.Internal.h"

namespace KaGuYa::Start::Runtime::SaveLoadSystemPageNativeDetail
{
std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences* manager, std::int32_t clear)
{
    auto* position = manager->begin;
    auto* const end = manager->end;
    for (; position != end; ++position)
    {
        const auto base = Read(position->baseAt00);
        if (!Read(ResolveWithBase<GuiClickedEventNativeBody>(*position, base)->clickedFlagAt04)) continue;
        const auto result = Read(ResolveWithBase<GuiClickedEventNativeBody>(*position, base)->eventValueAt08);
        if (clear)
        {
            auto* selected = ResolveWithBase<GuiClickedEventNativeBody>(*position, base);
            selected->vptrAt00->clearAt04(selected);
        }
        return result;
    }
    return -1;
}
void __fastcall ResetInput(Common* p, void*)
{
    Page(p).keyboardEventAt7C = -1;
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
namespace
{
void SetPressed(GuiControlNativeBody50h* child)
{
    const auto* t = reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00);
    t->setPressedAt70(child, 1);
}
// 511B08/511B64的释放路径：无child不清7C，存在child即使无event也清7C。
void CompleteKeyboard(Common* p, std::int32_t event)
{
    FreshReference child;
    p->vptrAt00->findChildAt14(p, &child.slot, event);
    LocalReference childLifetime{child.slot};
    if (child.slot.baseAt00)
    {
        auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
        Slot clicked = Copy(selected->eventAt3C);
        LocalReference clickedLifetime{clicked};
        if (clicked.baseAt00)
        {
            auto* receiver = Resolve<GuiClickedEventNativeBody>(clicked);
            receiver->vptrAt00->invokeAt00(receiver);
        }
        Page(p).keyboardEventAt7C = -1;
        clickedLifetime.Reload();
    }
    childLifetime.Reload();
}
}
std::int32_t __fastcall PriorityInput(Common* p, void*)
{
    const auto pending = Read(Page(p).keyboardEventAt7C);
    if (pending == 12)
    {
        if (!p->vptrAt00->heldAt6C(p, 6))
        { p->vptrAt00->resetInputAt3C(p); return 1; }
        if (!p->vptrAt00->heldAt6C(p, 8)) CompleteKeyboard(p, 12);
        return 1;
    }
    if (pending >= 0)
    {
        if (!p->vptrAt00->heldAt6C(p, 11)) CompleteKeyboard(p, Read(Page(p).keyboardEventAt7C));
        return 1;
    }
    if (p->vptrAt00->heldAt6C(p, 6) && p->vptrAt00->pressedAt70(p, 8))
    {
        FreshReference child;
        p->vptrAt00->findChildAt14(p, &child.slot, 12);
        LocalReference childLifetime{child.slot};
        const bool found = child.slot.baseAt00 != 0;
        if (found)
        {
            SetPressed(Resolve<GuiControlNativeBody50h>(child.slot));
            Page(p).keyboardEventAt7C = 12;
        }
        childLifetime.Reload();
        return found ? 1 : 0; // 即使没有12也不转去action11。
    }
    if (!p->vptrAt00->pressedAt70(p, 11)) return 0;
    const auto shortcuts = SaveLoadSystemPageNativeShortcuts();
    for (unsigned i = 0; i != 6; i += 2)
    {
        FreshReference current;
        p->vptrAt00->findChildAt14(p, &current.slot, shortcuts[i]);
        LocalReference currentLifetime{current.slot};
        if (!current.slot.baseAt00) { currentLifetime.Reload(); continue; }
        auto* selected = Resolve<GuiControlNativeBody50h>(current.slot);
        if (selected->vptrAt00->readStateAt48(selected)) { currentLifetime.Reload(); continue; }
        FreshReference next;
        p->vptrAt00->findChildAt14(p, &next.slot, shortcuts[i + 1]);
        LocalReference nextLifetime{next.slot};
        bool accepted = false;
        if (next.slot.baseAt00)
        {
            selected = Resolve<GuiControlNativeBody50h>(next.slot);
            if (selected->vptrAt00->enabledAt1C(selected))
            {
                selected = Resolve<GuiControlNativeBody50h>(next.slot);
                const auto state = selected->vptrAt00->resourceStateAt38(selected);
                if (state == 0 || state == 4)
                {
                    p->vptrAt00->resetChildrenAt68(p);
                    SetPressed(Resolve<GuiControlNativeBody50h>(next.slot));
                    Page(p).keyboardEventAt7C = shortcuts[i + 1];
                    accepted = true;
                }
            }
        }
        nextLifetime.Reload();
        currentLifetime.Reload();
        return accepted ? 1 : 0; // 找到state0后只尝试其配对，不扫描后续配对。
    }
    return 0;
}
}
