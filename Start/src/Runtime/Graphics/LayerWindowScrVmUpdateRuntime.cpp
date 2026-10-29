#include <Runtime/LayerWindowScrVmUpdateRuntime.h>

#include <Runtime/ScrResultLayerApply.h>

namespace KaGuYa::Start::Runtime
{
LayerWindowScrVmUpdateRuntime::LayerWindowScrVmUpdateRuntime(
    ScrScriptRuntimeState& scriptRuntime,
    LayerWindowRuntime& layerWindow,
    LayerWindowScrResultApplier& resultApplier) noexcept
    : scriptRuntime_(&scriptRuntime),
      layerWindow_(&layerWindow),
      resultApplier_(&resultApplier)
{
}

void LayerWindowScrVmUpdateRuntime::ApplyResultLayerList(
    const std::int32_t listIndex)
{
    const ScrContainer* const script = scriptRuntime_->LoadedScript();
    if (!script)
    {
        // 现代缺正文保护；不将此拒绝冒充4CB030的原异常类别。
        throw ScrVmUpdateCommandException(
            "SCR update has no loaded script container");
    }

    // 4D294B调用4CB030；异常原样外传，由UPDATE的原EH边界区分类别。
    resultApplier_->Apply(*script, listIndex);
}

void LayerWindowScrVmUpdateRuntime::SelectLayerEffect(
    const std::int32_t selector)
{
    static_cast<void>(layerWindow_->Effects().Select(selector));
}

bool LayerWindowScrVmUpdateRuntime::ReadActiveResultPair(
    const std::size_t index, ScrVmUpdatePair& destination) const
{
    // 4D2BBD..4D2BCB按本次index取层；失败不修改调用方已持有的pair。
    ILayerWindowPatternLayer* const baseLayer = layerWindow_->PatternLayerAt(index);
    auto* const resultLayer = dynamic_cast<IScrResultPatternLayer*>(baseLayer);
    if (!resultLayer) return false;

    // 4D2BD1/4D2BD4先读X/Y，交调用方按4D2C1A/4D2C1F立即写回既有正文。
    destination = {resultLayer->X(), resultLayer->Y()};
    return true;
}
}
