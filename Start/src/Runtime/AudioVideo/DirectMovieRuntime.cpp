#include <Runtime/DirectMovieRuntime.h>
#include <Runtime/ProcessWinHookRuntime.h>

#include <dshow.h>

#include <array>
#include <cstddef>
#include <utility>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Strmiids.lib")

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr UINT kDirectShowNotifyMessage = 0x8001u;
constexpr long kMovieWindowStyle = 0x44000000L;
constexpr long kMutedVolume = -10000L;
constexpr long kEventPollTimeoutMilliseconds = 0L;
constexpr DWORD kAbsolutePositioning = 1u;

static_assert(sizeof(void*) == 4u, "DirectMovieRuntime must be built for x86");
static_assert(
    kMovieWindowStyle == (WS_CHILD | WS_CLIPSIBLINGS),
    "DirectShow movie window style no longer matches Start.exe:4BC790");

constexpr std::array<DirectMovieInterface, 8> kAcquireOrder = {
    DirectMovieInterface::GraphBuilder,
    DirectMovieInterface::MediaControl,
    DirectMovieInterface::MediaSeeking,
    DirectMovieInterface::MediaPosition,
    DirectMovieInterface::MediaEventEx,
    DirectMovieInterface::VideoWindow,
    DirectMovieInterface::BasicVideo,
    DirectMovieInterface::BasicAudio};

constexpr const char* kAcquireErrorMessagesCp932[] = {
    "IGraphBuilder"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IMediaControl"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IMediaSeeking"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IMediaPosition"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IMediaEventEx"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IVideoWindow"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IBasicVideo"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow",
    "IBasicAudio"
    "\x83\x49\x83\x75\x83\x57\x83\x46\x83\x4E\x83\x67\x82\xCC"
    "\x8D\xEC\x90\xAC\x82\xC9\x8E\xB8\x94\x73 : DirectShow"};

constexpr char kRenderFileErrorCp932[] =
    "\x83\x74\x83\x40\x83\x43\x83\x8B\x96\xBC\x82\xCC\x90\xDD\x92\xE8"
    "\x82\xC9\x8E\xB8\x94\x73 : DirectShow";
constexpr char kOwnerErrorCp932[] =
    "\x90\x65\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x82\xCC\x90\xDD"
    "\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow";
constexpr char kWindowStyleErrorCp932[] =
    "\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x83\x58\x83\x5E\x83\x43"
    "\x83\x8B\x82\xCC\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow";
constexpr char kWindowPositionErrorCp932[] =
    "\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x88\xCA\x92\x75\x82\xCC"
    "\x90\xDD\x92\xE8\x82\xC9\x8E\xB8\x94\x73 : DirectShow";
constexpr char kNotifyWindowErrorCp932[] =
    "\x83\x43\x83\x78\x83\x93\x83\x67\x92\xCA\x92\x6D\x82\xF0\x8F\x88"
    "\x97\x9D\x82\xB7\x82\xE9\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45"
    "\x82\xCC\x93\x6F\x98\x5E\x82\xC9\x8E\xB8\x94\x73 : DirectShow";

class DirectShowMovieBackend final : public IDirectMovieBackend
{
public:
    ~DirectShowMovieBackend() override
    {
        ReleaseAll();
    }

