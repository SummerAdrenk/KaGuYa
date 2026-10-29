#pragma once
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/SoundMappingParameterReceiver.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class ParamsRuntimeRootOwner;
struct EnvironmentRootStorage;

struct TextEffectEnvironmentReceiver final
{
    void* object;
    RuntimeCString* (*mappingAt30)(void*, std::uint32_t);
    SoundMappingParameterReceiver (*parametersAt1C)(void*);
    RuntimeCString* Mapping(std::uint32_t port) const { return mappingAt30(object, port); }
    SoundMappingParameterReceiver Parameters() const { return parametersAt1C(object); }
    static TextEffectEnvironmentReceiver Native(EnvironmentRootStorage*) noexcept;
};
class ITextWindowEffectRestoreSource
{
public:
    virtual ~ITextWindowEffectRestoreSource() = default;
    virtual const ParamsGameSystem* CaptureGameInlineAt4BC350() = 0;
    virtual TextEffectEnvironmentReceiver CaptureEnvironmentAt4BC379() = 0;
    virtual TextSoundRootReceiver CaptureRootAfter413BE0() = 0;
    virtual const char* CaptureMappingNameVia417990(std::uint32_t port) = 0;
    virtual SoundMappingParameterReceiver CaptureParametersVia417990() = 0;
    virtual std::int32_t MappingCountVia417990() = 0;
    virtual bool MappingEmptyVia417990(std::uint32_t port) = 0;
    virtual const ParamsGameSystem* CaptureGameGetterAt4BC4AE() = 0;
};
class EnvironmentTextWindowEffectRestoreSource final : public ITextWindowEffectRestoreSource
{
public:
    EnvironmentTextWindowEffectRestoreSource(EnvironmentRootOwner&, ParamsRuntimeRootOwner&, ITextWindowSoundSource&) noexcept;
    const ParamsGameSystem* CaptureGameInlineAt4BC350() override;
    TextEffectEnvironmentReceiver CaptureEnvironmentAt4BC379() override;
    TextSoundRootReceiver CaptureRootAfter413BE0() override;
    const char* CaptureMappingNameVia417990(std::uint32_t) override;
    SoundMappingParameterReceiver CaptureParametersVia417990() override;
    std::int32_t MappingCountVia417990() override;
    bool MappingEmptyVia417990(std::uint32_t) override;
    const ParamsGameSystem* CaptureGameGetterAt4BC4AE() override;
private:
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
    ITextWindowSoundSource& sound_;
};

// 4BC34B..4BC4EF两个不同循环；不包含WaitMark的root取源或窗口状态副作用。
void RestoreTextWindowEffectsAt4BC34B(ITextWindowEffectRestoreSource&);
}
