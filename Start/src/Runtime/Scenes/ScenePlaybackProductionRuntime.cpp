#include <Runtime/ScenePlaybackProductionRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
ScenePlaybackButtonFactorySample18::ScenePlaybackButtonFactorySample18(
    ScenePlaybackButtonGroupSample18ProductionDependencies dependencies) noexcept
    : dependencies_(dependencies)
{
}

std::shared_ptr<IScenePlaybackButton> ScenePlaybackButtonFactorySample18::CreateButtonAt49F270(
    const std::int32_t group)
{
    return CreateScenePlaybackButtonGroupSample18Production(group, dependencies_);
}

ScenePlaybackProductionObjectGraphRuntimeSample18::ScenePlaybackProductionObjectGraphRuntimeSample18(
    ScenePlaybackProductionDependenciesSample18 dependencies)
    : buttons_(dependencies.buttons), scene_(dependencies.host, buttons_, dependencies.scripts)
{
}

ScenePlaybackSceneConstructionServicesSample18::ScenePlaybackSceneConstructionServicesSample18(
    ISceneConstructionServices& upstream, ScenePlaybackProductionDependenciesSample18 dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream), dependencies_(dependencies)
{
}

std::shared_ptr<ISceneTask> ScenePlaybackSceneConstructionServicesSample18::CreateScenePlaybackMode()
{
    auto owner = std::make_shared<ScenePlaybackProductionObjectGraphRuntimeSample18>(dependencies_);
    auto* scene = &owner->Scene();
    // SceneFactory 接收的任务引用持有整个对象图，而非借用局部 services 的裸任务。
    return std::shared_ptr<ISceneTask>(std::move(owner), scene);
}
}
