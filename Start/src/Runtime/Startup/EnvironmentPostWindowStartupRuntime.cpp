#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/SoundRuntimeApi.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/ScriptMusicSource.h>
#include <Runtime/ScriptVoiceSource.h>
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/SoundMappingRestoreSource.h>
#include <Runtime/DemoSoundManagerReceiver.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/RootFrameControlRuntime.h>
#include <Runtime/WindowModeCommandsRuntime.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentPersistenceLifecycle.h>
#include <Runtime/EnvironmentPostWindowPersistenceNative.h>
#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/FileManagerStartupErrorReport.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RootLogicalRectangleRuntime.h>
#include <Runtime/SaveFileCatalogReadSession.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsStartupFailureNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/PostWindowNativeDrawScope.h>
#include <optional>
#include <Runtime/DrawLayersView.h>
#include <Runtime/DrawTransitionView.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/SystemInformationDisplayRuntime.h>
#include <Runtime/InformationDisplayNativeReference.h>
#include <Runtime/FileInfoProgressRuntime.h>
#include <Runtime/IFileInfoProgress.h>

#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NativeGraphicsStartupMessages final : public IFileInfoProgressConfirmation
{
public:
    int Confirm(HWND owner, const char* text, const char* caption, UINT flags) override
    {
        return ::MessageBoxA(owner, text, caption, flags);
    }
};


}

EnvironmentPostWindowStartupRuntime::EnvironmentPostWindowStartupRuntime(
    EnvironmentStartupRuntime& environment, ParamsRuntimeRootOwner& params,
    FileManagerRootRuntime& fileManager, SoundManagerNativeRuntime& soundServices,
    IRootLogicalRectangleSynchronization& rootSynchronization, YtlReferenceSlotRuntime& selectedRootSlot,
    ArchiveCodecServices codecs, PreWindowLifecycleState& rootState,
    HINSTANCE resourceInstance, IStartFileApi& files, IScrVmProgramRandSource& random,
    const SaveFileFormatProfile& saveProfile, const FileTransactionContext* transaction,
    YtlReferenceSlotRuntime* rootFontReference, YtlReferenceSlotRuntime* rootSoundReference,
    Platform::IInputStartupRuntime& input, YtlReferenceSlotRuntime* rootDrawReference,
    YtlReferenceSlotRuntime* rootSceneReference,YtlReferenceSlotRuntime* rootInformationReference,
    SystemDialogNativeServices** rootInformationDialog) noexcept
    : rootSynchronization_(rootSynchronization), environment_(environment), params_(params), fileManager_(fileManager),
      selectedFileManagerSlot_(selectedRootSlot), codecs_(codecs), rootState_(rootState),
      resourceInstance_(resourceInstance), files_(files), random_(random), saveProfile_(saveProfile),
      transaction_(transaction), drawManager_(rootDrawReference ? *rootDrawReference : independentDrawManager_),
      informationReference_(rootInformationReference?*rootInformationReference:independentInformationReference_),
      informationDialog_(rootInformationDialog?*rootInformationDialog:independentInformationDialog_),input_(input),
      soundManager_(rootSoundReference ? *rootSoundReference : independentSoundManager_), soundServices_(soundServices),
      sceneReference_(rootSceneReference ? *rootSceneReference : independentSceneReference_),
      fontManager_(rootFontReference ? *rootFontReference : independentFontManager_), fontServices_(&fontPlatform_)
{
}

EnvironmentPostWindowStartupRuntime::~EnvironmentPostWindowStartupRuntime()
{
    // 原464367已退借用+30；Post成员尾不能补扫后序回调晚填的原Ref。
    if(&sceneReference_==&independentSceneReference_)ReleaseSceneManager();
    else if(!nativeSceneSelected_){sceneManager_.reset();publishedSceneManager_=nullptr;}
    // 4640E0没有释放独立Input singleton；普通451150仅由477620进入。
    // 根4643A1已负责借用槽；晚填不能在PostWindow成员析构时再访问一次。
    if (&soundManager_ == &independentSoundManager_) ReleaseSoundManager();
    // 根4642B1已负责+60；其后回调重填不得由现代成员退出再释放一遍。
    if (&fontManager_ == &independentFontManager_) ReleaseFontManager();
    // 根46432D已访问借用槽；后序回调晚填不能由Post成员尾再次补扫。
    if(&informationReference_==&independentInformationReference_)ReleaseInformationDisplay();
    else if(!nativeInformationSelected_){information_.reset();publishedInformation_=nullptr;}
    if (&drawManager_ == &independentDrawManager_) ReleaseDrawManagerFinal();
}

