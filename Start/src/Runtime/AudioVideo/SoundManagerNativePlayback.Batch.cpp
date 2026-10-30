#include <Runtime/SoundManagerNativePlayback.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Stride(YtlReferenceControlRuntime* control)
{
    return control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
}
std::int32_t Count(YtlReferenceControlRuntime* control)
{
    return control ? *reinterpret_cast<const volatile std::int32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x10) : 0;
}
RuntimeCString* NameAt(const YtlReferenceSlotRuntime& input, std::uint32_t index)
{
    const auto& names = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    const auto base = names.baseAt00;
    if (!base) return nullptr;
    const auto stride = Stride(names.controlAt08);
    const auto offset = (names.biasAt04 + index) * stride;
    return reinterpret_cast<RuntimeCString*>(base + offset);
}
SoundManagerNativeSlot8h* VoiceSlot(SoundManagerNativeBody50h& input, std::uint32_t index)
{
    auto& body = static_cast<volatile SoundManagerNativeBody50h&>(input);
    const auto slot = body.voicesBeginAt14 + index;
    const auto base = reinterpret_cast<std::uintptr_t>(body.slotsBeginAt04);
    return reinterpret_cast<SoundManagerNativeSlot8h*>(base + slot * 8U);
}
EnvironmentSoundParamsArchive* InitialParametersAt42C52F(SoundManagerNativeBody50h& body)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(body.paramsAt24);
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    const auto stride = Stride(ref.controlAt08);
    return reinterpret_cast<EnvironmentSoundParamsArchive*>(base + ref.biasAt04 * stride);
}
class StateControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit StateControl(void* payload) noexcept
        : EnvironmentObjectReferenceControl(payload, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete[](payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); } // 410B40与已审计18h退役叶一致。
};
static_assert(sizeof(StateControl) == 0x18);
struct StateReference final
{
    YtlReferenceSlotRuntime slot{};
    bool active = true;
    ~StateReference() noexcept { if (active) ReleaseYtlReference(slot); } // 40CF70 flags=5。
    void BuildAt4127B0(std::int32_t count)
    {
        ReleaseYtlReference(slot);
        if (count <= 0) return;
        const auto n = static_cast<std::uint32_t>(count);
        const auto bytes = n > 0x3FFFFFFFU ? 0xFFFFFFFFU : n * 4U;
        auto* const payload = ::operator new[](bytes); // 原正文未清零、无array cookie。
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
        slot.biasAt04 = 0; slot.baseAt00 = reinterpret_cast<std::uintptr_t>(payload);
        auto* const raw = ::operator new(0x18);
        auto* const control = raw ? ::new(raw) StateControl(payload) : nullptr;
        slot.controlAt08 = control;
        control->referenceCountAt04 = 1; // 原空control直接失败，不另设bad_alloc兜底。
        slot.controlAt08->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
        static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = n;
    }
    std::uint32_t* AtCaptured(std::uintptr_t base, std::uint32_t index, YtlReferenceControlRuntime* control)
    {
        if (!base) return nullptr;
        return reinterpret_cast<std::uint32_t*>(base + (slot.biasAt04 + index) * Stride(control));
    }
    std::uint32_t* AtCurrent(std::uint32_t index)
    {
        const auto& current = static_cast<const volatile YtlReferenceSlotRuntime&>(slot);
        if (!current.baseAt00) return nullptr;
        const auto stride = Stride(current.controlAt08);
        const auto offset = (current.biasAt04 + index) * stride;
        return reinterpret_cast<std::uint32_t*>(current.baseAt00 + offset); // 410481晚读base。
    }
    void FinishCaptured(YtlReferenceControlRuntime* control) noexcept
    {
        active = false; // 42CA03 state7 -> terminate，不再展开重复释放Ref。
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1); // 原EBX固定，不重读slot的control。
        }
    }
};
union TextStorage { RuntimeCString value; TextStorage() noexcept {} ~TextStorage() noexcept {} };
struct TextUnwind final
{
    RuntimeCString* value;
    ~TextUnwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
};
template<class Function>
Function ParameterSlot(EnvironmentSoundParamsArchive* params, std::size_t offset)
{
    const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(params);
    return reinterpret_cast<Function>(table[offset / 4]);
}
}

