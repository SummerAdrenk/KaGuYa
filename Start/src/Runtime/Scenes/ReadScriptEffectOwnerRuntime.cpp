#include <Runtime/ReadScriptEffectOwnerRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ReadScriptEffectOwnerRuntime::~ReadScriptEffectOwnerRuntime()
{
    effect_.reset();
    source_.reset();
    previous_.reset();
    auxiliary_.reset();
}

bool ReadScriptEffectOwnerRuntime::HasEffectAt1C() const { return effect_ != nullptr; }

IReadScriptEffectRuntime& ReadScriptEffectOwnerRuntime::RequireEffect() const
{
    if (!effect_ || finishAttempted_)
        throw std::logic_error("ReadScript effect is absent or already finalized");
    return *effect_;
}

bool ReadScriptEffectOwnerRuntime::TickEffectAtVslot00() { return RequireEffect().TickAtVslot00(); }
void ReadScriptEffectOwnerRuntime::DrawEffectAtVslot04() { RequireEffect().DrawAtVslot04(); }
std::uint8_t ReadScriptEffectOwnerRuntime::ReadyEffectAtVslot08() const
{
    return RequireEffect().ReadyAtVslot08();
}

void ReadScriptEffectOwnerRuntime::FinishAndReleaseEffect()
{
    if (!effect_) return;
    // 原先减引用计数后才调用删除链，Finish 抛出时不可再次执行相同最后一帧。
    if (!finishAttempted_)
    {
        finishAttempted_ = true;
        effect_->Finish();
    }
    effect_.reset();
    finishAttempted_ = false;
}

void ReadScriptEffectOwnerRuntime::InstallEffect(std::unique_ptr<IReadScriptEffectRuntime> newEffect)
{
    FinishAndReleaseEffect();
    effect_ = std::move(newEffect);
}

void ReadScriptEffectOwnerRuntime::ReleaseResourcesAt4C0990()
{
    FinishAndReleaseEffect();
    // 4C0990 为正序，不是 C++ 成员的逆序；异常不会先行清空后面的资源槽。
    source_.reset();
    previous_.reset();
    auxiliary_.reset();
}

std::shared_ptr<RuntimeGraphicsSurface>& ReadScriptEffectOwnerRuntime::SourceAt28() noexcept { return source_; }
std::shared_ptr<RuntimeGraphicsSurface>& ReadScriptEffectOwnerRuntime::PreviousAt34() noexcept { return previous_; }
std::shared_ptr<RuntimeGraphicsSurface>& ReadScriptEffectOwnerRuntime::AuxiliaryAt40() noexcept { return auxiliary_; }
}