void EnvironmentPostWindowStartupRuntime::CreateInputAggregate()
{
    // 48ABCA先取Environment+2C，48ABDA传给4507E0；不读取GetActiveWindow或缓存创建前的HWND。
    input_.InitializeAt4507E0(reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C));
}

void EnvironmentPostWindowStartupRuntime::ReleaseInputAggregate() noexcept
{
    // 477620在SoundManager释放前进入451150，失败后的空槽同样执行。
    input_.ReleaseAt451150();
}

void EnvironmentPostWindowStartupRuntime::CreateSoundManager(ISoundManagerSavedParamsSource& source)
{
    soundServices_.CreateAt48AAFF(soundManager_, source);
}

void EnvironmentPostWindowStartupRuntime::CreateSoundManager()
{
    SoundManagerSavedParamsSource source(environment_.RootSource());
    CreateSoundManager(source);
}

void EnvironmentPostWindowStartupRuntime::ReleaseSoundManager() noexcept
{
    soundServices_.Release(soundManager_);
}

SoundManagerNativeBody50h* EnvironmentPostWindowStartupRuntime::SoundManagerAt24() const noexcept
{ return ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(soundManager_); }
ScriptMusicManagerReceiver EnvironmentPostWindowStartupRuntime::MusicReceiverAt24() const noexcept
{ return soundServices_.Music(SoundManagerAt24()); }
ScriptVoiceManagerReceiver EnvironmentPostWindowStartupRuntime::VoiceReceiverAt24() const noexcept
{ return soundServices_.Voice(SoundManagerAt24()); }
ReadScriptSoundWaitReceiver EnvironmentPostWindowStartupRuntime::SoundWaitReceiverAt24() const noexcept
{ return soundServices_.Wait(SoundManagerAt24()); }
TextSoundManagerReceiver EnvironmentPostWindowStartupRuntime::TextSoundReceiverAt24() const noexcept
{ return soundServices_.Text(SoundManagerAt24()); }
ScriptSoundManagerReceiver EnvironmentPostWindowStartupRuntime::SoundLeafReceiverAt24() const noexcept
{ return soundServices_.Effects(SoundManagerAt24()); }
SoundMappingSlotReceiver EnvironmentPostWindowStartupRuntime::SoundMappingReceiverAt24() const noexcept
{ return soundServices_.Mappings(SoundManagerAt24()); }
DemoSoundManagerReceiver EnvironmentPostWindowStartupRuntime::DemoSoundReceiverAt24() const noexcept
{ return soundServices_.Demo(SoundManagerAt24()); }

void EnvironmentPostWindowStartupRuntime::CreateSceneManager(
    ISceneConstructionServices& scenes, ISceneManagerPublicDialogFactory& dialogs)
{
    // 48ABF6构造完成后才释放旧+30；构造不运行场景，也不补写未初始化的+38。
    auto next = std::make_unique<SceneManagerRuntime>(scenes, dialogs);
    ReleaseSceneManager();
    sceneManager_ = std::move(next);
    publishedSceneManager_ = sceneManager_.get();
    nativeSceneSelected_=false;sceneServices_=nullptr;
}

void EnvironmentPostWindowStartupRuntime::CreateSceneManager(SceneManagerNativeServices& services)
{
    auto* next=NewSceneManagerNativeAt48ABE1(services);
    // 新正文构造失败不改变旧域；新control失败保留原已发布base，不能由unique_ptr回滚。
    if(!nativeSceneSelected_){sceneManager_.reset();publishedSceneManager_=nullptr;}
    sceneServices_=&services;nativeSceneSelected_=true;
    AdoptSceneManagerNativeAt48AC01(sceneReference_,next,services);
}

