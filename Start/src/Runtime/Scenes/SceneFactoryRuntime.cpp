#include <Runtime/SceneFactoryRuntime.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kTitleLogoCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x8D\x83\x53";
constexpr char kTitleCp932[] =
    "\x83\x5E\x83\x43\x83\x67\x83\x8B";
constexpr char kMusicAppreciationModeCp932[] =
    "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";
constexpr char kCgDisplayModeCp932[] =
    "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";
constexpr char kScenePlaybackModeCp932[] =
    "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";
constexpr char kLogScreenCp932[] =
    "\x83\x8D\x83\x4F\x89\xE6\x96\xCA";
constexpr char kSaveScreenCp932[] =
    "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
constexpr char kLoadScreenCp932[] =
    "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";
constexpr char kLoadAfterSelectionCp932[] =
    "\x83\x8D\x81\x5B\x83\x68\x91\x49\x91\xF0\x8C\xE3\x8F\x88\x97\x9D";
constexpr char kScriptProcessingCp932[] =
    "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char kSettingsScreenCp932[] =
    "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
constexpr char kAffectionScreenCp932[] =
    "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA";
constexpr char kMoviePlaybackCp932[] =
    "\x83\x80\x81\x5B\x83\x72\x81\x5B\x8D\xC4\x90\xB6";
constexpr char kEndingCp932[] =
    "\x83\x47\x83\x93\x83\x66\x83\x43\x83\x93\x83\x4F";
constexpr char kTextDisplayProcessingCp932[] =
    "\x83\x65\x83\x4C\x83\x58\x83\x67\x95\x5C\x8E\xA6\x8F\x88\x97\x9D";
constexpr char kProgramSelectionProcessingCp932[] =
    "\x83\x76\x83\x8D\x83\x4F\x83\x89\x83\x80\x91\x49\x91\xF0\x8F\x88\x97\x9D";
constexpr char kGraffitiPageProcessingCp932[] =
    "\x97\x8E\x8F\x91\x82\xAB\x83\x79\x81\x5B\x83\x57\x8F\x88\x97\x9D";
}

SceneConstructionServicesForwarderRuntime::
SceneConstructionServicesForwarderRuntime(
    ISceneConstructionServices& upstream) noexcept
    : upstream_(&upstream)
{
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateTitleLogo()
{
    return upstream_->CreateTitleLogo();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateTitle()
{
    return upstream_->CreateTitle();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateMusicAppreciationMode()
{
    return upstream_->CreateMusicAppreciationMode();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateCgDisplayMode()
{
    return upstream_->CreateCgDisplayMode();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateScenePlaybackMode()
{
    return upstream_->CreateScenePlaybackMode();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateLogScreen()
{
    return upstream_->CreateLogScreen();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateSaveScreen()
{
    return upstream_->CreateSaveScreen();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateLoadScreen()
{
    return upstream_->CreateLoadScreen();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateLoadAfterSelection()
{
    return upstream_->CreateLoadAfterSelection();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateScriptProcessing()
{
    return upstream_->CreateScriptProcessing();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateSettingsScreen()
{
    return upstream_->CreateSettingsScreen();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateAffectionScreen()
{
    return upstream_->CreateAffectionScreen();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateMoviePlayback()
{
    return upstream_->CreateMoviePlayback();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateEnding()
{
    return upstream_->CreateEnding();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateTextDisplayProcessing()
{
    return upstream_->CreateTextDisplayProcessing();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateProgramSelectionProcessing()
{
    return upstream_->CreateProgramSelectionProcessing();
}

std::shared_ptr<ISceneTask>
SceneConstructionServicesForwarderRuntime::CreateGraffitiPageProcessing()
{
    return upstream_->CreateGraffitiPageProcessing();
}

ISceneConstructionServices&
SceneConstructionServicesForwarderRuntime::Upstream() const noexcept
{
    return *upstream_;
}

SceneFactoryRuntime::SceneFactoryRuntime(ISceneConstructionServices& services) noexcept
    : services_(&services)
{
}

std::shared_ptr<ISceneTask> SceneFactoryRuntime::Create(
    const char* const sceneNameCp932)
{
    // 4AAF90 使用严格窄字节比较，没有别名、正规化或未知项兜底。
    if (sceneNameCp932 != nullptr)
    {
        if (std::strcmp(sceneNameCp932, kTitleLogoCp932) == 0)
            return services_->CreateTitleLogo();
        if (std::strcmp(sceneNameCp932, kTitleCp932) == 0)
            return services_->CreateTitle();
        if (std::strcmp(sceneNameCp932, kMusicAppreciationModeCp932) == 0)
            return services_->CreateMusicAppreciationMode();
        if (std::strcmp(sceneNameCp932, kCgDisplayModeCp932) == 0)
            return services_->CreateCgDisplayMode();
        if (std::strcmp(sceneNameCp932, kScenePlaybackModeCp932) == 0)
            return services_->CreateScenePlaybackMode();
        if (std::strcmp(sceneNameCp932, kLogScreenCp932) == 0)
            return services_->CreateLogScreen();
        if (std::strcmp(sceneNameCp932, kSaveScreenCp932) == 0)
            return services_->CreateSaveScreen();
        if (std::strcmp(sceneNameCp932, kLoadScreenCp932) == 0)
            return services_->CreateLoadScreen();
        if (std::strcmp(sceneNameCp932, kLoadAfterSelectionCp932) == 0)
            return services_->CreateLoadAfterSelection();
        if (std::strcmp(sceneNameCp932, kScriptProcessingCp932) == 0)
            return services_->CreateScriptProcessing();
        if (std::strcmp(sceneNameCp932, kSettingsScreenCp932) == 0)
            return services_->CreateSettingsScreen();
        if (std::strcmp(sceneNameCp932, kAffectionScreenCp932) == 0)
            return services_->CreateAffectionScreen();
        if (std::strcmp(sceneNameCp932, kMoviePlaybackCp932) == 0)
            return services_->CreateMoviePlayback();
        if (std::strcmp(sceneNameCp932, kEndingCp932) == 0)
            return services_->CreateEnding();
        if (std::strcmp(sceneNameCp932, kTextDisplayProcessingCp932) == 0)
            return services_->CreateTextDisplayProcessing();
        if (std::strcmp(sceneNameCp932, kProgramSelectionProcessingCp932) == 0)
            return services_->CreateProgramSelectionProcessing();
        if (std::strcmp(sceneNameCp932, kGraffitiPageProcessingCp932) == 0)
            return services_->CreateGraffitiPageProcessing();
    }

    // 4AB8F5：未知名称以整数 0 离开，不返回空替代场景。
    throw 0;
}
}
