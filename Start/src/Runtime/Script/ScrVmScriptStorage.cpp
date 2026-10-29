#include <Runtime/ScrVmScriptStorage.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T& Require(const YtlReferenceSlotRuntime& slot)
{
    auto* body = ResolveEnvironmentObjectReference<T>(slot);
    if (!body) throw std::logic_error("CScript body is null (modern safety boundary)");
    return *body;
}

std::int32_t& Field(ScrVmWaitState& body, unsigned index)
{
    switch (index)
    {
    case 0: return body.valueAt00;
    case 1: return body.valueAt04;
    case 2: return body.valueAt08;
    default: throw std::out_of_range("CScript wait field index");
    }
}
}

ScrVmScriptStorage::ScrVmScriptStorage(IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings) noexcept
    : memory_(memory), strings_(strings), effect_(fixtureEffect_), wait_(fixtureWait_),
      publication_(fixturePublication_) {}
ScrVmScriptStorage::ScrVmScriptStorage(ScriptNativeBody& body, ScriptNativePublication& publication,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : memory_(memory), strings_(strings), body_(&body), effect_(body.effectAt0C), wait_(body.waitAt18),
      publication_(publication) {}
ScrVmScriptStorage::~ScrVmScriptStorage() noexcept { if (!body_) Release(); }

void ScrVmScriptStorage::CreateEffectAt4C8ECC()
    { CreateScriptEffectAt4C8ECC(effect_, strings_, memory_); }
void ScrVmScriptStorage::CreatePairsAt4C9025(std::int32_t count)
    { CreateScriptEffectPairsAt4C9025(EffectAt4AD140(), count, memory_); }
void ScrVmScriptStorage::CreateWaitAt4C917D()
{
    // 分配叶可在原发布前后回调；同步在异常路径也执行，不清掉仍存活旧正文的标志。
    try { CreateScriptWaitAt4C917D(wait_, memory_); }
    catch (...) { SynchronizeWaitPublication(); throw; }
    SynchronizeWaitPublication();
}

void ScrVmScriptStorage::InitializeAt4C8CF0(ParamsRuntimeRootOwner& params)
{
    CreateEffectAt4C8ECC();
    try { InitializeScriptEffectAndWaitAt4C8FA1(effect_, wait_, params, memory_); }
    catch (...) { SynchronizeWaitPublication(); throw; }
    SynchronizeWaitPublication();
}

ScrVmUpdateEffectState& ScrVmScriptStorage::EffectAt4AD140() { return Require<ScrVmUpdateEffectState>(effect_); }
const ScrVmUpdateEffectState& ScrVmScriptStorage::EffectAt4AD140() const { return Require<ScrVmUpdateEffectState>(effect_); }
ScrVmWaitState& ScrVmScriptStorage::WaitState() { return Require<ScrVmWaitState>(wait_); }
const ScrVmWaitState& ScrVmScriptStorage::WaitState() const { return Require<ScrVmWaitState>(wait_); }

void ScrVmScriptStorage::SynchronizeWaitPublication() const noexcept
{
    const auto* current = ResolveEnvironmentObjectReference<ScrVmWaitState>(wait_);
    if (current != publication_.waitBody) { publication_.waitBody = current; publication_.waitMask = 0; }
}

void ScrVmScriptStorage::WriteWaitValue(ScrVmWaitState& target, unsigned index, std::int32_t value)
{
    Field(target, index) = value;
    SynchronizeWaitPublication();
    if (&target == publication_.waitBody) publication_.waitMask |= 1u << index;
}

void ScrVmScriptStorage::SetWaitState(ScrVmWaitState value)
{
    auto& target = WaitState();
    WriteWaitValue(target, 0, value.valueAt00);
    WriteWaitValue(target, 1, value.valueAt04);
    WriteWaitValue(target, 2, value.valueAt08);
}

bool ScrVmScriptStorage::HasWaitValue(unsigned index) const
{
    if (index >= 3) throw std::out_of_range("CScript wait field index");
    SynchronizeWaitPublication();
    return (publication_.waitMask & (1u << index)) != 0;
}

std::int32_t ScrVmScriptStorage::ReadWaitValue(unsigned index) const
{
    const auto& target = WaitState();
    if (!HasWaitValue(index)) throw std::bad_optional_access{};
    return Field(const_cast<ScrVmWaitState&>(target), index);
}

void ScrVmScriptStorage::Release()
{
    // 4CADD0：wait先于effect；无control正文保持原样，不以现代owner补回收。
    ReleaseYtlReference(wait_);
    SynchronizeWaitPublication();
    ReleaseYtlReference(effect_);
}
}
