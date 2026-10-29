#pragma once

#include <Runtime/ScrVmParamsGeneration.h>

namespace KaGuYa::Start::Runtime
{
struct ScrVmRuntimeStateBundle;

// 46CE30原地复位；不重新取根，也不替换CParams正文。
class ScrParamsResetRuntime final
{
public:
    explicit ScrParamsResetRuntime(ScrVmRuntimeStateBundle& bundle);
    ScrParamsResetRuntime(ScrVmRuntimeStateBundle& bundle, IEnvironmentRootMemory& memory,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    void ResetAt46CE30();
    static void ResetCapturedAt46CE30(ScrVmParamsGeneration& current,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

private:
    void CheckBundle() const;
    ScrVmRuntimeStateBundle& bundle_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
};
}
