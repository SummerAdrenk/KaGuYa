#include <Runtime/EditWindowNativeMaps.h>
#include <bit>
#include <cstring>
#include <mbctype.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32Map final:public IEditWindowNativeMapApi
{
public:
    LRESULT CallWindowProcA(WNDPROC p,HWND window,UINT m,WPARAM w,LPARAM l) override{return ::CallWindowProcA(p,window,m,w,l);}
    LONG GetWindowLongA(HWND window,int i) override{return ::GetWindowLongA(window,i);}
    LONG SetWindowLongA(HWND window,int i,LONG v) override{return ::SetWindowLongA(window,i,v);}
    int GetWindowTextLengthA(HWND window) override{return ::GetWindowTextLengthA(window);}
    int GetWindowTextA(HWND window,LPSTR text,int n) override{return ::GetWindowTextA(window,text,n);}
    BOOL GetWindowRect(HWND window,RECT* r) override{return ::GetWindowRect(window,r);}
    BOOL GetClientRect(HWND window,RECT* r) override{return ::GetClientRect(window,r);}
    BOOL EqualRect(const RECT* a,const RECT* b) override{return ::EqualRect(a,b);}
    BOOL MoveWindow(HWND window,int x,int y,int w,int h,BOOL b) override{return ::MoveWindow(window,x,y,w,h,b);}
    LRESULT SendMessageA(HWND window,UINT m,WPARAM w,LPARAM l) override{return ::SendMessageA(window,m,w,l);}
    int GetObjectA(HGDIOBJ h,int n,void* p) override{return ::GetObjectA(h,n,p);}
    BOOL DeleteObject(HGDIOBJ h) override{return ::DeleteObject(h);}
    HFONT CreateFontIndirectA(const LOGFONTA* f) override{return ::CreateFontIndirectA(f);}
    HGDIOBJ SelectObject(HDC dc,HGDIOBJ h) override{return ::SelectObject(dc,h);}
    BOOL PatBlt(HDC dc,int x,int y,int w,int h,DWORD rop) override{return ::PatBlt(dc,x,y,w,h,rop);}
    COLORREF SetTextColor(HDC dc,COLORREF c) override{return ::SetTextColor(dc,c);}
    int SetBkMode(HDC dc,int m) override{return ::SetBkMode(dc,m);}
    HMENU GetMenu(HWND window) override{return ::GetMenu(window);}
    BOOL SetWindowTextA(HWND window,const char* text) override{return ::SetWindowTextA(window,text);}
    BOOL DestroyWindow(HWND window) override{return ::DestroyWindow(window);}
    int IsMbLead(std::uint32_t value) override{return _ismbblead(value);}
};
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
}
IEditWindowNativeMapApi& DefaultEditWindowNativeMapApi() noexcept{static Win32Map api;return api;}
void FillEditControlNativeFontAt48CF30(LOGFONTA& font,std::int32_t height)
{
    font.lfHeight=Sub(0,height);font.lfWidth=0;font.lfEscapement=0;font.lfOrientation=0;font.lfWeight=700;
    font.lfItalic=0;font.lfUnderline=0;font.lfStrikeOut=0;font.lfCharSet=0x80;
    font.lfOutPrecision=0;font.lfClipPrecision=0;font.lfQuality=2;font.lfPitchAndFamily=0x30;
    constexpr char face[]="\x82\x6C\x82\x72\x20\x83\x53\x83\x56\x83\x62\x83\x4E";
    std::memcpy(font.lfFaceName,face,sizeof(face)); // 原5ADC0C/568F50，只复制至NUL。
}
BOOL ProcessEditControlNativeAt485480(EditControlNativeBody28h& body,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,IEditWindowNativeMapApi& api)
{
    if(map)return 0;
    auto& endpoint=body.endpointAt00;
    if(message==WM_KEYDOWN)
    {
        if(w==VK_TAB)body.eventAt24=3;
        else if(w==VK_RETURN)body.eventAt24=1;
        else if(w==VK_ESCAPE)body.eventAt24=2;
        const auto window=Read(endpoint.windowAt04);const auto procedure=Read(endpoint.previousAt20);
        *result=api.CallWindowProcA(procedure,window,WM_KEYDOWN,w,l);return 1;
    }
    if(message!=WM_SIZE)return 0;
    RECT rectangle{};
    if(api.GetWindowRect(Read(endpoint.windowAt04),&rectangle))
    {
        const auto height=Sub(rectangle.bottom,rectangle.top);
        if(height)
        {
            const auto old=reinterpret_cast<HFONT>(api.SendMessageA(Read(endpoint.windowAt04),WM_GETFONT,0,0));
            bool create=!old;
            if(old)
            {
                LOGFONTA description;
                const auto copied=api.GetObjectA(old,0x3C,&description);
                if(copied==0x3C&&description.lfHeight!=Sub(rectangle.bottom,rectangle.top))
                {static_cast<void>(api.DeleteObject(old));create=true;}
            }
            if(create)
            {
                LOGFONTA description;FillEditControlNativeFontAt48CF30(description,Sub(rectangle.bottom,rectangle.top));
                const auto font=api.CreateFontIndirectA(&description);
                static_cast<void>(api.SendMessageA(Read(endpoint.windowAt04),WM_SETFONT,reinterpret_cast<WPARAM>(font),1));
            }
        }
    }
    *result=0;return 1;
}
BOOL ProcessEditWindowNativeAt4855D0(EditWindowNativeBody98h& body,UINT message,WPARAM w,LPARAM l,
    LRESULT* result,DWORD map,EditWindowNativeMapServices& services)
{
    if(map)return 0;
    auto& api=services.api;auto& endpoint=body.outerAt00;
    if(message==WM_CREATE)
    {
        auto style=0x50000080u;
        const auto alignment=Read(body.alignmentAt80);
        if(alignment==1)style=0x50000081u;else if(alignment==2)style=0x50000082u;
        services.creation.CreateControlAt485628(body.controlAt24,Read(endpoint.windowAt04),style);
        body.identifierAt78=static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
            api.GetMenu(Read(body.controlAt24.endpointAt00.windowAt04))))&0xFFFFu;
        *result=0;return 1;
    }
    if(message==WM_SIZE)
    {
        if(Read(body.controlAt24.endpointAt00.windowAt04))
        {
            RECT rectangle{};
            if(api.GetClientRect(Read(endpoint.windowAt04),&rectangle))
            {
                RECT margins;
                if(Read(body.layoutAt6C.baseAt00))
                    std::memcpy(&margins,ResolveEnvironmentObjectReference<RECT>(body.layoutAt6C),16);
                else std::memcpy(&margins,&body.marginsAt5C,16);
                static_cast<void>(api.MoveWindow(Read(body.controlAt24.endpointAt00.windowAt04),
                    Add(rectangle.left,margins.left),Add(rectangle.top,margins.top),
                    Sub(Sub(Sub(rectangle.right,rectangle.left),margins.right),margins.left),
                    Sub(Sub(Sub(rectangle.bottom,rectangle.top),margins.bottom),margins.top),1));
            }
        }
        *result=0;return 1;
    }
    if(message==WM_ERASEBKGND)
    {
        if(Read(body.brushAt8C.baseAt00))
        {
            RECT rectangle{};
            if(api.GetClientRect(Read(endpoint.windowAt04),&rectangle))
            {
                const auto brush=*ResolveEnvironmentObjectReference<HBRUSH>(body.brushAt8C);
                const auto dc=reinterpret_cast<HDC>(w);
                const auto old=api.SelectObject(dc,brush);
                static_cast<void>(api.PatBlt(dc,rectangle.left,rectangle.top,Sub(rectangle.right,rectangle.left),
                    Sub(rectangle.bottom,rectangle.top),0x00F00021u));
                static_cast<void>(api.SelectObject(dc,old));
                *result=1;return 1;
            }
        }
        const auto window=Read(endpoint.windowAt04);const auto procedure=Read(endpoint.previousAt20);
        *result=api.CallWindowProcA(procedure,window,WM_ERASEBKGND,w,l);return 1;
    }
    if(message==WM_CTLCOLOREDIT)
    {
        if(Read(body.brushAt8C.baseAt00))
        {
            const auto dc=reinterpret_cast<HDC>(w);
            static_cast<void>(api.SetTextColor(dc,Read(body.textColorAt84)));
            static_cast<void>(api.SetBkMode(dc,1));
            *result=std::bit_cast<LRESULT>(reinterpret_cast<std::uint32_t>(
                *ResolveEnvironmentObjectReference<HBRUSH>(body.brushAt8C)));
            return 1;
        }
        const auto window=Read(endpoint.windowAt04);const auto procedure=Read(endpoint.previousAt20);
        *result=api.CallWindowProcA(procedure,window,WM_CTLCOLOREDIT,w,l);return 1;
    }
    if(message==WM_COMMAND)
    {
        BOOL unused;
        *result=ApplyEditWindowNativeByteLimitAt4824A0(body,WM_COMMAND,w,l,&unused,api,services.strings);return 1;
    }
    return 0;
}
}
