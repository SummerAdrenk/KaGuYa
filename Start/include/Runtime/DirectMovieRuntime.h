#pragma once

#include <Platform/Input.h>

#include <Windows.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class WinHookRootNativeOwner;
struct WinHookNativeServices;
enum class DirectMovieInterface : std::uint8_t
{
    GraphBuilder,
    MediaControl,
    MediaSeeking,
    MediaPosition,
    MediaEventEx,
    VideoWindow,
    BasicVideo,
    BasicAudio
};

enum class DirectMovieState : std::uint32_t
{
    Inactive = 0u,
    Running = 1u,
    Prepared = 2u,
    Paused = 3u
};

struct DirectMoviePosition
{
    long x = 0;
    long y = 0;
    long width = 0;
    long height = 0;
};

class IDirectMovieHost
{
public:
    virtual ~IDirectMovieHost() = default;

    [[nodiscard]] virtual HWND MainWindow() const = 0;

    // Start.exe:4AFFF0 返回的是 x/y/width/height，而不是 Win32 的 right/bottom。
    [[nodiscard]] virtual DirectMoviePosition CurrentVideoPosition() const = 0;

    // messageCp932 指向样本 18 原始 CP932 文本；主线必须按原 0x10 图标语义显示。
    virtual void ShowDirectShowError(const char* messageCp932) = 0;
};

// 该边界逐项对应样本 18 的 DirectShow COM 调用。Acquire 的调用顺序属于行为契约。
class IDirectMovieBackend
{
public:
    virtual ~IDirectMovieBackend() = default;

    [[nodiscard]] virtual HRESULT Acquire(DirectMovieInterface interfaceId) = 0;
    [[nodiscard]] virtual bool Has(DirectMovieInterface interfaceId) const noexcept = 0;
    virtual void ReleaseAll() noexcept = 0;

    [[nodiscard]] virtual HRESULT RenderFile(const wchar_t* path) = 0;
    [[nodiscard]] virtual HRESULT PutOwner(HWND window) = 0;
    [[nodiscard]] virtual HRESULT PutWindowStyle(long style) = 0;
    [[nodiscard]] virtual HRESULT PutMessageDrain(HWND window) = 0;
    [[nodiscard]] virtual HRESULT SetWindowPosition(
        const DirectMoviePosition& position) = 0;
    [[nodiscard]] virtual HRESULT SetNotifyWindow(
        HWND window,
        long message,
        LONG_PTR instanceData) = 0;

    [[nodiscard]] virtual HRESULT Run() = 0;
    [[nodiscard]] virtual HRESULT Pause() = 0;
    [[nodiscard]] virtual HRESULT Stop() = 0;
    [[nodiscard]] virtual HRESULT SeekToBeginning() = 0;

    [[nodiscard]] virtual HRESULT GetVolume(long& volume) = 0;
    [[nodiscard]] virtual HRESULT PutVolume(long volume) = 0;
    [[nodiscard]] virtual HRESULT PutVisible(bool visible) = 0;
    [[nodiscard]] virtual HRESULT PutFullScreenMode(bool enabled) = 0;
    [[nodiscard]] virtual HRESULT SetWindowForeground(bool focus) = 0;

    [[nodiscard]] virtual HRESULT GetEvent(
        long& eventCode,
        LONG_PTR& parameter1,
        LONG_PTR& parameter2,
        long timeoutMilliseconds) = 0;
    [[nodiscard]] virtual HRESULT FreeEventParams(
        long eventCode,
        LONG_PTR parameter1,
        LONG_PTR parameter2) = 0;
};

// Scene::CDirectMovie 的行为对象。COM 接口布局由生产 backend 持有，状态字段严格保留原四态。
class DirectMovieRuntime final : public Platform::IWindowMessageHook,public Platform::NativeWindowMessageHook
{
public:
    // 使用真实 DirectShow Filter Graph；调用线程必须已按原 Start 启动链初始化 COM。
    [[nodiscard]] static std::unique_ptr<DirectMovieRuntime> CreateProduction(
        IDirectMovieHost& host);

    // 注入入口只用于精确差分；构造过程仍会按原顺序调用八次 Acquire。
    DirectMovieRuntime(
        IDirectMovieHost& host,
        std::unique_ptr<IDirectMovieBackend> backend);
    DirectMovieRuntime(IDirectMovieHost& host, std::unique_ptr<IDirectMovieBackend> backend,
        Platform::WindowHookDispatcher& dispatcher);
    DirectMovieRuntime(IDirectMovieHost&,std::unique_ptr<IDirectMovieBackend>,WinHookRootNativeOwner&,WinHookNativeServices&);
    ~DirectMovieRuntime() override;

    DirectMovieRuntime(const DirectMovieRuntime&) = delete;
    DirectMovieRuntime& operator=(const DirectMovieRuntime&) = delete;

    // 4BC720：输入是当前 ACP 窄字符串，固定转换进 WCHAR[260]。
    // 初始化接口失败抛 1..8；Load 的五个受控失败点抛 1..5。
    void Load(const char* pathCurrentCodePage, bool loop);

    void Run();
    void TogglePause();
    void RewindToPrepared();
    void FinishPlayback();
    void PulseFullScreenMode();
    void Resize();
    void ReleaseInterfaces() noexcept;

    void SetVolume(long directShowVolume) noexcept;
    void SetMuted(bool muted) noexcept;

    [[nodiscard]] long Volume() const noexcept;
    [[nodiscard]] bool Muted() const noexcept;
    [[nodiscard]] bool Looping() const noexcept;
    [[nodiscard]] DirectMovieState State() const noexcept;
    [[nodiscard]] bool InterfacesReleased() const noexcept;

    // 4BF3C0 的完整入口；alreadyHandled 非零时必须立刻返回 false。
    [[nodiscard]] bool HandleWindowMessage(
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result,
        bool alreadyHandled);

    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result) override;
    std::int32_t ProcessWindowMessage(HWND,UINT,WPARAM,LPARAM,LRESULT*,std::uint32_t) override;

private:
    void AcquireInterfaces();
    [[noreturn]] void ThrowLoadError(int errorCode, const char* messageCp932);
    void ProcessDirectShowEvents();

    IDirectMovieHost* host_ = nullptr;
    std::unique_ptr<IDirectMovieBackend> backend_;
    // 4A0C67在Acquire成功后注册；dispatcher与host必须长于正文，不随Input先销毁。
    // 显式历史测试登记的退役回调；正式native构造不设置，不持第二套registry。
    void* controlledHooks_ = nullptr;
    void (*removeControlledHook_)(void*,DirectMovieRuntime*) = nullptr;
    WinHookRootNativeOwner* nativeHooks_ = nullptr;
    WinHookNativeServices* nativeHookServices_ = nullptr;
    bool interfacesReleased_ = false;

    // 原对象 +0x24 在本链中没有消费证据，保持中性零值且禁止业务命名。
    std::uint32_t neutralFieldAt24_ = 0u;
    bool loop_ = false;
    DirectMovieState state_ = DirectMovieState::Inactive;
    long volume_ = 0;
    bool muted_ = false;
};
}
