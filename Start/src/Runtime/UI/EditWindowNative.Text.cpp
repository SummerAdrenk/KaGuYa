#include <Runtime/EditWindowNative.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Win32Text final:public IEditWindowNativeTextApi
{
public:
    int GetWindowTextLengthA(HWND window) override{return ::GetWindowTextLengthA(window);}
    int GetWindowTextA(HWND window,LPSTR text,int capacity) override{return ::GetWindowTextA(window,text,capacity);}
};
[[noreturn]] void InvalidText(){throw ATL::CAtlException(E_INVALIDARG);}
char* Chars(RuntimeCString& text) noexcept
{return *reinterpret_cast<char* volatile*>(&text);}
ATL::CStringData* Header(char* chars) noexcept
{return reinterpret_cast<ATL::CStringData*>(chars)-1;}
}
IEditWindowNativeTextApi& DefaultEditWindowNativeTextApi() noexcept
{static Win32Text api;return api;}
RuntimeCString* ReadEditWindowNativeTextAt50B000(EditWindowNativeBody98h& body,RuntimeCString* out,
    IRuntimeStringManagerSource& strings,IEditWindowNativeTextApi& api)
{
    InitializeEmptyRuntimeCString(*out,strings);
    struct Unwind final
    {
        RuntimeCString* text;bool active=true;
        ~Unwind() noexcept{if(active)DestroyRuntimeCStringAt40CD40(text);}
    } unwind{out}; // 50B052/5A5E30：GetNil成功之后才保护返回槽。
    const auto length=api.GetWindowTextLengthA(body.controlAt24.endpointAt00.windowAt04);
    if(length<0)InvalidText();
    auto* data=Header(Chars(*out));
    const auto shared=1u-static_cast<std::uint32_t>(data->nRefs);
    const auto room=static_cast<std::uint32_t>(data->nAllocLength)-static_cast<std::uint32_t>(length);
    if(std::bit_cast<std::int32_t>(shared|room)<0)PrepareRuntimeCStringWriteAt42D590(*out,length);
    auto* chars=Chars(*out);data=Header(chars);
    if(length>data->nAllocLength)InvalidText();
    data->nDataLength=length;chars[length]=0;
    // 50B09F晚取同一正文+28；Win32返回整数被忽略，50B0A8之后按有界字节串定长。
    const auto count=std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(length)+1u);
    static_cast<void>(api.GetWindowTextA(body.controlAt24.endpointAt00.windowAt04,chars,count));
    chars=Chars(*out);
    int actual=0;
    if(chars)
    {
        // 54C210的上界是DWORD；返回值在50B0BA另按signed检查。
        actual=std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
            strnlen_s(chars,static_cast<std::uint32_t>(Header(chars)->nAllocLength))));
        if(actual<0)InvalidText();
    }
    data=Header(Chars(*out));
    if(actual>data->nAllocLength)InvalidText();
    data->nDataLength=actual;Chars(*out)[actual]=0;
    unwind.active=false;
    return out;
}
}
