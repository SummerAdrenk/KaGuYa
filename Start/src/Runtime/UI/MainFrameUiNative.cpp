#include <Runtime/MainFrameUiNative.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <atlbase.h>
#include <bit>
#include <cstring>
#include <cwchar>

// 原540148以8调用CRT安全失败叶；使用工具链真实range-check入口，不伪造C++错误。
extern "C" __declspec(noreturn) void __cdecl __report_rangecheckfailure();

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
HWND Bar(const MainFrameNativeBody90h& body,unsigned index)noexcept
{return reinterpret_cast<HWND>(Read(body.opaqueAt24[index]));}
LONG Add(LONG a,LONG b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)+static_cast<DWORD>(b));}
LONG Subtract(LONG a,LONG b)noexcept{return std::bit_cast<LONG>(static_cast<DWORD>(a)-static_cast<DWORD>(b));}

class Api final:public IMainFrameUiNativeApi
{
public:
    BOOL SetMenuDefaultItem(HMENU m,UINT i,UINT p)override{return ::SetMenuDefaultItem(m,i,p);}
    BOOL GetMenuItemInfoA(HMENU m,UINT i,BOOL p,MENUITEMINFOA* v)override{return ::GetMenuItemInfoA(m,i,p,v);}
    BOOL SetMenuItemInfoA(HMENU m,UINT i,BOOL p,const MENUITEMINFOA* v)override{return ::SetMenuItemInfoA(m,i,p,v);}
    BOOL CheckMenuRadioItem(HMENU m,UINT a,UINT b,UINT c,UINT f)override{return ::CheckMenuRadioItem(m,a,b,c,f);}
    ULONGLONG VerSetConditionMask(ULONGLONG m,DWORD t,BYTE c)override{return ::VerSetConditionMask(m,t,c);}
    BOOL VerifyVersionInfoW(OSVERSIONINFOEXW* v,DWORD t,DWORDLONG m)override{return ::VerifyVersionInfoW(v,t,m);}
    int LoadStringA(HINSTANCE h,UINT i,LPSTR b,int n)override{return ::LoadStringA(h,i,b,n);}
    int LoadStringW(HINSTANCE h,UINT i,LPWSTR b,int n)override{return ::LoadStringW(h,i,b,n);}
    LRESULT SendMessageA(HWND w,UINT m,WPARAM p,LPARAM l)override{return ::SendMessageA(w,m,p,l);}
    HWND SetFocus(HWND w)override{return ::SetFocus(w);}
    LONG GetWindowLongA(HWND w,int i)override{return ::GetWindowLongA(w,i);}
    void PostQuitMessage(int n)override{::PostQuitMessage(n);}
    BOOL GetClientRect(HWND w,RECT* r)override{return ::GetClientRect(w,r);}
    BOOL InvalidateRect(HWND w,const RECT* r,BOOL b)override{return ::InvalidateRect(w,r,b);}
    BOOL GetWindowRect(HWND w,RECT* r)override{return ::GetWindowRect(w,r);}
    BOOL SetWindowPos(HWND w,HWND a,int x,int y,int cx,int cy,UINT f)override{return ::SetWindowPos(w,a,x,y,cx,cy,f);}
    int StrncpyAt54B740(char* d,std::size_t n,const char* s,std::size_t c)override{return ::strncpy_s(d,n,s,c);}
    int WcsncpyAt54B540(wchar_t* d,std::size_t n,const wchar_t* s,std::size_t c)override{return ::wcsncpy_s(d,n,s,c);}
    [[noreturn]] void RangeCheckFailureAt540148()override{::__report_rangecheckfailure();}
};

WORD StatusResource(WORD id)noexcept
{
    if(static_cast<WORD>(id+0x1000u)<=0x1EFu)return static_cast<WORD>(((id-0xF000u)>>4)-0x1100u);
    if(static_cast<WORD>(id+0x1EF0u)<=0xFu)return 0xEFDA;
    if(static_cast<WORD>(id+0x100u)<=0xFDu)return 0xEF1F;
    return id;
}