SceneManagerView EnvironmentPostWindowStartupRuntime::BorrowSceneManagerAt30() const noexcept
{
    if(nativeSceneSelected_)
        return SceneManagerView(ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(sceneReference_),*sceneServices_);
    return SceneManagerView(publishedSceneManager_);
}

void EnvironmentPostWindowStartupRuntime::ReleaseSceneManager() noexcept
{
    // 477620先销毁SceneManager，后销毁输入和SoundManager；正文析构期间保留发布地址。
    if(sceneServices_){SceneManagerNativeScope scope(*sceneServices_);ReleaseYtlReference(sceneReference_);}
    else ReleaseYtlReference(sceneReference_); // 原空/Probe控制块无需构造额外服务资格。
    if(!nativeSceneSelected_){sceneManager_.reset();publishedSceneManager_=nullptr;}
}

void EnvironmentPostWindowStartupRuntime::ConfigureFrameTimer(RootFrameControlRuntime& frame)
{
    if (&frame.Lifecycle() != &rootState_)
        throw std::invalid_argument("post-window timer belongs to a different root");
    frame.ConfigureTimerAt48AD8A();
}

void EnvironmentPostWindowStartupRuntime::ResetRuntimeState(RootFrameControlRuntime& frame)
{
    if (&frame.Lifecycle() != &rootState_)
        throw std::invalid_argument("post-window state belongs to a different root");
    // 48AD94..48ADE3四次取slot2；+0C只写低byte，不能抹除高24位。
    environment_.Archives().UnSaveData().ModeAt04() = 0;
    environment_.Archives().UnSaveData().DwordAt0C() &= 0xFFFFFF00u;
    environment_.Archives().UnSaveData().FlagsAt18() = 0;
    environment_.Archives().UnSaveData().FlagsAt14() = 0;
    frame.InitializeWindowFlagsAt48ADEA();
    frame.WriteValueAt214(0);
}

void EnvironmentPostWindowStartupRuntime::QueueTitleLogoScene()
{
    const auto scene=BorrowSceneManagerAt30();
    if (!scene.IsNative()&&!scene)
        throw std::logic_error("48AE0D requires the published SceneManager");
    // 5B33E8的CP932原串；47F920保存指针，所以必须有静态生命周期。
    static constexpr char titleLogo[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x83\x8D\x83\x53";
    scene.QueueScene(titleLogo);
}

void EnvironmentPostWindowStartupRuntime::PostInitialWindowModeCommand(IWindowModeCommandPlatformApi& platform)
{
    // 48AE27只读SaveData+05；PostMessage失败不阻止初始化成功，不同步Dispatch。
    if (SaveDataAtSlot1().bytesAt04[1] == 0)
        static_cast<void>(platform.PostWindowMessage(reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C),
            WM_COMMAND, 0x800B, 0));
}

void EnvironmentPostWindowStartupRuntime::CreateFontManager()
{
    CreateFontManager(fontPlatform_);
}

void EnvironmentPostWindowStartupRuntime::CreateFontManager(IFontResourcePlatformApi& platform)
{
    // 服务在原1Ch/18h之外。48AC96先构造，再释放旧Ref，base早于control分配发布。
    // 失败前缀由原factory保留，不以临时unique_ptr追加回滚或字体目录枚举。
    fontServices_ = &platform;
    FontManagerNativeScope scope(platform);
    CreateAndReplaceFontManagerAt48AC96(fontManager_);
}

void EnvironmentPostWindowStartupRuntime::ReleaseFontManager() noexcept
{
    // 4642B1只control门控；base无control的部分发布不可偷清或补析构。
    FontManagerNativeScope scope(*fontServices_);
    ReleaseFontManagerReferenceAt4642B1(fontManager_);
}

EnvironmentPathRuntime& EnvironmentPostWindowStartupRuntime::Paths() const
{
    auto* paths = environment_.PathsAt48();
    if (!paths) throw std::logic_error("post-window startup requires the initialized Environment path owner");
    return *paths;
}

