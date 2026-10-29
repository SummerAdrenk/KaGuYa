#pragma once
#include <Runtime/CompleteSceneFactoryNativeConstructorsRuntime.h>
#include <Runtime/RootDemoSceneNativeServicesRuntime.h>
#include <Runtime/RootTitleSceneNativeSource.h>
#include <Runtime/RootTitleDialogNativeSource.h>
#include <Runtime/RootCgSceneServicesRuntime.h>
#include <Runtime/RootReplaySceneServicesRuntime.h>
#include <Runtime/RootMusicSceneServicesRuntime.h>
#include <Runtime/RootLogSceneServicesRuntime.h>
#include <Runtime/RootSaveLoadSceneServicesRuntime.h>
#include <Runtime/RootLoadAfterSelectionNativeSource.h>
#include <Runtime/RootRestoredScriptNativeServices.h>
#include <Runtime/RootReadScriptSceneNativeServicesRuntime.h>
#include <Runtime/RootSettingSceneServicesRuntime.h>
#include <Runtime/RootLikabilitySceneNativeSource.h>
#include <Runtime/RootMovieSceneNativeSource.h>
#include <Runtime/RootTextDisplayNativeSource.h>
#include <Runtime/RootProgramSceneNativeSource.h>
#include <Runtime/RootProgramDialogNativeSource.h>
#include <Runtime/RootGraffitiCountDialogNativeSource.h>
#include <Runtime/DrawTextRebuildServices.h>
#include <Runtime/TextWindowNativeFormat.h>
#include <Runtime/RootLegacyTitleServices.h>
#include <Runtime/LegacyMenuSelectionSource.h>
#include <Runtime/LegacyCharacterSelectionRootSource.h>
#include <Runtime/LegacyItemAcquisitionSource.h>
#include <Runtime/LegacyMapSelectionRootSource.h>

