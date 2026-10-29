#include <Runtime/GuiClickedEventNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const void* p,unsigned offset)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<std::uintptr_t>(p)+offset);}
GuiClickedEventNativeBody* Resolve(const YtlReferenceSlotRuntime* slot,std::uintptr_t base)noexcept
{
    if(!base)return nullptr;
    auto* control=Read<YtlReferenceControlRuntime*>(slot,8);
    const auto stride=control?Read<unsigned>(control,0x0C):0u;
    return reinterpret_cast<GuiClickedEventNativeBody*>(base+Read<unsigned>(slot,4)*stride);
}
}
std::int32_t PollGuiClickedEventManagerNativeAt50FDB0(GuiClickedEventReferences* manager,std::int32_t clearMatched)
{
    auto* position=Read<YtlReferenceSlotRuntime*>(manager,0);
    const auto* end=Read<YtlReferenceSlotRuntime*>(manager,4);
    for(;position!=end;++position)
    {
        const auto base=Read<std::uintptr_t>(position,0);
        if(Read<std::int32_t>(Resolve(position,base),4)==0)continue;
        const auto value=Read<std::int32_t>(Resolve(position,base),8);
        if(clearMatched)
        {
            auto* body=Resolve(position,base);
            const auto* table=Read<const GuiClickedEventNativeVtable*>(body,0);
            Read<decltype(GuiClickedEventNativeVtable::clearAt04)>(table,4)(body);
        }
        return value;
    }
    return -1;
}
}
