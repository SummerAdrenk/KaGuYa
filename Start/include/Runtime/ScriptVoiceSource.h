#pragma once
#include <Runtime/ScrVmVoiceHandler.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct SoundManagerNativeBody50h;
class SoundManagerNativePlayback;
class ISoundManagerNativeOpen;
class RuntimeCStringArrayReference;

// 4D333F/50929F固定manager正文和正文外服务；不复制数组，不增持声音对象。
class ScriptVoiceManagerReceiver final
{
public:
    ScriptVoiceManagerReceiver(IScrVmVoiceRuntimeBoundary*) noexcept;
    static ScriptVoiceManagerReceiver Native(SoundManagerNativeBody50h*,
        SoundManagerNativePlayback&, ISoundManagerNativeOpen&) noexcept;
    explicit operator bool() const noexcept { return object_ != nullptr; }
    void* Object() const noexcept { return object_; }
    void StopAndClearAllVoiceSlots() const;
    bool PlayBatchAt42C4F0(const RuntimeCStringArrayReference&) const;
    bool PlayBatchRawAt42C4F0(const YtlReferenceSlotRuntime*) const;
private:
    struct Operations
    {
        void (*stop)(void*, SoundManagerNativePlayback*);
        bool (*batch)(void*, SoundManagerNativePlayback*, ISoundManagerNativeOpen*, const RuntimeCStringArrayReference&);
        bool (*rawBatch)(void*, SoundManagerNativePlayback*, ISoundManagerNativeOpen*, const YtlReferenceSlotRuntime*);
    };
    ScriptVoiceManagerReceiver(void* object, SoundManagerNativePlayback* playback,
        ISoundManagerNativeOpen* open, const Operations& operations) noexcept
        : object_(object), playback_(playback), open_(open), operations_(&operations) {}
    void* object_;
    SoundManagerNativePlayback* playback_;
    ISoundManagerNativeOpen* open_;
    const Operations* operations_;
};
class IScriptVoiceSource
{
public:
    virtual ~IScriptVoiceSource() = default;
    virtual ScriptVoiceManagerReceiver CaptureVoiceManagerAfterEnsureAt413BE0() = 0;
};

class RootScriptVoiceSource final : public IScriptVoiceSource
{
public:
    explicit RootScriptVoiceSource(RootRuntimeOwner& owner) noexcept : owner_(owner) {}
    ScriptVoiceManagerReceiver CaptureVoiceManagerAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& owner_;
};
}
