#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/DirectMovieRuntime.h>
#include <Runtime/PopupMenuNativeBody.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/RootScriptReceiverNativeServicesRuntime.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/SaveLoadResourceProfile.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ReleaseFontConstructionReference(YtlReferenceSlotRuntime& slot) noexcept
{
    // 59236F/40D130使用原进程RemoveFontResourceExA；不能借尚未构造的post_.fontPlatform_。
    // 服务只活在本次展开调用内，不进入原1Ch正文或18h控制块，也不依赖外层TLS残留。
    Win32FontResourcePlatformApi platform;
    FontManagerNativeScope scope(platform);
    ReleaseYtlReference(slot);
}
void ReleaseSceneConstructionReference(YtlReferenceSlotRuntime& slot) noexcept
{ReleaseYtlReference(slot);} // 592343/40D210：无control不清base/bias，不要求正文服务。
}

RootReferenceConstructionUnwind::~RootReferenceConstructionUnwind() noexcept
{
    // 40D158：无control的部分发布不清base/bias，也不进入正文服务。
    if (slot_ && slot_->controlAt08)
    {
        if (sounds_) sounds_->Release(*slot_);
        else release_(*slot_);
    }
}
RootEditWindowConstructionUnwind::~RootEditWindowConstructionUnwind() noexcept
{
    if(armed_)
    {
        EditWindowNativeScope scope(maps_,thunks_);
        DestroyEditWindowNativeReferencesAt40DE10(refs_);
    }
}
RootInformationDisplayConstructionUnwind::~RootInformationDisplayConstructionUnwind() noexcept
{
    if(armed_)ReleaseInformationDisplayReferenceAt40D1A0(slot_,services_);
}

RootRuntimePayload::RootRuntimePayload(const RootRuntimeDependencies& dependencies, FileManagerRootRuntime& fileManager,
    SoundManagerNativeRuntime& sounds, IRootLogicalRectangleSynchronization& rootSynchronization)
    : fileManagerConstructionUnwind_(fileManagerSlot_, ReleaseFileManagerAt46444F),
      modeConstructionUnwind_(mode_, ReleaseWindowModeReferenceAt464415),
      drawConstructionUnwind_(draw_, ReleaseDrawManagerReferenceAt40D050),
      soundConstructionUnwind_(sounds_, sounds),
      sceneConstructionUnwind_(scenes_,ReleaseSceneConstructionReference),
      informationConstructionUnwind_(information_,informationDialog_),
      movieConstructionUnwind_(movie_),
      editConstructionUnwind_(edits_,editMaps_,editThunks_),
      fontConstructionUnwind_(font_, ReleaseFontConstructionReference),
      frame_(dependencies.timerSource, dependencies.timerApi),
      post_(dependencies.environment, dependencies.params, fileManager, sounds, rootSynchronization, fileManagerSlot_,
          dependencies.codecs, frame_.Lifecycle(), dependencies.resourceInstance, dependencies.files,
          dependencies.random, dependencies.saveProfile, dependencies.transaction, &font_, &sounds_,dependencies.input, &draw_,&scenes_,&information_,&informationDialog_),
      params_(ScrVmParamsGenerationSlot::RootManagedTag{}, dependencies.scriptParamsMemory,
          dependencies.scriptParamsStrings),
      staticParams_(dependencies.params)
{
    // 413D95..413DC7清+1F0/+1FC两Ref；不写+219，不调用4778B0/489C90。
    // 413DD1/413DDB只清这两个DWORD；+208在原构造未写，不能人为赋0或-1。
    menuPosition_.widthAt20C=0;
    menuPosition_.heightAt210=0;
    fontConstructionUnwind_.Disarm();
    editConstructionUnwind_.Disarm();
    movieConstructionUnwind_.Disarm();
    informationConstructionUnwind_.Disarm();
    sceneConstructionUnwind_.Disarm();
    soundConstructionUnwind_.Disarm();
    drawConstructionUnwind_.Disarm();
    modeConstructionUnwind_.Disarm();
    fileManagerConstructionUnwind_.Disarm();
}

