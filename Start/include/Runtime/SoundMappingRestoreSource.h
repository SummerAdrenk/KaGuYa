#pragma once
#include <Runtime/SoundMappingParameterReceiver.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class SoundSlotManagerRuntime;
class ISoundMappingRestoreSlotBoundary;
class EnvironmentRegistryRuntime;
class ParamsRuntimeRootOwner;
class RootRuntimeOwner;
struct SoundManagerNativeBody50h;
class SoundManagerNativeVolume;
class SoundManagerNativeEffects;

// 46D420只捕获借用receiver，不增持、不检查空值；检查留在原调用发生的位置。
class SoundMappingSlotReceiver final
{
public:
    explicit SoundMappingSlotReceiver(ISoundMappingRestoreSlotBoundary& fixture) noexcept;
    static SoundMappingSlotReceiver Native(SoundSlotManagerRuntime* manager) noexcept;
    static SoundMappingSlotReceiver Native(SoundManagerNativeBody50h*,
        SoundManagerNativeVolume&, SoundManagerNativeEffects&) noexcept;
    bool AnyVoiceSlotPlaying() const;
    void RestoreFirst(std::uint32_t port, const char* name, bool voice) const;
    void RestoreSecond(std::uint32_t port, const char* name, std::int32_t volume) const;
    const void* Context() const noexcept { return context_; }
private:
    struct Operations
    {
        bool (*voice)(void*, SoundManagerNativeVolume*);
        void (*first)(void*, SoundManagerNativeEffects*, std::uint32_t, const char*, bool);
        void (*second)(void*, SoundManagerNativeEffects*, std::uint32_t, const char*, std::int32_t);
    };
    SoundMappingSlotReceiver(void* context, const Operations& operations) noexcept
        : context_(context), operations_(&operations) {}
    void* context_;
    const Operations* operations_;
    // 两个服务不拥有正文；各次捕获固定receiver，不能由回调之后的当前根替代。
    SoundManagerNativeVolume* volume_ = nullptr;
    SoundManagerNativeEffects* effects_ = nullptr;
};

class ISoundMappingRestoreSource
{
public:
    virtual ~ISoundMappingRestoreSource() = default;
    virtual std::int32_t MappingCountAt46D778() = 0;
    virtual bool MappingEmptyAt46D8E1(std::uint32_t port) = 0;
    virtual const ParamsGameSystem* CaptureGameSystemAt418480() = 0;
    virtual const char* CaptureMappingName(std::uint32_t port) = 0;
    virtual SoundMappingSlotReceiver CaptureSoundManagerAt413BE0() = 0;
    virtual SoundMappingParameterReceiver CaptureParametersAt1C() = 0;
};

class EnvironmentSoundMappingRestoreSource final : public ISoundMappingRestoreSource
{
public:
    EnvironmentSoundMappingRestoreSource(EnvironmentRegistryRuntime& registry,
        ParamsRuntimeRootOwner& params, RootRuntimeOwner& root) noexcept;
    std::int32_t MappingCountAt46D778() override;
    bool MappingEmptyAt46D8E1(std::uint32_t port) override;
    const ParamsGameSystem* CaptureGameSystemAt418480() override;
    const char* CaptureMappingName(std::uint32_t port) override;
    SoundMappingSlotReceiver CaptureSoundManagerAt413BE0() override;
    SoundMappingParameterReceiver CaptureParametersAt1C() override;
private:
    EnvironmentRegistryRuntime& registry_;
    ParamsRuntimeRootOwner& params_;
    RootRuntimeOwner& root_;
};
}
