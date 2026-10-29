#include <Runtime/ReadScriptWaitStateSource.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/ScrVmRuntimeComposition.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

namespace KaGuYa::Start::Runtime
{
FixedReadScriptWaitStateSource::FixedReadScriptWaitStateSource(ScrVmRuntimeStateBundle& bundle,
    const ScrVmExecutableProfile& profile,const SaveDataEnvironmentArchive& save,UnSaveDataRuntime& unsave,
    Platform::MouseView mouse,Platform::KeyboardView keyboard,std::uint8_t& rootByte) noexcept
    :bundle_(bundle),profile_(profile),save_(save),unsave_(unsave),mouse_(mouse),input_(mouse,keyboard),rootByte_(rootByte){}
const SaveDataEnvironmentArchive& FixedReadScriptWaitStateSource::CaptureSaveAt4180C0(){return save_;}
UnSaveDataRuntime& FixedReadScriptWaitStateSource::CaptureUnSaveAt4180C0(){return unsave_;}
bool FixedReadScriptWaitStateSource::ReadPressedAt47F4E0(std::int32_t action){return input_.ReadPressedAtVslot04(action);}
bool FixedReadScriptWaitStateSource::ReadHeldAt4198D0(std::int32_t action){return input_.ReadHeldAtVslot00(action);}
bool FixedReadScriptWaitStateSource::MousePressedEdge(std::size_t button)
{return mouse_.PreviousPressed(button)==0&&mouse_.Pressed(button)!=0&&mouse_.ReleaseSuppressionCountdown()==0;}
void FixedReadScriptWaitStateSource::SetMouseCountdownAt28(std::int32_t value){mouse_.SetReleaseSuppressionCountdown(value);}
bool FixedReadScriptWaitStateSource::SuppressEffectAt4C3430()
{return input_.ReadHeldAtVslot00(6)||(unsave_.FlagsAt14()&2u)!=0||save_.valueAt6F==0;}
bool FixedReadScriptWaitStateSource::SkipEffectAt4AA970()
{return input_.ReadHeldAtVslot00(5)||MousePressedEdge(0);}
std::int32_t FixedReadScriptWaitStateSource::ReadWaitValueAfter413BE0(unsigned index){return bundle_.scriptStorage.ReadWaitValue(index);}
void FixedReadScriptWaitStateSource::WriteWaitSkippedAfter413BE0(std::int32_t value)
{
    if (CompiledGameProfile().waitSkipResultEnabled)
        bundle_.values.WriteSigned(profile_.ResolveNamedValue("wait_skiped"),value);
}
void FixedReadScriptWaitStateSource::ClearRootByteAt1B0After413BE0(){rootByte_=0;}
}
