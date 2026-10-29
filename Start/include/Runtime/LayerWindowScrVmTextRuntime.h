#pragma once

#include <Runtime/LayerWindowEffectRuntime.h>
#include <Runtime/LayerWindowScrResultApplier.h>
#include <Runtime/MappedSoundSlotRuntime.h>
#include <Runtime/ScrScriptRuntime.h>
#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/ScrVmTextHandler.h>

namespace KaGuYa::Start::Runtime
{
// 对应 opcode 7 / 0x4D1E50 访问的生产对象边界。
// 旧接口沿用了“StringGroup”占位名；IDA 已确认两组实际属于专用音效映射槽。
class LayerWindowScrVmTextRuntime final : public IScrVmTextRuntimeBoundary
{
public:
    LayerWindowScrVmTextRuntime(
        ScrScriptRuntimeState& scriptRuntime,
        const IScrResourceNameSource& resourceNames,
        LayerWindowScrResultApplier& resultApplier,
        LayerWindowRuntime& layerWindow,
        IScrVmSoundRuntimeBoundary& soundRuntime) noexcept;

    [[nodiscard]] std::size_t FirstStringGroupCountAt50() const override;
    [[nodiscard]] std::size_t SecondStringGroupCountAt48() const override;
    void ApplyResultLayerList(std::int32_t listIndex) override;
    void ApplyFirstStringSlot(
        std::int32_t value,
        std::size_t groupIndex) override;
    void ApplySecondStringSlot(
        std::int32_t value,
        std::size_t groupIndex) override;
    void SelectEffect(std::int32_t selector) override;

private:
    ScrScriptRuntimeState* scriptRuntime_ = nullptr;
    LayerWindowScrResultApplier* resultApplier_ = nullptr;
    LayerWindowRuntime* layerWindow_ = nullptr;
    MappedSoundSlotRuntime mappedSound_;
};
}
