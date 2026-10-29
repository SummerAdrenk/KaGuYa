#pragma once

#include <Runtime/ProgramFixedActionRuntime.h>
#include <Runtime/ScrVmYieldDispatcher.h>

namespace KaGuYa::Start::Runtime
{
class IScrVmYieldScriptPreparer
{
public:
    virtual ~IScrVmYieldScriptPreparer() = default;
    virtual void PrepareCurrentScript() = 0;
};

class IScrVmYieldEnvironmentCleanup
{
public:
    virtual ~IScrVmYieldEnvironmentCleanup() = default;
    virtual void CleanupCurrentEnvironment() = 0;
};

class IScrVmYieldResetCoordinator
{
public:
    virtual ~IScrVmYieldResetCoordinator() = default;
    virtual void ResetAt46CE30() = 0;
    virtual void ResetAt4C1760() = 0;
};

// 把 0x4BFFD0 的宿主副作用接到已恢复的固定动作生产链。
class ScrVmYieldRuntimeServices final : public IScrVmYieldBoundary
{
public:
    ScrVmYieldRuntimeServices(
        IScrVmYieldScriptPreparer& scriptPreparer,
        IScrVmYieldEnvironmentCleanup& environmentCleanup,
        IScrVmYieldResetCoordinator& resetCoordinator,
        ProgramFixedActionRuntime& fixedActions) noexcept;

    void PrepareCurrentScript() override;
    void CleanupCurrentEnvironment() override;
    void ResetAt46CE30() override;
    void ResetAt4C1760() override;
    void ExecuteFixedAction(ScrVmFixedAction action) override;

private:
    IScrVmYieldScriptPreparer* scriptPreparer_ = nullptr;
    IScrVmYieldEnvironmentCleanup* environmentCleanup_ = nullptr;
    IScrVmYieldResetCoordinator* resetCoordinator_ = nullptr;
    ProgramFixedActionRuntime* fixedActions_ = nullptr;
};
}
