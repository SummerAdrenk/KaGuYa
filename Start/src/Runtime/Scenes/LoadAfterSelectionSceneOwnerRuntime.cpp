#include <Runtime/LoadAfterSelectionSceneOwnerRuntime.h>
#include <Runtime/DrawManagerRuntime.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/ScrVmRuntimeComposition.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
RestoredScriptServicesDependencies RestoreDependencies(
    const LoadAfterSelectionSceneOwnerDependencies& dependencies,
    INormalStartupRootOwnerSynchronizer& resetOwners)
{
    // 显式拒绝装配错误，不复制 values 或临时换 generation 来掩盖根身份不一致。
    if (&dependencies.bundle.values != &dependencies.scene.values)
        throw std::invalid_argument("LoadAfter scene and restore bundle use different values owners");
    dependencies.bundle.RequireScriptView(dependencies.script);
    return {dependencies.bundle.paramsGeneration, dependencies.script, dependencies.scene.values,
        dependencies.resourceNames, dependencies.profile, dependencies.catalog, dependencies.transaction,
        dependencies.files, dependencies.key, dependencies.codecs, dependencies.scene.save, dependencies.scene.unsave,
        resetOwners, dependencies.scene.graphics, dependencies.vm, dependencies.errors};
}
}

LoadAfterSelectionSceneOwnerRuntime::LoadAfterSelectionSceneOwnerRuntime(
    LoadAfterSelectionSceneOwnerDependencies dependencies)
    : resetOwners_(dependencies.scene.drawing.Layers(), dependencies.scene.graphics),
      restored_(RestoreDependencies(dependencies, resetOwners_)),
      transaction_(dependencies.bundle.scriptCommand, restored_),
      services_(dependencies.scene),
      scene_(dependencies.bundle.scriptCommand, transaction_, services_)
{
}

LoadAfterSelectionOwnerConstructionServicesRuntime::LoadAfterSelectionOwnerConstructionServicesRuntime(
    ISceneConstructionServices& upstream, LoadAfterSelectionSceneOwnerDependencies dependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream), dependencies_(dependencies)
{
}

std::shared_ptr<ISceneTask> LoadAfterSelectionOwnerConstructionServicesRuntime::CreateLoadAfterSelection()
{
    auto owner = std::make_shared<LoadAfterSelectionSceneOwnerRuntime>(dependencies_);
    return std::shared_ptr<ISceneTask>(owner, &owner->Scene());
}
}
