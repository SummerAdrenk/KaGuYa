#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class ILikabilityDialog
{
public:
    virtual ~ILikabilityDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
};

using LikabilityDialogRef = std::shared_ptr<ILikabilityDialog>;

enum class LikabilityScenePhase : std::uint32_t
{
    BeginDialog = 0,
    PollDialog = 1,
    AdvanceScene = 2,
};

class ILikabilitySceneServices
{
public:
    virtual ~ILikabilitySceneServices() = default;

    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual LikabilityDialogRef CreateLikabilityDialog() = 0;
    virtual void AdvanceActiveSceneFromPendingAt4C1760() = 0;
};

// LikabilityScene::CLikabilityScene（49DFB0/4B6280）的三阶段状态机。
class LikabilitySceneRuntime final : public ISceneTask
{
public:
    explicit LikabilitySceneRuntime(ILikabilitySceneServices& services);

    // 4C3630：Update 后调用原空 Draw；argument 未被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] LikabilityScenePhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistrySlot2DwordAt04() const noexcept;
    [[nodiscard]] const LikabilityDialogRef& Dialog() const noexcept;

private:
    void Update();

    ILikabilitySceneServices* services_ = nullptr;
    LikabilityScenePhase phase_ = LikabilityScenePhase::BeginDialog;
    std::uint32_t savedRegistrySlot2DwordAt04_ = 0;
    LikabilityDialogRef dialog_;
};
}
