#include <Runtime/PropertySheetNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
}
LRESULT CALLBACK BoundPropertySheetNativeAt439E40(HWND first,UINT m,WPARAM w,LPARAM l)
{return DispatchPropertySheetNativeAt439E40(*reinterpret_cast<EditWindowEndpointNativeBody24h*>(first),m,w,l,RequirePropertySheetNativeServices());}
LRESULT DispatchPropertySheetNativeAt439E40(EditWindowEndpointNativeBody24h& body,UINT message,WPARAM w,LPARAM l,PropertySheetNativeServices& s)
{
    EditWindowMessageNativeContext24h context;context.message.hwnd=Read(body.windowAt04);
    auto* previous=Read(body.currentMessageAt18);const auto window=Read(body.windowAt04);
    context.message.lParam=l;body.currentMessageAt18=&context;const auto* table=Read(body.vptrAt00);
    context.sizeAt1C=0x24;context.handledAt20=1;context.message.message=message;context.message.wParam=w;
    context.message.time=0;context.message.pt.y=0;context.message.pt.x=0;LRESULT result=0;
    if(!table->processAt00(&body,window,message,w,l,&result,0))
    {
        const auto selectedWindow=Read(body.windowAt04);
        if(message!=WM_NCDESTROY)result=s.api.CallWindowProcA(Read(body.previousAt20),selectedWindow,message,w,l);
        else
        {
            const auto original=s.api.GetWindowLongA(selectedWindow,GWL_WNDPROC);
            const auto nextWindow=Read(body.windowAt04);const auto proc=Read(body.previousAt20);
            result=s.api.CallWindowProcA(proc,nextWindow,WM_NCDESTROY,w,l);
            if(Read(body.previousAt20)!=&::DefWindowProcA)
            {
                const auto actual=s.api.GetWindowLongA(Read(body.windowAt04),GWL_WNDPROC);
                if(actual==original)
                {const auto next=std::bit_cast<LONG>(reinterpret_cast<DWORD>(Read(body.previousAt20)));static_cast<void>(s.api.SetWindowLongA(Read(body.windowAt04),GWL_WNDPROC,next));}
            }
            body.destroyPendingAt1C=Read(body.destroyPendingAt1C)|1u;
        }
    }
    const auto flags=Read(body.destroyPendingAt1C);
    if((flags&1u)&&!previous)
    {
        const auto finalWindow=Read(body.windowAt04);body.destroyPendingAt1C=flags&~1u;const auto* finalTable=Read(body.vptrAt00);
        body.windowAt04=nullptr;body.currentMessageAt18=nullptr;finalTable->finalMessageAt0C(&body,finalWindow);return result;
    }
    body.currentMessageAt18=previous;return result;
}
BOOL ProcessGamePropertyNativeAt42E590(GamePropertyNativeBody33Ch& body,HWND,UINT message,WPARAM w,LPARAM l,LRESULT* result,DWORD map,
    PropertySheetNativeServices& s)
{
    if(map)return FALSE;auto& sheet=body.sheetAt00;
    const auto forward=[&]()
    {
        const auto window=Read(sheet.endpointAt00.windowAt04);const auto previous=Read(sheet.endpointAt00.previousAt20);
        return s.api.CallWindowProcA(previous,window,message,w,l);
    };
    if(message==WM_SHOWWINDOW)
    {
        if(!Read(body.centeredAt338)){static_cast<void>(CenterDialogWindowNativeAt413990(&sheet.endpointAt00.windowAt04,nullptr,s.endpoint.api));body.centeredAt338=1;}
        *result=forward();return TRUE;
    }
    if(message==WM_DESTROY)
    {
        const auto window=Read(sheet.endpointAt00.windowAt04);
        HWND forwarded;
        if(!window){body.lastPageAt334=static_cast<std::int32_t>(Read(sheet.headerAt24.startAt1C));forwarded=nullptr;}
        else
        {
            const auto tab=reinterpret_cast<HWND>(s.api.SendMessageA(window,0x474,0,0));
            const auto selected=s.api.SendMessageA(tab,0x130B,0,0);
            forwarded=Read(sheet.endpointAt00.windowAt04);body.lastPageAt334=static_cast<std::int32_t>(selected);
        }
        *result=s.api.CallWindowProcA(Read(sheet.endpointAt00.previousAt20),forwarded,WM_DESTROY,w,l);return TRUE;
    }
    if(message==WM_COMMAND)
    {
        const auto value=forward();
        if(!HIWORD(w)&&(LOWORD(w)==1||LOWORD(w)==2)&&(Read(sheet.headerAt24.flagsAt04)&0x400u))
            if(!s.api.SendMessageA(Read(sheet.endpointAt00.windowAt04),0x476,0,0))static_cast<void>(s.api.DestroyWindow(Read(sheet.endpointAt00.windowAt04)));
        *result=value;return TRUE;
    }
    if(message==WM_SYSCOMMAND)
    {
        if((Read(sheet.headerAt24.flagsAt04)&0x400u)&&((w&0xFFF0u)==SC_CLOSE))
        {static_cast<void>(s.api.SendMessageA(Read(sheet.endpointAt00.windowAt04),WM_CLOSE,0,0));*result=0;return TRUE;}
        *result=0;return FALSE;
    }
    return FALSE;
}
}
