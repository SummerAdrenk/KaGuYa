#include <Runtime/ReadScriptMenuInteractionRuntime.h>
#include <Runtime/ReadScriptTextServicesRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
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

bool Same(const std::string& value, const char* expected)
{
    const auto locale = _create_locale(LC_CTYPE, ".932");
    if (!locale) throw std::runtime_error("ReadScript menu CP932 locale unavailable");
    const auto result = _mbscmp_l(reinterpret_cast<const unsigned char*>(value.c_str()),
        reinterpret_cast<const unsigned char*>(expected), locale);
    _free_locale(locale);
    return result == 0;
}
}

ReadScriptMenuInteractionRuntime::ReadScriptMenuInteractionRuntime(IReadScriptMenuInteractionHost& host,
    IReadScriptToolServices& tools, IReadScriptLayerServices& layers, IReadScriptMenuServices& menu,
    SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave, IReadScriptTextServices& text) noexcept
    : host_(host), tools_(tools), layers_(layers), menu_(menu),
      fixedState_(std::in_place, save, unsave), stateSource_(*fixedState_), text_(text) {}

ReadScriptMenuInteractionRuntime::ReadScriptMenuInteractionRuntime(IReadScriptMenuInteractionHost& host,
    IReadScriptToolServices& tools, IReadScriptLayerServices& layers, IReadScriptMenuServices& menu,
    IReadScriptInteractionStateSource& stateSource, IReadScriptTextServices& text) noexcept
    : host_(host), tools_(tools), layers_(layers), menu_(menu),
      fixedState_(std::nullopt), stateSource_(stateSource), text_(text) {}

bool ReadScriptMenuInteractionRuntime::SuspendForLayers(ReadScriptSceneState& state)
{
    if (!layers_.PrepareLayersAt46A980()) return false;
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
    return true;
}

bool ReadScriptMenuInteractionRuntime::StartSaveLoad(ReadScriptSceneState& state, std::int32_t substate)
{
    host_.BackupBeforeSaveLoadAt4B01A0();
    state.substate = substate;
    return SuspendForLayers(state);
}

bool ReadScriptMenuInteractionRuntime::RunAt4B11B0(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 0:
        host_.ClearModalResultAt10();
        // 4B120B/121D/1231/1246/125A 每次独立取源；读、写不可合并为一个 receiver。
        stateSource_.CaptureUnSaveAt4180C0().BytesAt24()[5] = 1;
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(stateSource_.CaptureUnSaveAt4180C0().ModeAt04());
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        state.interactionValueAt14 = std::bit_cast<std::int32_t>(stateSource_.CaptureUnSaveAt4180C0().FlagsAt14());
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() &= 2u;
        menu_.RebuildChoicesAt4369E0();
        menu_.ClearToolByteAt38();
        tools_.SetToolModeAt434FB0(2);
        state.interactionInitializedAtB8 = menu_.ReadScriptByteAt24();
        state.substate = 1;
        [[fallthrough]];
    case 1:
    {
        // 4B12FF..1311 先固定写目标，再读 +10；不能依赖赋值式的 RHS 优先求值。
        auto& unsave = stateSource_.CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10.value());
        state.substate = 2;
        if (layers_.PrepareLayersAt46AD20(1))
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
        if (state.interactionInitializedAtB8.value() == 0 && stateSource_.CaptureSaveAt4180C0().bytesAt74[2] != 0)
            host_.AutoSaveAt46B280(1);
        state.interactionInitializedAtB8 = std::uint8_t{1};
        state.substate = 3;
        [[fallthrough]];
    case 3:
        return RunSelecting(state);
    case 4:
        if ((stateSource_.CaptureSaveAt4180C0().bytesAt34[4] & 8u) != 0)
        {
            // 4B1D8E..1DA0：取 UnSave 以后才读 +14，与 case16 的 esi 锁存不同。
            auto& unsave = stateSource_.CaptureUnSaveAt4180C0();
            unsave.FlagsAt14() = std::bit_cast<std::uint32_t>(state.interactionValueAt14.value());
        }
        else stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() = 1;
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt20() = 1;
        stateSource_.CaptureUnSaveAt4180C0().BytesAt24()[5] = 0;
        state.mainState = 0;
        return true;
    default:
        return RunCommand(state);
    }
}

