#include <Runtime/ParamsLegacyTables.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
static_assert(sizeof(void*) == 4);
static_assert(alignof(ParamsLegacyVoiceRuntime) <= alignof(std::uint32_t));
static_assert(alignof(ParamsLegacyMultiRuntime) <= alignof(std::uint32_t));
static_assert(alignof(ParamsLegacySoundRuntime) <= alignof(std::uint32_t));

std::uint32_t AllocationBytes(std::uint32_t count, std::uint32_t stride,
    std::uint32_t cookieBytes) noexcept
{
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - cookieBytes) / stride ? maximum : count * stride + cookieBytes;
}

template<class Element>
Element* Advance(Element* elements, std::uint32_t index) noexcept
{
    const auto offset = index * static_cast<std::uint32_t>(sizeof(Element));
    return reinterpret_cast<Element*>(reinterpret_cast<std::uintptr_t>(elements) + offset);
}

template<class Element>
void DestroyCookiePayload(Element* elements, std::uint32_t* cookie, std::uint32_t count) noexcept
{
    // 43C63F/43C39F/43C55F：state0覆盖逆序析构和数组删除；进入后任何异常立即终止。
    while (count) std::destroy_at(Advance(elements, --count));
    // 析构范围用入口cookie，删除尺寸却重读cookie并按原DWORD回绕。
    const auto bytes = *cookie * static_cast<std::uint32_t>(sizeof(Element)) + 4u;
    ::operator delete[](cookie, static_cast<std::size_t>(bytes));
}

class RetiredLegacyControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredLegacyControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18={541C5F,412460}：退役后再次删除使用14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredLegacyControl) == 0x18);

void RetireLegacyControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 411230/411110/4111D0/410B40：flags=0也退役，只换vptr；padding与payload原样保留。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* const retired = ::new (static_cast<void*>(control)) RetiredLegacyControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

template<class Element>
class LegacyArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit LegacyArrayControl(Element* elements) noexcept
        : EnvironmentObjectReferenceControl(elements, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 三个typed表分别对应43C600/43C360/43C520；不从Ref base或control.count取析构范围。
        auto* const elements = static_cast<Element*>(payloadAt14);
        if (!elements) return;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
        const auto count = *cookie;
        DestroyCookiePayload(elements, cookie, count);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireLegacyControl(this, flags); }
};
static_assert(sizeof(LegacyArrayControl<ParamsLegacyVoiceRuntime>) == 0x18);
static_assert(sizeof(LegacyArrayControl<ParamsLegacyMultiRuntime>) == 0x18);
static_assert(sizeof(LegacyArrayControl<ParamsLegacySoundRuntime>) == 0x18);

class LegacyIntegerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit LegacyIntegerControl(std::int32_t* elements) noexcept
        : EnvironmentObjectReferenceControl(elements, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 43BAA0只转发free(payload)。沿工程数组释放适配口，不读count、不计算sized delete。
        ::operator delete[](payloadAt14);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireLegacyControl(this, flags); }
};
static_assert(sizeof(LegacyIntegerControl) == 0x18);

template<class Element>
struct ConstructionCleanup final
{
    void* raw;
    Element* elements;
    std::uint32_t allocationBytes;
    std::uint32_t completed = 0;
    bool active = true;

    ~ConstructionCleanup() noexcept
    {
        if (!active) return;
        // 先由iterator退成功前缀，再走592AF0/592B4D/592B82；二次异常不继续释放外块。
        while (completed) std::destroy_at(Advance(elements, --completed));
        ::operator delete[](raw, static_cast<std::size_t>(allocationBytes));
    }
};

template<class Control, class Element>
void PublishControl(Slot& reference, Element* elements, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    void* const raw = memory.AllocateBody(sizeof(Control));
    auto* const control = raw ? ::new (raw) Control(elements) : nullptr;
    reference.controlAt08 = control;
    // 原null+4硬故障点：先发布null，再明确终止；不回滚body，不伪造成功空表。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    reference.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C =
        static_cast<std::uint32_t>(sizeof(Element));
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 = count;
}

