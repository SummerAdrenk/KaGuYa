#include <Runtime/ReadScriptSuspendWaitRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
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

ReadScriptSuspendWaitRuntime::ReadScriptSuspendWaitRuntime(
    IReadScriptSuspendEffectOwner& effects, ScrVmRuntimeStateBundle& bundle,
    const ScrVmExecutableProfile& profile, const SaveDataEnvironmentArchive& save,
    UnSaveDataRuntime& unsave, Platform::MouseView mouse,
    Platform::KeyboardView keyboard, DrawLayersView manager,
    IReadScriptSoundWaitSource& sound, IGraphicsScreenUpdateApi& screen,
    std::uint8_t& rootByteAt1B0,PatternLayerNativeRenderServices* patterns) noexcept
    : effects_(effects),fixedState_(std::in_place,bundle,profile,save,unsave,mouse,keyboard,rootByteAt1B0),
      state_(&*fixedState_),manager_(manager),patterns_(patterns),sound_(sound),screen_(screen){}
ReadScriptSuspendWaitRuntime::ReadScriptSuspendWaitRuntime(IReadScriptSuspendEffectOwner& effects,
    ScrVmRuntimeStateBundle& bundle,const ScrVmExecutableProfile& profile,const SaveDataEnvironmentArchive& save,
    UnSaveDataRuntime& unsave,Platform::MouseView mouse,Platform::KeyboardView keyboard,IDrawLayersViewSource& manager,
    IReadScriptSoundWaitSource& sound,IGraphicsScreenUpdateApi& screen,std::uint8_t& rootByteAt1B0,
    PatternLayerNativeRenderServices& patterns) noexcept
    :ReadScriptSuspendWaitRuntime(effects,bundle,profile,save,unsave,mouse,keyboard,DrawLayersView(nullptr),sound,screen,rootByteAt1B0,&patterns)
{source_=&manager;}
ReadScriptSuspendWaitRuntime::ReadScriptSuspendWaitRuntime(IReadScriptSuspendEffectOwner& effects,IReadScriptWaitStateSource& state,
    IDrawLayersViewSource& manager,IReadScriptSoundWaitSource& sound,IGraphicsScreenUpdateApi& screen,
    PatternLayerNativeRenderServices& patterns) noexcept
    :effects_(effects),state_(&state),source_(&manager),patterns_(&patterns),sound_(sound),screen_(screen){}
DrawLayersView ReadScriptSuspendWaitRuntime::CaptureLayers() const
{return source_?source_->CaptureDrawLayersAt413BE0():manager_;}
std::uint8_t ReadScriptSuspendWaitRuntime::PrepareLayersAt46A980()
{
    if(source_)return PrepareDrawLayersViewAt46A980(*source_, 1);
    struct Fixed final:IDrawLayersViewSource
    {DrawLayersView value;explicit Fixed(DrawLayersView view):value(view){}DrawLayersView CaptureDrawLayersAt413BE0() override{return value;}} fixed(manager_);
    return PrepareDrawLayersViewAt46A980(fixed, 1);
}

bool ReadScriptSuspendWaitRuntime::MousePressedEdge(const std::size_t button) const
{
    return state_->MousePressedEdge(button);
}

bool ReadScriptSuspendWaitRuntime::SuppressEffectAt4C3430() const
{
    return state_->SuppressEffectAt4C3430();
}

bool ReadScriptSuspendWaitRuntime::SkipEffectAt4AA970() const
{
    return state_->SkipEffectAt4AA970();
}

void ReadScriptSuspendWaitRuntime::WriteWaitSkipped(const std::int32_t value)
{
    // 每次解析名称并访问当前 generation；不存在时由值表抛错，不缓存索引或补默认值。
    state_->WriteWaitSkippedAfter413BE0(value);
}

bool ReadScriptSuspendWaitRuntime::RunSuspendAt4AC4C0(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 0:
        state.suspendValueAtB4 = std::bit_cast<std::int32_t>(state_->CaptureUnSaveAt4180C0().ModeAt04());
        state_->CaptureUnSaveAt4180C0().ModeAt04() = 0;
        state.drawEnabled = false;
        if (effects_.HasEffectAt1C())
        {
            if (!SuppressEffectAt4C3430()) state_->SetMouseCountdownAt28(5);
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
        state_->SetMouseCountdownAt28(0);
        state.substate = AddWrapping(state.substate, 1);
        // 清 +28 后重新判断；不能复用先前的输入结果，或将系统抑制等同用户跳过。
        if (SkipEffectAt4AA970()) WriteWaitSkipped(1);
        return true;
    case 2:
        effects_.ReleaseResourcesAt4C0990();
        state.substate = AddWrapping(state.substate, 1);
        CaptureLayers().RegisteredLayers().RenderDirtyRects(screen_);
        if (state_->ReadHeldAt4198D0(6) || (ReadUnSaveFlagsByte(*state_) & 2u) != 0)
            state_->ClearRootByteAt1B0After413BE0();
        // 此帧仍禁止普通 Draw；即使没有 Effect，也不能合并到 sub3。
        return false;
    case 3:
    {
        auto& unsave=state_->CaptureUnSaveAt4180C0();
        unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.suspendValueAtB4.value());
        state.mainState = state.savedMainStateAt4C.value();
        state.substate = state.savedSubstateAt50.value();
        state.drawEnabled = true;
        return true;
    }
    default:
        return false;
    }
}

