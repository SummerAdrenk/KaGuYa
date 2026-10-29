#include <Runtime/ParamsPatternNative.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

void ReleaseIntegerRangeAt407D20(Slot* begin, Slot* end) noexcept
{
    // 原范围固定、正序；payload后重取槽control，单项异常进入541E55。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}

class RetiredPatternControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredPatternControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：已退休control的删除尺寸是14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredPatternControl) == 0x18);

class PatternControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PatternControl(ParamsPatternNative* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() noexcept override
    {
        // 43CB53缓存正文；两组本身终止，43CB6D启用的state0覆盖后两表及正文delete。
        auto* const body = static_cast<ParamsPatternNative*>(payloadAt14);
        if (!body) return;
        std::destroy_at(body);
        ::operator delete(body, std::size_t{0x34});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 411800：flags=0仍退化vptr，保留+04..+17；首次删除传18h。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredPatternControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(PatternControl) == 0x18);
}

void DestroyIntegersAt43A760(ParamsPatternReferenceVector& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    ReleaseIntegerRangeAt407D20(storage.begin, storage.end);
    // 43A774/43A77C：范围回调结束后才按当前capacity/begin决定所删外块。
    auto* const capacity = storage.capacity;
    auto* const begin = storage.begin;
    auto bytes = Distance(begin, capacity) * 12u;
    void* allocation = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t raw;
        std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
        bytes += 0x23u;
        if (address - raw - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        allocation = reinterpret_cast<void*>(raw);
    }
    memory.DeleteVector(allocation, bytes);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

ParamsPatternNative::ParamsPatternNative() noexcept
    : itemsAt04{nullptr, nullptr, nullptr}, integersAt10{nullptr, nullptr, nullptr},
      firstGroupsAt1C{nullptr, nullptr, nullptr}, secondGroupsAt28{nullptr, nullptr, nullptr} {}

ParamsPatternNative::~ParamsPatternNative() noexcept { DestroyStorage(); }

void ParamsPatternNative::DestroyStorage(IEnvironmentRootMemory& memory) noexcept
{
    DestroyEnvironmentArchiveReferencesAt497080(secondGroupsAt28, memory);
    DestroyEnvironmentArchiveReferencesAt497080(firstGroupsAt1C, memory);
    // 43CB30 state0到返回前未撤；独立可抛的43A760在这里须终止。
    DestroyIntegersAt43A760(integersAt10, memory);
    DestroyEnvironmentArchiveReferencesAt497080(itemsAt04, memory);
}

void RebuildParamsPatternAt434D61(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    void* const storage = memory.AllocateBody(0x34);
    auto* const body = storage ? ::new (storage) ParamsPatternNative : nullptr;
    // 434DFC：分配回调改入的control只删一次，不递减，也不释放其payload。
    if (auto* const rebound = slot.controlAt08) rebound->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* const controlStorage = memory.AllocateBody(0x18);
    auto* const control = controlStorage ? ::new (controlStorage) PatternControl(body) : nullptr;
    slot.controlAt08 = control;
    // 安全差异：原434E35写null+4；与GSRef一致，保留发布残留后明确抛bad_alloc。
    if (!control) throw std::bad_alloc{};
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x34;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

ParamsPatternNative* ResolveParamsPattern(const YtlReferenceSlotRuntime& slot) noexcept
{
    return ResolveEnvironmentObjectReference<ParamsPatternNative>(slot);
}

ParamsPatternReference::~ParamsPatternReference() noexcept { reset(); }
ParamsPatternReference::ParamsPatternReference(const ParamsPatternReference& other) noexcept
    : slot_(other.slot_)
{
    if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04;
}
ParamsPatternReference& ParamsPatternReference::operator=(const ParamsPatternReference& other)
{
    if (this != &other)
    {
        ParamsPatternReference retained(other);
        *this = std::move(retained);
    }
    return *this;
}
ParamsPatternReference::ParamsPatternReference(ParamsPatternReference&& other) noexcept
    : slot_(std::exchange(other.slot_, YtlReferenceSlotRuntime{})) {}
ParamsPatternReference& ParamsPatternReference::operator=(ParamsPatternReference&& other)
{
    if (this != &other)
    {
        reset();
        slot_ = std::exchange(other.slot_, YtlReferenceSlotRuntime{});
    }
    return *this;
}
void ParamsPatternReference::reset() { ReleaseYtlReference(slot_); }

ParamsPatternReference ParamsPatternReference::CreateFixture(std::uint8_t legacyByte)
{
    auto body = std::make_unique<ParamsPatternNative>();
    body->legacyByte00 = legacyByte;
    void* const storage = ::operator new(sizeof(PatternControl));
    if (!storage) throw std::bad_alloc{};
    auto* control = ::new (storage) PatternControl(body.get());
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x34;
    control->countAt10 = 1;
    ParamsPatternReference result;
    result.slot_ = {reinterpret_cast<std::uintptr_t>(body.release()), 0, control};
    return result;
}
}
