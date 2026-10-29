#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
bool SkipAnimation()
{
    auto& source = RequireProgramMain00DialogNativeServices().source;
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(source.InputAt418210(0x51BB5B)->keyboardAt18);
    auto* at = Read(keyboard->actionsAt14[0].begin);
    auto* end = Read(keyboard->actionsAt14[0].end);
    if (at != end)
    {
        const auto buffer = Read(keyboard->bufferAt10);
        for (; at != end; ++at)
            if (Read(at->states[buffer]) && !Read(at->states[1u - buffer])) return true;
    }
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(source.InputAt418210(0x51BBAB)->mouseAt00);
    if (!Read(mouse->previousAt30[0]) && Read(mouse->pressedAt10[0]) && !Read(mouse->suppressionAt28)) return true;
    mouse = Resolve<MouseManagerNativeBody3Ch>(source.InputAt418210(0x51BBE2)->mouseAt00);
    return !Read(mouse->previousAt30[1]) && Read(mouse->pressedAt10[1]) && !Read(mouse->suppressionAt28);
}
std::int32_t FirstEvent(GuiClickedEventReferences& events, bool clear)
{
    auto* at = Read(events.begin);
    auto* end = Read(events.end);
    for (; at != end; ++at)
    {
        auto* event = Resolve<GuiClickedEventNativeBody>(*at);
        if (!Read(event->clickedFlagAt04)) continue;
        event = Resolve<GuiClickedEventNativeBody>(*at);
        const auto value = Read(event->eventValueAt08);
        if (clear)
        { event = Resolve<GuiClickedEventNativeBody>(*at); Read(event->vptrAt00)->clearAt04(event); }
        return value;
    }
    return -1;
}
}
void AnimateProgramMain00DialogNativeAt51BB50(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    if (SkipAnimation())
    {
        if (Read(b.stageAt9C) < 3) b.stageAt9C = 3;
        b.delayAtB4 = 0;
    }
    else if (Read(b.delayAtB4)) { b.delayAtB4 = Sub(Read(b.delayAtB4), 1); return; }
    bool repeat = false;
    for (;;)
    {
        switch (Read(b.stageAt9C))
        {
        case 0:
        {
            const auto before = Read(b.weightBeforeAtA0), amount = Read(b.amountAtB8);
            if (Sub(before, amount) == Read(b.weightAfterAtA4))
            { repeat = true; b.stageAt9C = 1; continue; }
            b.amountAtB8 = Add(amount, 1);
            RenderProgramMain00DialogNativeAt5279C0(b, Read(b.characterAt8C), std::uint32_t(Read(b.stressBeforeAtA8)),
                Read(b.trainingLevelAtB0), Sub(before, Add(amount, 1)), 0, 1, 0, 1, 1);
            b.delayAtB4 = 4;
            PlaySe(SoundReceiverOrder::BeforeVolume,0x61A670, 0x51BCF9, 0x51BD00, 0x51BD35, 0x51BD3A, 0x51BD77);
            break;
        }
        case 1:
        {
            std::int32_t stage = 1;
            if (Read(b.weightBeforeAtA0) != Read(b.weightAfterAtA4))
            {
                RenderProgramMain00DialogNativeAt5279C0(b, Read(b.characterAt8C), std::uint32_t(Read(b.stressBeforeAtA8)),
                    Read(b.trainingLevelAtB0), Read(b.weightAfterAtA4), 0, 1, 0, 0, 1);
                stage = Read(b.stageAt9C);
                repeat = false;
                b.delayAtB4 = 20;
            }
            b.amountAtB8 = 0;
            b.stageAt9C = Add(stage, 1);
            break;
        }
        case 2:
        {
            const auto amount = Read(b.amountAtB8), before = Read(b.stressBeforeAtA8);
            if (Add(before, amount) == Read(b.stressAfterAtAC))
            { repeat = true; b.stageAt9C = 3; continue; }
            b.amountAtB8 = Add(amount, 1);
            RenderProgramMain00DialogNativeAt5279C0(b, Read(b.characterAt8C), std::uint32_t(Add(before, Add(amount, 1))),
                Read(b.trainingLevelAtB0), Read(b.weightAfterAtA4), 0, 1, 1, 0, 1);
            b.delayAtB4 = 10;
            PlaySe(SoundReceiverOrder::BeforeVolume,0x61A674, 0x51BEAE, 0x51BEB5, 0x51BEE8, 0x51BEED, 0x51BF24);
            return;
        }
        case 3:
            RenderProgramMain00DialogNativeAt5279C0(b, Read(b.characterAt8C), std::uint32_t(Read(b.stressAfterAtAC)),
                Read(b.trainingLevelAtB0), Read(b.weightAfterAtA4), 0, 0, 0, 0, 1);
            b.stageAt9C = Add(Read(b.stageAt9C), 1);
            b.delayAtB4 = 20;
            return;
        case 4: b.phaseAt98 = Add(Read(b.phaseAt98), 1); break;
        default: break;
        }
        if (!repeat) return;
    }
}
std::int32_t PollProgramMain00DialogNativeAt52C990(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    Methods(b).processInputAt54(&b.common);
    bool repeat = false;
    for (;;)
    {
        switch (Read(b.phaseAt98))
        {
        case 0:
        {
            auto* manager = Resolve<GuiClickedEventReferences>(b.eventsAt5C);
            auto* at = Read(manager->begin);
            auto* end = Read(manager->end);
            for (; at != end; ++at)
            {
                if (!Read(Resolve<GuiClickedEventNativeBody>(*at)->clickedFlagAt04)) continue;
                auto* event = Resolve<GuiClickedEventNativeBody>(*at);
                const auto value = Read(event->eventValueAt08);
                event = Resolve<GuiClickedEventNativeBody>(*at);
                Read(event->vptrAt00)->clearAt04(event);
                if (value < 0) break;
                switch (value)
                {
                case 40: case 41: case 42:
                    b.characterAt8C = Sub(value, 40);
                    RefreshProgramMain00DialogNativeAt5009C0(b, 1);
                    InvalidateScreen(b, false, 0x52CAC8, 0x52CAD0);
                    PlaySe(SoundReceiverOrder::AfterVolume,0x61A658, 0x52CB01, 0x52CB08, 0x52CB23, 0x52CB2C, 0x52CB49);
                    break;
                case 43: case 44: case 45:
                    b.operationAtBC = Sub(value, 43);
                    if (!PrepareProgramMain00DialogNativeAt50BCC0(b)) return 1;
                    b.phaseAt98 = Add(Read(b.phaseAt98), 1);
                    repeat = true;
                    PlaySe(SoundReceiverOrder::AfterVolume,0x61A65C, 0x52CB7D, 0x52CB84, 0x52CB9F, 0x52CBA8, 0x52CBC5);
                    break;
                case 47: b.characterAt8C = -1; [[fallthrough]];
                case 46:
                    b.operationAtBC = Sub(value, 43);
                    PlaySe(SoundReceiverOrder::BeforeVolume,0x61A658, 0x52CBFE, 0x52CC05, 0x52CC38, 0x52CC3D, 0x52CC78);
                    return 0;
                default: break;
                }
                break;
            }
            break;
        }
        case 1:
        {
            RenderProgramMain00DialogNativeAt5279C0(b, Read(b.characterAt8C), std::uint32_t(Read(b.stressBeforeAtA8)),
                Read(b.trainingLevelAtB0), Read(b.weightBeforeAtA0), 0, 1, 0, 0, 0);
            RefreshProgramMain00DialogNativeAt5009C0(b, 0);
            InvalidateScreen(b, true, 0x52CD06, 0x52CE0E, 0x52CDE4, 0x52CEEC);
            auto* parent = Read(b.common.parentAt04);
            const auto* parentMethods = reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(parent->vptrAt00));
            Slot system;
            parentMethods->findDialogAt90(parent, &system, 0);
            RefFrame systemFrame{system};
            auto* selected = Resolve<Common>(system);
            if (Read(selected->vptrAt00)->readStateAt20(selected))
            { selected = Resolve<Common>(system); Read(selected->vptrAt00)->writeStateAt1C(selected, 0); }
            systemFrame.Reload(false);
            b.phaseAt98 = Add(Read(b.phaseAt98), 1);
            b.stageAt9C = 0; b.amountAtB8 = 0; b.delayAtB4 = 60;
            return 1;
        }
        case 2: AnimateProgramMain00DialogNativeAt51BB50(b); break;
        case 3: CommitProgramMain00DialogNativeAt508860(b); return 0;
        default: break;
        }
        if (!repeat) return 1;
    }
}
void ChangeProgramMain00DialogNativeAt518820(ProgramMain00DialogNativeBodyC0h& b, YtlReferenceSlotRuntime ownedChild)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    RefFrame argumentFrame{ownedChild};
    const auto value = Read(Resolve<GuiControlNativeBody50h>(ownedChild)->eventAt04);
    switch (value)
    {
    case 43: case 44: case 45:
    {
        const auto record = std::uint32_t(TrainingRecordProgramMain00DialogNativeAt50B470(b, Sub(value, 43)));
        if (record < 27)
        {
            Slot label;
            Methods(b).findChildAt14(&b.common, &label, ProgramMain00DialogNativeRecordsSample18()[record].event);
            RefFrame labelFrame{label};
            if (Read(label.baseAt00))
            {
                auto* child = Resolve<GuiControlNativeBody50h>(ownedChild);
                const auto state = Read(child->vptrAt00)->resourceStateAt38(child);
                child = Resolve<GuiControlNativeBody50h>(label);
                Read(child->vptrAt00)->writeEnabledAt4C(child, state == 3 || state == 4 || state == 5);
                child = Resolve<GuiControlNativeBody50h>(label);
                const auto invalidate = Methods(b).invalidateAt64;
                RECT rectangle;
                auto* selected = Read(child->vptrAt00)->rectAt3C(child, &rectangle);
                invalidate(&b.common, *selected);
            }
            labelFrame.Reload();
        }
        [[fallthrough]];
    }
    case 40: case 41: case 42: case 46: case 47:
    {
        auto* child = Resolve<GuiControlNativeBody50h>(ownedChild);
        const auto state = Read(child->vptrAt00)->resourceStateAt38(child);
        if ((state == 3 || state == 4) && value != FirstEvent(*Resolve<GuiClickedEventReferences>(b.eventsAt5C), false))
            PlaySe(SoundReceiverOrder::BeforeVolume,0x61A65C, 0x51899A, 0x5189A1, 0x5189D4, 0x5189D9, 0x518A14);
        break;
    }
    default: break;
    }
    argumentFrame.Reload(false);
}
}
