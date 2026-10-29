#include <Runtime/ReadScriptSoundWaitSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SoundManagerNativeMusic.h>

namespace KaGuYa::Start::Runtime
{
ReadScriptSoundWaitReceiver::ReadScriptSoundWaitReceiver(SoundSlotManagerRuntime* object) noexcept
    : object_(object), music_(nullptr), volume_(nullptr), playback_(nullptr)
{
    static const Operations operations{
        [](void* p) { return static_cast<std::uint32_t>(static_cast<SoundSlotManagerRuntime*>(p)->Layout().currentMusicSlot); },
        [](void* p) { return static_cast<std::uint32_t>(static_cast<SoundSlotManagerRuntime*>(p)->Layout().firstBoundary); },
        [](void* p) { return static_cast<std::uint32_t>(static_cast<SoundSlotManagerRuntime*>(p)->Layout().thirdBoundary); },
        [](void* p, SoundManagerNativeMusic*, std::uint32_t slot) {
            return static_cast<SoundSlotManagerRuntime*>(p)->IsAbsoluteSlotPlayingAt419370(slot);
        },
        [](void* p, SoundManagerNativeMusic*, std::uint32_t slot) {
            static_cast<SoundSlotManagerRuntime*>(p)->StopAbsoluteSlotAt4383B0(slot);
        },
        [](void* p, SoundManagerNativeVolume*) { return static_cast<SoundSlotManagerRuntime*>(p)->AnyVoiceSlotPlaying(); },
        [](void* p, SoundManagerNativePlayback*) { static_cast<SoundSlotManagerRuntime*>(p)->StopAndClearAllVoiceSlots(); }};
    operations_ = &operations;
}
ReadScriptSoundWaitReceiver ReadScriptSoundWaitReceiver::Native(SoundManagerNativeBody50h* body,
    SoundManagerNativeMusic& music, SoundManagerNativeVolume& volume, SoundManagerNativePlayback& playback) noexcept
{
    static const Operations operations{
        [](void* p) { return static_cast<const volatile SoundManagerNativeBody50h*>(p)->musicAt10; },
        [](void* p) { return static_cast<const volatile SoundManagerNativeBody50h*>(p)->firstEffectsBeginAt18; },
        [](void* p) { return static_cast<const volatile SoundManagerNativeBody50h*>(p)->remainingEffectsBeginAt20; },
        [](void* p, SoundManagerNativeMusic* service, std::uint32_t slot) {
            return service->IsAbsolutePlayingAt419370(*static_cast<SoundManagerNativeBody50h*>(p), slot);
        },
        [](void* p, SoundManagerNativeMusic* service, std::uint32_t slot) {
            (void)service->StopAbsoluteAt4383B0(*static_cast<SoundManagerNativeBody50h*>(p), slot);
        },
        [](void* p, SoundManagerNativeVolume* service) {
            return service->AnyVoicePlayingAt4196C0(*static_cast<SoundManagerNativeBody50h*>(p));
        },
        [](void* p, SoundManagerNativePlayback* service) {
            (void)service->StopAndClearAt4384D0(*static_cast<SoundManagerNativeBody50h*>(p));
        }};
    return {body, &music, &volume, &playback, operations};
}
std::uint32_t ReadScriptSoundWaitReceiver::MusicSlotAt10() const { return operations_->musicSlot(object_); }
std::uint32_t ReadScriptSoundWaitReceiver::FirstEffectsBeginAt18() const { return operations_->firstBegin(object_); }
std::uint32_t ReadScriptSoundWaitReceiver::RemainingBeginAt20() const { return operations_->remainingBegin(object_); }
bool ReadScriptSoundWaitReceiver::IsPlayingAt419370(std::uint32_t slot) const { return operations_->playing(object_, music_, slot); }
void ReadScriptSoundWaitReceiver::StopAt4383B0(std::uint32_t slot) const { operations_->stop(object_, music_, slot); }
bool ReadScriptSoundWaitReceiver::AnyVoiceAt4196C0() const { return operations_->voice(object_, volume_); }
void ReadScriptSoundWaitReceiver::StopVoicesAt4384D0() const { operations_->stopVoice(object_, playback_); }

ReadScriptSoundWaitRootReceiver RootReadScriptSoundWaitSource::CaptureRootAfterEnsureAt413BE0()
{
    owner_.EnsureCreatedAt413BE0();
    return {&owner_.RequirePublished(), [](void* p) {
        return static_cast<RootRuntimePayload*>(p)->PostWindow().SoundWaitReceiverAt24();
    }};
}
}
