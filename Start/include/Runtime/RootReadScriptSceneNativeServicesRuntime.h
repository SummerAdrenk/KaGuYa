#pragma once
#include <Runtime/ReadScriptSceneNativeProductionSource.h>
#include <Runtime/ReadScriptSceneRestoreNative.h>
#include <Runtime/RootReadScriptYieldNative.h>
#include <Runtime/RootScriptRecoveryRuntime.h>
#include <Runtime/RootReadScriptInteractionNativeHost.h>
#include <Runtime/RootReadScriptReturnTitleNative.h>
#include <Runtime/RootReadScriptToolNativeServices.h>
#include <Runtime/RootReadScriptTextNativeServices.h>
#include <Runtime/RootReadScriptMenuNativeServices.h>
#include <Runtime/RootQuickOperationsNativeSource.h>
#include <Runtime/ReadScriptInteractionStateSource.h>
#include <Runtime/ReadScriptWaitStateSource.h>
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <Runtime/ReadScriptEffectMaskNative.h>
#include <Runtime/ReadScriptPacketEffects.h>
#include <Runtime/MoveEffectNativeConstruction.h>
#include <Runtime/FreeScrollNativeRuntime.h>
#include <Runtime/PatternResourceLoadRuntime.h>
#include <Runtime/SelectWindowNativeWorkflow.h>

namespace KaGuYa::Start::Runtime
{
class RootDrawServicesRuntime;
class NativeInputRuntime;
// 各业务依赖必须来自同一Root。构造不捕获原正文；所有外部依赖须活过最后一个BCh Ref。
struct RootReadScriptSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeGraphicsRuntimeLibrary& graphics;
    RootDrawServicesRuntime& draws;
    IRestoredScriptNativeExecution& execution;
    RootSaveLoadNativeSource& persistence;
    GeneralConfirmDialogNativeServices& confirmation;
    ISavePreviewCaptureServices& preview;
    IStartFileApi& messageFiles;
    const IMessageEnvironmentPathSource& messagePaths;
    IScrVmCommandErrorSink& errors;
    IFileInfoProgressConfirmation& voiceErrors;
    const char* const& fontFaceAt619824;
    const IEnvironmentKeyTableSource& key;
    IArchiveReaderMemoryApi& readers;
    ArchiveCodecServices codecs;
    const FileTransactionContext* transaction=nullptr;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
    decltype(&::DeleteFileA) deleteFile=&::DeleteFileA;
};

// 只拥有原生服务适配器；原BCh/CCh/E8/Ref、Root及DLL均由外层owner持有。
// 不使用旧ReadScriptSceneContextRuntime的第二份scalar/effects/scroll状态。
class RootReadScriptSceneNativeServicesRuntime final
{
public:
    explicit RootReadScriptSceneNativeServicesRuntime(RootReadScriptSceneNativeDependencies)noexcept;
    RootReadScriptSceneNativeServicesRuntime(const RootReadScriptSceneNativeServicesRuntime&)=delete;
    RootReadScriptSceneNativeServicesRuntime& operator=(const RootReadScriptSceneNativeServicesRuntime&)=delete;
    ReadScriptSceneNativeServices& Services()noexcept{return services_;}
    ReadScriptSceneNativeProductionSource& Source()noexcept{return source_;}
    SelectWindowNativeWorkflowServices& SelectionWorkflow()noexcept{return selection_;}
private:
    class Layers final:public IDrawLayersViewSource
    {
    public:
        explicit Layers(RootRuntimeOwner& root)noexcept:root_(root){}
        DrawLayersView CaptureDrawLayersAt413BE0()override;
    private:
        RootRuntimeOwner& root_;
    } layers_;
    RootScriptRecoveryRuntime recovery_;
    RootReadScriptYieldNative yield_;
    NativeDrawTransitionViewSource transition_;
    ReadScriptPacketNativeServices packets_;
    RootReadScriptPacketEnvironmentSource packetEnvironment_;
    ReadScriptFlashShakeNativeServices flashCalls_;
    ReadScriptEffectFactoryNativeServices effects_;
    RootArchiveResourceSource archives_;
    ReadScriptEffectMaskNative masks_;
    RootMoveEffectNativeConstructionSource moves_;
    ReadScriptFlashShakeNativeSource flash_;
    RootReadScriptWaitStateSource waits_;
    RootReadScriptSoundWaitSource soundWait_;
    RootReadScriptInteractionStateSource interaction_;
    RootQuickFileNativeSource files_;
    RootQuickOperationsNativeServicesRuntime quick_;
    AutoSaveOperationsNativeServices automatic_;
    RootReadScriptInteractionNativeRuntime host_;
    RootReadScriptReturnTitleNativeRuntime returnTitle_;
    RootReadScriptToolNativeServices tools_;
    RootReadScriptTextNativeServices text_;
    EnvironmentTextWindowSoundSource sound_;
    RootSelectWindowNativeWorkflowSource choices_;
    SelectWindowNativeWorkflowServices selection_;
    RootReadScriptMenuNativeServices menu_;
    RootReadScriptMenuNativeServices commandMenu_;
    RootStartupConfigGameSystemSource startup_;
    FreeScrollNativeCanvasCopy canvasCopy_;
    FreeScrollNativeDrawServices scrollDrawing_;
    FreeScrollNativeInputServices scrollInput_;
    PatternResourceNativeInitializationSource animation_;
    ReadScriptSceneNativeProductionSource source_;
    RootReadScriptSceneRestoreSource restore_;
    ReadScriptSceneNativeServices services_;
};
}
