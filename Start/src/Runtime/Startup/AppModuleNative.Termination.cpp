#include <Runtime/AppModuleNative.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
class Platform final:public IAppModuleNativePlatformApi
{
public:
    DWORD GetCurrentThreadId()override{return ::GetCurrentThreadId();}
    void EnterCriticalSection(LPCRITICAL_SECTION p)override{::EnterCriticalSection(p);}
    void LeaveCriticalSection(LPCRITICAL_SECTION p)override{::LeaveCriticalSection(p);}
    void DeleteCriticalSection(LPCRITICAL_SECTION p)override{::DeleteCriticalSection(p);}
    BOOL DestroyWindow(HWND w)override{return ::DestroyWindow(w);}
    void Free(void* p)override{std::free(p);}
    void RaiseException(DWORD code,DWORD flags)override{::RaiseException(code,flags,0,nullptr);}
} platform;
void ReleaseCom(void* object)
{
    using Release=ULONG(WINAPI*)(void*);
    auto* table=Read(*static_cast<Release* const*>(object));
    Read(table[2])(object);
}
}
IAppModuleNativePlatformApi& DefaultAppModuleNativePlatformApi()noexcept{return platform;}
void ReleaseAppModuleWindowsNativeAt43D866(AppModuleNativeBody3Ch& body,
    AppModuleNativeBody3Ch*& publication,AppModuleNativeServices& services)
{
    auto* lock=reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<std::uintptr_t>(Read(publication))+0x10u);
    services.platform.EnterCriticalSection(lock);
    auto* windows=Read(body.windowsAt38);
    if(windows&&Read(windows->countAt04)>0)
    {
        // 原43D888只销毁首窗口；回调后重新读取全局+38，不能补循环或保留旧record。
        static_cast<void>(services.platform.DestroyWindow(Read(*Read(windows->windowsAt00))));
        windows=Read(body.windowsAt38);
    }
    if(windows)
    {
        if(auto* data=Read(windows->windowsAt00)){services.platform.Free(data);windows->windowsAt00=nullptr;}
        windows->countAt04=0;windows->capacityAt08=0;
        services.memory.DeleteBody(windows,0x0C);
    }
    body.windowsAt38=nullptr;
    services.platform.LeaveCriticalSection(lock);
}
void ReleaseAppModuleMessageLoopsNativeAt43D8D9(AppModuleNativeBody3Ch& body,AppModuleNativeServices& services)
{
    auto* loops=Read(body.loopsAt34);
    if(!loops)return;
    if(auto* data=Read(loops->threadsAt00)){services.platform.Free(data);loops->threadsAt00=nullptr;}
    if(auto* data=Read(loops->loopsAt04)){services.platform.Free(data);loops->loopsAt04=nullptr;}
    loops->countAt08=0;
    services.memory.DeleteBody(loops,0x0C);
    // 原43D910..922不清全局+34，不能借现代owner析构补写。
}
void ReleaseAppModuleFactoriesNativeAt43D922(AppModuleNativeBody3Ch& body)
{
    auto* entry=Read(body.objectMapAt2C);
    if(!entry||!Read(entry->classIdAt00))return;
    do
    {
        if(auto* factory=Read(entry->factoryAt10))ReleaseCom(factory);
        const auto callback=Read(entry->objectMainAt20);
        entry->factoryAt10=nullptr;
        callback(FALSE);
        entry=reinterpret_cast<AppModuleObjectMapNative24h*>(reinterpret_cast<std::uintptr_t>(entry)+0x24u);
    }while(Read(entry->classIdAt00));
}
namespace
{
template<class Body>void Shutdown(Body& body,AppModuleNativeServices& services)
{
    if(!Read(body.sizeAt04))return;
    if(Read(body.terminationAt0C))
    {
        if(reinterpret_cast<std::uintptr_t>(&body)+4u==0)services.platform.RaiseException(EXCEPTION_ACCESS_VIOLATION,1);
        auto* item=Read(body.terminationAt0C);
        while(item)
        {
            const auto argument=Read(item->argumentAt04);
            const auto callback=Read(item->callbackAt00);
            callback(argument);
            auto* next=Read(item->nextAt08);
            services.memory.DeleteBody(item,0x0C);
            item=next;
        }
        static_cast<volatile Body&>(body).terminationAt0C=nullptr;
        static_cast<volatile Body&>(body).terminationAt0C=nullptr;
    }
    if(auto* git=Read(body.gitAt28))ReleaseCom(git);
    services.platform.DeleteCriticalSection(&body.lockAt10);
    body.sizeAt04=0;
}
}
void ShutdownAppModuleNativeAt438830(AppModuleNativeBody3Ch& body,AppModuleNativeServices& services){Shutdown(body,services);}
void ShutdownAppModuleNativeAt438830(AppModuleBaseNativeBody30h& body,AppModuleNativeServices& services){Shutdown(body,services);}
}
