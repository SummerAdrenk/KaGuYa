#pragma once

#include <Runtime/ScenePlaybackButtonGroupCatalogRuntime.h>
#include <Runtime/CgDisplayButtonGroupSample18StaticData.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ScenePlaybackButtonGroupFactoryDependenciesAt49F270 final
{
    ICgDisplayButtonGroupRuntimeServices& runtimeServices;
    const IScenePlaybackButtonGroupCatalog& catalog;
    const ICgDisplayPageBaseVariantSourceAt499DC0& pageBaseVariantSource;
    const CgDisplayButtonGroupSample18StaticDataAt499DC0& staticData;
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphicsProvider;
    ICgDisplayButtonGroupButtonFactoryAt499DC0& buttonFactory;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
};

// 样本 18 CModeReplayBtn 构造；保持 int 1/2/8 错误，不转换成空按钮组。
[[nodiscard]] std::unique_ptr<CgDisplayButtonGroupRuntime> CreateScenePlaybackButtonGroupAt49F270(
    std::int32_t group, ScenePlaybackButtonGroupFactoryDependenciesAt49F270 dependencies);

struct ScenePlaybackButtonGroupSample18ProductionDependencies final
{
    const ParamsGameSystem& gameSystem;
    const SecretScArchive& secretSc;
    const IProgramRegistrySlot1& slot1;
    ICgDisplayButtonGroupRuntimeServices& runtimeServices;
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceApi& surfaceApi;
    IGraphicsSurfaceCompositionApi& compositionApi;
    IGraphicsSurfaceCopyApi& copyApi;
    ICgUiButtonRuntimeServices& buttonServices;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
};

// 返回对象仅借用调用方的运行服务，不保存构造期目录和适配器的地址。
[[nodiscard]] std::unique_ptr<CgDisplayButtonGroupRuntime> CreateScenePlaybackButtonGroupSample18Production(
    std::int32_t group, ScenePlaybackButtonGroupSample18ProductionDependencies dependencies);
}
