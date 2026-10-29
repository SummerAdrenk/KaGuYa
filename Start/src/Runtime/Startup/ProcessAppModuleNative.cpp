#include <Runtime/ProcessAppModuleNative.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <bit>
#include <cstdlib>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(AppModuleNativeBody3Ch)constinit std::byte storage[sizeof(AppModuleNativeBody3Ch)]{};
constinit AppModuleNativeBody3Ch* publication=nullptr;
constinit AppModuleNativeBody3Ch* atlPublication=nullptr;
constinit GUID library{};
// 原PE601008/60100C均为5FF3F8，空范围重定位为同一个本地稳定地址，不造业务entry。
constinit const AppModuleAutoMapNative20h* emptyMarker=nullptr;
constinit const AppModuleAutoMapNative20h* const* autoBegin=&emptyMarker;
constinit const AppModuleAutoMapNative20h* const* autoEnd=&emptyMarker;
AppModuleNativeBody3Ch& Body()noexcept{return *std::launder(reinterpret_cast<AppModuleNativeBody3Ch*>(storage));}
AppModuleNativeServices Services()noexcept{return {DefaultEnvironmentRootMemory(),DefaultAppModuleNativePlatformApi()};}
void __cdecl ExitAt5AA420(){auto services=Services();ReleaseProcessAppModuleNativeAt5AA420(Body(),services);}
void __cdecl Initialize()
{
    ::new(storage)AppModuleNativeBody3Ch;
    ConstructProcessAppModuleNativeAt401340(Body(),publication,atlPublication,ProcessFrameworkStartupFailedNative(),
        DefaultWindowClassNativeApi(),ExitAt5AA420,&std::atexit);
}
}
AppModuleNativeBody3Ch& ProcessAppModuleNative()noexcept{return Body();}
AppModuleNativeBody3Ch*& ProcessAppModulePublicationNative()noexcept{return publication;}
AppModuleNativeBody3Ch*& ProcessAtlModulePublicationNative()noexcept{return atlPublication;}
GUID& ProcessAppModuleLibraryIdNative()noexcept{return library;}
AppModuleAutoMapNativeSource ProcessAppModuleAutoMapNative()noexcept{return {autoBegin,autoEnd};}
void ConstructProcessAppModuleNativeAt401340(AppModuleNativeBody3Ch& body,AppModuleNativeBody3Ch*& first,
    AppModuleNativeBody3Ch*& second,BYTE& failed,IWindowClassNativeApi& api,AppModuleNativeExit exit,AppModuleNativeRegistrar registrar)
{
    first=&body;std::uint32_t result=0;
    if(!api.InitializeCriticalSectionAndSpinCount(&body.lockAt10,0))
    {
        result=api.GetLastError();if(std::bit_cast<std::int32_t>(result)>0)result=(result&0xFFFFu)|0x80070000u;
    }
    if(std::bit_cast<std::int32_t>(result)<0)failed=1;else body.sizeAt04=0x24;
    second=&body;body.vptrAt00=&AppModuleNativeMethods();static_cast<void>(registrar(exit));
}
void ReleaseProcessAppModuleNativeAt5AA420(AppModuleNativeBody3Ch& body,AppModuleNativeServices& services)
{
    body.vptrAt00=&AppModuleBaseNativeMethods();ShutdownAppModuleNativeAt438830(body,services);
}
HRESULT InitializeProcessAppModuleFrameworkNativeAt43D724(HINSTANCE image)
{
    auto services=Services();
    return InitializeAppModuleNativeAt418DC0(Body(),nullptr,image,nullptr,library,{autoBegin,autoEnd},services);
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD504",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD504")) Initializer const KaGuYaAppModuleInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaAppModuleInitializer")
}
