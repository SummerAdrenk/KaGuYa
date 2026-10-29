#pragma once
#include <Runtime/LegacyDisplayDeviceOwner.h>
#include <Runtime/MainFrameApplicationNative.h>
#include <Runtime/MainFrameDisplayNative.h>
#include <exception>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class EnvironmentStartupRuntime;
class IGraphicsPresentNativeApi;

// 正文外的明确服务合同；现有MainFrame接口/测试子类不增加纯虚槽。
class IRootLegacyDisplaySettingsRuntime
{
public:
    virtual ~IRootLegacyDisplaySettingsRuntime() = default;
    virtual void AfterPreWindow() = 0;
    virtual void AfterPostWindow() = 0;
    virtual bool Dispatch(UINT, WPARAM, LPARAM, LRESULT&) noexcept = 0;
    virtual bool PreTranslate(MSG&) = 0;
    virtual bool Poll() = 0;
    virtual BYTE SetWindowMode(WindowModeView, BYTE, LONG, LONG) noexcept = 0;
    virtual void Activate(HWND, bool, IMainFrameNativeWindowApi&, IMainFrameNativeModules&) noexcept = 0;
    // false表示801F确认栈尚未退出，关闭须延迟；不能在活跃receiver栈上销毁它。
    virtual bool BeforeClose(HWND) noexcept = 0;
    virtual void BeforeWindowRelease() noexcept = 0;
    virtual void BeforeSubsystemRelease() noexcept = 0;
    virtual void FinishRun() = 0;
};

LegacyDisplaySettingsFamily CompiledLegacyDisplaySettingsFamily() noexcept;
class RootLegacyDisplaySettingsScope final
{
public:
    RootLegacyDisplaySettingsScope(LegacyDisplaySettingsFamily, IRootLegacyDisplaySettingsRuntime*) noexcept;
    ~RootLegacyDisplaySettingsScope() noexcept;
    RootLegacyDisplaySettingsScope(const RootLegacyDisplaySettingsScope&) = delete;
    RootLegacyDisplaySettingsScope& operator=(const RootLegacyDisplaySettingsScope&) = delete;
private:
    friend IRootLegacyDisplaySettingsRuntime* BoundRootLegacyDisplaySettings();
    LegacyDisplaySettingsFamily family_;
    IRootLegacyDisplaySettingsRuntime* runtime_;
    RootLegacyDisplaySettingsScope* previous_;
};
// Disabled返回nullptr；启用但无scope/无服务抛出，不回退现代边框全屏。
IRootLegacyDisplaySettingsRuntime* BoundRootLegacyDisplaySettings();

// 装配器代理原六入口，在477620真实释放之前关闭receiver；不改原正文/EH布局。
class RootLegacyDisplayApplicationSource final : public IMainFrameApplicationNativeSource
{
public:
    RootLegacyDisplayApplicationSource(IMainFrameApplicationNativeSource& base,
        IRootLegacyDisplaySettingsRuntime* legacy) noexcept : base_(base), legacy_(legacy) {}
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override { return base_.RootAt6198C4(); }
    EnvironmentRootStorage* EnvironmentAt4180C0() override { return base_.EnvironmentAt4180C0(); }
    bool InitializeBeforeWindowAt4778B0(void*) override;
    bool InitializeAfterWindowAt489C90(void*) override;
    void ReleaseSubsystemsAt477620(void*) override;
private:
    IMainFrameApplicationNativeSource& base_;
    IRootLegacyDisplaySettingsRuntime* legacy_;
};

