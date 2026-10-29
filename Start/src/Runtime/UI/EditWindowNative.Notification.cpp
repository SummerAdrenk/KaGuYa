#include <Runtime/EditWindowNativeMaps.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
[[noreturn]] void InvalidText(){throw ATL::CAtlException(E_INVALIDARG);}
struct UnwrittenText final{union{RuntimeCString text;};UnwrittenText() noexcept{}~UnwrittenText() noexcept{}};
ATL::CStringData* Header(const char* chars) noexcept{return reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars))-1;}
}
LRESULT ApplyEditWindowNativeByteLimitAt4824A0(EditWindowNativeBody98h& body,UINT,WPARAM w,LPARAM,BOOL*,
    IEditWindowNativeMapApi& api,IRuntimeStringManagerSource& strings)
{
    const auto bits=static_cast<std::uint32_t>(w);
    if((bits&0xFFFFu)!=Read(body.identifierAt78)||(bits&0xFFFF0000u)!=0x03000000u||Read(body.byteLimitAt7C)<=0)return 0;
    UnwrittenText original;
    InitializeEmptyRuntimeCString(original.text,strings);
    struct Unwind final
    {
        RuntimeCString& text;bool active=true;
        ~Unwind() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&text);}
    } lifetime{original.text};
    const auto requested=api.GetWindowTextLengthA(Read(body.controlAt24.endpointAt00.windowAt04));
    if(requested<0)InvalidText();
    auto* chars=const_cast<char*>(original.text.GetString());auto* header=Header(chars);
    const auto room=std::uint32_t(header->nAllocLength)-std::uint32_t(requested);
    const auto shared=1u-std::uint32_t(header->nRefs);
    if(std::bit_cast<std::int32_t>(room|shared)<0)
    {PrepareRuntimeCStringWriteAt42D590(original.text,requested);chars=const_cast<char*>(original.text.GetString());}
    header=Header(chars);if(requested>header->nAllocLength)InvalidText();
    header->nDataLength=requested;chars[requested]=0;
    static_cast<void>(api.GetWindowTextA(Read(body.controlAt24.endpointAt00.windowAt04),chars,Add(requested,1)));
    // 原482572仍使用此前EDI，不与50B000的回读返回槽混成同一helper。
    const auto actual=std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        strnlen_s(chars,static_cast<std::uint32_t>(Header(chars)->nAllocLength))));
    header=Header(chars);if(actual<0||actual>header->nAllocLength)InvalidText();
    header->nDataLength=actual;chars[actual]=0;
    if(Header(chars)->nDataLength>Read(body.byteLimitAt7C))
    {
        const auto selection=static_cast<std::uint32_t>(api.SendMessageA(Read(body.controlAt24.endpointAt00.windowAt04),EM_GETSEL,0,0))&0xFFFFu;
        std::int32_t previous=0,position=0,cut=0;
        for(;;)
        {
            cut=previous;
            if(position<0||position>Header(chars)->nDataLength)InvalidText();
            const auto character=static_cast<std::int32_t>(static_cast<signed char>(chars[position]));
            if(api.IsMbLead(std::bit_cast<std::uint32_t>(character)))position=Add(position,1);
            const auto before=position;position=Add(position,1);
            const auto limit=Read(body.byteLimitAt7C);
            if(before<limit)
            {
                previous=position;
                if(position<limit)continue;
            }
            if(before>=limit)position=cut;
            break;
        }
        UnwrittenText prefix;
        // 原第二TEXT无额外EH态；Win32回调失败只清original，不伪造prefix回收。
        ConstructRuntimeCStringLeftAt419D00(&prefix.text,original.text,position,strings);
        const auto* selected=prefix.text.GetString();
        static_cast<void>(api.SetWindowTextA(Read(body.controlAt24.endpointAt00.windowAt04),selected));
        static_cast<void>(api.SendMessageA(Read(body.controlAt24.endpointAt00.windowAt04),EM_SETSEL,selection,selection));
        static_cast<void>(api.SendMessageA(Read(body.controlAt24.endpointAt00.windowAt04),EM_SCROLLCARET,0,0));
        ReleaseRuntimeCStringHeader(Header(selected));
    }
    lifetime.active=false; // 482653撤state0后正常Free可抛，不重试第一次TEXT。
    ReleaseRuntimeCStringHeader(Header(chars));
    return 0;
}
}
