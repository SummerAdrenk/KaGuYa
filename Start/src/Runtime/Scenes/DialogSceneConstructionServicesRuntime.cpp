#include <Runtime/DialogSceneConstructionServicesRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
DialogSceneConstructionServicesRuntime::
DialogSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    IMessageLogSceneServices& messageLogServices,
    ISettingSceneServices& settingServices,
    ILikabilitySceneServices& likabilityServices,
    IGraffitiPageSceneServices& graffitiPageServices) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      messageLogServices_(&messageLogServices),
      settingServices_(&settingServices),
      likabilityServices_(&likabilityServices),
      graffitiPageServices_(&graffitiPageServices)
{
}

std::shared_ptr<ISceneTask>
DialogSceneConstructionServicesRuntime::CreateLogScreen()
{
    return std::make_shared<MessageLogSceneRuntime>(*messageLogServices_);
}

std::shared_ptr<ISceneTask>
DialogSceneConstructionServicesRuntime::CreateSettingsScreen()
{
    return std::make_shared<SettingSceneRuntime>(*settingServices_);
}

std::shared_ptr<ISceneTask>
DialogSceneConstructionServicesRuntime::CreateAffectionScreen()
{
    return std::make_shared<LikabilitySceneRuntime>(*likabilityServices_);
}

std::shared_ptr<ISceneTask>
DialogSceneConstructionServicesRuntime::CreateGraffitiPageProcessing()
{
    return std::make_shared<GraffitiPageSceneRuntime>(*graffitiPageServices_);
}
}
