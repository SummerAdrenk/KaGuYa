#include <Runtime/AppModuleNative.h>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
void NotifyAuto(AppModuleAutoMapNativeSource source,BOOL starting)
{
    auto address=reinterpret_cast<std::uintptr_t>(Read(source.beginAt601008));
    auto end=reinterpret_cast<std::uintptr_t>(Read(source.endAt60100C));
    while(address<end)
    {
        auto* entry=Read(*reinterpret_cast<const AppModuleAutoMapNative20h* const*>(address));
        if(entry)
        {
            Read(entry->objectMainAt1C)(starting);
            end=reinterpret_cast<std::uintptr_t>(Read(source.endAt60100C));
        }
        // 418E42/43D971只在非空项调用后重读end；空项继续使用本轮缓存值。
        address+=sizeof(void*);
    }
}
}
HRESULT InitializeAppModuleNativeAt418DC0(AppModuleNativeBody3Ch& body,AppModuleObjectMapNative24h* entries,
    HINSTANCE,const GUID* library,GUID& targetLibrary,AppModuleAutoMapNativeSource source,AppModuleNativeServices& services)
{
    if(library){GUID value;std::memcpy(&value,library,sizeof(value));std::memcpy(&targetLibrary,&value,sizeof(value));}
    if(reinterpret_cast<std::uintptr_t>(entries)!=0xFFFFFFFFu)
    {
        body.objectMapAt2C=entries;
        if(entries&&Read(entries->classIdAt00))
        {
            do
            {
                Read(entries->objectMainAt20)(TRUE);
                entries=reinterpret_cast<AppModuleObjectMapNative24h*>(reinterpret_cast<std::uintptr_t>(entries)+0x24u);
            }while(Read(entries->classIdAt00));
        }
    }
    NotifyAuto(source,TRUE);
    body.threadAt30=services.platform.GetCurrentThreadId();
    body.loopsAt34=nullptr;
    try
    {
        auto* raw=services.memory.AllocateBody(0x0C);
        auto* loops=raw?::new(raw)AppModuleMessageLoopsNative0Ch:nullptr;
        if(loops){loops->threadsAt00=nullptr;loops->loopsAt04=nullptr;loops->countAt08=0;}
        body.loopsAt34=loops;
    }
    catch(...)
    {
        // 原catch只包分配；保留回调已写的+34，再按418E9D读取，不强制改回nullptr。
    }
    if(!Read(body.loopsAt34))return E_OUTOFMEMORY;
    body.windowsAt38=nullptr;
    return S_OK;
}
void NotifyAppModuleAutoMapNativeAt43D954(AppModuleAutoMapNativeSource source){NotifyAuto(source,FALSE);}
}