RootRuntimePayload::~RootRuntimePayload() noexcept
{
    // 464116先Popup，46414B才转场；正文析构所需API不存进A4h。
    // 独立Input不属于4640E0根成员析构；不得在两Ref释放之间补调用451150。
    if (popup_.controlAt08)
    {
        GraphicsSurfaceNativeScope scope(surfaceApi_);
        ReleaseYtlReference(popup_);
    }
    if (transition_.controlAt08)
    {
        GraphicsSurfaceNativeScope scope(surfaceApi_);
        ReleaseYtlReference(transition_);
    }
    // 464191..464213只检查control；无control的部分发布不解引用base，也不要求Graphics。
    if (previewSlots_.captureAt1E4.controlAt08 || previewSlots_.thumbnailAt1D8.controlAt08)
    {
        // 原根析构EHFlags=5；真正释放非空handle时缺服务仍终止，不借外层scope蒙混通过。
        GraphicsSurfaceNativeScope scope(surfaceApi_);
        ReleaseYtlReference(previewSlots_.captureAt1E4);
        ReleaseYtlReference(previewSlots_.thumbnailAt1D8);
    }
    // 46421D..263：Script先于CParams。
    // 先退役正文外借用图；不释放/增持任何原Ref，不把477620场景退出当作Root析构。
    RetireScriptReceiverServices();
    ReleaseYtlReference(script_);
    params_.Release();
    frame_.TimerAt6C().DestroyAt43F820(); // 4642A9：内嵌Timer早于Font，不留到成员自动析构。
    post_.ReleaseFontManager();
    {
        // 4642EB Font之后、Movie之前；不继承其它root残留的窗口服务TLS。
        EditWindowNativeScope scope(editMaps_,editThunks_);
        DestroyEditWindowNativeReferencesAt40DE10(edits_);
    }
    // 4642F3..464326在Font之后、Information之前释放root+48。
    ReleaseDirectMovie();
    post_.ReleaseInformationDisplay();
    post_.ReleaseSceneManager();
    post_.ReleaseSoundManager();
    post_.ReleaseDrawManagerFinal();
    ReleaseWindowModeReferenceAt464415(mode_);
    // 46444F最后读取当前+00；前序回调可重填。借用服务/裸槽析构不得追加第三次释放。
    ReleaseFileManagerAt46444F(fileManagerSlot_);
}

WindowModeView RootRuntimePayload::WindowModeAt0C() const
{
    auto* const body = ResolveWindowModeReference(mode_);
    // 原空base随后非法访问；保留现有明确失败的现代边界，不制造mode0。
    if (!body) throw std::logic_error("root window mode reference has an empty base");
    return WindowModeView(*body, modeServices_);
}

ScrVmParamsGeneration& RootRuntimePayload::ParamsAt47BDB0()
{ return params_.Current(); }

void RootRuntimePayload::ReplaceParamsAt481720()
{ params_.Replace(); }

ScriptNativeBody* RootRuntimePayload::ScriptAt47CD00()
{
    return GetScriptAt47CD00(script_, staticParams_);
}

RootScriptReceiverNativeServicesRuntime& RootRuntimePayload::AdoptScriptReceiverServices(
    std::unique_ptr<RootScriptReceiverNativeServicesRuntime> services)
{
    if(scriptServices_)throw std::logic_error("root CScript services are already installed");
    if(!services)throw std::invalid_argument("root CScript services are null");
    auto& state=services->State();
    // 只解本次已发布Ref；不得再次47CD00/413BE0换掉caller选定的receiver。
    if(&state.NativeBody()!=ResolveEnvironmentObjectReference<ScriptNativeBody>(script_)||
        &state.Publication()!=&scriptPublication_||state.paramsGeneration.get()!=&params_)
        throw std::invalid_argument("root CScript services borrow a different receiver, publication or Params slot");
    scriptServices_=std::move(services);
    return *scriptServices_;
}
void RootRuntimePayload::RetireScriptReceiverServices() noexcept{scriptServices_.reset();}

