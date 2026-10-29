#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
enum class SaveLoadSceneMode : std::uint8_t
{
    Load = 0,
    Save = 1,
};

enum class SaveLoadScenePhase : std::uint32_t
{
    BeginDialog = 0,
    PollDialog = 1,
    ReturnToPreviousScene = 2,
    QueueLoadAfterSelection = 3,
};

class ISaveLoadDialog
{
public:
    virtual ~ISaveLoadDialog() = default;

    virtual void BeginAtVslot44() = 0;
    [[nodiscard]] virtual bool PollAtVslot48() = 0;
    virtual void EndAtVslot4C() = 0;
    [[nodiscard]] virtual std::uint32_t CompletionResultAtVslot50() const = 0;
};

using SaveLoadDialogRef = std::shared_ptr<ISaveLoadDialog>;

class ISaveLoadSceneServices
{
public:
    virtual ~ISaveLoadSceneServices() = default;

    [[nodiscard]] virtual std::uint8_t ReadRootByteAt1AF() const = 0;
    [[nodiscard]] virtual SaveLoadDialogRef CreateSaveLoadDialog(
        SaveLoadSceneMode mode,
        std::uint8_t rootByteAt1AF) = 0;

    [[nodiscard]] virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual std::uint8_t ReadRootByteAt219() const = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;

    virtual void WriteRootSubobjectAt4AD230DwordAt10(std::uint32_t value) = 0;
    virtual void AdvanceActiveSceneFromPendingAt4C1760() = 0;
    virtual void ClearActiveSceneAt4C1460() = 0;
    virtual void QueueLoadAfterSelectionSceneAt47F920() = 0;
};

// SaveLoadScene::CSaveLoadScene（4A17B0/4BA650）的四阶段状态机。
class SaveLoadSceneRuntime final : public ISceneTask
{
public:
    SaveLoadSceneRuntime(
        ISaveLoadSceneServices& services,
        SaveLoadSceneMode mode);

    // 4C3630：Update 后调用原始空 Draw；argument 不被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] SaveLoadSceneMode Mode() const noexcept;
    [[nodiscard]] SaveLoadScenePhase Phase() const noexcept;
    [[nodiscard]] std::uint32_t SavedRegistrySlot2DwordAt04() const noexcept;
    [[nodiscard]] std::uint8_t SavedRootByteAt219() const noexcept;
    [[nodiscard]] const SaveLoadDialogRef& Dialog() const noexcept;

private:
    void Update();

    ISaveLoadSceneServices* services_ = nullptr;
    SaveLoadSceneMode mode_ = SaveLoadSceneMode::Load;
    SaveLoadScenePhase phase_ = SaveLoadScenePhase::BeginDialog;
    SaveLoadDialogRef dialog_;
    std::uint32_t savedRegistrySlot2DwordAt04_ = 0;
    std::uint8_t savedRootByteAt219_ = 0;
};
}
