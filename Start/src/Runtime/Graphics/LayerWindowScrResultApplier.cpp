#include <Runtime/LayerWindowScrResultApplier.h>

#include <Runtime/PatternResourceExecutor.h>
#include <Runtime/ScrResultLayerPlan.h>

#include <vector>

namespace KaGuYa::Start::Runtime
{
LayerWindowScrResultException::LayerWindowScrResultException(
    const LayerWindowScrResultError error,
    const std::size_t patternLayerIndex,
    const char* const message)
    : std::runtime_error(message),
      error_(error),
      patternLayerIndex_(patternLayerIndex)
{
}

LayerWindowScrResultError LayerWindowScrResultException::Error() const noexcept
{
    return error_;
}

std::size_t LayerWindowScrResultException::PatternLayerIndex() const noexcept
{
    return patternLayerIndex_;
}

LayerWindowScrResultApplier::LayerWindowScrResultApplier(
    LayerWindowRuntime& layerWindow,
    const IParamsPatternSource& pattern,
    IPatternUseRegistry& patternUseRegistry) noexcept
    : layerWindow_(&layerWindow),
      pattern_(&pattern),
      patternUseRegistry_(&patternUseRegistry)
{
}

void LayerWindowScrResultApplier::Apply(
    const ScrContainer& container,
    const std::int32_t layerGroupIndex)
{
    const std::vector<ScrResultLayerPlan> plans = BuildScrResultLayerPlans(
        container, layerGroupIndex, layerWindow_->PatternLayerCount());

    for (const ScrResultLayerPlan& plan : plans)
    {
        ILayerWindowPatternLayer* const baseLayer =
            layerWindow_->PatternLayerAt(plan.patternLayerIndex);
        if (!baseLayer)
        {
            throw LayerWindowScrResultException(
                LayerWindowScrResultError::MissingPatternLayer,
                plan.patternLayerIndex,
                "LayerWindow PatternLayer reference is empty");
        }

        auto* const resultLayer =
            dynamic_cast<IScrResultPatternLayer*>(baseLayer);
        auto* const executionTarget =
            dynamic_cast<IPatternResourceExecutionTarget*>(baseLayer);
        if (!resultLayer || !executionTarget ||
            dynamic_cast<void*>(resultLayer) != dynamic_cast<void*>(executionTarget))
        {
            throw LayerWindowScrResultException(
                LayerWindowScrResultError::IncompatiblePatternLayer,
                plan.patternLayerIndex,
                "LayerWindow PatternLayer does not expose the production result/resource interfaces");
        }

        ScrPatternResourceItemLoader loader(
            *pattern_, *resultLayer, *executionTarget);
        ApplyScrResultLayerPlan(
            *resultLayer, plan, loader, *patternUseRegistry_);
    }
}
}
