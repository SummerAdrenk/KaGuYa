#include <Runtime/ModeButtonNativeGlobals.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString) constinit std::byte storage[sizeof(RuntimeCString)]{};
RuntimeCString& Cue() noexcept{return *reinterpret_cast<RuntimeCString*>(storage);}
void __cdecl ExitAt5ABAE0(){DestroyRuntimeCStringAt40CD40(&Cue());}
void __cdecl Initialize(){InitializeModeButtonNativeExitCueAt404830(Cue(),ExitAt5ABAE0,&std::atexit);}
}
RuntimeCString& ProcessModeButtonNativeExitCue() noexcept{return Cue();}
const char* ModeButtonNativeExitCueLiteralSample18() noexcept
{
    return "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x89\xB9\x81\x7A.ogg";
}
void InitializeModeButtonNativeExitCueAt404830(RuntimeCString& out,ModeButtonNativeExit callback,
    ModeButtonNativeExitRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(out,strings);
    struct Prefix final{RuntimeCString& out;bool active=true;~Prefix() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&out);}} prefix{out};
    SetRuntimeCStringAt437680(out,ModeButtonNativeExitCueLiteralSample18(),35);
    static_cast<void>(registrar(callback)); // 原_atexit返回负数也不撤销已完成的字符串。
    prefix.active=false;
}
// 原5AD660→404830先于后续Setting/Log字体初始化；raw槽无C++自动静态析构。
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD660",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD660")) Initializer const KaGuYaModeButtonCueInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaModeButtonCueInitializer")
}
