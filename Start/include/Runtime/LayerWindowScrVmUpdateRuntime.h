#pragma once

#include <Runtime/LayerWindowEffectRuntime.h>
#include <Runtime/LayerWindowScrResultApplier.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmUpdateHandler.h>

#include <cstddef>

namespace KaGuYa::Start::Runtime
{
// 对应 opcode 6 / 0x4D2800 访问的生产图层边界。
// ResultLayer 应用、效果选择与活动坐标均作用于同一个 LayerWindow 对象图。
class LayerWindowScrVmUpdateRuntime final : public IScrVmUpdateRuntimeBoundary
{
public:
    LayerWindowScrVmUpdateRuntime(
        ScrScriptRuntimeState& scriptRuntime,
        LayerWindowRuntime& layerWindow,
        LayerWindowScrResultApplier& resultApplier) noexcept;

    void ApplyResultLayerList(std::int32_t listIndex) override;
    void SelectLayerEffect(std::int32_t selector) override;
    [[nodiscard]] bool ReadActiveResultPair(
        std::size_t index, ScrVmUpdatePair& destination) const override;

private:
    ScrScriptRuntimeState* scriptRuntime_ = nullptr;
    LayerWindowRuntime* layerWindow_ = nullptr;
    LayerWindowScrResultApplier* resultApplier_ = nullptr;
};
}
