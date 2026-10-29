#include <Runtime/ToolWindowInteractionRuntime.h>

#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/ToolWindowLayerRuntime.h>

#include <array>
#include <bit>
#include <span>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ToolWindowInteractionRuntime::ToolWindowInteractionRuntime(ToolWindowLayerRuntime& window,
    IToolWindowInteractionServices& services) noexcept
    : window_(window), services_(services)
{
}

ToolWindowInteractionRuntime::SelectedInteraction
ToolWindowInteractionRuntime::SelectInteraction()
{
    auto owner = window_.SelectedDialogAt417B50();
    auto* interaction = dynamic_cast<IToolWindowDialogInteraction*>(owner.get());
    if (!interaction)
        throw std::logic_error("CToolWin 交互要求当前 Dialog 实现 IToolWindowDialogInteraction");
    return {std::move(owner), interaction};
}

bool ToolWindowInteractionRuntime::CommonGate(bool checkHeld)
{
    return services_.ReadUnSaveByteAt0C() != 0 &&
        !(checkHeld && services_.IsSkipKeyHeldAt4198D0()) &&
        (services_.ReadUnSaveFlagsAt14() & 3u) != 3u &&
        services_.ReadTextGlyphCursorAt50() < 0;
}

bool ToolWindowInteractionRuntime::WindowGateAt419BF0(bool checkHeld)
{
    return CommonGate(checkHeld) && (services_.ReadUnSaveFlagsAt20() & 1u) != 0;
}

bool ToolWindowInteractionRuntime::VoiceGateAt419AC0(bool checkHeld)
{
    return CommonGate(checkHeld) && (services_.ReadUnSaveFlagsAt18() & 2u) == 0 &&
        services_.HasVoiceArrayStorageAt3C();
}

bool ToolWindowInteractionRuntime::AffectionGateAt419910(bool checkHeld)
{
    return CommonGate(checkHeld) && services_.ReadUnSaveModeAt04() != 2u &&
        services_.ReadReplayValueAt4C73A0() == 0 &&
        services_.ReadAffectionDisplayFlagAt4C73A0() != 0 &&
        (services_.ReadUnSaveFlagsAt20() & 1u) != 0;
}

bool ToolWindowInteractionRuntime::SaveLoadGate(bool checkHeld, bool load)
{
    if ((checkHeld && services_.IsSkipKeyHeldAt4198D0()) ||
        (services_.ReadUnSaveFlagsAt14() & 3u) == 3u ||
        services_.ReadTextGlyphCursorAt50() >= 0 ||
        services_.ReadReplayValueAt4C73A0() != 0)
        return false;
    return (load ? services_.ReadUnSaveByteAt09() : services_.ReadUnSaveByteAt08()) != 0;
}

std::int32_t ToolWindowInteractionRuntime::SkipStatus(bool checkHeld)
{
    if (services_.ReadUnSaveByteAt0C() == 0) return 0;
    if (checkHeld && services_.IsSkipKeyHeldAt4198D0()) return 2;
    if ((services_.ReadUnSaveFlagsAt14() & 3u) == 3u) return 2;
    // 43514D..43515F 再次取 UnSave，不能复用前一次 Flags14。
    return static_cast<std::int32_t>(services_.ReadUnSaveFlagsAt14() & 1u);
}

std::int32_t ToolWindowInteractionRuntime::AutoStatus(bool checkHeld)
{
    if ((services_.ReadUnSaveFlagsAt18() & 1u) == 0) return 0;
    if ((services_.ReadUnSaveFlagsAt18() & 2u) != 0) return 2;
    if (checkHeld && services_.IsSkipKeyHeldAt4198D0()) return 0;
    return (services_.ReadUnSaveFlagsAt14() & 3u) != 3u ? 1 : 0;
}

void ToolWindowInteractionRuntime::SetControlEnabled(std::int32_t id, bool value)
{
    auto selected = SelectInteraction();
    selected.interaction->SetControlEnabledAt508470(id, value ? 1 : 0);
}

