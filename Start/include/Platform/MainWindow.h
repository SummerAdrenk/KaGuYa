#pragma once

#include <Platform/Input.h>
#include <Platform/MessageLoop.h>

#include <Windows.h>

namespace KaGuYa::Start::Platform
{
class IMainWindowRuntime
{
public:
    virtual ~IMainWindowRuntime() = default;

    [[nodiscard]] virtual bool QuitRequested() const = 0;
    [[nodiscard]] virtual bool CloseSuppressed() const = 0;
    [[nodiscard]] virtual bool CloseConfirmationEnabled() const = 0;
    virtual INT_PTR ShowCloseConfirmation() = 0;

    [[nodiscard]] virtual bool FullscreenModeEnabled() const = 0;
    [[nodiscard]] virtual bool MinimizeWhenDeactivated() const = 0;
    virtual void SynchronizeSavedNormalPosition() = 0;
    [[nodiscard]] virtual RECT SavedNormalPosition() const = 0;

    [[nodiscard]] virtual bool HasResizeTarget() const = 0;
    virtual void NotifyResizeTarget() = 0;
    // 42ECB6传的是4875F0的空对象指针，不是窗口模式整数。
    virtual void UpdateVisibleNativeWindowLayout() = 0;
};

class IMainWindowSubsystemBridge
{
public:
    virtual ~IMainWindowSubsystemBridge() = default;
    // 420F90：激活先通知Sound，后续窗口分支才可能重置Graphics。
    virtual void ActivateAppRestore(WPARAM active) = 0;
    virtual void RenderReset() = 0;
};

class IMainWindowApi
{
public:
    virtual ~IMainWindowApi() = default;

    [[nodiscard]] virtual bool IsVisible(HWND window) const = 0;
    [[nodiscard]] virtual bool IsIconic(HWND window) const = 0;
    virtual void OpenIconicWindow(HWND window) = 0;
    virtual void MinimizeWindow(HWND window) = 0;
    virtual void ReadPlacement(HWND window, WINDOWPLACEMENT& placement) = 0;
    virtual void WritePlacement(HWND window, const WINDOWPLACEMENT& placement) = 0;
    virtual LRESULT CallPrevious(
        WNDPROC previousProcedure,
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam) = 0;
};

class Win32MainWindowApi final : public IMainWindowApi
{
public:
    [[nodiscard]] bool IsVisible(HWND window) const override;
    [[nodiscard]] bool IsIconic(HWND window) const override;
    void OpenIconicWindow(HWND window) override;
    void MinimizeWindow(HWND window) override;
    void ReadPlacement(HWND window, WINDOWPLACEMENT& placement) override;
    void WritePlacement(HWND window, const WINDOWPLACEMENT& placement) override;
    LRESULT CallPrevious(
        WNDPROC previousProcedure,
        HWND window,
        UINT message,
        WPARAM wParam,
        LPARAM lParam) override;
};

class MainWindow final
{
public:
    MainWindow(
        MessageLoop& messageLoop,
        IMessageFilter& messageFilter,
        IIdleHandler& idleHandler,
        IWindowMessageHook& memberMenuMap,
        IWindowHookMessageSource& windowHooks,
        IMainWindowRuntime& runtime,
        IMainWindowSubsystemBridge& subsystems,
        IMainWindowApi& windowApi);
    MainWindow(MessageLoop&,IMessageFilter&,IIdleHandler&,IWindowMessageHook&,
        IMainWindowRuntime&,IMainWindowSubsystemBridge&,IMainWindowApi&);

    void Attach(HWND window, WNDPROC previousProcedure);
    [[nodiscard]] bool Dispatch(
        unsigned int messageMapId,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT& result);

    [[nodiscard]] bool PaintSuppressed() const noexcept { return paintSuppressed_; }

private:
    LRESULT Forward(UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    LRESULT OnActivateApp(WPARAM wParam, LPARAM lParam);
    LRESULT OnSize(WPARAM wParam, LPARAM lParam);

    MessageLoop& messageLoop_;
    IMessageFilter& messageFilter_;
    IIdleHandler& idleHandler_;
    IWindowMessageHook& memberMenuMap_;
    IWindowHookMessageSource& windowHooks_;
    IMainWindowRuntime& runtime_;
    IMainWindowSubsystemBridge& subsystems_;
    IMainWindowApi& windowApi_;
    HWND window_ = nullptr;
    WNDPROC previousProcedure_ = nullptr;
    bool paintSuppressed_ = true;
    bool handlersRegistered_ = false;
};
}
