#include <Runtime/ProgramTimeSoundNativeGlobals.h>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString)constinit std::byte storage[3*sizeof(RuntimeCString)]{};
RuntimeCString* Slots()noexcept{return reinterpret_cast<RuntimeCString*>(storage);}
void __cdecl Exit(){DestroyProgramTimeSoundsNativeAt5AC290(Slots());}
void __cdecl Initialize(){InitializeProgramTimeSoundsNativeAt406330(Slots(),Exit,&std::atexit);}
struct Prefix final
{
    RuntimeCString* slots;
    int state=-1;
    ~Prefix()noexcept
    {
        // FuncInfo5FDAD4/map5FDAF8：0/1清0；2/3先1后0；4先2后1后0。
        // GetNil发布前无资格；任何展开Free再抛均保留原终止边界。
        if(state>=4)DestroyRuntimeCStringAt40CD40(slots+2);
        if(state>=2)DestroyRuntimeCStringAt40CD40(slots+1);
        if(state>=0)DestroyRuntimeCStringAt40CD40(slots);
    }
};
}
RuntimeCString* ProcessProgramTimeSoundsNative()noexcept{return Slots();}
void InitializeProgramTimeSoundsNativeAt406330(RuntimeCString* slots,ProgramTimeSoundNativeExit callback,
    ProgramTimeSoundNativeRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    Prefix prefix{slots};
    InitializeEmptyRuntimeCString(slots[0],strings);prefix.state=0;
    SetRuntimeCStringAt437680(slots[0],"BGM16.bgm",9);prefix.state=1;
    InitializeEmptyRuntimeCString(slots[1],strings);prefix.state=2;
    SetRuntimeCStringAt437680(slots[1],"BGM16.bgm",9);prefix.state=3;
    InitializeEmptyRuntimeCString(slots[2],strings);prefix.state=4;
    SetRuntimeCStringAt437680(slots[2],"BGM16.bgm",9);
    static_cast<void>(registrar(callback)); // 原406432忽略_atexit返回值。
    prefix.state=-1;
}
void DestroyProgramTimeSoundsNativeAt5AC290(RuntimeCString* slots)
{
    // 原eh vector destructor iterator：普通析构失败后，仍逆序展开余下元素；
    // 失败元素不重试，余下清理再次抛错才terminate。
    struct Remaining final
    {
        RuntimeCString* slots;unsigned count=3;
        ~Remaining()noexcept{while(count)DestroyRuntimeCStringAt40CD40(slots+--count);}
    } remaining{slots};
    while(remaining.count)DestroyRuntimeCStringAt40CD40(slots+--remaining.count);
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD730",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD730")) Initializer const KaGuYaProgramTimeSoundsInitializer=Initialize;
#pragma comment(linker,"/include:_KaGuYaProgramTimeSoundsInitializer")
}
