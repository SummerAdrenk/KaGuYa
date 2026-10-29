#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class ISettingDialog
{
public:
    virtual ~ISettingDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
    virtual void EndAtVslot4C() = 0;
};

using SettingDialogRef = std::shared_ptr<ISettingDialog>;

enum class SettingScenePhase : std::uint32_t
{
    BeginDialog = 0,
    PollDialog = 1,
    AdvanceScene = 2,
};

class ISettingSceneServices
{
public:
    virtual ~ISettingSceneServices() = default;

    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt14() const = 0;
    virtual void WriteRegistrySlot2DwordAt14(std::uint32_t value) = 0;
    [[nodiscard]] virtual SettingDialogRef CreateSettingDialog() = 0;
    virtual void AdvanceActiveSceneFromPendingAt4C1760() = 0;
};

// SettingScene::CSettingScene（4A1A00/4BA800）的三阶段状态机。
class SettingSceneRuntime final : public ISceneTask
{
public:
    explicit SettingSceneRuntime(ISettingSceneServices& services);

    // 4C3630：Update 后调用原空 Draw；argument 未被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] SettingScenePhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistrySlot2DwordAt04() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistrySlot2DwordAt14() const noexcept;
    [[nodiscard]] const SettingDialogRef& Dialog() const noexcept;

private:
    void Update();

    ISettingSceneServices* services_ = nullptr;
    SettingScenePhase phase_ = SettingScenePhase::BeginDialog;
    std::uint32_t savedRegistrySlot2DwordAt04_ = 0;
    std::uint32_t savedRegistrySlot2DwordAt14_ = 0;
    SettingDialogRef dialog_;
};
}
