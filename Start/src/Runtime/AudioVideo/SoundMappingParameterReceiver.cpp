#include <Runtime/SoundMappingParameterReceiver.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SoundMappingParameterReceiver::SoundMappingParameterReceiver(
    const ISoundMappingRestoreParameterSource& fixture) noexcept : context_(&fixture)
{
    static const Operations operations{
        [](const void* p) { return static_cast<const ISoundMappingRestoreParameterSource*>(p)->HasLoopEffectType(); },
        [](const void* p) { return static_cast<const ISoundMappingRestoreParameterSource*>(p)->LoopEffectValueOrThrow(); },
        [](const void* p) { return static_cast<const ISoundMappingRestoreParameterSource*>(p)->NormalEffectValueOrThrow(); }};
    operations_ = &operations;
}
SoundMappingParameterReceiver SoundMappingParameterReceiver::Native(EnvironmentSoundParamsArchive& archive) noexcept
{
    return Native(&archive);
}
SoundMappingParameterReceiver SoundMappingParameterReceiver::Native(EnvironmentSoundParamsArchive* archive) noexcept
{
    static const Operations operations{
        [](const void* p) {
            if (!p) throw std::logic_error("SoundParams +28 receiver absent (modern safety boundary)");
            return const_cast<EnvironmentSoundParamsArchive*>(static_cast<const EnvironmentSoundParamsArchive*>(p))->HasLoopEffectAt28(); },
        [](const void* p) {
            if (!p) throw std::logic_error("SoundParams +30 receiver absent (modern safety boundary)");
            return std::bit_cast<std::int32_t>(*const_cast<EnvironmentSoundParamsArchive*>(static_cast<const EnvironmentSoundParamsArchive*>(p))->LoopEffectVolumeAt30()); },
        [](const void* p) {
            if (!p) throw std::logic_error("SoundParams +24 receiver absent (modern safety boundary)");
            return std::bit_cast<std::int32_t>(*const_cast<EnvironmentSoundParamsArchive*>(static_cast<const EnvironmentSoundParamsArchive*>(p))->NormalEffectVolumeAt24()); }};
    return {archive, operations};
}
SoundMappingParameterReceiver SoundMappingParameterReceiver::RawNative(EnvironmentSoundParamsArchive* archive) noexcept
{
    static const Operations operations{
        [](const void* p) {
            using F = std::uint8_t (__thiscall*)(const void*);
            const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(p);
            return reinterpret_cast<F>(table[0x28 / 4])(p) != 0;
        },
        [](const void* p) {
            using F = const std::int32_t* (__thiscall*)(const void*);
            const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(p);
            return *static_cast<const volatile std::int32_t*>(reinterpret_cast<F>(table[0x30 / 4])(p));
        },
        [](const void* p) {
            using F = const std::int32_t* (__thiscall*)(const void*);
            const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(p);
            return *static_cast<const volatile std::int32_t*>(reinterpret_cast<F>(table[0x24 / 4])(p));
        }};
    return {archive, operations};
}
}