    HRESULT Acquire(const DirectMovieInterface interfaceId) override
    {
        switch (interfaceId)
        {
        case DirectMovieInterface::GraphBuilder:
            return CoCreateInstance(
                CLSID_FilterGraph,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_IGraphBuilder,
                reinterpret_cast<void**>(&graphBuilder_));
        case DirectMovieInterface::MediaControl:
            return graphBuilder_->QueryInterface(
                IID_IMediaControl,
                reinterpret_cast<void**>(&mediaControl_));
        case DirectMovieInterface::MediaSeeking:
            return graphBuilder_->QueryInterface(
                IID_IMediaSeeking,
                reinterpret_cast<void**>(&mediaSeeking_));
        case DirectMovieInterface::MediaPosition:
            return graphBuilder_->QueryInterface(
                IID_IMediaPosition,
                reinterpret_cast<void**>(&mediaPosition_));
        case DirectMovieInterface::MediaEventEx:
            return graphBuilder_->QueryInterface(
                IID_IMediaEventEx,
                reinterpret_cast<void**>(&mediaEvent_));
        case DirectMovieInterface::VideoWindow:
            return graphBuilder_->QueryInterface(
                IID_IVideoWindow,
                reinterpret_cast<void**>(&videoWindow_));
        case DirectMovieInterface::BasicVideo:
            return graphBuilder_->QueryInterface(
                IID_IBasicVideo,
                reinterpret_cast<void**>(&basicVideo_));
        case DirectMovieInterface::BasicAudio:
            return graphBuilder_->QueryInterface(
                IID_IBasicAudio,
                reinterpret_cast<void**>(&basicAudio_));
        }
        return E_INVALIDARG;
    }

    bool Has(const DirectMovieInterface interfaceId) const noexcept override
    {
        switch (interfaceId)
        {
        case DirectMovieInterface::GraphBuilder:
            return graphBuilder_ != nullptr;
        case DirectMovieInterface::MediaControl:
            return mediaControl_ != nullptr;
        case DirectMovieInterface::MediaSeeking:
            return mediaSeeking_ != nullptr;
        case DirectMovieInterface::MediaPosition:
            return mediaPosition_ != nullptr;
        case DirectMovieInterface::MediaEventEx:
            return mediaEvent_ != nullptr;
        case DirectMovieInterface::VideoWindow:
            return videoWindow_ != nullptr;
        case DirectMovieInterface::BasicVideo:
            return basicVideo_ != nullptr;
        case DirectMovieInterface::BasicAudio:
            return basicAudio_ != nullptr;
        }
        return false;
    }

    void ReleaseAll() noexcept override
    {
        // 4C08C0/4A3EA0：先解除窗口和事件绑定，再严格按此顺序 Release。
        if (videoWindow_ != nullptr)
            static_cast<void>(videoWindow_->put_Visible(OAFALSE));
        if (mediaEvent_ != nullptr)
            static_cast<void>(mediaEvent_->SetNotifyWindow(0, 0, 0));

        ReleaseInterface(basicAudio_);
        ReleaseInterface(basicVideo_);
        ReleaseInterface(videoWindow_);
        ReleaseInterface(mediaEvent_);
        ReleaseInterface(mediaPosition_);
        ReleaseInterface(mediaSeeking_);
        ReleaseInterface(mediaControl_);
        ReleaseInterface(graphBuilder_);
    }

    HRESULT RenderFile(const wchar_t* const path) override
    {
        return graphBuilder_->RenderFile(path, nullptr);
    }

    HRESULT PutOwner(const HWND window) override
    {
        return videoWindow_->put_Owner(reinterpret_cast<OAHWND>(window));
    }

    HRESULT PutWindowStyle(const long style) override
    {
        return videoWindow_->put_WindowStyle(style);
    }

    HRESULT PutMessageDrain(const HWND window) override
    {
        return videoWindow_->put_MessageDrain(reinterpret_cast<OAHWND>(window));
    }

    HRESULT SetWindowPosition(const DirectMoviePosition& position) override
    {
        return videoWindow_->SetWindowPosition(
            position.x,
            position.y,
            position.width,
            position.height);
    }

    HRESULT SetNotifyWindow(
        const HWND window,
        const long message,
        const LONG_PTR instanceData) override
    {
        return mediaEvent_->SetNotifyWindow(
            reinterpret_cast<OAHWND>(window),
            message,
            instanceData);
    }

    HRESULT Run() override
    {
        return mediaControl_->Run();
    }

    HRESULT Pause() override
    {
        return mediaControl_->Pause();
    }

    HRESULT Stop() override
    {
        return mediaControl_->Stop();
    }

    HRESULT SeekToBeginning() override
    {
        LONGLONG position = 0;
        return mediaSeeking_->SetPositions(
            &position,
            kAbsolutePositioning,
            nullptr,
            0u);
    }

