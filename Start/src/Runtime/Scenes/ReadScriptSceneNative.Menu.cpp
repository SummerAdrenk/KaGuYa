#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
{
namespace
{
constexpr char kSave[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA"; // 5AE218
constexpr char kLoad[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"; // 5AE224
constexpr char kQuickSave[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"; // 5AE8AC
constexpr char kQuickLoad[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"; // 5AE8BC
constexpr char kSettings[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA"; // 5B4E68
constexpr char kLog[] = "\x83\x8D\x83\x4F\x89\xE6\x96\xCA"; // 5B56E0
constexpr char kLikability[] = "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA"; // 5B4E80
constexpr char kLoadAfter[] = "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D"; // 5B4DB0

std::uint8_t LowByte(const std::uint32_t& value) noexcept
{
    // 4B1762/4B17A1/4B1985 是 BYTE test，不能先读取整个 DWORD 再截断。
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

using Release=ReadScriptCommandRelease;
}


IReadScriptMenuServices& MenuFlow::Menu()
{ return command_ ? source_.CommandMenu() : source_.Menu(); }

bool MenuFlow::SuspendForLayers(OwnedStateView& state)
{
    if (!layers_.PrepareLayersAt46A980()) return false;
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
    return true;
}

bool MenuFlow::StartSaveLoad(OwnedStateView& state, std::int32_t substate)
{
    source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
    state.substate = substate;
    return SuspendForLayers(state);
}

bool MenuFlow::RunAt4B11B0(OwnedStateView& state)
{
    switch (state.substate)
    {
    case 0:
        source_.InteractionHost().ClearModalResultAt10();
        // 4B120B/121D/1231/1246/125A 每次独立取源；读、写不可合并为一个 receiver。
        source_.InteractionState().CaptureUnSaveAt4180C0().BytesAt24()[5] = 1;
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04());
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        state.interactionValueAt14 = std::bit_cast<std::int32_t>(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14());
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() &= 2u;
        Menu().RebuildChoicesAt4369E0();
        Menu().ClearToolByteAt38();
        source_.Tools().SetToolModeAt434FB0(2);
        if (!command_) state.interactionInitializedAtB8 = Menu().ReadScriptByteAt24();
        state.substate = 1;
        [[fallthrough]];
    case 1:
    {
        // 4B12FF..1311 先固定写目标，再读 +10；不能依赖赋值式的 RHS 优先求值。
        auto& unsave = source_.InteractionState().CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10);
        state.substate = command_ ? 3 : 2;
        if (layers_.PrepareLayersAt46AD20(command_ ? 2 : 1))
        {
            state.savedMainStateAt4C = state.mainState;
            state.savedSubstateAt50 = state.substate;
            state.mainState = 2;
            state.substate = 0;
            return true;
        }
        [[fallthrough]];
    }
    case 2:
        // Hakoniwa 494E20由1直接到3；没有普通文字选项的自动保存阶段。
        if (!command_)
        {
            if (state.interactionInitializedAtB8 == 0 && source_.InteractionState().CaptureSaveAt4180C0().bytesAt74[2] != 0)
                source_.InteractionHost().AutoSaveAt46B280(1);
            state.interactionInitializedAtB8 = std::uint8_t{1};
        }
        state.substate = 3;
        [[fallthrough]];
    case 3:
        return RunSelecting(state);
    case 4:
        if ((source_.InteractionState().CaptureSaveAt4180C0().bytesAt34[4] & 8u) != 0)
        {
            // 4B1D8E..1DA0：取 UnSave 以后才读 +14，与 case16 的 esi 锁存不同。
            auto& unsave = source_.InteractionState().CaptureUnSaveAt4180C0();
            unsave.FlagsAt14() = std::bit_cast<std::uint32_t>(state.interactionValueAt14);
        }
        else source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() = 1;
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt20() = 1;
        source_.InteractionState().CaptureUnSaveAt4180C0().BytesAt24()[5] = 0;
        state.mainState = 0;
        return true;
    default:
        return RunCommand(state);
    }
}

bool MenuFlow::RunSelecting(OwnedStateView& state)
{
    source_.InteractionHost().ProcessSoundRequestsAt4A9090();
    auto* root=CaptureReadScriptCommandRoot(source_,0x4B13BF,0x4B13C4);
    if (CompareReadScriptCommandNative(source_,root,0x4B13CA,kSave,Release::CachedHeader))
    {
        source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
        root=CaptureReadScriptCommandRoot(source_,0x4B1419,0x4B141E);
        SetReadScriptCommandNative(source_,root,0x4B1424,"",0);
        state.substate = 8;
        return SuspendForLayers(state);
    }
    root=CaptureReadScriptCommandRoot(source_,0x4B145D,0x4B1462);
    if (CompareReadScriptCommandNative(source_,root,0x4B1468,kLoad,Release::ReturnedCString))
    {
        source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
        root=CaptureReadScriptCommandRoot(source_,0x4B14A7,0x4B14AC);
        SetReadScriptCommandNative(source_,root,0x4B14B2,"",0);
        state.substate = 9;
        return SuspendForLayers(state);
    }
    root=CaptureReadScriptCommandRoot(source_,0x4B14EA,0x4B14EF);
    auto matched=CompareReadScriptCommandNative(source_,root,0x4B14F9,kQuickSave,Release::ReturnedCString);
    root=CaptureReadScriptCommandRoot(source_,0x4B1523,0x4B1528);
    if (matched)
    {
        SetReadScriptCommandNative(source_,root,0x4B1532,"",0);
        state.substate = 11;
        return false;
    }
    matched=CompareReadScriptCommandNative(source_,root,0x4B1566,kQuickLoad,Release::ReturnedCString);
    root=CaptureReadScriptCommandRoot(source_,0x4B1590,0x4B1595);
    if (matched)
    {
        SetReadScriptCommandNative(source_,root,0x4B159F,"",0);
        state.substate = 13;
        return false;
    }
    matched=CompareReadScriptCommandNative(source_,root,0x4B15D3,kSettings,Release::ReturnedCString);
    source_.EnsureCommandRootAt413BE0(0x4B15FD);
    if (matched)
    {
        root=source_.CommandRootAt6198C4(0x4B1606);
        SetReadScriptCommandNative(source_,root,0x4B160C,"",0);
        state.substate = 15;
        return SuspendForLayers(state);
    }
    root=source_.CommandRootAt6198C4(0x4B163F);
    const bool replay = source_.ReadCommandReplayAt4C73A0(root);
    const auto input = source_.Tools().UpdateToolAt4BE200();
    // 4B16AB..4B16B3：test esi/esi + setnz，不是正文的 input & 4。
    const auto result = Menu().TickChoicesAt4BBFD0(input != 0);
    if (result >= 0)
    {
        Menu().WriteMenuResult(result);
        if (command_)
            (void)source_.DrawLayers().CaptureDrawLayersAt413BE0()
                .RequestOwnedLayerAtVslot10(StartupCommandSelectionLayerIndex(), 0, 0);
        else layers_.SetLayerVisible(5, 0, 0);
        state.substate = 4;
        return false;
    }
    bool showLog=(LowByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt1C()) & 2u) != 0;
    if (!showLog)
    {
        source_.EnsureCommandRootAt413BE0(0x4B1771);
        showLog=source_.Text().PollKeyboardAt47F4E0(7);
    }
    if (showLog)
    {
        state.substate = 7;
        return SuspendForLayers(state);
    }
    if ((LowByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt20()) & 2u) != 0 || source_.Tools().PollConfiguredKeyAt4B0630(0x14) ||
        source_.Tools().ToolEventAt34() == 11)
    {
        state.substate = 5;
        return true;
    }
    if (source_.Tools().ToolEventAt34() == 3 || (!replay && source_.Tools().PollConfiguredKeyAt4B0630(0x1C)) ||
        source_.Tools().PollSaveAt47F830()) return StartSaveLoad(state, 8);
    if (source_.Tools().ToolEventAt34() == 2 || (!replay && source_.Tools().PollConfiguredKeyAt4B0630(0x24)) ||
        source_.Tools().PollLoadAt47F650()) return StartSaveLoad(state, 9);
    if (source_.Tools().ToolEventAt34() == 1 || source_.Tools().PollQuickSaveAt47F780())
    {
        state.substate = 11;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 0 || source_.Tools().PollQuickLoadAt47F6D0())
    {
        state.substate = 13;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 6)
    {
        // 4B1979/1991 分别取标志与 +3C 原始槽；19BB 的第三次取源属于 ReplayVoice，
        // 其内部先固定 root，再取 voices，最后解析固定 root，不能在这里提前代取。
        if ((LowByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 &&
            source_.InteractionState().CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
            source_.Text().ReplayVoiceAt42C4F0()) source_.Text().AdjustEffectsAt4C2410();
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 4 || source_.Tools().PollConfiguredKeyAt4B0630(0x34))
    {
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt18() ^= 2u; // 4B1CB5：DWORD xor。
        source_.Tools().SetToolModeAt434FB0(2);
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 7)
    {
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04());
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        layers_.ResetOpacityAt47EE40();
        layers_.SetLayerVisible(6, 1, 0);
        source_.InteractionHost().CaptureToolAt4C01F0();
        source_.InteractionHost().SetToolCaptureModeAt4C2300(false);
        source_.InteractionHost().RestoreToolAt4BF4E0();
        state.substate = 17;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 8 || source_.Tools().PollLikabilityAt47F530(1))
    {
        source_.InteractionHost().CaptureToolAt4C01F0();
        state.substate = 21;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 9)
    {
        state.substate = 19;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 10) source_.InteractionHost().ShowPublicDialogAt4C1760();
    return false;
}

void MenuFlow::StopForScene()
{
    source_.Text().StopVoiceAt4384D0();
    source_.Text().StopEffectsAt413270();
    source_.Text().RefreshEffectsAt433F60();
}

bool MenuFlow::RunCommand(OwnedStateView& state)
{
    switch (state.substate)
    {
    case 5:
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt20() = 2;
        ++state.substate;
        layers_.BeginHiddenWindowAt4ADF70();
        return SuspendForLayers(state);
    case 6:
        if (source_.Text().PollKeyboardAt47F4E0(0) || source_.Text().PollMouseAt4B0740())
        {
            source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt20() = 1;
            state.substate = 1;
            layers_.EndHiddenWindowAt4AC970();
            state.drawEnabled = true;
            return true;
        }
        state.drawEnabled = !layers_.UpdateHiddenWindowAt4BDD00();
        source_.Text().RefreshEffectsAt433F60();
        return false;
    case 7:
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        source_.InteractionHost().ActivateSceneAt4A9030(kLog);
        state.substate = 1;
        return false;
    case 8:
    case 9:
    {
        const auto* command = state.substate == 8 ? kSave : kLoad;
        StopForScene();
        source_.InteractionHost().MarkRootSaveLoadAt1AF();
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        source_.InteractionHost().ActivateSceneAt4A9030(command);
        state.substate = 1;
        return false;
    }
    case 11:
    case 13:
    {
        const bool load = state.substate == 13;
        source_.InteractionHost().CaptureToolAt4C01F0();
        source_.InteractionHost().SetToolCaptureModeAt4C2300(false);
        source_.InteractionHost().CaptureQuickOperationAt4C1C60();
        source_.InteractionHost().SetToolCaptureModeAt4C2300(true);
        if (load && source_.InteractionHost().QuickLoadAt485CE0())
        {
            source_.InteractionHost().ClearScenesAt4C1460();
            source_.InteractionHost().QueueSceneAt47F920(kLoadAfter);
            source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
            return false;
        }
        if (!load) source_.InteractionHost().QuickSaveAt485FC0(1);
        if (load) state.substate = 3;
        source_.InteractionHost().RestoreToolAt4BF4E0();
        if (!load) state.substate = 3;
        return false;
    }
    case 15:
        StopForScene();
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        source_.InteractionHost().ActivateSceneAt4A9030(kSettings);
        state.substate = 16;
        return false;
    case 16:
        if ((source_.InteractionState().CaptureSaveAt4180C0().bytesAt34[4] & 8u) != 0)
        {
            // 4B239C..23AE：esi 在再次取源之前锁存，后续回调不得改变本次恢复值。
            const auto flags = std::bit_cast<std::uint32_t>(state.interactionValueAt14) & 2u;
            source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() = flags;
        }
        else source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() = 0;
        layers_.SetLayerVisible(1, 1, 0);
        source_.Tools().SetToolModeAt434FB0(2);
        state.substate = 1;
        return true;
    case 17:
    {
        if (layers_.UpdateLayerSixAt4BE530()) return false;
        layers_.SetLayerVisible(6, 0, 0);
        auto& unsave = source_.InteractionState().CaptureUnSaveAt4180C0(); // 4B2489..249B 同 case1。
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10);
        state.substate = 3;
        source_.InteractionHost().SetToolCaptureModeAt4C2300(true);
        source_.InteractionHost().RestoreToolAt4BF4E0();
        return false;
    }
    case 19:
        return RunReadScriptReturnTitleNative(state.body,source_.ReturnTitleNativeServices(),ReadScriptReturnTitleCaller::Menu);
    case 21:
        source_.InteractionHost().ActivateSceneAt4A9030(kLikability);
        state.substate = 1;
        return false;
    default:
        // 原 jump table 的 10/12/14/18/20 和范围外值直接返回 0。
        return false;
    }
}

bool RunMenu(OwnedStateView& state, Body& body, IReadScriptSceneNativeSource& source)
{
    InteractionLayers layers(body, source);
    MenuFlow flow(source, layers);
    return flow.RunAt4B11B0(state);
}
bool RunCommandMenu(OwnedStateView& state, Body& body, IReadScriptSceneNativeSource& source)
{
    InteractionLayers layers(body, source);
    MenuFlow flow(source, layers, true);
    return flow.RunAt4B11B0(state);
}
}
