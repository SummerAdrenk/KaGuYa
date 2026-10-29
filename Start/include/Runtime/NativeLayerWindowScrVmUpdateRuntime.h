#pragma once

#include <Runtime/ScrVmUpdateHandler.h>
#include <Runtime/ScriptResultLayerNativeBinding.h>

namespace KaGuYa::Start::Runtime
{
class ILayerWindowEffectNativeSource;

// 借同一CScript正文；每个原取根点现场Capture，不持有或镜像Draw/Window。
class NativeLayerWindowScrVmUpdateRuntime final : public IScrVmUpdateRuntimeBoundary
{
public:
    NativeLayerWindowScrVmUpdateRuntime(ScriptNativeBody& receiver,
        ScriptResultLayerNativeBindingServices& results,
        ILayerWindowEffectNativeSource& effects) noexcept;

    void ApplyResultLayerList(std::int32_t listIndex) override;
    void SelectLayerEffect(std::int32_t selector) override;
    [[nodiscard]] bool ReadActiveResultPair(std::size_t index,
        ScrVmUpdatePair& destination) const override;

private:
    ScriptNativeBody& receiver_;
    ScriptResultLayerNativeBindingServices& results_;
    ILayerWindowEffectNativeSource& effects_;
};
}
