#include <Runtime/ReplaySceneNativeGlobals.h>
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString) constinit std::byte storage[sizeof(RuntimeCString)]{};
RuntimeCString& Value()noexcept{return *reinterpret_cast<RuntimeCString*>(storage);}
void __cdecl ExitAt5ABBC0(){DestroyRuntimeCStringAt40CD40(&Value());}
void __cdecl Initialize(){InitializeReplaySceneNativeBackgroundMusicAt404C20(Value(),ExitAt5ABBC0,&std::atexit);}
}
RuntimeCString& ProcessReplaySceneNativeBackgroundMusic()noexcept{return Value();}
void InitializeReplaySceneNativeBackgroundMusicAt404C20(RuntimeCString& out,CgSceneNativeExit callback,
    CgSceneNativeExitRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(out,strings);
    struct Prefix
    {
        RuntimeCString& out;bool active=true;
        ~Prefix() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&out);}
    } prefix{out};
    const auto name=CurrentAppreciationSceneProfile().replayBgm;
    SetRuntimeCStringAt437680(out,name.data(),static_cast<int>(name.size()));
    (void)registrar(callback);
    prefix.active=false;
}
// 原PE实际CRT表5AD688紧随CG的5AD684；不是首次访问时的业务懒初始化。
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD688",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD688")) Initializer const KaGuYaReplayMusicInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaReplayMusicInitializer")
}
