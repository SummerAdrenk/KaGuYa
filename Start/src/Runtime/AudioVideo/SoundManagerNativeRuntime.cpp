#include <Runtime/SoundManagerNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
SoundManagerNativeRuntime::SoundManagerNativeRuntime(ParamsRuntimeRootOwner& params,
    NativeSoundRuntimeLibrary& library, IArchiveResourceSource& archives, IRuntimeWideStringNativeApi& wide) noexcept
    : params_(params), library_(library),
      volume_(params, library.ModuleSource(), library.StringSource(), library.Win32Source()),
      playback_(params, library.Calls()), open_(archives, library.Calls(), wide, library.StringSource()),
      music_(library.Calls(), open_), effects_(open_),
      demo_{library.Calls(), playback_, wide, library.StringSource()} {}

void SoundManagerNativeRuntime::CreateAt48AAFF(YtlReferenceSlotRuntime& slot, ISoundManagerSavedParamsSource& saved)
{
    auto* const body = CreateSoundManagerAt48AAFF(saved, params_, library_.Calls(), library_.StringSource(), Memory());
    InstallSoundManagerReferenceAt48AB35(slot, body, Memory());
}
void SoundManagerNativeRuntime::Release(YtlReferenceSlotRuntime& slot)
{ ReleaseSoundManagerReference(slot, Memory()); }
ScriptMusicManagerReceiver SoundManagerNativeRuntime::Music(SoundManagerNativeBody50h* body) noexcept
{ return ScriptMusicManagerReceiver::Native(body, music_); }
ScriptVoiceManagerReceiver SoundManagerNativeRuntime::Voice(SoundManagerNativeBody50h* body) noexcept
{ return ScriptVoiceManagerReceiver::Native(body, playback_, open_); }
ReadScriptSoundWaitReceiver SoundManagerNativeRuntime::Wait(SoundManagerNativeBody50h* body) noexcept
{ return ReadScriptSoundWaitReceiver::Native(body, music_, volume_, playback_); }
TextSoundManagerReceiver SoundManagerNativeRuntime::Text(SoundManagerNativeBody50h* body) noexcept
{ return {Voice(body), Effects(body), TextSoundVolumeReceiver::Native(body, volume_), Wait(body)}; }
ScriptSoundManagerReceiver SoundManagerNativeRuntime::Effects(SoundManagerNativeBody50h* body) noexcept
{ return ScriptSoundManagerReceiver::Native(body, music_, volume_, effects_); }
SoundMappingSlotReceiver SoundManagerNativeRuntime::Mappings(SoundManagerNativeBody50h* body) noexcept
{ return SoundMappingSlotReceiver::Native(body, volume_, effects_); }
DemoSoundManagerReceiver SoundManagerNativeRuntime::Demo(SoundManagerNativeBody50h* body) noexcept
{ return DemoSoundManagerReceiver::Native(body, demo_); }
bool SoundManagerNativeRuntime::PlayCueAt4BCD70(SoundManagerNativeBody50h* body,const char* name,
    std::uint32_t groupIndex,std::int32_t fallback,std::uint8_t checkType)
{return effects_.PlayCueAt4BCD70(*body,name,groupIndex,fallback,checkType);}
bool SoundManagerNativeRuntime::PlayBgmCapturedAt42BFE0(SoundManagerNativeBody50h* body,
    const char* name,std::int32_t value,std::int32_t loop)
{
    // 42BFE7首个end/begin门控早于params vslot18及空名字拒绝。
    if(!music_.HasSlotsAt4D5B1A(*body))return true;
    return music_.ApplyActiveAt4D5B28(*body,name,value,loop);
}
bool SoundManagerNativeRuntime::ClearVoicesCapturedAt4384D0(SoundManagerNativeBody50h* body)
{return playback_.StopAndClearAt4384D0(*body);}
bool SoundManagerNativeRuntime::PlayMusicCapturedAt4BCCC0(SoundManagerNativeBody50h* body,
    RuntimeCString& mapping,const char* name,std::int32_t value,std::int32_t loop)
{
    // 复用同一opener/archives与已审计音乐核，不另取Root、不复制50h或环境名。
    return music_.PlayMusicAt4BCCC0(*body,mapping,name,value,loop);
}
void SoundManagerNativeRuntime::StopEffectsCapturedAt413270(SoundManagerNativeBody50h* body,
    EnvironmentVectorStorage<RuntimeCString>* mappings)
{music_.StopEffectsAt413270(*body,mappings);}
bool SoundManagerNativeRuntime::StopAbsoluteCapturedAt4383B0(SoundManagerNativeBody50h* body,std::uint32_t slot)
{return music_.StopAbsoluteAt4383B0(*body,slot);}
void SoundManagerNativeRuntime::InitializeHandlesCapturedAt42CA60(SoundManagerNativeBody50h* body)
{InitializeSoundHandlesNativeAt42CA60(*body,params_,library_.Calls());}
bool SoundManagerNativeRuntime::RestoreFirstCapturedAt42C1B0(SoundManagerNativeBody50h* body,const char* name,
    std::uint32_t group,std::uint8_t alternate)
{return effects_.RestoreFirstAt42C1B0(*body,name,group,alternate);}
bool SoundManagerNativeRuntime::RestoreSecondCapturedAt42C070(SoundManagerNativeBody50h* body,const char* name,
    std::uint32_t group,std::int32_t value)
{return effects_.RestoreSecondAt42C070(*body,name,group,value);}
}
