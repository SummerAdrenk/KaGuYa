#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
enum class ProgramControlSoundNativeId:unsigned {Confirm,Hover,Increase,Decrease};
using ProgramControlSoundNativeExit=void(__cdecl*)();
using ProgramControlSoundNativeRegistrar=int(__cdecl*)(ProgramControlSoundNativeExit);
RuntimeCString& ProcessProgramControlSoundNative(ProgramControlSoundNativeId);
const char* ProgramControlSoundNativeLiteralSample18(ProgramControlSoundNativeId);
void InitializeProgramControlSoundNativeSample18(ProgramControlSoundNativeId,RuntimeCString& unconstructed,
    ProgramControlSoundNativeExit,ProgramControlSoundNativeRegistrar,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
}
