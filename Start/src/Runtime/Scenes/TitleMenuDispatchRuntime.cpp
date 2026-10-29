#include <Runtime/TitleMenuDispatchRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 18 号样本中的场景名和脚本名均由 CP932 窄字符串传入。
constexpr char kStartScript[] =
    "\x83\x58\x83\x5E\x81\x5B\x83\x67\x2E\x73\x63\x72";
constexpr char kScriptProcessingScene[] =
    "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char kAfterLoadSelectionScene[] =
    "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
constexpr char kCgModeScene[] =
    "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char kScenePlaybackModeScene[] =
    "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";
constexpr char kMusicAppreciationModeScene[] =
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
}

TitleMenuDispatchRuntime::TitleMenuDispatchRuntime(
    ITitleDialogRuntime& dialog,
    ITitleMenuRuntimeServices& services) noexcept
    : dialog_(&dialog), services_(&services)
{
}

void TitleMenuDispatchRuntime::Update()
{
    if (dialog_->Run())
        return;

    switch (dialog_->SelectionResult())
    {
    case 0u:
        // 4BB2A4..4BB2E9：原函数不检查脚本装载结果，并按此顺序写两个根对象字节。
        static_cast<void>(services_->LoadScript(kStartScript, -1));
        services_->QueueScene(kScriptProcessingScene);
        services_->SetRootByteAt218(1u);
        services_->SetRootByteAt219(1u);
        break;

    case 1u:
        // 选择读取存档时，原函数先清空 SceneController，再排队后处理场景。
        services_->ClearSceneController();
        services_->QueueScene(kAfterLoadSelectionScene);
        break;

    case 2u:
        services_->QueueScene(kCgModeScene);
        break;

    case 3u:
        services_->QueueScene(kScenePlaybackModeScene);
        break;

    case 4u:
        services_->QueueScene(kMusicAppreciationModeScene);
        break;

    default:
        break;
    }
}
}
