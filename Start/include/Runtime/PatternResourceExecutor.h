#pragma once

#include <Runtime/PatternResourcePlan.h>
#include <Runtime/PatternResourceLoadTarget.h>
#include <Runtime/ParamsPatternSource.h>
#include <Runtime/ScrResultLayerApply.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class PatternSpriteRectState : std::uint8_t
{
    Valid,
    Empty,
    InvalidLoadMode,
};

struct PatternSpriteRectResult final
{
    PatternSpriteRectState state = PatternSpriteRectState::Empty;
    RECT rect{};
};

enum class PatternResourceExecutionError : std::uint8_t
{
    InvalidSpriteState,
    MissingLogicalResource,
};

class PatternResourceExecutionException final : public std::runtime_error
{
public:
    PatternResourceExecutionException(
        PatternResourceExecutionError error,
        std::size_t spriteSlotIndex,
        std::string resourceNameCp932);

    [[nodiscard]] PatternResourceExecutionError Error() const noexcept;
    [[nodiscard]] std::size_t SpriteSlotIndex() const noexcept;
    [[nodiscard]] const std::string& ResourceNameCp932() const noexcept;

private:
    PatternResourceExecutionError error_;
    std::size_t spriteSlotIndex_ = 0;
    std::string resourceNameCp932_;
};

// 0x4CB030 资源循环实际依赖的 CPatternLayer/CSprite 行为边界。
class IPatternResourceExecutionTarget
{
public:
    virtual ~IPatternResourceExecutionTarget() = default;

    [[nodiscard]] virtual std::size_t SpriteSlotCount() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t LayerX() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t LayerY() const noexcept = 0;
    // 4CB892与4CB9EC分别只借字符和读取长度，不复制、不增持、不合并两次读取。
    [[nodiscard]] virtual const char* LogicalNameCharacters(
        std::size_t spriteSlotIndex) const = 0;
    [[nodiscard]] virtual std::int32_t LogicalNameLength(std::size_t spriteSlotIndex) const = 0;
    [[nodiscard]] virtual PatternSpriteRectResult LocalDrawableRect(
        std::size_t spriteSlotIndex) const = 0;

    virtual void ClearSlot(std::size_t spriteSlotIndex) = 0;
    [[nodiscard]] virtual PatternSpriteReceiver CaptureSpriteReceiver(std::uint32_t slotIndex) const = 0;
    virtual void LoadResource(std::uint32_t slotIndex, PatternSpriteReceiver capturedReceiver,
        const PatternResourceLoadRequest& request, const IPatternResourceFailureName& failureName,
        IPatternResourceLoadCommit& commit, IRuntimeStringManagerSource& strings) = 0;

    [[nodiscard]] virtual const char* ReceiverPrimaryNameCharacters(PatternSpriteReceiver receiver) const = 0;
    // 只返回固定receiver内的容器头借用；CaptureRange由执行器在原读取区间调用。
    [[nodiscard]] virtual PatternResourceNamesBorrow ReceiverAuxiliaryNames(
        PatternSpriteReceiver receiver) const = 0;
    virtual void PrepareReceiverFileGroup(PatternSpriteReceiver receiver, FileGroupChange changes) = 0;
    virtual void SetReceiverLogicalName(PatternSpriteReceiver receiver, const RuntimeCString& source) = 0;
    virtual void SetReceiverOrigin(PatternSpriteReceiver receiver, std::int32_t x, std::int32_t y) = 0;
    virtual void SetReceiverActive(PatternSpriteReceiver receiver, bool active) = 0;
    [[nodiscard]] virtual PatternSpriteRectResult ReceiverDrawableRect(PatternSpriteReceiver receiver) const = 0;
    virtual void Invalidate(const RECT& rect) = 0;
};

// 对应 0x4CB8A5..0x4CCE73。异常时保留已经发生的清理和成员装载，不执行事务回滚。
// 各原取根点分别借用当前 Pattern，不在入口固定正文或增加 Ref 持有。
void ExecutePatternResourcePlan(
    const IParamsPatternSource& source,
    IPatternResourceExecutionTarget& target,
    const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 4CB805..4CC252：与工具口共核，原Ref坏状态在原字段读点访问，不翻译为空计划。
void ExecutePatternResourcePlanNative(
    const IParamsPatternSource& source, IPatternResourceExecutionTarget& target,
    const PatternResourcePlan& plan,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 将 0x4AD900 的计划生成与 0x4CB030 的槽位执行接入 ResultLayer 应用阶段。
class ScrPatternResourceItemLoader final : public IScrPatternItemLoader
{
public:
    ScrPatternResourceItemLoader(
        const IParamsPatternSource& source,
        IScrResultPatternLayer& resultLayer,
        IPatternResourceExecutionTarget& executionTarget) noexcept;

    void LoadPatternItem(
        IScrResultPatternLayer& layer,
        std::int32_t patternItemIndex) override;

private:
    const IParamsPatternSource& source_;
    IScrResultPatternLayer& resultLayer_;
    IPatternResourceExecutionTarget& executionTarget_;
};
}
