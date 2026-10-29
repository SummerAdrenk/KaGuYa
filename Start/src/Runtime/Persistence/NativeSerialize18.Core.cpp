#include "NativeSerialize18.Internal.h"
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace{thread_local NativeSerialize18Services* active=nullptr;}
IStartFileApi& DefaultNativeSerialize18Files() noexcept{static Win32StartFileApi api;return api;}
NativeSerialize18Scope::NativeSerialize18Scope(NativeSerialize18Services& s) noexcept:previous_(active){active=&s;}
NativeSerialize18Scope::~NativeSerialize18Scope() noexcept{active=previous_;}
NativeSerialize18Services& RequireNativeSerialize18Services()
{if(!active)throw std::logic_error("NativeSerialize18 services are not bound");return *active;}
NativeSerialize18Body* ConstructNativeSerialize18(void* p) noexcept
{
    using namespace NativeSerialize18Detail;WriteAt(p,0,&NativeSerialize18Methods());
    WriteAt<B*>(p,4,nullptr);WriteAt<B*>(p,0xC,nullptr);WriteAt<B*>(p,8,nullptr);
    WriteAt<U>(p,0x14,0);WriteAt<std::uint8_t>(p,0x10,1);return static_cast<NativeSerialize18Body*>(p);
}
void DestroyNativeSerialize18At463E60(void* p)
{
    using namespace NativeSerialize18Detail;WriteAt(p,0,&NativeSerialize18Methods());Vector v{Plus(p,4)};
    if(auto* begin=v.Begin()){Free(begin,Address(v.Capacity())-Address(begin));v.Begin(nullptr);v.End(nullptr);v.Capacity(nullptr);}
    WriteAt(p,0,&NativeISerializeMethods()); // 原normal/EH均只退表，不清mode/cursor/padding。
}
NativeSerialize18Body* ConstructNativeSerialize18InlineDword(void* p) noexcept
{
    using namespace NativeSerialize18Detail;
    WriteAt(p,0,&NativeSerialize18Methods());
    WriteAt<B*>(p,4,nullptr);WriteAt<B*>(p,0xC,nullptr);WriteAt<B*>(p,8,nullptr);
    WriteAt<U>(p,0x14,0);WriteAt<U>(p,0x10,1);
    return static_cast<NativeSerialize18Body*>(p);
}
namespace NativeSerialize18Detail
{
std::byte* __fastcall Reset(void* p,void*)
{auto* begin=ReadAt<B*>(p,4);WriteAt(p,8,begin);WriteAt<U>(p,0x14,0);WriteAt<std::uint8_t>(p,0x10,1);return begin;}
std::uint8_t __fastcall GetMode(void* p,void*){return ReadAt<std::uint8_t>(p,0x10);}
std::uint8_t* __fastcall ModeAddress(void* p,void*){return reinterpret_cast<std::uint8_t*>(Plus(p,0x10));}
std::uint8_t __fastcall SetMode(void* p,void*,std::uint8_t mode)
{
    WriteAt(p,0x10,mode);WriteAt<U>(p,0x14,0);if(!mode)return mode;
    void* v=Buffer(p);Vector{v}.End(Vector{v}.Begin());return static_cast<std::uint8_t>(Address(v)); // 原AL=buffer地址低字节。
}
std::uint8_t __fastcall WriteMode(void* p,void*,std::uint8_t mode){WriteAt(p,0x10,mode);return mode;}
NativeSerialize18ByteVector* __fastcall GetBuffer(void* p,void*){return reinterpret_cast<NativeSerialize18ByteVector*>(Plus(p,4));}
U* __fastcall CursorAddress(void* p,void*){return reinterpret_cast<U*>(Plus(p,0x14));}
void* __fastcall Delete(void* p,void*,U flags)
{DestroyNativeSerialize18At463E60(p);if(flags&1)RequireNativeSerialize18Services().memory.DeleteBody(p,0x18);return p;}
void* __fastcall DeleteBase(void* p,void*,U flags)
{WriteAt(p,0,&NativeISerializeMethods());if(flags&1)RequireNativeSerialize18Services().memory.DeleteBody(p,4);return p;}
[[noreturn]] void Pure(){_purecall();std::terminate();}
}
const NativeSerialize18Vtable31& NativeSerialize18Methods() noexcept
{
    using namespace NativeSerialize18Detail;static const auto table=[]
    {
        NativeSerialize18Vtable31 t{};auto& b=t.common;
#define S(field,fn) b.field=reinterpret_cast<decltype(b.field)>(&fn)
        S(resetAt00,Reset);S(modeAt04,GetMode);S(modeAddressAt08,ModeAddress);S(setModeAt0C,SetMode);S(writeModeOnlyAt10,WriteMode);S(bufferAt14,GetBuffer);
        S(stateAt18,State);S(intVectorAt1C,IntVector);S(longVectorAt20,LongVector);S(wordVectorAt24,WordVector);S(stringVectorAt28,StringVector);
        S(byteVectorAt2C,ByteVector);S(bitVectorAt30,BitVector);S(unsignedVectorAt34,UnsignedVector);
        S(intAt38,Int);S(longAt3C,Long);S(wordAt40,Word);S(dwordAt44,Dword);S(shortAt48,Short);S(stringAt4C,String);
        S(byteAt50,Byte);S(boolAt54,Bool);S(unsignedAt58,Unsigned);S(archiveAt5C,Archive);S(copyAt60,Copy);
        S(saveFileAt64,SaveFile);S(loadFileAt68,LoadFile);S(cursorAddressAt6C,CursorAddress);S(deletingDestructorAt70,Delete);
#undef S
        t.readStreamAt74=reinterpret_cast<decltype(t.readStreamAt74)>(&ReadStream);
        t.importByteReferenceAt78=reinterpret_cast<decltype(t.importByteReferenceAt78)>(&ImportReference);return t;
    }();return table;
}
const NativeISerializeVtable29& NativeISerializeMethods() noexcept
{
    static const auto table=[]
    {
        NativeISerializeVtable29 t;for(unsigned i=0;i<28;++i)
        {const auto fn=&NativeSerialize18Detail::Pure;std::memcpy(reinterpret_cast<std::byte*>(&t)+i*4,&fn,4);}
        t.deletingDestructorAt70=reinterpret_cast<decltype(t.deletingDestructorAt70)>(&NativeSerialize18Detail::DeleteBase);return t;
    }();return table;
}
}
