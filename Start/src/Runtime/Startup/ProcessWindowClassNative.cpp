#include <Runtime/ProcessWindowClassNative.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE601028的文件初值就是2Ch，其余十DWORD为0；4012F0成功时不重写size。
constinit WindowClassWinModuleNativeBody2Ch module{.sizeAt00=0x2C};
void __cdecl ExitAt5AA340()
{ReleaseWindowClassWinModuleNativeAt5AA340(module,ProcessAtlBaseModuleNative(),DefaultWindowClassNativeApi());}
void __cdecl Initialize()
{InitializeProcessWindowClassWinModuleNativeAt4012F0(module,ProcessFrameworkStartupFailedNative(),DefaultWindowClassNativeApi(),ExitAt5AA340,&std::atexit);}
}
WindowClassWinModuleNativeBody2Ch& ProcessWindowClassWinModuleNative()noexcept{return module;}
void InitializeProcessWindowClassWinModuleNativeAt4012F0(WindowClassWinModuleNativeBody2Ch& body,BYTE& sharedFailed,
    IWindowClassNativeApi& api,WindowClassWinModuleNativeExit exit,WindowClassWinModuleNativeRegistrar registrar)
{
    InitializeWindowClassWinModuleNativeAt4012F0(body,sharedFailed,api);
    static_cast<void>(registrar(exit));
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4A8",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4A8")) Initializer const KaGuYaWindowClassWinModuleInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaWindowClassWinModuleInitializer")
}
