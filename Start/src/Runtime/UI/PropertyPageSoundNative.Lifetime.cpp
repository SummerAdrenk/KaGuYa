#include "PropertyPageSoundNative.Internal.h"
#include <new>
#include <cstdlib>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace PropertySoundDetail;
BOOL __fastcall Process(DialogEndpointNativeBody20h* p,void*,HWND w,UINT m,WPARAM a,LPARAM b,LRESULT* r,DWORD map)
{return ProcessPropertyPageSoundNativeAt42FF30(*reinterpret_cast<Page*>(p),w,m,a,b,r,map,RequirePropertyPageSoundNativeServices());}
DialogEndpointNativeBody20h* __fastcall Delete(DialogEndpointNativeBody20h* p,void*,DWORD flags)
{
    auto& s=RequirePropertyPageSoundNativeServices();DestroyPropertyPageSoundNativeAt40EB10(*reinterpret_cast<Page*>(p),s);
    if(flags&1)s.memory.DeleteBody(p,0xA8);return p;
}
INT_PTR CALLBACK BoundPage(HWND first,UINT m,WPARAM w,LPARAM l)
{auto& s=RequirePropertyPageSoundNativeServices();return DispatchDialogEndpointNativeAt415810(*reinterpret_cast<DialogEndpointNativeBody20h*>(first),m,w,l,s.endpoints.api);}
DLGPROC __fastcall Procedure(DialogEndpointNativeBody20h*,void*) {return &BoundPage;}
void __fastcall Final(DialogEndpointNativeBody20h*,void*,HWND) {} // 原424DB0确为retn 4。
BOOL __fastcall ListProcess(EditWindowEndpointNativeBody24h* p,void*,HWND w,UINT m,WPARAM a,LPARAM b,LRESULT* r,DWORD map)
{return ProcessPropertyPageSoundCheckListNativeAt42DAB0(*p,w,m,a,b,r,map,RequirePropertyPageSoundNativeServices().api);}
EditWindowEndpointNativeBody24h* __fastcall ListDelete(EditWindowEndpointNativeBody24h* p,void*,std::uint32_t flags)
{
    auto& s=RequirePropertyPageSoundNativeServices();if(auto* t=Read(p->thunkAt14))FreeWindowThunkNativeAt53F190(t,s.endpoints.thunks);
    if(flags&1)s.memory.DeleteBody(p,0x24);return p;
}
LRESULT CALLBACK BoundList(HWND first,UINT m,WPARAM w,LPARAM l)
{return DispatchEditWindowNativeMessageAt439F60(*reinterpret_cast<EditWindowEndpointNativeBody24h*>(first),m,w,l,RequirePropertyPageSoundNativeServices().api);}
WNDPROC __fastcall ListProcedure(EditWindowEndpointNativeBody24h*,void*) {return &BoundList;}
void __fastcall ListFinal(EditWindowEndpointNativeBody24h*,void*,HWND) {} // 原424DD0确为retn 4。
// x86自由函数用ECX/EDX shim实现原thiscall表，dummy EDX不占原栈实参。
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const DialogEndpointNativeVtable4 table{
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::processAt00)>(&Process),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::procedureAt08)>(&Procedure),
    reinterpret_cast<decltype(DialogEndpointNativeVtable4::finalMessageAt0C)>(&Final)};
const EditWindowEndpointNativeVtable4 listTable{
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::processAt00)>(&ListProcess),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::deleteAt04)>(&ListDelete),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::procedureAt08)>(&ListProcedure),
    reinterpret_cast<decltype(EditWindowEndpointNativeVtable4::finalMessageAt0C)>(&ListFinal)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
