#include <Runtime/ScrResultLayerApply.h>

#include <algorithm>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] bool FilterArgumentsEqual(
    const std::span<const std::uint32_t> first,
    const std::span<const std::uint32_t> second) noexcept
{
    return first.size() == second.size() &&
           std::equal(first.begin(), first.end(), second.begin());
}
}

void ApplyScrResultLayerPlan(
    IScrResultPatternLayer& layer,
    const ScrResultLayerPlan& plan,
    IScrPatternItemLoader& patternItemLoader,
    IPatternUseRegistry& patternUseRegistry)
{
    RECT oldRect{};
    if (layer.DrawingEnabled())
        oldRect = layer.ComputePatternUnionRect();

    std::int32_t currentPatternItemIndex = layer.CurrentPatternItemIndex();
    if (currentPatternItemIndex != plan.patternItemIndex)
    {
        if (plan.patternItemIndex < 0)
        {
            layer.ClearAllPatternItems();
            layer.Invalidate(oldRect);
        }
        else
        {
            // 0x4CB6ED..0x4CB701 仅在旧 filter mode 非零时执行重置。
            if (layer.FilterMode() != 0)
                layer.ResetFilterBeforePatternLoad();
            patternItemLoader.LoadPatternItem(layer, plan.patternItemIndex);
            (void)MarkPrimaryPatternUse(plan.patternLayerIndex,
                plan.patternItemIndex, patternUseRegistry);
        }

        layer.SetCurrentPatternItemIndex(plan.patternItemIndex);
        currentPatternItemIndex = plan.patternItemIndex;
    }

    if (currentPatternItemIndex >= 0)
    {
        const std::int32_t targetFilterMode =
            static_cast<std::int32_t>(plan.valueAt18);
        if (layer.FilterMode() != targetFilterMode ||
            !FilterArgumentsEqual(layer.FilterArguments(), plan.extrasAt1C))
        {
            layer.SetFilterParameters(targetFilterMode, plan.extrasAt1C);
            layer.MarkFilterRefreshPending();
            layer.ApplyFilterCommand();
            layer.Invalidate(layer.ComputePatternUnionRect());
        }

        if (layer.X() != plan.x || layer.Y() != plan.y)
        {
            layer.Invalidate(oldRect);
            layer.SetPosition(plan.x, plan.y);
            layer.Invalidate(layer.ComputePatternUnionRect());
        }
    }

    layer.SetResultValueAt10(plan.valueAt10);
    layer.SetHasPatternItem(plan.patternItemIndex != -1);
}

PatternUseMarkResult MarkPrimaryPatternUse(
    const std::size_t patternLayerIndex,
    const std::int32_t targetPatternItemIndex,
    IPatternUseRegistry& registry)
{
    if (patternLayerIndex != 0u || targetPatternItemIndex < 0)
        return PatternUseMarkResult::NotApplicable;

    for (PatternUseRecord& record : registry.PrimaryUseRecords())
    {
        if (record.patternItemIndex != targetPatternItemIndex)
            continue;
        record.markerAt04 = 1u;
        return PatternUseMarkResult::Marked;
    }
    return PatternUseMarkResult::NotFound;
}
}
