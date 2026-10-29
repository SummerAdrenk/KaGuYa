#include <Runtime/EditWindowNativeCollection.h>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;
constexpr std::uint32_t Maximum=0x15555555u;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
std::uint32_t Address(const void* p) noexcept{return reinterpret_cast<std::uint32_t>(p);}
std::uint32_t Distance(const Slot* end,const Slot* begin) noexcept
{return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Address(end)-Address(begin))/12);}
Slot* At(Slot* p,std::uint32_t bytes) noexcept{return reinterpret_cast<Slot*>(Address(p)+bytes);}
void Copy(Slot& out,const Slot& in) noexcept
{
    out.baseAt00=Read(in.baseAt00);out.biasAt04=Read(in.biasAt04);out.controlAt08=Read(in.controlAt08);
    if(out.controlAt08)++out.controlAt08->referenceCountAt04;
}
void* Resolve(const Slot& slot,std::uint32_t base) noexcept
{
    if(!base)return nullptr;
    auto* control=Read(slot.controlAt08);
    const auto stride=control?Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C):0u;
    return reinterpret_cast<void*>(base+Read(slot.biasAt04)*stride);
}
void* Resolve(const Slot& slot) noexcept{return Resolve(slot,Read(slot.baseAt00));}
void ReleaseRange(Slot* begin,Slot* end) noexcept
{for(;begin!=end;begin=At(begin,12))ReleaseYtlReference(*begin);}
Slot* Allocate(std::uint32_t capacity)
{
    const auto bytes=capacity<=Maximum?capacity*12u:0xFFFFFFFFu;
    if(bytes<0x1000u)return bytes?static_cast<Slot*>(::operator new(bytes)):nullptr;
    const auto expanded=bytes+0x23u;void* raw=::operator new(expanded<=bytes?0xFFFFFFFFu:expanded);
    if(!raw)_invalid_parameter_noinfo_noreturn();
    auto* aligned=reinterpret_cast<Slot*>((Address(raw)+0x23u)&~31u);
    std::memcpy(At(aligned,0xFFFFFFFCu),&raw,4);return aligned;
}
void DeleteStorage(Slot* begin,Slot* capacity) noexcept
{
    auto bytes=Distance(capacity,begin)*12u;void* raw=begin;
    if(bytes>=0x1000u)
    {
        std::memcpy(&raw,At(begin,0xFFFFFFFCu),4);bytes+=0x23u;
        if(Address(begin)-Address(raw)-4u>0x1Fu)_invalid_parameter_noinfo_noreturn();
    }
    ::operator delete(raw,bytes);
}
Slot* CopyRange(Slot* begin,Slot* end,Slot* out) noexcept
{for(;begin!=end;begin=At(begin,12),out=At(out,12))Copy(*out,*begin);return out;}
}
Slot* GrowEditWindowNativeReferencesAt452E60(EditWindowNativeReferences& refs,Slot* position,const Slot* source)
{
    const auto index=Distance(position,Read(refs.begin)),size=Distance(Read(refs.end),Read(refs.begin));
    if(size==Maximum)throw std::length_error("vector<T> too long");
    const auto required=size+1,capacity=Distance(Read(refs.capacity),Read(refs.begin)),half=capacity>>1;
    const auto grown=capacity>Maximum-half?required:capacity+half;
    const auto nextCapacity=grown<required?required:grown;
    auto* next=Allocate(nextCapacity);auto* inserted=At(next,index*12u);
    auto* cleanupBegin=inserted;
    try
    {
        Copy(*inserted,*source);
        auto* oldEnd=Read(refs.end); // 452F91本次EBX范围界限固定，不按每项回读end。
        if(position==oldEnd)static_cast<void>(CopyRange(Read(refs.begin),oldEnd,next));
        else
        {
            static_cast<void>(CopyRange(Read(refs.begin),position,next));
            cleanupBegin=next; // 第二段失败时原清理开始位置已经改为new begin。
            static_cast<void>(CopyRange(position,Read(refs.end),At(inserted,12)));
        }
    }
    catch(...){ReleaseRange(cleanupBegin,At(inserted,12));DeleteStorage(next,At(next,nextCapacity*12u));throw;}
    // 45300B先撤保护；旧释放回调之后重读begin/capacity，再发布新头。
    if(Read(refs.begin))
    {
        ReleaseRange(Read(refs.begin),Read(refs.end));
        DeleteStorage(Read(refs.begin),Read(refs.capacity));
    }
    refs.begin=next;refs.end=At(next,required*12u);refs.capacity=At(next,nextCapacity*12u);
    return At(refs.begin,index*12u);
}
void AppendEditWindowNativeReferenceAt46FC43(EditWindowNativeReferences& refs,const Slot& source)
{
    auto* position=Read(refs.end);
    if(position==Read(refs.capacity)){static_cast<void>(GrowEditWindowNativeReferencesAt452E60(refs,position,&source));return;}
    position->baseAt00=Read(source.baseAt00);position->biasAt04=0;position->controlAt08=Read(source.controlAt08);
    if(position->controlAt08)++position->controlAt08->referenceCountAt04;
    refs.end=At(Read(refs.end),12);
}
Slot* FindEditWindowNativeAt479DF0(EditWindowNativeReferences& refs,Slot* out,const void* target)
{
    out->baseAt00=0;out->biasAt04=0;out->controlAt08=nullptr;
    auto* at=Read(refs.begin);auto* end=Read(refs.end);
    for(;at!=end;at=At(at,12))
    {
        const auto base=Read(at->baseAt00);
        if(Resolve(*at,base)!=target)continue;
        if(out!=at)
        {
            out->baseAt00=base;out->biasAt04=Read(at->biasAt04);out->controlAt08=Read(at->controlAt08);
            if(out->controlAt08)++out->controlAt08->referenceCountAt04;
        }
        break;
    }
    return out;
}
void RemoveEditWindowNativeAt486B60(EditWindowNativeReferences& refs,const void* target)
{
    if(!target){ReleaseRange(Read(refs.begin),Read(refs.end));refs.end=Read(refs.begin);return;}
    auto* capturedEnd=Read(refs.end);auto* position=Read(refs.begin);
    if(position==capturedEnd)return;
    for(;;)
    {
        if(Resolve(*position)==target)break;
        position=At(position,12);
        if(position==Read(refs.end))return;
    }
    auto* source=At(position,12);
    while(source!=capturedEnd)
    {
        if(position!=source){ReleaseYtlReference(*position);Copy(*position,*source);}
        source=At(source,12);position=At(position,12);
    }
    auto* last=At(Read(refs.end),0xFFFFFFF4u);
    // 486C89进入state1终止尾；内联移位期间的异常则允许传播。
    ReleaseRange(last,At(last,12));
    refs.end=At(Read(refs.end),0xFFFFFFF4u);
}
void DestroyEditWindowNativeReferencesAt40DE10(EditWindowNativeReferences& refs) noexcept
{
    auto* begin=Read(refs.begin);if(!begin)return;
    auto* end=Read(refs.end);ReleaseRange(begin,end);
    // 非空范围在回调后重读begin；空范围保留入口ESI，与原40DE97相同。
    if(begin!=end)begin=Read(refs.begin);
    DeleteStorage(begin,Read(refs.capacity));refs.begin=nullptr;refs.end=nullptr;refs.capacity=nullptr;
}
}
