#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <array>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = ToolWindowNativeBody40h;
using Base = GuiDialogNativeBody5Ch;
using Services = ToolWindowNativeInteractionServices;
using Source = IToolWindowNativeInteractionSource;
using ReadKind = ToolUnSaveNativeRead;
class Selected final
{
public:
    Selected(Body& body, Services& services)
    { SelectToolDialogNativeAt417B50(body, slot_, services.window.replay); }
    ~Selected() noexcept { if (active_) ReleaseYtlReference(slot_); }
    Base* Get() noexcept { return ResolveEnvironmentObjectReference<Base>(slot_); }
    void Finish(bool clear = false) noexcept
    {
        active_ = false;
        auto* control = slot_.controlAt08;
        if (!control) return;
        if (--control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (slot_.controlAt08) slot_.controlAt08->DeleteControlAtVslot04(1);
        }
        if (clear) { slot_.controlAt08 = nullptr; slot_.baseAt00 = 0; }
    }
    void ReleaseAt40D830() noexcept { active_ = false; ReleaseYtlReference(slot_); }
private:
    union { YtlReferenceSlotRuntime slot_; };
    bool active_ = true;
};
bool Held(Source& source, bool inlineLoop = false)
{
    auto* keyboard = source.CaptureKeyboardAt418210();
    return inlineLoop ? ReadKeyboardSixHeldAt4AA573(*keyboard) : ReadKeyboardHeldAt4198D0(*keyboard, 6);
}
bool CommonGate(Source& source, bool checkHeld, ReadKind initial = ReadKind::Inline)
{
    if (std::uint8_t(source.CaptureUnSave(initial)->DwordAt0C()) == 0) return false;
    if (checkHeld && Held(source)) return false;
    if ((source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt14() & 3U) == 3U) return false;
    return source.ReadTextCursorAfter413BE0() < 0;
}
bool WindowGate(Source& source, bool checkHeld)
{ return CommonGate(source, checkHeld) && (source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt20() & 1U); }
bool VoiceGate(Source& source, bool checkHeld)
{
    return CommonGate(source, checkHeld) && !(source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt18() & 2U) &&
        source.CaptureUnSave(ReadKind::Lookup417990)->ArrayReferenceAt3C().RawSlot().baseAt00 != 0;
}
bool AffectionGate(Source& source, bool checkHeld)
{
    constexpr char name[] = "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F";
    return CommonGate(source, checkHeld) && source.CaptureUnSave(ReadKind::Lookup417990)->ModeAt04() != 2 &&
        source.ReadNamedAfterRoot("replay") == 0 && source.ReadNamedAfterRoot(name) != 0 &&
        (source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt20() & 1U);
}
bool SaveLoadGate(Source& source, bool checkHeld, bool load)
{
    if (checkHeld && Held(source, true)) return false; // 435932/435BCC固定本次end。
    if ((source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt14() & 3U) == 3U) return false;
    if (source.ReadTextCursorAfter413BE0() >= 0 || source.ReadNamedAfterRoot("replay") != 0) return false;
    return source.CaptureUnSave(ReadKind::Lookup417990)->BytesAt08()[load ? 1 : 0] != 0;
}
int SkipStatus(Source& source, bool checkHeld)
{
    return ReadToolSkipStatusNativeAt4B0760(source, checkHeld);
}
int AutoStatus(Source& source, bool checkHeld)
{
    return ReadToolAutoStatusNativeAt4B0470(source, checkHeld);
}
void Enable(Body& body, Services& services, int id, bool enabled)
{
    Selected selected(body, services);
    SetToolControlEnabledNativeAt508470(*selected.Get(), id, enabled ? 1 : 0);
    selected.Finish();
}
void CheckControl(Body& body, Services& services, int id, bool checked)
{
    Selected selected(body, services);
    SetToolControlCheckedNativeAt501FA0(*selected.Get(), id, checked ? 1 : 0);
    selected.Finish();
}
std::uint32_t EventMask(const Body& body) noexcept { return (~std::uint32_t(body.valueAt34)) >> 31U; }
}
std::int32_t ReadToolSkipStatusNativeAt4B0760(Source& source, std::uint32_t checkHeld)
{
    auto byte=[](const UnSaveDataRuntime* body,std::uint32_t offset)
    {return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);};
    // 4B07AF/4B081C只读BYTE；4B07FA读DWORD，各自重新取Env，不能共享字段快照。
    if (!byte(source.CaptureUnSave(ReadKind::Inline),0x0C)) return 0;
    if (checkHeld && Held(source)) return 2;
    const auto* current=source.CaptureUnSave(ReadKind::Lookup417990);
    if ((*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(current)+0x14u)&3u)==3u) return 2;
    return byte(source.CaptureUnSave(ReadKind::Lookup417990),0x14)&1u;
}
std::int32_t ReadToolAutoStatusNativeAt4B0470(Source& source, std::uint32_t checkHeld)
{
    auto byte=[](const UnSaveDataRuntime* body,std::uint32_t offset)
    {return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);};
    if (!(byte(source.CaptureUnSave(ReadKind::Inline),0x18)&1u)) return 0;
    if (byte(source.CaptureUnSave(ReadKind::Lookup417990),0x18)&2u) return 2;
    if (checkHeld && Held(source)) return 0;
    const auto* current=source.CaptureUnSave(ReadKind::Lookup417990);
    return (*reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(current)+0x14u)&3u)!=3u;
}
void SetToolModeNativeAt434FB0(Body& body, std::int32_t mode, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    auto& source = services.state;
    const bool checkHeld = mode != 2;
    Enable(body, services, 2, WindowGate(source, checkHeld));
    const auto skip = SkipStatus(source, checkHeld);
    if (skip == 2) { Enable(body, services, 5, true); CheckControl(body, services, 5, true); }
    else if (skip == 1)
    {
        if (mode != 1) { Enable(body, services, 5, true); CheckControl(body, services, 5, false); }
    }
    else
    {
        Enable(body, services, 5, false);
        if (mode == 2)
        {
            Selected selected(body, services);
            auto* dialog = selected.Get(); // 435364..390在再次取UnSave之前固定receiver。
            const auto checked = (source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt14() >> 1U) & 1U;
            SetToolControlCheckedNativeAt501FA0(*dialog, 5, checked);
            selected.Finish();
        }
    }
    Enable(body, services, 3, CommonGate(source, checkHeld) &&
        (source.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt1C() & 1U));
    const auto automatic = AutoStatus(source, checkHeld);
    if (automatic == 2) { Enable(body, services, 4, true); CheckControl(body, services, 4, true); }
    else if (automatic == 1)
    {
        if (mode != 1) { Enable(body, services, 4, true); CheckControl(body, services, 4, false); }
    }
    else Enable(body, services, 4, false);
    Enable(body, services, 6, VoiceGate(source, checkHeld));
    const bool save = SaveLoadGate(source, checkHeld, false);
    Enable(body, services, 0, save); Enable(body, services, 7, save);
    const bool load = SaveLoadGate(source, checkHeld, true);
    Enable(body, services, 1, load); Enable(body, services, 8, load);
    Enable(body, services, 9, WindowGate(source, checkHeld) && source.ReadMessageEnabledAfter413BE0() != 0);
    Enable(body, services, 10, AffectionGate(source, checkHeld));
    const bool remaining = CommonGate(source, checkHeld, ReadKind::Lookup417990);
    Enable(body, services, 11, remaining); Enable(body, services, 12, remaining);
    {
        Selected selected(body, services);
        ReconcileToolDialogControlsAt527720(*selected.Get());
        selected.ReleaseAt40D830();
    }
    if (body.window.layer.enabledAt0C) PropagateToolDirtyNativeAt415910(body, services);
}
void PropagateToolDirtyNativeAt415910(Body& body, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        auto* dirty = dialog->vptrAt00->dirtyAt00(dialog);
        dirty->vptrAt00->coalesceAt04(dirty); selected.Finish();
    }
    std::int32_t count;
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        auto* dirty = dialog->vptrAt00->dirtyAt00(dialog);
        count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty)); selected.Finish();
    }
    if (!count) return;
    RECT* rectangles;
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        auto* dirty = dialog->vptrAt00->dirtyAt00(dialog);
        rectangles = dirty->vptrAt00->rectsAt1C(dirty); selected.Finish(true);
    }
    for (std::int32_t i = 0; i < count; ++i, ++rectangles)
    {
        auto* parent = body.window.layer.parentAt10;
        parent->vptrAt00->addDirtyAt20(parent, rectangles);
    }
    Selected selected(body, services); auto* dialog = selected.Get();
    auto* dirty = dialog->vptrAt00->dirtyAt00(dialog);
    dirty->vptrAt00->clearAt00(dirty); selected.Finish();
}
std::uint32_t TickToolWindowNativeAt4BE200(Body& body, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    const bool enabled = body.window.layer.enabledAt0C != 0;
    body.valueAt34 = -1;
    if (!enabled) return EventMask(body);
    if (!body.valueAt38)
    {
        if ((services.state.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt14() & 3U) == 1U && Held(services.state))
        { SetToolModeNativeAt434FB0(body, 0, services); body.valueAt38 = 1; }
    }
    else if (!Held(services.state, true))
    { SetToolModeNativeAt434FB0(body, 0, services); body.valueAt38 = 0; }
    if (body.valueAt38) return EventMask(body);
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        dialog->vptrAt00->processInputAt54(dialog); selected.Finish();
    }
    PropagateToolDirtyNativeAt415910(body, services);
    std::int32_t event;
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        event = dialog->vptrAt00->resultAt50(dialog); selected.Finish(true);
    }
    constexpr std::array<int, 13> events{3, 2, 11, -1, 4, 5, 6, 1, 0, 7, 8, 9, 10};
    if (std::uint32_t(event) < events.size()) body.valueAt34 = events[std::uint32_t(event)];
    else
    {
        Selected selected(body, services);
        const auto state = ScanToolControlStateNativeAt50FD20(*selected.Get());
        selected.ReleaseAt40D830();
        if (state == 1) return 2;
        if (state == 2) return 4;
    }
    return EventMask(body);
}
void CaptureToolWindowNativeAt4C01F0(Body& body, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    if (!body.window.layer.enabledAt0C) return;
    {
        Selected selected(body, services); auto* dialog = selected.Get();
        dialog->vptrAt00->setActiveAt30(dialog, nullptr); selected.Finish();
    }
    constexpr int ids[]{2,5,3,4,6,0,7,1,8,9,10,11,12};
    for (int id : ids)
    {
        Selected selected(body, services);
        ResetToolControlNativeAt5312B0(*selected.Get(), id); selected.Finish();
    }
    {
        Selected selected(body, services);
        ReconcileToolDialogControlsAt527720(*selected.Get()); selected.Finish(true);
    }
    PropagateToolDirtyNativeAt415910(body, services);
}
void RestoreToolWindowNativeAt4BF4E0(Body& body, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    {
        Selected selected(body, services);
        InvalidateToolControlsNativeAt530DC0(*selected.Get()); selected.Finish(true);
    }
    PropagateToolDirtyNativeAt415910(body, services);
}
void SetToolCaptureModeNativeAt4C2300(Body& body, std::uint8_t value, Services& services)
{
    ToolWindowNativeScope scope(services.window);
    Selected selected(body, services); auto* dialog = selected.Get();
    dialog->vptrAt00->writeStateAt1C(dialog, value); selected.Finish();
}
void ResetToolSkipLatchNativeAt4AB910(Body& body, std::uint8_t value, Services& services)
{
    body.valueAt38 = 0;
    if (value && (services.state.CaptureUnSave(ReadKind::Lookup417990)->FlagsAt14() & 3U) != 3U && Held(services.state))
        body.valueAt38 = 1;
}
}
