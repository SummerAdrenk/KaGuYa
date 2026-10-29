#include <Runtime/MainFrameTerminalNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
MainFrameTerminalNativeRuntime::MainFrameTerminalNativeRuntime(IMainFrameNativeRootSource& roots,
    IMainFrameNativeWindowApi& windows,IMainFrameNativeModules& modules,IMainFrameCloseNativeDialogs& dialogs,
    MainFrameUiNativeServices ui,IMainFrameNativeCommandHandlers& commands)noexcept
    :roots_(roots),windows_(windows),modules_(modules),dialogs_(dialogs),ui_(ui),commands_(commands){}
LRESULT MainFrameTerminalNativeRuntime::CloseAt423750(MainFrameNativeBody90h& frame,UINT m,WPARAM w,LPARAM l,BOOL*)
{return CloseMainFrameNativeAt423750(frame,m,w,l,roots_,windows_,dialogs_);}
LRESULT MainFrameTerminalNativeRuntime::DisplayChangeAt424230(MainFrameNativeBody90h& frame,UINT m,WPARAM w,LPARAM l,BOOL*)
{return DisplayChangeMainFrameNativeAt424230(frame,m,w,l,roots_,windows_,modules_);}
LRESULT MainFrameTerminalNativeRuntime::ActivateAt420F90(MainFrameNativeBody90h& frame,UINT m,WPARAM w,LPARAM l,BOOL*)
{return ActivateMainFrameNativeAt420F90(frame,m,w,l,roots_,windows_,modules_);}
LRESULT MainFrameTerminalNativeRuntime::DeviceChangeAt424070(MainFrameNativeBody90h& frame,UINT m,WPARAM w,LPARAM l,BOOL* h)
{return commands_.DeviceChangeAt424070(frame,m,w,l,h);}
LRESULT MainFrameTerminalNativeRuntime::CommandAt428C10(MainFrameNativeBody90h& frame,DWORD n,DWORD id,LPARAM l,BOOL* h)
{return commands_.CommandAt428C10(frame,n,id,l,h);}
LRESULT MainFrameTerminalNativeRuntime::UpdateMenuAt428430(UpdateUiNativeBody18h& frame,UINT m,HMENU w,LPARAM l,BOOL* h)
{return UpdateMainFrameMenuNativeAt428430(frame,m,w,l,h,ui_);}
LRESULT MainFrameTerminalNativeRuntime::NotifyAt423550(MainFrameNativeBody90h& frame,WPARAM w,LPARAM l,BOOL* h)
{return commands_.ChevronAt423550(frame,w,l,h);}
void MainFrameTerminalNativeRuntime::UpdateUiAt439100(MainFrameNativeBody90h& frame,BOOL resize)
{ResizeMainFrameUiNativeAt439100(frame,resize,ui_);}
BOOL MainFrameTerminalNativeRuntime::FrameBaseAt42DC50(MainFrameNativeBody90h& frame,HWND window,UINT m,
    WPARAM w,LPARAM l,LRESULT* r,DWORD id)
{return ProcessFrameBaseNativeAt42DC50(frame,window,m,w,l,r,id,ui_);}
}
