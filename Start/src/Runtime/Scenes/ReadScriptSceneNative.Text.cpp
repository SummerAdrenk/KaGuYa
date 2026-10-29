#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
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

using Release=ReadScriptCommandRelease;

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


bool TextFlow::SuspendForLayers(OwnedStateView& state)
{
    // 4B0130：先更新全部图层；只有原AL非零才保存返回状态并进入mainState2。
    if (!layers_.PrepareLayersAt46A980()) return false;
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
    return true;
}

void TextFlow::ToggleAuto()
{
    source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt18() ^= 2u;
    source_.Tools().SetToolModeAt434FB0(false);
}

void TextFlow::EnableSkip()
{
    // 4B2E3B/4B2E4D、4B3904/4B3916：先提交UnSave DWORD，再独立捕获Save BYTE。
    source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
    if ((source_.InteractionState().CaptureSaveAt4180C0().bytesAt34[4] & 4u) != 0) source_.Text().StopVoiceAt4384D0();
}

void TextFlow::EnterWait(OwnedStateView& state)
{
    source_.Tools().SetToolModeAt434FB0(true);
    layers_.SetLayerVisible(4, 1, 0);
    state.substate = 3;
}

bool TextFlow::StartSaveLoad(OwnedStateView& state, std::int32_t substate)
{
    source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
    state.substate = substate;
    return SuspendForLayers(state);
}

[[noreturn]] void TextFlow::ThrowTextFailure(OwnedStateView& state)
{
    // 4B439E 先查询文件名，4B43BF 才修改 owner+B0；名称查询失败不能覆盖旧错误。
    const auto scriptName = source_.Text().ReadCurrentScriptNameForError();
    FormatRuntimeCStringAt417130(state.errorMessageAtB0, kTextFailure, scriptName);
    throw state.errorMessageAtB0.GetString();
}

