#include <Runtime/DemoSoundManagerReceiver.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SoundManagerNativeMusic.h>
#include <Runtime/SoundManagerNativeOpen.h>

namespace KaGuYa::Start::Runtime
{
DemoSoundManagerReceiver::DemoSoundManagerReceiver(SoundSlotManagerRuntime* object) noexcept
    : object_(object), services_(nullptr)
{
    static const Operations operations{
        [](void* p, DemoSoundNativeServices*, RuntimeCString& name, IArchiveResourceSource&) {
            static_cast<SoundSlotManagerRuntime*>(p)->StopDemoMusicAt4C3500(name);
        },
        [](void* p, DemoSoundNativeServices*, RuntimeCString& output, const char* name,
            std::int32_t value, std::int32_t loop, IArchiveResourceSource& archives) {
            return static_cast<SoundSlotManagerRuntime*>(p)->PlayDemoMusicAt4BCCC0(output, name, value, loop, archives);
        },
        [](void* p, DemoSoundNativeServices*) { static_cast<SoundSlotManagerRuntime*>(p)->StopAndClearAllVoiceSlots(); },
        [](void* p, DemoSoundNativeServices*, const char* name, IArchiveResourceSource& archives) {
            return static_cast<SoundSlotManagerRuntime*>(p)->PlayDemoVoiceAt4BCE10(name, archives);
        },
        [](void* p, DemoSoundNativeServices*, IArchiveResourceSource&) { static_cast<SoundSlotManagerRuntime*>(p)->StopEffectsKeepingMappingsAt413270(); }};
    operations_ = &operations;
}
DemoSoundManagerReceiver DemoSoundManagerReceiver::Native(SoundManagerNativeBody50h* body,
    DemoSoundNativeServices& services) noexcept
{
    static const Operations operations{
        [](void* p, DemoSoundNativeServices* s, RuntimeCString& name, IArchiveResourceSource& archives) {
            SoundManagerNativeOpen open(archives, s->sound, s->wide, s->strings);
            SoundManagerNativeMusic music(s->sound, open);
            music.StopMusicAt4C3500(*static_cast<SoundManagerNativeBody50h*>(p), name);
        },
        [](void* p, DemoSoundNativeServices* s, RuntimeCString& output, const char* name,
            std::int32_t value, std::int32_t loop, IArchiveResourceSource& archives) {
            SoundManagerNativeOpen open(archives, s->sound, s->wide, s->strings);
            SoundManagerNativeMusic music(s->sound, open);
            return music.PlayMusicAt4BCCC0(*static_cast<SoundManagerNativeBody50h*>(p), output, name, value, loop);
        },
        [](void* p, DemoSoundNativeServices* s) {
            (void)s->playback.StopAndClearAt4384D0(*static_cast<SoundManagerNativeBody50h*>(p));
        },
        [](void* p, DemoSoundNativeServices* s, const char* name, IArchiveResourceSource& archives) {
            SoundManagerNativeOpen open(archives, s->sound, s->wide, s->strings);
            return s->playback.PlaySingleAt4BCE10(*static_cast<SoundManagerNativeBody50h*>(p), name, open, s->strings);
        },
        [](void* p, DemoSoundNativeServices* s, IArchiveResourceSource& archives) {
            SoundManagerNativeOpen open(archives, s->sound, s->wide, s->strings);
            SoundManagerNativeMusic music(s->sound, open);
            music.StopEffectsAt413270(*static_cast<SoundManagerNativeBody50h*>(p), nullptr);
        }};
    return {body, &services, operations};
}
void DemoSoundManagerReceiver::StopDemoMusicAt4C3500(RuntimeCString& name, IArchiveResourceSource& archives) const
{ operations_->stopMusic(object_, services_, name, archives); }
bool DemoSoundManagerReceiver::PlayDemoMusicAt4BCCC0(RuntimeCString& output, const char* name,
    std::int32_t value, std::int32_t loop, IArchiveResourceSource& archives) const
{ return operations_->music(object_, services_, output, name, value, loop, archives); }
void DemoSoundManagerReceiver::StopAndClearAllVoiceSlots() const
{ operations_->stopVoice(object_, services_); }
bool DemoSoundManagerReceiver::PlayDemoVoiceAt4BCE10(const char* name, IArchiveResourceSource& archives) const
{ return operations_->voice(object_, services_, name, archives); }
void DemoSoundManagerReceiver::StopEffectsKeepingMappingsAt413270(IArchiveResourceSource& archives) const
{ operations_->stopEffects(object_, services_, archives); }
}
