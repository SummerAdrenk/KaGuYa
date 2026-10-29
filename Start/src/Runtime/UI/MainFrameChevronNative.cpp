#include <Runtime/MainFrameChevronNative.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
RECT ReadRectangle(const RECT& r)noexcept
{return {Read(r.left),Read(r.top),Read(r.right),Read(r.bottom)};}
int Decrement(int value)noexcept
{return std::bit_cast<int>(static_cast<DWORD>(value)-1u);}

class Api final:public IMainFrameChevronNativeApi
{
public:
    LRESULT SendMessageA(HWND w,UINT m,WPARAM p,LPARAM l)override{return ::SendMessageA(w,m,p,l);}
    HMENU CreatePopupMenu()override{return ::CreatePopupMenu();}
    BOOL GetClientRect(HWND w,RECT* r)override{return ::GetClientRect(w,r);}
    int GetMenuItemCount(HMENU m)override{return ::GetMenuItemCount(m);}
    BOOL AppendMenuA(HMENU m,UINT f,UINT_PTR id,LPCSTR t)override{return ::AppendMenuA(m,f,id,t);}
    BYTE IsOldWindowsAt4137B0()override{return IsMainFrameOldWindowsNativeAt4137B0();}
    BOOL GetMenuItemInfoA(HMENU m,UINT i,BOOL p,MENUITEMINFOA* v)override{return ::GetMenuItemInfoA(m,i,p,v);}
    int LoadStringA(HINSTANCE h,UINT i,LPSTR b,int n)override{return ::LoadStringA(h,i,b,n);}
    BOOL DestroyMenu(HMENU m)override{return ::DestroyMenu(m);}
    BOOL MessageBeep(UINT t)override{return ::MessageBeep(t);}
    int MapWindowPoints(HWND a,HWND b,POINT* p,UINT n)override{return ::MapWindowPoints(a,b,p,n);}
    BOOL IsWindow(HWND w)override{return ::IsWindow(w);}
    HMONITOR MonitorFromPoint(POINT p,DWORD f)override{return ::MonitorFromPoint(p,f);}
    BOOL GetMonitorInfoA(HMONITOR h,MONITORINFO* i)override{return ::GetMonitorInfoA(h,i);}
    BOOL TrackPopupMenuEx(HMENU m,UINT f,int x,int y,HWND w,TPMPARAMS* p)override{return ::TrackPopupMenuEx(m,f,x,y,w,p);}
    BOOL RemoveMenu(HMENU m,UINT i,UINT f)override{return ::RemoveMenu(m,i,f);}
    BOOL PeekMessageA(MSG* m,HWND w,UINT a,UINT b,UINT f)override{return ::PeekMessageA(m,w,a,b,f);}
    BOOL PtInRect(const RECT* r,POINT p)override{return ::PtInRect(r,p);}
};
}

IMainFrameChevronNativeApi& DefaultMainFrameChevronNativeApi()noexcept{static Api api;return api;}
MainFrameChevronNativeServices DefaultMainFrameChevronNativeServices()noexcept
{return {DefaultMainFrameChevronNativeApi(),ProcessAtlBaseModuleNative()};}

BYTE BuildMainFrameChevronMenuNativeAt42D230(MainFrameChevronMenuInfo0Ch* context,MainFrameChevronNativeServices services)
{
    auto& api=services.api;
    MainFrameChevronBandInfo50h band{};band.cbSize=0x50;band.fMask=0x10;
    auto* notification=Read(context->notificationAt04);
    static_cast<void>(api.SendMessageA(Read(notification->hdr.hwndFrom),0x41D,Read(notification->uBand),reinterpret_cast<LPARAM>(&band)));
    const auto child=band.childAt20;
    const auto count=api.SendMessageA(child,0x418,0,0);
    if(count<=0)return 0;
    const auto source=reinterpret_cast<HMENU>(api.SendMessageA(child,0x52E,0,0));
    context->commandBarAt08=source?BYTE{1}:BYTE{0};
    const auto menu=api.CreatePopupMenu();
    RECT client{};static_cast<void>(api.GetClientRect(child,&client));
    for(int index=0;index<count;++index)
    {
        TBBUTTON button{};
        static_cast<void>(api.SendMessageA(child,0x417,index,reinterpret_cast<LPARAM>(&button)));
        if(Read(button.fsState)&8u)continue;
        RECT item{};
        static_cast<void>(api.SendMessageA(child,0x41D,index,reinterpret_cast<LPARAM>(&item)));
        const BYTE enabled=(Read(button.fsState)>>2)&1u;
        if(item.right<=client.right && item.bottom<=client.bottom)continue;
        if(Read(button.fsStyle)&1u)
        {
            if(api.GetMenuItemCount(menu)>0)static_cast<void>(api.AppendMenuA(menu,0x800,0,nullptr));
        }
        else if(Read(context->commandBarAt08))
        {
            char text[0xC8]{};MENUITEMINFOA info{};info.cbSize=0x30;
            if(api.IsOldWindowsAt4137B0())info.cbSize=0x2C;
            info.fMask=0x14;info.cch=0xC8;info.dwTypeData=text;
            static_cast<void>(api.GetMenuItemInfoA(source,index,TRUE,&info));
            static_cast<void>(api.AppendMenuA(menu,0x10u|(enabled^1u),reinterpret_cast<UINT_PTR>(info.hSubMenu),info.dwTypeData));
        }
        else
        {
            char text[0xC8]{};TBBUTTONINFOA info{};
            info.cbSize=0x20;info.dwMask=2;info.pszText=text;info.cchText=0xC8;
            LPCSTR label=text;
            const auto got=api.SendMessageA(child,0x441,Read(button.idCommand),reinterpret_cast<LPARAM>(&info));
            if(got==-1 || !text[0])
            {
                label="";
                const auto length=api.LoadStringA(Read(services.nativeBase.resourceInstanceAt08),Read(button.idCommand),text,0xC8);
                for(int i=0;i<length;++i)if(text[i]=='\n'){label=text+i+1;break;}
            }
            // 42D51E在同步回调后重新读取idCommand，不能沿用441调用前的值。
            static_cast<void>(api.AppendMenuA(menu,enabled^1u,static_cast<UINT_PTR>(Read(button.idCommand)),label));
        }
    }
    // 42D543只测试零而非<=0；CreatePopupMenu返回NULL不增加早退。
    if(api.GetMenuItemCount(menu)==0)
    {
        if(menu)static_cast<void>(api.DestroyMenu(menu));
        static_cast<void>(api.MessageBeep(0xFFFFFFFFu));
        return 0;
    }
    context->menuAt00=menu;
    return 1;
}