const DialogEndpointNativeVtable4& PropertyPageSoundNativeMethods() noexcept {return table;}
const EditWindowEndpointNativeVtable4& PropertyPageSoundCheckListNativeMethods() noexcept {return listTable;}
UINT CALLBACK PropertyPageSoundCreationNativeAt430E60(HWND w,UINT m,PropertyPageNativeHeader34h* p)
{return PropertyPageCreationNativeAt430F20(w,m,p);}
void InitializePropertyPageSoundNativeAt40BADD(Page& p,WindowClassModuleNativeBody38h& module) noexcept
{
    InitializePropertyPageNativeAt40B91C(p.pageAt00,table,0xD4,&PropertyPageSoundCreationNativeAt430E60,module);
    p.voicesAt58={nullptr,nullptr,nullptr};p.volumesAt64={nullptr,nullptr,nullptr};
    p.listAt78.windowAt04=nullptr;p.listAt78.thunkAt14=nullptr;p.listAt78.currentMessageAt18=nullptr;
    p.listAt78.destroyPendingAt1C=0;p.listAt78.previousAt20=&::DefWindowProcA;p.listAt78.vptrAt00=&listTable;
    p.treeAt9C=nullptr;
}
void DestroyPropertyPageSoundNativeAt40EB10(Page& p,Services& s) noexcept
{
    PropertyPageSoundNativeScope scope(s);
    if(auto* t=Read(p.listAt78.thunkAt14))FreeWindowThunkNativeAt53F190(t,s.endpoints.thunks);
    if(auto* b=Read(p.volumesAt64.begin))
    {
        FreeVector(b,Delta(Read(p.volumesAt64.capacity),b)&~7u,s);p.volumesAt64={nullptr,nullptr,nullptr};
    }
    if(auto* b=Read(p.voicesAt58.begin))
    {
        ReleaseVoiceRange(b,Read(p.voicesAt58.end));b=Read(p.voicesAt58.begin);
        FreeVector(b,12*Count12(Read(p.voicesAt58.capacity),b),s);p.voicesAt58={nullptr,nullptr,nullptr};
    }
    if(auto* t=Read(p.pageAt00.endpointAt00.thunkAt14))FreeWindowThunkNativeAt53F190(t,s.endpoints.thunks);
}
}

