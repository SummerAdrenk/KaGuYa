#include <Runtime/SceneManagerDispatchRuntime.h>

namespace KaGuYa::Start::Runtime
{
SceneManagerDispatchRuntime::SceneManagerDispatchRuntime(ISceneFactory& factory)
    : controller_(factory)
{
    scheduler_.Register(controller_.BorrowNativeTask(),1);
}

void SceneManagerDispatchRuntime::QueueScene(const char* const sceneNameCp932)
{
    controller_.Queue(sceneNameCp932);
}

void SceneManagerDispatchRuntime::PushActiveScene(const char* const sceneNameCp932)
{
    controller_.PushActiveScene(sceneNameCp932);
}

void SceneManagerDispatchRuntime::ClearScenes() noexcept
{
    controller_.Clear();
}

bool SceneManagerDispatchRuntime::HasPendingSceneWork() const noexcept
{
    // 4836AA..4836CD：先读pending ptr4字节数，非空不访问active Ref12。
    return HasSceneControllerWorkNativeAt4836AA(controller_.NativeBody());
}

bool SceneManagerDispatchRuntime::RunSceneTasks(const std::int32_t argument)
{
    if (!HasPendingSceneWork())
        return false;

    // 4836EE通过原调度链；即使当前仅一个注册项也不跳过阻断/失活/改优先级分支。
    SceneControllerRuntime::NativeCallScope scope(controller_);
    scheduler_.Run(argument);
    return true;
}

SceneControllerRuntime& SceneManagerDispatchRuntime::Controller() noexcept
{
    return controller_;
}

const SceneControllerRuntime& SceneManagerDispatchRuntime::Controller() const noexcept
{
    return controller_;
}
}
