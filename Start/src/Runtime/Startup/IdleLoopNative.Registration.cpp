#include <Runtime/IdleLoopNative.h>
#include <atlbase.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void Append(IdleLoopNativeArray0Ch& array,void* callback,const void* marker,IWindowClassNativeApi& api)
{
    auto count=Read(array.countAt04);const auto capacity=Read(array.capacityAt08);
    if(count==capacity)
    {
        auto* base=Read(array.valuesAt00);
        const auto address=reinterpret_cast<DWORD>(marker),begin=reinterpret_cast<DWORD>(base);
        if(address>=begin&&address<begin+capacity*4u)throw ATL::CAtlException(E_FAIL);
        const auto nextCapacity=capacity?count*2u:1u;
        if(std::bit_cast<LONG>(nextCapacity)<0||nextCapacity>0x1FFFFFFFu)return;
        auto* next=static_cast<void**>(api.Recalloc(base,nextCapacity,4));if(!next)return;
        count=Read(array.countAt04);array.capacityAt08=nextCapacity;array.valuesAt00=next;
    }
    auto* target=reinterpret_cast<void**>(reinterpret_cast<DWORD>(Read(array.valuesAt00))+count*4u);
    if(target){*target=callback;count=Read(array.countAt04);}
    array.countAt04=count+1u;
}
void Remove(IdleLoopNativeArray0Ch& array,void* target,IAtlBaseModuleNativeDiagnostics& diagnostics)
{
    auto count=Read(array.countAt04);if(std::bit_cast<LONG>(count)<=0)return;
    auto* base=Read(array.valuesAt00);DWORD index=0;
    for(;std::bit_cast<LONG>(index)<std::bit_cast<LONG>(count);++index)
    {
        auto* current=reinterpret_cast<void* const*>(reinterpret_cast<DWORD>(base)+index*4u);
        if(Read(*current)==target)break;
    }
    if(index==0xFFFFFFFFu||std::bit_cast<LONG>(index)<0||std::bit_cast<LONG>(index)>=std::bit_cast<LONG>(count))return;
    if(index!=count-1u)
    {
        const auto destination=reinterpret_cast<DWORD>(base)+index*4u;
        const auto bytes=(count-index)*4u;
        diagnostics.MoveBytesAt43D2D0(reinterpret_cast<void*>(destination),bytes,
            reinterpret_cast<const void*>(destination+4u),bytes-4u);
        count=Read(array.countAt04); // 原43D2D0正常返回后才重读，不清尾槽或缩capacity。
    }
    array.countAt04=count-1u;
}
}
void RegisterIdleLoopFrameNativeAt423990(void* frame,AppModuleNativeBody3Ch& module,
    AppModuleNativeBody3Ch*& publication,AppModuleMessageLoopsNativeServices& services)
{
    const auto thread=services.module.platform.GetCurrentThreadId();
    auto* loop=static_cast<IdleLoopNativeBody38h*>(FindAppModuleMessageLoopNativeAt417F50(module,publication,thread,services));
    void* filter=frame?reinterpret_cast<void*>(reinterpret_cast<DWORD>(frame)+0x34u):nullptr;
    Append(loop->filtersAt04,filter,&filter,services.allocation);
    void* idle=frame?reinterpret_cast<void*>(reinterpret_cast<DWORD>(frame)+0x38u):nullptr;
    Append(loop->idlesAt10,idle,&idle,services.allocation);
    *reinterpret_cast<volatile BYTE*>(reinterpret_cast<DWORD>(frame)+0x8Cu)=0;
}
void RemoveIdleLoopFrameNativeAt42E8D5(void* frame,IdleLoopNativeBody38h& loop,IAtlBaseModuleNativeDiagnostics& diagnostics)
{
    auto* filter=frame?reinterpret_cast<void*>(reinterpret_cast<DWORD>(frame)+0x34u):nullptr;
    Remove(loop.filtersAt04,filter,diagnostics);
    auto* idle=frame?reinterpret_cast<void*>(reinterpret_cast<DWORD>(frame)+0x38u):nullptr;
    Remove(loop.idlesAt10,idle,diagnostics);
}
}