void ToolWindowInteractionRuntime::SetControlChecked(std::int32_t id, bool value)
{
    auto selected = SelectInteraction();
    selected.interaction->SetControlCheckedAt501FA0(id, value ? 1 : 0);
}

void ToolWindowInteractionRuntime::SetModeAt434FB0(std::int32_t mode)
{
    const bool checkHeld = mode != 2;
    SetControlEnabled(2, WindowGateAt419BF0(checkHeld));

    const auto skip = SkipStatus(checkHeld);
    if (skip == 2)
    {
        SetControlEnabled(5, true);
        SetControlChecked(5, true);
    }
    else if (skip == 1)
    {
        if (mode != 1)
        {
            SetControlEnabled(5, true);
            SetControlChecked(5, false);
        }
    }
    else
    {
        SetControlEnabled(5, false);
        if (mode == 2)
        {
            // 435364 先选 child，435390 之后才重新读取 Flags14。
            auto selected = SelectInteraction();
            const auto checked = (services_.ReadUnSaveFlagsAt14() >> 1u) & 1u;
            selected.interaction->SetControlCheckedAt501FA0(5, static_cast<std::int32_t>(checked));
        }
    }

    SetControlEnabled(3, CommonGate(checkHeld) && (services_.ReadUnSaveFlagsAt1C() & 1u) != 0);

    const auto automatic = AutoStatus(checkHeld);
    if (automatic == 2)
    {
        SetControlEnabled(4, true);
        SetControlChecked(4, true);
    }
    else if (automatic == 1)
    {
        if (mode != 1)
        {
            SetControlEnabled(4, true);
            SetControlChecked(4, false);
        }
    }
    else
        SetControlEnabled(4, false);

    SetControlEnabled(6, VoiceGateAt419AC0(checkHeld));

    const bool save = SaveLoadGate(checkHeld, false);
    SetControlEnabled(0, save);
    SetControlEnabled(7, save);
    const bool load = SaveLoadGate(checkHeld, true);
    SetControlEnabled(1, load);
    SetControlEnabled(8, load);

    SetControlEnabled(9, WindowGateAt419BF0(checkHeld) &&
        services_.ReadMessageWindowEnabledAt0C() != 0);
    SetControlEnabled(10, AffectionGateAt419910(checkHeld));
    const bool remaining = CommonGate(checkHeld);
    SetControlEnabled(11, remaining);
    SetControlEnabled(12, remaining);
    {
        auto selected = SelectInteraction();
        selected.interaction->ReconcileControlsAt527720();
    }
    if (window_.EnabledByteAt0C() != 0) PropagateDirtyRectsAt415910();
}

std::uint32_t ToolWindowInteractionRuntime::CurrentEventMask() const
{
    if (!window_.State().valueAt34)
        throw std::logic_error("CToolWin 4BE200 的事件字段 +34 尚未赋值");
    return (~static_cast<std::uint32_t>(*window_.State().valueAt34)) >> 31u;
}

std::uint32_t ToolWindowInteractionRuntime::TickAt4BE200()
{
    const bool enabled = window_.EnabledByteAt0C() != 0;
    window_.State().valueAt34 = -1;
    if (!enabled) return CurrentEventMask();
    if (!window_.State().valueAt38)
        throw std::logic_error("CToolWin 4BE200 的按住锁存 +38 尚未由生产调用方赋值");

    if (*window_.State().valueAt38 == 0)
    {
        if ((services_.ReadUnSaveFlagsAt14() & 3u) == 1u &&
            services_.IsSkipKeyHeldAt4198D0())
        {
            SetModeAt434FB0(0);
            window_.State().valueAt38 = std::uint8_t{1};
        }
    }
    else if (!services_.IsSkipKeyHeldAt4198D0())
    {
        SetModeAt434FB0(0);
        window_.State().valueAt38 = std::uint8_t{0};
    }

    if (!window_.State().valueAt38)
        throw std::logic_error("CToolWin 4BE200 回调后缺失按住锁存 +38");
    if (*window_.State().valueAt38 != 0) return CurrentEventMask();
    {
        auto selected = SelectInteraction();
        selected.interaction->UpdateAtVslot54();
    }
    PropagateDirtyRectsAt415910();
    std::int32_t result;
    {
        auto selected = SelectInteraction();
        result = selected.interaction->ResultAtVslot50();
    }

    constexpr std::array<std::int32_t, 13> events{3, 2, 11, -1, 4, 5, 6, 1, 0, 7, 8, 9, 10};
    if (static_cast<std::uint32_t>(result) < events.size())
        window_.State().valueAt34 = events[static_cast<std::uint32_t>(result)];
    else
    {
        std::int32_t state;
        {
            auto selected = SelectInteraction();
            state = selected.interaction->ScanControlStateAt50FD20();
        }
        if (state == 1) return 2;
        if (state == 2) return 4;
    }
    return CurrentEventMask();
}

