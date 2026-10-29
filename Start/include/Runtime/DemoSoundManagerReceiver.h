#pragma once
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class IArchiveResourceSource;
class SoundSlotManagerRuntime;
class SoundModuleNativeApi;
class SoundManagerNativePlayback;
class IRuntimeWideStringNativeApi;
struct SoundManagerNativeBody50h;

// 服务须活过借用receiver；不拥有50h，opener在调用时使用caller传入的archive。
struct DemoSoundNativeServices final
{
    SoundModuleNativeApi& sound;
    SoundManagerNativePlayback& playback;
    IRuntimeWideStringNativeApi& wide;
    IRuntimeStringManagerSource& strings;
};
class DemoSoundManagerReceiver final
{
public:
    DemoSoundManagerReceiver(SoundSlotManagerRuntime*) noexcept;
    static DemoSoundManagerReceiver Native(SoundManagerNativeBody50h*, DemoSoundNativeServices&) noexcept;
    void StopDemoMusicAt4C3500(RuntimeCString&, IArchiveResourceSource&) const;
    bool PlayDemoMusicAt4BCCC0(RuntimeCString&, const char*, std::int32_t value,
        std::int32_t loop, IArchiveResourceSource&) const;
    void StopAndClearAllVoiceSlots() const;
    bool PlayDemoVoiceAt4BCE10(const char*, IArchiveResourceSource&) const;
    void StopEffectsKeepingMappingsAt413270(IArchiveResourceSource&) const;
    void* Object() const noexcept { return object_; }
private:
    struct Operations
    {
        void (*stopMusic)(void*, DemoSoundNativeServices*, RuntimeCString&, IArchiveResourceSource&);
        bool (*music)(void*, DemoSoundNativeServices*, RuntimeCString&, const char*, std::int32_t, std::int32_t, IArchiveResourceSource&);
        void (*stopVoice)(void*, DemoSoundNativeServices*);
        bool (*voice)(void*, DemoSoundNativeServices*, const char*, IArchiveResourceSource&);
        void (*stopEffects)(void*, DemoSoundNativeServices*, IArchiveResourceSource&);
    };
    DemoSoundManagerReceiver(void* object, DemoSoundNativeServices* services, const Operations& operations) noexcept
        : object_(object), services_(services), operations_(&operations) {}
    void* object_;
    DemoSoundNativeServices* services_;
    const Operations* operations_;
};
}