int AdjustMainFrameChevronXNativeAt43A680(POINT point,IMainFrameChevronNativeApi& api)
{
    if(api.MonitorFromPoint(point,0))return point.x;
    const auto monitor=api.MonitorFromPoint(point,2);
    if(!monitor)return point.x;
    MONITORINFO info{};info.cbSize=0x28;
    if(!api.GetMonitorInfoA(monitor,&info))return point.x;
    if(point.x<info.rcWork.left)return info.rcWork.left;
    return point.x>info.rcWork.right?info.rcWork.right:point.x;
}

LRESULT ChevronMainFrameNativeAt423550(MainFrameNativeBody90h& body,WPARAM,NMREBARCHEVRON* notification,BOOL* handled,
    MainFrameChevronNativeServices services)
{
    MainFrameChevronMenuInfo0Ch context;
    context.menuAt00=nullptr;context.notificationAt04=notification;context.commandBarAt08=0;
    if(!BuildMainFrameChevronMenuNativeAt42D230(&context,services)){*handled=FALSE;return 1;}
    auto& api=services.api;
    const auto from=Read(notification->hdr.hwndFrom);
    POINT point{Read(notification->rc.left),Read(notification->rc.bottom)};
    static_cast<void>(api.MapWindowPoints(from,nullptr,&point,1));
    RECT rectangle=ReadRectangle(notification->rc);
    static_cast<void>(api.MapWindowPoints(from,nullptr,reinterpret_cast<POINT*>(&rectangle),2));
    const UINT flags=api.IsOldWindowsAt4137B0()?0x40u:0x1040u;
    TPMPARAMS parameters;parameters.cbSize=0x14;parameters.rcExclude=rectangle;
    const auto bar=reinterpret_cast<HWND>(api.SendMessageA(Read(body.windowAt00.windowAt04),0x52D,0,0));
    const auto valid=api.IsWindow(bar);
    HMENU menu;
    if(valid)
    {
        menu=Read(context.menuAt00);
        MainFrameChevronPopup18h popup{0x18,menu,flags,point.x,point.y,&parameters};
        static_cast<void>(api.SendMessageA(bar,0x52F,0,reinterpret_cast<LPARAM>(&popup)));
    }
    else
    {
        const auto y=point.y;const auto window=Read(body.windowAt00.windowAt04);
        const auto x=AdjustMainFrameChevronXNativeAt43A680(point,api);
        menu=Read(context.menuAt00);
        static_cast<void>(api.TrackPopupMenuEx(menu,flags,x,y,window,&parameters));
    }
    // 原无EH清理资格；仅正常控制流反向卸载借用子菜单，再释放新菜单。
    if(Read(context.commandBarAt08))
        for(int index=Decrement(api.GetMenuItemCount(menu));index>=0;index=Decrement(index))
            static_cast<void>(api.RemoveMenu(menu,index,0x400));
    if(menu)static_cast<void>(api.DestroyMenu(menu));
    const auto lateFrom=Read(notification->hdr.hwndFrom);
    rectangle=ReadRectangle(notification->rc);
    static_cast<void>(api.MapWindowPoints(lateFrom,nullptr,reinterpret_cast<POINT*>(&rectangle),2));
    MSG message{};
    if(api.PeekMessageA(&message,Read(body.windowAt00.windowAt04),0x201,0x201,0) && api.PtInRect(&rectangle,message.pt))
        static_cast<void>(api.PeekMessageA(&message,Read(body.windowAt00.windowAt04),0x201,0x201,1));
    return 0;
}

MainFrameChevronNativeRuntime::MainFrameChevronNativeRuntime(MainFrameChevronNativeServices services)noexcept:services_(services){}
LRESULT MainFrameChevronNativeRuntime::ChevronAt423550(MainFrameNativeBody90h& body,WPARAM w,LPARAM l,BOOL* handled)
{return ChevronMainFrameNativeAt423550(body,w,reinterpret_cast<NMREBARCHEVRON*>(l),handled,services_);}
}