void ToolWindowInteractionRuntime::CaptureAt4C01F0()
{
    if (window_.EnabledByteAt0C() == 0) return;
    {
        auto selected = SelectInteraction();
        selected.interaction->ClearActiveControlAtVslot30();
    }
    constexpr std::array<std::int32_t, 13> ids{2, 5, 3, 4, 6, 0, 7, 1, 8, 9, 10, 11, 12};
    for (const auto id : ids)
    {
        auto selected = SelectInteraction();
        selected.interaction->ResetControlAt5312B0(id);
    }
    {
        auto selected = SelectInteraction();
        selected.interaction->ReconcileControlsAt527720();
    }
    PropagateDirtyRectsAt415910();
}

void ToolWindowInteractionRuntime::RestoreAt4BF4E0()
{
    {
        auto selected = SelectInteraction();
        selected.interaction->InvalidateAllControlsAt530DC0();
    }
    PropagateDirtyRectsAt415910();
}

void ToolWindowInteractionRuntime::SetCaptureModeAt4C2300(std::uint8_t value)
{
    auto selected = SelectInteraction();
    selected.interaction->SetInputByteAtVslot1C(value);
}

void ToolWindowInteractionRuntime::PropagateDirtyRectsAt415910()
{
    {
        auto selected = SelectInteraction();
        selected.interaction->OwnDirtyRectsAtVslot00().PrepareAtVslot04();
    }
    std::size_t rawCount;
    {
        auto selected = SelectInteraction();
        rawCount = selected.interaction->OwnDirtyRectsAtVslot00().CountAtVslot18();
    }
    if (!std::in_range<std::uint32_t>(rawCount))
        throw std::logic_error("CToolWin 415910 的现代 dirty count 超出原 DWORD 范围");
    const auto count = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(rawCount));
    if (count == 0) return;

    std::span<const RECT> rects;
    {
        auto selected = SelectInteraction();
        rects = selected.interaction->OwnDirtyRectsAtVslot00().RectsAtVslot1C();
    }
    if (count > 0 && static_cast<std::size_t>(count) > rects.size())
        throw std::logic_error("CToolWin 415910 跨次选择后的 dirty count/data 不一致");
    // 415A1A 只以零跳过 data/clear；415AA4 再以有符号比较决定是否遍历。
    for (std::int32_t index = 0; index < count; ++index)
    {
        auto* parent = window_.RegisteredLayerList();
        if (!parent) throw std::logic_error("CToolWin 415910 缺少真实父 dirty 接收者");
        parent->DirtyRects().Add(rects[static_cast<std::size_t>(index)]);
    }
    {
        auto selected = SelectInteraction();
        selected.interaction->OwnDirtyRectsAtVslot00().ClearAtVslot00();
    }
}

void ToolWindowInteractionRuntime::ResetSkipLatchAt4AB910(std::uint8_t value)
{
    window_.State().valueAt38 = std::uint8_t{0};
    if (value != 0 && (services_.ReadUnSaveFlagsAt14() & 3u) != 3u &&
        services_.IsSkipKeyHeldAt4198D0())
        window_.State().valueAt38 = std::uint8_t{1};
}
}
