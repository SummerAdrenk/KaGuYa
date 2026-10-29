#include "MusicDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
void __fastcall ResetInputAt5106F0(Common* body, void*)
{
    Read(body->vptrAt00)->setActiveAt30(body, nullptr);
    body->inputLatchAt30 = 0;
    reinterpret_cast<Body*>(body)->unwrittenAt60 = 0;
    for (auto* slot = Read(body->navigationAt24.begin); slot != Read(body->navigationAt24.end); ++slot)
    {
        auto* op = Resolve<NavigationEx24h>(*slot);
        op->countdownAt14 = 0;
        op->repeatedAt18 = 0;
    }
}
std::int32_t __fastcall ActiveInputAt510C80(Common* body, void*)
{
    auto* const active = Read(body->activeAt4C);
    if (active && Read(active->typeAt08) != 2) return 0;
    auto& latch = reinterpret_cast<Body*>(body)->unwrittenAt60;
    const auto previous = std::bit_cast<std::int32_t>(Read(latch));
    if (!previous)
    {
        if (Read(body->vptrAt00)->pressedAt70(body, 5))
        {
            latch = 1;
            reinterpret_cast<const GuiCheckBoxNativeVtable30*>(Read(active->vptrAt00))->setPressedAt70(active, 1);
            return 1;
        }
    }
    else if (!Read(body->vptrAt00)->heldAt6C(body, 5))
    {
        Slot event = Copy(active->eventAt3C);
        auto* captured = Read(event.controlAt08);
        RefLife life{event};
        if (Read(event.baseAt00))
        {
            auto* receiver = Resolve<GuiClickedEventNativeBody>(event);
            Read(receiver->vptrAt00)->invokeAt00(receiver);
        }
        reinterpret_cast<const GuiCheckBoxNativeVtable30*>(Read(active->vptrAt00))->setPressedAt70(active, 0);
        latch = 0;
        life.Captured(captured, false);
    }
    return previous;
}
namespace
{
std::int32_t Navigate(Common& body, const Slot& operation, std::int32_t fallback)
{
    auto* active = Read(body.activeAt4C);
    if (!active)
    {
        for (std::int32_t i = 0;; ++i)
        {
            auto* op = Resolve<GuiNavigationNativeBody>(operation);
            const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Read(op->pairsAt08.end)) -
                reinterpret_cast<std::uintptr_t>(Read(op->pairsAt08.begin)));
            const auto count = std::bit_cast<std::int32_t>(bytes) >> 3;
            if (i >= count) return fallback;
            op = Resolve<GuiNavigationNativeBody>(operation);
            const auto event = Read(Read(op->pairsAt08.begin)[i].sourceEventValue);
            Fresh selected;
            Read(body.vptrAt00)->findChildAt14(&body, &selected.slot, event);
            RefLife life{selected.slot};
            if (!Read(selected.slot.baseAt00)) throw 1;
            auto* child = Resolve<Control>(selected.slot);
            const auto state = Read(child->vptrAt00)->resourceStateAt38(child);
            if (state == 1 || state == 2) { life.Reload(); continue; }
            Read(body.vptrAt00)->setActiveAt30(&body, Resolve<Control>(selected.slot));
            body.inputLatchAt30 = 1;
            life.Reload(false);
            return 3;
        }
    }
    const auto first = Read(active->eventAt04);
    auto event = first;
    for (;;)
    {
        auto* op = Resolve<GuiNavigationNativeBody>(operation);
        event = Read(op->vptrAt00)->nextAt00(op, event);
        if (event < 0 || event == first) return fallback;
        Fresh selected;
        Read(body.vptrAt00)->findChildAt14(&body, &selected.slot, event);
        RefLife life{selected.slot};
        if (!Read(selected.slot.baseAt00)) throw 1;
        auto* child = Resolve<Control>(selected.slot);
        const auto state = Read(child->vptrAt00)->resourceStateAt38(child);
        if (state == 1 || state == 2) { life.Reload(); continue; }
        Read(body.vptrAt00)->resetChildrenAt68(&body);
        Read(body.vptrAt00)->setActiveAt30(&body, Resolve<Control>(selected.slot));
        body.inputLatchAt30 = 1;
        life.Reload(false);
        return 3;
    }
}
}
std::int32_t __fastcall UpperInputAt51EB70(Common* body, void*)
{
    if (Read(body->vptrAt00)->movedAt7C(body) || Read(body->vptrAt00)->mousePressAt80(body))
    {
        Read(body->vptrAt00)->resetInputAt3C(body);
        return 0;
    }
    if (Read(body->activeAt4C) && Read(body->vptrAt00)->activeInputAt38(body)) return 1;
    const auto fallback = Read(body->inputLatchAt30) ? 2 : 0;
    for (auto* slot = Read(body->navigationAt24.begin); slot != Read(body->navigationAt24.end); ++slot)
    {
        const auto input = Read(Resolve<GuiNavigationNativeBody>(*slot)->inputAt04);
        if (!Read(body->vptrAt00)->heldAt6C(body, input))
        {
            auto* op = Resolve<NavigationEx24h>(*slot);
            op->countdownAt14 = 0;
            op->repeatedAt18 = 0;
            continue;
        }
        auto* const op = Resolve<NavigationEx24h>(*slot);
        const auto countdown = Read(op->countdownAt14);
        if (countdown)
        {
            if (countdown > 0) op->countdownAt14 = countdown - 1;
            continue;
        }
        const auto result = Navigate(*body, *slot, fallback);
        if (!Read(op->repeatedAt18))
        {
            op->countdownAt14 = Read(op->firstDelayAt1C);
            op->repeatedAt18 = 1;
        }
        else op->countdownAt14 = Read(op->repeatDelayAt20);
        return result;
    }
    return fallback;
}
}
