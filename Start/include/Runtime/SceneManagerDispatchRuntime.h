#pragma once

#include <Runtime/SceneControllerRuntime.h>
#include <Runtime/TaskControllerNativeRuntime.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// CSceneManager 中“SceneController 有工作时经唯一 CTaskController 项执行”的可达边界。
// 原 CTaskController 仅在 45BC4E 注册这一项，固定优先级为 1。
class SceneManagerDispatchRuntime final
{
public:
    explicit SceneManagerDispatchRuntime(ISceneFactory& factory);

    void QueueScene(const char* sceneNameCp932);
    void PushActiveScene(const char* sceneNameCp932);
    void ClearScenes() noexcept;

    [[nodiscard]] bool HasPendingSceneWork() const noexcept;
    [[nodiscard]] bool RunSceneTasks(std::int32_t argument);

    [[nodiscard]] SceneControllerRuntime& Controller() noexcept;
    [[nodiscard]] const SceneControllerRuntime& Controller() const noexcept;
    [[nodiscard]] TaskControllerNativeBody18h* TaskControllerBody() const noexcept{return scheduler_.Body();}

private:
    // 原先TaskController后SceneController；成员逆序退役先controller再只借task的调度器。
    TaskControllerNativeRuntime scheduler_;
    SceneControllerRuntime controller_;
};
}
