#include <Runtime/MovieDirectoryNativeGlobals.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString)constinit std::byte storage[sizeof(RuntimeCString)]{};
RuntimeCString& Directory()noexcept{return *reinterpret_cast<RuntimeCString*>(storage);}
void __cdecl Exit(){DestroyRuntimeCStringAt40CD40(&Directory());}
void __cdecl Initialize(){InitializeMovieDirectoryNativeAt404A70(Directory(),Exit,&std::atexit);}
}
RuntimeCString& ProcessMovieDirectoryNative()noexcept{return Directory();}
void InitializeMovieDirectoryNativeAt404A70(RuntimeCString& out,MovieDirectoryNativeExit callback,
    MovieDirectoryNativeRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(out,strings);
    struct Unwind final{RuntimeCString& text;bool active=true;~Unwind()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&text);}} unwind{out};
    SetRuntimeCStringAt437680(out,"Movie\\",6);
    static_cast<void>(registrar(callback));unwind.active=false;
}
// 原404A70为独立CRT初始化项，getter不懒建，raw槽不再参加C++静态析构。
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD66C",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD66C")) Initializer const KaGuYaMovieDirectoryInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaMovieDirectoryInitializer")
}