    HRESULT GetVolume(long& volume) override
    {
        return basicAudio_->get_Volume(&volume);
    }

    HRESULT PutVolume(const long volume) override
    {
        return basicAudio_->put_Volume(volume);
    }

    HRESULT PutVisible(const bool visible) override
    {
        return videoWindow_->put_Visible(visible ? OATRUE : OAFALSE);
    }

    HRESULT PutFullScreenMode(const bool enabled) override
    {
        return videoWindow_->put_FullScreenMode(enabled ? OATRUE : OAFALSE);
    }
    HRESULT SetWindowForeground(const bool focus) override
    {
        return videoWindow_->SetWindowForeground(focus ? OATRUE : OAFALSE);
    }

    HRESULT GetEvent(
        long& eventCode,
        LONG_PTR& parameter1,
        LONG_PTR& parameter2,
        const long timeoutMilliseconds) override
    {
        return mediaEvent_->GetEvent(
            &eventCode,
            &parameter1,
            &parameter2,
            timeoutMilliseconds);
    }

    HRESULT FreeEventParams(
        const long eventCode,
        const LONG_PTR parameter1,
        const LONG_PTR parameter2) override
    {
        return mediaEvent_->FreeEventParams(eventCode, parameter1, parameter2);
    }

private:
    template <typename Interface>
    static void ReleaseInterface(Interface*& value) noexcept
    {
        if (value == nullptr)
            return;
        value->Release();
        value = nullptr;
    }

    IGraphBuilder* graphBuilder_ = nullptr;
    IMediaControl* mediaControl_ = nullptr;
    IMediaSeeking* mediaSeeking_ = nullptr;
    IMediaPosition* mediaPosition_ = nullptr;
    IBasicAudio* basicAudio_ = nullptr;
    IBasicVideo* basicVideo_ = nullptr;
    IMediaEventEx* mediaEvent_ = nullptr;
    IVideoWindow* videoWindow_ = nullptr;
};
}

std::unique_ptr<DirectMovieRuntime> DirectMovieRuntime::CreateProduction(
    IDirectMovieHost& host)
{
    return std::make_unique<DirectMovieRuntime>(
        host,
        std::make_unique<DirectShowMovieBackend>(),ProcessWinHookRootNativeOwner(),ProcessWinHookNativeServices());
}

DirectMovieRuntime::DirectMovieRuntime(
    IDirectMovieHost& host,
    std::unique_ptr<IDirectMovieBackend> backend)
    : host_(&host), backend_(std::move(backend))
{
    AcquireInterfaces();
}

DirectMovieRuntime::~DirectMovieRuntime()
{
    ReleaseInterfaces();
    // 4A3F8D..4A3F97：释放最后一个COM接口之后才注销，不在清root槽时提前移除。
    if(removeControlledHook_)removeControlledHook_(controlledHooks_,this);
    if(nativeHooks_)
    {
        auto* body=nativeHooks_->CaptureAt450E20();WinHookNativeScope scope(*nativeHookServices_);
        body->vptrAt00->removeAt04(body,static_cast<Platform::NativeWindowMessageHook*>(this));
    }
}

DirectMovieRuntime::DirectMovieRuntime(IDirectMovieHost& host,std::unique_ptr<IDirectMovieBackend> backend,
    WinHookRootNativeOwner& hooks,WinHookNativeServices& services)
    :host_(&host),backend_(std::move(backend)),nativeHooks_(&hooks),nativeHookServices_(&services)
{
    AcquireInterfaces();
    // 4A0C58完成Acquire后才450E20/Add；登记有真实第零消息虚槽的子对象，不强转旧析构槽。
    auto* body=hooks.CaptureAt450E20();WinHookNativeScope scope(services);
    body->vptrAt00->addAt00(body,static_cast<Platform::NativeWindowMessageHook*>(this));
}

void DirectMovieRuntime::AcquireInterfaces()
{
    for (std::size_t index = 0; index < kAcquireOrder.size(); ++index)
    {
        if (FAILED(backend_->Acquire(kAcquireOrder[index])))
        {
            host_->ShowDirectShowError(kAcquireErrorMessagesCp932[index]);
            throw static_cast<int>(index + 1u);
        }
    }
}

