#pragma once

#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>
#include <Runtime/ScrVmYieldDispatcher.h>

#include <cstdint>
#include <optional>
#include <string>

namespace KaGuYa::Start::Runtime
{
// CReadScript 的逻辑状态所有者。optional 字段表示原构造函数尚未写入，
// 避免用零初始化掩盖“首次进入对应状态前不可读取”的原约束。
struct ReadScriptSceneState final : IScrVmYieldOwnerState
{
    void SetStateAndClearSubstate(std::int32_t state) noexcept override;

    std::int32_t mainState = 0;
    bool drawEnabled = true;
    std::int32_t substate = 0;
    std::optional<std::int32_t> interactionValueAt10;
    std::optional<std::int32_t> interactionValueAt14;
    std::optional<std::int32_t> savedMainStateAt4C;
    std::optional<std::int32_t> savedSubstateAt50;
    std::optional<std::int32_t> neutralDwordAt54;
    std::optional<std::int32_t> waitValueAt58;
    std::optional<std::int32_t> waitValueAt5C;
    std::string errorMessageAtB0;
    std::optional<std::int32_t> suspendValueAtB4;
    std::optional<std::uint8_t> interactionInitializedAtB8;
};

class IReadScriptUpdateBoundary
{
public:
    virtual ~IReadScriptUpdateBoundary() = default;

    [[nodiscard]] virtual bool RunVmYield(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunUpdateEffect(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunSuspend(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunTextInteraction(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunMenuInteraction(ReadScriptSceneState& state) = 0;
    [[nodiscard]] virtual bool RunWait(ReadScriptSceneState& state) = 0;
};

class IReadScriptDrawBoundary
{
public:
    virtual ~IReadScriptDrawBoundary() = default;

    virtual void PrepareRootOwners() = 0;
    virtual void SubmitCurrentDirtyRect() = 0;
    virtual void SubmitDrawRecords() = 0;
    virtual void CompleteRootOwnerDraw() = 0;
};

class IReadScriptErrorBoundary
{
public:
    virtual ~IReadScriptErrorBoundary() = default;

    virtual void ShowOriginalError(const char* messageCurrentCodePage) = 0;
    virtual void ShowSyntaxError() = 0;
    virtual void CleanupCurrentEnvironment() = 0;
};

// ReadScriptScene::CReadScript（4A14D0/4BA530/4B5A20）的外层场景状态机。
class ReadScriptSceneRuntime final : public ISceneTask
{
public:
    ReadScriptSceneRuntime(
        IReadScriptUpdateBoundary& updateBoundary,
        IReadScriptDrawBoundary& drawBoundary,
        IReadScriptErrorBoundary& errorBoundary) noexcept;

    void Run(std::int32_t argument) override;

    void Update();
    void Draw();

    [[nodiscard]] ReadScriptSceneState& State() noexcept;
    [[nodiscard]] const ReadScriptSceneState& State() const noexcept;

private:
    void DispatchUpdate();
    ReadScriptSceneState state_{};
    IReadScriptUpdateBoundary* updateBoundary_ = nullptr;
    IReadScriptDrawBoundary* drawBoundary_ = nullptr;
    IReadScriptErrorBoundary* errorBoundary_ = nullptr;
};

// 生产工厂位于 ReadScriptSceneOwnerRuntime.h；必须逐场景构造独立可变上下文。
}
