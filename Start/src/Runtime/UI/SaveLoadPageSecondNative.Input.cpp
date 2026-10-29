#include "SaveLoadPageSecondNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageSecondNativeDetail;
void InvokeChildEvent(GuiControlNativeBody50h* child)
{
    Slot clicked = Copy(child->eventAt3C);
    LocalReference lifetime{clicked};
    Invoke(clicked);
    lifetime.Reload(); // 4F64D0，正常与EH均payload后重读control。
}
void ResetKeyboardPage(Common* p)
{
    Page(p).keyboardEventAt98 = -1;
    p->vptrAt00->resetChildrenAt68(p);
    p->vptrAt00->setActiveAt30(p, nullptr);
}
}
void ResetInputSaveLoadPageSecondNativeAt510790(Common* p)
{
    const auto* table = p->vptrAt00;
    Page(p).keyboardEventAt98 = -1;
    Page(p).activeModeAtA8 = -1;
    Page(p).repeatEventAtA4 = -1;
    table->setActiveAt30(p, nullptr); // 510795先选表，三个字段写入后才读+30槽。
    p->inputLatchAt30 = 0;
}
std::int32_t ActiveInputSaveLoadPageSecondNativeAt511080(Common* p)
{
    const auto mode = Read(Page(p).activeModeAtA8);
    const auto pending = mode >= 0 ? 1 : 0;
    if (mode == 0)
    {
        if (!p->vptrAt00->heldAt6C(p, 5))
        {
            Slot clicked = Copy(Read(p->activeAt4C)->eventAt3C);
            LocalReference lifetime{clicked};
            auto* const cached = clicked.controlAt08; // 51127B/5112CD..E9缓存EDI的正常尾。
            Invoke(clicked);
            p->vptrAt00->resetChildrenAt68(p);
            auto* active = Read(p->activeAt4C);
            active->vptrAt00->focusAt20(active);
            Page(p).activeModeAtA8 = -1;
            lifetime.Captured(cached, false);
        }
        return pending;
    }
    if (mode == 1)
    {
        if (!p->vptrAt00->heldAt6C(p, 8))
        {
            const auto event = Add(Read(p->activeAt4C)->eventAt04, 16);
            FreshReference child;
            p->vptrAt00->findChildAt14(p, &child.slot, event);
            LocalReference lifetime{child.slot};
            if (Read(child.slot.baseAt00))
            {
                Slot clicked = Copy(Resolve<GuiControlNativeBody50h>(child.slot)->eventAt3C);
                LocalReference eventLifetime{clicked};
                Invoke(clicked);
                p->vptrAt00->resetChildrenAt68(p);
                auto* active = Read(p->activeAt4C);
                active->vptrAt00->focusAt20(active);
                Page(p).activeModeAtA8 = -1;
                eventLifetime.Reload();
            }
            lifetime.Reload(false); // 511221正常内联尾不清写child。
        }
        return pending;
    }
    if (p->vptrAt00->pressedAt70(p, 5))
    {
        SetPressed(Read(p->activeAt4C));
        Page(p).activeModeAtA8 = 0;
        return 1;
    }
    if (p->vptrAt00->heldAt6C(p, 6) || !p->vptrAt00->pressedAt70(p, 8)) return pending;
    const auto event = Add(Read(p->activeAt4C)->eventAt04, 16);
    FreshReference child;
    p->vptrAt00->findChildAt14(p, &child.slot, event);
    LocalReference lifetime{child.slot};
    bool accepted = false;
    if (Read(child.slot.baseAt00))
    {
        auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
        if (selected->vptrAt00->readStateAt48(selected))
        {
            SetPressed(Resolve<GuiControlNativeBody50h>(child.slot));
            Page(p).activeModeAtA8 = 1;
            accepted = true;
        }
    }
    lifetime.Reload();
    return accepted ? 1 : pending;
}
std::int32_t PriorityInputSaveLoadPageSecondNativeAt511620(Common* p)
{
    const auto pending = Read(Page(p).keyboardEventAt98);
    if (pending == 50 || pending == 51)
    {
        FreshReference child;
        p->vptrAt00->findChildAt14(p, &child.slot, pending);
        LocalReference lifetime{child.slot};
        bool valid = false;
        if (Read(child.slot.baseAt00))
        {
            auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
            valid = selected->vptrAt00->readStateAt48(selected) != 0;
        }
        if (!valid) ResetKeyboardPage(p);
        else if (!p->vptrAt00->heldAt6C(p, pending == 51 ? 9 : 10))
        {
            ResetKeyboardPage(p);
            // reset/blur回调后重读两页号；跨页释放不再次触发翻页事件。
            if (Read(Page(p).pressPageAt9C) == Read(Page(p).pageAt7C))
                InvokeChildEvent(Resolve<GuiControlNativeBody50h>(child.slot));
        }
        lifetime.Reload(false); // 51194D/511A5C内联终止尾，EH用4F6460。
        return 1;
    }
    if (pending == 52)
    {
        if (!p->vptrAt00->heldAt6C(p, 27))
        {
            FreshReference child;
            p->vptrAt00->findChildAt14(p, &child.slot, 52);
            LocalReference lifetime{child.slot};
            if (Read(child.slot.baseAt00))
            {
                Slot clicked = Copy(Resolve<GuiControlNativeBody50h>(child.slot)->eventAt3C);
                LocalReference eventLifetime{clicked};
                Invoke(clicked);
                p->vptrAt00->resetChildrenAt68(p);
                Page(p).keyboardEventAt98 = -1;
                eventLifetime.Reload();
            }
            lifetime.Reload(); // 不存在52时98仍为52；这里不清active。
        }
        return 1;
    }
    // 按27、9、10的优先级；命中但缺child/禁用时也不试下一键。
    for (const auto action : {27, 9, 10})
    {
        if (!p->vptrAt00->pressedAt70(p, action)) continue;
        const auto event = action == 27 ? 52 : (action == 9 ? 51 : 50);
        FreshReference child;
        p->vptrAt00->findChildAt14(p, &child.slot, event);
        LocalReference lifetime{child.slot};
        bool accepted = false;
        if (Read(child.slot.baseAt00))
        {
            auto* selected = Resolve<GuiControlNativeBody50h>(child.slot);
            if (event == 52 || selected->vptrAt00->readStateAt48(selected))
            {
                SetPressed(Resolve<GuiControlNativeBody50h>(child.slot));
                if (event != 52)
                {
                    // 51171B/5117AF在解析child前缓存表地址，回调后才读+30槽。
                    const auto* table = p->vptrAt00;
                    selected = Resolve<GuiControlNativeBody50h>(child.slot);
                    table->setActiveAt30(p, selected);
                    const auto page = Read(Page(p).pageAt7C);
                    Page(p).keyboardEventAt98 = event;
                    Page(p).pressPageAt9C = page;
                }
                else Page(p).keyboardEventAt98 = 52;
                accepted = true;
            }
        }
        lifetime.Reload();
        return accepted ? 1 : 0;
    }
    return 0;
}
}
