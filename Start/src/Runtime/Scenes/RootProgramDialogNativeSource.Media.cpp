#include <Runtime/RootProgramDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ProgramTimeSoundNativeGlobals.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
const int* Volume(void* receiver,unsigned offset)
{
    using Function=const int*(__thiscall*)(void*);
    const auto* table=*static_cast<const std::uintptr_t* const*>(receiver);
    return reinterpret_cast<Function>(table[offset/4])(receiver);
}
}
std::uint8_t RootProgramDialogNativeSource::LoadSurfaceAt480000(void* draw,YtlReferenceSlotRuntime* output,const char* name,unsigned)
{
    if(!*static_cast<const volatile std::uintptr_t*>(&output->baseAt00))
        AdoptGraphicsSurfaceAt407A80(*output,NewGraphicsSurfaceAt40C2E0(dependencies_.graphics,dependencies_.strings));
    return dependencies_.pictures.LoadIntoAt4800A0(draw,*ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(*output),name);
}
void* RootProgramDialogNativeSource::GraphicsAt4182F0(unsigned){return dependencies_.graphics.CaptureAt4182F0();}
POINT* RootProgramDialogNativeSource::ScreenSizeAt47CC00(void* graphics,POINT* out,unsigned)
{return dependencies_.graphics.ReadScreenSizeCapturedAt47CC00(*static_cast<GraphicsRootNativeBody1Ch*>(graphics),out);}
const char* RootProgramDialogNativeSource::TimeSoundNameAt61A664(unsigned index)
{return ProcessProgramTimeSoundsNative()[index].GetString();}
const int* RootProgramDialogNativeSource::BgmVolumeAtVslot14(void* params){return Volume(params,0x14);}
const int* RootProgramDialogNativeSource::SeVolumeAtVslot24(void* params){return Volume(params,0x24);}
const char* RootProgramDialogNativeSource::SoundNameAt61A6xx(unsigned address)
{
    const auto& names=dependencies_.cues;
    switch(address)
    {
    case 0x61A658:return names.confirmAt61A658.GetString();
    case 0x61A65C:return names.hoverAt61A65C.GetString();
    case 0x61A670:return names.increaseAt61A670.GetString();
    case 0x61A674:return names.decreaseAt61A674.GetString();
    default:throw std::invalid_argument("Unknown Program control sound address");
    }
}
std::uint8_t RootProgramDialogNativeSource::PlayBgmAt42BFE0(void* sound,const char* name,int volume,int loop,unsigned)
{return dependencies_.root.NativeSoundServices().PlayBgmCapturedAt42BFE0(static_cast<SoundManagerNativeBody50h*>(sound),name,volume,loop);}
std::uint8_t RootProgramDialogNativeSource::ClearSoundAt4384D0(void* sound,unsigned)
{return dependencies_.root.NativeSoundServices().ClearVoicesCapturedAt4384D0(static_cast<SoundManagerNativeBody50h*>(sound));}
void RootProgramDialogNativeSource::StopSoundAt413270(void* sound,EnvironmentVectorStorage<RuntimeCString>* names,unsigned)
{dependencies_.root.NativeSoundServices().StopEffectsCapturedAt413270(static_cast<SoundManagerNativeBody50h*>(sound),names);}
std::uint8_t RootProgramDialogNativeSource::PlaySeAt4BCD70(void* sound,const char* name,int channel,int volume,std::uint8_t restart,unsigned)
{return dependencies_.root.NativeSoundServices().PlayCueAt4BCD70(static_cast<SoundManagerNativeBody50h*>(sound),name,unsigned(channel),volume,restart);}
void RootProgramDialogNativeSource::CommitAt4B02E0(void* graphics,int argument,unsigned)
{dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*static_cast<GraphicsRootNativeBody1Ch*>(graphics),reinterpret_cast<const RECT*>(unsigned(argument)));}
}
