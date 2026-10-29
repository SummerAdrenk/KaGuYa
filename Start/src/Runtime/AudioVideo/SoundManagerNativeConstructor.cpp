#include <Runtime/SoundManagerNativeLifecycle.h>
#include <Runtime/SoundSlotVectorNative.h>
#include <Runtime/SoundLowerReferenceNative.h>
#include <Runtime/SoundParamsReference.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t ReadGameSystem(ParamsRuntimeRootOwner& params, std::uint32_t offset)
{
    auto* const root = params.CaptureAt418480();
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(root->gameSystemAt04.Native());
    const auto base = ref.baseAt00;
    std::uintptr_t address = 0;
    if (base)
    {
        auto* const control = ref.controlAt08;
        const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
            reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
        address = base + ref.biasAt04 * stride;
    }
    return *reinterpret_cast<const volatile std::uint32_t*>(address + offset);
}
SoundManagerNativeSlot8h* At(SoundManagerNativeBody50h& body, std::uint32_t index)
{
    const auto base = reinterpret_cast<std::uintptr_t>(static_cast<volatile SoundManagerNativeBody50h&>(body).slotsBeginAt04);
    return reinterpret_cast<SoundManagerNativeSlot8h*>(base + index * 8U);
}
std::int32_t SlotCount(SoundManagerNativeBody50h& body)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    const auto bits = static_cast<std::uint32_t>(end - begin);
    return std::bit_cast<std::int32_t>((bits >> 3) | (bits & 0x80000000U ? 0xE0000000U : 0U));
}
}
void InitializeSoundHandlesNativeAt42CA60(SoundManagerNativeBody50h& body, ParamsRuntimeRootOwner& params, SoundModuleNativeApi& sound)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    auto* const music = At(body, current.musicAt10);
    music->handleAt00 = sound.AddAt412AB0(1, 0); // music固定目标槽，返回后不重取begin。
    auto index = current.voicesBeginAt14;
    for (;; ++index)
    {
        const auto voices = ReadGameSystem(params, 0x4C);
        const auto bound = voices + current.voicesBeginAt14;
        if (std::bit_cast<std::int32_t>(index) >= std::bit_cast<std::int32_t>(bound)) break;
        const auto handle = sound.AddAt412AB0(0, 0);
        At(body, index)->handleAt00 = handle; // voice/effect正常返回后才取当前slots起址。
    }
    auto count = SlotCount(body);
    index = current.firstEffectsBeginAt18;
    while (std::bit_cast<std::int32_t>(index) < count)
    {
        const auto handle = sound.AddAt412AB0(0, 0);
        At(body, index)->handleAt00 = handle;
        ++index; count = SlotCount(body);
    }
}
namespace
{
void ReleaseParamsUnwind(YtlReferenceSlotRuntime& ref) noexcept { ReleaseYtlReference(ref); } // 460810 flags5。
}
SoundManagerNativeBody50h* ConstructSoundManagerAt45D120(SoundManagerNativeBody50h& body,
    const YtlReferenceSlotRuntime& saved, ParamsRuntimeRootOwner& params, SoundModuleNativeApi& sound,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    const SoundManagerNativeScope scope(memory);
    body.vptrAt00 = &SoundManagerNativeMethods();
    body.slotsBeginAt04 = body.slotsEndAt08 = body.slotsCapacityAt0C = nullptr;
    body.paramsAt24 = {};
    body.lowerAt30 = {nullptr, nullptr, nullptr};
    struct Members final
    {
        SoundManagerNativeBody50h& body; IEnvironmentRootMemory& memory; bool active = true;
        ~Members() noexcept
        {
            if (!active) return;
            DestroySoundLowerAt462240(body, memory);
            ReleaseParamsUnwind(body.paramsAt24);
            DestroySoundSlotsAt4627C0(body, memory);
        }
    } unwind{body, memory};
    ShareSoundParamsAt45D1A3(body.paramsAt24, saved);
    sound.SetGlobalFocusAt45D1F4();
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    current.musicAt10 = 0; current.voicesBeginAt14 = 1;
    auto bound = 1U + ReadGameSystem(params, 0x4C); current.firstEffectsBeginAt18 = bound;
    bound += ReadGameSystem(params, 0x50); current.secondEffectsBeginAt1C = bound;
    bound += ReadGameSystem(params, 0x48); current.remainingEffectsBeginAt20 = bound;
    bound += ReadGameSystem(params, 0x44);
    ResizeSoundSlotsAt4574C0(body, bound, strings, memory);
    InitializeSoundHandlesNativeAt42CA60(body, params, sound);
    ClearSoundLowerAt45D368(body); // state4 -> terminate；清理结束后恢复state2。
    for (std::uint32_t i = 0; std::bit_cast<std::int32_t>(i) <
        std::bit_cast<std::int32_t>(current.secondEffectsBeginAt1C - current.firstEffectsBeginAt18); ++i)
    {
        // 下标必须在GetNil前固定，+08原不写；body/control分配失败的早期孤立块不补回收。
        auto* const lower = CreateSoundLowerAt45D394(body, i, strings, memory);
        YtlReferenceSlotRuntime temporary{};
        BuildSoundLowerReferenceAt45D3E7(temporary, lower, memory);
        struct LocalRef final
        {
            YtlReferenceSlotRuntime* slot;
            ~LocalRef() noexcept { if (slot) ReleaseYtlReference(*slot); }
        } local{&temporary}; // state5获资格后才append。
        AppendSoundLowerAt45D435(body, temporary, memory);
        auto* const capturedControl = temporary.controlAt08; // 慢路径45D466在append返回后才恢复ESI。
        local.slot = nullptr;
        ReleaseSoundLowerCapturedAt45D469(temporary, capturedControl);
    }
    current.policyAt3C = ReadGameSystem(params, 0x54);
    const auto* const device = sound.DefaultDeviceAt45D4E2();
    std::memcpy(&body.deviceAt40, device, 16); // 原movups直接读，空指针不得改默认值。
    unwind.active = false;
    return &body;
}
}
