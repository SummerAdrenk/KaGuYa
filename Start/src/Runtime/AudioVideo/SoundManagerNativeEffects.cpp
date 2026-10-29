#include <Runtime/SoundManagerNativeEffects.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <mbstring.h>

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
EnvironmentSoundParamsArchive* InlineParams(Body& body, std::uintptr_t capturedBase)
{
    if (!capturedBase) return nullptr;
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24);
    const auto stride = Stride(ref.controlAt08);
    return reinterpret_cast<EnvironmentSoundParamsArchive*>(capturedBase + ref.biasAt04 * stride);
}
template<class Function>
Function Slot(EnvironmentSoundParamsArchive* body, std::size_t offset)
{ return reinterpret_cast<Function>((*reinterpret_cast<const std::uintptr_t* const*>(body))[offset / 4]); }
Lower* ResolveLower(const YtlReferenceSlotRuntime& input, std::uintptr_t base)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    if (!base) return nullptr;
    const auto stride = Stride(ref.controlAt08);
    return reinterpret_cast<Lower*>(base + ref.biasAt04 * stride);
}
Lower* ResolveLower(const YtlReferenceSlotRuntime& input)
{ return ResolveLower(input, static_cast<const volatile YtlReferenceSlotRuntime&>(input).baseAt00); }
struct FoundLower final { YtlReferenceSlotRuntime* item; std::uintptr_t base; };
FoundLower FindLower(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end, std::uint32_t target)
{
    for (auto* item = begin; item != end; ++item)
    {
        const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*item).baseAt00;
        if (static_cast<volatile Lower&>(*ResolveLower(*item, base)).slotAt00 == target) return {item, base};
    }
    return {end, 0};
}
bool SameName(Body& body, std::uint32_t target, const char* name)
{
    const auto base = reinterpret_cast<std::uintptr_t>(static_cast<volatile Body&>(body).slotsBeginAt04);
    auto* const slot = reinterpret_cast<SoundManagerNativeSlot8h*>(base + target * 8U);
    if (!name) throw ATL::CAtlException(E_FAIL); // 42C228/42C11C：先计算槽地址，后拒绝空指针。
    return _mbsicmp(reinterpret_cast<const unsigned char*>(slot->nameAt04.GetString()),
        reinterpret_cast<const unsigned char*>(name)) == 0;
}
using Bool = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*);
using NamedBool = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
using NamedValue = std::int32_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
using ValuePointer = const std::int32_t* (__thiscall*)(EnvironmentSoundParamsArchive*);
}

bool SoundManagerNativeEffects::RestoreFirstAt42C1B0(Body& body, const char* name,
    std::uint32_t groupIndex, std::uint8_t restoreAlternate)
{
    auto* parameters = ResolveSoundParamsAt418560(body);
    if (!Slot<Bool>(parameters, 0xA4)(parameters)) return true;
    parameters = ResolveSoundParamsAt418560(body);
    if (!Slot<NamedBool>(parameters, 0xA8)(parameters, name)) return true;
    auto& current = static_cast<volatile Body&>(body);
    if (SameName(body, current.firstEffectsBeginAt18 + groupIndex, name)) return true;
    parameters = ResolveSoundParamsAt418560(body);
    auto value = Slot<NamedValue>(parameters, 0xAC)(parameters, name);
    if (value < 0)
    {
        const std::int32_t* pointer;
        try
        {
            parameters = ResolveSoundParamsAt418560(body);
            pointer = Slot<ValuePointer>(parameters, 0x24)(parameters);
        }
        catch (...) { return false; }
        value = *pointer; // 42C277已撤局部try；返回指针非法不属于fallback查询失败。
        if (value < 0) return false;
    }
    parameters = ResolveSoundParamsAt418560(body);
    using Relation = RuntimeCString* (__thiscall*)(EnvironmentSoundParamsArchive*, RuntimeCString*, const char*);
    ArchiveRawResourceNameRuntime relation(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* out) { (void)Slot<Relation>(parameters, 0xB0)(parameters, out, name); });
    const auto target = current.firstEffectsBeginAt18 + groupIndex;
    auto* const begin = current.lowerAt30.begin;
    auto* const end = current.lowerAt30.end;
    auto found = FindLower(begin, end, target);
    auto playbackValue = value;
    bool store = found.item != end;
    if (restoreAlternate && store)
    {
        playbackValue = static_cast<volatile Lower&>(*ResolveLower(*found.item, found.base)).alternateAt04;
        if (playbackValue < 0) { playbackValue = value; store = false; }
        else
        {
            // 42C43C..42C47C：第二次从原begin搜索，边界/target仍使用同一捕获值。
            found = FindLower(begin, end, target);
            store = found.item != end;
        }
    }
    if (store)
    {
        AssignRuntimeCStringAt410220(ResolveLower(*found.item, found.base)->relationAt0C, relation.Value());
        // 42C37B/42C4A6：赋值允许回调改变此Ref，再解引用当前正文写+08。
        static_cast<volatile Lower&>(*ResolveLower(*found.item)).resolvedAt08 = value;
    }
    const bool result = open_.OpenAt42BC90(body, current.firstEffectsBeginAt18 + groupIndex,
        name, playbackValue, 1, 0);
    relation.ReleaseNormal(); // 42C31C：先撤资格再释放；失败不再次展开同一临时。
    return result;
}

