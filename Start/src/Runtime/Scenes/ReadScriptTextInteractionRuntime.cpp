#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/ReadScriptTextServicesRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <atlbase.h>
#include <cstdio>
#include <cstring>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 18号 .rdata 原 CP932 常量；长度由原437680的参数或结尾NUL确定。
constexpr char kSave[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA"; // 5AE218
constexpr char kLoad[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"; // 5AE224
constexpr char kQuickSave[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"; // 5AE8AC
constexpr char kQuickLoad[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"; // 5AE8BC
constexpr char kHide[] = "\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x8F\xC1\x8B\x8E"; // 5AE964
constexpr char kSettings[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA"; // 5B4E68
constexpr char kLog[] = "\x83\x8D\x83\x4F\x89\xE6\x96\xCA"; // 5B56E0
constexpr char kLikability[] = "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA"; // 5B4E80
constexpr char kLoadAfter[] = "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D"; // 5B4DB0
constexpr char kTextFailure[] = "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 : %s"; // 5B56BC

void FormatTextFailureAt417130(std::string& destination, const char* name)
{
    // PE 564BA3/564AA7两遍均以5C78D0的"(null)"替代null，非null只读至首NUL。
    // 不依赖C CRT对null %s的非标准行为，也不提前复制借用名称。
    const int length = _scprintf(kTextFailure, name ? name : "(null)");
    if (length < 0) throw ATL::CAtlException(E_FAIL); // 417190..417192
    const auto size = static_cast<std::size_t>(length);
    // 417194..4171AA在测长成功后才准备目标容量，失败不能先提交错误前缀。
    // 现代string无CString共享分离，分配失败抛bad_alloc；为合法写入须先resize，
    // 不冒充原4171D3在第二遍后才写长度，也不模拟CString的分配大小/失败ABI。
    destination.reserve(size);
    destination.resize(size);
    // 4171C4重读同一raw；原不检查第二遍返回值，保留CRT自身的错误处理。
    static_cast<void>(sprintf_s(destination.data(), size + 1u, kTextFailure,
        name ? name : "(null)"));
    destination.data()[size] = '\0'; // 4171D9，长度仍为首遍测得值。
}

bool Same(const std::string& value, const char* expected)
{
    const auto locale = _create_locale(LC_CTYPE, ".932");
    if (!locale) throw std::runtime_error("ReadScript interaction CP932 locale unavailable");
    const auto result = _mbscmp_l(reinterpret_cast<const unsigned char*>(value.c_str()),
        reinterpret_cast<const unsigned char*>(expected), locale);
    _free_locale(locale);
    return result == 0;
}

std::uint32_t SaveDword(const SaveDataEnvironmentArchive& save, std::size_t index) noexcept
{
    std::uint32_t value;
    std::memcpy(&value, save.bytesAt34.data() + index, sizeof(value));
    return value;
}

std::uint8_t FlagByte(const std::uint32_t& flags) noexcept
{
    // 原test byte ptr只消费低BYTE，不把+18/+1C/+20扩大成DWORD读取。
    return *reinterpret_cast<const unsigned char*>(&flags);
}
}

ReadScriptTextInteractionRuntime::ReadScriptTextInteractionRuntime(
    IReadScriptTextInteractionHost& host, IReadScriptToolServices& tools, IReadScriptLayerServices& layers,
    IReadScriptInteractionStateSource& stateSource, IReadScriptTextServices& text) noexcept
    : host_(host), tools_(tools), layers_(layers), stateSource_(stateSource), text_(text) {}

ReadScriptTextInteractionRuntime::ReadScriptTextInteractionRuntime(
    IReadScriptTextInteractionHost& host, IReadScriptToolServices& tools, IReadScriptLayerServices& layers,
    const ScrVmValueTable& params,
    SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave, IReadScriptTextServices& text) noexcept
    : host_(host), tools_(tools), layers_(layers), fixedStateSource_(std::in_place, save, unsave, &params),
      stateSource_(*fixedStateSource_), text_(text) {}

bool ReadScriptTextInteractionRuntime::SuspendForLayers(ReadScriptSceneState& state)
{
    // 4B0130：先更新全部图层；只有原AL非零才保存返回状态并进入mainState2。
    if (!layers_.PrepareLayersAt46A980()) return false;
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
    return true;
}

void ReadScriptTextInteractionRuntime::ToggleAuto()
{
    stateSource_.CaptureUnSaveAt4180C0().FlagsAt18() ^= 2u;
    tools_.SetToolModeAt434FB0(false);
}

void ReadScriptTextInteractionRuntime::EnableSkip()
{
    // 4B2E3B/4B2E4D、4B3904/4B3916：先提交UnSave DWORD，再独立捕获Save BYTE。
    stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
    if ((stateSource_.CaptureSaveAt4180C0().bytesAt34[4] & 4u) != 0) text_.StopVoiceAt4384D0();
}

void ReadScriptTextInteractionRuntime::EnterWait(ReadScriptSceneState& state)
{
    tools_.SetToolModeAt434FB0(true);
    layers_.SetLayerVisible(4, 1, 0);
    state.substate = 3;
}

bool ReadScriptTextInteractionRuntime::StartSaveLoad(ReadScriptSceneState& state, std::int32_t substate)
{
    host_.BackupBeforeSaveLoadAt4B01A0();
    state.substate = substate;
    return SuspendForLayers(state);
}

[[noreturn]] void ReadScriptTextInteractionRuntime::ThrowTextFailure(ReadScriptSceneState& state)
{
    // 4B439E 先查询文件名，4B43BF 才修改 owner+B0；名称查询失败不能覆盖旧错误。
    const auto scriptName = text_.ReadCurrentScriptNameForError();
    FormatTextFailureAt417130(state.errorMessageAtB0, scriptName);
    throw state.errorMessageAtB0.data();
}

bool ReadScriptTextInteractionRuntime::RunAt4B2730(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 0:
        // 4B27AB忽略SetFormat的返回AL；原异常仍向外层传播。
        text_.SelectFormatAt4C2740(std::bit_cast<std::int32_t>(stateSource_.ReadMessageFormatAt47BDB0()), true);
        tools_.SetToolModeAt434FB0(false);
        tools_.InitializeToolAt4AB910(true);
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(stateSource_.CaptureUnSaveAt4180C0().ModeAt04());
        state.substate = 1;
        [[fallthrough]];
    case 1:
    {
        // 4B281B..4B282D先固定目的地址，之后才读owner+10；不能用赋值的RHS先求值。
        auto& unsave = stateSource_.CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10.value());
        state.substate = 2;
        if (layers_.PrepareLayersAt46AD20())
        {
            state.savedMainStateAt4C = state.mainState;
            state.savedSubstateAt50 = state.substate;
            state.mainState = 2;
            state.substate = 0;
            return true;
        }
    }
        [[fallthrough]];
    case 2:
        return RunRevealing(state);
    case 3:
    case 22:
        return RunWaiting(state);
    case 4:
        state.mainState = 0;
        return true;
    default:
        return RunCommand(state);
    }
}

bool ReadScriptTextInteractionRuntime::RunRevealing(ReadScriptSceneState& state)
{
    host_.ClearModalResultAt10();
    host_.ProcessSoundRequestsAt4A9090();
    if (Same(host_.ReadPendingCommand(), kSettings))
    {
        host_.WritePendingCommand({});
        state.substate = 15;
        return SuspendForLayers(state);
    }
    bool autoKeyAvailable = true;
    if (tools_.PollConfiguredKeyAt4B0630(0x34))
    {
        ToggleAuto();
        autoKeyAvailable = false;
    }
    if (tools_.PollConfiguredKeyAt4B0630(0x2C))
    {
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
        tools_.SetToolModeAt434FB0(false);
    }
    if (SaveDword(stateSource_.CaptureSaveAt4180C0(), 0) == 0)
    {
        const auto replay = tools_.ReadReplayValue();
        if (!replay && tools_.PollConfiguredKeyAt4B0630(0x24)) host_.WritePendingCommand(kLoad);
        else if (!replay && tools_.PollConfiguredKeyAt4B0630(0x1C)) host_.WritePendingCommand(kSave);
        else if (tools_.PollConfiguredKeyAt4B0630(0x14)) host_.WritePendingCommand(kHide);
    }
    const auto input = tools_.UpdateToolAt4BE200();
    const auto text = text_.TickTextAt4BB530((input & 4u) != 0);
    if (text < 0) ThrowTextFailure(state);
    if (text == 0)
    {
        // 4B2B6A实际调用418210：读取Keyboard action6的当前byte，不是Script或WaitMark字段。
        // 4B2BB4/2BD0/2BE8/2BFC各自取源，宽度依次为DWORD/BYTE/DWORD/指针DWORD。
        // 2C16..2C44的SoundParams固定receiver及第二次voices捕获由文字服务保留，
        // 此处的非空检查不能把先前捕获的数组传入或替代该独立取源。
        if (host_.ScriptHasWaitMarkerAt5C() || (stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() & 3u) == 3u ||
            (FlagByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 ||
            SaveDword(stateSource_.CaptureSaveAt4180C0(), 8) != 0 ||
            (stateSource_.CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
                text_.VoiceArrayAllowedAtSoundParams60() && text_.AnyVoicePlayingAt4196C0()))
        {
            EnterWait(state);
        }
        else state.substate = 22;
        return false;
    }
    if (tools_.ToolEventAt34() == 5 ||
        (tools_.ToolSkipPredicateAt4B0760(true) != 1 && tools_.PollConfiguredKeyAt4B0630(0x2C)))
    {
        EnableSkip();
        return false;
    }
    if (tools_.ToolEventAt34() == 4 ||
        (autoKeyAvailable && tools_.ToolAutoPredicateAt4B0470(true) != 1 &&
            tools_.PollConfiguredKeyAt4B0630(0x34))) ToggleAuto();
    return false;
}

bool ReadScriptTextInteractionRuntime::RunWaiting(ReadScriptSceneState& state)
{
    host_.ProcessSoundRequestsAt4A9090();
    // 原每次比较前重新查询pending；不缓存成一次snapshot掩盖调用副作用。
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
        text_.WriteWaitMarkEnabledAt0C(0);
        return false;
    }
    if (Same(host_.ReadPendingCommand(), kHide))
    {
        host_.WritePendingCommand({});
        state.substate = 5;
        return true;
    }
    if (Same(host_.ReadPendingCommand(), kSettings))
    {
        host_.WritePendingCommand({});
        state.substate = 15;
        return SuspendForLayers(state);
    }
    const bool immediateResume = state.substate == 22;
    if (immediateResume) EnterWait(state);
    bool autoKeyAvailable = true;
    if (tools_.PollConfiguredKeyAt4B0630(0x34))
    {
        ToggleAuto();
        autoKeyAvailable = false;
    }
    if (tools_.PollConfiguredKeyAt4B0630(0x2C))
    {
        stateSource_.CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
        tools_.SetToolModeAt434FB0(false);
    }
    const auto replay = tools_.ReadReplayValue();
    const auto input = tools_.UpdateToolAt4BE200();
    if (text_.AdvanceTextAt4BC260(static_cast<std::uint8_t>(input)))
    {
        state.substate = 4;
        if ((stateSource_.CaptureSaveAt4180C0().bytesAt34[4] & 4u) != 0) text_.StopVoiceAt4384D0();
        if (immediateResume)
        {
            state.mainState = 0;
            return true;
        }
        return false;
    }
    if ((FlagByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt1C()) & 2u) != 0 || text_.PollKeyboardAt47F4E0(7))
    {
        state.substate = 7;
        return SuspendForLayers(state);
    }
    if ((FlagByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt20()) & 2u) != 0 || tools_.PollConfiguredKeyAt4B0630(0x14) ||
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
        return true;
    }
    if (tools_.ToolEventAt34() == 0 || tools_.PollQuickLoadAt47F6D0())
    {
        state.substate = 13;
        return true;
    }
    if (tools_.ToolEventAt34() == 6)
    {
        // 4B358B/35A3独立检查；35C0固定root后35CD重捕获voices仍在ReplayVoice内。
        if ((FlagByte(stateSource_.CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 &&
            stateSource_.CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
            text_.ReplayVoiceAt42C4F0()) text_.AdjustEffectsAt4C2410();
        return false;
    }
    if (tools_.ToolEventAt34() == 5 || tools_.PollConfiguredKeyAt4B0630(0x2C))
    {
        EnableSkip();
        state.substate = 4;
        return false;
    }
    if (tools_.ToolEventAt34() == 4 || (autoKeyAvailable && tools_.PollConfiguredKeyAt4B0630(0x34)))
    {
        ToggleAuto();
        return false;
    }
    if (tools_.ToolEventAt34() == 7)
    {
        // 4B36E3/36F7不是同一receiver；第二次捕获失败须保留已保存的模式。
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
        return true;
    }
    if (tools_.ToolEventAt34() == 10) host_.ShowPublicDialogAt4C1760();
    return false;
}

void ReadScriptTextInteractionRuntime::StopForScene()
{
    text_.StopVoiceAt4384D0();
    text_.StopEffectsAt413270();
    text_.RefreshEffectsAt433F60();
}

bool ReadScriptTextInteractionRuntime::RunCommand(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 5:
        // 4B3A05/3A1A必须分两次取源，前一写入不因后一捕获异常而回滚。
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
        const auto* const command = state.substate == 8 ? kSave : kLoad;
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
        const bool quickLoad = state.substate == 13;
        const auto oldWait = text_.ReadWaitMarkEnabledAt0C();
        text_.WriteWaitMarkEnabledAt0C(0);
        tools_.CaptureToolAt4C01F0();
        tools_.SetToolCaptureModeAt4C2300(false);
        host_.CaptureQuickOperationAt4C1C60();
        tools_.SetToolCaptureModeAt4C2300(true);
        if (quickLoad && host_.QuickLoadAt485CE0())
        {
            host_.ClearScenesAt4C1460();
            host_.QueueSceneAt47F920(kLoadAfter);
            stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
            return false;
        }
        if (!quickLoad) host_.QuickSaveAt485FC0(1);
        // 4B3F53的失败quick-load先写substate；quick-save则在RestoreTool后写。
        if (quickLoad) state.substate = 3;
        text_.WriteWaitMarkEnabledAt0C(oldWait);
        tools_.RestoreToolAt4BF4E0();
        if (!quickLoad) state.substate = 3;
        return false;
    }
    case 15:
        StopForScene();
        stateSource_.CaptureUnSaveAt4180C0().ModeAt04() = 0;
        host_.ActivateSceneAt4A9030(kSettings);
        state.substate = 16;
        return false;
    case 16:
        layers_.SetLayerVisible(1, 1, 0);
        tools_.SetToolModeAt434FB0(false);
        state.substate = 1;
        return false;
    case 17:
    {
        if (layers_.UpdateLayerSixAt4BE530()) return false;
        layers_.SetLayerVisible(6, 0, 0);
        // 4B4105..4117与sub1相同：取目的receiver在先，读恢复值在后。
        auto& unsave = stateSource_.CaptureUnSaveAt4180C0();
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
            if (!dialog) throw std::logic_error("ReadScript exit dialog owner is null");
            const auto oldWait = text_.ReadWaitMarkEnabledAt0C();
            text_.WriteWaitMarkEnabledAt0C(0);
            tools_.CaptureToolAt4C01F0();
            tools_.SetToolCaptureModeAt4C2300(false);
            dialog->CaptureDisplayAt48CA30();
            tools_.SetToolCaptureModeAt4C2300(true);
            const bool confirmed = dialog->RunModalAt5071D0() == 6;
            text_.WriteWaitMarkEnabledAt0C(oldWait);
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
        // 4B4376：10/12/14/18/20、负值和22以外值是原空分支，不抛自造状态错误。
        return false;
    }
}
}
