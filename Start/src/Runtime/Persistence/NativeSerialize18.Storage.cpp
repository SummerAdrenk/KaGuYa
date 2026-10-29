#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime::NativeSerialize18Detail
{
U Count(B* begin,B* end,U width) noexcept
{const int delta=std::bit_cast<int>(Address(end)-Address(begin));return static_cast<U>(delta>>(width==4?2:width==2?1:0));}
namespace
{
U Maximum(U width) noexcept{return width==4?0x3FFFFFFFu:0x7FFFFFFFu;}
[[noreturn]] void TooLong(){throw std::length_error("vector<T> too long");}
U Grow(U capacity,U needed,U maximum) noexcept
{return capacity>maximum-(capacity>>1)?needed:(std::max)(needed,capacity+(capacity>>1));}
}
B* Allocate(U count,U width)
{
    const U bytes=count>Maximum(width)?0xFFFFFFFFu:count*width;auto& memory=RequireNativeSerialize18Services().memory;
    if(bytes<0x1000)return bytes?static_cast<B*>(memory.AllocateBody(bytes)):nullptr;
    const U extra=bytes+0x23;auto* raw=memory.AllocateBody(extra<=bytes?0xFFFFFFFFu:extra);
    if(!raw)_invalid_parameter_noinfo_noreturn();auto* aligned=reinterpret_cast<B*>((Address(raw)+0x23u)&0xFFFFFFE0u);
    WriteAt(aligned-4,0,raw);return aligned;
}
void Free(B* begin,U bytes)
{
    void* raw=begin;if(bytes>=0x1000){raw=ReadAt<void*>(begin-4);bytes+=0x23;if(Address(begin)-Address(raw)-4>0x1F)_invalid_parameter_noinfo_noreturn();}
    RequireNativeSerialize18Services().memory.DeleteVector(raw,bytes);
}
void Commit(Vector v,B* next,U count,U capacity,U width)
{
    if(auto* begin=v.Begin())Free(begin,(Address(v.Capacity())-Address(begin))&~(width-1u));
    v.Begin(next);v.End(Plus(next,count*width));v.Capacity(Plus(next,capacity*width));
}
B* InsertByte(Vector v,B* position,const B* value)
{
    const U offset=Address(position)-Address(v.Begin());const U size=Address(v.End())-Address(v.Begin());
    if(size==0x7FFFFFFF)TooLong();const U nextSize=size+1,capacity=Address(v.Capacity())-Address(v.Begin());
    const U nextCapacity=Grow(capacity,nextSize,0x7FFFFFFF);B* next=Allocate(nextCapacity,1);
    // 4539FA先写插入值，allocation回调后才读源；memmove和旧delete失败均没有new块回滚。
    *reinterpret_cast<volatile B*>(Plus(next,offset))=*reinterpret_cast<const volatile B*>(value);
    auto* currentEnd=v.End();
    if(position==currentEnd)std::memmove(next,v.Begin(),Address(currentEnd)-Address(v.Begin()));
    else
    {
        std::memmove(next,v.Begin(),Address(position)-Address(v.Begin()));
        std::memmove(Plus(next,offset+1),position,Address(v.End())-Address(position));
    }
    Commit(v,next,nextSize,nextCapacity,1);return Plus(v.Begin(),offset);
}
void AppendByte(Vector v,const B* value)
{
    auto* end=v.End();if(v.Capacity()==end){InsertByte(v,end,value);return;}
    *reinterpret_cast<volatile B*>(end)=*reinterpret_cast<const volatile B*>(value);
    v.End(Plus(v.End(),1)); // 原inc当前end，不缓存end+1在GetBuffer之前。
}
void ResizePrimitive(Vector v,U count,U width)
{
    const auto begin=v.Begin(),end=v.End();const U size=Count(begin,end,width),capacity=Count(begin,v.Capacity(),width);
    if(count>capacity)
    {
        const U maximum=Maximum(width);if(count>maximum)TooLong();const U nextCapacity=Grow(capacity,count,maximum);
        B* next=Allocate(nextCapacity,width);std::memset(Plus(next,size*width),0,(count-size)*width);
        std::memmove(next,v.Begin(),Address(v.End())-Address(v.Begin()));Commit(v,next,count,nextCapacity,width);return;
    }
    if(count>size){B* nextEnd=Plus(end,(count-size)*width);std::memset(end,0,(count-size)*width);v.End(nextEnd);}
    else if(count!=size)v.End(Plus(begin,count*width));
}
}
namespace KaGuYa::Start::Runtime
{
void ResizeNativeSerialize18ByteVectorAt4570D0(void* p,unsigned count,const void* unusedCtrl)
{static_cast<void>(unusedCtrl);NativeSerialize18Detail::ResizePrimitive({p},count,1);}
void DestroyNativeSerialize18PrimitiveVector(void* p,unsigned width)
{
    using namespace NativeSerialize18Detail;Vector v{p};if(auto* b=v.Begin()){Free(b,(Address(v.Capacity())-Address(b))&~(width-1u));v.Begin(nullptr);v.End(nullptr);v.Capacity(nullptr);}
}
}
