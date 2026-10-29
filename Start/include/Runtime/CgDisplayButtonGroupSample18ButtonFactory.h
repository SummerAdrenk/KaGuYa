#pragma once

#include <Runtime/CgDisplayButtonGroupFactoryRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 18 号样本的 CCGdispBtn 主按钮与翻页按钮生产工厂。
class CgDisplayButtonGroupSample18ButtonFactoryAt499DC0 final
    : public ICgDisplayButtonGroupButtonFactoryAt499DC0
{
public:
    CgDisplayButtonGroupSample18ButtonFactoryAt499DC0(
        ICgUiButtonRuntimeServices& buttonServices,
        IGraphicsSurfaceCompositionApi& compositionApi,
        ICgOperationSurfaceFactory& surfaceFactory) noexcept;

    [[nodiscard]] std::shared_ptr<CgUiButtonRuntime>
        AllocateButtonAt459530() override;
    [[nodiscard]] bool AttachSurfaceAt4C1F20(
        CgUiButtonRuntime& button,
        std::uint32_t stateSurfaceSlot,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& identifier,
        const std::shared_ptr<RuntimeGraphicsSurface>& surface) override;
    [[nodiscard]] bool LoadButtonAt480930(
        CgUiButtonRuntime& button,
        std::uintptr_t recordAddress,
        std::int32_t finalArgument) override;

private:
    ICgUiButtonRuntimeServices* buttonServices_ = nullptr;
    IGraphicsSurfaceCompositionApi* compositionApi_ = nullptr;
    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
};
}
