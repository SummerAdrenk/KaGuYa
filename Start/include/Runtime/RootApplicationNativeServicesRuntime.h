#pragma once

#include <Runtime/RootApplicationLifecycleNativeSource.h>
#include <Runtime/RootDrawServicesRuntime.h>
#include <Runtime/RootSceneAuxiliaryNativeServicesRuntime.h>
#include <Runtime/RootSceneNativeServicesRuntime.h>
#include <Runtime/RootModalDialogNativeServicesRuntime.h>
#include <Runtime/RootEditWindowNativeServicesRuntime.h>
#include <Runtime/RootScriptNativeExecutionRuntime.h>
#include <Runtime/DrawManagerSpriteResourceNative.h>
#include <Runtime/AnimationFormatNativePictures.h>
#include <Runtime/MessageWindowConfigExpressionParser.h>
#include <Runtime/WindowModeCommandsRuntime.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/WindowInputUpdateRuntime.h>
#include <Runtime/RootPreSceneEngineUpdateRuntime.h>
#include <Runtime/RootFrameRuntime.h>
#include <Runtime/MainFrameNativeCallbacks.h>

namespace KaGuYa::Start::Runtime
{
// 冷装配只借已活来源；三个文件域显式指定，不把封包、存档和已读记录悄悄合并。
struct RootApplicationNativeServiceDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    NativeSoundRuntimeLibrary& sound;
    IStartFileApi& archiveFiles;
    IStartFileApi& persistenceFiles;
    IStartFileApi& messageFiles;
    ICrtRandomSource& random;
    const SaveFileFormatProfile& saveProfile;
    ArchiveCodecServices codecs;
    const ToolDialogNativeRelation& toolRelations;
    const CountDownWindowResourceProfile& countdownProfile;
    WindowClassModuleNativeBody38h& nativeBase;
    DialogEndpointNativeServices endpoint;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    const FileTransactionContext* archiveTransaction;
    const FileTransactionContext* persistenceTransaction;
    const CountDownWindowNativeResourceProfile* nativeCountdownProfile = nullptr;
    const CountDownWindowFiveFrameResourceProfile* fiveFrameCountdownProfile = nullptr;
};

// 三生命周期装配审计第5节的生产长图；没有测试Scene/固定cue/固定font替身。
// 不拥有Root正文或进程singleton，不Ensure、不加载DLL、不做I/O。
// 本集合须覆盖根最终退役及逃逸Ref；Process使用显式存储，不登记自动析构。
class RootApplicationNativeServicesRuntime final
{
public:
    explicit RootApplicationNativeServicesRuntime(RootApplicationNativeServiceDependencies);
    RootApplicationNativeServicesRuntime(const RootApplicationNativeServicesRuntime&) = delete;
    RootApplicationNativeServicesRuntime& operator=(const RootApplicationNativeServicesRuntime&) = delete;
    RootApplicationLifecycleNativeSource& Lifecycle() noexcept { return lifecycle_; }
    RootDrawServicesRuntime& Draws() noexcept { return draws_; }
    RootSceneNativeServicesRuntime& Scenes() noexcept { return scenes_; }
    RootSceneAuxiliaryNativeServicesRuntime& Auxiliary() noexcept { return auxiliary_; }
    RootScriptNativeExecutionRuntime& Execution() noexcept { return execution_; }
    RootEditWindowNativeServicesRuntime& Edits() noexcept { return edits_; }
    GuiControlResourceLoadRuntime& Pictures() noexcept { return pictures_; }
    RootDrawManagerSpriteResourceNative& Sprites() noexcept { return sprites_; }
    FileInfoProgressNativeServices& Progress() noexcept { return progress_; }
    NativeGraphicsRuntimeLibrary& Graphics() noexcept { return graphics_; }
    IScrVmCommandErrorSink& ScriptErrors() noexcept { return scriptErrors_; }
    IModalDialogRunnerServices& Modal() noexcept { return modal_.Services(); }
    RootFrameRuntime& Frame() noexcept { return frame_; }
    RootMainFrameNativeIdleSource& Idle() noexcept { return idle_; }
private:
    NativeGraphicsRuntimeLibrary& graphics_;
    ArchiveReaderMemoryApi readers_;
    RootArchiveResourceSource archives_;
    ParamsResourceNameRuntime names_;
    MessageWindowConfigExpressionParser parser_;
    MessageWindowNativeConfigExpressionAdapter expressions_;
    EnvironmentInvalidOpcodeRuntime scriptErrors_;
    RootGraphicsResourceErrorDisplayNative resourceErrors_;
    AnimationFormatNativePictureServices pictureServices_;
    RootDrawManagerSpriteResourceNative sprites_;
    GuiControlResourceMessageApi messages_;
    GuiControlResourceLoadRuntime pictures_;
    RootDrawServicesRuntime draws_;
    Platform::Win32MousePositionApi mousePosition_;
    WindowInputUpdateRuntime inputUpdate_;
    RootPreSceneEngineUpdateRuntime preScene_;
    RootFrameRuntime frame_;
    RootMainFrameNativeIdleSource idle_;
    RootModalDialogNativeServicesRuntime modal_;
    RootEditWindowNativeServicesRuntime edits_;
    Win32RootLogicalRectanglePlatformApi rectangles_;
    RootSceneAuxiliaryNativeServicesRuntime auxiliary_;
    RootScriptNativeExecutionRuntime execution_;
    Win32EnvironmentPathApi paths_;
    Win32WindowModePlatformApi windows_;
    Win32WindowModeCommandPlatformApi commands_;
    FileInfoProgressNativeSource progressSource_;
    FileInfoProgressNativeServices progress_;
    RootSceneNativeServicesRuntime scenes_;
    RootApplicationLifecycleNativeSource lifecycle_;
};
}
