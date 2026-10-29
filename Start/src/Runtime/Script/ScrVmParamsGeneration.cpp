#include <Runtime/ScrVmParamsGeneration.h>

namespace KaGuYa::Start::Runtime
{
ScrVmParamsGenerationSlot::ScrVmParamsGenerationSlot()
    : ScrVmParamsGenerationSlot(DefaultEnvironmentRootMemory(), DefaultRuntimeStringManagerSource()) {}

ScrVmParamsGenerationSlot::ScrVmParamsGenerationSlot(IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
    : ScrVmParamsGenerationSlot(DeferredTag{}, memory, strings)
{
    Replace();
}

ScrVmParamsGenerationSlot::ScrVmParamsGenerationSlot(DeferredTag,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : memory_(memory), strings_(strings) {}

ScrVmParamsGenerationSlot::ScrVmParamsGenerationSlot(RootManagedTag,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : memory_(memory), strings_(strings), automaticRelease_(false) {}

ScrVmParamsGenerationSlot::~ScrVmParamsGenerationSlot() noexcept
{ if (automaticRelease_) Release(); }

const ScrVmParamsGeneration* ScrVmParamsGenerationSlot::Published() const noexcept
{
    // 与47BE20返回路径相同，不能只把base强转而忽略当前control/stride/bias。
    return ResolveEnvironmentObjectReference<ScriptParamsNativeBody>(reference_);
}

ScrVmParamsGeneration& ScrVmParamsGenerationSlot::Current()
{
    return *GetParamsAt47BDB0(reference_, memory_, strings_);
}

const ScrVmParamsGeneration& ScrVmParamsGenerationSlot::Current() const
{
    return *GetParamsAt47BDB0(reference_, memory_, strings_);
}

void ScrVmParamsGenerationSlot::Replace()
{
    ReplaceParamsAt481720(reference_, memory_, strings_);
}

void ScrVmParamsGenerationSlot::Release() { ReleaseYtlReference(reference_); }

std::shared_ptr<ScrVmParamsGenerationSlot> CreateScrVmParamsGenerationSlot(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    return std::make_shared<ScrVmParamsGenerationSlot>(memory, strings);
}
}
