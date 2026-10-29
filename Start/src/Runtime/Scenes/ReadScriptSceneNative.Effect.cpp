#include "ReadScriptSceneNative.Internal.h"
#include <Runtime/ScriptNativeBody.h>
#include <new>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
{
namespace
{
// 5B5668/5B5684/5B56A0：原 CP932 char*，由 CReadScript 外层异常显示消费。
char unknownEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x8E\xED\x97\xDE\x82\xAA\x82\xC8\x82\xA2\x81\x49\x81\x49";
char emptyEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x83\x47\x83\x89\x81\x5B\x81\x40\x82\xBB\x82\xCC\x82\x50";
char failedEffect[] = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x82\xCC\x83\x47\x83\x89\x81\x5B\x81\x40\x82\xBB\x82\xCC\x82\x51";

void Suspend(OwnedStateView& state)
{
    state.savedMainStateAt4C = state.mainState;
    state.savedSubstateAt50 = state.substate;
    state.mainState = 2;
    state.substate = 0;
}
// 只装配已有原生业务叶，按使用点才取服务；Has/Ready/Release不要求Mask或VM源。
class EffectLeaves final : public IReadScriptUpdateEffectActions
{
public:
    EffectLeaves(ReadScriptEffectOwnerNative& owner, IReadScriptSceneNativeSource& source) noexcept
        : owner_(owner), source_(source) {}
    bool PrepareLayersAt46A980() override
    {
        auto& services = source_.EffectFactory();
        ReadScriptEffectNativeCallScope scope(services);
        static_cast<void>(services.draw.CaptureDrawManagerAfter413BE0());
        // Ero Zemi 4B65B5：场景效果隐藏时保留未被脚本清除的日期栏。
        return PrepareDrawLayersAt46A980(services.draw, 1) != 0;
    }
    void RestorePatternsAt4C4330() override
    {
        auto& services = source_.EffectFactory();
        ReadScriptEffectNativeCallScope scope(services);
        auto* manager = services.draw.CaptureDrawManagerAfter413BE0();
        auto* window = reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*manager, 0));
        RestoreLayerWindowPatternsNativeAt4C4330(*window, services.pattern);
    }
    std::uint16_t ReadSelector() override
    {
        auto* script = source_.MoveConstruction().CaptureScriptAfter413BE0();
        const auto* effect = ResolveEnvironmentObjectReference<ScriptEffectNative>(script->effectAt0C);
        return static_cast<const volatile std::uint16_t&>(effect->status);
    }
    const char* ReadResourceName() override
    {
        auto* script = source_.MoveConstruction().CaptureScriptAfter413BE0();
        return ResolveEnvironmentObjectReference<ScriptEffectNative>(script->effectAt0C)->resourceNameAt04.GetString();
    }
    void CreateBlend() override { CreateReadScriptBlendNativeAt4A7BF0(owner_, source_.EffectFactory()); }
    void CreateMosaic() override { CreateReadScriptMosaicNativeAt4B45C0(owner_, source_.EffectFactory()); }
    void CreateUniversal(const char* name) override
    { CreateReadScriptUniversalNativeAt4C3A20(owner_, name, source_.EffectFactory(), source_.EffectMasks()); }
    void CreateTransit(std::int32_t opcode) override
    { CreateReadScriptTransitNativeAt4C3650(owner_, opcode, source_.EffectFactory()); }
    void CreateMove() override
    {
        auto& services = source_.EffectFactory();
        ReadScriptEffectNativeCallScope scope(services);
        void* raw = ::operator new(0x24);
        MoveEffectNativeBody24h* created = nullptr;
        try
        {
            if (raw)
            {
                created = ::new(raw) MoveEffectNativeBody24h;
                ConstructMoveEffectNativeAt49C0F0(*created, source_.MoveConstruction(), services.pattern);
            }
        }
        catch (...) { ::operator delete(raw, std::size_t{0x24}); throw; }
        owner_.InstallEffect(created, 0x24);
    }
    void CreateFlash(const char* name, std::int32_t count) override
    {
        auto& services = source_.EffectFactory();
        ReadScriptEffectNativeCallScope scope(services);
        owner_.InstallEffect(NewReadScriptFlashNative(name, count, source_.FlashShakeSource(), services.flashShake), 0x34);
    }
    void CreateShake(std::int32_t mode, std::int32_t scale, std::int32_t interval) override
    {
        auto& services = source_.EffectFactory();
        ReadScriptEffectNativeCallScope scope(services);
        owner_.InstallEffect(NewReadScriptShakeNative(mode, scale, interval, source_.FlashShakeSource(), services.flashShake), 0x28);
    }
    bool HasEffect() const override { return owner_.HasEffectAt1C(); }
    std::uint8_t ReadyEffect() const override { return owner_.ReadyEffectAtVslot08(); }
    void ReleaseResources() override { owner_.ReleaseResourcesAt4C0990(); }
private:
    ReadScriptEffectOwnerNative& owner_;
    IReadScriptSceneNativeSource& source_;
};
}

void EffectFlow::CreateDeferredEffect()
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

bool EffectFlow::RunAt4C4760(OwnedStateView& state)
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

bool RunEffect(OwnedStateView& state, ReadScriptEffectOwnerNative& owner, IReadScriptSceneNativeSource& source)
{
    EffectLeaves actions(owner, source);
    EffectFlow flow(actions);
    return flow.RunAt4C4760(state);
}
}