SaveDataEnvironmentArchive& EnvironmentPostWindowStartupRuntime::SaveDataAtSlot1()
{
    return environment_.Archives().SaveData();
}

const RuntimeCString& EnvironmentPostWindowStartupRuntime::LibraryPathAt18After413BE0()
{
    // 483891先同步真实运行根，随后重新捕获Env路径槽；不以文件manager getter增加读源。
    rootSynchronization_.SynchronizeRuntimeAt413BE0();
    return environment_.PathsAt48()->libraryAt18;
}

void EnvironmentPostWindowStartupRuntime::ConfirmFirstInstance()
{
    ConfirmNativeStartupSettingsAt489C90(resourceInstance_, *this, files_);
}

void EnvironmentPostWindowStartupRuntime::CreateFileManager()
{
    fileManager_.CreateFileManager(selectedFileManagerSlot_, resourceInstance_,
        environment_.WindowFields().titleAt10);
}

void EnvironmentPostWindowStartupRuntime::CreateFileManager(IFileManagerStartupFactory& factory)
{
    fileManager_.CreateFileManager(selectedFileManagerSlot_, factory);
}

FileManagerNative28h* EnvironmentPostWindowStartupRuntime::FileManagerAt00() const noexcept
{
    // 479FF0解析进入489C90时已选定的root；安装/普通释放不改用全局当前根。
    return ResolveFileManagerAt479FF0(selectedFileManagerSlot_);
}

void EnvironmentPostWindowStartupRuntime::VerifyStartupFileSet()
{
    class CurrentEnvironmentReport final : public IFileManagerStartupErrorReport
    {
    public:
        explicit CurrentEnvironmentReport(EnvironmentStartupRuntime& environment) noexcept
            : environment_(environment) {}
        void ReportScanFailure(HWND window) override
        {
            // 48AE9E的progress HWND虚调用之后，48AEA3才取Environment标题。
            FileManagerStartupErrorReport report(environment_.WindowFields().titleAt10);
            report.ReportScanFailure(window);
        }
    private:
        EnvironmentStartupRuntime& environment_;
    } report(environment_);
    fileManager_.VerifyStartupFileSet(report);
}

void EnvironmentPostWindowStartupRuntime::InstallEnvironmentKeyTable()
{
    fileManager_.InstallEnvironmentKeyTable(params_, codecs_);
}

void EnvironmentPostWindowStartupRuntime::LoadSecretEnvironment()
{
    EnvironmentPostWindowPersistenceNativeServices services{environment_, fileManager_, rootState_, files_, transaction_,
        DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()};
    LoadSecretEnvironmentPostWindowNativeAt48A16A(services);
}

void EnvironmentPostWindowStartupRuntime::ScanSaveFileCatalog()
{
    // 48A1E3：每次完整扫描创建自己的临时 CParams；不会替换正在运行的 VM generation。
    auto& owner = environment_.Archives();
    SaveFileCatalogReadSession session(files_, owner.SaveData(), Paths().saveAt10, random_, saveProfile_);
    session.ScanFrom(environment_);
}

void EnvironmentPostWindowStartupRuntime::LoadGlobalEnvironment()
{
    EnvironmentPostWindowPersistenceNativeServices services{environment_, fileManager_, rootState_, files_, transaction_,
        DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory()};
    LoadGlobalEnvironmentPostWindowNativeAt48A1E8(services);
}

void EnvironmentPostWindowStartupRuntime::InitializeThroughGlobal()
{
    ConfirmFirstInstance();
    CreateFileManager();
    InitializeAfterFileManager();
}

void EnvironmentPostWindowStartupRuntime::InitializeThroughGlobal(IFileManagerStartupFactory& factory)
{
    ConfirmFirstInstance();
    CreateFileManager(factory);
    InitializeAfterFileManager();
}

void EnvironmentPostWindowStartupRuntime::InitializeAfterFileManager()
{
    VerifyStartupFileSet();
    InstallEnvironmentKeyTable();
    LoadSecretEnvironment();
    ScanSaveFileCatalog();
    LoadGlobalEnvironment();
}

