#include <Runtime/ScrResultLayerPlan.h>

#include <bit>
#include <optional>
#include <utility>

namespace KaGuYa::Start::Runtime
{
std::vector<ScrResultLayerPlan> BuildScrResultLayerPlans(
    const ScrContainer& container,
    const std::int32_t layerGroupIndex,
    const std::size_t patternLayerCount)
{
    std::vector<ScrLayerRecord> records;
    for (std::int32_t entryIndex = 0;; ++entryIndex)
    {
        std::optional<ScrLayerRecord> record =
            FindScrLayerRecord(container, layerGroupIndex, entryIndex);
        if (!record)
            break;
        records.push_back(std::move(*record));
    }

    std::vector<ScrResultLayerPlan> plans;
    plans.reserve(patternLayerCount);
    for (std::size_t patternLayerIndex = 0;
         patternLayerIndex < patternLayerCount;
         ++patternLayerIndex)
    {
        ScrResultLayerPlan plan;
        plan.patternLayerIndex = patternLayerIndex;

        for (const ScrLayerRecord& record : records)
        {
            if (patternLayerIndex != record.valueAt04)
                continue;

            plan.usesDefaultRecord = false;
            plan.patternItemIndex = std::bit_cast<std::int32_t>(record.valueAt00);
            plan.x = record.valueAt08;
            plan.y = record.valueAt0C;
            plan.valueAt10 = record.valueAt10;
            plan.valueAt18 = record.valueAt18;
            plan.extrasAt1C = record.extrasAt1C;
            break;
        }
        plans.push_back(std::move(plan));
    }
    return plans;
}
}
