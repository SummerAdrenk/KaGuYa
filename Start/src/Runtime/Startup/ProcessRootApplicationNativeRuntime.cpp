#include <Runtime/ProcessRootApplicationNativeRuntime.h>
#include <Runtime/RootApplicationNativeServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/RootCriticalSectionNative.h>
#include <Runtime/RootProcessRetirementNative.h>
#include <Runtime/ProcessEnvironmentRuntime.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/LoadSpriteErrorEnvironment.h>
#include <Runtime/SaveFileMagicNativeGlobals.h>
#include <Runtime/SaveFileMetadataLoader.h>
#include <Runtime/RootMainFrameSettingsServicesRuntime.h>
#include <Runtime/RootMainFrameNativeServicesRuntime.h>
#include <Runtime/CountDownWindowNativeResourceProfile.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/StartupResourceProfile.h>
#include <Platform/NativeTimerProcess.h>
#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
ToolDialogNativeRelation SelectedToolRelation()
{
    if (CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::Configured)
        return RequireConfiguredStartupResources(CompiledGameProfile().startupProfile).toolRelation;
    return {};
}
struct ProcessRuntime final
{
    Win32StartFileApi archiveFiles;
    Win32StartFileApi persistenceFiles;
    Win32StartFileApi messageFiles;
    NativeGraphicsRuntimeLibrary graphics;
    RootLoadSpriteErrorEnvironment codecEnvironment;
    ArchiveHuffmanErrorSource codecErrors;
    MsvcCrtRandomSource random;
    SaveFileFormatProfile saveProfile;
    ToolDialogNativeRelation toolRelations;
    // 只供旧显式接口的元数据引用；正式域不读取这些空串/零坐标，不作资源默认值。
    CountDownWindowResourceProfile unusedHostCountdownMetadata;
    RootRuntimeOwner root;
    RootApplicationNativeServicesRuntime application;
    RootMainFrameSettingsServicesRuntime settings;
    RootMainFrameNativeServicesRuntime frame;

    explicit ProcessRuntime(const CountDownWindowResourceProfile* countdown)
        : codecEnvironment(ProcessEnvironmentStartupRuntime().RootSource()), codecErrors(codecEnvironment),
          saveProfile(CreateNativeSaveFileFormatProfile(ProcessSaveFileMagicNative())), toolRelations(SelectedToolRelation()),
          root({.environment=ProcessEnvironmentStartupRuntime(), .params=ProcessParamsRuntimeRootOwner(),
              .codecs={ProcessArchiveCodecManagerSource(), codecErrors},
              .timerSource=Platform::ProcessNativeTimerSource(), .timerApi=Platform::ProcessNativeTimerPlatformApi(),
              .resourceInstance=ProcessAtlBaseModuleNative().resourceInstanceAt08,
              .files=persistenceFiles, .random=random, .saveProfile=saveProfile},
              ProcessRootCriticalSectionNativeReference()),
          application({.root=root, .environment=ProcessEnvironmentStartupRuntime(),
              .params=ProcessParamsRuntimeRootOwner(), .input=ProcessNativeInputRuntime(),
              .graphics=graphics, .sound=ProcessSoundRuntimeLibrary(), .archiveFiles=archiveFiles,
              .persistenceFiles=persistenceFiles, .messageFiles=messageFiles, .random=random,
              .saveProfile=saveProfile, .codecs={ProcessArchiveCodecManagerSource(), codecErrors},
              .toolRelations=toolRelations, .countdownProfile=countdown ? *countdown : unusedHostCountdownMetadata,
              .nativeBase=ProcessAtlBaseModuleNative(), .endpoint=DefaultDialogEndpointNativeServices(),
              .strings=DefaultRuntimeStringManagerSource(), .memory=DefaultEnvironmentRootMemory(),
              .archiveTransaction=nullptr, .persistenceTransaction=nullptr,
              .nativeCountdownProfile=countdown || CompiledGameProfile().fiveFrameCountdown
                  ? nullptr : &Sample18CountDownWindowNativeResourceProfile(),
              .fiveFrameCountdownProfile=countdown ? nullptr : CompiledGameProfile().fiveFrameCountdown}),
          settings({.root=root, .environment=ProcessEnvironmentStartupRuntime(), .params=ProcessParamsRuntimeRootOwner(),
              .input=ProcessNativeInputRuntime(), .graphics=graphics, .sound=ProcessSoundRuntimeLibrary(),
              .draws=application.Draws(), .errors=application.ScriptErrors(), .module=ProcessAtlBaseModuleNative(),
              .endpoint=DefaultDialogEndpointNativeServices(), .messageFiles=messageFiles,
              .bitmapResourceModule=ProcessAtlBaseModuleNative().instanceAt04,
              .strings=DefaultRuntimeStringManagerSource(), .memory=DefaultEnvironmentRootMemory()}),
          frame({root, ProcessEnvironmentStartupRuntime(), ProcessParamsRuntimeRootOwner(), ProcessNativeInputRuntime(),
              ProcessSoundRuntimeLibrary(), application, settings.Command(), ProcessAtlBaseModuleNative(),
              DefaultDialogEndpointNativeServices(), DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()})
    {
        // 原事务不存在；三个正式文件域直连Win32，未开启现代journal或替代路径。
        // Root中的按值resourceInstance只保留旧显式接口；native生命周期使用ATL+08借槽。
    }
};
// 不用static ProcessRuntime：自动析构会先结束服务，再补删原退出回调后的合法晚建根。
alignas(ProcessRuntime) constinit std::byte processRuntimeStorage[sizeof(ProcessRuntime)]{};
ProcessRuntime& Current() noexcept
{ return *std::launder(reinterpret_cast<ProcessRuntime*>(processRuntimeStorage)); }
}
RootApplicationNativeServicesRuntime& ConstructProcessRootApplicationNativeRuntimeAtEntry(
    const CountDownWindowResourceProfile& countdown)
{
    auto* runtime = ::new (static_cast<void*>(processRuntimeStorage)) ProcessRuntime(&countdown);
    BindProcessRootRuntimeOwnerMetadata(runtime->root);
    return runtime->application;
}
RootApplicationNativeServicesRuntime& ConstructProcessRootApplicationNativeRuntimeAtEntry()
{
    auto* runtime = ::new (static_cast<void*>(processRuntimeStorage)) ProcessRuntime(nullptr);
    BindProcessRootRuntimeOwnerMetadata(runtime->root);
    return runtime->application;
}
RootRuntimeOwner& ProcessRootRuntimeOwner() noexcept { return Current().root; }
RootApplicationNativeServicesRuntime& ProcessRootApplicationNativeServices() noexcept { return Current().application; }
RootMainFrameNativeServicesRuntime& ProcessRootMainFrameNativeServices() noexcept { return Current().frame; }
}