void EnvironmentPostWindowStartupRuntime::InitializeGraphicsLibrary(NativeGraphicsRuntimeLibrary& graphics)
{
    NativeGraphicsStartupMessages messages;
    InitializeGraphicsLibrary(graphics, messages);
}

void EnvironmentPostWindowStartupRuntime::InitializeGraphicsLibrary(NativeGraphicsRuntimeLibrary& graphics,
    IFileInfoProgressConfirmation& messages)
{
    // 48A34C..48A398：先 height、width、HWND，最后晚取 slot1+8；不缓存 SaveData 副本。
    // 原 Graphics 只在同步 InitSystem 内消费此 byte，Start 后续不读回它。
    std::uint8_t renderOption = 1;
    const auto height = std::bit_cast<std::int32_t>(environment_.WindowFields().heightAt1C);
    const auto width = std::bit_cast<std::int32_t>(environment_.WindowFields().widthAt18);
    const auto window = reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C);
    // 48A36E固定Environment；48A376取Graphics后，才在原Environment中查询slot1。
    auto& modeEnvironment = environment_.CaptureAt4180C0();
    auto* const graphicsRoot = graphics.CaptureAt4182F0();
    auto* const saved = ResolveEnvironmentArchiveAt417990(modeEnvironment, 1);
    std::uint32_t mode;
    std::memcpy(&mode, reinterpret_cast<const std::byte*>(saved) + 8, sizeof(mode));
    if (!graphics.InitializeCapturedAt440770(*graphicsRoot, mode, window, width, height, &renderOption))
    {
        ReportGraphicsInitializationFailureNativeAt48AECE(graphics, fileManager_, environment_, messages);
    }
    graphics.ApplyInitialFilteringAt48A3A5();
}

void EnvironmentPostWindowStartupRuntime::CreateDrawManager(ICrtRandomSource& random,
    IGraphicsRuntimeCalls& graphics)
{
    drawGraphics_ = &graphics;
    GraphicsSurfaceNativeScope surfaces(graphics);
    std::optional<PostWindowNativeDrawScope> layers;
    if (drawManager_.controlAt08 && nativeLayers_) layers.emplace(*nativeLayers_, *nativeLayerRuntime_, &graphics, nativeGui_);
    // 48A593完成2Ch正文之后才进入48A5B3旧槽退役；失败不回滚rand或已写前缀。
    auto* next = NewDrawManagerNativeAt48A593(random);
    InstallDrawManagerReferenceAt48A5B3(drawManager_, next);
}

void EnvironmentPostWindowStartupRuntime::InitializeSoundLibrary(ISoundRuntimeLibrary& sound)
{
    NativeGraphicsStartupMessages messages;
    InitializeSoundLibrary(sound, messages);
}

void EnvironmentPostWindowStartupRuntime::InitializeSoundLibrary(ISoundRuntimeLibrary& sound,
    IFileInfoProgressConfirmation& messages)
{
    // 48AAE2..48AAF9：使用环境根 +2C HWND；53DFC0 内部从同根路径追加 Lib\\sound.dll。
    const auto window = reinterpret_cast<HWND>(environment_.CaptureAt4180C0().windowAt2C);
    if (sound.Initialize(window)) return; // 路径按53DFC0内部原时点取得，不提前借Paths。
    // 48AF6A..48AFA1：PE 5B33C0 固定 CP932 提示，无换行；不是 Sound 包装的错误字符串。
    static constexpr char Error[] =
        "\x83\x54\x83\x45\x83\x93\x83\x68\x83\x89\x83\x43\x83\x75\x83\x89\x83\x8A\x82\xCC"
        "\x8F\x89\x8A\xFA\x89\xBB\x82\xC9\x8E\xB8\x94\x73\x81\x49\x81\x49";
    const auto* progress = fileManager_.InitializationProgress();
    if (!progress) throw std::logic_error("Sound initialization report requires the root progress owner");
    static_cast<void>(messages.Confirm(progress->WindowAt47ADF0(), Error,
        environment_.WindowFields().titleAt10.GetString(), 0x10u));
    // 与 Graphics 不同：此分支不清 slot1+4；报告异常也按原调用链外传。
    throw 1;
}

