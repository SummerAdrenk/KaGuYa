#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SoundManagerNativeMusic.h>
#include <Runtime/SoundManagerNativeEffects.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ClearCapturedMappings(EnvironmentVectorStorage<RuntimeCString>* mappings)
{
    if (!mappings) return;
    auto& current = static_cast<volatile EnvironmentVectorStorage<RuntimeCString>&>(*mappings);
    auto at = reinterpret_cast<std::uintptr_t>(current.begin);
    while (at != reinterpret_cast<std::uintptr_t>(current.end))
    {
        SetRuntimeCStringAt437680(*reinterpret_cast<RuntimeCString*>(at), "", 0);
        at += 4U;
    }
}
}
ScriptSoundManagerReceiver::ScriptSoundManagerReceiver(IScriptSoundLeafManager* object) noexcept
    : object_(object), music_(nullptr), volume_(nullptr), effects_(nullptr)
{
    static const Operations operations{
        [](void* p, SoundManagerNativeMusic*) { static_cast<IScriptSoundLeafManager*>(p)->StopEffectsKeepingMappingsAt413270(); },
        [](void* p, SoundManagerNativeMusic*, EnvironmentVectorStorage<RuntimeCString>* mappings) {
            // 413270固定caller提供的begin，逐项晚读end；不重新取得manager绑定的映射。
            ClearCapturedMappings(mappings);
            static_cast<IScriptSoundLeafManager*>(p)->StopEffectsKeepingMappingsAt413270();
        },
        [](void* p, SoundManagerNativeMusic*, std::uint32_t i) { static_cast<IScriptSoundLeafManager*>(p)->StopAndClearMappedSlotAtPort(i); },
        [](void* p, SoundManagerNativeMusic*, std::uint32_t i) { static_cast<IScriptSoundLeafManager*>(p)->StopAndClearRemainingSlot(i); },
        [](void* p, SoundManagerNativeVolume*) { return static_cast<IScriptSoundLeafManager*>(p)->AnyVoiceSlotPlaying(); },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* name, bool voice) {
            (void)static_cast<IScriptSoundLeafManager*>(p)->RestoreFirstMappedEffectRaw(i, name, voice);
        },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* name, std::int32_t value) {
            (void)static_cast<IScriptSoundLeafManager*>(p)->RestoreSecondMappedEffectRaw(i, name, value);
        },
        [](void* p, SoundManagerNativeEffects*, std::uint32_t i, const char* name, std::int32_t value) {
            return static_cast<IScriptSoundLeafManager*>(p)->OpenRemainingSlot(i, name, value) != ScrVmSoundOpenResult::Failed;
        }};
    operations_ = &operations;
}
ScriptSoundManagerReceiver ScriptSoundManagerReceiver::Native(SoundManagerNativeBody50h* body,
    SoundManagerNativeMusic& music, SoundManagerNativeVolume& volume, SoundManagerNativeEffects& effects) noexcept
{
    static const Operations operations{
        [](void* p, SoundManagerNativeMusic* service) {
            service->StopEffectsAt413270(*static_cast<SoundManagerNativeBody50h*>(p), nullptr);
        },
        [](void* p, SoundManagerNativeMusic* service, EnvironmentVectorStorage<RuntimeCString>* mappings) {
            service->StopEffectsAt413270(*static_cast<SoundManagerNativeBody50h*>(p), mappings);
        },
        [](void* p, SoundManagerNativeMusic* service, std::uint32_t i) {
            auto& body = *static_cast<SoundManagerNativeBody50h*>(p);
            (void)service->StopAbsoluteAt4383B0(body, static_cast<volatile SoundManagerNativeBody50h&>(body).firstEffectsBeginAt18 + i);
        },
        [](void* p, SoundManagerNativeMusic* service, std::uint32_t i) {
            auto& body = *static_cast<SoundManagerNativeBody50h*>(p);
            (void)service->StopAbsoluteAt4383B0(body, static_cast<volatile SoundManagerNativeBody50h&>(body).remainingEffectsBeginAt20 + i);
        },
        [](void* p, SoundManagerNativeVolume* service) { return service->AnyVoicePlayingAt4196C0(*static_cast<SoundManagerNativeBody50h*>(p)); },
        [](void* p, SoundManagerNativeEffects* service, std::uint32_t i, const char* name, bool voice) {
            (void)service->RestoreFirstAt42C1B0(*static_cast<SoundManagerNativeBody50h*>(p), name, i, static_cast<std::uint8_t>(voice));
        },
        [](void* p, SoundManagerNativeEffects* service, std::uint32_t i, const char* name, std::int32_t value) {
            (void)service->RestoreSecondAt42C070(*static_cast<SoundManagerNativeBody50h*>(p), name, i, value);
        },
        [](void* p, SoundManagerNativeEffects* service, std::uint32_t i, const char* name, std::int32_t value) {
            return service->OpenRemainingAt4D1AA8(*static_cast<SoundManagerNativeBody50h*>(p), name, i, value);
        }};
    return {body, &music, &volume, &effects, operations};
}
void ScriptSoundManagerReceiver::StopEffectsKeepingMappingsAt413270() const
{ operations_->stopEffects(object_, music_); }
void ScriptSoundManagerReceiver::StopEffectsAt413270(EnvironmentVectorStorage<RuntimeCString>* mappings) const
{ operations_->stopWithMappings(object_, music_, mappings); }
void ScriptSoundManagerReceiver::StopAndClearMappedSlotAtPort(std::uint32_t i) const
{ operations_->stopMapped(object_, music_, i); }
void ScriptSoundManagerReceiver::StopAndClearRemainingSlot(std::uint32_t i) const
{ operations_->stopRemaining(object_, music_, i); }
bool ScriptSoundManagerReceiver::AnyVoiceSlotPlaying() const
{ return operations_->voice(object_, volume_); }
void ScriptSoundManagerReceiver::RestoreFirstMappedEffectRaw(std::uint32_t i, const char* name, bool voice) const
{ operations_->first(object_, effects_, i, name, voice); }
void ScriptSoundManagerReceiver::RestoreSecondMappedEffectRaw(std::uint32_t i, const char* name, std::int32_t value) const
{ operations_->second(object_, effects_, i, name, value); }
bool ScriptSoundManagerReceiver::OpenRemainingSlot(std::uint32_t i, const char* name, std::int32_t value) const
{ return operations_->remaining(object_, effects_, i, name, value); }

ScriptSoundManagerReceiver ScriptSoundRootReceiver::Resolve() const
{
    if (!object || !resolveAt4188F0)
        throw std::logic_error("SOUND null root receiver (modern safety boundary)");
    return resolveAt4188F0(object);
}
RootScriptSoundLeafSource::RootScriptSoundLeafSource(
    EnvironmentRegistryRuntime& environment, RootRuntimeOwner& root) noexcept
    : environment_(environment), root_(root) {}
ScriptSoundRootReceiver RootScriptSoundLeafSource::CaptureRootAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {root_.Published(), [](void* p) -> ScriptSoundManagerReceiver {
        return static_cast<RootRuntimePayload*>(p)->PostWindow().SoundLeafReceiverAt24();
    }};
}
EnvironmentVectorStorage<RuntimeCString>* RootScriptSoundLeafSource::CaptureMappingsAt30()
{
    return &environment_.UnSaveData().mappingsAt30_;
}
SoundMappingParameterReceiver RootScriptSoundLeafSource::CaptureSoundParametersAt1C()
{
    return SoundMappingParameterReceiver::Native(
        ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(environment_.SaveData().soundParamsAt1C));
}
}