template<class Element>
Element* RebuildCookieArray(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ReleaseYtlReference(reference);
    if (!count) return nullptr;
    const auto bytes = AllocationBytes(count, static_cast<std::uint32_t>(sizeof(Element)), 4);
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](bytes));
    auto* const elements = cookie ? reinterpret_cast<Element*>(cookie + 1) : nullptr;
    ConstructionCleanup<Element> cleanup{cookie, elements, bytes};
    if (cookie)
    {
        *cookie = count;
        while (cleanup.completed != count)
        {
            std::construct_at(Advance(elements, cleanup.completed), strings);
            ++cleanup.completed;
        }
    }
    // 41B61B/41C11A/41C494撤raw块资格，后续delete-only或control分配失败均不回收正文。
    cleanup.active = false;
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    PublishControl<LegacyArrayControl<Element>>(reference, elements, count, memory);
    return elements;
}

const void* Resolve(const Slot& reference, std::uint32_t index) noexcept
{
    const auto base = reference.baseAt00;
    if (!base) return nullptr;
    const auto stride = reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (reference.biasAt04 + index) * stride;
    return reinterpret_cast<const void*>(base + offset);
}
}

ParamsLegacyVoiceRuntime::ParamsLegacyVoiceRuntime(IRuntimeStringManagerSource& strings)
{
    // 40CA60无SEH，00..03未写；GetNil成功后才清两个Ref。
    InitializeEmptyRuntimeCString(nameAt04, strings);
    std::construct_at(&stringsAt08);
    std::construct_at(&stringsAt14);
}

ParamsLegacyVoiceRuntime::~ParamsLegacyVoiceRuntime() noexcept
{
    // 40FF30 flags5：14 -> 08 -> name04，异常不继续后续字段。
    std::destroy_at(&stringsAt14);
    std::destroy_at(&stringsAt08);
    DestroyRuntimeCStringAt40CD40(&nameAt04);
}

ParamsLegacyMultiRuntime::ParamsLegacyMultiRuntime(IRuntimeStringManagerSource& strings)
{
    // 40C8E0：CString发布后才写04/08/0C。
    InitializeEmptyRuntimeCString(nameAt00, strings);
    std::construct_at(&integersAt04);
}

ParamsLegacyMultiRuntime::~ParamsLegacyMultiRuntime() noexcept
{
    // 40FC50 flags5：原位释放整数Ref，然后取当前name。
    ReleaseYtlReference(integersAt04);
    DestroyRuntimeCStringAt40CD40(&nameAt00);
}

ParamsLegacySoundRuntime::ParamsLegacySoundRuntime(IRuntimeStringManagerSource& strings)
{
    // 40CA00：CString发布后才写两个内Ref。
    InitializeEmptyRuntimeCString(nameAt00, strings);
    std::construct_at(&stringsAt04);
    std::construct_at(&stringsAt10);
}

ParamsLegacySoundRuntime::~ParamsLegacySoundRuntime() noexcept
{
    // 40FDF0 flags5：10 -> 04 -> name00。
    std::destroy_at(&stringsAt10);
    std::destroy_at(&stringsAt04);
    DestroyRuntimeCStringAt40CD40(&nameAt00);
}

ParamsLegacyVoiceRuntime* ConstructParamsLegacyVoiceAt40CA60(
    ParamsLegacyVoiceRuntime* storage, IRuntimeStringManagerSource& strings)
{
    return std::construct_at(storage, strings);
}

ParamsLegacyMultiRuntime* ConstructParamsLegacyMultiAt40C8E0(
    ParamsLegacyMultiRuntime* storage, IRuntimeStringManagerSource& strings)
{
    return std::construct_at(storage, strings);
}

ParamsLegacySoundRuntime* ConstructParamsLegacySoundAt40CA00(
    ParamsLegacySoundRuntime* storage, IRuntimeStringManagerSource& strings)
{
    return std::construct_at(storage, strings);
}

