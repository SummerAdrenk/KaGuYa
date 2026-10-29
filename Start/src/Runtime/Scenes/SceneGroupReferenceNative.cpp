#include <Runtime/SceneGroupReferenceNative.h>
#include <algorithm>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;
using Groups=SceneGroupReferencesNative;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
Slot* Advance(Slot* p,int n) noexcept
{return reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(p)+std::uint32_t(n)*12u);}
int Distance(Slot* first,Slot* last) noexcept
{return std::bit_cast<int>(reinterpret_cast<std::uintptr_t>(last)-reinterpret_cast<std::uintptr_t>(first))/12;}
Slot Copy(const Slot& source) noexcept
{
    Slot result;result.baseAt00=Read(source.baseAt00);result.biasAt04=Read(source.biasAt04);
    result.controlAt08=Read(source.controlAt08);
    if(auto* control=result.controlAt08)++control->referenceCountAt04;
    return result;
}
void ReleaseRange(Slot* first,Slot* last) noexcept
{
    // 498B80/5F0C94每元素均为terminate边界；回调可改当前槽，但不改已捕获end。
    for(auto* p=first;p!=last;p=Advance(p,1))ReleaseYtlReference(*p);
}
Slot* Allocate(unsigned capacity)
{
    const auto bytes=capacity>0x15555555u?0xFFFFFFFFu:capacity*12u;
    if(bytes<0x1000u)return bytes?static_cast<Slot*>(::operator new(std::size_t{bytes})):nullptr;
    const auto extra=bytes+0x23u;
    auto* raw=::operator new(std::size_t{extra<=bytes?0xFFFFFFFFu:extra});
    if(!raw)_invalid_parameter_noinfo_noreturn();
    const auto aligned=(reinterpret_cast<std::uintptr_t>(raw)+0x23u)&~std::uintptr_t{31};
    reinterpret_cast<void**>(aligned)[-1]=raw;return reinterpret_cast<Slot*>(aligned);
}
void FreeBytes(Slot* first,unsigned bytes)
{
    void* raw=first;
    if(bytes>=0x1000u)
    {
        raw=reinterpret_cast<void**>(first)[-1];bytes+=0x23u;
        if(reinterpret_cast<std::uintptr_t>(first)-reinterpret_cast<std::uintptr_t>(raw)-4u>0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw,std::size_t{bytes});
}
void Free(Slot* first,Slot* capacity){FreeBytes(first,std::uint32_t(Distance(first,capacity))*12u);}
}
void AppendSceneGroupReferenceNativeAt499030(SceneGroupReferencesNative& groups,const YtlReferenceSlotRuntime& source)
{
    auto* position=Read(groups.end);
    if(position!=Read(groups.capacity))
    {*position=Copy(source);groups.end=Advance(position,1);return;}
    // 49E6E8从尾插进入；分配器重入改end后，49916A仍可能转到原非尾插分支。
    const auto size=unsigned(Distance(Read(groups.begin),position));
    if(size==0x15555555u)throw std::length_error("vector<T> too long");
    const auto capacity=unsigned(Distance(Read(groups.begin),Read(groups.capacity)));
    const auto required=size+1u;
    const auto grown=capacity>0x15555555u-capacity/2u?required:capacity+capacity/2u;
    const auto nextCapacity=(std::max)(required,grown);
    auto* next=Allocate(nextCapacity);auto* inserted=Advance(next,int(size));
    auto* protectedFirst=inserted;auto* protectedEnd=Advance(inserted,1);
    // 分配器和source均可重入；原499161固定当时end，之后不循环重读end。
    try
    {
        *inserted=Copy(source);auto* last=Read(groups.end);auto* output=next;
        if(position==last)
        {
            for(auto* p=Read(groups.begin);p!=last;p=Advance(p,1),output=Advance(output,1))*output=Copy(*p);
            ReleaseRange(output,output);
        }
        else
        {
            for(auto* p=Read(groups.begin);p!=position;p=Advance(p,1),output=Advance(output,1))*output=Copy(*p);
            ReleaseRange(output,output);protectedFirst=next;output=Advance(inserted,1);last=Read(groups.end);
            for(auto* p=position;p!=last;p=Advance(p,1),output=Advance(output,1))*output=Copy(*p);
            ReleaseRange(output,output);
        }
    }
    catch(...)
    {
        // 499263按当前受保护区间做4C5500，再4C5930释放新块，随后重抛；不回滚旧vector。
        // 原handler内正常清理；回调此刻不处于rethrow的展开阶段。
        ReleaseRange(protectedFirst,protectedEnd);FreeBytes(next,nextCapacity*12u);
        throw;
    }
    if(auto* first=Read(groups.begin))
    {ReleaseRange(first,Read(groups.end));Free(Read(groups.begin),Read(groups.capacity));}
    groups.begin=next;groups.end=Advance(next,int(required));groups.capacity=Advance(next,int(nextCapacity));
}
void DestroySceneGroupReferencesNativeAt4A3950(SceneGroupReferencesNative& groups) noexcept
{
    if(auto* first=Read(groups.begin))
    {
        ReleaseRange(first,Read(groups.end));Free(Read(groups.begin),Read(groups.capacity));
        groups.begin=nullptr;groups.end=nullptr;groups.capacity=nullptr;
    }
}
}
