#pragma once

#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/ParamsPatternSource.h>
#include <Runtime/ScrResultLayerApply.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
enum class LayerWindowScrResultError : std::uint8_t
{
    MissingPatternLayer,
    IncompatiblePatternLayer,
};

class LayerWindowScrResultException final : public std::runtime_error
{
public:
    LayerWindowScrResultException(
        LayerWindowScrResultError error,
        std::size_t patternLayerIndex,
        const char* message);

    [[nodiscard]] LayerWindowScrResultError Error() const noexcept;
    [[nodiscard]] std::size_t PatternLayerIndex() const noexcept;

private:
    LayerWindowScrResultError error_;
    std::size_t patternLayerIndex_ = 0;
};

// 对应 4CB030 的六层外循环。每层使用同一个生产对象承载 ResultLayer 状态和资源副作用。
class LayerWindowScrResultApplier final
{
public:
    LayerWindowScrResultApplier(
        LayerWindowRuntime& layerWindow,
        const IParamsPatternSource& pattern,
        IPatternUseRegistry& patternUseRegistry) noexcept;

    void Apply(const ScrContainer& container, std::int32_t layerGroupIndex);

private:
    LayerWindowRuntime* layerWindow_ = nullptr;
    const IParamsPatternSource* pattern_ = nullptr;
    IPatternUseRegistry* patternUseRegistry_ = nullptr;
};
}
