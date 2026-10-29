#pragma once

#include <Runtime/SceneManagerDispatchRuntime.h>
#include <Runtime/TitleMenuDispatchRuntime.h>
#include <Runtime/RuntimeCStringResult.h>

#include <cstdint>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class ITitleSceneRuntimeServices
{
public:
    virtual ~ITitleSceneRuntimeServices() = default;

    // 4BADF0..4BAE20：清全局描述项当前目标的 +0x10 DWORD。
    virtual void ClearDescriptorTargetDwordAt10() = 0;

    // SystemInfomation[2]+0x04。零值跳过标题模态分支。
    [[nodiscard]] virtual std::uint32_t GameInputStateModeAt04() const = 0;
    [[nodiscard]] virtual RuntimeCStringResult CloneCurrentSceneNameAt414540() const = 0;

    // 47F650 的业务按键名称尚未闭环，禁止提前命名。
    [[nodiscard]] virtual bool EvaluateInputPredicate47F650() = 0;

    virtual void ClearCurrentSceneName() = 0;
    virtual void SetRootByteAt1AF(std::uint8_t value) = 0;
    virtual void PushActiveScene(const char* sceneNameCp932) = 0;
};

class ITitleSceneState
{
public:
    virtual ~ITitleSceneState() = default;

    virtual void ClearDescriptorTargetDwordAt10() = 0;
    [[nodiscard]] virtual std::uint32_t GameInputStateModeAt04() const = 0;
    [[nodiscard]] virtual RuntimeCStringResult CloneCurrentSceneNameAt414540() const = 0;
    [[nodiscard]] virtual bool EvaluateInputPredicate47F650() = 0;
    virtual void ClearCurrentSceneName() = 0;
    virtual void SetRootByteAt1AF(std::uint8_t value) = 0;
};

// 把标题对象自身/根状态与唯一 SceneController 绑定到同一生产服务边界。
class TitleSceneRuntimeServices final : public ITitleSceneRuntimeServices
{
public:
    TitleSceneRuntimeServices(
        ITitleSceneState& state,
        SceneManagerDispatchRuntime& sceneManager) noexcept;

    void ClearDescriptorTargetDwordAt10() override;
    [[nodiscard]] std::uint32_t GameInputStateModeAt04() const override;
    [[nodiscard]] RuntimeCStringResult CloneCurrentSceneNameAt414540() const override;
    [[nodiscard]] bool EvaluateInputPredicate47F650() override;
    void ClearCurrentSceneName() override;
    void SetRootByteAt1AF(std::uint8_t value) override;
    void PushActiveScene(const char* sceneNameCp932) override;

private:
    ITitleSceneState* state_ = nullptr;
    SceneManagerDispatchRuntime* sceneManager_ = nullptr;
};

// TitleScene::CTitle::Update（4BADA0）的完整可达协调顺序：
// 阶段 0 初始化 -> Load/Settings 模态拦截 -> 最终标题菜单。
class TitleSceneRuntime final : public ISceneTask
{
public:
    TitleSceneRuntime(
        ITitleDialogRuntime& dialog,
        ITitleSceneRuntimeServices& sceneServices,
        ITitleMenuRuntimeServices& menuServices) noexcept;

    // 原 ITask 包装不使用帧参数。
    void Run(std::int32_t argument) override;

    [[nodiscard]] std::uint32_t Stage() const noexcept;

private:
    void Update();

    ITitleDialogRuntime* dialog_ = nullptr;
    ITitleSceneRuntimeServices* sceneServices_ = nullptr;
    TitleMenuDispatchRuntime menuDispatch_;
    std::uint32_t stage_ = 0u;
};
}
