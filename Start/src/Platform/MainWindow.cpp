#include <Platform/MainWindow.h>

namespace KaGuYa::Start::Platform
{
bool Win32MainWindowApi::IsVisible(const HWND window) const
{
    return IsWindowVisible(window) != FALSE;
}

bool Win32MainWindowApi::IsIconic(const HWND window) const
{
    return ::IsIconic(window) != FALSE;
}

void Win32MainWindowApi::OpenIconicWindow(const HWND window)
{
    OpenIcon(window);
}

void Win32MainWindowApi::MinimizeWindow(const HWND window)
{
    CloseWindow(window);
}

void Win32MainWindowApi::ReadPlacement(const HWND window, WINDOWPLACEMENT& placement)
{
    GetWindowPlacement(window, &placement);
}

void Win32MainWindowApi::WritePlacement(const HWND window, const WINDOWPLACEMENT& placement)
{
    SetWindowPlacement(window, &placement);
}

LRESULT Win32MainWindowApi::CallPrevious(
    const WNDPROC previousProcedure,
    const HWND window,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam)
{
    return CallWindowProcA(previousProcedure, window, message, wParam, lParam);
}

MainWindow::MainWindow(
    MessageLoop& messageLoop,
    IMessageFilter& messageFilter,
    IIdleHandler& idleHandler,
    IWindowMessageHook& memberMenuMap,
    IWindowHookMessageSource& windowHooks,
    IMainWindowRuntime& runtime,
    IMainWindowSubsystemBridge& subsystems,
    IMainWindowApi& windowApi)
    : messageLoop_(messageLoop),
      messageFilter_(messageFilter),
      idleHandler_(idleHandler),
      memberMenuMap_(memberMenuMap),
      windowHooks_(windowHooks),
      runtime_(runtime),
      subsystems_(subsystems),
      windowApi_(windowApi)
{
}

MainWindow::MainWindow(MessageLoop& loop,IMessageFilter& filter,IIdleHandler& idle,IWindowMessageHook& menu,
    IMainWindowRuntime& runtime,IMainWindowSubsystemBridge& subsystems,IMainWindowApi& api)
    :MainWindow(loop,filter,idle,menu,DefaultWindowHookMessageSource(),runtime,subsystems,api){}

void MainWindow::Attach(const HWND window, const WNDPROC previousProcedure)
{
    window_ = window;
    previousProcedure_ = previousProcedure;
}

bool MainWindow::Dispatch(
    const unsigned int messageMapId,
    const UINT message,
    const WPARAM wParam,
    const LPARAM lParam,
    LRESULT& result)
{
    // Start.exe:42E810 只在 message-map id 为零时处理这一组主窗口消息。
    if (messageMapId != 0)
    {
        return false;
    }

    switch (message)
    {
    case WM_CREATE:
        messageLoop_.AddMessageFilter(&messageFilter_);
        messageLoop_.AddIdleHandler(&idleHandler_);
        handlersRegistered_ = true;
        paintSuppressed_ = false;
        result = 0;
        return true;

    case WM_DESTROY:
        if (handlersRegistered_)
        {
            // 42E810 从两个线程级数组中各删除首个匹配项，然后继续后续窗口过程链。
            messageLoop_.RemoveMessageFilter(&messageFilter_);
            messageLoop_.RemoveIdleHandler(&idleHandler_);
            handlersRegistered_ = false;
        }
        // 42E9C4写result=1后仍进入成员映射和全局hook，不在这里提前返回。
        result = 1;
        break;

    case WM_CLOSE:
        result = OnClose(wParam, lParam);
        return true;

    case WM_ACTIVATEAPP:
        result = OnActivateApp(wParam, lParam);
        return true;

    case WM_SIZE:
        result = OnSize(wParam, lParam);
        return true;

    default:
        break;
    }
    // 42E9CD先调用MainFrame+54成员映射；未处理才到42EE81的全局CWinHook。
    if (memberMenuMap_.Dispatch(messageMapId, message, wParam, lParam, result))
        return true;
    return windowHooks_.DispatchNative(window_,message,wParam,lParam,&result,0)!=0;
}

LRESULT MainWindow::Forward(const UINT message, const WPARAM wParam, const LPARAM lParam)
{
    return windowApi_.CallPrevious(previousProcedure_, window_, message, wParam, lParam);
}

LRESULT MainWindow::OnClose(const WPARAM wParam, const LPARAM lParam)
{
    // 423750 对不可见窗口和已经进入退出状态的窗口不做确认，直接转发旧过程。
    if (!windowApi_.IsVisible(window_) || runtime_.QuitRequested())
    {
        return Forward(WM_CLOSE, wParam, lParam);
    }

    if (windowApi_.IsIconic(window_))
    {
        windowApi_.OpenIconicWindow(window_);
    }

    if (runtime_.CloseSuppressed())
    {
        return 0;
    }

    if (runtime_.CloseConfirmationEnabled() && runtime_.ShowCloseConfirmation() != IDYES)
    {
        return 0;
    }

    return Forward(WM_CLOSE, wParam, lParam);
}

LRESULT MainWindow::OnActivateApp(const WPARAM wParam, const LPARAM lParam)
{
    // 42103E..4210E5先通知Sound；窗口模式不影响这次调用和最终转发。
    subsystems_.ActivateAppRestore(wParam);

    bool resetRenderer = false;
    if (runtime_.FullscreenModeEnabled())
    {
        if (wParam != FALSE)
        {
            if (windowApi_.IsIconic(window_))
            {
                runtime_.SynchronizeSavedNormalPosition();
                WINDOWPLACEMENT placement{};
                placement.length = sizeof(placement);
                windowApi_.ReadPlacement(window_, placement);
                placement.showCmd = SW_SHOWNORMAL;
                placement.rcNormalPosition = runtime_.SavedNormalPosition();
                windowApi_.WritePlacement(window_, placement);
                resetRenderer = true;
            }
        }
        else if (runtime_.MinimizeWhenDeactivated() && !windowApi_.IsIconic(window_))
        {
            windowApi_.MinimizeWindow(window_);
            resetRenderer = true;
        }
    }

    if (resetRenderer)
    {
        subsystems_.RenderReset();
    }

    return Forward(WM_ACTIVATEAPP, wParam, lParam);
}

LRESULT MainWindow::OnSize(const WPARAM wParam, const LPARAM lParam)
{
    if (windowApi_.IsVisible(window_) &&
        runtime_.FullscreenModeEnabled() &&
        wParam != SIZE_MINIMIZED)
    {
        if (runtime_.HasResizeTarget())
        {
            runtime_.NotifyResizeTarget();
        }
        runtime_.UpdateVisibleNativeWindowLayout();
    }

    return Forward(WM_SIZE, wParam, lParam);
}
}
