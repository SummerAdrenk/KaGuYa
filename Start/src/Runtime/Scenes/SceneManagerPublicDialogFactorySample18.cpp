#include <Runtime/SceneManagerPublicDialogFactorySample18.h>

namespace KaGuYa::Start::Runtime
{
SceneManagerPublicDialogFactorySample18::
SceneManagerPublicDialogFactorySample18(
    ICgUiButtonRuntimeServices& buttonServices,
    IGraphicsSurfaceCompositionApi& compositionApi,
    ICgOperationSurfaceFactory& surfaceFactory,
    ISceneManagerOptionCharAvailabilitySample18& optionAvailability,
    ISceneManagerOptionCharInheritedServices& optionInheritedServices) noexcept
    : buttonServices_(&buttonServices),
      compositionApi_(&compositionApi),
      surfaceFactory_(&surfaceFactory),
      optionAvailability_(&optionAvailability),
      optionInheritedServices_(&optionInheritedServices)
{
}

std::shared_ptr<ISceneManagerModeSelectButton>
SceneManagerPublicDialogFactorySample18::CreateModeSelectButtonAt45AC40(
    const std::int32_t mode)
{
    return std::make_shared<SceneManagerModeSelectButtonRuntime>(
        mode, *buttonServices_, *compositionApi_, *surfaceFactory_);
}

std::shared_ptr<ISceneManagerOptionCharSelectDialog>
SceneManagerPublicDialogFactorySample18::
CreateOptionCharSelectDialogAt4E4A20()
{
    return std::make_shared<
        SceneManagerOptionCharSelectDialogObjectGraphRuntime>(
            *surfaceFactory_,
            *compositionApi_,
            *optionAvailability_,
            *optionInheritedServices_);
}
}