// 仅装饰Previous(WM_CLOSE)边界：原退出确认取消时根本不会到这里。
class RootLegacyDisplayWindowApi final : public IMainFrameNativeWindowApi
{
public:
    RootLegacyDisplayWindowApi(IMainFrameNativeWindowApi& base,
        IRootLegacyDisplaySettingsRuntime* legacy) noexcept : base_(base), legacy_(legacy) {}
    HDC BeginPaint(HWND w, PAINTSTRUCT* p) override { return base_.BeginPaint(w,p); }
    BOOL EndPaint(HWND w, const PAINTSTRUCT* p) override { return base_.EndPaint(w,p); }
    BOOL ValidateRect(HWND w, const RECT* p) override { return base_.ValidateRect(w,p); }
    BOOL AdjustWindowRectEx(RECT* r,DWORD s,BOOL m,DWORD e) override { return base_.AdjustWindowRectEx(r,s,m,e); }
    BOOL IsWindowVisible(HWND w) override { return base_.IsWindowVisible(w); }
    BOOL IsIconic(HWND w) override { return base_.IsIconic(w); }
    BOOL OpenIcon(HWND w) override { return base_.OpenIcon(w); }
    BOOL CloseWindow(HWND w) override { return base_.CloseWindow(w); }
    BOOL GetWindowPlacement(HWND w,WINDOWPLACEMENT* p) override { return base_.GetWindowPlacement(w,p); }
    BOOL SetWindowPlacement(HWND w,const WINDOWPLACEMENT* p) override { return base_.SetWindowPlacement(w,p); }
    int MessageBoxA(HWND w,const char* t,const char* c,UINT f) override { return base_.MessageBoxA(w,t,c,f); }
    BOOL PostMessageA(HWND w,UINT m,WPARAM p,LPARAM l) override { return base_.PostMessageA(w,m,p,l); }
    LRESULT CallWindowProcA(WNDPROC,HWND,UINT,WPARAM,LPARAM) override;
private:
    IMainFrameNativeWindowApi& base_;
    IRootLegacyDisplaySettingsRuntime* legacy_;
};

// 长期MainFrame服务拥有此对象；Root/Env只是有身份校验的借用，析构不Cancel/切屏。
class RootLegacyDisplaySettingsRuntime final : public IRootLegacyDisplaySettingsRuntime
{
public:
    RootLegacyDisplaySettingsRuntime(LegacyDisplaySettingsFamily, RootRuntimeOwner&,
        EnvironmentStartupRuntime&, IGraphicsWindowModeApi&, IGraphicsPresentNativeApi&,
        ILegacyDisplayDeviceApi&, IWindowModePlatformApi&, IMainFrameNativeWindowApi&,
        const HINSTANCE& resources) noexcept;
    ~RootLegacyDisplaySettingsRuntime() override;
    RootLegacyDisplaySettingsRuntime(const RootLegacyDisplaySettingsRuntime&) = delete;
    RootLegacyDisplaySettingsRuntime& operator=(const RootLegacyDisplaySettingsRuntime&) = delete;
    [[nodiscard]] LegacyDisplaySettingsFamily Family() const noexcept { return family_; }
    [[nodiscard]] IRootLegacyDisplaySettingsRuntime* EnabledBinding() noexcept
    { return family_ == LegacyDisplaySettingsFamily::Disabled ? nullptr : this; }
    void AfterPreWindow() override;
    void AfterPostWindow() override;
    bool Dispatch(UINT,WPARAM,LPARAM,LRESULT&) noexcept override;
    bool PreTranslate(MSG&) override;
    bool Poll() override;
    BYTE SetWindowMode(WindowModeView,BYTE,LONG,LONG) noexcept override;
    void Activate(HWND,bool,IMainFrameNativeWindowApi&,IMainFrameNativeModules&) noexcept override;
    bool BeforeClose(HWND) noexcept override;
    void BeforeWindowRelease() noexcept override;
    void BeforeSubsystemRelease() noexcept override;
    void FinishRun() override;
    void ObserveRunUnwind() noexcept;
    // 失败责任仍由本对象持有；即使Root/Env已退役，显式重试也只用设备API，不读它们。
    [[nodiscard]] bool HasOutstandingRestore() const noexcept;
    [[nodiscard]] LegacyDisplayOwnershipResult RetryDesktopRestore();
private:
    struct State;
    struct Operation;
    State& RequireState();
    void RememberFailure(const char*, std::exception_ptr) noexcept;
    void Quiesce() noexcept;
    void LeaveDispatch() noexcept;
    LegacyDisplaySettingsFamily family_;
    RootRuntimeOwner& root_;
    EnvironmentStartupRuntime& environment_;
    IGraphicsWindowModeApi& graphics_;
    IGraphicsPresentNativeApi& present_;
    ILegacyDisplayDeviceApi& display_;
    IWindowModePlatformApi& platform_;
    IMainFrameNativeWindowApi& windows_;
    const HINSTANCE& resources_;
    std::unique_ptr<State> state_;
    std::exception_ptr failure_;
    unsigned dispatchDepth_ = 0;
    HWND deferredClose_ = nullptr;
    bool forcedRelease_ = false;
};
}
