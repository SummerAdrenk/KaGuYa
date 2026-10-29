#pragma once

#include <Runtime/SceneManagerModeSelectButtonRuntime.h>
#include <Runtime/SceneManagerOptionCharSelectDialogObjectGraphRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 样本 18 的 CSceneManager 公共对话框工厂。构造参数分别对应
// 0x45AC40 与 0x4E4A20 已证明的外部依赖，不在工厂中复制其状态。
class SceneManagerPublicDialogFactorySample18 final
    : public ISceneManagerPublicDialogFactory
{
public:
    SceneManagerPublicDialogFactorySample18(
        ICgUiButtonRuntimeServices& buttonServices,
        IGraphicsSurfaceCompositionApi& compositionApi,
        ICgOperationSurfaceFactory& surfaceFactory,
        ISceneManagerOptionCharAvailabilitySample18& optionAvailability,
        ISceneManagerOptionCharInheritedServices& optionInheritedServices)
        noexcept;

    [[nodiscard]] std::shared_ptr<ISceneManagerModeSelectButton>
        CreateModeSelectButtonAt45AC40(std::int32_t mode) override;
    [[nodiscard]] std::shared_ptr<ISceneManagerOptionCharSelectDialog>
        CreateOptionCharSelectDialogAt4E4A20() override;

private:
    ICgUiButtonRuntimeServices* buttonServices_ = nullptr;
    IGraphicsSurfaceCompositionApi* compositionApi_ = nullptr;
    ICgOperationSurfaceFactory* surfaceFactory_ = nullptr;
    ISceneManagerOptionCharAvailabilitySample18* optionAvailability_ = nullptr;
    ISceneManagerOptionCharInheritedServices* optionInheritedServices_ = nullptr;
};
}
