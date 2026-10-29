#pragma once

#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/RootFrameControlRuntime.h>
#include <Runtime/RootLogicalRectangleRuntime.h>
#include <Runtime/RootSavePreviewRuntime.h>
#include <Runtime/ScrVmParamsGeneration.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/ProcessSoundRuntime.h>
#include <Runtime/EditWindowNativeCollection.h>
#include <Runtime/InformationDisplayNativeReference.h>
#include <Runtime/DirectMovieReferenceNative.h>
#include <Runtime/RootMenuPositionNativeState.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class DirectMovieRuntime;
class IDirectMovieHost;
struct PopupMenuNativeBodyA4h;
struct PopupMenuNativeDependencies;
class RootScriptReceiverNativeServicesRuntime;
class RootCriticalSectionNativeReference;
// 原singleton依赖仅借用，不复制Environment/Params/Files或建立反向保活环。
// 全部依赖须长于owner；本类不替代完整进程根及其启动/退出协调器。
struct RootRuntimeDependencies final
{
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    ArchiveCodecServices codecs;
    Platform::ProcessTimerSource& timerSource;
    Platform::ITimerPlatformApi& timerApi;
    HINSTANCE resourceInstance;
    IStartFileApi& files;
    IScrVmProgramRandSource& random;
    const SaveFileFormatProfile& saveProfile;
    const FileTransactionContext* transaction = nullptr;
    IEnvironmentRootMemory& scriptParamsMemory = DefaultEnvironmentRootMemory();
    IRuntimeStringManagerSource& scriptParamsStrings = DefaultRuntimeStringManagerSource();
    NativeSoundRuntimeLibrary& sound = ProcessSoundRuntimeLibrary();
    IRuntimeWideStringNativeApi& soundWide = DefaultRuntimeWideStringNativeApi();
    Platform::IInputStartupRuntime& input = ProcessNativeInputRuntime();
};

// 原Ref成员的构造展开资格；正常构造末尾撤销，不承担完整root析构或晚填槽的清理。
// 释放函数自行建立正文外服务作用域，不能借用排在本守卫之后的成员。
class RootReferenceConstructionUnwind final
{
public:
    using Release = void (*)(YtlReferenceSlotRuntime&) noexcept;
    RootReferenceConstructionUnwind(YtlReferenceSlotRuntime& slot, Release release) noexcept
        : slot_(&slot), release_(release) {}
    RootReferenceConstructionUnwind(YtlReferenceSlotRuntime& slot, SoundManagerNativeRuntime& sounds) noexcept
        : slot_(&slot), sounds_(&sounds) {}
    ~RootReferenceConstructionUnwind() noexcept;
    RootReferenceConstructionUnwind(const RootReferenceConstructionUnwind&) = delete;
    RootReferenceConstructionUnwind& operator=(const RootReferenceConstructionUnwind&) = delete;
    void Disarm() noexcept { slot_ = nullptr; }
private:
    YtlReferenceSlotRuntime* slot_;
    Release release_ = nullptr;
    SoundManagerNativeRuntime* sounds_ = nullptr;
};

// 413BE0的受限生产payload，不是原21Ch布局，也不是完整root。
// 收束已有frame、模式、唯一声音Ref、后窗口owner、DirectMovie、转场引用、预览双Ref、CParams懒槽和+218/+219；
// Popup+1FC为独立原生Ref12；计时状态和creator引用计数使用原生正文。
class RootEditWindowConstructionUnwind final
{
public:
    RootEditWindowConstructionUnwind(EditWindowNativeReferences& refs,EditWindowNativeMapServices*& maps,
        WindowThunkNativeServices*& thunks) noexcept:refs_(refs),maps_(maps),thunks_(thunks){}
    ~RootEditWindowConstructionUnwind() noexcept;
    void Disarm() noexcept{armed_=false;}
private:
    EditWindowNativeReferences& refs_;
    EditWindowNativeMapServices*& maps_;
    WindowThunkNativeServices*& thunks_;
    bool armed_=true;
};
class RootInformationDisplayConstructionUnwind final
{
public:
    RootInformationDisplayConstructionUnwind(YtlReferenceSlotRuntime& slot,SystemDialogNativeServices*& services) noexcept
        :slot_(slot),services_(services){}
    ~RootInformationDisplayConstructionUnwind() noexcept;
    void Disarm() noexcept{armed_=false;}
private:
    YtlReferenceSlotRuntime& slot_;
    SystemDialogNativeServices*& services_;
    bool armed_=true;
};
class RootMovieConstructionUnwind final
{
public:
    explicit RootMovieConstructionUnwind(RootDirectMovieReference& movie)noexcept:movie_(movie){}
    ~RootMovieConstructionUnwind()noexcept{if(armed_)movie_.Release();}
    void Disarm()noexcept{armed_=false;}
private:
    RootDirectMovieReference& movie_;
    bool armed_=true;
};
class RootRuntimePayload final
{
public:
    RootRuntimePayload(const RootRuntimeDependencies& dependencies, FileManagerRootRuntime& fileManager,
        SoundManagerNativeRuntime& sounds, IRootLogicalRectangleSynchronization& rootSynchronization);
    ~RootRuntimePayload() noexcept;
    RootRuntimePayload(const RootRuntimePayload&) = delete;
    RootRuntimePayload& operator=(const RootRuntimePayload&) = delete;

