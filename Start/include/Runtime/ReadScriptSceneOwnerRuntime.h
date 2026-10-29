#pragma once

#include <Runtime/ReadScriptSceneContextRuntime.h>
#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/ReadScriptSceneRestoreNative.h>

namespace KaGuYa::Start::Runtime
{
class SceneManagerRuntime;
class IToolWindowInteractionServices;

struct ReadScriptSceneOwnerDependencies final
{
    ReadScriptSceneContextDependencies context;
    SceneManagerRuntime& sceneManager;
    IToolWindowInteractionServices& toolInteraction;
    IReadScriptDrawBoundary& drawing;
    IReadScriptErrorBoundary& errors;
};
struct ReadScriptSceneOwnerNativeDependencies final
{
    ReadScriptSceneContextNativeDependencies context;
    IReadScriptSceneRestoreSource& restore;
    IReadScriptDrawBoundary& drawing;
    IReadScriptErrorBoundary& errors;
};

// 4A14D0/4A4920：一个 owning Ref 对应一份完整 ReadScript 对象图。
// 返回给 Controller 的 scene 指针与本 owner 共享唯一控制块，不建立第二份拥有关系。
class ReadScriptSceneOwnerRuntime final
{
public:
    explicit ReadScriptSceneOwnerRuntime(ReadScriptSceneOwnerDependencies dependencies);
    explicit ReadScriptSceneOwnerRuntime(ReadScriptSceneOwnerNativeDependencies dependencies);
    ~ReadScriptSceneOwnerRuntime() noexcept;
    ReadScriptSceneOwnerRuntime(const ReadScriptSceneOwnerRuntime&) = delete;
    ReadScriptSceneOwnerRuntime& operator=(const ReadScriptSceneOwnerRuntime&) = delete;

    [[nodiscard]] ReadScriptSceneRuntime& Scene() noexcept { return scene_; }
    [[nodiscard]] ReadScriptSceneContextRuntime& Context() noexcept { return context_; }

private:
    ReadScriptSceneContextRuntime context_;
    ReadScriptSceneRuntime scene_;
};

class ReadScriptSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    ReadScriptSceneConstructionServicesRuntime(ISceneConstructionServices& upstream,
        ReadScriptSceneOwnerDependencies dependencies) noexcept;
    ReadScriptSceneConstructionServicesRuntime(ISceneConstructionServices& upstream,
        ReadScriptSceneOwnerNativeDependencies dependencies) noexcept;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateScriptProcessing() override;

private:
    // 按值保存引用集合；不会借用调用者栈上的临时 Dependencies 对象。
    std::variant<ReadScriptSceneOwnerDependencies, ReadScriptSceneOwnerNativeDependencies> dependencies_;
};
}
