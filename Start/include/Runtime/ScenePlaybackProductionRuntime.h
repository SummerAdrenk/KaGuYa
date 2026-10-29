#pragma once

#include <Runtime/ScenePlaybackButtonGroupFactoryRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ScenePlaybackButtonFactorySample18 final : public IScenePlaybackButtonFactory
{
public:
    explicit ScenePlaybackButtonFactorySample18(
        ScenePlaybackButtonGroupSample18ProductionDependencies dependencies) noexcept;
    std::shared_ptr<IScenePlaybackButton> CreateButtonAt49F270(std::int32_t group) override;

private:
    ScenePlaybackButtonGroupSample18ProductionDependencies dependencies_;
};

struct ScenePlaybackProductionDependenciesSample18 final
{
    IScenePlaybackHostServices& host;
    // 脚本服务属于共享 CScript 根 owner；不能每个鉴赏页面重建 VM。
    IScenePlaybackScriptServices& scripts;
    ScenePlaybackButtonGroupSample18ProductionDependencies buttons;
};

class ScenePlaybackProductionObjectGraphRuntimeSample18 final
{
public:
    explicit ScenePlaybackProductionObjectGraphRuntimeSample18(
        ScenePlaybackProductionDependenciesSample18 dependencies);
    ScenePlaybackRuntime& Scene() noexcept { return scene_; }

private:
    // Scene 先析构，释放五组按钮及源图；构造适配器随完整 owner 一起存活。
    ScenePlaybackButtonFactorySample18 buttons_;
    ScenePlaybackRuntime scene_;
};

class ScenePlaybackSceneConstructionServicesSample18 final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    ScenePlaybackSceneConstructionServicesSample18(ISceneConstructionServices& upstream,
        ScenePlaybackProductionDependenciesSample18 dependencies) noexcept;
    std::shared_ptr<ISceneTask> CreateScenePlaybackMode() override;

private:
    ScenePlaybackProductionDependenciesSample18 dependencies_;
};
}
