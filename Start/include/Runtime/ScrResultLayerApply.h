#pragma once

#include <Runtime/ScrResultLayerPlan.h>

#include <Windows.h>

#include <cstdint>
#include <span>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
// CPatternLayer 在 0x4CB030 已确认应用阶段所暴露的行为边界。
class IScrResultPatternLayer
{
public:
    virtual ~IScrResultPatternLayer() = default;

    [[nodiscard]] virtual std::int32_t CurrentPatternItemIndex() const noexcept = 0;
    [[nodiscard]] virtual bool DrawingEnabled() const noexcept = 0;
    [[nodiscard]] virtual RECT ComputePatternUnionRect() const = 0;

    // sub_442700(layer, -1)：清空所有 CSprite、资源引用、过滤参数及附加状态。
    virtual void ClearAllPatternItems() = 0;
    virtual void SetCurrentPatternItemIndex(std::int32_t index) noexcept = 0;

    [[nodiscard]] virtual std::int32_t FilterMode() const noexcept = 0;
    [[nodiscard]] virtual std::span<const std::uint32_t> FilterArguments() const noexcept = 0;
    virtual void ResetFilterBeforePatternLoad() = 0;
    virtual void SetFilterParameters(
        std::int32_t mode,
        std::span<const std::uint32_t> arguments) = 0;
    virtual void MarkFilterRefreshPending() noexcept = 0;
    virtual void ApplyFilterCommand() = 0;

    [[nodiscard]] virtual std::int32_t X() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t Y() const noexcept = 0;
    virtual void SetPosition(std::int32_t x, std::int32_t y) noexcept = 0;

    virtual void SetResultValueAt10(std::uint8_t value) noexcept = 0;
    virtual void SetHasPatternItem(bool hasPatternItem) noexcept = 0;
    virtual void Invalidate(const RECT& rect) = 0;
};

class IScrPatternItemLoader
{
public:
    virtual ~IScrPatternItemLoader() = default;

    // 对应 0x4CB2E6..0x4CCE73。资源分支必须完整成功后才能提交 current id。
    virtual void LoadPatternItem(
        IScrResultPatternLayer& layer,
        std::int32_t patternItemIndex) = 0;
};

// 固定全局第 4 个管理槽中，0x4CCD95..0x4CCDE6 实际访问的记录字段。
struct PatternUseRecord final
{
    std::int32_t patternItemIndex = -1;
    std::uint8_t markerAt04 = 0;
};

class IPatternUseRegistry
{
public:
    virtual ~IPatternUseRegistry() = default;
    [[nodiscard]] virtual std::span<PatternUseRecord> PrimaryUseRecords() = 0;
};

enum class PatternUseMarkResult : std::uint8_t
{
    NotApplicable,
    NotFound,
    Marked,
};

[[nodiscard]] PatternUseMarkResult MarkPrimaryPatternUse(
    std::size_t patternLayerIndex,
    std::int32_t targetPatternItemIndex,
    IPatternUseRegistry& registry);

// 对应 0x4CB5C9..0x4CCFCB 中已确认的状态比较、提交及失效顺序。
void ApplyScrResultLayerPlan(
    IScrResultPatternLayer& layer,
    const ScrResultLayerPlan& plan,
    IScrPatternItemLoader& patternItemLoader,
    IPatternUseRegistry& patternUseRegistry);
}
