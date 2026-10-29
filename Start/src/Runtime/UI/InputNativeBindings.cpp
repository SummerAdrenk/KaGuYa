#include <Runtime/InputNativeObjects.h>
#include <bit>
#include <algorithm>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Binding=KeyboardBindingNative3;
using Storage=EnvironmentVectorStorage<Binding>;
std::uint32_t Address(const void* p) noexcept{return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p));}
std::uint32_t Count(const void* end,const void* begin) noexcept
{return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Address(end)-Address(begin))/3);}
Binding* Allocate(std::uint32_t count,IEnvironmentRootMemory& memory)
{
    const auto bytes=count>0x55555555u?0xFFFFFFFFu:count*3u;
    if(bytes<0x1000u)return bytes?static_cast<Binding*>(memory.AllocateBody(bytes)):nullptr;
    const auto extended=bytes+0x23u;
    void* raw=memory.AllocateBody(extended<=bytes?0xFFFFFFFFu:extended);
    if(!raw)_invalid_parameter_noinfo_noreturn();
    const auto aligned=(Address(raw)+0x23u)&0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned-4u),&raw,4);
    return reinterpret_cast<Binding*>(aligned);
}
void Delete(Binding* begin,std::uint32_t bytes,IEnvironmentRootMemory& memory)
{
    void* raw=begin;
    if(bytes>=0x1000u)
    {
        std::memcpy(&raw,reinterpret_cast<const void*>(Address(begin)-4u),4);
        bytes+=0x23u;
        if(Address(begin)-Address(raw)-4u>0x1Fu)_invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw,bytes);
}
}
void AppendKeyboardBindingNativeAt44FA70(Storage& storage,const Binding& binding,IEnvironmentRootMemory& memory)
{
    auto* const position=storage.end;
    if(position!=storage.capacity){std::memcpy(position,&binding,3);storage.end=position+1;return;}
    const auto size=Count(storage.end,storage.begin),capacity=Count(storage.capacity,storage.begin);
    if(size==0x55555555u)throw std::length_error("vector<T> too long");
    const auto required=size+1u;
    const auto next=capacity>0x55555555u-capacity/2u?required:
        (std::max)(capacity+capacity/2u,required);
    auto* allocated=Allocate(next,memory);
    // 44FB48先复制新元素，随后才移旧元素；source可以来自旧存储本身。
    std::memcpy(allocated+size,&binding,3);
    if(size)std::memcpy(allocated,storage.begin,size*3u);
    if(storage.begin)Delete(storage.begin,Count(storage.capacity,storage.begin)*3u,memory);
    storage.begin=allocated;storage.end=allocated+required;storage.capacity=allocated+next;
}
void DestroyKeyboardBindingsNativeAt4500D0(Storage& storage,IEnvironmentRootMemory& memory)
{
    if(!storage.begin)return;
    Delete(storage.begin,Count(storage.capacity,storage.begin)*3u,memory);
    storage.begin=nullptr;storage.end=nullptr;storage.capacity=nullptr;
}
const std::uint8_t* MamatomoKeyboardNativeTableAt601358() noexcept
{
    static constexpr std::uint8_t table[]{
        0x00,0x1B,0x01,0x26,0x02,0x28,0x03,0x25,0x04,0x27,0x05,0x0D,0x1A,0x20,
        0x06,0x11,0x07,0x08,0x08,0x2E,0x09,0x21,0x0A,0x22,0x0B,0x09,0x0C,0x2D,
        0x1B,0x24,0x0D,0x10,0x0E,0x70,0x0F,0x71,0x10,0x72,0x11,0x73,0x12,0x74,
        0x13,0x75,0x14,0x76,0x15,0x77,0x16,0x78,0x17,0x79,0x18,0x7A,0x19,0x7B,
        0x1C,0x30,0x1D,0x31,0x1E,0x32,0x1F,0x33,0x20,0x34,0x21,0x35,0x22,0x36,
        0x23,0x37,0x24,0x38,0x25,0x39,0x26,0x60,0x27,0x61,0x28,0x62,0x29,0x63,
        0x2A,0x64,0x2B,0x65,0x2C,0x66,0x2D,0x67,0x2E,0x68,0x2F,0x69,0x30,0x49,
        0x31,0x4E,0x32,0x59,0xFF};
    return table;
}
}
