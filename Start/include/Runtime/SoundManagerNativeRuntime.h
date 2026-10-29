#pragma once

#include <Runtime/NativeSoundRuntimeLibrary.h>
#include <Runtime/SoundManagerNativeLifecycle.h>
#include <Runtime/SoundManagerNativeEffects.h>
#include <Runtime/SoundManagerNativeMusic.h>
#include <Runtime/DemoSoundManagerReceiver.h>
#include <Runtime/ScriptMusicSource.h>
#include <Runtime/ScriptVoiceSource.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/SoundMappingRestoreSource.h>

namespace KaGuYa::Start::Runtime
{
// 正文之外的稳定服务。根各代只拥有Ref12/50h，不保存另一套槽名或params镜像。
// library、archives及所有输入服务须长于本服务和逃逸receiver。
class SoundManagerNativeRuntime final
{
public:
    SoundManagerNativeRuntime(ParamsRuntimeRootOwner&, NativeSoundRuntimeLibrary&,
        IArchiveResourceSource&, IRuntimeWideStringNativeApi& = DefaultRuntimeWideStringNativeApi()) noexcept;
    SoundManagerNativeRuntime(const SoundManagerNativeRuntime&) = delete;
    SoundManagerNativeRuntime& operator=(const SoundManagerNativeRuntime&) = delete;

    NativeSoundRuntimeLibrary& Library() noexcept { return library_; }
    IEnvironmentRootMemory& Memory() noexcept { return library_.MemorySource(); }
    // 48AAFF/48AB35：整个新构造先完成，再退役旧槽；失败不回滚原副作用。
    void CreateAt48AAFF(YtlReferenceSlotRuntime&, ISoundManagerSavedParamsSource&);
    void Release(YtlReferenceSlotRuntime&);
    ScriptMusicManagerReceiver Music(SoundManagerNativeBody50h*) noexcept;
    ScriptVoiceManagerReceiver Voice(SoundManagerNativeBody50h*) noexcept;
    ReadScriptSoundWaitReceiver Wait(SoundManagerNativeBody50h*) noexcept;
    TextSoundManagerReceiver Text(SoundManagerNativeBody50h*) noexcept;
    ScriptSoundManagerReceiver Effects(SoundManagerNativeBody50h*) noexcept;
    SoundMappingSlotReceiver Mappings(SoundManagerNativeBody50h*) noexcept;
    DemoSoundManagerReceiver Demo(SoundManagerNativeBody50h*) noexcept;
    bool PlayCueAt4BCD70(SoundManagerNativeBody50h* body,const char* name,
        std::uint32_t groupIndex,std::int32_t fallback,std::uint8_t checkType);
    // 只消费已选50h，不额外写环境名或捕获Root/Environment。
    bool PlayBgmCapturedAt42BFE0(SoundManagerNativeBody50h*,const char*,std::int32_t,std::int32_t);
    bool PlayMusicCapturedAt4BCCC0(SoundManagerNativeBody50h*,RuntimeCString& mapping,
        const char*,std::int32_t,std::int32_t);
    bool ClearVoicesCapturedAt4384D0(SoundManagerNativeBody50h*);
    void StopEffectsCapturedAt413270(SoundManagerNativeBody50h*,EnvironmentVectorStorage<RuntimeCString>*);
    bool StopAbsoluteCapturedAt4383B0(SoundManagerNativeBody50h*,std::uint32_t);
    void InitializeHandlesCapturedAt42CA60(SoundManagerNativeBody50h*);
    bool RestoreFirstCapturedAt42C1B0(SoundManagerNativeBody50h*,const char*,std::uint32_t,std::uint8_t);
    bool RestoreSecondCapturedAt42C070(SoundManagerNativeBody50h*,const char*,std::uint32_t,std::int32_t);
private:
    ParamsRuntimeRootOwner& params_;
    NativeSoundRuntimeLibrary& library_;
    SoundManagerNativeVolume volume_;
    SoundManagerNativePlayback playback_;
    SoundManagerNativeOpen open_;
    SoundManagerNativeMusic music_;
    SoundManagerNativeEffects effects_;
    DemoSoundNativeServices demo_;
};
}
