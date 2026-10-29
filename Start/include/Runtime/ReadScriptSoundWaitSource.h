#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class SoundSlotManagerRuntime;
struct SoundManagerNativeBody50h;
class SoundManagerNativeMusic;
class SoundManagerNativeVolume;
class SoundManagerNativePlayback;

// 4188F0选定的正文与外部服务；不增持、不缓存槽号、不重新捕获全局根。
class ReadScriptSoundWaitReceiver final
{
public:
    ReadScriptSoundWaitReceiver(SoundSlotManagerRuntime*) noexcept;
    static ReadScriptSoundWaitReceiver Native(SoundManagerNativeBody50h*, SoundManagerNativeMusic&,
        SoundManagerNativeVolume&, SoundManagerNativePlayback&) noexcept;
    std::uint32_t MusicSlotAt10() const;
    std::uint32_t FirstEffectsBeginAt18() const;
    std::uint32_t RemainingBeginAt20() const;
    bool IsPlayingAt419370(std::uint32_t slot) const;
    void StopAt4383B0(std::uint32_t slot) const;
    bool AnyVoiceAt4196C0() const;
    void StopVoicesAt4384D0() const;
    void* Object() const noexcept { return object_; }
private:
    struct Operations
    {
        std::uint32_t (*musicSlot)(void*);
        std::uint32_t (*firstBegin)(void*);
        std::uint32_t (*remainingBegin)(void*);
        bool (*playing)(void*, SoundManagerNativeMusic*, std::uint32_t);
        void (*stop)(void*, SoundManagerNativeMusic*, std::uint32_t);
        bool (*voice)(void*, SoundManagerNativeVolume*);
        void (*stopVoice)(void*, SoundManagerNativePlayback*);
    };
    ReadScriptSoundWaitReceiver(void* object, SoundManagerNativeMusic* music, SoundManagerNativeVolume* volume,
        SoundManagerNativePlayback* playback, const Operations& operations) noexcept
        : object_(object), music_(music), volume_(volume), playback_(playback), operations_(&operations) {}
    void* object_;
    SoundManagerNativeMusic* music_;
    SoundManagerNativeVolume* volume_;
    SoundManagerNativePlayback* playback_;
    const Operations* operations_;
};

struct ReadScriptSoundWaitRootReceiver final
{
    void* object;
    ReadScriptSoundWaitReceiver (*resolveAt4188F0)(void*);
    ReadScriptSoundWaitReceiver Resolve() const { return resolveAt4188F0(object); }
};
class IReadScriptSoundWaitSource
{
public:
    virtual ~IReadScriptSoundWaitSource() = default;
    virtual ReadScriptSoundWaitRootReceiver CaptureRootAfterEnsureAt413BE0() = 0;
};
class RootReadScriptSoundWaitSource final : public IReadScriptSoundWaitSource
{
public:
    explicit RootReadScriptSoundWaitSource(RootRuntimeOwner& owner) noexcept : owner_(owner) {}
    ReadScriptSoundWaitRootReceiver CaptureRootAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& owner_;
};
}