bool ReadScriptMenuInteractionRuntime::RunSelecting(ReadScriptSceneState& state)
{
    host_.ProcessSoundRequestsAt4A9090();
    if (Same(host_.ReadPendingCommand(), kSave))
    {
        host_.BackupBeforeSaveLoadAt4B01A0();
        host_.WritePendingCommand({});
        state.substate = 8;
        return SuspendForLayers(state);
    }
    if (Same(host_.ReadPendingCommand(), kLoad))
    {
        host_.BackupBeforeSaveLoadAt4B01A0();
        host_.WritePendingCommand({});
        state.substate = 9;
        return SuspendForLayers(state);
    }
    if (Same(host_.ReadPendingCommand(), kQuickSave))
    {
        host_.WritePendingCommand({});
        state.substate = 11;
        return false;
    }
    if (Same(host_.ReadPendingCommand(), kQuickLoad))
    {
        host_.WritePendingCommand({});
        state.substate = 13;
        return false;
    }
    if (Same(host_.ReadPendingCommand(), kSettings))
    {
        host_.WritePendingCommand({});
        state.substate = 15;
        return SuspendForLayers(state);
    }
    const bool replay = tools_.ReadReplayValue();
    const auto input = tools_.UpdateToolAt4BE200();
    // 4B16AB..4B16B3：test esi/esi + setnz，不是正文的 input & 4。
    const auto result = menu_.TickChoicesAt4BBFD0(input != 0);
    if (result >= 0)
    {
        menu_.WriteMenuResult(result);
        layers_.SetLayerVisible(5, 0, 0);
        state.substate = 4;
        return false;
    }
    if ((LowByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt1C()) & 2u) != 0 || text_.PollKeyboardAt47F4E0(7))
    {
        state.substate = 7;
        return SuspendForLayers(state);
    }
    if ((LowByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt20()) & 2u) != 0 || tools_.PollConfiguredKeyAt4B0630(0x14) ||
        tools_.ToolEventAt34() == 11)
    {
        state.substate = 5;
        return true;
    }
    if (tools_.ToolEventAt34() == 3 || (!replay && tools_.PollConfiguredKeyAt4B0630(0x1C)) ||
        tools_.PollSaveAt47F830()) return StartSaveLoad(state, 8);
    if (tools_.ToolEventAt34() == 2 || (!replay && tools_.PollConfiguredKeyAt4B0630(0x24)) ||
        tools_.PollLoadAt47F650()) return StartSaveLoad(state, 9);
    if (tools_.ToolEventAt34() == 1 || tools_.PollQuickSaveAt47F780())
    {
        state.substate = 11;
        return false;
    }
    if (tools_.ToolEventAt34() == 0 || tools_.PollQuickLoadAt47F6D0())
    {
        state.substate = 13;
        return false;
    }
    if (tools_.ToolEventAt34() == 6)
    {
        // 4B1979/1991 分别取标志与 +3C 原始槽；19BB 的第三次取源属于 ReplayVoice，
        // 其内部先固定 root，再取 voices，最后解析固定 root，不能在这里提前代取。
        if ((LowByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 &&
            stateSource_.CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
            text_.ReplayVoiceAt42C4F0()) text_.AdjustEffectsAt4C2410();
        return false;
    }
    if (tools_.ToolEventAt34() == 4 || tools_.PollConfiguredKeyAt4B0630(0x34))
    {
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt18() ^= 2u; // 4B1CB5：DWORD xor。
        tools_.SetToolModeAt434FB0(2);
        return false;
    }
    if (tools_.ToolEventAt34() == 7)
    {
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(stateSource_.CaptureUnSaveAt4180C0().ModeAt04());
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        layers_.ResetOpacityAt47EE40();
        layers_.SetLayerVisible(6, 1, 0);
        tools_.CaptureToolAt4C01F0();
        tools_.SetToolCaptureModeAt4C2300(false);
        tools_.RestoreToolAt4BF4E0();
        state.substate = 17;
        return false;
    }
    if (tools_.ToolEventAt34() == 8 || tools_.PollLikabilityAt47F530(1))
    {
        tools_.CaptureToolAt4C01F0();
        state.substate = 21;
        return false;
    }
    if (tools_.ToolEventAt34() == 9)
    {
        state.substate = 19;
        return false;
    }
    if (tools_.ToolEventAt34() == 10) host_.ShowPublicDialogAt4C1760();
    return false;
}

