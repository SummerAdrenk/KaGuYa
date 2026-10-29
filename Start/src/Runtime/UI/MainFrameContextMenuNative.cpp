#include <Runtime/MainFrameContextMenuNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const void* p,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(p)+offset);}
void* Resolve(const void* ref)noexcept
{
    const auto base=Read<DWORD>(ref);if(!base)return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    return reinterpret_cast<void*>(base+Read<DWORD>(ref,4)*stride);
}
void* InlineUnsave(IMainFrameContextMenuNativeSource& source)
{
    auto* e=source.EnvironmentAt4180C0();
    // 原内联先读end再读begin；与417990的bias先读协议不可互换。
    const auto end=Read<DWORD>(e,0x34),begin=Read<DWORD>(e,0x30);
    const auto count=static_cast<DWORD>(std::bit_cast<std::int32_t>(end-begin)/12);
    return count>2?Resolve(reinterpret_cast<const void*>(begin+0x18u)):nullptr;
}
void* ExplicitUnsave(IMainFrameContextMenuNativeSource& source)
{return ResolveEnvironmentArchiveAt417990(*source.EnvironmentAt4180C0(),2);}
void* Root(IMainFrameContextMenuNativeSource& source)
{source.EnsureRootAt413BE0();return source.RootAt6198C4();}
BYTE Mode(IMainFrameContextMenuNativeSource& source)
{return Read<BYTE>(Resolve(&source.ModeAt0C(Root(source))),4);}
bool ThirdLayerNegative(IMainFrameContextMenuNativeSource& source)
{
    auto* draw=static_cast<DrawManagerNativeBody2Ch*>(Resolve(&source.DrawAt18(Root(source))));
    return Read<std::int32_t>(DrawLayerAt418C10(*draw,3),0x50)<0;
}
class Win32Api final:public IMainFrameContextMenuNativeApi
{
public:
    HMENU LoadMenuA(HINSTANCE i,LPCSTR n)override{return ::LoadMenuA(i,n);}
    HMENU GetSubMenu(HMENU m,int n)override{return ::GetSubMenu(m,n);}
    BOOL IsMenu(HMENU m)override{return ::IsMenu(m);}
    BOOL SetMenuItemInfoA(HMENU m,UINT i,BOOL p,const MENUITEMINFOA* v)override{return ::SetMenuItemInfoA(m,i,p,v);}
    UINT EnableMenuItem(HMENU m,UINT i,UINT f)override{return ::EnableMenuItem(m,i,f);}
    BOOL ClientToScreen(HWND w,POINT* p)override{return ::ClientToScreen(w,p);}
    HMONITOR MonitorFromPoint(POINT p,DWORD f)override{return ::MonitorFromPoint(p,f);}
    BOOL GetMonitorInfoA(HMONITOR m,MONITORINFO* i)override{return ::GetMonitorInfoA(m,i);}
    BOOL TrackPopupMenu(HMENU m,UINT f,int x,int y,int n,HWND w,const RECT* r)override{return ::TrackPopupMenu(m,f,x,y,n,w,r);}
    BOOL DestroyMenu(HMENU m)override{return ::DestroyMenu(m);}
};
}
IMainFrameContextMenuNativeApi& DefaultMainFrameContextMenuNativeApi()noexcept
{static Win32Api api;return api;}
void PrepareMainFrameContextMenuNativeAt413F30(HMENU* menu,IMainFrameContextMenuNativeSource& source,
    IMainFrameContextMenuNativeApi& api)
{
    MENUITEMINFOA mii; mii.cbSize=0x30;mii.fMask=MIIM_STATE;
    const auto set=[&](UINT id){static_cast<void>(api.SetMenuItemInfoA(*menu,id,FALSE,&mii));};
    if(Mode(source)){mii.fState=0x0B;set(0x800B);}
    if(!Mode(source)){mii.fState=0x0B;set(0x800C);}
    mii.fState=0;
    if(!(Read<BYTE>(InlineUnsave(source),0x18)&1))mii.fState|=3;
    if(Read<BYTE>(InlineUnsave(source),0x18)&2)mii.fState|=8;
    if(mii.fState)set(0x800E);
    mii.fState=0;
    if(!Read<BYTE>(InlineUnsave(source),0x0C)||!(Read<BYTE>(ExplicitUnsave(source),0x14)&1))mii.fState|=3;
    if(Read<BYTE>(InlineUnsave(source),0x14)&2)mii.fState|=8;
    if(mii.fState)set(0x800F);
    if(!Read<BYTE>(InlineUnsave(source),0x0C)||!(Read<BYTE>(ExplicitUnsave(source),0x1C)&1)||!ThirdLayerNegative(source))
    {mii.fState=3;set(0x8010);}
    if(!Read<BYTE>(InlineUnsave(source),0x0C)||(Read<BYTE>(ExplicitUnsave(source),0x18)&2)||!Read<DWORD>(ExplicitUnsave(source),0x3C))
    {mii.fState=3;set(0x8011);}
    // 41437A没有重新赋fState；8012沿用此前值，包括0/8/0B，不纠正成固定3。
    if(!Read<BYTE>(InlineUnsave(source),0x0C)||!(Read<BYTE>(ExplicitUnsave(source),0x20)&1)||!ThirdLayerNegative(source))set(0x8012);
}
int ClampMainFramePopupXNativeAt43A680(POINT point,IMainFrameContextMenuNativeApi& api)
{
    if(api.MonitorFromPoint(point,MONITOR_DEFAULTTONULL))return point.x;
    const auto monitor=api.MonitorFromPoint(point,MONITOR_DEFAULTTONEAREST);if(!monitor)return point.x;
    MONITORINFO info{};info.cbSize=0x28;
    if(!api.GetMonitorInfoA(monitor,&info))return point.x;
    if(point.x<info.rcWork.left)return info.rcWork.left;
    return point.x>info.rcWork.right?info.rcWork.right:point.x;
}
LRESULT ShowMainFrameContextMenuNativeAt428C10(MainFrameNativeBody90h& frame,DWORD notification,DWORD,LPARAM packed,
    BOOL*,IMainFrameContextMenuNativeSource& source,IMainFrameContextMenuNativeApi& api)
{
    const auto instance=source.ResourceInstanceAt61A6A0();
    // 原state0先于Win32 LoadMenu；该API不产生C++异常，返回失败仍发布NULL再继续。
    struct MenuLocal{HMENU value;MenuLocal()noexcept{}} parent;
    try
    {
        parent.value=api.LoadMenuA(instance,MAKEINTRESOURCEA(0x80));
        const auto position=std::bit_cast<std::int32_t>(Read<DWORD>(InlineUnsave(source),4)-1u);
        const auto child=api.GetSubMenu(parent.value,position);HMENU borrowed=child;
        if(api.IsMenu(child))
        {
            const UINT flags=static_cast<WORD>(notification)==2?0x16u:2u;
            PrepareMainFrameContextMenuNativeAt413F30(&borrowed,source,api);
            if(Read<DWORD>(ExplicitUnsave(source),4)==1&&!ThirdLayerNegative(source))
                static_cast<void>(api.EnableMenuItem(child,0,0x401));
            POINT point{static_cast<LONG>(LOWORD(packed)),static_cast<LONG>(HIWORD(packed))};
            static_cast<void>(api.ClientToScreen(frame.windowAt00.windowAt04,&point));
            frame.paintByteAt8C=1;source.WritePopupAt1B1(Root(source),1);
            const auto window=frame.windowAt00.windowAt04;const auto y=point.y;
            const auto x=ClampMainFramePopupXNativeAt43A680(point,api);
            static_cast<void>(api.TrackPopupMenu(child,flags,x,y,0,window,nullptr));
            frame.paintByteAt8C=0;
            // 428DB3的原PE E8目标418210，旧IDA误标threadpool；随后是两层Ref和原raw vslot08。
            auto* input=source.InputAt418210();auto* keyboard=Resolve(&input->keyboardAt18);
            auto* raw=Resolve(reinterpret_cast<const void*>(reinterpret_cast<DWORD>(keyboard)+4u));
            using Reset=void(__thiscall*)(void*);
            Read<Reset>(Read<const void*>(raw),8)(raw);
        }
        if(parent.value)static_cast<void>(api.DestroyMenu(parent.value));
    }
    catch(...)
    {
        // 原state1子菜单析构为空；state0释放父菜单。正常Destroy抛出也仍有此展开资格。
        try{if(parent.value)static_cast<void>(api.DestroyMenu(parent.value));}catch(...){std::terminate();}
        throw;
    }
    return 0;
}
}
