#include <Runtime/SoundManagerNativeVolume.h>
#include <Runtime/SoundManagerNativePlayback.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SoundManagerNativeBody50h;
using Lower = SoundLowerVolumeNative10h;
std::uint32_t Stride(YtlReferenceControlRuntime* control)
{
    return control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
}
Lower* ResolveLower(const YtlReferenceSlotRuntime& input, std::uintptr_t base)
{
    if (!base) return nullptr;
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    const auto stride = Stride(ref.controlAt08);
    const auto bias = ref.biasAt04;
    return reinterpret_cast<Lower*>(base + bias * stride);
}
struct LowerPair final { Lower* value; Lower* index; };
LowerPair ResolveLowerPair(const YtlReferenceSlotRuntime& input)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    const auto base = ref.baseAt00;
    if (!base) return {nullptr, nullptr};
    auto* const control = ref.controlAt08;
    const auto firstStride = Stride(control);
    const auto bias = ref.biasAt04;
    auto* const first = reinterpret_cast<Lower*>(base + bias * firstStride);
    // 43409F..4340B2再次读同一control的stride，不能合并成一次解址。
    auto* const second = reinterpret_cast<Lower*>(base + bias * Stride(control));
    return {first, second};
}
std::int32_t SlotHandle(Body& body, std::uint32_t index)
{
    auto* const slots = static_cast<volatile Body&>(body).slotsBeginAt04;
    return reinterpret_cast<const volatile SoundManagerNativeSlot8h*>(
        reinterpret_cast<std::uintptr_t>(slots) + index * 8U)->handleAt00;
}
std::int32_t Half(std::int32_t value) noexcept
{
    const auto bits = std::bit_cast<std::uint32_t>(value);
    return std::bit_cast<std::int32_t>((bits >> 1) | (bits & 0x80000000U));
}
}
EnvironmentSoundParamsArchive* ResolveSoundParamsAt418560(Body& body)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24);
    const auto firstBase = ref.baseAt00;
    if (!firstBase) return nullptr;
    const auto stride = Stride(ref.controlAt08);
    const auto bias = ref.biasAt04;
    const auto finalBase = ref.baseAt00;
    return reinterpret_cast<EnvironmentSoundParamsArchive*>(finalBase + bias * stride);
}

SoundManagerNativeVolume::SoundManagerNativeVolume(ParamsRuntimeRootOwner& params,
    ISoundModuleNativeSource& modules, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32) noexcept
    : params_(params), sound_(modules, strings, win32) {}

std::int32_t SoundManagerNativeVolume::ReadVoiceCount()
{
    return ReadSoundVoiceCountNative(params_);
}

bool SoundManagerNativeVolume::IsPlaying(std::int32_t handle)
{
    return sound_.IsPlaying(handle);
}
void SoundManagerNativeVolume::SetVolume(std::int32_t handle, std::int32_t value)
{
    sound_.SetVolume(handle, 100, value);
}

bool SoundManagerNativeVolume::AnyVoicePlayingAt4196C0(Body& body)
{
    bool any = false;
    for (std::int32_t i = 0; i < ReadVoiceCount(); ++i)
    {
        const auto start = static_cast<volatile Body&>(body).voicesBeginAt14;
        const auto handle = SlotHandle(body, start + static_cast<std::uint32_t>(i));
        // 41988C只累积结果，已有播放声音也不能短路后续槽和Params捕获。
        if (IsPlaying(handle)) any = true;
    }
    return any;
}
std::uint8_t SoundManagerNativeVolume::ReadRelationMaskAt419470(Body& body,
    const RuntimeCString* relation)
{
    bool any = false;
    for (std::int32_t i = 0; i < ReadVoiceCount(); ++i)
    {
        const auto start = static_cast<volatile Body&>(body).voicesBeginAt14;
        const auto handle = SlotHandle(body, start + static_cast<std::uint32_t>(i));
        if (!IsPlaying(handle)) continue;
        any = true;
        auto* const parameters = ResolveSoundParamsAt418560(body);
        const auto newStart = static_cast<volatile Body&>(body).voicesBeginAt14;
        auto* const slots = static_cast<volatile Body&>(body).slotsBeginAt04;
        auto* const table = *reinterpret_cast<const std::uintptr_t* const*>(parameters);
        auto* const voice = reinterpret_cast<SoundManagerNativeSlot8h*>(
            reinterpret_cast<std::uintptr_t>(slots) + (newStart + static_cast<std::uint32_t>(i)) * 8U);
        const char* const voiceName = voice->nameAt04.GetString();
        using Match = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*, const char*);
        const auto matches = reinterpret_cast<Match>(table[0xB4 / 4]);
        const char* const relationName = relation->GetString();
        if (matches(parameters, relationName, voiceName)) return 3;
    }
    return any ? 1 : 0;
}

void SoundManagerNativeVolume::AdjustAt4C2410(Body& body)
{
    auto& current = static_cast<volatile Body&>(body);
    for (auto* item = current.lowerAt30.begin; item != current.lowerAt30.end; ++item)
    {
        auto* const initial = ResolveLower(*item, static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00);
        const auto index = static_cast<volatile Lower&>(*initial).slotAt00;
        if (!IsPlaying(SlotHandle(body, index))) continue;
        const auto policy = current.policyAt3C;
        if (policy == 1)
        {
            if (!AnyVoicePlayingAt4196C0(body)) continue;
            const auto pair = ResolveLowerPair(*item);
            const auto value = static_cast<volatile Lower&>(*pair.value).alternateAt04;
            const auto newIndex = static_cast<volatile Lower&>(*pair.index).slotAt00;
            SetVolume(SlotHandle(body, newIndex), value);
        }
        else if (policy == 0 || policy == 2)
        {
            auto* const lower = ResolveLower(*item, static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00);
            const auto mask = ReadRelationMaskAt419470(body, &lower->relationAt0C);
            if (!(mask & 1)) continue;
            const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00;
            auto value = static_cast<volatile Lower&>(*ResolveLower(*item, base)).alternateAt04;
            if (!(mask & 2))
            {
                if (!current.policyAt3C) continue;
                value = Half(static_cast<volatile Lower&>(*ResolveLower(*item, base)).resolvedAt08);
            }
            const auto newIndex = static_cast<volatile Lower&>(*ResolveLower(*item, base)).slotAt00;
            SetVolume(SlotHandle(body, newIndex), value);
        }
    }
}
void SoundManagerNativeVolume::RefreshAt433F60(Body& body)
{
    auto& current = static_cast<volatile Body&>(body);
    const bool any = AnyVoicePlayingAt4196C0(body);
    if (any && current.policyAt3C != 2) return;
    for (auto* item = current.lowerAt30.begin; item != current.lowerAt30.end; ++item)
    {
        auto* const initial = ResolveLower(*item, static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00);
        const auto index = static_cast<volatile Lower&>(*initial).slotAt00;
        if (!IsPlaying(SlotHandle(body, index))) continue;
        if (any)
        {
            auto* const lower = ResolveLower(*item, static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00);
            if ((ReadRelationMaskAt419470(body, &lower->relationAt0C) & 3) != 1) continue;
        }
        const auto pair = ResolveLowerPair(*item);
        auto value = static_cast<volatile Lower&>(*pair.value).resolvedAt08;
        const auto newIndex = static_cast<volatile Lower&>(*pair.index).slotAt00;
        if (any) value = Half(value);
        SetVolume(SlotHandle(body, newIndex), value);
    }
}
}