template<class Char,class Notification>
void Tooltip(Notification& note,WPARAM id,MainFrameUiNativeServices services)
{
    // 入口只测试uFlags低字节；复制/回调之后才对完整DWORD置缓存位。
    if(!id || (Read(*reinterpret_cast<const BYTE*>(&note.uFlags))&1u))return;
    Char buffer[0x100]{};
    const auto instance=Read(services.nativeBase.resourceInstanceAt08);
    int length;
    if constexpr(std::is_same_v<Char,char>)length=services.api.LoadStringA(instance,id,buffer,0x100);
    else length=services.api.LoadStringW(instance,id,buffer,0x100);
    if(length<=0)return;
    for(int i=0;i<length;++i)
    {
        if(buffer[i]!=static_cast<Char>('\n'))continue;
        int error;
        if constexpr(std::is_same_v<Char,char>)error=services.api.StrncpyAt54B740(note.szText,0x50,buffer+i+1,_TRUNCATE);
        else error=services.api.WcsncpyAt54B540(note.szText,0x50,buffer+i+1,_TRUNCATE);
        static_cast<void>(CheckMainFrameUiErrnoNativeAt4136D0(error));
        break;
    }
    // 原42DED8/42DF89：正长度即缓存，无LF也不改szText而照样置8000。
    note.uFlags=Read(note.uFlags)|0x8000u;
}
}

IMainFrameUiNativeApi& DefaultMainFrameUiNativeApi()noexcept{static Api api;return api;}
MainFrameUiNativeServices DefaultMainFrameUiNativeServices()noexcept
{return {DefaultMainFrameUiNativeApi(),ProcessAtlBaseModuleNative()};}

BYTE IsMainFrameOldWindowsNativeAt4137B0(IMainFrameUiNativeApi& api)
{
    OSVERSIONINFOEXW version{};version.dwOSVersionInfoSize=0x11C;
    ULONGLONG mask=api.VerSetConditionMask(0,2,3);
    mask=api.VerSetConditionMask(mask,1,3);
    mask=api.VerSetConditionMask(mask,0x20,3);
    version.dwMajorVersion=4;version.dwMinorVersion=0x5A;version.wServicePackMajor=0;
    return api.VerifyVersionInfoW(&version,0x23,mask)==FALSE?BYTE{1}:BYTE{0};
}

int CheckMainFrameUiErrnoNativeAt4136D0(int error)
{
    if(error==0 || error==0x50)return error;
    throw ATL::CAtlException(error==12?E_OUTOFMEMORY:(error==22 || error==34)?E_INVALIDARG:E_FAIL);
}

LRESULT UpdateMainFrameMenuNativeAt428430(UpdateUiNativeBody18h& body,UINT,HMENU menu,LPARAM,BOOL* handled,
    MainFrameUiNativeServices services)
{
    *handled=FALSE;
    if(!menu)return 1;
    auto* data=Read(body.dataAt10);if(!data)return 1;
    auto* map=Read(body.mapAt0C);
    auto command=Read(map->commandAt00);
    while(command!=0xFFFFu)
    {
        if(Read(*reinterpret_cast<const BYTE*>(&map->flagsAt02))&1u)
        {
            const UINT item=command;
            if(Read(data->flagsAt00)&0x4000u)
            {
                static_cast<void>(services.api.SetMenuDefaultItem(menu,0xFFFFFFFFu,FALSE));
                data->flagsAt00=static_cast<WORD>(Read(data->flagsAt00)&0xBFFFu);
            }
            MENUITEMINFOA set{};set.cbSize=0x30;
            if(IsMainFrameOldWindowsNativeAt4137B0(services.api))set.cbSize=0x2C;
            set.fMask=1;set.wID=item;const auto flags=Read(data->flagsAt00);
            if(flags&0x100u)set.fState|=3;
            if(flags&0x200u)set.fState|=8;
            if(flags&0x1000u)set.fState|=0x1000;
            if(flags&0x2000u)
            {
                MENUITEMINFOA get{};get.cbSize=0x30;
                if(IsMainFrameOldWindowsNativeAt4137B0(services.api))get.cbSize=0x2C;
                get.fMask=0x10;get.wID=item;
                if(services.api.GetMenuItemInfoA(menu,item,FALSE,&get))
                {
                    set.fMask|=0x10;set.fType|=get.fType&0xFFFFF7FBu;
                    set.dwTypeData=static_cast<LPSTR>(Read(data->textAt04));
                }
            }
            static_cast<void>(services.api.SetMenuItemInfoA(menu,item,FALSE,&set));
            if(Read(data->flagsAt00)&0x800u)
            {
                const auto checked=Read(map->commandAt00);
                // 同一DWORD为text/radio union；此处无回调间隔，不以WORD别名读指针对象。
                const auto range=std::bit_cast<DWORD>(Read(data->textAt04));
                static_cast<void>(services.api.CheckMenuRadioItem(menu,LOWORD(range),HIWORD(range),checked,0));
            }
        }
        command=Read((map+1)->commandAt00);++map;++data;
    }
    return 0;
}

