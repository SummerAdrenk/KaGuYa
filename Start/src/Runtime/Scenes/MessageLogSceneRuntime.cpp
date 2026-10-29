#include <Runtime/MessageLogSceneRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
MessageLogSceneRuntime::MessageLogSceneRuntime(
    IMessageLogSceneServices& services)
    : services_(&services)
{
    // 49E1D2..49E275：先保存并清零 registry，再建立 0xDC dialog。
    savedRegistrySlot2DwordAt04_ = services_->ReadRegistrySlot2DwordAt04();
    services_->WriteRegistrySlot2DwordAt04(0u);
    phase_ = MessageLogScenePhase::BeginDialog;
    dialog_ = services_->CreateMessageLogDialog();

    // 49E336..49E38D：两个 root byte 在 dialog 构造之后依次保存、清零。
    savedRootByteAt218_ = services_->ReadRootByteAt218();
    savedRootByteAt219_ = services_->ReadRootByteAt219();
    services_->WriteRootByteAt218(0u);
    services_->WriteRootByteAt219(0u);

    if (!dialog_)
        throw std::runtime_error("Message log dialog construction returned null");
}

void MessageLogSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    // 5B4E28 -> 4B5E20：Draw 是原始空函数。
}

MessageLogScenePhase MessageLogSceneRuntime::Phase() const noexcept
{
    return phase_;
}

std::uint32_t MessageLogSceneRuntime::SavedRegistrySlot2DwordAt04() const noexcept
{
    return savedRegistrySlot2DwordAt04_;
}

std::uint8_t MessageLogSceneRuntime::SavedRootByteAt218() const noexcept
{
    return savedRootByteAt218_;
}

std::uint8_t MessageLogSceneRuntime::SavedRootByteAt219() const noexcept
{
    return savedRootByteAt219_;
}

const MessageLogDialogRef& MessageLogSceneRuntime::Dialog() const noexcept
{
    return dialog_;
}

void MessageLogSceneRuntime::Update()
{
    switch (phase_)
    {
    case MessageLogScenePhase::BeginDialog:
        services_->WriteRegistrySlot2DwordAt04(
            savedRegistrySlot2DwordAt04_);
        dialog_->BeginAtVslot44();
        services_->WriteRegistrySlot2DwordAt04(0u);
        phase_ = MessageLogScenePhase::PollDialog;
        // 4B69F7：phase 0 在同一帧直接贯穿第一次 poll。
        [[fallthrough]];

    case MessageLogScenePhase::PollDialog:
        if (!dialog_->PollAtVslot48())
        {
            dialog_->EndAtVslot4C();
            phase_ = MessageLogScenePhase::ReturnToPreviousScene;
        }
        return;

    case MessageLogScenePhase::ReturnToPreviousScene:
        services_->AdvanceActiveSceneFromPendingAt4C1760();
        services_->WriteRegistrySlot2DwordAt04(
            savedRegistrySlot2DwordAt04_);
        services_->WriteRootByteAt218(savedRootByteAt218_);
        services_->WriteRootByteAt219(savedRootByteAt219_);
        return;
    }
}
}
