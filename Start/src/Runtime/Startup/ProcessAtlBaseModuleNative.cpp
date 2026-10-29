#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <cstdlib>
#include <new>

extern "C" IMAGE_DOS_HEADER __ImageBase;
namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(WindowClassModuleNativeBody38h)constinit std::byte storage[sizeof(WindowClassModuleNativeBody38h)]{};
constinit BYTE failed=0;
WindowClassModuleNativeBody38h& Body()noexcept{return *std::launder(reinterpret_cast<WindowClassModuleNativeBody38h*>(storage));}
AtlBaseModuleNativeServices Services()noexcept{return {DefaultWindowClassNativeApi(),DefaultAtlBaseModuleNativeDiagnostics()};}
void __cdecl ExitAt5AC73C(){auto services=Services();ReleaseAtlBaseModuleNativeAt53E949(Body(),services);}
void __cdecl Initialize()
{
    ::new(storage)WindowClassModuleNativeBody38h;
    auto services=Services();
    InitializeAtlBaseModuleProcessNativeAt40752C(Body(),reinterpret_cast<HINSTANCE>(&__ImageBase),
        AtlBaseModuleNativeVersionGuid(),failed,services,ExitAt5AC73C,&std::atexit);
}
}
WindowClassModuleNativeBody38h& ProcessAtlBaseModuleNative()noexcept{return Body();}
BYTE& ProcessFrameworkStartupFailedNative()noexcept{return failed;}
void InitializeAtlBaseModuleProcessNativeAt40752C(WindowClassModuleNativeBody38h& body,HINSTANCE image,
    const GUID& guid,BYTE& sharedFailed,AtlBaseModuleNativeServices& services,AtlBaseModuleNativeExit exit,
    AtlBaseModuleNativeRegistrar registrar)
{
    ConstructAtlBaseModuleNativeAt53E893(body,image,guid,sharedFailed,services);
    static_cast<void>(registrar(exit));
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4A0",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4A0")) Initializer const KaGuYaAtlBaseModuleInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaAtlBaseModuleInitializer")
}
