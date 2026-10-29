#pragma once
#include <Runtime/SceneManagerView.h>
#include <Runtime/InformationDisplayView.h>

#include <Runtime/ArchiveCodecServices.h>

#include <Runtime/NativeStartupSettingsDialogRuntime.h>
#include <Platform/Input.h>
#include <Runtime/ProcessInputRuntime.h>
#include <Runtime/FontManagerRuntime.h>
#include <Runtime/WaitMarkServicesRuntime.h>
#include <Runtime/LayerWindowDrawStateRuntime.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class IRootLogicalRectangleSynchronization;
class EnvironmentStartupRuntime;
class EnvironmentPathRuntime;
class FileManagerRootRuntime;
class FileManagerNative28h;
class IFileManagerStartupFactory;
class ParamsRuntimeRootOwner;
class IScrVmProgramRandSource;
struct PreWindowLifecycleState;
struct FileTransactionContext;
struct DrawLayersNativeCallServices;
struct SaveFileFormatProfile;
class IGraphicsRuntimeCalls;
class NativeGraphicsRuntimeLibrary;
class ISoundRuntimeLibrary;
struct SoundManagerNativeBody50h;
class SoundManagerNativeRuntime;
class ISoundManagerSavedParamsSource;
class ScriptMusicManagerReceiver;
class ScriptVoiceManagerReceiver;
class ScriptSoundManagerReceiver;
class SoundMappingSlotReceiver;
class DemoSoundManagerReceiver;
class ReadScriptSoundWaitReceiver;
struct TextSoundManagerReceiver;
class ResourceCatalog;
class ISoundEffectOpenGate;
class ISoundFirstEffectOpenModeSource;
class SceneManagerRuntime;
class ISceneConstructionServices;
class ISceneManagerPublicDialogFactory;
class RootFrameControlRuntime;
class IWindowModeCommandPlatformApi;
class IFileInfoProgressConfirmation;
class ICrtRandomSource;
class DrawManagerRuntime;
struct DrawManagerNativeBody2Ch;
struct PostWindowNativeLayerServices;
struct LayerWindowNativeServices;
class DrawLayersView;
class DrawTransitionView;
class IGraphicsTransitApi;
class SystemInformationDisplayRuntime;
class ISystemMenuDialogServices;
struct GuiModalDialogDependencies;
class ILayerWindowPatternLayerFactory;
class ILayerWindowDrawState;
class ILayerWindowEffectServices;
class IMessageWindowConstructionServices;
class IMessageWindowLayerState;
class IMessageWindowConfigExpressionParser;
class IColorTransWindowResourceLoader;
class IToolWindowDialogFactory;
class IToolWindowReplaySource;
class ScrVmValueTable;
class ScrVmExecutableProfile;
class ITextWindowConstructionServices;
class IGraphicsWindowSurfaceApi;
class IGraphicsSurfaceApi;
class IWaitMarkResourceLoader;
class IWaitMarkStateSource;
class IWaitMarkRenderer;
class ISelectWindowConstructionServices;
class ICgOperationSurfaceFactory;
class IGraphicsSurfaceCompositionApi;
class ICgOperationSliderInputSource;
class IOpacityDialogResetStateServices;
struct CountDownWindowDependencies;
struct CountDownWindowResourceProfile;

