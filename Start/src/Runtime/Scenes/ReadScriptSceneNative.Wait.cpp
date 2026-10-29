#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
{
namespace
{
// 保留原 CP932 char* 异常类型，交由外层 Update 的原文诊断分支处理。
char kPatternMissing[] = "\x83\x8C\x83\x43\x83\x84\x94\xD4\x8D\x86\x82\xAA\x82\xA8\x82\xA9\x82\xB5\x82\xA2\x81\x49: [WaitStep]"; // 5B56EC
char kPatternHidden[] = "\x83\x8C\x83\x43\x83\x84\x82\xAA\x95\x5C\x8E\xA6\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49: [WaitStep]"; // 5B5710
char kSpriteMissing[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x94\xD4\x8D\x86\x82\xAA\x82\xA8\x82\xA9\x82\xB5\x82\xA2\x81\x49: [WaitStep]"; // 5B573C
char kSpriteUnnamed[] = "\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x83\x8D\x81\x5B\x83\x68\x82\xB3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49: [WaitStep]"; // 5B5764
char kSpriteLooping[] = "\x83\x41\x83\x6A\x83\x81\x81\x5B\x83\x56\x83\x87\x83\x93\x82\xAA\x83\x8B\x81\x5B\x83\x76\x82\xB5\x82\xC4\x82\xA2\x82\xE9\x82\xCC\x82\xC5\x8F\x49\x97\xB9\x82\xF0\x91\xD2\x82\xC4\x82\xDC\x82\xB9\x82\xF1\x81\x49: [WaitStep]"; // 5B5798

std::int32_t AddWrapping(std::int32_t value, std::uint32_t delta) noexcept
{
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(value) + delta);
}
std::uint8_t ReadUnSaveFlagsByte(IReadScriptWaitStateSource& source)
{
    // 4C50B1/4C5323/4AC66E均为byte读取；不能扩大为整个DWORD访问。
    return *reinterpret_cast<const volatile std::uint8_t*>(&source.CaptureUnSaveAt4180C0().FlagsAt14());
}
}


DrawLayersView WaitFlow::CaptureLayers() const
{ return source_.DrawLayers().CaptureDrawLayersAt413BE0(); }
std::uint8_t WaitFlow::PrepareLayersAt46A980()
{ return PrepareDrawLayersViewAt46A980(source_.DrawLayers(), 1); }

bool WaitFlow::MousePressedEdge(const std::size_t button) const
{
    return source_.WaitState().MousePressedEdge(button);
}

bool WaitFlow::SuppressEffectAt4C3430() const
{
    return source_.WaitState().SuppressEffectAt4C3430();
}

bool WaitFlow::SkipEffectAt4AA970() const
{
    return source_.WaitState().SkipEffectAt4AA970();
}

void WaitFlow::WriteWaitSkipped(const std::int32_t value)
{
    // 每次解析名称并访问当前 generation；不存在时由值表抛错，不缓存索引或补默认值。
    source_.WaitState().WriteWaitSkippedAfter413BE0(value);
}

bool WaitFlow::RunSuspendAt4AC4C0(OwnedStateView& state)
{
    switch (state.substate)
    {
    case 0:
        state.suspendValueAtB4 = std::bit_cast<std::int32_t>(source_.WaitState().CaptureUnSaveAt4180C0().ModeAt04());
        source_.WaitState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        state.drawEnabled = false;
        if (effects_.HasEffectAt1C())
        {
            if (!SuppressEffectAt4C3430()) source_.WaitState().SetMouseCountdownAt28(5);
            state.substate = AddWrapping(state.substate, 1);
        }
        else state.substate = 2;
        WriteWaitSkipped(0);
        return true;
    case 1:
        if (!SuppressEffectAt4C3430() && !SkipEffectAt4AA970() &&
            effects_.TickEffectAtVslot00())
        {
            effects_.DrawEffectAtVslot04();
            return false;
        }
        source_.WaitState().SetMouseCountdownAt28(0);
        state.substate = AddWrapping(state.substate, 1);
        // 清 +28 后重新判断；不能复用先前的输入结果，或将系统抑制等同用户跳过。
        if (SkipEffectAt4AA970()) WriteWaitSkipped(1);
        return true;
    case 2:
        effects_.ReleaseResourcesAt4C0990();
        state.substate = AddWrapping(state.substate, 1);
        CaptureLayers().RegisteredLayers().RenderDirtyRects(source_.Screen());
        if (source_.WaitState().ReadHeldAt4198D0(6) || (ReadUnSaveFlagsByte(source_.WaitState()) & 2u) != 0)
            source_.WaitState().ClearRootByteAt1B0After413BE0();
        // 此帧仍禁止普通 Draw；即使没有 Effect，也不能合并到 sub3。
        return false;
    case 3:
    {
        auto& unsave=source_.WaitState().CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.suspendValueAtB4);
        state.mainState = state.savedMainStateAt4C;
        state.substate = state.savedSubstateAt50;
        state.drawEnabled = true;
        return true;
    }
    default:
        return false;
    }
}

bool WaitFlow::CompleteWait(OwnedStateView& state)
{
    // 4C52BA/52C1先选UnSave；52C9才读scene+10，回调后的值不能提前冻结。
    auto& unsave=source_.WaitState().CaptureUnSaveAt4180C0();
    unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10);
    state.mainState = 0;
    state.substate = 0;
    return true;
}

