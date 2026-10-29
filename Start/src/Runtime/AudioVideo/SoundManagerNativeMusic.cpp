#include <Runtime/SoundManagerNativeMusic.h>
#include <bit>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SoundManagerNativeBody50h;
std::uintptr_t Slots(Body& body)
{ return reinterpret_cast<std::uintptr_t>(static_cast<volatile Body&>(body).slotsBeginAt04); }
std::int32_t Handle(Body& body, std::uint32_t byteOffset)
{ return *reinterpret_cast<const volatile std::int32_t*>(Slots(body) + byteOffset); }
RuntimeCString& Name(Body& body, std::uint32_t byteOffset)
{ return *reinterpret_cast<RuntimeCString*>(Slots(body) + byteOffset + 4U); }
std::uint32_t Bytes(Body& body)
{
    auto& b = static_cast<volatile Body&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(b.slotsEndAt08);
    const auto begin = Slots(body);
    return static_cast<std::uint32_t>(end - begin);
}
std::int32_t SlotCount(Body& body)
{
    const auto bytes = Bytes(body);
    return std::bit_cast<std::int32_t>((bytes >> 3) | (bytes & 0x80000000U ? 0xE0000000U : 0U));
}
EnvironmentSoundParamsArchive* InlineParamsAt4BCCF7(Body& body)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24);
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    return reinterpret_cast<EnvironmentSoundParamsArchive*>(base + ref.biasAt04 * stride);
}
}
SoundManagerNativeMusic::SoundManagerNativeMusic(SoundModuleNativeApi& sound, ISoundManagerNativeOpen& open) noexcept
    : sound_(sound), open_(open) {}
bool SoundManagerNativeMusic::IsAbsolutePlayingAt419370(Body& body, std::uint32_t slot)
{ return sound_.IsPlaying(Handle(body, slot * 8U)); }
bool SoundManagerNativeMusic::StopAbsoluteAt4383B0(Body& body, std::uint32_t slot)
{
    const auto bytes = slot * 8U;
    const auto handle = Handle(body, bytes);
    sound_.StopAt4385F1(handle);
    SetRuntimeCStringAt437680(Name(body, bytes), "", 0); // 原offset固定，slots起址晚读。
    return true;
}
void SoundManagerNativeMusic::StopMusicAt4C3500(Body& body, RuntimeCString& environmentName)
{
    SetRuntimeCStringAt437680(environmentName, "", 0);
    StopAbsoluteAt4383B0(body, static_cast<volatile Body&>(body).musicAt10);
}
bool SoundManagerNativeMusic::PlayMusicAt4BCCC0(Body& body, RuntimeCString& environmentName,
    const char* name, std::int32_t value, std::int32_t loop)
{
    const auto bytes = name ? static_cast<std::uint32_t>(std::strlen(name)) : 0U;
    SetRuntimeCStringAt437680(environmentName, name, std::bit_cast<std::int32_t>(bytes));
    if (!HasSlotsAt4D5B1A(body)) return true;
    return ApplyActiveAt4D5B28(body, name, value, loop);
}
bool SoundManagerNativeMusic::HasSlotsAt4D5B1A(Body& body)
{ return (Bytes(body) & 0xFFFFFFF8U) != 0; }
void SoundManagerNativeMusic::StopCurrentAt4D5A39(Body& body)
{ (void)StopAbsoluteAt4383B0(body, static_cast<volatile Body&>(body).musicAt10); }
bool SoundManagerNativeMusic::ApplyActiveAt4D5B28(Body& body, const char* name,
    std::int32_t value, std::int32_t loop)
{
    // 4BCCF7..4BCD80与4D5B28..4D5B7E有相同的内联Ref/active/当前槽/空名/比较顺序。
    // caller分别已提交环境名；本方法不重复提交，也不重取资格前的Environment receiver。
    auto* const parameters = InlineParamsAt4BCCF7(body);
    using Active = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*);
    const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(parameters);
    if (!reinterpret_cast<Active>(table[0x18 / 4])(parameters)) return true;
    auto& current = static_cast<volatile Body&>(body);
    const auto offset = current.musicAt10 * 8U;
    const auto slotAddress = Slots(body) + offset;
    if (!name) throw ATL::CAtlException(E_FAIL); // 原空名拒绝晚于环境写入和active虚调用。
    const char* const existing = reinterpret_cast<SoundManagerNativeSlot8h*>(slotAddress)->nameAt04.GetString();
    if (_mbsicmp(reinterpret_cast<const unsigned char*>(existing), reinterpret_cast<const unsigned char*>(name)) == 0) return true;
    return open_.OpenAt42BC90(body, current.musicAt10, name, value, loop, 0);
}
void SoundManagerNativeMusic::StopEffectsAt413270(Body& body, EnvironmentVectorStorage<RuntimeCString>* mappings)
{
    if (mappings)
    {
        auto& current = static_cast<volatile EnvironmentVectorStorage<RuntimeCString>&>(*mappings);
        auto at = reinterpret_cast<std::uintptr_t>(current.begin);
        while (at != reinterpret_cast<std::uintptr_t>(current.end))
        {
            SetRuntimeCStringAt437680(*reinterpret_cast<RuntimeCString*>(at), "", 0);
            at += 4U; // 原begin只取一次，end在每项之后重读。
        }
    }
    auto count = SlotCount(body);
    auto index = static_cast<volatile Body&>(body).firstEffectsBeginAt18;
    while (std::bit_cast<std::int32_t>(index) < count)
    {
        StopAbsoluteAt4383B0(body, index);
        ++index; count = SlotCount(body);
    }
}
}
