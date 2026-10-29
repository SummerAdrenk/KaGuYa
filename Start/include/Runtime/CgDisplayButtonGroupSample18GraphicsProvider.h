#pragma once

#include <Runtime/ParamsPatternSource.h>

#include <Runtime/CgDisplayButtonGroupCatalogRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 18 号样本中 0x499DC0 使用的 Graphics 边界。资源名和坐标表只接受
// Start.exe 固定地址；其他 executable profile 必须提供自己的实现。
class CgDisplayButtonGroupSample18GraphicsProviderAt499DC0 final
    : public ICgDisplayButtonGroupGraphicsProviderAt499DC0
{
public:
    CgDisplayButtonGroupSample18GraphicsProviderAt499DC0(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceApi& surfaceApi,
        IGraphicsSurfaceCompositionApi& compositionApi,
        IGraphicsSurfaceCopyApi& copyApi) noexcept;

    [[nodiscard]] IGraphicsSurfaceCompositionApi& CompositionApi() override;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface>
        AllocateSurfaceAt40C2E0() override;
    [[nodiscard]] bool LoadSurfaceAt4800A0(
        RuntimeGraphicsSurface& destination,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& resource)
        override;
    [[nodiscard]] bool CreateSurfaceAt415010(
        RuntimeGraphicsSurface& destination,
        std::int32_t width,
        std::int32_t height,
        std::int32_t alphaArgument,
        std::int32_t clearArgument) override;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface>
        CopySurfaceAt444470(
            const RuntimeGraphicsSurface& source,
            std::int32_t copyPixelsArgument) override;
    void BlitSurfaceAt4425C0(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source,
        std::int32_t destinationX,
        std::int32_t destinationY) override;
    [[nodiscard]] bool DrawValueAt4BC950(
        std::int32_t value,
        RuntimeGraphicsSurface& destination,
        std::uintptr_t resourcePointerTableAddress,
        std::uintptr_t coordinateTableAddress) override;
    [[nodiscard]] bool DrawValueAt4B4D20(
        std::int32_t value,
        RuntimeGraphicsSurface& destination,
        std::uintptr_t resourcePointerTableAddress,
        std::uintptr_t xCoordinateDwordArrayAddress,
        std::int32_t y) override;

private:
    [[nodiscard]] bool DrawDigit(
        RuntimeGraphicsSurface& destination,
        const char* resourcePrefixCp932,
        std::int32_t digit,
        std::int32_t x,
        std::int32_t y);

    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
    IGraphicsSurfaceApi* surfaceApi_ = nullptr;
    IGraphicsSurfaceCompositionApi* compositionApi_ = nullptr;
    IGraphicsSurfaceCopyApi* copyApi_ = nullptr;
};

struct CgDisplayButtonGroupSample18ProductionDependenciesAt499DC0 final
{
    SecretCgArchive& secretCg;
    ISecretCgParamsRootSource& thumbnailParams;
    const IProgramRegistrySlot1& slot1;
    ICgDisplayButtonGroupRuntimeServices& runtimeServices;
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceApi& surfaceApi;
    IGraphicsSurfaceCompositionApi& compositionApi;
    IGraphicsSurfaceCopyApi& copyApi;
    ICgUiButtonRuntimeServices& buttonServices;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
};

// sample 18 的生产入口在栈上建立固定表和 Graphics 适配器。返回对象只持有
// compositionApi、按钮和 Surface，不会悬挂引用到这两个构造期适配器。
[[nodiscard]] std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupSample18ProductionAt499DC0(
    std::int32_t categoryIndex,
    CgDisplayButtonGroupSample18ProductionDependenciesAt499DC0 dependencies);
}
