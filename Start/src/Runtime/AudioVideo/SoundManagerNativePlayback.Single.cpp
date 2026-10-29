#include <Runtime/SoundManagerNativePlayback.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
EnvironmentSoundParamsArchive* InlineParamsAt4BCE84(SoundManagerNativeBody50h& body)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24);
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    return reinterpret_cast<EnvironmentSoundParamsArchive*>(base + ref.biasAt04 * stride);
}
template<class F>
F Slot(EnvironmentSoundParamsArchive* body, std::size_t offset)
{ return reinterpret_cast<F>((*reinterpret_cast<const std::uintptr_t* const*>(body))[offset / 4]); }
}
bool SoundManagerNativePlayback::PlaySingleAt4BCE10(SoundManagerNativeBody50h& body, const char* name,
    ISoundManagerNativeOpen& open, IRuntimeStringManagerSource& strings)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    if (static_cast<std::uint32_t>(end - begin) < 8U) return true;
    using Ready = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*);
    auto* params = ResolveSoundParamsAt418560(body);
    if (!Slot<Ready>(params, 0xA4)(params)) return true;
    using Allowed = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
    params = ResolveSoundParamsAt418560(body);
    if (!Slot<Allowed>(params, 0xA8)(params, name)) return true;
    using Value = std::int32_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
    params = InlineParamsAt4BCE84(body);
    auto value = Slot<Value>(params, 0xAC)(params, name);
    if (value < 0)
    {
        using Fallback = const std::int32_t* (__thiscall*)(EnvironmentSoundParamsArchive*);
        const std::int32_t* pointer;
        try
        {
            params = InlineParamsAt4BCE84(body);
            pointer = Slot<Fallback>(params, 0x6C)(params);
        }
        catch (...) { return false; }
        value = *pointer; // 4BCEE1撤state0；解引用在catch外，不能默认音量或吞非法访问。
        if (value < 0) return false;
    }
    auto held = ArchiveResourceNameAt40B530(name, strings); // 所有资格通过后才复制原输入。
    const bool success = open.OpenAt42BC90(body, current.voicesBeginAt14, held.Value().GetString(), value, 0, 0);
    held.ReleaseNormal(); // 4BCF31撤资格后正常Free；不因为Free失败重试持有名字。
    return success;
}
}
