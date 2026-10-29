#include "GraffitiCountDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::GraffitiCountDialogNativeDetail::Entries
{
namespace
{
std::int32_t Poll(Common& body, std::int32_t& lifecycle)
{
    auto& dialog = body.dialog;
    switch (Read(lifecycle))
    {
    case 0: lifecycle = 3; [[fallthrough]];
    case 3: Read(dialog.vptrAt00)->processInputAt54(&dialog); return 1;
    case 4:
        AddFullScreen(body);
        Read(dialog.vptrAt00)->writeEnabledAt24(&dialog, 0);
        lifecycle = -1;
        return 1;
    default: return 0;
    }
}
void BaseInput(Common& body) { ProcessGuiDialogInputAt518C30(body.dialog); }
}
void __fastcall BeginLikable(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Likable*>(dialog);
    Read(dialog->vptrAt00)->writeEnabledAt24(dialog, 1);
    RefreshGraffitiLikableAt528770(body, Read(body.initialAt94));
    AddFullScreen(body.common);
    dialog->activeAt4C = nullptr; // 原直接写，不调用blur/reset虚槽。
    body.delayAtAC = ReadNamed("prog_param04");
    body.incrementAt9C = 0;
    body.lifecycleAtA0 = 0;
    body.animationAtA4 = 0;
    body.resumeAtA8 = 1;
    body.skipAtB0 = 0;
}
void __fastcall BeginWeight(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Weight*>(dialog);
    Read(dialog->vptrAt00)->writeEnabledAt24(dialog, 1);
    SetGraffitiWeightWidgetsAt5398B0(body, 0);
    AddFullScreen(body.common);
    dialog->activeAt4C = nullptr;
    body.lifecycleAtA0 = 0;
    body.delayAtB0 = 0;
    body.animationAtA4 = 0;
    body.resumeAtA8 = 1;
    body.skipAtB4 = 0;
    body.skipResumeAtAC = 1;
    // 50FBD0不写+98，第一次case1才写decrement=0。
}
std::int32_t __fastcall PollLikable(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Likable*>(dialog);
    return Poll(body.common, body.lifecycleAtA0);
}
std::int32_t __fastcall PollWeight(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Weight*>(dialog);
    return Poll(body.common, body.lifecycleAtA0);
}
std::int32_t __fastcall EndLikable(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Likable*>(dialog);
    WriteNamedLate("prog_param02", body.targetAt98);
    return 1;
}
std::int32_t __fastcall EndWeight(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Weight*>(dialog);
    WriteNamedLate("prog_param01", body.targetAt94);
    return 1;
}
void __fastcall InputLikable(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Likable*>(dialog);
    if (!Read(body.skipAtB0) && SkipRequested() != SkipReason::None) body.skipAtB0 = 1;
    if (Read(body.skipAtB0)) body.delayAtAC = 0;
    // 原同一帧的0->resume、1完成->2转换，不人为拆成多个帧。
    for (;;)
    {
        switch (Read(body.animationAtA4))
        {
        case 0:
            if (Read(body.delayAtAC))
            {
                body.delayAtAC = Sub(Read(body.delayAtAC), 1);
                BaseInput(body.common);
                return;
            }
            body.animationAtA4 = Read(body.resumeAtA8);
            continue;
        case 1:
        {
            const auto increment = Read(body.incrementAt9C);
            const auto initial = Read(body.initialAt94);
            if (Add(initial, increment) == Read(body.targetAt98))
            { body.animationAtA4 = 2; continue; }
            body.incrementAt9C = Add(increment, 1);
            RefreshGraffitiLikableAt528770(body, Add(initial, Add(increment, 1)));
            body.delayAtAC = ReadNamed("prog_param05", true);
            AddDialogRectangle(body.common);
            PlayCue(0x61A688);
            body.animationAtA4 = 0;
            BaseInput(body.common);
            return;
        }
        case 2:
            body.animationAtA4 = 3;
            body.resumeAtA8 = 3;
            RefreshGraffitiLikableAt528770(body, Read(body.targetAt98));
            AddDialogRectangle(body.common);
            body.delayAtAC = ReadNamed("prog_param06");
            body.animationAtA4 = 0;
            BaseInput(body.common);
            return;
        case 3:
            body.lifecycleAtA0 = 4;
            [[fallthrough]];
        default:
            BaseInput(body.common);
            return;
        }
    }
}
void __fastcall InputWeight(Dialog* dialog, void*)
{
    auto& body = *reinterpret_cast<Weight*>(dialog);
    if (!Read(body.skipAtB4))
    {
        const auto reason = SkipRequested();
        if (reason == SkipReason::Latched) body.skipAtB4 = 1;
        else if (reason == SkipReason::Action5)
        {
            body.animationAtA4 = Read(body.skipResumeAtAC);
            body.delayAtB0 = 0; // 51B28D直接落到51B299，不设置skip锁存。
        }
    }
    if (Read(body.skipAtB4)) body.delayAtB0 = 0;
    for (;;)
    {
        switch (Read(body.animationAtA4))
        {
        case 0:
            if (Read(body.delayAtB0))
            {
                body.delayAtB0 = Sub(Read(body.delayAtB0), 1);
                BaseInput(body.common);
                return;
            }
            body.animationAtA4 = Read(body.resumeAtA8);
            continue;
        case 1:
            SetGraffitiWeightWidgetsAt5398B0(body, 1);
            RefreshGraffitiWeightAt528A80(body, Read(body.initialAt90));
            body.delayAtB0 = ReadNamed("prog_param04", true);
            AddDialogRectangle(body.common);
            body.animationAtA4 = Add(Read(body.animationAtA4), 1);
            body.decrementAt98 = 0;
            body.skipResumeAtAC = 3;
            BaseInput(body.common);
            return;
        case 2:
        {
            const auto initial = Read(body.initialAt90);
            const auto decrement = Read(body.decrementAt98);
            if (Sub(initial, decrement) == Read(body.targetAt94))
            { body.animationAtA4 = 3; continue; }
            body.decrementAt98 = Add(decrement, 1);
            RefreshGraffitiWeightAt528A80(body, Sub(initial, Add(decrement, 1)));
            body.delayAtB0 = ReadNamed("prog_param05", true);
            AddDialogRectangle(body.common);
            body.resumeAtA8 = Read(body.animationAtA4);
            PlayCue(0x61A68C);
            body.animationAtA4 = 0;
            BaseInput(body.common);
            return;
        }
        case 3:
            body.animationAtA4 = 4;
            body.resumeAtA8 = 4;
            body.skipResumeAtAC = 4;
            RefreshGraffitiWeightAt528A80(body, Read(body.targetAt94));
            AddDialogRectangle(body.common);
            body.delayAtB0 = ReadNamed("prog_param06", true);
            body.animationAtA4 = 0;
            BaseInput(body.common);
            return;
        case 4:
            if (!Read(body.trainingAt9C)) { body.animationAtA4 = 6; continue; }
            SetGraffitiWeightWidgetsAt5398B0(body, 2);
            AddDialogRectangle(body.common);
            if (!Read(body.skipAtB4))
            {
                PlayCue(0x61A690);
                body.animationAtA4 = 5;
            }
            else body.animationAtA4 = 6;
            body.skipResumeAtAC = 6;
            BaseInput(body.common);
            return;
        case 5:
        {
            auto& s = RequireGraffitiCountDialogNativeServices();
            const auto sound = s.source.CaptureSoundAfter413BE0();
            const auto slot = sound.remainingBeginAt20(sound.object,sound.service);
            if (sound.isPlayingAt419370(sound.object,sound.service, slot))
            { BaseInput(body.common); return; }
            body.delayAtB0 = ReadNamed("prog_param06", true);
            body.resumeAtA8 = 6;
            body.animationAtA4 = 0;
            BaseInput(body.common);
            return;
        }
        case 6:
            body.lifecycleAtA0 = 4;
            [[fallthrough]];
        default:
            BaseInput(body.common);
            return;
        }
    }
}
}