BOOL ProcessFrameBaseNativeAt42DC50(MainFrameNativeBody90h& body,HWND,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,MainFrameUiNativeServices services)
{
    if(map)return FALSE;
    auto& api=services.api;
    switch(message)
    {
    case WM_ERASEBKGND:
    {
        const BOOL present=Bar(body,2)!=nullptr;*result=present;return present;
    }
    case WM_MENUSELECT:
    {
        const auto status=Bar(body,1);
        if(status)
        {
            if(HIWORD(w)==0xFFFFu && !l)static_cast<void>(api.SendMessageA(status,0x409,0,0));
            else
            {
                char buffer[0x100]{};
                if(!(HIWORD(w)&0x10u))
                {
                    const auto id=StatusResource(LOWORD(w));
                    const auto length=api.LoadStringA(Read(services.nativeBase.resourceInstanceAt08),id,buffer,0x100);
                    for(int i=0;i<length;++i)
                    {
                        if(buffer[i]!='\n')continue;
                        if(static_cast<unsigned>(i)>=sizeof(buffer))api.RangeCheckFailureAt540148();
                        buffer[i]=0;break;
                    }
                }
                static_cast<void>(api.SendMessageA(Bar(body,1),0x409,1,0));
                static_cast<void>(api.SendMessageA(Bar(body,1),0x401,0x1FF,reinterpret_cast<LPARAM>(buffer)));
            }
        }
        *result=1;return FALSE;
    }
    case WM_SETFOCUS:
        if(const auto client=Bar(body,2))static_cast<void>(api.SetFocus(client));
        *result=1;return FALSE;
    case WM_DESTROY:
        if(!(static_cast<DWORD>(api.GetWindowLongA(Read(body.windowAt00.windowAt04),GWL_STYLE))&0xC0000000u))
            api.PostQuitMessage(1);
        *result=1;return FALSE;
    case WM_NOTIFY:
    {
        const auto code=Read(reinterpret_cast<const NMHDR*>(l)->code);
        if(code==static_cast<UINT>(-520))Tooltip<char>(*reinterpret_cast<NMTTDISPINFOA*>(l),w,services);
        else if(code==static_cast<UINT>(-530))Tooltip<wchar_t>(*reinterpret_cast<NMTTDISPINFOW*>(l),w,services);
        else return FALSE;
        *result=0;return TRUE;
    }
    default:return FALSE;
    }
}

void ResizeMainFrameUiNativeAt439100(MainFrameNativeBody90h& body,BOOL resizeBars,MainFrameUiNativeServices services)
{
    auto& api=services.api;RECT rectangle{};
    static_cast<void>(api.GetClientRect(Read(body.windowAt00.windowAt04),&rectangle));
    if(const auto top=Bar(body,0);top && (static_cast<DWORD>(api.GetWindowLongA(top,GWL_STYLE))&0x10000000u))
    {
        if(resizeBars)
        {
            static_cast<void>(api.SendMessageA(Bar(body,0),WM_SIZE,0,0));
            static_cast<void>(api.InvalidateRect(Bar(body,0),nullptr,TRUE));
        }
        RECT bar{};static_cast<void>(api.GetWindowRect(Bar(body,0),&bar));
        rectangle.top=Add(rectangle.top,Subtract(bar.bottom,bar.top));
    }
    if(const auto bottom=Bar(body,1);bottom && (static_cast<DWORD>(api.GetWindowLongA(bottom,GWL_STYLE))&0x10000000u))
    {
        if(resizeBars)static_cast<void>(api.SendMessageA(Bar(body,1),WM_SIZE,0,0));
        RECT bar{};static_cast<void>(api.GetWindowRect(Bar(body,1),&bar));
        rectangle.bottom=Add(rectangle.bottom,Subtract(bar.top,bar.bottom));
    }
    if(const auto client=Bar(body,2))
        static_cast<void>(api.SetWindowPos(client,nullptr,rectangle.left,rectangle.top,
            Subtract(rectangle.right,rectangle.left),Subtract(rectangle.bottom,rectangle.top),0x14));
}
}
