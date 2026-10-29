#include <Runtime/TitleMenuRuntimeServices.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
TitleMenuRuntimeServices::TitleMenuRuntimeServices(
    NormalStartupScriptTransaction& normalStartup,
    SceneManagerDispatchRuntime& sceneManager,
    ITitleMenuRootState& rootState) noexcept
    : normalStartup_(&normalStartup),
      sceneManager_(&sceneManager),
      rootState_(&rootState)
{
}

bool TitleMenuRuntimeServices::LoadScript(
    const char* const scriptNameCp932,
    const std::int32_t argument)
{
    if (argument != -1)
    {
        throw std::invalid_argument(
            "title menu normal-startup service received a non-minus-one argument");
    }
    return normalStartup_->Execute(scriptNameCp932).success;
}

void TitleMenuRuntimeServices::ClearSceneController()
{
    sceneManager_->ClearScenes();
}

void TitleMenuRuntimeServices::QueueScene(const char* const sceneNameCp932)
{
    sceneManager_->QueueScene(sceneNameCp932);
}

void TitleMenuRuntimeServices::SetRootByteAt218(const std::uint8_t value)
{
    rootState_->WriteByteAt218(value);
}

void TitleMenuRuntimeServices::SetRootByteAt219(const std::uint8_t value)
{
    rootState_->WriteByteAt219(value);
}
}