bool SoundManagerNativeEffects::RestoreSecondAt42C070(Body& body, const char* name,
    std::uint32_t groupIndex, std::int32_t fallback)
{
    auto& current = static_cast<volatile Body&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    if (!(static_cast<std::uint32_t>(end - begin) & 0xFFFFFFF8U)) return true;
    auto* parameters = InlineParams(body,
        static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24).baseAt00);
    const auto loop = Slot<Bool>(parameters, 0x28)(parameters);
    const auto nextBase = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24).baseAt00;
    parameters = InlineParams(body, nextBase);
    if (loop)
    {
        using Mute = const std::uint8_t* (__thiscall*)(EnvironmentSoundParamsArchive*);
        if (*Slot<Mute>(parameters, 0x2C)(parameters)) return true;
    }
    else if (!Slot<NamedBool>(parameters, 0x58)(parameters, name)) return true;
    if (SameName(body, current.secondEffectsBeginAt1C + groupIndex, name)) return true;
    parameters = InlineParams(body,
        static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24).baseAt00);
    const auto currentLoop = Slot<Bool>(parameters, 0x28)(parameters);
    parameters = ResolveSoundParamsAt418560(body);
    const auto value = currentLoop ? *Slot<ValuePointer>(parameters, 0x30)(parameters)
        : Slot<NamedValue>(parameters, 0x5C)(parameters, name);
    // 42C17C：fallback选中后不再判负，也没有局部catch。
    return open_.OpenAt42BC90(body, current.secondEffectsBeginAt1C + groupIndex,
        name, value < 0 ? fallback : value, 1, 0);
}

bool SoundManagerNativeEffects::OpenRemainingAt4D1AA8(Body& body, const char* name,
    std::uint32_t groupIndex, std::int32_t fallback)
{
    auto& current = static_cast<volatile Body&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    if (static_cast<std::uint32_t>(end - begin) < 8U) return true;
    auto* parameters = ResolveSoundParamsAt418560(body); // 4D1ACC晚读base，不套42C070固定base。
    if (!Slot<NamedBool>(parameters, 0x58)(parameters, name)) return true;
    parameters = ResolveSoundParamsAt418560(body);
    const auto value = Slot<NamedValue>(parameters, 0x5C)(parameters, name);
    return open_.OpenAt42BC90(body, current.remainingEffectsBeginAt20 + groupIndex,
        name, value < 0 ? fallback : value, 0, 0);
}
bool SoundManagerNativeEffects::PlayCueAt4BCD70(Body& body,const char* name,
    std::uint32_t groupIndex,std::int32_t fallback,std::uint8_t checkType)
{
    auto& current=static_cast<volatile Body&>(body);
    const auto begin=reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    const auto end=reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    // 4BCD7D的mask门控与脚本4D1AA8的unsigned差值判定分别保留。
    if(!(std::uint32_t(end-begin)&0xFFFFFFF8u))return true;
    if(checkType)
    {
        auto* params=ResolveSoundParamsAt418560(body);
        if(!Slot<NamedBool>(params,0x58)(params,name))return true;
    }
    auto* params=ResolveSoundParamsAt418560(body);
    const auto value=Slot<NamedValue>(params,0x5C)(params,name);
    return open_.OpenAt42BC90(body,current.remainingEffectsBeginAt20+groupIndex,
        name,value<0?fallback:value,0,0);
}
}