void DirectMovieRuntime::Load(
    const char* const pathCurrentCodePage,
    const bool loop)
{
    // 4BC733..4BC751：固定 CP_ACP、固定 260 WCHAR，且不检查转换返回值。
    std::array<wchar_t, 260> widePath{};
    static_cast<void>(MultiByteToWideChar(
        CP_ACP,
        0u,
        pathCurrentCodePage,
        -1,
        widePath.data(),
        static_cast<int>(widePath.size())));

    if (FAILED(backend_->RenderFile(widePath.data())))
        ThrowLoadError(1, kRenderFileErrorCp932);

    loop_ = loop;
    const HWND mainWindow = host_->MainWindow();
    if (FAILED(backend_->PutOwner(mainWindow)))
        ThrowLoadError(2, kOwnerErrorCp932);
    if (FAILED(backend_->PutWindowStyle(kMovieWindowStyle)))
        ThrowLoadError(3, kWindowStyleErrorCp932);

    // 4BC7A3..4BC7AC：put_MessageDrain 的 HRESULT 明确被忽略。
    static_cast<void>(backend_->PutMessageDrain(mainWindow));
    if (FAILED(backend_->SetWindowPosition(host_->CurrentVideoPosition())))
        ThrowLoadError(4, kWindowPositionErrorCp932);
    if (FAILED(backend_->SetNotifyWindow(
            mainWindow,
            static_cast<long>(kDirectShowNotifyMessage),
            0)))
    {
        ThrowLoadError(5, kNotifyWindowErrorCp932);
    }

    state_ = DirectMovieState::Prepared;
    Run();
}

void DirectMovieRuntime::Run()
{
    if (state_ == DirectMovieState::Inactive)
        return;

    if (backend_->Has(DirectMovieInterface::BasicAudio))
    {
        long currentVolume = 0;
        const HRESULT getVolumeResult = backend_->GetVolume(currentVolume);
        if (getVolumeResult != E_NOTIMPL && SUCCEEDED(getVolumeResult))
        {
            const long targetVolume = muted_ ? kMutedVolume : volume_;
            static_cast<void>(backend_->PutVolume(targetVolume));
        }
    }

    // 4BCC94..4BCCAB：+94 为 SetWindowForeground，不是 +4C 的 put_Visible。
    static_cast<void>(backend_->Run());
    static_cast<void>(backend_->SetWindowForeground(true));
    state_ = DirectMovieState::Running;
}

void DirectMovieRuntime::TogglePause()
{
    if (!backend_->Has(DirectMovieInterface::MediaControl))
        return;

    if (state_ == DirectMovieState::Running)
    {
        if (SUCCEEDED(backend_->Pause()))
            state_ = DirectMovieState::Paused;
        return;
    }

    if (state_ == DirectMovieState::Prepared ||
        state_ == DirectMovieState::Paused)
    {
        Run();
    }
}

void DirectMovieRuntime::RewindToPrepared()
{
    if (!backend_->Has(DirectMovieInterface::MediaControl) ||
        !backend_->Has(DirectMovieInterface::MediaSeeking))
    {
        return;
    }
    if (state_ != DirectMovieState::Running &&
        state_ != DirectMovieState::Paused)
    {
        return;
    }

    static_cast<void>(backend_->Stop());
    state_ = DirectMovieState::Prepared;
    static_cast<void>(backend_->SeekToBeginning());
    static_cast<void>(backend_->Pause());
}

void DirectMovieRuntime::FinishPlayback()
{
    if (backend_->Has(DirectMovieInterface::MediaControl) &&
        state_ == DirectMovieState::Running &&
        SUCCEEDED(backend_->Pause()))
    {
        state_ = DirectMovieState::Inactive;
    }
}

void DirectMovieRuntime::PulseFullScreenMode()
{
    if (!backend_->Has(DirectMovieInterface::VideoWindow))
        return;

    // 4B501A..4B5030：不是持久切换，而是同次调用内 true -> false。
    static_cast<void>(backend_->PutFullScreenMode(true));
    static_cast<void>(backend_->PutFullScreenMode(false));
}