// 489C90 的后窗口对象持有与装配边界；借用同一环境，不创建第二套归档。
// 完整生产 root 仍须提供所有具体场景和图层服务，不能用空工厂替代。
class EnvironmentPostWindowStartupRuntime final : public INativeStartupSettingsDialogServices
{
public:
    EnvironmentPostWindowStartupRuntime(EnvironmentStartupRuntime& environment,
        ParamsRuntimeRootOwner& params, FileManagerRootRuntime& fileManager, SoundManagerNativeRuntime& soundServices,
        IRootLogicalRectangleSynchronization& rootSynchronization,
        YtlReferenceSlotRuntime& selectedRootSlot,
        ArchiveCodecServices codecs, PreWindowLifecycleState& rootState, HINSTANCE resourceInstance,
        IStartFileApi& files, IScrVmProgramRandSource& random,
        const SaveFileFormatProfile& saveProfile,
        const FileTransactionContext* transaction = nullptr,
        YtlReferenceSlotRuntime* rootFontReference = nullptr,
        YtlReferenceSlotRuntime* rootSoundReference = nullptr,
        Platform::IInputStartupRuntime& input = ProcessNativeInputRuntime(),
        YtlReferenceSlotRuntime* rootDrawReference = nullptr,
        YtlReferenceSlotRuntime* rootSceneReference = nullptr,
        YtlReferenceSlotRuntime* rootInformationReference = nullptr,
        SystemDialogNativeServices** rootInformationDialog = nullptr) noexcept;
    ~EnvironmentPostWindowStartupRuntime();

    void ConfirmFirstInstance();
    void CreateFileManager();
    void CreateFileManager(IFileManagerStartupFactory& factory);
    // 根 getter 完成后才转发同一唯一 owner 的当前发布槽；不缓存目录。
    [[nodiscard]] FileManagerNative28h* FileManagerAt00() const noexcept;
    void VerifyStartupFileSet();
    void InstallEnvironmentKeyTable();
    void LoadSecretEnvironment();
    void ScanSaveFileCatalog();
    void LoadGlobalEnvironment();