void ReadScriptMenuInteractionRuntime::StopForScene()
{
    text_.StopVoiceAt4384D0();
    text_.StopEffectsAt413270();
    text_.RefreshEffectsAt433F60();
}

bool ReadScriptMenuInteractionRuntime::RunCommand(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 5:
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt20() = 2;
        ++state.substate;
        layers_.BeginHiddenWindowAt4ADF70();
        return SuspendForLayers(state);
    case 6:
        if (text_.PollKeyboardAt47F4E0(0) || text_.PollMouseAt4B0740())
        {
            stateSource_.CaptureUnSaveAt4180C0().FlagsAt20() = 1;
            state.substate = 1;
            layers_.EndHiddenWindowAt4AC970();
            state.drawEnabled = true;
            return true;
        }
        state.drawEnabled = !layers_.UpdateHiddenWindowAt4BDD00();
        text_.RefreshEffectsAt433F60();
        return false;
    case 7:
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        host_.ActivateSceneAt4A9030(kLog);
        state.substate = 1;
        return false;
    case 8:
    case 9:
    {
        const auto* command = state.substate == 8 ? kSave : kLoad;
        StopForScene();
        host_.MarkRootSaveLoadAt1AF();
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        host_.ActivateSceneAt4A9030(command);
        state.substate = 1;
        return false;
    }
    case 11:
    case 13:
    {
        const bool load = state.substate == 13;
        tools_.CaptureToolAt4C01F0();
        tools_.SetToolCaptureModeAt4C2300(false);
        host_.CaptureQuickOperationAt4C1C60();
        tools_.SetToolCaptureModeAt4C2300(true);
        if (load && host_.QuickLoadAt485CE0())
        {
            host_.ClearScenesAt4C1460();
            host_.QueueSceneAt47F920(kLoadAfter);
            stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
            return false;
        }
        if (!load) host_.QuickSaveAt485FC0(1);
        if (load) state.substate = 3;
        tools_.RestoreToolAt4BF4E0();
        if (!load) state.substate = 3;
        return false;
    }
    case 15:
        StopForScene();
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        host_.ActivateSceneAt4A9030(kSettings);
        state.substate = 16;
        return false;
    case 16:
        if ((stateSource_.CaptureSaveAt4180C0().bytesAt34[4] & 8u) != 0)
        {
            // 4B239C..23AE：esi 在再次取源之前锁存，后续回调不得改变本次恢复值。
            const auto flags = std::bit_cast<std::uint32_t>(state.interactionValueAt14.value()) & 2u;
            stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() = flags;
        }
        else stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() = 0;
        layers_.SetLayerVisible(1, 1, 0);
        tools_.SetToolModeAt434FB0(2);
        state.substate = 1;
        return true;
    case 17:
    {
        if (layers_.UpdateLayerSixAt4BE530()) return false;
        layers_.SetLayerVisible(6, 0, 0);
        auto& unsave = stateSource_.CaptureUnSaveAt4180C0(); // 4B2489..249B 同 case1。
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10.value());
        state.substate = 3;
        tools_.SetToolCaptureModeAt4C2300(true);
        tools_.RestoreToolAt4BF4E0();
        return false;
    }
    case 19:
        if (stateSource_.CaptureSaveAt4180C0().bytesAt77[6] != 0)
        {
            auto dialog = host_.CreateExitDialogAt4E3330();
            if (!dialog) throw std::logic_error("ReadScript menu exit dialog owner is null");
            tools_.CaptureToolAt4C01F0();
            tools_.SetToolCaptureModeAt4C2300(false);
            dialog->CaptureDisplayAt48CA30();
            tools_.SetToolCaptureModeAt4C2300(true);
            const bool confirmed = dialog->RunModalAt5071D0() == 6;
            tools_.RestoreToolAt4BF4E0();
            dialog.reset();
            if (!confirmed)
            {
                state.substate = state.savedSubstateAt50.value();
                return false;
            }
        }
        host_.PostExitCommand8015();
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        return false;
    case 21:
        host_.ActivateSceneAt4A9030(kLikability);
        state.substate = 1;
        return false;
    default:
        // 原 jump table 的 10/12/14/18/20 和范围外值直接返回 0。
        return false;
    }
}
}
