#pragma once

#include <Runtime/GraffitiPageSceneRuntime.h>
#include <Runtime/LikabilitySceneRuntime.h>
#include <Runtime/MessageLogSceneRuntime.h>
#include <Runtime/SceneFactoryRuntime.h>
#include <Runtime/SettingSceneRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 这四个场景的构造函数只保存根服务引用，并在原构造点取得各自 Dialog 的
// 强引用。适配层不复制根状态，也不接管尚未闭环的其他 SceneFactory 分支。
class DialogSceneConstructionServicesRuntime final
    : public SceneConstructionServicesForwarderRuntime
{
public:
    DialogSceneConstructionServicesRuntime(
        ISceneConstructionServices& upstream,
        IMessageLogSceneServices& messageLogServices,
        ISettingSceneServices& settingServices,
        ILikabilitySceneServices& likabilityServices,
        IGraffitiPageSceneServices& graffitiPageServices) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask> CreateLogScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateSettingsScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateAffectionScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateGraffitiPageProcessing() override;

private:
    IMessageLogSceneServices* messageLogServices_ = nullptr;
    ISettingSceneServices* settingServices_ = nullptr;
    ILikabilitySceneServices* likabilityServices_ = nullptr;
    IGraffitiPageSceneServices* graffitiPageServices_ = nullptr;
};
}