void DestroyParamsLegacyVoiceAt40FF30(ParamsLegacyVoiceRuntime& body) noexcept { std::destroy_at(&body); }
void DestroyParamsLegacyMultiAt40FC50(ParamsLegacyMultiRuntime& body) noexcept { std::destroy_at(&body); }
void DestroyParamsLegacySoundAt40FDF0(ParamsLegacySoundRuntime& body) noexcept { std::destroy_at(&body); }

ParamsLegacyVoiceRuntime* RebuildParamsLegacyVoiceAt41B581(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    return RebuildCookieArray<ParamsLegacyVoiceRuntime>(reference, count, memory, strings);
}

ParamsLegacyMultiRuntime* RebuildParamsLegacyMultiAt41C0A6(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    return RebuildCookieArray<ParamsLegacyMultiRuntime>(reference, count, memory, strings);
}

ParamsLegacySoundRuntime* RebuildParamsLegacySoundAt41C420(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    return RebuildCookieArray<ParamsLegacySoundRuntime>(reference, count, memory, strings);
}

std::int32_t* RebuildParamsLegacyIntegersAt41C2DE(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(reference);
    if (!count) return nullptr;
    auto* const elements = static_cast<std::int32_t*>(::operator new[](AllocationBytes(count, 4, 0)));
    // 41C301重读control，41C321先发布base；整个分支没有数组块展开清理资格。
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    PublishControl<LegacyIntegerControl>(reference, elements, count, memory);
    return elements;
}

EnvironmentObjectReferenceControl* AllocateParamsLegacyIntegerControl(
    std::int32_t* elements, IEnvironmentRootMemory& memory)
{
    // 与既有41C323路径使用同一具体类；字段发布仍由调用者按各自缓存/重读合同完成。
    void* const raw = memory.AllocateBody(sizeof(LegacyIntegerControl));
    return raw ? ::new (raw) LegacyIntegerControl(elements) : nullptr;
}

void ReleaseParamsLegacyTable(Slot& reference) { ReleaseYtlReference(reference); }

std::uint32_t ParamsLegacyTableCount(const Slot& reference) noexcept
{
    return reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 : 0u;
}

ParamsLegacyVoiceRuntime* ResolveParamsLegacyVoice(Slot& reference, std::uint32_t index) noexcept
{
    return const_cast<ParamsLegacyVoiceRuntime*>(ResolveParamsLegacyVoice(std::as_const(reference), index));
}

const ParamsLegacyVoiceRuntime* ResolveParamsLegacyVoice(const Slot& reference, std::uint32_t index) noexcept
{
    return static_cast<const ParamsLegacyVoiceRuntime*>(Resolve(reference, index));
}

ParamsLegacyMultiRuntime* ResolveParamsLegacyMulti(Slot& reference, std::uint32_t index) noexcept
{
    return const_cast<ParamsLegacyMultiRuntime*>(ResolveParamsLegacyMulti(std::as_const(reference), index));
}

const ParamsLegacyMultiRuntime* ResolveParamsLegacyMulti(const Slot& reference, std::uint32_t index) noexcept
{
    return static_cast<const ParamsLegacyMultiRuntime*>(Resolve(reference, index));
}

ParamsLegacySoundRuntime* ResolveParamsLegacySound(Slot& reference, std::uint32_t index) noexcept
{
    return const_cast<ParamsLegacySoundRuntime*>(ResolveParamsLegacySound(std::as_const(reference), index));
}

const ParamsLegacySoundRuntime* ResolveParamsLegacySound(const Slot& reference, std::uint32_t index) noexcept
{
    return static_cast<const ParamsLegacySoundRuntime*>(Resolve(reference, index));
}

std::int32_t* ResolveParamsLegacyInteger(Slot& reference, std::uint32_t index) noexcept
{
    return const_cast<std::int32_t*>(ResolveParamsLegacyInteger(std::as_const(reference), index));
}

const std::int32_t* ResolveParamsLegacyInteger(const Slot& reference, std::uint32_t index) noexcept
{
    return static_cast<const std::int32_t*>(Resolve(reference, index));
}
}
