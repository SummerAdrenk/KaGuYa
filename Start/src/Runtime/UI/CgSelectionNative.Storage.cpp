#include <Runtime/CgSelectionNative.h>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Pair=CgSelectionPairNative8h;
std::uint32_t Bytes(const void* begin,const void* end) noexcept
{ return std::uint32_t(reinterpret_cast<std::uintptr_t>(end)-reinterpret_cast<std::uintptr_t>(begin)); }
std::uint32_t Count(const void* begin,const void* end) noexcept
{ return std::uint32_t(std::bit_cast<std::int32_t>(Bytes(begin,end))>>3); }
Pair* Offset(Pair* begin,std::uint32_t count) noexcept
{ return reinterpret_cast<Pair*>(reinterpret_cast<std::uintptr_t>(begin)+count*8u); }
void Free(Pair* begin,Pair* capacity,IEnvironmentRootMemory& memory)
{
    auto bytes=Bytes(begin,capacity)&0xFFFFFFF8u;
    void* raw=begin;
    if(bytes>=0x1000)
    {
        std::memcpy(&raw,reinterpret_cast<const std::byte*>(begin)-4,4);
        bytes+=0x23u;
        if(reinterpret_cast<std::uintptr_t>(begin)-reinterpret_cast<std::uintptr_t>(raw)-4u>0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw,bytes);
}
Pair* Allocate(std::uint32_t capacity,IEnvironmentRootMemory& memory)
{
    auto bytes=capacity*8u;
    if(capacity>0x1FFFFFFFu)bytes=0xFFFFFFFFu;
    if(bytes<0x1000u)return bytes?static_cast<Pair*>(memory.AllocateBody(bytes)):nullptr;
    const auto addition=bytes+0x23u;
    const auto requested=addition<=bytes?0xFFFFFFFFu:addition;
    auto* raw=memory.AllocateBody(requested);
    if(!raw)_invalid_parameter_noinfo_noreturn();
    const auto aligned=(reinterpret_cast<std::uintptr_t>(raw)+0x23u)&~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned-4),&raw,4);
    return reinterpret_cast<Pair*>(aligned);
}
void Copy(Pair& to,const Pair& from) noexcept
{ to.ordinalAt00=from.ordinalAt00;to.resourceIdAt04=from.resourceIdAt04; }
}
Pair* InsertCgSelectionPairNativeAt499780(CgSelectionPairVector& storage,Pair* position,
    const Pair& pair,IEnvironmentRootMemory& memory)
{
    const auto insertion=Count(storage.begin,position);
    const auto size=Count(storage.begin,storage.end);
    if(size==0x1FFFFFFFu)throw std::length_error("vector<T> too long");
    const auto required=size+1u;
    const auto capacity=Count(storage.begin,storage.capacity);
    const auto growth=capacity+capacity/2u;
    const auto nextCapacity=capacity>0x1FFFFFFFu-capacity/2u?required:(growth<required?required:growth);
    auto* next=Allocate(nextCapacity,memory);
    auto* inserted=Offset(next,insertion);
    // 499842先读插入源；分配回调之后才取当前旧begin/end；不能memcpy旧快照。
    Copy(*inserted,pair);
    auto* output=next;
    auto* end=storage.end;
    auto* cursor=storage.begin;
    if(position==end)
    {
        while(cursor!=end){Copy(*output,*cursor);output=Offset(output,1);cursor=Offset(cursor,1);}
    }
    else
    {
        while(cursor!=position){Copy(*output,*cursor);output=Offset(output,1);cursor=Offset(cursor,1);}
        end=storage.end;output=Offset(inserted,1);cursor=position;
        while(cursor!=end){Copy(*output,*cursor);output=Offset(output,1);cursor=Offset(cursor,1);}
    }
    if(storage.begin)Free(storage.begin,storage.capacity,memory);
    storage.begin=next;storage.end=Offset(next,required);storage.capacity=Offset(next,nextCapacity);
    return Offset(storage.begin,insertion);
}
void AppendCgSelectionPairNativeAt4B7584(CgSelectionPairVector& storage,const Pair& pair,IEnvironmentRootMemory& memory)
{
    auto* position=storage.end;
    if(position==storage.capacity){(void)InsertCgSelectionPairNativeAt499780(storage,position,pair,memory);return;}
    Copy(*position,pair);storage.end=Offset(storage.end,1);
}
void DestroyCgSelectionNativeAt4A4F40(CgSelectionNativeBody20h& body,IEnvironmentRootMemory& memory)
{
    auto& pairs=body.pairsAt14;
    if(!pairs.begin)return;
    Free(pairs.begin,pairs.capacity,memory);
    pairs.begin=nullptr;pairs.end=nullptr;pairs.capacity=nullptr;
}
}
