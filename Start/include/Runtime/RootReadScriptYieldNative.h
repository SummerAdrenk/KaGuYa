#pragma once
#include <Runtime/ScrVmYieldDispatcher.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootScriptRecoveryRuntime;
// 原4BFFD0的原生Root副作用；不借shared_ptr固定动作队列，也不缓存Manager或CParams。
class RootReadScriptYieldNative final:public IScrVmYieldBoundary
{
public:
    RootReadScriptYieldNative(RootRuntimeOwner&,RootScriptRecoveryRuntime&,
        IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource())noexcept;
    void PrepareCurrentScript()override;
    void CleanupCurrentEnvironment()override;
    void ResetAt46CE30()override;
    void ResetAt4C1760()override;
    void ExecuteFixedAction(ScrVmFixedAction)override;
private:
    RootRuntimeOwner& root_;
    RootScriptRecoveryRuntime& recovery_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
};
}
