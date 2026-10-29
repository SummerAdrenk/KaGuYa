#include <Runtime/MainFrameDisplayNative.h>
#include <Runtime/WindowModeNative.h>
#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void* Capture(IMainFrameNativeRootSource& roots){roots.EnsureRootAt413BE0();return roots.RootAt6198C4();}
WindowModeNative2Ch* Mode(IMainFrameNativeRootSource& roots,void* root)
{
    const auto& ref=roots.ModeAt0C(root);const auto base=Read(ref.baseAt00);if(!base)return nullptr;
    const auto control=Read(ref.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    return reinterpret_cast<WindowModeNative2Ch*>(base+Read(ref.biasAt04)*stride);
}
LRESULT Previous(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,IMainFrameNativeWindowApi& windows)
{return windows.CallWindowProcA(Read(frame.windowAt00.previousAt20),Read(frame.windowAt00.windowAt04),message,w,l);}
// 原PE5AE9DC的40字节SJIS消息，保持ANSI调用，不随宿主代码页重编码。
const char graphicsError[]=
    "\x83\x66\x83\x6F\x83\x43\x83\x58\x83\x8D\x83\x58\x83\x67\x82\xAA"
    "\x94\xAD\x90\xB6\x82\xB5\x82\xBD\x82\xCC\x82\xC5\x8F\x49\x97\xB9"
    "\x82\xB5\x82\xDC\x82\xB7\x81\x49";
static_assert(sizeof(graphicsError)==41);
}
MainFrameNativeModules::MainFrameNativeModules(NativeSoundRuntimeLibrary& sound,GraphicsRootOwner& graphics,
    IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& windows)noexcept
    :sound_(sound),graphics_(graphics),strings_(strings),windows_(windows),screen_(graphics,strings,windows){}
void MainFrameNativeModules::ActivateSoundAt42103E(WPARAM active){sound_.ActivateAppRestoreIfLoaded(active);}
DWORD MainFrameNativeModules::CheckGraphicsDeviceAt42426B()
{
    auto* root=graphics_.CaptureAt4182F0();if(!Read(root->moduleAt00.baseAt00))return 0;
    auto* module=ResolveGraphicsModuleReference(root->moduleAt00);
    using Function=DWORD(__cdecl*)();
    return reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(module,"CheckDeviceLost",15,strings_,windows_))();
}
void MainFrameNativeModules::ResetGraphicsAt432B40(){screen_.ResetRendererIfLoaded();}
LRESULT ActivateMainFrameNativeAt420F90(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,
    IMainFrameNativeRootSource& roots,IMainFrameNativeWindowApi& windows,IMainFrameNativeModules& modules)
{
    modules.ActivateSoundAt42103E(w);
    if (auto* legacy=BoundRootLegacyDisplaySettings())
    {
        legacy->Activate(Read(frame.windowAt00.windowAt04),w!=0,windows,modules);
        return Previous(frame,message,w,l,windows);
    }
    if(Read(Mode(roots,Capture(roots))->modeAt04))
    {
        bool reset=false;
        if(w)
        {
            if(windows.IsIconic(Read(frame.windowAt00.windowAt04)))
            {
                auto* root=Capture(roots);auto* selected=Mode(roots,root);roots.SynchronizeModeAt42116C(root,*selected);
                WINDOWPLACEMENT placement;placement.length=0x2C;
                static_cast<void>(windows.GetWindowPlacement(Read(frame.windowAt00.windowAt04),&placement));
                placement.showCmd=SW_SHOWNORMAL;
                selected=Mode(roots,Capture(roots));std::memcpy(&placement.rcNormalPosition,&selected->savedMonitorRectAt1C,sizeof(RECT));
                static_cast<void>(windows.SetWindowPlacement(Read(frame.windowAt00.windowAt04),&placement));reset=true;
            }
        }
        else if(Read(roots.ControlAt1AC(Capture(roots)).minimizeAt1B2))
        {
            if(!windows.IsIconic(Read(frame.windowAt00.windowAt04)))
            {static_cast<void>(windows.CloseWindow(Read(frame.windowAt00.windowAt04)));reset=true;}
        }
        if(reset)modules.ResetGraphicsAt432B40();
    }
    return Previous(frame,message,w,l,windows);
}
LRESULT DisplayChangeMainFrameNativeAt424230(MainFrameNativeBody90h& frame,UINT message,WPARAM w,LPARAM l,
    IMainFrameNativeRootSource& roots,IMainFrameNativeWindowApi& windows,IMainFrameNativeModules& modules)
{
    if(windows.IsWindowVisible(Read(frame.windowAt00.windowAt04)))
    {
        if(modules.CheckGraphicsDeviceAt42426B())
        {
            if(!windows.IsIconic(Read(frame.windowAt00.windowAt04)))static_cast<void>(windows.CloseWindow(Read(frame.windowAt00.windowAt04)));
            static_cast<void>(windows.MessageBoxA(Read(frame.windowAt00.windowAt04),graphicsError,"Graphics Error",MB_ICONERROR));
            roots.ControlAt1AC(Capture(roots)).valueAt1BC=1;
            const auto window=roots.EnvironmentAt4180C0()->windowAt2C;
            static_cast<void>(windows.PostMessageA(reinterpret_cast<HWND>(window),WM_CLOSE,0,0));
        }
        // 原01/02/03消息表无新版正常全屏最小化分支；设备lost和Previous仍保留。
        else if(!BoundRootLegacyDisplaySettings()&&Read(Mode(roots,Capture(roots))->modeAt04)&&!windows.IsIconic(Read(frame.windowAt00.windowAt04)))
        {static_cast<void>(windows.CloseWindow(Read(frame.windowAt00.windowAt04)));modules.ResetGraphicsAt432B40();}
    }
    return Previous(frame,message,w,l,windows);
}
}