PopupMenuNativeBodyA4h* RootRuntimePayload::PopupMenuAt47C8A0(PopupMenuNativeDependencies& dependencies)
{
    BindGraphicsSurfaceApi(dependencies.surfaces);
    PopupMenuNativeScope scope(dependencies);
    return GetPopupMenuAt47C8A0(popup_);
}

IGraphicsSurfaceApi& RootRuntimePayload::RequireGraphicsSurfaceApi() const
{
    if (!surfaceApi_) throw std::logic_error("root Surface Graphics API has not been bound");
    return *surfaceApi_;
}

RootSavePreviewRuntime RootRuntimePayload::CreateSavePreviewService(IGraphicsSurfaceApi& surfaces,
    IGraphicsWindowSurfaceApi& pixels, IGraphicsSurfaceSaveApi& saver,
    IStartFileApi& files, ISavePreviewCaptureServices& capture)
{
    BindGraphicsSurfaceApi(surfaces);
    return RootSavePreviewRuntime(previewSlots_, surfaces, pixels, saver, files, capture,
        CurrentSaveLoadResourceProfile().thumbnailSize);
}

void RootRuntimePayload::InstallDirectMovie(std::unique_ptr<DirectMovieRuntime> movie)
{
    movie_.ReplaceControlled(std::move(movie));
}

void RootRuntimePayload::ReleaseDirectMovie()
{
    // 477769..477795 / 4B96CC..4B9703：旧析构/COM回调期间仍可见旧发布地址。
    movie_.Release();
}

void RootRuntimeOwner::InstallDirectMovieAt4A0C76(std::unique_ptr<DirectMovieRuntime> movie)
{
    EnsureCreatedAt413BE0();
    RequirePublished().InstallDirectMovie(std::move(movie));
}

DirectMovieNativeBody38h* RootRuntimeOwner::CreateAndInstallDirectMovie(DirectMovieNativeServices& services,IEnvironmentRootMemory& memory)
{
    auto* raw=memory.AllocateBody(0x38);
    struct Raw final
    {
        void* body;IEnvironmentRootMemory& memory;bool active=true;
        ~Raw()noexcept{if(active&&body)memory.DeleteBody(body,0x38);}
    } cleanup{raw,memory};
    auto* created=raw?ConstructDirectMovieNativeAt4A0BFD(*static_cast<DirectMovieNativeBody38h*>(raw),services):nullptr;
    // 4A0C72 state0：取得Root及分配控制块的后续异常，不再撤销新正文/COM/WinHook。
    cleanup.active=false;InstallNativeDirectMovieAt4A0C76(created,services,memory);return created;
}
void RootRuntimeOwner::InstallNativeDirectMovieAt4A0C76(DirectMovieNativeBody38h* movie,
    DirectMovieNativeServices& services,IEnvironmentRootMemory& memory)
{
    // caller已完成Acquire及WinHook登记；本调用不以RAII撤销已成功构造的新正文。
    EnsureCreatedAt413BE0();RequirePublished().movie_.ReplaceNative(movie,services,memory);
}

std::uint8_t RootRuntimePayload::ReadByteAt1AF() const
{
    if (!byteAt1AFPublished_) throw std::logic_error("root+1AF read before publication");
    return frame_.Lifecycle().byteAt1AF;
}

std::uint8_t RootRuntimePayload::ReadByteAt218() const
{
    if (!byteAt218_) throw std::logic_error("root+218 read before publication");
    return *byteAt218_;
}

std::uint8_t RootRuntimePayload::ReadByteAt219() const
{
    if (!byteAt219_) throw std::logic_error("root+219 read before publication");
    return *byteAt219_;
}

void RootRuntimePayload::RequirePreWindowInitialization() const
{
    if (!frame_.Lifecycle().initializedAt4778B0)
        throw std::logic_error("root lifecycle read before 4778B0 initialization");
}

