#include <Runtime/ScriptMusicSource.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SoundSlotManagerRuntime.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundManagerNativeMusic.h>

#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
ScriptMusicManagerReceiver::ScriptMusicManagerReceiver(IScrVmMusicRuntimeBoundary* object) noexcept
    : object_(object), service_(nullptr)
{
    static const Operations operations{
        [](void* p, void*) { return static_cast<IScrVmMusicRuntimeBoundary*>(p)->HasPlaybackSlots(); },
        [](void* p, void*, const char* name, std::int32_t value) {
            auto& manager = *static_cast<IScrVmMusicRuntimeBoundary*>(p);
            if (!manager.IsCurrentSlotActive()) return;
            if (!name) throw ATL::CAtlException(E_FAIL);
            if (_mbsicmp(reinterpret_cast<const unsigned char*>(manager.CurrentSlotResourceName()),
                reinterpret_cast<const unsigned char*>(name)) != 0) manager.SwitchCurrentSlot(name, value);
        },
        [](void* p, void*) { static_cast<IScrVmMusicRuntimeBoundary*>(p)->StopMusicSlots(); }};
    operations_ = &operations;
}
ScriptMusicManagerReceiver ScriptMusicManagerReceiver::Native(SoundManagerNativeBody50h* body,
    SoundManagerNativeMusic& service) noexcept
{
    static const Operations operations{
        [](void* p, void* s) { return static_cast<SoundManagerNativeMusic*>(s)->HasSlotsAt4D5B1A(*static_cast<SoundManagerNativeBody50h*>(p)); },
        [](void* p, void* s, const char* name, std::int32_t value) {
            (void)static_cast<SoundManagerNativeMusic*>(s)->ApplyActiveAt4D5B28(*static_cast<SoundManagerNativeBody50h*>(p), name, value, 1);
        },
        [](void* p, void* s) { static_cast<SoundManagerNativeMusic*>(s)->StopCurrentAt4D5A39(*static_cast<SoundManagerNativeBody50h*>(p)); }};
    return {body, &service, operations};
}
bool ScriptMusicManagerReceiver::HasPlaybackSlots() const
{ return operations_->hasSlots(object_, service_); }
void ScriptMusicManagerReceiver::ApplyActiveName(const char* name, std::int32_t value) const
{ operations_->apply(object_, service_, name, value); }
void ScriptMusicManagerReceiver::StopMusicSlots() const
{ operations_->stop(object_, service_); }

RootScriptMusicSource::RootScriptMusicSource(EnvironmentRegistryRuntime& environment,
    RootRuntimeOwner& root) noexcept : environment_(environment), root_(root) {}

ScriptMusicParameterReceiver RootScriptMusicSource::CaptureParametersAt1C()
{
    return {ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(environment_.SaveData().soundParamsAt1C),
        [](void* p) -> const std::uint32_t* {
            if (!p) throw std::logic_error("MUSIC null SoundParams (modern safety boundary)");
            return static_cast<EnvironmentSoundParamsArchive*>(p)->MusicVolumeAt14();
        }};
}
ScriptMusicManagerReceiver RootScriptMusicSource::CaptureSoundManagerAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.RequirePublished().PostWindow().MusicReceiverAt24();
}

void ApplyScriptMusicAt4D59E0(const std::int32_t resourceId, IScriptMusicSource& source,
    const IScrResourceNameSource& names, IScrVmMusicNameWriter& writer)
{
    if (resourceId < 0)
    {
        // 4D59F5..4D5A1C先固定manager；清名重入不能让Stop转向新manager。
        const auto manager = source.CaptureSoundManagerAfterEnsureAt413BE0();
        writer.WriteCurrentMusicNameAt2C(nullptr);
        if (!manager) throw std::logic_error("MUSIC null SoundManager (modern safety boundary)");
        manager.StopMusicSlots();
        return;
    }
    // 4D5A59/4D5A7B/4D5AA6：裸借用三个receiver，不增持，也不提前读音量值。
    const auto parameters = source.CaptureParametersAt1C();
    const auto pattern = names.CapturePatternNames();
    const auto manager = source.CaptureSoundManagerAfterEnsureAt413BE0();
    if (!parameters.volumeAt14) throw std::logic_error("MUSIC volume reader is not configured");
    const auto* const volume = parameters.volumeAt14(parameters.object);
    if (!volume) throw std::logic_error("MUSIC null volume pointer (modern safety boundary)");
    const auto value = std::bit_cast<std::int32_t>(*volume);
    const char* const name = pattern.FindNameAt4AD1B0(resourceId);
    writer.WriteCurrentMusicNameAt2C(name);
    if (!manager) throw std::logic_error("MUSIC null SoundManager (modern safety boundary)");
    if (manager.HasPlaybackSlots()) manager.ApplyActiveName(name, value);
}
}
