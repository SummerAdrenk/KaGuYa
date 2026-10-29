#include <Runtime/SettingSceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SettingSceneRuntime::SettingSceneRuntime(ISettingSceneServices& services)
    : services_(&services)
{
    // 4A1A52..4A1B8F：两个值都在清理 registry 之前保存，构造失败不恢复。
    savedRegistrySlot2DwordAt04_ = services_->ReadRegistrySlot2DwordAt04();
    savedRegistrySlot2DwordAt14_ = services_->ReadRegistrySlot2DwordAt14();
    services_->WriteRegistrySlot2DwordAt04(0u);
    services_->WriteRegistrySlot2DwordAt14(
        savedRegistrySlot2DwordAt14_ & 2u);
    phase_ = SettingScenePhase::BeginDialog;
    dialog_ = services_->CreateSettingDialog();
    if (!dialog_)
        throw std::runtime_error("Setting dialog construction returned null");
}

void SettingSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    // 5B5808 -> 4B5E20：Draw 是原始空函数。
}

SettingScenePhase SettingSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t SettingSceneRuntime::SavedRegistrySlot2DwordAt04() const noexcept
{
    return savedRegistrySlot2DwordAt04_;
}

std::uint32_t SettingSceneRuntime::SavedRegistrySlot2DwordAt14() const noexcept
{
    return savedRegistrySlot2DwordAt14_;
}

const SettingDialogRef& SettingSceneRuntime::Dialog() const noexcept
{
    return dialog_;
}

void SettingSceneRuntime::Update()
{
    switch (phase_)
    {
    case SettingScenePhase::BeginDialog:
        services_->WriteRegistrySlot2DwordAt04(savedRegistrySlot2DwordAt04_);
        dialog_->BeginAtVslot44();
        services_->WriteRegistrySlot2DwordAt04(0u);
        phase_ = SettingScenePhase::PollDialog;
        // 4BA8D9：phase 0 直接落入 phase 1，同帧执行首次 poll。
        [[fallthrough]];

    case SettingScenePhase::PollDialog:
        if (!dialog_->PollAtVslot48())
        {
            dialog_->EndAtVslot4C();
            phase_ = SettingScenePhase::AdvanceScene;
        }
        return;

    case SettingScenePhase::AdvanceScene:
        // 4BA81D..4BA87D：必须先推进场景，再按 +04、+14 顺序恢复。
        services_->AdvanceActiveSceneFromPendingAt4C1760();
        services_->WriteRegistrySlot2DwordAt04(savedRegistrySlot2DwordAt04_);
        services_->WriteRegistrySlot2DwordAt14(savedRegistrySlot2DwordAt14_);
        return;
    }
}
}
