#include <Runtime/EnvironmentErrorRuntime.h>
#include <Runtime/SpriteNativeStorage.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
union Text{RuntimeCString value;Text()noexcept{}~Text()noexcept{}};
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
ATL::CStringData* Header(const char* p)noexcept
{return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(p)-16u);}
char* Offset(char* p,std::uint32_t offset)noexcept
{return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
std::uint32_t Count(const RuntimeCString* begin,const RuntimeCString* end)noexcept
{
    const auto n=std::uint32_t(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin));
    return (n>>2)|((n&0x80000000u)?0xC0000000u:0u);
}
}
void DisplayEnvironmentErrorsAt4AC730(EnvironmentRootStorage& root,decltype(&::MessageBoxA) messageBox,IRuntimeStringManagerSource& strings)
{
    auto* end=Read(root.errorsAt3C.end);
    auto* begin=Read(root.errorsAt3C.begin);
    const auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin));
    if(bytes<4)return;
    Text text;InitializeEmptyRuntimeCString(text.value,strings);
    struct Unwind final{RuntimeCString& text;bool active=true;~Unwind()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&text);}} unwind{text.value};
    SetRuntimeCStringAt437680(text.value,"",0);
    // 4AC7BD缓存ESI，只有4AC80C/4AC894在扩容后重取输出槽。
    auto* cached=const_cast<char*>(text.value.GetString());
    end=Read(root.errorsAt3C.end);begin=Read(root.errorsAt3C.begin);
    auto count=Count(begin,end);
    std::uint32_t index=0;
    if(count)do
    {
        if(index)
        {
            const auto previous=Read(Header(cached)->nDataLength);
            const auto length=std::bit_cast<std::int32_t>(std::uint32_t(previous)+1u);
            if(length<0)throw ATL::CAtlException(E_INVALIDARG);
            const auto capacity=Read(Header(cached)->nAllocLength);
            const auto refs=Read(Header(cached)->nRefs);
            const auto prepare=(std::uint32_t(capacity)-std::uint32_t(length))|(1u-std::uint32_t(refs));
            if(std::bit_cast<std::int32_t>(prepare)<0)
            {
                PrepareRuntimeCStringWriteAt42D590(text.value,length);
                cached=const_cast<char*>(text.value.GetString());
            }
            *Offset(cached,std::uint32_t(previous))='\n';
            if(length>Read(Header(cached)->nAllocLength))throw ATL::CAtlException(E_INVALIDARG);
            Header(cached)->nDataLength=length;*Offset(cached,std::uint32_t(length))=0;
            begin=Read(root.errorsAt3C.begin);
        }
        const auto* element=reinterpret_cast<const RuntimeCString*>(reinterpret_cast<std::uintptr_t>(begin)+index*4u);
        const auto* source=element->GetString();
        const auto previous=Read(Header(cached)->nDataLength);
        const auto offset=std::uint32_t(reinterpret_cast<std::uintptr_t>(source)-reinterpret_cast<std::uintptr_t>(cached));
        const auto limit=Read(Header(source)->nDataLength);
        if(limit<0)throw ATL::CAtlException(E_INVALIDARG);
        const auto added=source?std::uint32_t(strnlen_s(source,std::uint32_t(limit))):0u;
        if(std::bit_cast<std::int32_t>(0x7FFFFFFFu-added)<previous)throw ATL::CAtlException(E_INVALIDARG);
        const auto length=std::bit_cast<std::int32_t>(std::uint32_t(previous)+added);
        if(length<0)throw ATL::CAtlException(E_INVALIDARG);
        const auto capacity=Read(Header(cached)->nAllocLength);
        const auto refs=Read(Header(cached)->nRefs);
        const auto prepare=(std::uint32_t(capacity)-std::uint32_t(length))|(1u-std::uint32_t(refs));
        if(std::bit_cast<std::int32_t>(prepare)<0)
        {
            PrepareRuntimeCStringWriteAt42D590(text.value,length);
            cached=const_cast<char*>(text.value.GetString());
        }
        // 4AC89A..4AC8C4复制后仍使用原length/offset及缓存ESI。
        if(offset<=std::uint32_t(previous))source=Offset(cached,offset);
        static_cast<void>(MoveRuntimeBytesAt43D9B0(Offset(cached,std::uint32_t(previous)),added,source,added));
        if(length>Read(Header(cached)->nAllocLength))throw ATL::CAtlException(E_INVALIDARG);
        Header(cached)->nDataLength=length;++index;*Offset(cached,std::uint32_t(length))=0;
        end=Read(root.errorsAt3C.end);begin=Read(root.errorsAt3C.begin);count=Count(begin,end);
    }while(index<count);
    const auto* title=root.titleAt10.GetString();const auto window=reinterpret_cast<HWND>(std::uintptr_t(Read(root.windowAt2C)));
    static_cast<void>(messageBox(window,cached,title,MB_ICONERROR));
    // 43F240保存元素范围，回调后重读begin；正常清理可能终止，不能改成吞异常。
    ClearSpriteNameVectorAt43F240(root.errorsAt3C);
    unwind.active=false;ReleaseRuntimeCStringHeader(Header(cached));
}
}
