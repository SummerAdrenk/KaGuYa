#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
class ISceneConstructionServices
{
public:
    virtual ~ISceneConstructionServices() = default;

    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateTitleLogo() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateTitle() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateMusicAppreciationMode() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateCgDisplayMode() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateScenePlaybackMode() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateLogScreen() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateSaveScreen() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateLoadScreen() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateLoadAfterSelection() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateScriptProcessing() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateSettingsScreen() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateAffectionScreen() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateMoviePlayback() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateEnding() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateTextDisplayProcessing() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateProgramSelectionProcessing() = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneTask> CreateGraffitiPageProcessing() = 0;
};

// 生产构造服务按已闭环的场景分支逐层叠加。未被当前层接管的入口必须
// 原样转交，不能返回空对象、占位场景或按名称猜测其他实现。
class SceneConstructionServicesForwarderRuntime
    : public ISceneConstructionServices
{
public:
    explicit SceneConstructionServicesForwarderRuntime(
        ISceneConstructionServices& upstream) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask> CreateTitleLogo() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateTitle() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateMusicAppreciationMode() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateCgDisplayMode() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateScenePlaybackMode() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateLogScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateSaveScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateLoadScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateLoadAfterSelection() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateScriptProcessing() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateSettingsScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateAffectionScreen() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateMoviePlayback() override;
    [[nodiscard]] std::shared_ptr<ISceneTask> CreateEnding() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateTextDisplayProcessing() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateProgramSelectionProcessing() override;
    [[nodiscard]] std::shared_ptr<ISceneTask>
        CreateGraffitiPageProcessing() override;

protected:
    [[nodiscard]] ISceneConstructionServices& Upstream() const noexcept;

private:
    ISceneConstructionServices* upstream_ = nullptr;
};

// Scene::CSceneFactory::Create（4AAF90）的十七项硬编码映射。
// 这是内部所有权模型；原 x86 工厂返回裸 ITask*，由 SceneController 立即建立 owning ref。
class SceneFactoryRuntime final : public ISceneFactory
{
public:
    explicit SceneFactoryRuntime(ISceneConstructionServices& services) noexcept;

    [[nodiscard]] std::shared_ptr<ISceneTask> Create(
        const char* sceneNameCp932) override;

private:
    ISceneConstructionServices* services_ = nullptr;
};
}