bool TextFlow::RunAt4B2730(OwnedStateView& state)
{
    switch (state.substate)
    {
    case 0:
        // 4B27AB忽略SetFormat的返回AL；原异常仍向外层传播。
        source_.Text().SelectFormatAt4C2740(std::bit_cast<std::int32_t>(source_.InteractionState().ReadMessageFormatAt47BDB0()), true);
        source_.Tools().SetToolModeAt434FB0(false);
        source_.Tools().InitializeToolAt4AB910(true);
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04());
        state.substate = 1;
        [[fallthrough]];
    case 1:
    {
        // 4B281B..4B282D先固定目的地址，之后才读owner+10；不能用赋值的RHS先求值。
        auto& unsave = source_.InteractionState().CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10);
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

bool TextFlow::RunRevealing(OwnedStateView& state)
{
    source_.InteractionHost().ClearModalResultAt10();
    source_.InteractionHost().ProcessSoundRequestsAt4A9090();
    auto* root=CaptureReadScriptCommandRoot(source_,0x4B28AE,0x4B28B3);
    if (CompareReadScriptCommandNative(source_,root,0x4B28B9,kSettings,Release::CachedHeader))
    {
        root=CaptureReadScriptCommandRoot(source_,0x4B2901,0x4B2906);
        SetReadScriptCommandNative(source_,root,0x4B290C,"",0);
        state.substate = 15;
        return SuspendForLayers(state);
    }
    bool autoKeyAvailable = true;
    if (source_.Tools().PollConfiguredKeyAt4B0630(0x34))
    {
        ToggleAuto();
        autoKeyAvailable = false;
    }
    if (source_.Tools().PollConfiguredKeyAt4B0630(0x2C))
    {
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
        source_.Tools().SetToolModeAt434FB0(false);
    }
    if (SaveDword(source_.InteractionState().CaptureSaveAt4180C0(), 0) == 0)
    {
        const auto replay = source_.Tools().ReadReplayValue();
        if (!replay && source_.Tools().PollConfiguredKeyAt4B0630(0x24))
            SetReadScriptCommandNative(source_,CaptureReadScriptCommandRoot(source_,0x4B2A6F,0x4B2A74),0x4B2A7A,kLoad,10);
        else if (!replay && source_.Tools().PollConfiguredKeyAt4B0630(0x1C))
            SetReadScriptCommandNative(source_,CaptureReadScriptCommandRoot(source_,0x4B2AAD,0x4B2AB2),0x4B2AB8,kSave,10);
        else if (source_.Tools().PollConfiguredKeyAt4B0630(0x14))
            SetReadScriptCommandNative(source_,CaptureReadScriptCommandRoot(source_,0x4B2AE7,0x4B2AEC),0x4B2AF2,kHide,14);
    }
    const auto input = source_.Tools().UpdateToolAt4BE200();
    const auto text = source_.Text().TickTextAt4BB530((input & 4u) != 0);
    if (text < 0) ThrowTextFailure(state);
    if (text == 0)
    {
        // 4B2B6A实际调用418210：读取Keyboard action6的当前byte，不是Script或WaitMark字段。
        // 4B2BB4/2BD0/2BE8/2BFC各自取源，宽度依次为DWORD/BYTE/DWORD/指针DWORD。
        // 2C16..2C44的SoundParams固定receiver及第二次voices捕获由文字服务保留，
        // 此处的非空检查不能把先前捕获的数组传入或替代该独立取源。
        if (source_.InteractionHost().ScriptHasWaitMarkerAt5C() || (source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() & 3u) == 3u ||
            (FlagByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 ||
            SaveDword(source_.InteractionState().CaptureSaveAt4180C0(), 8) != 0 ||
            (source_.InteractionState().CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
                source_.Text().VoiceArrayAllowedAtSoundParams60() && source_.Text().AnyVoicePlayingAt4196C0()))
        {
            EnterWait(state);
        }
        else state.substate = 22;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 5 ||
        (source_.Tools().ToolSkipPredicateAt4B0760(true) != 1 && source_.Tools().PollConfiguredKeyAt4B0630(0x2C)))
    {
        EnableSkip();
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 4 ||
        (autoKeyAvailable && source_.Tools().ToolAutoPredicateAt4B0470(true) != 1 &&
            source_.Tools().PollConfiguredKeyAt4B0630(0x34))) ToggleAuto();
    return false;
}

bool TextFlow::RunWaiting(OwnedStateView& state)
{
    source_.InteractionHost().ProcessSoundRequestsAt4A9090();
    auto* root=CaptureReadScriptCommandRoot(source_,0x4B2E9D,0x4B2EA2);
    if (CompareReadScriptCommandNative(source_,root,0x4B2EA8,kSave,Release::CachedHeader))
    {
        source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
        root=CaptureReadScriptCommandRoot(source_,0x4B2EF7,0x4B2EFC);
        SetReadScriptCommandNative(source_,root,0x4B2F02,"",0);
        state.substate = 8;
        return SuspendForLayers(state);
    }
    root=CaptureReadScriptCommandRoot(source_,0x4B2F3B,0x4B2F40);
    if (CompareReadScriptCommandNative(source_,root,0x4B2F4A,kLoad,Release::ReturnedCString))
    {
        source_.InteractionHost().BackupBeforeSaveLoadAt4B01A0();
        root=CaptureReadScriptCommandRoot(source_,0x4B2F7F,0x4B2F84);
        SetReadScriptCommandNative(source_,root,0x4B2F8A,"",0);
        state.substate = 9;
        return SuspendForLayers(state);
    }
    root=CaptureReadScriptCommandRoot(source_,0x4B2FC2,0x4B2FC7);
    auto matched=CompareReadScriptCommandNative(source_,root,0x4B2FD1,kQuickSave,Release::ReturnedCString);
    // 4B2FFB/3000在判定前无条件取根；假分支的下一次manager沿用这个Root。
    root=CaptureReadScriptCommandRoot(source_,0x4B2FFB,0x4B3000);
    if (matched)
    {
        SetReadScriptCommandNative(source_,root,0x4B300A,"",0);
        state.substate = 11;
        return false;
    }
    matched=CompareReadScriptCommandNative(source_,root,0x4B303E,kQuickLoad,Release::ReturnedCString);
    root=CaptureReadScriptCommandRoot(source_,0x4B3068,0x4B306D);
    if (matched)
    {
        SetReadScriptCommandNative(source_,root,0x4B3077,"",0);
        state.substate = 13;
        source_.InteractionHost().WriteWaitMarkEnabledAt0C(0);
        return false;
    }
    matched=CompareReadScriptCommandNative(source_,root,0x4B30CB,kHide,Release::ReturnedCString);
    root=CaptureReadScriptCommandRoot(source_,0x4B30F5,0x4B30FA);
    if (matched)
    {
        SetReadScriptCommandNative(source_,root,0x4B3104,"",0);
        state.substate = 5;
        return true;
    }
    if (CompareReadScriptCommandNative(source_,root,0x4B3136,kSettings,Release::ReturnedCString))
    {
        root=CaptureReadScriptCommandRoot(source_,0x4B2901,0x4B2906);
        SetReadScriptCommandNative(source_,root,0x4B290C,"",0);
        state.substate = 15;
        return SuspendForLayers(state);
    }
    const bool immediateResume = state.substate == 22;
    if (immediateResume) EnterWait(state);
    bool autoKeyAvailable = true;
    if (source_.Tools().PollConfiguredKeyAt4B0630(0x34))
    {
        ToggleAuto();
        autoKeyAvailable = false;
    }
    if (source_.Tools().PollConfiguredKeyAt4B0630(0x2C))
    {
        source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt14() |= 2u;
        source_.Tools().SetToolModeAt434FB0(false);
    }
    const auto replay = source_.Tools().ReadReplayValue();
    const auto input = source_.Tools().UpdateToolAt4BE200();
    if (source_.Text().AdvanceTextAt4BC260(static_cast<std::uint8_t>(input)))
    {
        state.substate = 4;
        if ((source_.InteractionState().CaptureSaveAt4180C0().bytesAt34[4] & 4u) != 0) source_.Text().StopVoiceAt4384D0();
        if (immediateResume)
        {
            state.mainState = 0;
            return true;
        }
        return false;
    }
    bool showLog=(FlagByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt1C()) & 2u) != 0;
    if (!showLog)
    {
        // 4B3383只有Ensure，没有随后读取6198C4；case6的键盘叶不带这个前缀。
        source_.EnsureCommandRootAt413BE0(0x4B3383);
        showLog=source_.Text().PollKeyboardAt47F4E0(7);
    }
    if (showLog)
    {
        state.substate = 7;
        return SuspendForLayers(state);
    }
    if ((FlagByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt20()) & 2u) != 0 || source_.Tools().PollConfiguredKeyAt4B0630(0x14) ||
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
        return true;
    }
    if (source_.Tools().ToolEventAt34() == 0 || source_.Tools().PollQuickLoadAt47F6D0())
    {
        state.substate = 13;
        return true;
    }
    if (source_.Tools().ToolEventAt34() == 6)
    {
        // 4B358B/35A3独立检查；35C0固定root后35CD重捕获voices仍在ReplayVoice内。
        if ((FlagByte(source_.InteractionState().CaptureUnSaveAt4180C0().FlagsAt18()) & 2u) == 0 &&
            source_.InteractionState().CaptureUnSaveAt4180C0().ArrayReferenceAt3C().StorageAt3C() &&
            source_.Text().ReplayVoiceAt42C4F0()) source_.Text().AdjustEffectsAt4C2410();
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 5 || source_.Tools().PollConfiguredKeyAt4B0630(0x2C))
    {
        EnableSkip();
        state.substate = 4;
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 4 || (autoKeyAvailable && source_.Tools().PollConfiguredKeyAt4B0630(0x34)))
    {
        ToggleAuto();
        return false;
    }
    if (source_.Tools().ToolEventAt34() == 7)
    {
        // 4B36E3/36F7不是同一receiver；第二次捕获失败须保留已保存的模式。
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
        return true;
    }
    if (source_.Tools().ToolEventAt34() == 10) source_.InteractionHost().ShowPublicDialogAt4C1760();
    return false;
}

