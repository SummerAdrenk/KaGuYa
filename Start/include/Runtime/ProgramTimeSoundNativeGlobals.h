#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
using ProgramTimeSoundNativeExit=void(__cdecl*)();
using ProgramTimeSoundNativeRegistrar=int(__cdecl*)(ProgramTimeSoundNativeExit);
// 原61A664/668/66C连续三槽；getter不Capture、不懒建、不补默认字面量。
RuntimeCString* ProcessProgramTimeSoundsNative()noexcept;
void InitializeProgramTimeSoundsNativeAt406330(RuntimeCString* threeUnconstructedSlots,
    ProgramTimeSoundNativeExit,ProgramTimeSoundNativeRegistrar,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
void DestroyProgramTimeSoundsNativeAt5AC290(RuntimeCString* threeSlots);
}