namespace KaGuYa::Start::Runtime::PropertySoundDetail
{
namespace
{
// 5AEB1C仅两个槽；18h控制块不夹带allocator/owner，退役期显式绑定外部服务。
class RetiredVoiceControl : public YtlReferenceControlRuntime
{
public:
    RetiredVoiceControl() noexcept :YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override {::_purecall();} // 原ref_object基表slot00=541C5F纯虚叶。
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {::new(this) RetiredVoiceControl;if(flags&1)RequirePropertyPageSoundNativeServices().memory.DeleteBody(this,0x14);}
    DWORD strideAt0C, countAt10;
};
static_assert(sizeof(RetiredVoiceControl)==0x14);
class VoiceControl final : public RetiredVoiceControl
{
public:
    VoiceControl(PropertyPageSoundVoiceParam10h* p) noexcept :bodyAt14(p) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p=Read(bodyAt14);if(!p)return;
        DestroyRuntimeCStringAt40CD40(&p->keyAt08);DestroyRuntimeCStringAt40CD40(&p->masterAt04);
        RequirePropertyPageSoundNativeServices().memory.DeleteBody(p,0x10);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 411AAA先退回ref_object基表，即使flags不删控制块也必须发生。
        void* raw=this;::new(raw) RetiredVoiceControl;
        if(flags&1)RequirePropertyPageSoundNativeServices().memory.DeleteBody(raw,0x18);
    }
    PropertyPageSoundVoiceParam10h* bodyAt14;
};
static_assert(sizeof(VoiceControl)==0x18);
void* AllocateVector(DWORD count,DWORD width,Services& s)
{
    const DWORD bytes=count*width;if(bytes<0x1000)return bytes?s.memory.AllocateBody(bytes):nullptr;
    const DWORD extra=bytes+0x23;auto* raw=s.memory.AllocateBody(extra<bytes?0xFFFFFFFFu:extra);
    if(!raw)_invalid_parameter_noinfo_noreturn();auto* aligned=reinterpret_cast<void*>((Address(raw)+0x23)&~31u);
    reinterpret_cast<void**>(aligned)[-1]=raw;return aligned;
}
DWORD Growth(DWORD capacity,DWORD need,DWORD maximum) noexcept
{return capacity>maximum-(capacity>>1)?need:(std::max)(need,capacity+(capacity>>1));}
void CopyRef(YtlReferenceSlotRuntime& d,const YtlReferenceSlotRuntime& r)
{d.baseAt00=Read(r.baseAt00);d.biasAt04=Read(r.biasAt04);d.controlAt08=Read(r.controlAt08);if(d.controlAt08)++d.controlAt08->referenceCountAt04;}
}
void FreeVector(void* b,DWORD bytes,Services& s)
{
    void* raw=b;if(bytes>=0x1000)
    {raw=At<void*>(b,0xFFFFFFFCu);bytes+=0x23;if(Address(b)-Address(raw)-4>31)_invalid_parameter_noinfo_noreturn();}
    s.memory.DeleteVector(raw,bytes);
}
void ReleaseVoiceRange(YtlReferenceSlotRuntime* b,YtlReferenceSlotRuntime* e) noexcept
{
    // 4083E0逐项正序；payload回调后重读control。无control时不清base/bias。
    for(;b!=e;++b)if(auto* c=Read(b->controlAt08))
    {
        if(--c->referenceCountAt04==0)
        {if(Read(c->owningAt08))c->DestroyPayloadAtVslot00();if(auto* next=Read(b->controlAt08))next->DeleteControlAtVslot04(1);}
        b->controlAt08=nullptr;b->baseAt00=0;
    }
}
void ReleaseLocal(YtlReferenceSlotRuntime& r) noexcept
{
    // 40DC20/43BA50同样回读槽，不用std::shared_ptr或缓存payload替换。
    ReleaseVoiceRange(&r,&r+1);
}
void MakeVoice(YtlReferenceSlotRuntime& r,Services& s)
{
    auto* raw=s.memory.AllocateBody(0x10);auto* p=raw?::new(raw) PropertyPageSoundVoiceParam10h:nullptr;
    if(p){InitializeEmptyRuntimeCString(p->masterAt04,s.strings);InitializeEmptyRuntimeCString(p->keyAt08,s.strings);}
    // 原40CB30第二GetNil失败没有body/首串的回收资格。
    r.baseAt00=0;r.biasAt04=0;r.controlAt08=nullptr;if(!p)return;
    r.baseAt00=Address(p);auto* c=::new(s.memory.AllocateBody(0x18)) VoiceControl(p);r.controlAt08=c;
    c->referenceCountAt04=1;c->owningAt08=1;c->strideAt0C=0x10;c->countAt10=1;
}
void AppendPair(Page& p,PropertyPageSoundVolumePair8h value,Services& s)
{
    auto& v=p.volumesAt64;auto* e=Read(v.end);if(e!=Read(v.capacity)){*e=value;v.end=Read(v.end)+1;return;}
    const auto n=Count8(e,Read(v.begin)),cap=Count8(Read(v.capacity),Read(v.begin));
    if(n==0x1FFFFFFF)throw std::length_error("vector<T> too long");const auto nextCap=Growth(cap,n+1,0x1FFFFFFF);
    auto* next=static_cast<PropertyPageSoundVolumePair8h*>(AllocateVector(nextCap,8,s));next[n]=value;
    auto* old=Read(v.begin);auto* oldEnd=Read(v.end);for(auto* out=next;old!=oldEnd;++old,++out)*out=*old;
    if(auto* b=Read(v.begin))FreeVector(b,Delta(Read(v.capacity),b)&~7u,s);
    v.begin=next;v.end=next+n+1;v.capacity=next+nextCap;
}
void AppendVoice(Page& p,const YtlReferenceSlotRuntime& value,Services& s)
{
    auto& v=p.voicesAt58;auto* e=Read(v.end);if(e!=Read(v.capacity)){CopyRef(*e,value);v.end=Read(v.end)+1;return;}
    const auto n=Count12(e,Read(v.begin)),cap=Count12(Read(v.capacity),Read(v.begin));
    if(n==0x15555555)throw std::length_error("vector<T> too long");const auto nextCap=Growth(cap,n+1,0x15555555);
    auto* next=static_cast<YtlReferenceSlotRuntime*>(AllocateVector(nextCap,12,s));CopyRef(next[n],value);
    auto* old=Read(v.begin);auto* oldEnd=Read(v.end);for(auto* out=next;old!=oldEnd;++old,++out)CopyRef(*out,*old);
    if(auto* b=Read(v.begin))
    {ReleaseVoiceRange(b,Read(v.end));b=Read(v.begin);FreeVector(b,12*Count12(Read(v.capacity),b),s);}
    v.begin=next;v.end=next+n+1;v.capacity=next+nextCap;
}
}