    void InitializeGraphicsLibrary(NativeGraphicsRuntimeLibrary& graphics);
    void InitializeGraphicsLibrary(NativeGraphicsRuntimeLibrary& graphics, IFileInfoProgressConfirmation& messages);
    void CreateDrawManager(ICrtRandomSource& random, IGraphicsRuntimeCalls& graphics);
    void CreateControlledInformationDisplay(GuiModalDialogDependencies dependencies, ISystemMenuDialogServices& services);
    void CreateInformationDisplay(SystemInformationDisplayNativeServices& services);
    void BindInformationDisplayNativeServices(SystemInformationDisplayNativeServices& services) noexcept;
    // 先退役父及全部逃逸Ref，再结束服务借用；不改变原拥有槽或切换到受控域。
    void ClearInformationDisplayNativeServices() noexcept{informationServices_=nullptr;informationDialog_=nullptr;}
    // 八层由PostWindowNativeLayers按原捕获点创建；服务只借用，须长于图层和逃逸Ref。
    void BindNativeDrawServices(PostWindowNativeLayerServices&, LayerWindowNativeServices&,
        DrawLayersNativeWindowServices&, DrawGuiNativeServices&) noexcept;
    // 调用方结束借用前必须先释放Draw；这里只解除装配，不改变原Ref或正文。
    void ClearNativeDrawServices() noexcept;
    [[nodiscard]] PostWindowNativeLayerServices& NativeLayerServices() const;
    [[nodiscard]] DrawLayersNativeCallServices BorrowNativeDrawCallServices() const noexcept;
    void InitializeSoundLibrary(ISoundRuntimeLibrary& sound);
    void InitializeSoundLibrary(ISoundRuntimeLibrary& sound, IFileInfoProgressConfirmation& messages);
    void CreateSoundManager();
    void CreateSoundManager(ISoundManagerSavedParamsSource& source);
    void ReleaseSoundManager() noexcept;
    [[nodiscard]] SoundManagerNativeBody50h* SoundManagerAt24() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& SoundManagerSlotAt24() noexcept { return soundManager_; }
    [[nodiscard]] SoundManagerNativeRuntime& SoundServices() noexcept { return soundServices_; }
    // 只解析已捕获root的当前+24，不Ensure/分配或创建第二个manager。
    [[nodiscard]] ScriptMusicManagerReceiver MusicReceiverAt24() const noexcept;
    [[nodiscard]] ScriptVoiceManagerReceiver VoiceReceiverAt24() const noexcept;
    [[nodiscard]] ReadScriptSoundWaitReceiver SoundWaitReceiverAt24() const noexcept;
    [[nodiscard]] TextSoundManagerReceiver TextSoundReceiverAt24() const noexcept;
    [[nodiscard]] ScriptSoundManagerReceiver SoundLeafReceiverAt24() const noexcept;
    [[nodiscard]] SoundMappingSlotReceiver SoundMappingReceiverAt24() const noexcept;
    [[nodiscard]] DemoSoundManagerReceiver DemoSoundReceiverAt24() const noexcept;
    void CreateSceneManager(ISceneConstructionServices& scenes, ISceneManagerPublicDialogFactory& dialogs);
    // 原生整组入口：候选完成后才接原48h/Ref；与显式既有受控入口不同，不失败换路。
    void CreateSceneManager(SceneManagerNativeServices&);
    void ReleaseSceneManager() noexcept;
    // 所有Scene/Manager逃逸Ref先退役后，局部宿主才可解除服务借用。
    // 只移除正文外元数据，不补释放、不切换到受控域；再次使用前必须重新装配。
    void ClearSceneManagerNativeServices() noexcept{sceneServices_=nullptr;}
    [[nodiscard]] SceneManagerView BorrowSceneManagerAt30() const noexcept;
    [[nodiscard]] SceneManagerNativeServices* BorrowSceneManagerNativeServices()const noexcept{return sceneServices_;}
    // 只重绑原48h之外的长期服务；caller须保证旧/新服务覆盖其各自全部逃逸引用。
    void BindSceneManagerNativeServices(SceneManagerNativeServices& services)noexcept{sceneServices_=&services;}
    [[nodiscard]] YtlReferenceSlotRuntime& SceneManagerSlotAt30() noexcept{return sceneReference_;}
    // 仅显式既有域查询；原生消费者使用BorrowSceneManagerAt30，不能把48h转为现代类。
    [[nodiscard]] SceneManagerRuntime* SceneManagerAt30() const noexcept { return publishedSceneManager_; }
    void ConfigureFrameTimer(RootFrameControlRuntime& frame);
    void ResetRuntimeState(RootFrameControlRuntime& frame);
    void QueueTitleLogoScene();
    void PostInitialWindowModeCommand(IWindowModeCommandPlatformApi& platform);
    void CreateFontManager();
    void CreateFontManager(IFontResourcePlatformApi& platform);
    void ReleaseFontManager() noexcept;
    [[nodiscard]] FontManagerNativeBody1Ch* FontManagerAt60() const noexcept
        { return ResolveFontManagerReference(fontManager_); }
    [[nodiscard]] YtlReferenceSlotRuntime& FontManagerSlotAt60() noexcept { return fontManager_; }
    void CreateInputAggregate();
    void ReleaseInputAggregate() noexcept;
    [[nodiscard]] Platform::IInputRuntime& Input() noexcept { return input_.Input(); }
    [[nodiscard]] Platform::IInputStartupRuntime& InputLifecycle() noexcept { return input_; }
    void ReleaseDrawManager();
    void ReleaseDrawManagerFinal() noexcept;
    void ReleaseInformationDisplay() noexcept;
    [[nodiscard]] DrawManagerNativeBody2Ch* DrawManagerAt18() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& DrawManagerSlotAt18() noexcept { return drawManager_; }
    // 只选择当前+18；消费者借用统一view，不再依赖默认producer的现代类。
    [[nodiscard]] DrawLayersView DrawLayersAt18() const noexcept;
    [[nodiscard]] DrawTransitionView DrawTransitionAt18(IGraphicsTransitApi&) const noexcept;
    // 仅历史显式受控域查询；生产消费者借BorrowInformationDisplayAt3C，不强转原14h为本类。
    [[nodiscard]] SystemInformationDisplayRuntime* InformationDisplayAt3C() const noexcept { return publishedInformation_; }
    [[nodiscard]] InformationDisplayView BorrowInformationDisplayAt3C() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& InformationDisplaySlotAt3C() noexcept{return informationReference_;}

