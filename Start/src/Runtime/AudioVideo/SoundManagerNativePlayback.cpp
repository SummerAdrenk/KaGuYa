#include <Runtime/SoundManagerNativePlayback.h>

namespace KaGuYa::Start::Runtime
{
std::int32_t ReadSoundVoiceCountNative(ParamsRuntimeRootOwner& params)
{
    auto* const root = params.CaptureAt418480();
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(root->gameSystemAt04.Native());
    const auto base = ref.baseAt00;
    ParamsGameSystem* gameSystem = nullptr;
    if (base)
    {
        auto* const control = ref.controlAt08;
        const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
            reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
        const auto bias = ref.biasAt04;
        gameSystem = reinterpret_cast<ParamsGameSystem*>(base + bias * stride);
    }
    return *reinterpret_cast<const volatile std::int32_t*>(
        reinterpret_cast<const std::byte*>(gameSystem) + 0x4C);
}
SoundManagerNativePlayback::SoundManagerNativePlayback(ParamsRuntimeRootOwner& params,
    SoundModuleNativeApi& sound) noexcept : params_(params), sound_(sound) {}

bool SoundManagerNativePlayback::StopAndClearAt4384D0(SoundManagerNativeBody50h& body)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    for (std::int32_t i = 0; i < ReadSoundVoiceCountNative(params_); ++i)
    {
        const auto byteOffset = (current.voicesBeginAt14 + static_cast<std::uint32_t>(i)) * 8U;
        const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
        const auto handle = *reinterpret_cast<const volatile std::int32_t*>(begin + byteOffset);
        sound_.StopAt4385F1(handle);
        // 438696..4386A6保留旧byteOffset，但在Sound/CString回调后重新取slots起址。
        // module.base为空仍清名；失败不清本项、不回滚前面已清项。
        auto* const name = reinterpret_cast<RuntimeCString*>(
            reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04) + byteOffset + 4U);
        SetRuntimeCStringAt437680(*name, "", 0);
    }
    return true;
}
}
