#include <Runtime/ProgramControlSoundNativeGlobals.h>
#include <array>
#include <cstdlib>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString)constinit std::byte storage[4*sizeof(RuntimeCString)]{};
constexpr const char* literals[]{
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79\x83\x4E\x83\x8A\x83\x62\x83\x4E\x89\xB9\x81\x7A.ogg",
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79\x91\x49\x91\xF0\x89\xB9\x81\x7A.ogg",
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79\x83\x6E\x81\x5B\x83\x67\x83\x41\x83\x62\x83\x76\x89\xB9\x81\x7A.ogg",
    "sed:\\" "\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79\x83\x7C\x83\x43\x83\x93\x83\x67\x82\x74\x82\x6F\x89\xB9\x81\x7A.ogg"};
constexpr int lengths[]{0x21,0x1D,0x25,0x25};
unsigned Index(ProgramControlSoundNativeId id)
{
    const auto index=static_cast<unsigned>(id);
    if(index>=4)throw std::invalid_argument("Unknown Program control sound global");
    return index;
}
template<ProgramControlSoundNativeId Id>void __cdecl Exit()
{DestroyRuntimeCStringAt40CD40(&ProcessProgramControlSoundNative(Id));}
template<ProgramControlSoundNativeId Id>void __cdecl Initialize()
{InitializeProgramControlSoundNativeSample18(Id,ProcessProgramControlSoundNative(Id),Exit<Id>,&std::atexit);}
}
RuntimeCString& ProcessProgramControlSoundNative(ProgramControlSoundNativeId id)
{return reinterpret_cast<RuntimeCString*>(storage)[Index(id)];}
const char* ProgramControlSoundNativeLiteralSample18(ProgramControlSoundNativeId id){return literals[Index(id)];}
void InitializeProgramControlSoundNativeSample18(ProgramControlSoundNativeId id,RuntimeCString& text,
    ProgramControlSoundNativeExit callback,ProgramControlSoundNativeRegistrar registrar,IRuntimeStringManagerSource& strings)
{
    const auto index=Index(id);
    InitializeEmptyRuntimeCString(text,strings);
    // 4066B0/4067D0/406980/406860均在GetNil写槽后取得唯一state0。
    struct Prefix final{RuntimeCString& text;bool active=true;~Prefix()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&text);}} prefix{text};
    SetRuntimeCStringAt437680(text,literals[index],lengths[index]);
    static_cast<void>(registrar(callback));prefix.active=false;
}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD704",read)
#pragma section(".CRT$XCUKaGuYa5AD708",read)
#pragma section(".CRT$XCUKaGuYa5AD734",read)
#pragma section(".CRT$XCUKaGuYa5AD738",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD704")) Initializer const KaGuYaProgramConfirmInitializer=Initialize<ProgramControlSoundNativeId::Confirm>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD708")) Initializer const KaGuYaProgramHoverInitializer=Initialize<ProgramControlSoundNativeId::Hover>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD734")) Initializer const KaGuYaProgramIncreaseInitializer=Initialize<ProgramControlSoundNativeId::Increase>;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD738")) Initializer const KaGuYaProgramDecreaseInitializer=Initialize<ProgramControlSoundNativeId::Decrease>;
#pragma comment(linker,"/include:_KaGuYaProgramConfirmInitializer")
#pragma comment(linker,"/include:_KaGuYaProgramHoverInitializer")
#pragma comment(linker,"/include:_KaGuYaProgramIncreaseInitializer")
#pragma comment(linker,"/include:_KaGuYaProgramDecreaseInitializer")
}
