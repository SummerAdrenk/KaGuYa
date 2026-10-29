#include <Runtime/ReadScriptUpdateEffectRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 5B5668/5B5684/5B56A0：原 CP932 char*，由 CReadScript 外层异常显示消费。
char unknownEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x8E\xED\x97\xDE\x82\xAA\x82\xC8\x82\xA2\x81\x49\x81\x49";
char emptyEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x83\x47\x83\x89\x81\x5B\x81\x40\x82\xBB\x82\xCC\x82\x50";
char failedEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x83\x47\x83\x89\x81\x5B\x81\x40\x82\xBB\x82\xCC\x82\x51";

void Suspend(ReadScriptSceneState& state)
{
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
}
}

ReadScriptUpdateEffectRuntime::ReadScriptUpdateEffectRuntime(ReadScriptEffectOwnerRuntime& owner,
    const ScrVmScriptStorage& storage, IReadScriptEffectFactory& factory) noexcept
    : fixed_(std::in_place, owner, storage, factory), actions_(&*fixed_) {}
ReadScriptUpdateEffectRuntime::ReadScriptUpdateEffectRuntime(IReadScriptUpdateEffectActions& actions) noexcept
    : actions_(&actions) {}
ReadScriptUpdateEffectRuntime::ReadScriptUpdateEffectRuntime(const ReadScriptUpdateEffectRuntime& other) noexcept
    : fixed_(other.fixed_), actions_(fixed_ ? &*fixed_ : other.actions_) {}

bool FixedReadScriptUpdateEffectActions::PrepareLayersAt46A980() { return factory_.PrepareLayersAt46A980(); }
void FixedReadScriptUpdateEffectActions::RestorePatternsAt4C4330() { factory_.RestorePatternsAt4C4330(); }
std::uint16_t FixedReadScriptUpdateEffectActions::ReadSelector() { return storage_.EffectAt4AD140().status; }
const char* FixedReadScriptUpdateEffectActions::ReadResourceName() { return storage_.EffectAt4AD140().resourceNameAt04.GetString(); }
void FixedReadScriptUpdateEffectActions::CreateBlend() { factory_.CreateBlendAt4A7BF0(owner_); }
void FixedReadScriptUpdateEffectActions::CreateMosaic() { factory_.CreateMosaicAt4B45C0(owner_); }
void FixedReadScriptUpdateEffectActions::CreateUniversal(const char* name) { factory_.CreateUniversalAt4C3A20(owner_, name); }
void FixedReadScriptUpdateEffectActions::CreateTransit(std::int32_t opcode) { factory_.CreateTransitAt4C3650(owner_, opcode); }
void FixedReadScriptUpdateEffectActions::CreateMove() { owner_.InstallEffect(factory_.CreateMoveAt49C0F0()); }
void FixedReadScriptUpdateEffectActions::CreateFlash(const char* name, std::int32_t count)
    { owner_.InstallEffect(factory_.CreateFlashAt49BCA0(name, count)); }
void FixedReadScriptUpdateEffectActions::CreateShake(std::int32_t mode, std::int32_t scale, std::int32_t interval)
    { owner_.InstallEffect(factory_.CreateShakeAt49D4E0(mode, scale, interval)); }
bool FixedReadScriptUpdateEffectActions::HasEffect() const { return owner_.HasEffectAt1C(); }
std::uint8_t FixedReadScriptUpdateEffectActions::ReadyEffect() const { return owner_.ReadyEffectAtVslot08(); }
void FixedReadScriptUpdateEffectActions::ReleaseResources() { owner_.ReleaseResourcesAt4C0990(); }

void ReadScriptUpdateEffectRuntime::CreateDeferredEffect()
{
    const std::uint16_t selector = actions_->ReadSelector();
    if (selector <= 2)
        actions_->CreateShake(selector == 0 ? 2 : selector - 1, 10, 2);
    else if (selector <= 11)
    {
        const char* name = selector <= 5 ? "bg_white.bgd" : selector <= 8 ? "bg_red.bgd" : "bg_yellow.bgd";
        actions_->CreateFlash(name, (selector - 3) % 3 + 1);
    }
    else if (selector >= 13 && selector <= 20)
        actions_->CreateShake(selector - 10, 1, 2);
    else if (selector >= 21 && selector <= 24)
        actions_->CreateShake(selector - 10, 10, selector <= 22 ? 2 : 1);
    else
        throw unknownEffect;
}

bool ReadScriptUpdateEffectRuntime::RunAt4C4760(ReadScriptSceneState& state)
{
    switch (state.substate)
    {
    case 0:
        state.substate = 1;
        if (actions_->PrepareLayersAt46A980())
        {
            Suspend(state);
            return true;
        }
        [[fallthrough]];
    case 1:
    {
        actions_->RestorePatternsAt4C4330();
        state.mainState = 0;
        state.substate = 0;
        // 原顺序在恢复窗口之后读当前 selector；回调可修改同一 effect 状态。
        const auto selector = actions_->ReadSelector();
        if (selector != 0xFFFFu)
        {
            if ((selector & 0x8000u) == 0)
            {
                if (selector != 12)
                {
                    state.mainState = 1;
                    state.substate = 2;
                    return false;
                }
                actions_->CreateMove();
            }
            else
            {
                const int opcode = selector & 0x7FFFu;
                if (opcode == 0) actions_->CreateBlend();
                else if (opcode == 1) actions_->CreateUniversal(actions_->ReadResourceName());
                else if (opcode == 2) actions_->CreateMosaic();
                else actions_->CreateTransit(opcode);
            }
        }
        // FFFF 不创建效果，但不能删除或跳过仍存在的旧引用。
        if (!actions_->HasEffect()) return false;
        if (!actions_->ReadyEffect())
        {
            actions_->ReleaseResources();
            return false;
        }
        Suspend(state);
        return true;
    }
    case 2:
        CreateDeferredEffect();
        if (!actions_->HasEffect()) throw emptyEffect;
        if (!actions_->ReadyEffect()) throw failedEffect;
        state.savedMainStateAt4C = 0;
        state.savedSubstateAt50 = 0;
        state.mainState = 2;
        state.substate = 0;
        return true;
    default:
        return false;
    }
}
}
