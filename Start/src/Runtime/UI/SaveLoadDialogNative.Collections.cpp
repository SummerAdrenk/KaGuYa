#include <Runtime/SaveLoadDialogNative.h>
#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Distance(const POINT* begin,const POINT* end) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin)))>>3;}
void DeleteVector(POINT* begin,POINT* capacity) noexcept
{
    auto bytes=std::uint32_t(reinterpret_cast<std::uintptr_t>(capacity)-reinterpret_cast<std::uintptr_t>(begin))&0xFFFFFFF8u;
    void* raw=begin;
    if(bytes>=0x1000)
    {
        std::memcpy(&raw,reinterpret_cast<const std::byte*>(begin)-4,sizeof(raw));
        bytes+=0x23;
        if(reinterpret_cast<std::uintptr_t>(begin)-reinterpret_cast<std::uintptr_t>(raw)-4u>0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw,bytes);
}
void Copy(POINT* out,const POINT* begin,const POINT* end) noexcept
{
    for(;begin!=end;++begin,++out){out->x=begin->x;out->y=begin->y;}
}
}
POINT* GrowSaveLoadPagePositionsNativeAt4D8D30(SaveLoadPagePositionsNative& refs,POINT* position,const POINT& source)
{
    constexpr std::uint32_t maximum=0x1FFFFFFFu;
    const auto index=std::uint32_t(Distance(refs.begin,position));
    const auto size=std::uint32_t(Distance(refs.begin,refs.end));
    if(size==maximum)throw std::length_error("vector<T> too long");
    const auto required=size+1,capacity=std::uint32_t(Distance(refs.begin,refs.capacity));
    const auto nextCapacity=capacity>maximum-capacity/2?required:(std::max)(required,capacity+capacity/2);
    const auto capacityBytes=nextCapacity*8u;
    const auto allocationBytes=nextCapacity>maximum?0xFFFFFFFFu:capacityBytes;
    POINT* next=nullptr;
    if(allocationBytes>=0x1000)
    {
        const auto request=allocationBytes>0xFFFFFFFFu-0x23u?0xFFFFFFFFu:allocationBytes+0x23u;
        auto* raw=::operator new(request);
        if(!raw)_invalid_parameter_noinfo_noreturn();
        const auto aligned=(reinterpret_cast<std::uintptr_t>(raw)+0x23u)&~std::uintptr_t{31};
        std::memcpy(reinterpret_cast<void*>(aligned-4),&raw,sizeof(raw));
        next=reinterpret_cast<POINT*>(aligned);
    }
    else if(allocationBytes)next=static_cast<POINT*>(::operator new(allocationBytes));
    // 4D8DDC分配成功后才读取source；再读取当时旧头，不能预拷贝别名源或旧范围。
    auto* inserted=next+index;inserted->x=source.x;inserted->y=source.y;
    auto* currentEnd=refs.end;
    if(position==currentEnd)Copy(next,refs.begin,currentEnd);
    else{Copy(next,refs.begin,position);currentEnd=refs.end;Copy(inserted+1,position,currentEnd);}
    if(refs.begin)DeleteVector(refs.begin,refs.capacity);
    refs.begin=next;refs.end=next+required;refs.capacity=reinterpret_cast<POINT*>(reinterpret_cast<std::uintptr_t>(next)+capacityBytes);
    return next+index;
}
void DestroySaveLoadPagePositionsNativeAt4F7130(SaveLoadPagePositionsNative& refs) noexcept
{
    if(!refs.begin)return;
    DeleteVector(refs.begin,refs.capacity);refs={};
}
}