void TextFlow::StopForScene()
{
    source_.Text().StopVoiceAt4384D0();
    source_.Text().StopEffectsAt413270();
    source_.Text().RefreshEffectsAt433F60();
}

bool TextFlow::RunCommand(OwnedStateView& state)
{
    switch (state.substate)
    {
    case 5:
        // 4B3A05/3A1A必须分两次取源，前一写入不因后一捕获异常而回滚。
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
        const auto* const command = state.substate == 8 ? kSave : kLoad;
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
        const bool quickLoad = state.substate == 13;
        const auto oldWait = source_.InteractionHost().ReadWaitMarkEnabledAt0C();
        source_.InteractionHost().WriteWaitMarkEnabledAt0C(0);
        source_.InteractionHost().CaptureToolAt4C01F0();
        source_.InteractionHost().SetToolCaptureModeAt4C2300(false);
        source_.InteractionHost().CaptureQuickOperationAt4C1C60();
        source_.InteractionHost().SetToolCaptureModeAt4C2300(true);
        if (quickLoad && source_.InteractionHost().QuickLoadAt485CE0())
        {
            source_.InteractionHost().ClearScenesAt4C1460();
            source_.InteractionHost().QueueSceneAt47F920(kLoadAfter);
            source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
            return false;
        }
        if (!quickLoad) source_.InteractionHost().QuickSaveAt485FC0(1);
        // 4B3F53的失败quick-load先写substate；quick-save则在RestoreTool后写。
        if (quickLoad) state.substate = 3;
        source_.InteractionHost().WriteWaitMarkEnabledAt0C(oldWait);
        source_.InteractionHost().RestoreToolAt4BF4E0();
        if (!quickLoad) state.substate = 3;
        return false;
    }
    case 15:
        StopForScene();
        source_.InteractionState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        source_.InteractionHost().ActivateSceneAt4A9030(kSettings);
        state.substate = 16;
        return false;
    case 16:
        layers_.SetLayerVisible(1, 1, 0);
        source_.Tools().SetToolModeAt434FB0(false);
        state.substate = 1;
        return false;
    case 17:
    {
        if (layers_.UpdateLayerSixAt4BE530()) return false;
        layers_.SetLayerVisible(6, 0, 0);
        // 4B4105..4117与sub1相同：取目的receiver在先，读恢复值在后。
        auto& unsave = source_.InteractionState().CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10);
        state.substate = 3;
        source_.InteractionHost().SetToolCaptureModeAt4C2300(true);
        source_.InteractionHost().RestoreToolAt4BF4E0();
        return false;
    }
    case 19:
        return RunReadScriptReturnTitleNative(state.body,source_.ReturnTitleNativeServices(),ReadScriptReturnTitleCaller::Text);
    case 21:
        source_.InteractionHost().ActivateSceneAt4A9030(kLikability);
        state.substate = 1;
        return false;
    default:
        // 4B4376：10/12/14/18/20、负值和22以外值是原空分支，不抛自造状态错误。
        return false;
    }
}

bool RunText(OwnedStateView& state, Body& body, IReadScriptSceneNativeSource& source)
{
    InteractionLayers layers(body, source);
    TextFlow flow(source, layers);
    return flow.RunAt4B2730(state);
}
}