std::uint32_t& RootRuntimePayload::ModalGateAt1B8()
{
    RequirePreWindowInitialization();
    return frame_.Lifecycle().valueAt1B8;
}

std::uint32_t RootRuntimePayload::ModalGateAt1B8() const
{
    RequirePreWindowInitialization();
    return frame_.Lifecycle().valueAt1B8;
}

bool RootRuntimePayload::ExitRequestedAt1BC() const
{
    RequirePreWindowInitialization();
    return frame_.ExitRequestedAt1BC();
}

RootRuntimeOwner::RootRuntimeOwner(RootRuntimeDependencies dependencies)
    : dependencies_(dependencies), fileManager_(*this), soundArchives_(*this),
      soundServices_(dependencies.params, dependencies.sound, soundArchives_, dependencies.soundWide)
{
    // 4010C0：只建立锁，根正文留到413BE0；Win32 critical section支持同线程递归。
    ::InitializeCriticalSection(&lock_);
}
RootRuntimeOwner::RootRuntimeOwner(RootRuntimeDependencies dependencies, IArchiveResourceSource& archives)
    : dependencies_(dependencies), fileManager_(*this), soundArchives_(*this),
      soundServices_(dependencies.params, dependencies.sound, archives, dependencies.soundWide)
{
    ::InitializeCriticalSection(&lock_);
}
RootRuntimeOwner::RootRuntimeOwner(RootRuntimeDependencies dependencies, RootCriticalSectionNativeReference& lock)
    : dependencies_(dependencies), fileManager_(*this), soundArchives_(*this),
      soundServices_(dependencies.params, dependencies.sound, soundArchives_, dependencies.soundWide), borrowedLock_(&lock)
{
}

RootRuntimeOwner::~RootRuntimeOwner() noexcept
{
    Release();
    fileManager_.ReleaseInitializationProgress();
    if (!borrowedLock_) ::DeleteCriticalSection(&lock_);
}

void RootRuntimeOwner::SynchronizeRuntimeAt413BE0() { EnsureCreatedAt413BE0(); }

void RootRuntimeOwner::EnsureCreatedAt413BE0()
{
    const auto guard = AcquireCriticalLock();
    // 413C27检查在锁内；即使已有正文也不能走无锁快路径。
    if (!published_)
    {
        auto next = std::make_unique<RootRuntimePayload>(dependencies_, fileManager_, soundServices_, *this);
        // 413DE9仅覆盖singleton；构造回调已发布的内层根不会被原版补析构。
        // 有意保留原泄漏边界，不用unique_ptr赋值偷偷增加Timer/各槽的释放副作用。
        static_cast<void>(payload_.release());
        payload_ = std::move(next);
        published_ = payload_.get();
    }
}

YtlReferenceSlotRuntime& RootRuntimeOwner::CaptureFileManagerSlotAt413BE0()
{
    EnsureCreatedAt413BE0();
    return RequirePublished().FileManagerSlotAt00();
}

const YtlReferenceSlotRuntime* RootRuntimeOwner::PublishedFileManagerSlot() const noexcept
{
    return published_ ? &published_->FileManagerSlotAt00() : nullptr;
}

RootRuntimePayload& RootRuntimeOwner::RequirePublished()
{
    if (!published_) throw std::logic_error("runtime root has not been published");
    return *published_;
}

const RootRuntimePayload& RootRuntimeOwner::RequirePublished() const
{
    if (!published_) throw std::logic_error("runtime root has not been published");
    return *published_;
}

void RootRuntimeOwner::Release() noexcept
{
    const auto guard = AcquireCriticalLock();
    ReleaseCurrentUnderCriticalLockAt5AB3F6();
}
void RootRuntimeOwner::ReleaseCurrentUnderCriticalLockAt5AB3F6() noexcept
{
    // 5AB3F4进入同一off_60113C锁，5AB402析构、5AB415清发布，5AB42D才解锁。
    // 析构回调仍可获取当前发布对象；不能递归Release同一个正文。
    payload_.reset();
    published_ = nullptr;
}
}
