#include <Runtime/SoundMappingRestoreSource.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SoundManagerNativeEffects.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireReceiver(const void* value)
{
    if (!value) throw std::logic_error("46D420 calls absent SoundManager (modern safety boundary)");
}
const RuntimeCString& Mapping(EnvironmentRegistryRuntime& registry, std::uint32_t port)
{
    const auto table = registry.UnSaveData().MappingsAt30();
    if (port >= table.size()) throw std::out_of_range("46D420 mapping index (modern safety boundary)");
    return table[port];
}
}
SoundMappingSlotReceiver::SoundMappingSlotReceiver(ISoundMappingRestoreSlotBoundary& fixture) noexcept
    : context_(&fixture)
{
    static const Operations operations{
        [](void* p, SoundManagerNativeVolume*) { return static_cast<ISoundMappingRestoreSlotBoundary*>(p)->AnyVoiceSlotPlaying(); },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* n, bool v) {
            if (!n) throw std::logic_error("fixture received null sound name");
            (void)static_cast<ISoundMappingRestoreSlotBoundary*>(p)->RestoreFirstMappedEffect(i, n, v);
        },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* n, std::int32_t v) {
            if (!n) throw std::logic_error("fixture received null sound name");
            (void)static_cast<ISoundMappingRestoreSlotBoundary*>(p)->RestoreSecondMappedEffect(i, n, v);
        }};
    operations_ = &operations;
}
SoundMappingSlotReceiver SoundMappingSlotReceiver::Native(SoundSlotManagerRuntime* manager) noexcept
{
    static const Operations operations{
        [](void* p, SoundManagerNativeVolume*) { return static_cast<SoundSlotManagerRuntime*>(p)->AnyVoiceSlotPlaying(); },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* n, bool v) {
            (void)static_cast<SoundSlotManagerRuntime*>(p)->RestoreFirstMappedEffectRaw(i, n, v);
        },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* n, std::int32_t v) {
            (void)static_cast<SoundSlotManagerRuntime*>(p)->RestoreSecondMappedEffectRaw(i, n, v);
        }};
    return {manager, operations};
}
SoundMappingSlotReceiver SoundMappingSlotReceiver::Native(SoundManagerNativeBody50h* manager,
    SoundManagerNativeVolume& volume, SoundManagerNativeEffects& effects) noexcept
{
    static const Operations operations{
        [](void* p, SoundManagerNativeVolume* service) {
            return service->AnyVoicePlayingAt4196C0(*static_cast<SoundManagerNativeBody50h*>(p));
        },
        [](void* p, SoundManagerNativeEffects* service, std::uint32_t i, const char* name, bool voice) {
            (void)service->RestoreFirstAt42C1B0(*static_cast<SoundManagerNativeBody50h*>(p), name, i, static_cast<std::uint8_t>(voice));
        },
        [](void* p, SoundManagerNativeEffects* service, std::uint32_t i, const char* name, std::int32_t value) {
            (void)service->RestoreSecondAt42C070(*static_cast<SoundManagerNativeBody50h*>(p), name, i, value);
        }};
    SoundMappingSlotReceiver result(manager, operations);
    result.volume_ = &volume; result.effects_ = &effects;
    return result;
}
bool SoundMappingSlotReceiver::AnyVoiceSlotPlaying() const
{ RequireReceiver(context_); return operations_->voice(context_, volume_); }
void SoundMappingSlotReceiver::RestoreFirst(std::uint32_t i, const char* n, bool v) const
{ RequireReceiver(context_); operations_->first(context_, effects_, i, n, v); }
void SoundMappingSlotReceiver::RestoreSecond(std::uint32_t i, const char* n, std::int32_t v) const
{ RequireReceiver(context_); operations_->second(context_, effects_, i, n, v); }

EnvironmentSoundMappingRestoreSource::EnvironmentSoundMappingRestoreSource(
    EnvironmentRegistryRuntime& registry, ParamsRuntimeRootOwner& params, RootRuntimeOwner& root) noexcept
    : registry_(registry), params_(params), root_(root) {}
std::int32_t EnvironmentSoundMappingRestoreSource::MappingCountAt46D778()
{
    // 46D778..46D77E：32位地址差按有符号数算术右移，随后jge。
    const auto bytes = static_cast<std::uint32_t>(registry_.UnSaveData().Size()) * 4u;
    return std::bit_cast<std::int32_t>(bytes) >> 2;
}
bool EnvironmentSoundMappingRestoreSource::MappingEmptyAt46D8E1(std::uint32_t port)
{ return Mapping(registry_, port).GetLength() == 0; }
const ParamsGameSystem* EnvironmentSoundMappingRestoreSource::CaptureGameSystemAt418480()
{ return params_.GetOrCreateAt418480().GameSystem().get(); }
const char* EnvironmentSoundMappingRestoreSource::CaptureMappingName(std::uint32_t port)
{ return Mapping(registry_, port).GetString(); }
SoundMappingSlotReceiver EnvironmentSoundMappingRestoreSource::CaptureSoundManagerAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().SoundMappingReceiverAt24();
}
SoundMappingParameterReceiver EnvironmentSoundMappingRestoreSource::CaptureParametersAt1C()
{
    return SoundMappingParameterReceiver::Native(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(
        registry_.SaveData().soundParamsAt1C));
}
}
