#pragma once
#include <Runtime/NativeSerialize18.h>
#include <Runtime/RuntimeCStringVector.h>
#include <bit>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::NativeSerialize18Detail
{
using U=std::uint32_t;using B=std::byte;
// 按原对象表示读写；兼容冻结Confirm的18h声明，不借同布局强转违反C++别名规则。
template<class T>T ReadAt(const void* p,U off=0) noexcept{T v;std::memcpy(&v,static_cast<const B*>(p)+off,sizeof(v));return v;}
template<class T>void WriteAt(void* p,U off,T value) noexcept{std::memcpy(static_cast<B*>(p)+off,&value,sizeof(value));}
inline U Address(const void* p) noexcept{return static_cast<U>(reinterpret_cast<std::uintptr_t>(p));}
inline B* Plus(void* p,U n) noexcept{return reinterpret_cast<B*>(Address(p)+n);}
inline const B* Plus(const void* p,U n) noexcept{return reinterpret_cast<const B*>(Address(p)+n);}
template<class F>F Slot(const void* p,U offset) noexcept{return ReadAt<F>(ReadAt<const void*>(p),offset);}
inline std::uint8_t Mode(void* p){return Slot<decltype(NativeISerializeVtable29::modeAt04)>(p,4)(p);}
inline void* Buffer(void* p){return Slot<decltype(NativeISerializeVtable29::bufferAt14)>(p,0x14)(p);}
inline std::uint32_t* Cursor(void* p){return Slot<decltype(NativeISerializeVtable29::cursorAddressAt6C)>(p,0x6C)(p);}
inline void* Argument(void* p,U off,void* arg){return Slot<NativeSerialize18Argument>(p,off)(p,arg);}
struct Vector final
{
    void* p;
    B* Begin() const noexcept{return ReadAt<B*>(p);}
    B* End() const noexcept{return ReadAt<B*>(p,4);}
    B* Capacity() const noexcept{return ReadAt<B*>(p,8);}
    void Begin(B* v) const noexcept{WriteAt(p,0,v);}
    void End(B* v) const noexcept{WriteAt(p,4,v);}
    void Capacity(B* v) const noexcept{WriteAt(p,8,v);}
};
U Count(B* begin,B* end,U width) noexcept;
B* Allocate(U count,U width);
void Free(B* begin,U bytes);
void Commit(Vector,B* next,U count,U capacity,U width);
void ResizePrimitive(Vector,U count,U width);
B* InsertByte(Vector,B* position,const B* value);
void AppendByte(Vector,const B* value);
void* Scalars(void* self,void* target,U width,U cursorPolicy);
std::byte* __fastcall Reset(void*,void*);
std::uint8_t __fastcall GetMode(void*,void*);
std::uint8_t* __fastcall ModeAddress(void*,void*);
std::uint8_t __fastcall SetMode(void*,void*,std::uint8_t);
std::uint8_t __fastcall WriteMode(void*,void*,std::uint8_t);
NativeSerialize18ByteVector* __fastcall GetBuffer(void*,void*);
std::uint32_t* __fastcall CursorAddress(void*,void*);
void* __fastcall State(void*,void*,void*);void* __fastcall Copy(void*,void*,void*);
void* __fastcall Int(void*,void*,void*);void* __fastcall Long(void*,void*,void*);
void* __fastcall Word(void*,void*,void*);void* __fastcall Dword(void*,void*,void*);
void* __fastcall Short(void*,void*,void*);void* __fastcall String(void*,void*,void*);
void* __fastcall Byte(void*,void*,void*);void* __fastcall Bool(void*,void*,void*);void* __fastcall Unsigned(void*,void*,void*);
void* __fastcall IntVector(void*,void*,void*);void* __fastcall LongVector(void*,void*,void*);
void* __fastcall WordVector(void*,void*,void*);void* __fastcall UnsignedVector(void*,void*,void*);
void* __fastcall ByteVector(void*,void*,void*);void* __fastcall StringVector(void*,void*,void*);void* __fastcall BitVector(void*,void*,void*);
void* __fastcall Archive(void*,void*,void*);
int __fastcall SaveFile(void*,void*,const char*);int __fastcall LoadFile(void*,void*,const char*);
int __fastcall ReadStream(void*,void*,IFileStreamRuntime*,U);int __fastcall ImportReference(void*,void*,const YtlReferenceSlotRuntime*);
void* __fastcall Delete(void*,void*,U);void* __fastcall DeleteBase(void*,void*,U);
}
