#include <Runtime/DialogEndpointNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
HWND Window(const HWND* slot)noexcept{return *static_cast<const volatile HWND*>(slot);}
LONG Add(LONG a,LONG b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)+static_cast<DWORD>(b));}
LONG Sub(LONG a,LONG b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)-static_cast<DWORD>(b));}
}
BOOL CenterDialogWindowNativeAt413990(const HWND* slot,HWND center,IDialogEndpointNativeApi& api)
{
    const auto style=api.GetWindowLongA(Window(slot),GWL_STYLE);
    if(!center)center=(style&WS_CHILD)?api.GetParent(Window(slot)):api.GetWindow(Window(slot),GW_OWNER);
    // 原不初始化这三个RECT及MONITORINFO未写区域；被忽略的Win32失败不补零。
    RECT rectangle,bounds,target;static_cast<void>(api.GetWindowRect(Window(slot),&rectangle));
    if(!(style&WS_CHILD))
    {
        if(center&&(api.GetWindowLongA(center,GWL_STYLE)&(WS_VISIBLE|WS_MINIMIZE))!=WS_VISIBLE)center=nullptr;
        const auto monitor=api.MonitorFromWindow(center?center:Window(slot),MONITOR_DEFAULTTONEAREST);
        if(!monitor)return FALSE;
        MONITORINFO info;info.cbSize=0x28;if(!api.GetMonitorInfoA(monitor,&info))return FALSE;
        bounds=info.rcWork;
        if(center)static_cast<void>(api.GetWindowRect(center,&target));else target=bounds;
    }
    else
    {
        const auto parent=api.GetParent(Window(slot));static_cast<void>(api.GetClientRect(parent,&bounds));
        static_cast<void>(api.GetClientRect(center,&target));
        static_cast<void>(api.MapWindowPoints(center,parent,reinterpret_cast<POINT*>(&target),2));
    }
    const auto width=Sub(rectangle.right,rectangle.left),height=Sub(rectangle.bottom,rectangle.top);
    LONG x=Sub(Add(target.right,target.left)/2,width/2),y=Sub(Add(target.bottom,target.top)/2,height/2);
    if(Add(x,width)>bounds.right)x=Sub(bounds.right,width);
    if(x<bounds.left)x=bounds.left;
    if(Add(y,height)>bounds.bottom)y=Sub(bounds.bottom,height);
    if(y<bounds.top)y=bounds.top;
    return api.SetWindowPos(Window(slot),nullptr,x,y,-1,-1,0x15);
}
}
