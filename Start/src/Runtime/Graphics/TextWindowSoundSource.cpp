#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SoundManagerNativeVolume.h>
#include <Runtime/SoundSlotManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
TextSoundVolumeReceiver TextSoundVolumeReceiver::Native(SoundManagerNativeBody50h* body, SoundManagerNativeVolume& service) noexcept
{
    return {body, &service,
        [](void* p, void* v) { static_cast<SoundManagerNativeVolume*>(v)->AdjustAt4C2410(*static_cast<SoundManagerNativeBody50h*>(p)); },
        [](void* p, void* v) { static_cast<SoundManagerNativeVolume*>(v)->RefreshAt433F60(*static_cast<SoundManagerNativeBody50h*>(p)); }};
}
TextSoundVolumeReceiver TextSoundVolumeReceiver::Modern(SoundSlotManagerRuntime* body) noexcept
{
    return {body, nullptr,
        [](void* p, void*) { auto& b = *static_cast<SoundSlotManagerRuntime*>(p); b.AdjustFirstEffectsAt4C2410(b.VoiceRelationsAt24()); },
        [](void* p, void*) { auto& b = *static_cast<SoundSlotManagerRuntime*>(p); b.RefreshFirstEffectsAt433F60(b.VoiceRelationsAt24()); }};
}
TextSoundVoiceGateReceiver TextSoundVoiceGateReceiver::Native(EnvironmentSoundParamsArchive* object)
{
    const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(object);
    return {object, reinterpret_cast<Function>(table[0x60 / 4])};
}
TextSoundVoiceGateTableReceiver TextSoundVoiceGateTableReceiver::Native(EnvironmentSoundParamsArchive* object)
{
    return {object,*reinterpret_cast<const std::uintptr_t* const volatile*>(object)};
}
bool TextSoundVoiceGateTableReceiver::Allowed(const YtlReferenceSlotRuntime* names) const
{
    const auto entry=static_cast<const volatile std::uintptr_t*>(capturedVtable)[0x60/4];
    return reinterpret_cast<TextSoundVoiceGateReceiver::Function>(entry)(object,names)!=0;
}
EnvironmentTextWindowSoundSource::EnvironmentTextWindowSoundSource(EnvironmentRootOwner& environment, RootRuntimeOwner& root) noexcept
    : environment_(environment), root_(root) {}
TextSoundRootReceiver EnvironmentTextWindowSoundSource::CaptureRootAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {root_.Published(), [](void* p) {
        return static_cast<RootRuntimePayload*>(p)->PostWindow().TextSoundReceiverAt24();
    }};
}
TextSoundVoiceGateReceiver EnvironmentTextWindowSoundSource::CaptureParametersAt4188C0()
{
    auto* const save = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
    return TextSoundVoiceGateReceiver::Native(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(save->soundParamsAt1C));
}
const YtlReferenceSlotRuntime* EnvironmentTextWindowSoundSource::CaptureVoicesSlotAt3C()
{
    auto* const unsave = static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    return &unsave->ArrayReferenceAt3C().RawSlot();
}
EnvironmentVectorStorage<RuntimeCString>* EnvironmentTextWindowSoundSource::CaptureMappingsAt30()
{
    auto* const unsave = static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    return &unsave->mappingsAt30_;
}
}
