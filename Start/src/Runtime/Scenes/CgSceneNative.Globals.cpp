#include <Runtime/CgSceneNativeGlobals.h>
#include <Runtime/AppreciationSceneResourceProfile.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString) constinit std::byte storage[sizeof(RuntimeCString)]{};
RuntimeCString& Value() noexcept{return *reinterpret_cast<RuntimeCString*>(storage);}
void __cdecl ExitAt5ABB00(){DestroyRuntimeCStringAt40CD40(&Value());}
void __cdecl Initialize(){InitializeCgSceneNativeBackgroundMusicAt4048C0(Value(),ExitAt5ABB00,&std::atexit);}
}
RuntimeCString& ProcessCgSceneNativeBackgroundMusic() noexcept{return Value();}
const char* CgSceneNativeBackgroundMusicLiteralSample18() noexcept{return "bgm:\\BGM09.ogg";}
void InitializeCgSceneNativeBackgroundMusicAt4048C0(RuntimeCString& out,CgSceneNativeExit callback,
    CgSceneNativeExitRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(out,strings);
    struct Prefix
    {
        RuntimeCString& out;bool active=true;
        ~Prefix() noexcept{if(active)DestroyRuntimeCStringAt40CD40(&out);}
    } prefix{out};
    const auto name=CurrentAppreciationSceneProfile().cgBgm;
    SetRuntimeCStringAt437680(out,name.data(),static_cast<int>(name.size()));
    (void)registrar(callback); // 原_atexit返回值不参与分支；抛出才展开state0。
    prefix.active=false;
}
// 原PE 5AD684→4048C0，按实际CRT表顺序注册，不依赖首次getter的静态懒初始化。
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD684",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD684")) Initializer const KaGuYaCgMusicInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaCgMusicInitializer")
}
