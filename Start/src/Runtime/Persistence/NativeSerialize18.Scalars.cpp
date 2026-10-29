#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime::NativeSerialize18Detail
{
void* Scalars(void* self,void* target,U width,U policy)
{
    const auto writing=Mode(self);
    if(writing){for(U i=0;i<width;++i)AppendByte({Buffer(self)},Plus(target,i));return self;}
    for(U i=0;i<width;++i)
    {
        U old;
        if(policy==0)old=ReadAt<U>(self,0x14);
        else if(policy==1){auto* cursor=Cursor(self);old=ReadAt<U>(cursor);WriteAt(cursor,0,old+1);}
        else{old=ReadAt<U>(self,0x14);WriteAt(self,0x14,old+1);}
        auto* bytes=Vector{Buffer(self)}.Begin();
        const B value=*reinterpret_cast<const volatile B*>(Plus(bytes,old));*reinterpret_cast<volatile B*>(Plus(target,i))=value;
        if(policy==0)WriteAt(self,0x14,ReadAt<U>(self,0x14)+1);
    }
    // 原无end检查：不补0、不夹取、不造现代out_of_range；逐byte前缀及cursor副作用可观察。
    return self;
}
void* __fastcall Int(void* p,void*,void* a){return Scalars(p,a,4,1);}
void* __fastcall Long(void* p,void*,void* a){return Scalars(p,a,4,1);}
void* __fastcall Word(void* p,void*,void* a){return Scalars(p,a,2,1);}
void* __fastcall Dword(void* p,void*,void* a){return Scalars(p,a,4,1);}
void* __fastcall Short(void* p,void*,void* a){return Scalars(p,a,2,1);}
void* __fastcall Byte(void* p,void*,void* a){return Scalars(p,a,1,0);}
void* __fastcall Bool(void* p,void*,void* a){return Scalars(p,a,1,2);}
void* __fastcall Unsigned(void* p,void*,void* a){return Scalars(p,a,4,2);}
void* __fastcall String(void* p,void*,void* a)
{
    if(Mode(p))
    {
        const char* held=ReadAt<const char*>(a);const int length=std::bit_cast<int>(ReadAt<U>(held-12)+1u);
        for(int i=0;i<length;++i)AppendByte({Buffer(p)},reinterpret_cast<const B*>(held)+i);return p;
    }
    const U old=ReadAt<U>(p,0x14);auto* first=Plus(Vector{Buffer(p)}.Begin(),old);
    const U length=static_cast<U>(std::strlen(reinterpret_cast<const char*>(first)));
    // 两次GetBuffer/strlen分开；437680返回后加第一次长度到当前cursor，不发布提前快照。
    const U current=ReadAt<U>(p,0x14);auto* second=Plus(Vector{Buffer(p)}.Begin(),current);
    const int currentLength=second?static_cast<int>(std::strlen(reinterpret_cast<const char*>(second))):0;
    SetRuntimeCStringAt437680(*static_cast<RuntimeCString*>(a),reinterpret_cast<const char*>(second),currentLength);
    WriteAt(p,0x14,ReadAt<U>(p,0x14)+length+1);return p;
}
}
