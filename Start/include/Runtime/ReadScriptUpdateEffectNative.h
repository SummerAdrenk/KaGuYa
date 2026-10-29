#pragma once

#include <Runtime/ReadScriptUpdateEffectRuntime.h>
#include <Runtime/ReadScriptEffectFactoryNative.h>
#include <Runtime/MoveEffectNativeConstruction.h>

namespace KaGuYa::Start::Runtime
{
// main1全部分支直接使用同一四Ref owner；无现代Effect或备用工厂。
class ReadScriptUpdateEffectNative final : public IReadScriptUpdateEffectActions
{
public:
    ReadScriptUpdateEffectNative(ReadScriptEffectOwnerNative& owner,
        ReadScriptEffectFactoryNativeServices& services, IReadScriptEffectMaskNativeServices& masks,
        IMoveEffectNativeConstructionSource& source, IReadScriptFlashShakeNativeSource& flashShake) noexcept;
    bool PrepareLayersAt46A980() override;
    void RestorePatternsAt4C4330() override;
    std::uint16_t ReadSelector() override;
    const char* ReadResourceName() override;
    void CreateBlend() override;
    void CreateMosaic() override;
    void CreateUniversal(const char*) override;
    void CreateTransit(std::int32_t) override;
    void CreateMove() override;
    void CreateFlash(const char*, std::int32_t) override;
    void CreateShake(std::int32_t, std::int32_t, std::int32_t) override;
    bool HasEffect() const override;
    std::uint8_t ReadyEffect() const override;
    void ReleaseResources() override;
private:
    ReadScriptEffectOwnerNative& owner_;
    ReadScriptEffectFactoryNativeServices& services_;
    IReadScriptEffectMaskNativeServices& masks_;
    IMoveEffectNativeConstructionSource& source_;
    IReadScriptFlashShakeNativeSource& flashShake_;
};
}