void EnvironmentPostWindowStartupRuntime::CreateControlledInformationDisplay(GuiModalDialogDependencies dependencies,
    ISystemMenuDialogServices& services)
{
    // 48A64A..48A6F8：独立 parent owner，此处只构造，不提前 Begin/Run。
    auto next = std::make_unique<SystemInformationDisplayRuntime>(dependencies, services);
    ReleaseInformationDisplay();
    information_ = std::move(next);
    publishedInformation_ = information_.get();
    nativeInformationSelected_=false;informationServices_=nullptr;informationDialog_=nullptr;
}
void EnvironmentPostWindowStartupRuntime::BindInformationDisplayNativeServices(SystemInformationDisplayNativeServices& services) noexcept
{informationServices_=&services;informationDialog_=services.dialog;nativeInformationSelected_=true;}
void EnvironmentPostWindowStartupRuntime::CreateInformationDisplay(SystemInformationDisplayNativeServices& services)
{
    if(!nativeInformationSelected_){information_.reset();publishedInformation_=nullptr;}
    BindInformationDisplayNativeServices(services);
    CreateAndInstallInformationDisplayAt48A64A(informationReference_,services);
}
InformationDisplayView EnvironmentPostWindowStartupRuntime::BorrowInformationDisplayAt3C() const noexcept
{
    if(!nativeInformationSelected_)return InformationDisplayView(publishedInformation_);
    return {ResolveEnvironmentObjectReference<SystemInformationDisplayNativeBody14h>(informationReference_),informationServices_};
}

void EnvironmentPostWindowStartupRuntime::ReleaseDrawManager()
{
    if (!drawManager_.controlAt08) return;
    GraphicsSurfaceNativeScope surfaces(drawGraphics_);
    std::optional<PostWindowNativeDrawScope> layers;
    if (nativeLayers_) layers.emplace(*nativeLayers_, *nativeLayerRuntime_, drawGraphics_, nativeGui_);
    ReleaseDrawManagerReferenceAt477736(drawManager_);
}
void EnvironmentPostWindowStartupRuntime::ReleaseDrawManagerFinal() noexcept
{
    if (!drawManager_.controlAt08) return;
    GraphicsSurfaceNativeScope surfaces(drawGraphics_);
    std::optional<PostWindowNativeDrawScope> layers;
    if (nativeLayers_) layers.emplace(*nativeLayers_, *nativeLayerRuntime_, drawGraphics_, nativeGui_);
    ReleaseDrawManagerReferenceAt4643DB(drawManager_);
}
DrawManagerNativeBody2Ch* EnvironmentPostWindowStartupRuntime::DrawManagerAt18() const noexcept
{
    return ResolveDrawManagerReferenceAt4175A0(drawManager_);
}
DrawLayersView EnvironmentPostWindowStartupRuntime::DrawLayersAt18() const noexcept
{
    return DrawLayersView(DrawManagerAt18()).WithNativeCallServices(BorrowNativeDrawCallServices());
}
DrawLayersNativeCallServices EnvironmentPostWindowStartupRuntime::BorrowNativeDrawCallServices() const noexcept
{return {nativeLayers_,nativeLayerRuntime_,drawGraphics_,nativeWindowEffects_,nativeGui_};}
DrawTransitionView EnvironmentPostWindowStartupRuntime::DrawTransitionAt18(IGraphicsTransitApi& graphics) const noexcept
{
    return DrawTransitionView(DrawManagerAt18(), &graphics);
}

void EnvironmentPostWindowStartupRuntime::ReleaseInformationDisplay() noexcept
{
    if(nativeInformationSelected_)ReleaseInformationDisplayReferenceAt40D1A0(informationReference_,informationDialog_);
    else {information_.reset();publishedInformation_=nullptr;}
}

void EnvironmentPostWindowStartupRuntime::ReleaseInitializationProgress() noexcept
{
    fileManager_.ReleaseInitializationProgress();
}

void EnvironmentPostWindowStartupRuntime::ReleaseFileManager()
{
    fileManager_.ReleaseFileManager(selectedFileManagerSlot_);
}
}
