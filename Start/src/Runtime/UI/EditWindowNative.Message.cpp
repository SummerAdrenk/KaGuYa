#include <Runtime/EditWindowNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32Messages final:public IEditWindowNativeMessageApi
{
public:
    LRESULT CallWindowProcA(WNDPROC p,HWND window,UINT message,WPARAM w,LPARAM l) override
    {return ::CallWindowProcA(p,window,message,w,l);}
    LONG GetWindowLongA(HWND window,int index) override{return ::GetWindowLongA(window,index);}
    LONG SetWindowLongA(HWND window,int index,LONG value) override{return ::SetWindowLongA(window,index,value);}
};
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
}
IEditWindowNativeMessageApi& DefaultEditWindowNativeMessageApi() noexcept
{static Win32Messages api;return api;}
LRESULT DispatchEditWindowNativeMessageAt439F60(EditWindowEndpointNativeBody24h& body,UINT message,WPARAM w,LPARAM l,
    IEditWindowNativeMessageApi& api)
{
    auto* previous=Read(body.currentMessageAt18);
    EditWindowMessageNativeContext24h current;
    current.message.hwnd=Read(body.windowAt04);
    current.message.lParam=l;
    body.currentMessageAt18=&current;
    current.sizeAt1C=0x24;current.handledAt20=1;
    current.message.message=message;current.message.wParam=w;
    current.message.time=0;current.message.pt={0,0};
    LRESULT result=0;
    const auto* table=Read(body.vptrAt00);
    const auto window=Read(body.windowAt04);
    if(!table->processAt00(&body,window,message,w,l,&result,0))
    {
        const auto selectedWindow=Read(body.windowAt04); // 439FD0在判NCDESTROY之前读取。
        if(message!=WM_NCDESTROY)
        {
            const auto procedure=Read(body.previousAt20);
            result=api.CallWindowProcA(procedure,selectedWindow,message,w,l);
        }
        else
        {
            const auto old=api.GetWindowLongA(selectedWindow,GWL_WNDPROC);
            const auto nextWindow=Read(body.windowAt04);
            const auto procedure=Read(body.previousAt20);
            result=api.CallWindowProcA(procedure,nextWindow,WM_NCDESTROY,w,l);
            if(Read(body.previousAt20)!=&::DefWindowProcA)
            {
                const auto actual=api.GetWindowLongA(Read(body.windowAt04),GWL_WNDPROC);
                if(actual==old)
                {
                    const auto replacement=std::bit_cast<LONG>(reinterpret_cast<std::uint32_t>(Read(body.previousAt20)));
                    static_cast<void>(api.SetWindowLongA(Read(body.windowAt04),GWL_WNDPROC,replacement));
                }
            }
            body.destroyPendingAt1C=Read(body.destroyPendingAt1C)|1u;
        }
    }
    const auto flags=Read(body.destroyPendingAt1C);
    if((flags&1u)&&!previous)
    {
        const auto finalWindow=Read(body.windowAt04);
        body.destroyPendingAt1C=flags&~1u;
        const auto* finalTable=Read(body.vptrAt00);
        body.windowAt04=nullptr;body.currentMessageAt18=nullptr;
        finalTable->finalMessageAt0C(&body,finalWindow);
        // 43A061后仅返回局部result；FinalMessage可销毁正文，不允许再访body。
        return result;
    }
    body.currentMessageAt18=previous;
    return result;
}
}