bool WaitFlow::RunWaitAt4C4FC0(OwnedStateView& state)
{
    bool skip = false;
    if (state.substate != 0)
    {
        skip = source_.WaitState().ReadPressedAt47F4E0(0) || source_.WaitState().ReadPressedAt47F4E0(5) ||
            source_.WaitState().ReadHeldAt4198D0(6) || MousePressedEdge(0) || MousePressedEdge(1) ||
            (ReadUnSaveFlagsByte(source_.WaitState()) & 2u) != 0;
        if (skip) WriteWaitSkipped(1);
    }
    switch (state.substate)
    {
    case 0:
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(source_.WaitState().CaptureUnSaveAt4180C0().ModeAt04());
        source_.WaitState().CaptureUnSaveAt4180C0().ModeAt04() = 0;
        // 4C5151/4C5172/4C5194逐项读后即发布；直接使用原标量，不预校验整块。
        state.substate = source_.WaitState().ReadWaitValueAfter413BE0(0);
        state.waitValueAt58 = source_.WaitState().ReadWaitValueAfter413BE0(1);
        state.waitValueAt5C = source_.WaitState().ReadWaitValueAfter413BE0(2);
        // 4C519A先捕获一次外层receiver；46A980内部七槽仍各自重新捕获。
        static_cast<void>(CaptureLayers());
        if (PrepareLayersAt46A980() != 0)
        {
            state.savedMainStateAt4C = state.mainState;
            state.savedSubstateAt50 = state.substate;
            state.mainState = 2;
            state.substate = 0;
        }
        WriteWaitSkipped(0);
        return true;
    case 2:
        if (state.waitValueAt58 == 0) return CompleteWait(state);
        state.waitValueAt58 = AddWrapping(state.waitValueAt58, 0xFFFFFFFFu);
        break;
    case 3:
        return WaitForSound(state, skip);
    case 4:
    case 5:
        return WaitForSprite(state, skip);
    default:
        break;
    }
    return skip ? CompleteWait(state) : false;
}

bool WaitFlow::WaitForSound(OwnedStateView& state, const bool skip)
{
    const auto selector = state.waitValueAt58;
    // 4C5229先固定selector用于分派，5231再Ensure；分支内4188F0只解析已捕获root。
    const auto root = source_.SoundWait().CaptureRootAfterEnsureAt413BE0();
    if (selector == 1)
    {
        const auto sound = root.Resolve();
        if (skip) sound.StopVoicesAt4384D0();
        else if (sound.AnyVoiceAt4196C0()) return false;
    }
    else
    {
        // 原 +20 是第三边界，运算为 DWORD 回绕；不得把负 selector 转为宽 size_t 后加。
        const auto sound = root.Resolve();
        // 4C524F/4C529F仅普通音效分支重读selector；保留getter回调后的DWORD运算。
        std::uint32_t absoluteSlot;
        if (selector == 0) absoluteSlot = sound.MusicSlotAt10();
        else
        {
            absoluteSlot = std::bit_cast<std::uint32_t>(state.waitValueAt58) - 2u;
            absoluteSlot += sound.RemainingBeginAt20();
        }
        if (skip) sound.StopAt4383B0(absoluteSlot);
        else if (sound.IsPlayingAt419370(absoluteSlot)) return false;
    }
    return CompleteWait(state);
}

bool WaitFlow::WaitForSprite(OwnedStateView& state, const bool skip)
{
    if (source_.WaitState().CaptureSaveAt4180C0().bytesAt6D[0] == 0) return CompleteWait(state);
    if (skip && (ReadUnSaveFlagsByte(source_.WaitState()) & 2u) == 0 && !source_.WaitState().ReadHeldAt4198D0(6))
        return CompleteWait(state);

    // 4C5346先固定第一个byte，再413BE0；4C5384在enabled检查后重新读第二个byte。
    const auto patternIndex = std::bit_cast<std::uint32_t>(state.waitValueAt58) & 0xFFu;
    const auto item=CaptureLayers().WindowZero().PatternAt4AD2A0(patternIndex);
    if (!item) throw kPatternMissing;
    if (item.EnabledByteAt0C() == 0) throw kPatternHidden;
    // SAR 8 后 movzx AL，仅取第二个 byte；高 16 位和符号都不进入索引。
    const auto spriteIndex = (std::bit_cast<std::uint32_t>(state.waitValueAt58) >> 8) & 0xFFu;
    if (spriteIndex >= item.SpriteCountAtControl3C()) throw kSpriteMissing;
    if (item.SpriteAt4AD550(spriteIndex).LogicalNameLengthAt0C()==0) throw kSpriteUnnamed;
    if (state.substate == 4 && item.SpriteAt4AD550(spriteIndex).RepeatByteAt44())
        throw kSpriteLooping;
    const auto sprite = item.SpriteAt4AD550(spriteIndex);
    if (skip)
    {
        sprite.FinishForWaitAt4C540E();
        item.InvalidateAllAt443650();
        item.RefreshAt443B00(&source_.Patterns());
        return CompleteWait(state);
    }
    if ((sprite.FlagsByteAt34() & 1) == 0) return CompleteWait(state);
    if (state.substate == 5 &&
        item.SpriteAt4AD550(spriteIndex).ResultAt40() >= state.waitValueAt5C)
        return CompleteWait(state);
    return false;
}

bool RunSuspend(OwnedStateView& state, ReadScriptEffectOwnerNative& effects, IReadScriptSceneNativeSource& source)
{
    WaitFlow flow(effects, source);
    return flow.RunSuspendAt4AC4C0(state);
}
bool RunWait(OwnedStateView& state, IReadScriptSceneNativeSource& source)
{
    ReadScriptEffectOwnerNative effects(state.body.effectsAt1C);
    WaitFlow flow(effects, source);
    return flow.RunWaitAt4C4FC0(state);
}
}