namespace KaGuYa::Start::Runtime
{
struct LegacyTitleRuntimeBinding final
{
    ILegacyTitleQuickLoad& quickLoad;
    const LegacyTitleSceneData& data;
};
// 外层Process拥有平台、Draw、持久化及模态服务；此集合统一所有Scene的Root/Env来源。
// 每个借用必须活过全部task/GUI/控制块的最后Ref；不能传另一代Root的依赖或测试链尾。
struct RootSceneNativeServicesDependencies final
{
    RootScenePublicNativeDependencies common;
    ParamsRuntimeRootOwner& params;
    NativeSoundRuntimeLibrary& sound;
    RootDrawServicesRuntime& draws;
    IGuiControlResourceLoadServices& pictures;
    ISpriteNativeResourceSource& sprites;
    IArchiveResourceSource& archives;
    IArchiveReaderMemoryApi& readers;
    IRestoredScriptNativeExecution& execution;
    const IScrResourceLookupSource& resourceNames;
    IScrVmCommandErrorSink& errors;
    RootSaveLoadNativeSource& persistence;
    SaveLoadPageFactoryNative& pages;
    SaveLoadMessageBoxNativeServices& deletionConfirmation;
    GeneralConfirmDialogNativeServices& generalConfirmation;
    NameRegistrationDialogNativeServices& nameRegistration;
    IModalDialogRunnerServices& modal;
    ISavePreviewCaptureServices& preview;
    const char* const& textFontFace;
    // 文字读标记可用独立文件源；不得与common.files的封包读取来源混并。
    IStartFileApi& messageFiles;
    const IMessageEnvironmentPathSource& messagePaths;
    IFileInfoProgressConfirmation& voiceErrors;
    IEnvironmentPathApi& paths;
    WinHookRootNativeOwner& hooks;
    WinHookNativeServices& hookServices;
    TitleDialogNativeCueNames titleCues;
    GraffitiCountCueSlots graffitiCues;
    IRuntimeWideStringNativeApi& wide=DefaultRuntimeWideStringNativeApi();
    ScriptNamedValueNativeTables namedValues=GameScriptNamedValueTables();
    const TextDisplayFontContextNative14h* textFonts=TextDisplayFontContextsSample18At602198();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
    decltype(&::DeleteFileA) deleteFile=&::DeleteFileA;
    // 必须是已经完成的旧异步确认/恢复服务；尚未装配时不启用旧入口。
    LegacyTitleRuntimeBinding* legacyTitle = nullptr;
    const HINSTANCE* legacyResourceInstance = nullptr;
};

// 15类真实Scene服务 -> 完整17分支factory -> Manager，按成员顺序冷装配。
// 不创建任何原生正文、不取根、不增加Ref、不替外层执行477620或完整Root释放。
// 外层先释放Manager/场景及全部逃逸Ref，再结束Post的借用，最后才销毁本集合。
class RootSceneNativeServicesRuntime final
{
public:
    explicit RootSceneNativeServicesRuntime(RootSceneNativeServicesDependencies)noexcept;
    RootSceneNativeServicesRuntime(const RootSceneNativeServicesRuntime&)=delete;
    RootSceneNativeServicesRuntime& operator=(const RootSceneNativeServicesRuntime&)=delete;
    SceneManagerNativeServices& ManagerServices()noexcept{return manager_.Services();}
    SceneFactoryNativeRuntime& Factory()noexcept{return factory_;}
    SceneFactoryNativeTaskServices TaskServices()const noexcept{return constructors_.TaskServices();}
    RootScenePublicNativeServicesRuntime& PublicServices()noexcept{return public_;}
private:
    RootScenePublicNativeServicesRuntime public_;
    RootScriptRecoveryRuntime recovery_;
    RootQuickFileNativeSource quickFiles_;
    RootQuickOperationsNativeServicesRuntime quick_;
    RootRestoredScriptNativeServices restored_;
    RootReadScriptWaitStateSource wait_;
    EnvironmentTextWindowSoundSource sounds_;
    RootArchiveResourceSource archives_;
    ReadScriptEffectMaskNative masks_;
    RootTextWindowNativeFormatSource format_;
    DrawTextNativeRebuildServices rebuild_;
    RootDemoSceneNativeServicesRuntime demo_;
    TitleNameRegistrationNativeConstruction nameConstruction_;
    RootTitleDialogNativeSource titleDialogSource_;
    TitleDialogNativeServices titleDialog_;
    TitleDialogSceneNativeConstruction titleConstruction_;
    RootTitleSceneNativeSource titleSource_;
    std::optional<RootLegacyTitleServices> legacyTitle_;
    TitleSceneNativeServices title_;
    RootMusicSceneServicesRuntime music_;
    RootCgSceneServicesRuntime cg_;
    RootReplaySceneServicesRuntime replay_;
    RootLogSceneServicesRuntime log_;
    RootSaveLoadSceneServicesRuntime saveLoad_;
    RootLoadAfterSelectionNativeSource loadAfterSource_;
    LoadAfterSelectionNativeServices loadAfter_;
    RootReadScriptSceneNativeServicesRuntime readScript_;
    RootSettingSceneServicesRuntime setting_;
    RootLikabilitySceneServicesRuntime likability_;
    RootMovieSceneServicesRuntime movie_;
    RootTextDisplayNativeSource textSource_;
    TextDisplayNativeServices text_;
    RootProgramDialogServicesRuntime programDialog_;
    RootProgramSceneServicesRuntime program_;
    RootGraffitiPageNativeSource graffitiSource_;
    RootGraffitiCountDialogNativeSource graffitiCountSource_;
    GraffitiCountDialogNativeServices graffitiCount_;
    GraffitiCountDialogNativeConstructionSource graffitiConstruction_;
    GraffitiPageNativeServices graffiti_;
    LegacyMenuSelectionRootSource menuSource_;
    LegacyMenuSelectionServices menuSelection_;
    LegacyCharacterSelectionRootSource characterSource_;
    LegacyCharacterSelectionServices characterSelection_;
    LegacyItemAcquisitionRootSource itemSource_;
    LegacyItemAcquisitionServices itemServices_;
    LegacyMapSelectionRootSource mapSource_;
    LegacyMapSelectionServices mapSelection_;
    CompleteSceneFactoryNativeConstructorsRuntime constructors_;
    SceneFactoryNativeRuntime factory_;
    RootSceneManagerNativeServicesRuntime manager_;
};
}
