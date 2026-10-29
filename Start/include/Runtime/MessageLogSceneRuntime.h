#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class IMessageLogDialog
{
public:
    virtual ~IMessageLogDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
    virtual void EndAtVslot4C() = 0;
};

using MessageLogDialogRef = std::shared_ptr<IMessageLogDialog>;

enum class MessageLogScenePhase : std::uint32_t
{
    BeginDialog = 0,
    PollDialog = 1,
    ReturnToPreviousScene = 2,
};

class IMessageLogSceneServices
{
public:
    virtual ~IMessageLogSceneServices() = default;

    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual MessageLogDialogRef CreateMessageLogDialog() = 0;
    [[nodiscard]] virtual std::uint8_t ReadRootByteAt218() const = 0;
    virtual void WriteRootByteAt218(std::uint8_t value) = 0;
    [[nodiscard]] virtual std::uint8_t ReadRootByteAt219() const = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;
    virtual void AdvanceActiveSceneFromPendingAt4C1760() = 0;
};

// LogScene::CLogScene（49E180/4B6900）的三阶段状态机。
class MessageLogSceneRuntime final : public ISceneTask
{
public:
    explicit MessageLogSceneRuntime(IMessageLogSceneServices& services);

    // 4C3630：Update 后调用原始空 Draw；argument 不被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] MessageLogScenePhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistrySlot2DwordAt04() const noexcept;
    [[nodiscard]] std::uint8_t SavedRootByteAt218() const noexcept;
    [[nodiscard]] std::uint8_t SavedRootByteAt219() const noexcept;
    [[nodiscard]] const MessageLogDialogRef& Dialog() const noexcept;

private:
    void Update();

    IMessageLogSceneServices* services_ = nullptr;
    MessageLogScenePhase phase_ = MessageLogScenePhase::BeginDialog;
    std::uint32_t savedRegistrySlot2DwordAt04_ = 0;
    MessageLogDialogRef dialog_;
    std::uint8_t savedRootByteAt218_ = 0;
    std::uint8_t savedRootByteAt219_ = 0;
};
}
