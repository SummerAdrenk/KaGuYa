#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime::NativeSerialize18Detail
{
namespace
{
void* Sequence(void* p,void* a,U width,U slot)
{
    U count;const auto writing=Mode(p);if(writing){Vector v{a};count=Count(v.Begin(),v.End(),width);}
    Argument(p,0x58,&count);if(!writing)ResizePrimitive({a},count,width);
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)
        Argument(p,slot,Plus(Vector{a}.Begin(),i*width));
    return p;
}
}
void* __fastcall IntVector(void* p,void*,void* a){return Sequence(p,a,4,0x38);}
void* __fastcall LongVector(void* p,void*,void* a){return Sequence(p,a,4,0x3C);}
void* __fastcall WordVector(void* p,void*,void* a){return Sequence(p,a,2,0x40);}
void* __fastcall UnsignedVector(void* p,void*,void* a){return Sequence(p,a,4,0x58);}
void* __fastcall ByteVector(void* p,void*,void* a){return Sequence(p,a,1,0x50);}
void* __fastcall StringVector(void* p,void*,void* a)
{
    U count;const auto writing=Mode(p);if(writing){Vector v{a};count=Count(v.Begin(),v.End(),4);}
    Argument(p,0x58,&count);
    if(!writing)
    {
        auto& s=RequireNativeSerialize18Services();
        ResizeRuntimeCStringVectorAt456E80(*static_cast<EnvironmentVectorStorage<RuntimeCString>*>(a),count,s.strings,s.memory);
    }
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)Argument(p,0x4C,Plus(Vector{a}.Begin(),i*4));return p;
}
void* __fastcall BitVector(void* p,void*,void* a)
{
    U count;if(Mode(p))count=ReadAt<U>(a,0xC);else
    {
        Argument(p,0x58,&count);ResizeRuntimeBitVectorAt494BD0(*static_cast<RuntimeBitVectorStorage*>(a),count,false,RequireNativeSerialize18Services().memory);
        goto loop;
    }
    Argument(p,0x58,&count);
loop:
    for(U i=0;std::bit_cast<int>(i)<std::bit_cast<int>(count);++i)
    {
        const U offset=(i>>5)*4,mask=1u<<(i&31);std::uint8_t value;
        if(Mode(p)){value=(ReadAt<U>(ReadAt<void*>(a),offset)&mask)!=0;Argument(p,0x54,&value);}
        else
        {
            Argument(p,0x54,&value);void* words=ReadAt<void*>(a);const U old=ReadAt<U>(words,offset);
            WriteAt(words,offset,value?old|mask:old&~mask); // 任意非零byte为true，写端仍输出0/1。
        }
    }
    return p;
}
void* __fastcall State(void* p,void*,void* source)
{
    // 465E8F先缓存目标table指针，但在source getter之后才读该旧table的函数项。
    auto* targetTable=ReadAt<const void*>(p);auto* bytes=Slot<decltype(NativeISerializeVtable29::bufferAt14)>(source,0x14)(source);
    ReadAt<NativeSerialize18Argument>(targetTable,0x2C)(p,bytes);
    targetTable=ReadAt<const void*>(p);auto* mode=Slot<decltype(NativeISerializeVtable29::modeAddressAt08)>(source,8)(source);
    ReadAt<NativeSerialize18Argument>(targetTable,0x54)(p,mode);
    targetTable=ReadAt<const void*>(p);auto* cursor=Cursor(source);ReadAt<NativeSerialize18Argument>(targetTable,0x58)(p,cursor);return p;
}
void* __fastcall Copy(void* p,void*,void* source)
{
    Vector destination{Plus(p,4)},input{Buffer(source)};
    if(destination.p!=input.p)
    {
        auto* end=input.End();auto* begin=input.Begin();const U size=Address(end)-Address(begin);
        auto* old=destination.Begin();const U capacity=Address(destination.Capacity())-Address(old),oldSize=Address(destination.End())-Address(old);
        if(size>capacity)
        {
            if(size>0x7FFFFFFF)throw std::length_error("vector<T> too long");
            const U nextCapacity=capacity>0x7FFFFFFF-(capacity>>1)?size:(std::max)(size,capacity+(capacity>>1));
            if(old)Free(old,capacity);destination.Begin(nullptr);destination.End(nullptr);destination.Capacity(nullptr);
            auto* next=Allocate(nextCapacity,1);destination.Begin(next);destination.End(next);destination.Capacity(Plus(next,nextCapacity));
            std::memmove(next,begin,size);destination.End(Plus(next,size)); // 4659F0破坏性赋值：先delete旧块后分配。
        }
        else if(size<=oldSize){std::memmove(old,begin,size);destination.End(Plus(old,size));}
        else
        {std::memmove(old,begin,oldSize);auto* tail=destination.End();std::memmove(tail,Plus(begin,oldSize),Address(end)-Address(Plus(begin,oldSize)));destination.End(Plus(tail,size-oldSize));}
    }
    auto* mode=Slot<decltype(NativeISerializeVtable29::modeAddressAt08)>(source,8)(source);WriteAt(p,0x10,ReadAt<std::uint8_t>(mode));
    auto* cursor=Cursor(source);WriteAt(p,0x14,ReadAt<U>(cursor));return p;
}
void* __fastcall Archive(void* p,void*,void* archive)
{
    using Fn=void(__thiscall*)(void*,void*);Slot<Fn>(archive,0)(archive,p);return p;
}
}
namespace KaGuYa::Start::Runtime
{void* SerializeNativeArchiveAt466380(void* p,void* a){return NativeSerialize18Detail::Archive(p,nullptr,a);}}