bool ReadScriptSuspendWaitRuntime::CompleteWait(ReadScriptSceneState& state)
{
    // 4C52BA/52C1先选UnSave；52C9才读scene+10，回调后的值不能提前冻结。
    auto& unsave=state_->CaptureUnSaveAt4180C0();
    unsave.ModeAt04() = std::bit_cast<std::uint32_t>(state.interactionValueAt10.value());
    state.mainState = 0;
    state.substate = 0;
    return true;
}

bool ReadScriptSuspendWaitRuntime::RunWaitAt4C4FC0(ReadScriptSceneState& state)
{
    bool skip = false;
    if (state.substate != 0)
    {
        skip = state_->ReadPressedAt47F4E0(0) || state_->ReadPressedAt47F4E0(5) ||
            state_->ReadHeldAt4198D0(6) || MousePressedEdge(0) || MousePressedEdge(1) ||
            (ReadUnSaveFlagsByte(*state_) & 2u) != 0;
        if (skip) WriteWaitSkipped(1);
    }
    switch (state.substate)
    {
    case 0:
        state.interactionValueAt10 = std::bit_cast<std::int32_t>(state_->CaptureUnSaveAt4180C0().ModeAt04());
        state_->CaptureUnSaveAt4180C0().ModeAt04() = 0;
        // 4C5151/4C5172/4C5194逐项读后即发布；未初始化读取明确拒绝，不预校验整块。
        state.substate = state_->ReadWaitValueAfter413BE0(0);
        state.waitValueAt58 = state_->ReadWaitValueAfter413BE0(1);
        state.waitValueAt5C = state_->ReadWaitValueAfter413BE0(2);
        // 4C519A先捕获一次外层receiver；46A980内部七槽仍各自重新捕获。
        if(source_)static_cast<void>(CaptureLayers());
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
        if (state.waitValueAt58.value() == 0) return CompleteWait(state);
        state.waitValueAt58 = AddWrapping(state.waitValueAt58.value(), 0xFFFFFFFFu);
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

bool ReadScriptSuspendWaitRuntime::WaitForSound(ReadScriptSceneState& state, const bool skip)
{
    const auto selector = state.waitValueAt58.value();
    // 4C5229先固定selector用于分派，5231再Ensure；分支内4188F0只解析已捕获root。
    const auto root = sound_.CaptureRootAfterEnsureAt413BE0();
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
            absoluteSlot = std::bit_cast<std::uint32_t>(state.waitValueAt58.value()) - 2u;
            absoluteSlot += sound.RemainingBeginAt20();
        }
        if (skip) sound.StopAt4383B0(absoluteSlot);
        else if (sound.IsPlayingAt419370(absoluteSlot)) return false;
    }
    return CompleteWait(state);
}

bool ReadScriptSuspendWaitRuntime::WaitForSprite(ReadScriptSceneState& state, const bool skip)
{
    if (state_->CaptureSaveAt4180C0().bytesAt6D[0] == 0) return CompleteWait(state);
    if (skip && (ReadUnSaveFlagsByte(*state_) & 2u) == 0 && !state_->ReadHeldAt4198D0(6))
        return CompleteWait(state);

    // 4C5346先固定第一个byte，再413BE0；4C5384在enabled检查后重新读第二个byte。
    const auto patternIndex = std::bit_cast<std::uint32_t>(state.waitValueAt58.value()) & 0xFFu;
    const auto item=CaptureLayers().WindowZero().PatternAt4AD2A0(patternIndex);
    if (!item) throw kPatternMissing;
    if (item.EnabledByteAt0C() == 0) throw kPatternHidden;
    // SAR 8 后 movzx AL，仅取第二个 byte；高 16 位和符号都不进入索引。
    const auto spriteIndex = (std::bit_cast<std::uint32_t>(state.waitValueAt58.value()) >> 8) & 0xFFu;
    if (spriteIndex >= item.SpriteCountAtControl3C()) throw kSpriteMissing;
    if (item.SpriteAt4AD550(spriteIndex).LogicalNameLengthAt0C()==0) throw kSpriteUnnamed;
    if (state.substate == 4 && item.SpriteAt4AD550(spriteIndex).RepeatByteAt44())
        throw kSpriteLooping;
    const auto sprite = item.SpriteAt4AD550(spriteIndex);
    if (skip)
    {
        sprite.FinishForWaitAt4C540E();
        item.InvalidateAllAt443650();
        item.RefreshAt443B00(patterns_);
        return CompleteWait(state);
    }
    if ((sprite.FlagsByteAt34() & 1) == 0) return CompleteWait(state);
    if (state.substate == 5 &&
        item.SpriteAt4AD550(spriteIndex).ResultAt40() >= state.waitValueAt5C.value())
        return CompleteWait(state);
    return false;
}
}
