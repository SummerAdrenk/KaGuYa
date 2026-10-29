#pragma once

#include <cstdint>
#include <functional>

namespace KaGuYa::Start::Runtime
{
// 0x4BFFD0 交给 0x4A9030 的固定动作。当前证据只确认代码值，不猜测其业务名称。
enum class ScrVmFixedAction : std::int32_t
{
    Code8 = 8,
    Code9 = 9,
    Code10 = 10,
    Code11 = 11,
    Code12 = 12,
    Code13 = 13,
    Code15 = 15,
    Code16 = 16,
};

class IScrVmYieldOwnerState
{
public:
    virtual ~IScrVmYieldOwnerState() = default;
    virtual void SetStateAndClearSubstate(std::int32_t state) noexcept = 0;
};

// 独立使用 dispatcher 时的状态容器；生产 CReadScript 直接实现同一接口，禁止复制同步。
struct ScrVmYieldOwnerState final : IScrVmYieldOwnerState
{
    ScrVmYieldOwnerState() = default;
    ScrVmYieldOwnerState(const std::int32_t initialState, const std::int32_t initialSubstate)
        : state(initialState), substate(initialSubstate)
    {
    }

    void SetStateAndClearSubstate(const std::int32_t value) noexcept override
    {
        state = value;
        substate = 0;
    }

    std::int32_t state = 0;
    std::int32_t substate = 0;
};

class IScrVmYieldBoundary
{
public:
    virtual ~IScrVmYieldBoundary() = default;

    // 仅旧受控DispatchOneStep调用此准备叶；原4BFFD0没有额外pre-step。
    virtual void PrepareCurrentScript() = 0;

    // VM result 2 对应的 current-environment cleanup 调用。
    virtual void CleanupCurrentEnvironment() = 0;

    // VM result 6 依次调用 0x46CE30 与 0x4C1760；两个边界不可合并或交换。
    virtual void ResetAt46CE30() = 0;
    virtual void ResetAt4C1760() = 0;

    // VM result 8..13、15、16 通过 0x4A9030 消费各自不同的固定 payload。
    virtual void ExecuteFixedAction(ScrVmFixedAction action) = 0;
};

// 严格建模 Start.exe:0x4BFFD0 的 VM 外层 yield 分派。
class ScrVmYieldDispatcher final
{
public:
    using ExecuteVm = std::function<std::int32_t()>;

    ScrVmYieldDispatcher(
        IScrVmYieldOwnerState& ownerState,
        IScrVmYieldBoundary& boundary) noexcept;

    // 调用者已经取得 VM result 时使用。返回值与 0x4BFFD0 的返回值一致。
    [[nodiscard]] bool DispatchResult(std::int32_t result);

    // 旧受控宿主的显式准备 -> 执行 -> 分派；原生BCh只调用DispatchResult。
    [[nodiscard]] bool DispatchOneStep(const ExecuteVm& executeVm);

private:
    IScrVmYieldOwnerState& ownerState_;
    IScrVmYieldBoundary& boundary_;
};
}
