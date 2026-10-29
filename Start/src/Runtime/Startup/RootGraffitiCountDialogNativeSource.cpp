#include <Runtime/RootGraffitiCountDialogNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/SoundManagerNativeVolume.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
bool Play(void* object,void* service,const char* name,int group,int fallback,std::uint8_t checkType)
{return static_cast<SoundManagerNativeRuntime*>(service)->PlayCueAt4BCD70(
    static_cast<SoundManagerNativeBody50h*>(object),name,std::bit_cast<std::uint32_t>(group),fallback,checkType);}
std::uint32_t Remaining(void* object,void*)
{return static_cast<const volatile SoundManagerNativeBody50h*>(object)->remainingEffectsBeginAt20;}
bool Playing(void* object,void* service,std::uint32_t slot)
{return static_cast<SoundManagerNativeRuntime*>(service)->Wait(static_cast<SoundManagerNativeBody50h*>(object)).IsPlayingAt419370(slot);}
#define SYSTEM_SOUND "sed:\\\x83\x56\x83\x58\x83\x65\x83\x80\x89\xB9\x81\x79"
constexpr std::array<GraffitiCountCueDefaultRecord,3> records{{
    {0x61A688,0x406590,0x5BB688,SYSTEM_SOUND "\x83\x6E\x81\x5B\x83\x67\x83\x41\x83\x62\x83\x76\x89\xB9\x81\x7A.ogg"},
    {0x61A68C,0x406A10,0x5B1BA0,SYSTEM_SOUND "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x89\xB9\x81\x7A.ogg"},
    {0x61A690,0x406500,0x5B62F0,SYSTEM_SOUND "\x83\x58\x83\x65\x81\x5B\x83\x5E\x83\x58\x83\x41\x83\x62\x83\x76\x89\xB9\x81\x7A.ogg"}
}};
#undef SYSTEM_SOUND
}
const std::array<GraffitiCountCueDefaultRecord,3>& GraffitiCountCueDefaultRecords() noexcept{return records;}
RootGraffitiCountDialogNativeSource::RootGraffitiCountDialogNativeSource(
    RootRuntimeOwner& root,EnvironmentRootOwner& environment,GraffitiCountCueSlots cues) noexcept
    :root_(root),common_(root,environment),cues_(cues){}
EnvironmentRootStorage* RootGraffitiCountDialogNativeSource::CaptureEnvironmentAt4180C0()
{return common_.CaptureEnvironmentAt4180C0();}
GraffitiPageNativeParamsReceiver RootGraffitiCountDialogNativeSource::CaptureParamsRootAfter413BE0()
{return common_.CaptureParamsRootAfter413BE0();}
const char* RootGraffitiCountDialogNativeSource::ReadCueName(std::uint32_t address)
{
    // 返回当次真实CString chars；不Clone、不规范化，也不跨后面的413BE0保存副本。
    switch(address)
    {
    case 0x61A688:return cues_.likeAt61A688.GetString();
    case 0x61A68C:return cues_.weightAt61A68C.GetString();
    case 0x61A690:return cues_.trainingAt61A690.GetString();
    default:throw std::logic_error("Graffiti cue source received an unverified global slot");
    }
}
GraffitiCountDialogSoundBorrow RootGraffitiCountDialogNativeSource::CaptureSoundAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();auto& post=root_.Published()->PostWindow();
    // 固定这次4188F0的50h与正文外服务；后续Play/Remaining/Playing不重取根。
    return {post.SoundManagerAt24(),&post.SoundServices(),Play,Remaining,Playing};
}
}
