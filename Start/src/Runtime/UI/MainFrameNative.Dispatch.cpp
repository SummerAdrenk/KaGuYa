#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RootLegacyDisplaySettingsRuntime.h>
#include <CommCtrl.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void* Capture(IMainFrameNativeRootSource& source){source.EnsureRootAt413BE0();return source.RootAt6198C4();}
void* Resolve(const YtlReferenceSlotRuntime& slot)noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    auto* control=Read(slot.controlAt08);
    const auto stride=control?Read(*reinterpret_cast<const DWORD*>(reinterpret_cast<DWORD>(control)+0x0Cu)):0u;
    return reinterpret_cast<void*>(base+Read(slot.biasAt04)*stride);
}
BYTE Mode(IMainFrameNativeRootSource& source)
{return Read(*reinterpret_cast<const BYTE*>(reinterpret_cast<DWORD>(Resolve(source.ModeAt0C(Capture(source))))+4u));}
LRESULT Previous(MainFrameNativeBody90h& body,UINT message,WPARAM w,LPARAM l,IMainFrameNativeWindowApi& windows)
{return windows.CallWindowProcA(Read(body.windowAt00.previousAt20),Read(body.windowAt00.windowAt04),message,w,l);}
LONG Difference(LONG end,LONG begin)noexcept
{return std::bit_cast<LONG>(static_cast<DWORD>(end)-static_cast<DWORD>(begin));}
struct PaintUnwind final
{
    IMainFrameNativeWindowApi& windows;HWND window;PAINTSTRUCT& paint;bool active=true;
    ~PaintUnwind()noexcept{if(active)static_cast<void>(windows.EndPaint(window,&paint));}
};
class Windows final:public IMainFrameNativeWindowApi
{
public:
    HDC BeginPaint(HWND w,PAINTSTRUCT* p)override{return ::BeginPaint(w,p);}
    BOOL EndPaint(HWND w,const PAINTSTRUCT* p)override{return ::EndPaint(w,p);}
    BOOL ValidateRect(HWND w,const RECT* p)override{return ::ValidateRect(w,p);}
    BOOL AdjustWindowRectEx(RECT* r,DWORD style,BOOL menu,DWORD ex)override{return ::AdjustWindowRectEx(r,style,menu,ex);}
    BOOL IsWindowVisible(HWND w)override{return ::IsWindowVisible(w);}
    BOOL IsIconic(HWND w)override{return ::IsIconic(w);}
    BOOL OpenIcon(HWND w)override{return ::OpenIcon(w);}
    BOOL CloseWindow(HWND w)override{return ::CloseWindow(w);}
    BOOL GetWindowPlacement(HWND w,WINDOWPLACEMENT* p)override{return ::GetWindowPlacement(w,p);}
    BOOL SetWindowPlacement(HWND w,const WINDOWPLACEMENT* p)override{return ::SetWindowPlacement(w,p);}
    int MessageBoxA(HWND w,const char* text,const char* caption,UINT type)override{return ::MessageBoxA(w,text,caption,type);}
    BOOL PostMessageA(HWND w,UINT m,WPARAM p,LPARAM l)override{return ::PostMessageA(w,m,p,l);}
    LRESULT CallWindowProcA(WNDPROC p,HWND w,UINT m,WPARAM a,LPARAM b)override{return ::CallWindowProcA(p,w,m,a,b);}
};
}
IMainFrameNativeWindowApi& DefaultMainFrameNativeWindowApi()noexcept{static Windows windows;return windows;}
BOOL ProcessMainFrameNativeAt42E810(MainFrameNativeBody90h& body,HWND window,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,MainFrameNativeDispatchServices& services)
{
    if(map)return FALSE;
    auto& roots=services.roots;auto& windows=services.windows;auto& terminals=services.terminals;
    BOOL handled; // 原item槽没有入口初值；只有实际分支才赋原值。
    switch(message)
    {
    case WM_CREATE:
        RegisterIdleLoopFrameNativeAt423990(&body,services.module,services.publication,services.loops);*result=0;return TRUE;
    case WM_CLOSE:*result=terminals.CloseAt423750(body,message,w,l,&handled);return TRUE;
    case WM_DESTROY:
    {
        // 4826A0首先释放WindowMode；此处必须在它之前终止设置receiver并恢复桌面。
        if (auto* legacy=BoundRootLegacyDisplaySettings()) legacy->BeforeWindowRelease();
        roots.ReleaseWindowObjectsAt4826A0(Capture(roots));
        const auto thread=services.loops.module.platform.GetCurrentThreadId();
        auto* loop=static_cast<IdleLoopNativeBody38h*>(FindAppModuleMessageLoopNativeAt417F50(
            services.module,services.publication,thread,services.loops));
        RemoveIdleLoopFrameNativeAt42E8D5(&body,*loop,services.loops.diagnostics);*result=1;break;
    }
    case WM_PAINT:
    {
        const auto original=Read(body.windowAt00.windowAt04);PAINTSTRUCT paint;
        static_cast<void>(windows.BeginPaint(original,&paint));PaintUnwind unwind{windows,original,paint};
        if(!Read(body.paintByteAt8C))
        {
            roots.PaintRootAt483B70(Capture(roots),paint);
            static_cast<void>(windows.ValidateRect(Read(body.windowAt00.windowAt04),nullptr));
        }
        static_cast<void>(windows.EndPaint(original,&paint));unwind.active=false;*result=0;return TRUE;
    }
    case WM_GETMINMAXINFO:
        if(!Mode(roots))
        {
            const auto height=Read(roots.EnvironmentAt4180C0()->heightAt1C);
            const auto width=Read(roots.EnvironmentAt4180C0()->widthAt18);
            RECT rectangle{0,0,std::bit_cast<LONG>(width),std::bit_cast<LONG>(height)};
            const auto ex=Read(roots.EnvironmentAt4180C0()->exStyleAt24);
            const auto style=Read(roots.EnvironmentAt4180C0()->styleAt20);
            static_cast<void>(windows.AdjustWindowRectEx(&rectangle,style,FALSE,ex));
            auto* limits=reinterpret_cast<MINMAXINFO*>(l);
            const auto x=Difference(rectangle.right,rectangle.left),y=Difference(rectangle.bottom,rectangle.top);
            if(Read(limits->ptMaxTrackSize.x)<x||Read(limits->ptMaxTrackSize.y)<y)limits->ptMaxTrackSize={x,y};
        }
        *result=0;return TRUE;
    case WM_DISPLAYCHANGE:*result=terminals.DisplayChangeAt424230(body,message,w,l,&handled);return TRUE;
    case WM_SIZE:
        if(windows.IsWindowVisible(Read(body.windowAt00.windowAt04))&&Mode(roots)&&w!=SIZE_MINIMIZED)
        {
            if(roots.MovieBaseAt48(Capture(roots)))roots.ResizeMovieAt48(Capture(roots));
            roots.UpdateEditViewportAt4875F0(Capture(roots));
        }
        *result=Previous(body,message,w,l,windows);return TRUE;
    case WM_ACTIVATEAPP:*result=terminals.ActivateAt420F90(body,message,w,l,&handled);return TRUE;
    case WM_SYSCOMMAND:
        if(w==SC_MAXIMIZE)
        {
            if(windows.IsIconic(Read(body.windowAt00.windowAt04)))static_cast<void>(windows.OpenIcon(Read(body.windowAt00.windowAt04)));
            static_cast<void>(windows.PostMessageA(Read(body.windowAt00.windowAt04),WM_COMMAND,0x800B,0));*result=0;
        }
        else *result=Previous(body,message,w,l,windows);
        return TRUE;
    case WM_DEVICECHANGE:*result=terminals.DeviceChangeAt424070(body,message,w,l,&handled);return TRUE;
    case WM_NCLBUTTONDBLCLK:
    {
        auto* save=ResolveEnvironmentArchiveAt417990(*roots.EnvironmentAt4180C0(),1);
        if(Read(*reinterpret_cast<const BYTE*>(reinterpret_cast<DWORD>(save)+5u)))
        {static_cast<void>(windows.PostMessageA(Read(body.windowAt00.windowAt04),WM_SYSCOMMAND,SC_MAXIMIZE,0));*result=0;}
        else *result=Previous(body,message,w,l,windows);
        return TRUE;
    }
    case WM_COMMAND:
        if (auto* legacy=BoundRootLegacyDisplaySettings(); legacy && legacy->Dispatch(message,w,l,*result)) return TRUE;
        // 42EE45是shr eax,10h，保留无符号高WORD，不能采信伪代码的SHIWORD。
        if(LOWORD(w)==0x801C){*result=terminals.CommandAt428C10(body,HIWORD(w),0x801C,l,&handled);return TRUE;}
        break;
    default:break;
    }
    using Member=BOOL(__thiscall*)(void*,HWND,UINT,WPARAM,LPARAM,LRESULT*,DWORD);
    auto* table=static_cast<const Member*>(Read(body.menuVptrAt54));
    if(Read(table[0])(&body.menuVptrAt54,window,message,w,l,result,0))return TRUE;
    if(services.hooks.DispatchNative(window,message,w,l,result,0))return TRUE;
    if(message==WM_INITMENUPOPUP)
    {
        handled=TRUE;
        const auto value=terminals.UpdateMenuAt428430(body.updateAt3C,message,reinterpret_cast<HMENU>(w),l,&handled);
        const bool consumed=handled!=FALSE;*result=value;if(consumed)return TRUE;
    }
    else if(message==WM_COMMAND)
    {
        const auto enabled=Read(body.updateAt3C.byteAt16);
        handled=FALSE;
        if(enabled&&HIWORD(w)==1)
        {
            auto* data=Read(body.updateAt3C.dataAt10);auto* at=Read(body.updateAt3C.mapAt0C);WORD flags=0;
            if(data)
            {
                auto command=Read(at->commandAt00);
                while(command!=0xFFFFu)
                {
                    if(LOWORD(w)==command){flags=Read(data->flagsAt00);break;}
                    ++at;++data;command=Read(at->commandAt00);
                }
            }
            handled=(flags&0x100u)!=0;
        }
        const bool consumed=handled!=FALSE;*result=0;if(consumed)return TRUE;
    }
    if(message==WM_SIZE){if(w!=SIZE_MINIMIZED)terminals.UpdateUiAt439100(body,TRUE);*result=1;}
    else if(message==WM_NOTIFY)
    {
        const auto code=Read(reinterpret_cast<NMHDR*>(l)->code);
        if(code==static_cast<UINT>(-834)){terminals.UpdateUiAt439100(body,FALSE);*result=0;return TRUE;}
        if(code==static_cast<UINT>(-841))
        {
            handled=TRUE;const auto value=terminals.NotifyAt423550(body,w,l,&handled);
            const bool consumed=handled!=FALSE;*result=value;if(consumed)return TRUE;
        }
    }
    return terminals.FrameBaseAt42DC50(body,window,message,w,l,result,0)?TRUE:FALSE;
}
}
