#include <Runtime/SaveLoadSceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SaveLoadSceneRuntime::SaveLoadSceneRuntime(
    ISaveLoadSceneServices& services,
    const SaveLoadSceneMode mode)
    : services_(&services), mode_(mode)
{
    // 4A1802..4A1845：dialog 的第二参数严格来自 root+0x1AF。
    const auto rootByteAt1AF = services_->ReadRootByteAt1AF();
    dialog_ = services_->CreateSaveLoadDialog(mode_, rootByteAt1AF);
    if (!dialog_)
        throw std::runtime_error("Save/load dialog construction returned null");

    // 4A18E2..4A19B3：全局字段按原顺序保存和改写，不设置回滚守卫。
    savedRegistrySlot2DwordAt04_ = services_->ReadRegistrySlot2DwordAt04();
    services_->WriteRegistrySlot2DwordAt04(1u);
    savedRootByteAt219_ = services_->ReadRootByteAt219();
    services_->WriteRootByteAt219(0u);
    phase_ = SaveLoadScenePhase::BeginDialog;
}

void SaveLoadSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    // 5B4DAC -> 4B5E20：Draw 是原始空函数。
}

SaveLoadSceneMode SaveLoadSceneRuntime::Mode() const noexcept
{
    return mode_;
}

SaveLoadScenePhase SaveLoadSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t SaveLoadSceneRuntime::SavedRegistrySlot2DwordAt04() const noexcept
{
    return savedRegistrySlot2DwordAt04_;
}

std::uint8_t SaveLoadSceneRuntime::SavedRootByteAt219() const noexcept
{
    return savedRootByteAt219_;
}

const SaveLoadDialogRef& SaveLoadSceneRuntime::Dialog() const noexcept
{
    return dialog_;
}

void SaveLoadSceneRuntime::Update()
{
    switch (phase_)
    {
    case SaveLoadScenePhase::BeginDialog:
        services_->WriteRootSubobjectAt4AD230DwordAt10(0u);
        dialog_->BeginAtVslot44();
        phase_ = SaveLoadScenePhase::PollDialog;
        // 4BA6AF：phase 0 在同一帧直接贯穿第一次 poll。
        [[fallthrough]];

    case SaveLoadScenePhase::PollDialog:
        if (!dialog_->PollAtVslot48())
        {
            dialog_->EndAtVslot4C();
            phase_ = dialog_->CompletionResultAtVslot50() == 0u
                ? SaveLoadScenePhase::ReturnToPreviousScene
                : SaveLoadScenePhase::QueueLoadAfterSelection;
        }
        return;

    case SaveLoadScenePhase::ReturnToPreviousScene:
        services_->AdvanceActiveSceneFromPendingAt4C1760();
        services_->WriteRegistrySlot2DwordAt04(
            savedRegistrySlot2DwordAt04_);
        services_->WriteRootByteAt219(savedRootByteAt219_);
        return;

    case SaveLoadScenePhase::QueueLoadAfterSelection:
        services_->WriteRegistrySlot2DwordAt04(
            savedRegistrySlot2DwordAt04_);
        services_->WriteRootByteAt219(savedRootByteAt219_);
        services_->ClearActiveSceneAt4C1460();
        services_->QueueLoadAfterSelectionSceneAt47F920();
        return;
    }
}
}
