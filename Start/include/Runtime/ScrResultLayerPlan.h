#pragma once

#include <Runtime/ScrLayerTable.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x4CB030 为每个 CPatternLayer 选出的 ResultLayer 投影。
struct ScrResultLayerPlan final
{
    std::size_t patternLayerIndex = 0;
    bool usesDefaultRecord = true;
    std::int32_t patternItemIndex = -1;
    std::int16_t x = 0;
    std::int16_t y = 0;
    std::uint8_t valueAt10 = 0;
    std::int8_t valueAt18 = 0;
    std::vector<std::uint32_t> extrasAt1C;

    bool operator==(const ScrResultLayerPlan&) const = default;
};

// 对应 0x4CB030:4CB439..4CB691：按 valueAt04 为每个 PatternLayer 选择首条记录。
[[nodiscard]] std::vector<ScrResultLayerPlan> BuildScrResultLayerPlans(
    const ScrContainer& container,
    std::int32_t layerGroupIndex,
    std::size_t patternLayerCount);
}