    [[nodiscard]] RootFrameControlRuntime& Frame() noexcept { return frame_; }
    [[nodiscard]] const RootFrameControlRuntime& Frame() const noexcept { return frame_; }
    [[nodiscard]] EnvironmentPostWindowStartupRuntime& PostWindow() noexcept { return post_; }
    [[nodiscard]] const EnvironmentPostWindowStartupRuntime& PostWindow() const noexcept { return post_; }
    [[nodiscard]] YtlReferenceSlotRuntime& FileManagerSlotAt00() noexcept { return fileManagerSlot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& FileManagerSlotAt00() const noexcept { return fileManagerSlot_; }
    [[nodiscard]] YtlReferenceSlotRuntime& FontManagerSlotAt60() noexcept { return font_; }
    [[nodiscard]] YtlReferenceSlotRuntime& DrawManagerSlotAt18() noexcept { return draw_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& DrawManagerSlotAt18() const noexcept { return draw_; }
    [[nodiscard]] YtlReferenceSlotRuntime& SoundManagerSlotAt24() noexcept { return sounds_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& SoundManagerSlotAt24() const noexcept { return sounds_; }
    [[nodiscard]] YtlReferenceSlotRuntime& SceneManagerSlotAt30() noexcept{return scenes_;}
    [[nodiscard]] const YtlReferenceSlotRuntime& SceneManagerSlotAt30() const noexcept{return scenes_;}
    [[nodiscard]] YtlReferenceSlotRuntime& InformationDisplaySlotAt3C() noexcept{return information_;}
    [[nodiscard]] const YtlReferenceSlotRuntime& InformationDisplaySlotAt3C() const noexcept{return information_;}
    void BindInformationDisplayNativeServices(SystemDialogNativeServices* services) noexcept{informationDialog_=services;}
    [[nodiscard]] EditWindowNativeReferences& EditWindowsAt54() noexcept{return edits_;}
    // 宿主服务借用须覆盖root退役；不进入原Ref向量，也不产生另一份窗口持有。
    void BindEditWindowNativeServices(EditWindowNativeMapServices& maps,WindowThunkNativeServices& thunks) noexcept
    {editMaps_=&maps;editThunks_=&thunks;}
    void ClearEditWindowNativeServices() noexcept{editMaps_=nullptr;editThunks_=nullptr;}
    [[nodiscard]] EditWindowNativeMapServices* BorrowEditWindowNativeMapServices() const noexcept{return editMaps_;}
    [[nodiscard]] WindowThunkNativeServices* BorrowEditWindowNativeThunkServices() const noexcept{return editThunks_;}
    // 47BDB0/481720本身不调用413BE0；调用方先取得当前root，不能额外锁住整个构造。
    [[nodiscard]] ScrVmParamsGeneration& ParamsAt47BDB0();
    void ReplaceParamsAt481720();
    [[nodiscard]] ScrVmParamsGenerationSlot& ParamsSlotAt1C0() noexcept { return params_; }
    [[nodiscard]] const ScrVmParamsGenerationSlot& ParamsSlotAt1C0() const noexcept { return params_; }
    [[nodiscard]] ScriptNativeBody* ScriptAt47CD00();
    [[nodiscard]] YtlReferenceSlotRuntime& ScriptSlotAt1CC() noexcept { return script_; }
    [[nodiscard]] ScriptNativePublication& ScriptPublication() noexcept { return scriptPublication_; }
    // 现代服务图的拥有边界，不属于原21Ch布局；必须在原Script成功选定后显式安装。
    // 安装/退役要求消费者静止；不支持dispatch中销毁唯一receiver后继续旧调用。
    RootScriptReceiverNativeServicesRuntime& AdoptScriptReceiverServices(std::unique_ptr<RootScriptReceiverNativeServicesRuntime>);
    [[nodiscard]] RootScriptReceiverNativeServicesRuntime* BorrowScriptReceiverServices() const noexcept{return scriptServices_.get();}
    void RetireScriptReceiverServices() noexcept;
    // 旧测试域显式借用；非空native正文经此入口会拒绝，正式消费者使用MovieAt48。
    [[nodiscard]] DirectMovieRuntime* DirectMovieAt48() const { return movie_.ControlledBody(); }
    [[nodiscard]] DirectMovieView MovieAt48()const noexcept{return movie_.Borrow();}
    [[nodiscard]] bool MovieBasePresentAt48()const noexcept{return movie_.HasBase();}
    void ClearMovieNativeServices()noexcept{movie_.ClearNativeServices();}
    [[nodiscard]] YtlReferenceSlotRuntime& MovieSlotAt48(){return movie_.NativeSlot();}
    [[nodiscard]] const YtlReferenceSlotRuntime& MovieSlotAt48()const{return movie_.NativeSlot();}
    void ReleaseDirectMovie();
    [[nodiscard]] YtlReferenceSlotRuntime& WindowModeSlotAt0C() noexcept { return mode_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& WindowModeSlotAt0C() const noexcept { return mode_; }
    // 查询仅选当前槽，不Ensure、不增持；返回值固定该正文及本次借用服务。
    [[nodiscard]] WindowModeView WindowModeAt0C() const;
    void BindWindowModeServices(WindowModeRuntimeServices services) noexcept { modeServices_ = services; }
    [[nodiscard]] WindowModeRuntimeServices& WindowModeServices() noexcept { return modeServices_; }
    [[nodiscard]] YtlReferenceSlotRuntime& TransitionFrameSlotAt1F0() noexcept { return transition_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& TransitionFrameSlotAt1F0() const noexcept { return transition_; }
    [[nodiscard]] YtlReferenceSlotRuntime& PopupMenuSlotAt1FC() noexcept { return popup_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& PopupMenuSlotAt1FC() const noexcept { return popup_; }
    // 已选root的借用getter；依赖只在正文外绑定，不Ensure也不打开菜单。
    [[nodiscard]] PopupMenuNativeBodyA4h* PopupMenuAt47C8A0(PopupMenuNativeDependencies& dependencies);
    [[nodiscard]] RootSavePreviewSlots& SavePreviewSlots() noexcept { return previewSlots_; }
    [[nodiscard]] const RootSavePreviewSlots& SavePreviewSlots() const noexcept { return previewSlots_; }
    // 显式借用删除服务，不创建根、不占双Ref正文；依赖须活过使用它的所有逃逸引用。
    void BindGraphicsSurfaceApi(IGraphicsSurfaceApi& api) noexcept { surfaceApi_ = &api; }
    [[nodiscard]] IGraphicsSurfaceApi& RequireGraphicsSurfaceApi() const;
    // 已发布根的生产装配入口：登记和服务使用同一API，只借唯一双槽，不Ensure或分配正文。
    [[nodiscard]] RootSavePreviewRuntime CreateSavePreviewService(IGraphicsSurfaceApi& surfaces,
        IGraphicsWindowSurfaceApi& pixels, IGraphicsSurfaceSaveApi& saver,
        IStartFileApi& files, ISavePreviewCaptureServices& capture);

    [[nodiscard]] bool ByteAt1AFPublished() const noexcept { return byteAt1AFPublished_; }
    [[nodiscard]] std::uint8_t ReadByteAt1AF() const;
    void WriteByteAt1AF(std::uint8_t value) noexcept
        { frame_.Lifecycle().byteAt1AF = value; byteAt1AFPublished_ = true; }
    [[nodiscard]] bool ByteAt218Published() const noexcept { return byteAt218_.has_value(); }
    [[nodiscard]] std::uint8_t ReadByteAt218() const;
    void WriteByteAt218(std::uint8_t value) noexcept { byteAt218_ = value; }
    [[nodiscard]] bool ByteAt219Published() const noexcept { return byteAt219_.has_value(); }
    [[nodiscard]] std::uint8_t ReadByteAt219() const;
    void WriteByteAt219(std::uint8_t value) noexcept { byteAt219_ = value; }
    // 不以C++默认零冒充4778F6/477900；检查后仍借用同一frame.Lifecycle字段。
    [[nodiscard]] std::uint32_t& ModalGateAt1B8();
    [[nodiscard]] std::uint32_t ModalGateAt1B8() const;
    [[nodiscard]] bool ExitRequestedAt1BC() const;
    [[nodiscard]] RootMenuPositionNativeState0Ch& MenuPositionAt208()noexcept{return menuPosition_;}
    [[nodiscard]] const RootMenuPositionNativeState0Ch& MenuPositionAt208()const noexcept{return menuPosition_;}

private:
    friend class RootRuntimeOwner;
    RootMenuPositionNativeState0Ch menuPosition_;
    void InstallDirectMovie(std::unique_ptr<DirectMovieRuntime> movie);
    void RequirePreWindowInitialization() const;
    // 413C55..413C62先清+00；59231A最后展开此前槽，正常构造后撤销展开资格。
    YtlReferenceSlotRuntime fileManagerSlot_;
    RootReferenceConstructionUnwind fileManagerConstructionUnwind_;
    // 413C69..413C77再清模式Ref；592322在根构造失败时展开，不由成员正常析构回访。
    YtlReferenceSlotRuntime mode_;
    RootReferenceConstructionUnwind modeConstructionUnwind_;
    WindowModeRuntimeServices modeServices_;
    // 413C7E..413C8C；59232D/40D050展开。PostWindow只借同一槽，不另持manager。
    YtlReferenceSlotRuntime draw_;
    RootReferenceConstructionUnwind drawConstructionUnwind_;
    // 413C93..413CA1；592338/40D280展开。PostWindow只借此唯一Ref。
    YtlReferenceSlotRuntime sounds_;
    RootReferenceConstructionUnwind soundConstructionUnwind_;
    // 413CA8..413CB6原+30唯一Ref；592343/40D210展开，Post只借用不重复持有。
    YtlReferenceSlotRuntime scenes_;
    RootReferenceConstructionUnwind sceneConstructionUnwind_;
    // 413CBD..413CCB清原+3C；59234E/40D1A0在后续构造失败时退役，正常析构46432D。
    YtlReferenceSlotRuntime information_;
    SystemDialogNativeServices* informationDialog_=nullptr;
    RootInformationDisplayConstructionUnwind informationConstructionUnwind_;
    // 413CD2..413CE0 / 592359->40D6E0：在Edit/Font/Timer前清原+48唯一Ref。
    RootDirectMovieReference movie_;
    RootMovieConstructionUnwind movieConstructionUnwind_;
    // 413CE7..413CF5，592364/40DE10；窗口向量在Font及Timer之前取得展开资格。
    EditWindowNativeReferences edits_{};
    EditWindowNativeMapServices* editMaps_=nullptr;
    WindowThunkNativeServices* editThunks_=nullptr;
    RootEditWindowConstructionUnwind editConstructionUnwind_;
    // 413CFC..413D0A：Font Ref在内嵌Timer构造之前清零，PostWindow只借此槽。
    YtlReferenceSlotRuntime font_;
    // 413D14 state0Ah -> 59236F -> 40D130；Timer及后续成员构造失败只清一次。
    RootReferenceConstructionUnwind fontConstructionUnwind_;
    // Lifecycle存储保留到借用owner释放；内嵌计时在4642A9显式析构。
    RootFrameControlRuntime frame_;
    EnvironmentPostWindowStartupRuntime post_;
    YtlReferenceSlotRuntime transition_;
    YtlReferenceSlotRuntime popup_;
    // 413D59..413D8B只清两个Ref，不分配Surface；API登记不属于原18h槽存储。
    RootSavePreviewSlots previewSlots_{};
    IGraphicsSurfaceApi* surfaceApi_ = nullptr;
    // 413D1D..413D31只清Ref；slot自身不分配CParams，不与静态params.dat owner混同。
    ScrVmParamsGenerationSlot params_;
    ParamsRuntimeRootOwner& staticParams_;
    YtlReferenceSlotRuntime script_;
    ScriptNativePublication scriptPublication_;
    std::unique_ptr<RootScriptReceiverNativeServicesRuntime> scriptServices_;
    // 413BE0未写这三个byte；必须等4BB09F/4A2DBD/4A2DCF等实际写入，不预填默认值。
    bool byteAt1AFPublished_ = false;
    std::optional<std::uint8_t> byteAt218_;
    std::optional<std::uint8_t> byteAt219_;
};

class RootRuntimeOwner final : public IRootLogicalRectangleSynchronization, public IFileManagerRootReferenceSource
{
public:
    // 40E780的作用域释放：守卫仅能短于owner，不能跨模态循环持有。
    class CriticalLock final
    {
    public:
        ~CriticalLock() noexcept;
        CriticalLock(const CriticalLock&) = delete;
        CriticalLock& operator=(const CriticalLock&) = delete;
    private:
        friend class RootRuntimeOwner;
        explicit CriticalLock(RootRuntimeOwner& owner);
        RootRuntimeOwner& owner_;
    };

    explicit RootRuntimeOwner(RootRuntimeDependencies dependencies);
    // 正式Process借原4FC锁，不在完整图冷构造时再建/结束一把锁。
    RootRuntimeOwner(RootRuntimeDependencies dependencies, RootCriticalSectionNativeReference& lock);
    // 显式外部资源来源组合；正常根构造始终使用自身FileManager，没有失败换路。
    RootRuntimeOwner(RootRuntimeDependencies dependencies, IArchiveResourceSource& soundArchives);
    ~RootRuntimeOwner() noexcept override;
    RootRuntimeOwner(const RootRuntimeOwner&) = delete;
    RootRuntimeOwner& operator=(const RootRuntimeOwner&) = delete;

    void SynchronizeRuntimeAt413BE0() override;
    void EnsureCreatedAt413BE0();
    [[nodiscard]] YtlReferenceSlotRuntime& CaptureFileManagerSlotAt413BE0() override;
    [[nodiscard]] const YtlReferenceSlotRuntime* PublishedFileManagerSlot() const noexcept override;
    // 服务活过root各代正文；每次业务查询按原地址重新取根，不缓存native body。
    [[nodiscard]] FileManagerRootRuntime& FileManagerSource() noexcept { return fileManager_; }
    [[nodiscard]] SoundManagerNativeRuntime& NativeSoundServices() noexcept { return soundServices_; }
    [[nodiscard]] IStartFileApi& Files() const noexcept { return dependencies_.files; }
    // 4A0C67已注册新电影之后，4A0C76才解根，再释放旧正文并发布新正文。
    void InstallDirectMovieAt4A0C76(std::unique_ptr<DirectMovieRuntime> movie);
    void InstallNativeDirectMovieAt4A0C76(DirectMovieNativeBody38h*,DirectMovieNativeServices&,
        IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
    DirectMovieNativeBody38h* CreateAndInstallDirectMovie(DirectMovieNativeServices&,
        IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
    // 以下查询不隐式创建或重复加锁；与原调用后的6198C4读取分开。
    // 发布指针/字段并非跨线程生命期保障；外层须在停止消费者后才能Release。
    [[nodiscard]] RootRuntimePayload* Published() const noexcept { return published_; }
    // 原caller仅取6198C4再传给不消费ECX的叶时，仍保留发布槽读取，不能优化成弃值。
    [[nodiscard]] RootRuntimePayload* ReadPublishedAt6198C4() const noexcept
    { return *static_cast<RootRuntimePayload* const volatile*>(&published_); }
    [[nodiscard]] RootRuntimePayload& RequirePublished();
    [[nodiscard]] const RootRuntimePayload& RequirePublished() const;
    void Release() noexcept;
    // 5AB3F4已经取得同一原锁后调用；不补GetCurrentThreadId检查或重复Enter/Leave。
    void ReleaseCurrentUnderCriticalLockAt5AB3F6() noexcept;

    // 对应原CCriticalLock及锁虚槽+08；可用于需要同锁的生命周期操作，不暴露裸HANDLE。
    [[nodiscard]] CriticalLock AcquireCriticalLock();
    [[nodiscard]] bool LockHeldByCurrentThreadAt4076D0() const noexcept;

private:
    void Enter();
    void Leave() noexcept;
    RootRuntimeDependencies dependencies_;
    FileManagerRootRuntime fileManager_;
    // 稳定服务活过根各代与退出回调；VFS仍按42BCD1获取实际FileManager。
    RootArchiveResourceSource soundArchives_;
    SoundManagerNativeRuntime soundServices_;
    RootCriticalSectionNativeReference* borrowedLock_ = nullptr;
    CRITICAL_SECTION lock_{};
    // 锁对象+1C/+20的现代表示；原线程初值FFFFFFFF，递归深度初值0。
    std::atomic<DWORD> lockThread_{0xFFFFFFFFu};
    std::uint32_t lockDepth_ = 0;
    std::unique_ptr<RootRuntimePayload> payload_;
    RootRuntimePayload* published_ = nullptr;
};
}
