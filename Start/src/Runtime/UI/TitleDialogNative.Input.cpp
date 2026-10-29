#include "TitleDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::TitleDialogNativeDetail
{
std::int32_t PollManagerAt50FDB0(GuiClickedEventReferences* manager, std::int32_t clear)
{
    auto* position = manager->begin;
    auto* const end = manager->end;
    for (; position != end; ++position)
    {
        // 50FDC0 captures base, but each subsequent resolution reloads bias/control.
        const auto base = position->baseAt00;
        auto resolveCapturedBase = [&]() -> GuiClickedEventNativeBody*
        {
            if (!base) return nullptr;
            const auto stride = position->controlAt08 ?
                static_cast<EnvironmentObjectReferenceControl*>(position->controlAt08)->strideAt0C : 0u;
            return reinterpret_cast<GuiClickedEventNativeBody*>(base + position->biasAt04 * stride);
        };
        if (!resolveCapturedBase()->clickedFlagAt04) continue;
        const auto event = resolveCapturedBase()->eventValueAt08;
        if (clear)
        {
            auto* receiver = resolveCapturedBase();
            receiver->vptrAt00->clearAt04(receiver);
        }
        return event;
    }
    return -1;
}
void PlayCue(std::int32_t channel, bool hover)
{
    auto& source = RequireTitleDialogNativeServices().source;
    auto* params = source.SoundParamsAt4188C0(Registry(source, 1));
    const auto* name = hover ? source.HoverSoundAt61A65C() : source.ConfirmSoundAt61A658();
    auto* root = Root(source);
    const auto volume = *source.EffectVolumeAtVslot24(params);
    auto* sound = source.SoundAt4188F0(root);
    (void)source.StartSoundAt4BCD70(sound, name, channel, volume, 1);
}
std::int32_t __fastcall ActiveInput(Common* common, void*)
{
    auto& body = Dialog(common);
    const auto type = common->activeAt4C->typeAt08;
    if (type != 0 && type != 2) return 0;
    const auto latch = body.keyboardLatchAtC4;
    if (!latch)
    {
        if (!common->vptrAt00->pressedAt70(common, 5)) return 0;
        auto* child = common->activeAt4C;
        body.keyboardLatchAtC4 = 1;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!common->vptrAt00->heldAt6C(common, 5))
    {
        Slot event = Copy(common->activeAt4C->eventAt3C);
        LocalReference lifetime{event};
        auto* control = event.controlAt08;
        if (event.baseAt00)
        {
            auto* receiver = ResolveGuiClickedEvent(event);
            receiver->vptrAt00->invokeAt00(receiver);
        }
        body.keyboardLatchAtC4 = 0;
        lifetime.Captured(control, false);
    }
    return latch; // Original EBX, not normalized bool, and not the now-cleared field.
}
void __fastcall ResetInput(Common* common, void*)
{
    Dialog(common).keyboardLatchAtC4 = 0;
    common->vptrAt00->setActiveAt30(common, nullptr);
    common->inputLatchAt30 = 0;
}
void __fastcall ChildChanged(Common* common, void*, Slot argument)
{
    LocalReference lifetime{argument};
    const auto event = ResolveGuiControl(argument)->eventAt04;
    if (std::uint32_t(event) - 3u <= 5u)
    {
        auto* child = ResolveGuiControl(argument);
        const auto state = child->vptrAt00->resourceStateAt38(child);
        if ((state == 3 || state == 4) && PollManagerAt50FDB0(Manager(Dialog(common)), 0) != event)
            PlayCue(1, true);
    }
    // 518BE3 reloads argument.control after business, then again after payload.
    lifetime.Reload(false);
}
}