void DirectMovieRuntime::Resize()
{
    if (backend_->Has(DirectMovieInterface::VideoWindow))
        static_cast<void>(backend_->SetWindowPosition(host_->CurrentVideoPosition()));
}

void DirectMovieRuntime::ReleaseInterfaces() noexcept
{
    if (interfacesReleased_)
        return;
    backend_->ReleaseAll();
    interfacesReleased_ = true;
}

void DirectMovieRuntime::SetVolume(const long directShowVolume) noexcept
{
    // 4A0E68..4A0E80 已在调用方裁到 [-10000, 0]；对象本身只保存所得值。
    volume_ = directShowVolume;
}

void DirectMovieRuntime::SetMuted(const bool muted) noexcept
{
    muted_ = muted;
}

long DirectMovieRuntime::Volume() const noexcept
{
    return volume_;
}

bool DirectMovieRuntime::Muted() const noexcept
{
    return muted_;
}

bool DirectMovieRuntime::Looping() const noexcept
{
    return loop_;
}

DirectMovieState DirectMovieRuntime::State() const noexcept
{
    return state_;
}

bool DirectMovieRuntime::InterfacesReleased() const noexcept
{
    return interfacesReleased_;
}

bool DirectMovieRuntime::HandleWindowMessage(
    const HWND window,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam,
    LRESULT& result,
    const bool alreadyHandled)
{
    static_cast<void>(window);
    static_cast<void>(wParam);
    static_cast<void>(lParam);

    if (alreadyHandled)
        return false;

    if (message == kDirectShowNotifyMessage)
    {
        ProcessDirectShowEvents();
        result = 0;
        return true;
    }

    if (message != WM_DESTROY)
        return false;

    if (backend_->Has(DirectMovieInterface::VideoWindow))
    {
        if (state_ == DirectMovieState::Running)
        {
            FinishPlayback();
            // 4BF49D：即使 Pause 失败，WM_DESTROY 仍强制写回 inactive。
            state_ = DirectMovieState::Inactive;
        }
        static_cast<void>(backend_->PutVisible(false));
        static_cast<void>(backend_->PutOwner(nullptr));
    }

    result = 0;
    return true;
}

bool DirectMovieRuntime::Dispatch(
    const unsigned int messageMapId,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam,
    LRESULT& result)
{
    static_cast<void>(messageMapId);
    return HandleWindowMessage(
        host_->MainWindow(),
        message,
        wParam,
        lParam,
        result,
        false);
}
std::int32_t DirectMovieRuntime::ProcessWindowMessage(HWND window,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,std::uint32_t alreadyHandled)
{
    // 4BF3C6检查整个arg14；HWND来自主窗口分派，不能重新向host取另一个窗口。
    return HandleWindowMessage(window,message,w,l,*result,alreadyHandled!=0);
}

[[noreturn]] void DirectMovieRuntime::ThrowLoadError(
    const int errorCode,
    const char* const messageCp932)
{
    host_->ShowDirectShowError(messageCp932);
    throw errorCode;
}

void DirectMovieRuntime::ProcessDirectShowEvents()
{
    if (state_ != DirectMovieState::Running &&
        state_ != DirectMovieState::Paused)
    {
        return;
    }
    if (!backend_->Has(DirectMovieInterface::MediaEventEx))
        return;

    long eventCode = 0;
    LONG_PTR parameter1 = 0;
    LONG_PTR parameter2 = 0;
    while (SUCCEEDED(backend_->GetEvent(
        eventCode,
        parameter1,
        parameter2,
        kEventPollTimeoutMilliseconds)))
    {
        // 4BF411..4BF423：原程序先释放参数，再解释 EC_COMPLETE。
        static_cast<void>(backend_->FreeEventParams(
            eventCode,
            parameter1,
            parameter2));
        if (eventCode == EC_COMPLETE)
        {
            if (loop_)
                static_cast<void>(backend_->SeekToBeginning());
            else
                state_ = DirectMovieState::Inactive;
        }
    }
}
}