    // 此处不释放 progress；它必须继续存活到完整 489C90 成功或 EH 退出。
    void InitializeThroughGlobal();
    void InitializeThroughGlobal(IFileManagerStartupFactory& factory);
    void ReleaseInitializationProgress() noexcept;
    void PersistEnvironmentState();
    void ReleaseFileManager();

    [[nodiscard]] SaveDataEnvironmentArchive& SaveDataAtSlot1() override;
    [[nodiscard]] const RuntimeCString& LibraryPathAt18After413BE0() override;

private:
    [[nodiscard]] EnvironmentPathRuntime& Paths() const;
    void InitializeAfterFileManager();

    IRootLogicalRectangleSynchronization& rootSynchronization_;
    EnvironmentStartupRuntime& environment_;
    ParamsRuntimeRootOwner& params_;
    FileManagerRootRuntime& fileManager_;
    YtlReferenceSlotRuntime& selectedFileManagerSlot_;
    // 两个依赖只借用；与Archive/PL同源，不在PostWindow建立独立decoder拥有链。
    ArchiveCodecServices codecs_;
    PreWindowLifecycleState& rootState_;
    HINSTANCE resourceInstance_;
    IStartFileApi& files_;
    IScrVmProgramRandSource& random_;
    const SaveFileFormatProfile& saveProfile_;
    const FileTransactionContext* transaction_;
    // 4640E0 的父对象析构顺序：InformationDisplay 先于 DrawManager。
    YtlReferenceSlotRuntime independentDrawManager_;
    YtlReferenceSlotRuntime& drawManager_;
    IGraphicsRuntimeCalls* drawGraphics_ = nullptr;
    PostWindowNativeLayerServices* nativeLayers_ = nullptr;
    LayerWindowNativeServices* nativeLayerRuntime_ = nullptr;
    DrawLayersNativeWindowServices* nativeWindowEffects_ = nullptr;
    DrawGuiNativeServices* nativeGui_ = nullptr;
    std::unique_ptr<SystemInformationDisplayRuntime> information_;
    // 原 root base 在旧正文析构返回后才清空；不能把 unique_ptr::reset 的提前置空暴露给回调。
    SystemInformationDisplayRuntime* publishedInformation_ = nullptr;
    YtlReferenceSlotRuntime independentInformationReference_;
    YtlReferenceSlotRuntime& informationReference_;
    SystemDialogNativeServices* independentInformationDialog_=nullptr;
    SystemDialogNativeServices*& informationDialog_;
    SystemInformationDisplayNativeServices* informationServices_=nullptr;
    bool nativeInformationSelected_=true;
    // 418210独立惰性singleton；PostWindow只借外观，不析构或复制输入正文。
    Platform::IInputStartupRuntime& input_;
    // 独立工具只拥有本地Ref；正式PostWindow借root+24，没有第二个manager owner。
    YtlReferenceSlotRuntime independentSoundManager_;
    YtlReferenceSlotRuntime& soundManager_;
    SoundManagerNativeRuntime& soundServices_;
    std::unique_ptr<SceneManagerRuntime> sceneManager_;
    SceneManagerRuntime* publishedSceneManager_ = nullptr;
    YtlReferenceSlotRuntime independentSceneReference_;
    YtlReferenceSlotRuntime& sceneReference_;
    SceneManagerNativeServices* sceneServices_=nullptr;
    bool nativeSceneSelected_=false;
    Win32FontResourcePlatformApi fontPlatform_;
    // 独立装配器拥有自己的原Ref；根装配时只借root+60，不形成第二个owner。
    YtlReferenceSlotRuntime independentFontManager_;
    YtlReferenceSlotRuntime& fontManager_;
    IFontResourcePlatformApi* fontServices_;
};
}
