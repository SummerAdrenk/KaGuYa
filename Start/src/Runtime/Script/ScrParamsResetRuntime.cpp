#include <Runtime/ScrParamsResetRuntime.h>
#include <Runtime/ScrVmRuntimeComposition.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
ScrVmParamsGenerationSlot& RequireSlot(ScrVmRuntimeStateBundle& bundle)
{
    if (!bundle.paramsGeneration)
        throw std::invalid_argument("46CE30 params generation is null");
    return *bundle.paramsGeneration;
}
}

ScrParamsResetRuntime::ScrParamsResetRuntime(ScrVmRuntimeStateBundle& bundle)
    : ScrParamsResetRuntime(bundle, RequireSlot(bundle).Memory(), RequireSlot(bundle).Strings()) {}

ScrParamsResetRuntime::ScrParamsResetRuntime(ScrVmRuntimeStateBundle& bundle,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
    : bundle_(bundle), memory_(memory), strings_(strings)
{
    CheckBundle();
}

void ScrParamsResetRuntime::CheckBundle() const
{
    if (!bundle_.paramsGeneration)
        throw std::invalid_argument("46CE30 params generation is null");
    // 只比较稳定slot身份，装配不触发47BDB0或提前捕获正文。
    if (!bundle_.values.UsesParamsGenerationSlot(bundle_.paramsGeneration))
        throw std::invalid_argument("46CE30 bundle value table belongs to another generation slot");
    if (!bundle_.fileCallStack.UsesParamsGenerationSlot(bundle_.paramsGeneration))
        throw std::invalid_argument("46CE30 bundle file call stack belongs to another generation slot");
    if (!bundle_.localCallStack.UsesParamsGenerationSlot(bundle_.paramsGeneration))
        throw std::invalid_argument("46CE30 bundle local call stack belongs to another generation slot");
    if (!bundle_.menu.UsesParamsGenerationSlot(bundle_.paramsGeneration))
        throw std::invalid_argument("46CE30 bundle menu belongs to another generation slot");
}

void ScrParamsResetRuntime::ResetAt46CE30()
{
    CheckBundle();
    auto& current = bundle_.paramsGeneration->Current();
    ResetCapturedAt46CE30(current, memory_, strings_);
}

void ScrParamsResetRuntime::ResetCapturedAt46CE30(ScrVmParamsGeneration& current,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    current.ResetAt46CE30(memory, strings);
}
}
