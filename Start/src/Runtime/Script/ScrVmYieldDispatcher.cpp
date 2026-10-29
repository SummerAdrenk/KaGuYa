#include <Runtime/ScrVmYieldDispatcher.h>

#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
ScrVmYieldDispatcher::ScrVmYieldDispatcher(
    IScrVmYieldOwnerState& ownerState,
    IScrVmYieldBoundary& boundary) noexcept
    : ownerState_(ownerState),
      boundary_(boundary)
{
}

bool ScrVmYieldDispatcher::DispatchResult(const std::int32_t result)
{
    switch (result)
    {
    case 2:
        boundary_.CleanupCurrentEnvironment();
        return false;

    case 3:
        ownerState_.SetStateAndClearSubstate(1);
        return true;

    case 4:
        ownerState_.SetStateAndClearSubstate(4);
        return true;

    case 5:
        ownerState_.SetStateAndClearSubstate(3);
        return true;

    case 6:
        // 0x4BFFD0 保证这两次调用的先后顺序。
        boundary_.ResetAt46CE30();
        boundary_.ResetAt4C1760();
        return false;

    case 7:
        ownerState_.SetStateAndClearSubstate(5);
        return true;

    case 8:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code8);
        return false;
    case 9:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code9);
        return false;
    case 10:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code10);
        return false;
    case 11:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code11);
        return false;
    case 12:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code12);
        return false;
    case 13:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code13);
        return false;

    case 14:
        ownerState_.SetStateAndClearSubstate(7);
        return true;

    case 15:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code15);
        return false;
    case 16:
        boundary_.ExecuteFixedAction(ScrVmFixedAction::Code16);
        return false;

    default:
        throw std::runtime_error(
            "ReadScript: unexpected VM result " + std::to_string(result));
    }
}

bool ScrVmYieldDispatcher::DispatchOneStep(const ExecuteVm& executeVm)
{
    boundary_.PrepareCurrentScript();
    return DispatchResult(executeVm());
}
}
