#include <Runtime/AtlBaseModuleNative.h>
#include <atlbase.h>
#include <bit>
#include <intrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
std::int32_t Signed(std::uint32_t value)noexcept{return std::bit_cast<std::int32_t>(value);}
}
HINSTANCE* AtlBaseResourceSlotNativeAt53E968(WindowClassModuleNativeBody38h& body,std::int32_t index,
    AtlBaseModuleNativeServices& services)
{
    if(index<0||index>=Signed(Read(body.resourceCountAt30)))
    {services.windows.RaiseException(EXCEPTION_ARRAY_BOUNDS_EXCEEDED,1,0,nullptr);__debugbreak();}
    return reinterpret_cast<HINSTANCE*>(reinterpret_cast<std::uintptr_t>(Read(body.resourceInstancesAt2C))+
        static_cast<std::uint32_t>(index)*4u);
}
HINSTANCE GetAtlBaseResourceInstanceNativeAt53EA42(WindowClassModuleNativeBody38h& body,std::int32_t index,
    AtlBaseModuleNativeServices& services)
{
    auto* lock=&body.resourceLockAt14;services.windows.EnterCriticalSection(lock);
    HINSTANCE result=nullptr;
    if(Read(index)<=Signed(Read(body.resourceCountAt30))&&Read(index)>=0)
    {
        if(Read(index)==Signed(Read(body.resourceCountAt30)))result=Read(body.resourceInstanceAt08);
        else result=Read(*AtlBaseResourceSlotNativeAt53E968(body,Read(index),services));
    }
    services.windows.LeaveCriticalSection(lock);
    return result;
}
bool AppendAtlBaseResourceArrayNativeAt53E98D(WindowClassModuleNativeBody38h& body,const HINSTANCE* source,
    AtlBaseModuleNativeServices& services)
{
    auto count=Read(body.resourceCountAt30);auto capacity=Read(body.resourceArrayWordAt34);
    if(count==capacity)
    {
        auto* data=Read(body.resourceInstancesAt2C);
        const auto base=reinterpret_cast<std::uintptr_t>(data),address=reinterpret_cast<std::uintptr_t>(source);
        if(address>=base&&address<base+capacity*4u)throw ATL::CAtlException(E_FAIL);
        const auto next=capacity?count+count:1u;
        if(capacity&&(Signed(next)<0||next>0x1FFFFFFFu))return false;
        auto* grown=static_cast<HINSTANCE*>(services.windows.Recalloc(data,next,4));
        if(!grown)return false;
        count=Read(body.resourceCountAt30);body.resourceArrayWordAt34=next;body.resourceInstancesAt2C=grown;
    }
    const auto target=reinterpret_cast<std::uintptr_t>(Read(body.resourceInstancesAt2C))+count*4u;
    if(target)
    {
        *reinterpret_cast<HINSTANCE*>(target)=Read(*source);
        count=Read(body.resourceCountAt30);
    }
    body.resourceCountAt30=count+1u;
    return true;
}
bool AddAtlBaseResourceInstanceNativeAt53EA0F(WindowClassModuleNativeBody38h& body,HINSTANCE value,
    AtlBaseModuleNativeServices& services)
{
    auto* lock=&body.resourceLockAt14;services.windows.EnterCriticalSection(lock);
    const auto result=AppendAtlBaseResourceArrayNativeAt53E98D(body,&value,services);
    services.windows.LeaveCriticalSection(lock);return result;
}
bool RemoveAtlBaseResourceArrayNativeAt53EB0A(WindowClassModuleNativeBody38h& body,std::int32_t index,
    AtlBaseModuleNativeServices& services)
{
    if(index<0)return false;
    auto count=Read(body.resourceCountAt30);
    if(index>=Signed(count))return false;
    if(static_cast<std::uint32_t>(index)!=count-1u)
    {
        const auto bytes=(count-static_cast<std::uint32_t>(index))*4u;
        const auto destination=reinterpret_cast<std::uintptr_t>(Read(body.resourceInstancesAt2C))+
            static_cast<std::uint32_t>(index)*4u;
        services.diagnostics.MoveBytesAt43D2D0(reinterpret_cast<void*>(destination),bytes,
            reinterpret_cast<const void*>(destination+4u),bytes-4u);
        count=Read(body.resourceCountAt30);
    }
    body.resourceCountAt30=count-1u;return true;
}
bool RemoveAtlBaseResourceInstanceNativeAt53EB58(WindowClassModuleNativeBody38h& body,HINSTANCE value,
    AtlBaseModuleNativeServices& services)
{
    auto* lock=&body.resourceLockAt14;services.windows.EnterCriticalSection(lock);bool result=false;
    std::int32_t index=0;
    if(Signed(Read(body.resourceCountAt30))>0)
    {
        do
        {
            auto* slot=AtlBaseResourceSlotNativeAt53E968(body,index,services);
            const auto expected=Read(value);
            if(Read(*slot)==expected){static_cast<void>(RemoveAtlBaseResourceArrayNativeAt53EB0A(body,index,services));result=true;break;}
            index=Signed(static_cast<std::uint32_t>(index)+1u);
        }while(index<Signed(Read(body.resourceCountAt30)));
    }
    services.windows.LeaveCriticalSection(lock);return result;
}
}
