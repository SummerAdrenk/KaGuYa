#pragma once

#include <Runtime/ScrVmSoundHandler.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;
class RootRuntimeOwner;
class IScriptSoundGroupSource;
struct SoundManagerNativeBody50h;
class SoundManagerNativeMusic;
class SoundManagerNativeVolume;
class SoundManagerNativeEffects;

// 借用已捕获的manager，不通过全局根重取，也不额外保活旧正文。
class IScriptSoundLeafManager
{
public:
    virtual ~IScriptSoundLeafManager() = default;
    virtual void StopEffectsKeepingMappingsAt413270() = 0;
    virtual void StopAndClearMappedSlotAtPort(std::size_t port) = 0;
    virtual void StopAndClearRemainingSlot(std::size_t index) = 0;
    virtual bool AnyVoiceSlotPlaying() const = 0;
    virtual ScrVmSoundOpenResult RestoreFirstMappedEffectRaw(
        std::size_t index, const char* name, bool voice) = 0;
    virtual ScrVmSoundOpenResult RestoreSecondMappedEffectRaw(
        std::size_t index, const char* name, std::int32_t volume) = 0;
    virtual ScrVmSoundOpenResult OpenRemainingSlot(
        std::size_t index, const char* name, std::int32_t volume) = 0;
};

// 固定借用manager正文，服务在正文外；忽略的AL不伪造Opened/Skipped分类。
class ScriptSoundManagerReceiver final
{
public:
    ScriptSoundManagerReceiver(IScriptSoundLeafManager*) noexcept;
    static ScriptSoundManagerReceiver Native(SoundManagerNativeBody50h*,
        SoundManagerNativeMusic&, SoundManagerNativeVolume&, SoundManagerNativeEffects&) noexcept;
    explicit operator bool() const noexcept { return object_ != nullptr; }
    void* Object() const noexcept { return object_; }
    void StopEffectsKeepingMappingsAt413270() const;
    void StopEffectsAt413270(EnvironmentVectorStorage<RuntimeCString>* mappings) const;
    void StopAndClearMappedSlotAtPort(std::uint32_t) const;
    void StopAndClearRemainingSlot(std::uint32_t) const;
    bool AnyVoiceSlotPlaying() const;
    void RestoreFirstMappedEffectRaw(std::uint32_t, const char*, bool) const;
    void RestoreSecondMappedEffectRaw(std::uint32_t, const char*, std::int32_t) const;
    bool OpenRemainingSlot(std::uint32_t, const char*, std::int32_t) const;
private:
    struct Operations
    {
        void (*stopEffects)(void*, SoundManagerNativeMusic*);
        void (*stopWithMappings)(void*, SoundManagerNativeMusic*, EnvironmentVectorStorage<RuntimeCString>*);
        void (*stopMapped)(void*, SoundManagerNativeMusic*, std::uint32_t);
        void (*stopRemaining)(void*, SoundManagerNativeMusic*, std::uint32_t);
        bool (*voice)(void*, SoundManagerNativeVolume*);
        void (*first)(void*, SoundManagerNativeEffects*, std::uint32_t, const char*, bool);
        void (*second)(void*, SoundManagerNativeEffects*, std::uint32_t, const char*, std::int32_t);
        bool (*remaining)(void*, SoundManagerNativeEffects*, std::uint32_t, const char*, std::int32_t);
    };
    ScriptSoundManagerReceiver(void* p, SoundManagerNativeMusic* music, SoundManagerNativeVolume* volume,
        SoundManagerNativeEffects* effects, const Operations& operations) noexcept
        : object_(p), music_(music), volume_(volume), effects_(effects), operations_(&operations) {}
    void* object_;
    SoundManagerNativeMusic* music_;
    SoundManagerNativeVolume* volume_;
    SoundManagerNativeEffects* effects_;
    const Operations* operations_;
};

struct ScriptSoundRootReceiver final
{
    void* object;
    ScriptSoundManagerReceiver (*resolveAt4188F0)(void*);
    [[nodiscard]] ScriptSoundManagerReceiver Resolve() const;
};

class IScriptSoundLeafSource
{
public:
    virtual ~IScriptSoundLeafSource() = default;
    virtual ScriptSoundRootReceiver CaptureRootAfterEnsureAt413BE0() = 0;
    virtual EnvironmentVectorStorage<RuntimeCString>* CaptureMappingsAt30() = 0;
    virtual SoundMappingParameterReceiver CaptureSoundParametersAt1C() = 0;
};

class RootScriptSoundLeafSource final : public IScriptSoundLeafSource
{
public:
    RootScriptSoundLeafSource(EnvironmentRegistryRuntime& environment, RootRuntimeOwner& root) noexcept;
    ScriptSoundRootReceiver CaptureRootAfterEnsureAt413BE0() override;
    EnvironmentVectorStorage<RuntimeCString>* CaptureMappingsAt30() override;
    SoundMappingParameterReceiver CaptureSoundParametersAt1C() override;
private:
    EnvironmentRegistryRuntime& environment_;
    RootRuntimeOwner& root_;
};

// SOUND/TEXT/MENU共用的原叶入口；负ID清映射，正ID保留gate前后的独立取源。
void ApplyScriptFirstSoundAt4D6640(std::int32_t resourceId, std::uint32_t index,
    const IScrResourceNameSource& names, IScriptSoundLeafSource& source, IScrVmSoundOpenGate& gate);
void ApplyScriptSecondSoundAt4D62A0(std::int32_t resourceId, std::uint32_t index,
    const IScrResourceNameSource& names, IScriptSoundGroupSource& groups,
    IScriptSoundLeafSource& source, IScrVmSoundOpenGate& gate);
}
