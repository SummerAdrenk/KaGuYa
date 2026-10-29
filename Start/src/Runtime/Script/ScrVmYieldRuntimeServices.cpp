#include <Runtime/ScrVmYieldRuntimeServices.h>

namespace KaGuYa::Start::Runtime
{
ScrVmYieldRuntimeServices::ScrVmYieldRuntimeServices(
    IScrVmYieldScriptPreparer& scriptPreparer,
    IScrVmYieldEnvironmentCleanup& environmentCleanup,
    IScrVmYieldResetCoordinator& resetCoordinator,
    ProgramFixedActionRuntime& fixedActions) noexcept
    : scriptPreparer_(&scriptPreparer),
      environmentCleanup_(&environmentCleanup),
      resetCoordinator_(&resetCoordinator),
      fixedActions_(&fixedActions)
{
}

void ScrVmYieldRuntimeServices::PrepareCurrentScript()
{
    scriptPreparer_->PrepareCurrentScript();
}

void ScrVmYieldRuntimeServices::CleanupCurrentEnvironment()
{
    environmentCleanup_->CleanupCurrentEnvironment();
}

void ScrVmYieldRuntimeServices::ResetAt46CE30()
{
    resetCoordinator_->ResetAt46CE30();
}

void ScrVmYieldRuntimeServices::ResetAt4C1760()
{
    resetCoordinator_->ResetAt4C1760();
}

void ScrVmYieldRuntimeServices::ExecuteFixedAction(
    const ScrVmFixedAction action)
{
    fixedActions_->Execute(action);
}
}
