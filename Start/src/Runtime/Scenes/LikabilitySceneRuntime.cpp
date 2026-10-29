#include <Runtime/LikabilitySceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
LikabilitySceneRuntime::LikabilitySceneRuntime(
    ILikabilitySceneServices& services)
    : services_(&services)
{
    // 49E013..49E064：清零 registry 发生在 dialog 构造之前，失败不恢复。
    savedRegistrySlot2DwordAt04_ = services_->ReadRegistrySlot2DwordAt04();
    services_->WriteRegistrySlot2DwordAt04(0u);
    phase_ = LikabilityScenePhase::BeginDialog;
    dialog_ = services_->CreateLikabilityDialog();
    if (!dialog_)
        throw std::runtime_error("Likability dialog construction returned null");
}

void LikabilitySceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    // 5B581C -> 4B5E20：Draw 是原始空函数。
}

LikabilityScenePhase LikabilitySceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t LikabilitySceneRuntime::SavedRegistrySlot2DwordAt04() const noexcept
{
    return savedRegistrySlot2DwordAt04_;
}

const LikabilityDialogRef& LikabilitySceneRuntime::Dialog() const noexcept
{
    return dialog_;
}

void LikabilitySceneRuntime::Update()
{
    switch (phase_)
    {
    case LikabilityScenePhase::BeginDialog:
        services_->WriteRegistrySlot2DwordAt04(savedRegistrySlot2DwordAt04_);
        dialog_->BeginAtVslot44();
        services_->WriteRegistrySlot2DwordAt04(0u);
        phase_ = LikabilityScenePhase::PollDialog;
        // 4B62E4 直接落入 phase 1，同帧执行首次 poll。
        [[fallthrough]];

    case LikabilityScenePhase::PollDialog:
        if (!dialog_->PollAtVslot48())
            phase_ = LikabilityScenePhase::AdvanceScene;
        return;

    case LikabilityScenePhase::AdvanceScene:
        // 4B629D..4B62A9：必须先推进场景，再恢复 registry。
        services_->AdvanceActiveSceneFromPendingAt4C1760();
        services_->WriteRegistrySlot2DwordAt04(savedRegistrySlot2DwordAt04_);
        return;
    }
}
}
