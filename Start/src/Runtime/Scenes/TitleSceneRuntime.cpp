#include <Runtime/TitleSceneRuntime.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kLoadSceneNameCp932[] =
    "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
constexpr char kSettingsSceneNameCp932[] =
    "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
}

TitleSceneRuntimeServices::TitleSceneRuntimeServices(
    ITitleSceneState& state,
    SceneManagerDispatchRuntime& sceneManager) noexcept
    : state_(&state), sceneManager_(&sceneManager)
{
}

void TitleSceneRuntimeServices::ClearDescriptorTargetDwordAt10()
{
    state_->ClearDescriptorTargetDwordAt10();
}

std::uint32_t TitleSceneRuntimeServices::GameInputStateModeAt04() const
{
    return state_->GameInputStateModeAt04();
}

RuntimeCStringResult TitleSceneRuntimeServices::CloneCurrentSceneNameAt414540() const
{
    return state_->CloneCurrentSceneNameAt414540();
}

bool TitleSceneRuntimeServices::EvaluateInputPredicate47F650()
{
    return state_->EvaluateInputPredicate47F650();
}

void TitleSceneRuntimeServices::ClearCurrentSceneName()
{
    state_->ClearCurrentSceneName();
}

void TitleSceneRuntimeServices::SetRootByteAt1AF(const std::uint8_t value)
{
    state_->SetRootByteAt1AF(value);
}

void TitleSceneRuntimeServices::PushActiveScene(const char* const sceneNameCp932)
{
    sceneManager_->PushActiveScene(sceneNameCp932);
}

TitleSceneRuntime::TitleSceneRuntime(
    ITitleDialogRuntime& dialog,
    ITitleSceneRuntimeServices& sceneServices,
    ITitleMenuRuntimeServices& menuServices) noexcept
    : dialog_(&dialog),
      sceneServices_(&sceneServices),
      menuDispatch_(dialog, menuServices)
{
}

void TitleSceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
}

std::uint32_t TitleSceneRuntime::Stage() const noexcept
{
    return stage_;
}

void TitleSceneRuntime::Update()
{
    switch (stage_)
    {
    case 0u:
        // 4BADF0..4BAE4D：初始化完成后不返回，同帧继续中间状态判断。
        sceneServices_->ClearDescriptorTargetDwordAt10();
        dialog_->InvokeVslot44();
        stage_ = 1u;
        break;

    case 1u:
        break;

    default:
        return;
    }

    if (sceneServices_->GameInputStateModeAt04() != 0u)
    {
        bool loadRequested;
        {
            // 4BAFA3..4BB043：复制名称保持到谓词之后；先释放局部串，再进入模态分支。
            const auto currentScene = sceneServices_->CloneCurrentSceneNameAt414540();
            loadRequested = _mbscmp(reinterpret_cast<const unsigned char*>(currentScene.Value().GetString()),
                reinterpret_cast<const unsigned char*>(kLoadSceneNameCp932)) == 0 ||
                sceneServices_->EvaluateInputPredicate47F650();
        }
        if (loadRequested)
        {
            // 清名称和 +1AF 均先于工厂创建；下游异常不得回滚。
            sceneServices_->ClearCurrentSceneName();
            sceneServices_->SetRootByteAt1AF(0u);
            sceneServices_->PushActiveScene(kLoadSceneNameCp932);
            return;
        }

        bool settingsRequested;
        {
            // 4BB043..4BB198另取当前根/SceneManager并再次复制；不能沿用Load判断前的名称。
            const auto currentScene = sceneServices_->CloneCurrentSceneNameAt414540();
            settingsRequested = _mbscmp(reinterpret_cast<const unsigned char*>(currentScene.Value().GetString()),
                reinterpret_cast<const unsigned char*>(kSettingsSceneNameCp932)) == 0;
        }
        if (settingsRequested)
        {
            sceneServices_->ClearCurrentSceneName();
            sceneServices_->PushActiveScene(kSettingsSceneNameCp932);
            return;
        }
    }

    menuDispatch_.Update();
}
}