bool SoundManagerNativePlayback::PlayBatchAt42C4F0(SoundManagerNativeBody50h& body,
    const YtlReferenceSlotRuntime& input, ISoundManagerNativeOpen& open)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    const auto end = reinterpret_cast<std::uintptr_t>(current.slotsEndAt08);
    const auto begin = reinterpret_cast<std::uintptr_t>(current.slotsBeginAt04);
    if (static_cast<std::uint32_t>(end - begin) < 8U) return true;
    using Ready = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*);
    auto* parameters = InitialParametersAt42C52F(body); // 初始内联段固定base，不套后续getter的晚读base。
    if (!ParameterSlot<Ready>(parameters, 0xA4)(parameters)) return true;

    const auto& names = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    const auto initialCount = Count(names.controlAt08);
    if (initialCount > ReadSoundVoiceCountNative(params_)) return false;
    StateReference states;
    states.BuildAt4127B0(Count(names.controlAt08)); // GS捕获之后再读输入数量。
    StopAndClearAt4384D0(body);
    auto* const captured = states.slot.controlAt08; // 原42C5EA，Stop之后固定EBX。
    using NamedBool = std::uint8_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
    using NamedValue = std::int32_t (__thiscall*)(EnvironmentSoundParamsArchive*, const char*);
    using Fallback = const std::int32_t* (__thiscall*)(EnvironmentSoundParamsArchive*);
    std::uintptr_t stateBase;
    for (std::int32_t i = 0;; ++i)
    {
        const auto count = Count(names.controlAt08);
        stateBase = static_cast<const volatile YtlReferenceSlotRuntime&>(states.slot).baseAt00;
        if (i >= count) break;
        *states.AtCaptured(stateBase, i, captured) = 0;
        parameters = ResolveSoundParamsAt418560(body);
        auto* name = NameAt(input, i);
        const auto* table = *reinterpret_cast<const std::uintptr_t* const*>(parameters);
        auto* characters = name->GetString();
        const auto allowed = reinterpret_cast<NamedBool>(table[0xA8 / 4]);
        if (!allowed(parameters, characters))
        {
            *states.AtCaptured(states.slot.baseAt00, i, captured) = 1;
            continue;
        }
        parameters = ResolveSoundParamsAt418560(body);
        name = NameAt(input, i);
        table = *reinterpret_cast<const std::uintptr_t* const*>(parameters);
        characters = name->GetString();
        const auto read = reinterpret_cast<NamedValue>(table[0xAC / 4]);
        auto value = read(parameters, characters);
        if (value < 0)
        {
            const std::int32_t* fallback;
            try
            {
                parameters = ResolveSoundParamsAt418560(body);
                fallback = ParameterSlot<Fallback>(parameters, 0x6C)(parameters);
            }
            catch (...) { continue; } // 5E1088只覆盖state1的取param/+6C虚调用。
            value = *fallback; // 42C6FA先撤state1；返回指针读取不属于catch。
            if (value < 0) continue;
        }
        characters = NameAt(input, i)->GetString();
        auto* const header = CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(
            const_cast<char*>(characters)) - 1);
        characters = reinterpret_cast<const char*>(header + 1);
        // Clone引用写入空槽，仍由下方Unwind/缓存header负责释放。
        TextStorage held; std::memcpy(static_cast<void*>(&held.value), &characters, sizeof(characters));
        TextUnwind unwind{&held.value};
        const auto slot = current.voicesBeginAt14 + static_cast<std::uint32_t>(i);
        if (open.OpenAt42BC90(body, slot, characters, value, 0, 1)) *states.AtCurrent(i) = 3;
        unwind.value = nullptr; // 42C78D正常Free前撤state3，Free失败不再清持有串。
        ReleaseRuntimeCStringHeader(header);
    }
    bool success = true;
    for (std::int32_t i = 0; i < Count(captured); ++i)
    {
        if (*states.AtCaptured(stateBase, i, captured) == 3)
        {
            auto* slot = VoiceSlot(body, i); // 42C810：固定槽地址，然后Sound捕获、晚读handle。
            sound_.PlaySlotAt42C87F(&slot->handleAt00);
            auto* const name = NameAt(input, i);
            slot = VoiceSlot(body, i); // Sound回调后重取names、当前voices起址/slots。
            const char* const source = name->GetString();
            // 此槽仅借用chars，不Clone也不析构；410220自行取得目标引用。
            TextStorage borrowed; std::memcpy(static_cast<void*>(&borrowed.value), &source, sizeof(source));
            AssignRuntimeCStringAt410220(slot->nameAt04, borrowed.value);
        }
        stateBase = static_cast<const volatile YtlReferenceSlotRuntime&>(states.slot).baseAt00;
        success &= (*states.AtCaptured(stateBase, i, captured) & 1U) != 0;
    }
    states.FinishCaptured(captured);
    return success;
}
}
