#include <Runtime/ParamsDemoStorage.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
constexpr std::array<std::uint32_t, 10> CommandSizes{4, 4, 12, 12, 12, 16, 12, 4, 28, 16};
constexpr std::uint32_t MaximumReferences = 0x15555555u;
constexpr std::uint32_t DataKind = 10;
constexpr std::uint32_t EntriesKind = 11;
static_assert(sizeof(std::uintptr_t) == 4);

template<class T>
T* Advance(T* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pointer) + count * sizeof(T));
}

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

void CopyReference(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

void ReleaseRangeAt407E40(Slot* begin, Slot* end) noexcept
{
    // 5E1C94 state0 覆盖单项释放；异常终止，不继续后面的元素。
    for (; begin != end; begin = Advance(begin, 1)) ReleaseYtlReference(*begin);
}

void CopyRangeAt43AA80(const Slot* begin, const Slot* end, Slot* destination)
{
    for (; begin != end; begin = Advance(begin, 1), destination = Advance(destination, 1))
        CopyReference(*destination, *begin);
    // 43AAEA 的终止 state 仅保护末尾空范围清理，不把整个复制入口设成 noexcept。
    ReleaseRangeAt407E40(destination, destination);
}

Slot* AllocateVector(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    auto* raw = memory.AllocateBody(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}

void DeleteVectorAt43B450(Slot* begin, std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    auto bytes = capacity * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(address - 4u), sizeof(allocation));
        bytes += 0x23u;
        if (address - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
    }
    memory.DeleteVector(raw, bytes);
}

void DestroyEntryPayload(ParamsDemoEntryRuntime* elements, std::uint32_t* cookie,
    std::uint32_t count) noexcept
{
    // 43C2B5 state0 包含反向析构和数组删除；析构数量先缓存，删除大小晚读 cookie。
    while (count) DestroyParamsDemoEntryAt40FB50(*Advance(elements, --count));
    const auto bytes = *cookie * 16u + 4u;
    ::operator delete[](cookie, static_cast<std::size_t>(bytes));
}

class RetiredDemoControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredDemoControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役后再次虚调删除的大小为 14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredDemoControl) == 0x18);

void RetireControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredDemoControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    // 本次派生 slot04 删除为 18h；flags=0 只退役且保留其余二十字节。
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

template<std::uint32_t Kind>
class DemoControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DemoControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if constexpr (Kind < DataKind)
            DestroyParamsDemoCommandBody(static_cast<ParamsDemoCommandType>(Kind), payloadAt14);
        else if constexpr (Kind == DataKind)
        {
            auto* const body = static_cast<ParamsDemoDataRuntime*>(payloadAt14);
            if (!body) return;
            DestroyParamsDemoDataAt40DF10(*body);
            // 43C9D2 的正文删除在 40DF10 返回之后，payload 入口自身不加 noexcept。
            ::operator delete(body, std::size_t{0x0C});
        }
        else
        {
            auto* const elements = static_cast<ParamsDemoEntryRuntime*>(payloadAt14);
            if (!elements) return;
            auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
            DestroyEntryPayload(elements, cookie, *cookie);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireControl(this, flags); }
};

template<std::uint32_t Kind>
EnvironmentObjectReferenceControl* AllocateControl(void* body, IEnvironmentRootMemory& memory)
{
    static_assert(sizeof(DemoControl<Kind>) == 0x18);
    void* const raw = memory.AllocateBody(0x18);
    return raw ? ::new (raw) DemoControl<Kind>(body) : nullptr;
}

EnvironmentObjectReferenceControl* AllocateCommandControl(
    ParamsDemoCommandType type, void* body, IEnvironmentRootMemory& memory)
{
    switch (type)
    {
    case ParamsDemoCommandType::End: return AllocateControl<0>(body, memory);
    case ParamsDemoCommandType::Next: return AllocateControl<1>(body, memory);
    case ParamsDemoCommandType::Wait: return AllocateControl<2>(body, memory);
    case ParamsDemoCommandType::Sound: return AllocateControl<3>(body, memory);
    case ParamsDemoCommandType::Load: return AllocateControl<4>(body, memory);
    case ParamsDemoCommandType::Transit: return AllocateControl<5>(body, memory);
    case ParamsDemoCommandType::Disp: return AllocateControl<6>(body, memory);
    case ParamsDemoCommandType::Update: return AllocateControl<7>(body, memory);
    case ParamsDemoCommandType::Move: return AllocateControl<8>(body, memory);
    case ParamsDemoCommandType::Pos: return AllocateControl<9>(body, memory);
    }
    throw std::invalid_argument("Unknown native demo command type");
}

void PublishOwnedControl(Slot& reference, EnvironmentObjectReferenceControl* control,
    std::uint32_t stride, std::uint32_t count)
{
    // 41CE77/41CF75：此顺序仅供外数组和 DemoData，不能用来发布 command localRef。
    reference.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    reference.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 = count;
}

std::uint32_t ArrayBytes(std::uint32_t count) noexcept
{
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - 4u) / 16u ? maximum : count * 16u + 4u;
}

struct EntryConstructionCleanup final
{
    void* raw;
    ParamsDemoEntryRuntime* elements;
    std::uint32_t bytes;
    std::uint32_t constructed = 0;
    bool active = true;
    ~EntryConstructionCleanup() noexcept
    {
        if (!active) return;
        // EH iterator 先退已构造前缀，随后 592BD7 删除 raw；二次异常不继续回收。
        while (constructed) DestroyParamsDemoEntryAt40FB50(*Advance(elements, --constructed));
        ::operator delete[](raw, static_cast<std::size_t>(bytes));
    }
};

void* Resolve(const Slot& reference, std::uint32_t index) noexcept
{
    const auto base = reference.baseAt00;
    if (!base) return nullptr;
    const auto stride = reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (reference.biasAt04 + index) * stride;
    return reinterpret_cast<void*>(base + offset);
}
}

std::uint32_t ParamsDemoCommandBodySize(ParamsDemoCommandType type)
{
    const auto index = static_cast<std::uint32_t>(type);
    if (index >= CommandSizes.size()) throw std::invalid_argument("Unknown native demo command type");
    return CommandSizes[index];
}

void* AllocateParamsDemoCommandBody(ParamsDemoCommandType type, IEnvironmentRootMemory& memory)
{
    return memory.AllocateBody(ParamsDemoCommandBodySize(type));
}

void DestroyParamsDemoCommandBody(ParamsDemoCommandType type, void* body)
{
    const auto bytes = ParamsDemoCommandBodySize(type);
    switch (type)
    {
    case ParamsDemoCommandType::Sound:
        if (!body) return;
        DestroyRuntimeCStringAt40CD40(&static_cast<ParamsDemoCmdSound*>(body)->nameAt08);
        break;
    case ParamsDemoCommandType::Load:
        if (!body) return;
        DestroyRuntimeCStringAt40CD40(&static_cast<ParamsDemoCmdLoad*>(body)->nameAt08);
        break;
    case ParamsDemoCommandType::Transit:
        if (!body) return;
        DestroyRuntimeCStringAt40CD40(&static_cast<ParamsDemoCmdTransit*>(body)->maskAt0C);
        // 43C976 在前一次 Free 返回后才读取 effect 字段。
        DestroyRuntimeCStringAt40CD40(&static_cast<ParamsDemoCmdTransit*>(body)->effectAt04);
        break;
    default: break;
    }
    ::operator delete(body, static_cast<std::size_t>(bytes));
}

EnvironmentObjectReferenceControl* PublishParamsDemoCommandReference(Slot& local,
    ParamsDemoCommandType type, void* constructedBody, IEnvironmentRootMemory& memory)
{
    const auto stride = ParamsDemoCommandBodySize(type);
    local.baseAt00 = 0;
    local.biasAt04 = 0;
    local.controlAt08 = nullptr;
    if (!constructedBody) return nullptr;
    local.biasAt04 = 0;
    local.baseAt00 = reinterpret_cast<std::uintptr_t>(constructedBody);
    auto* const control = AllocateCommandControl(type, constructedBody, memory);
    // 419EB0 的 command 分支在发布 local.control 之前访问 control，空分配在此硬失败。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = stride;
    local.controlAt08 = control;
    control->countAt10 = 1;
    return control;
}

void ReleaseParamsDemoCommandAt40D3D0(Slot& local) noexcept { ReleaseYtlReference(local); }

void ReleaseParamsDemoCommandCached(Slot& local, YtlReferenceControlRuntime* control) noexcept
{
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    local.controlAt08 = nullptr;
    local.baseAt00 = 0;
}

Slot* InsertParamsDemoCommandAt408B70(ParamsDemoCommandVector& storage, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Distance(storage.begin, storage.end);
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = Distance(storage.begin, storage.capacity);
    const auto grown = capacity > MaximumReferences - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* const next = AllocateVector(nextCapacity, memory);
    auto* const inserted = Advance(next, index);
    auto* cleanupBegin = inserted;
    auto* const cleanupEnd = Advance(inserted, 1);
    try
    {
        // 408C87..408C9E：先读分配回调后的 source 并增持，才读取当前旧 end。
        if (!next) _invalid_parameter_noinfo_noreturn();
        CopyReference(*inserted, source);
        if (position == storage.end)
            CopyRangeAt43AA80(storage.begin, storage.end, next);
        else
        {
            CopyRangeAt43AA80(storage.begin, position, next);
            cleanupBegin = next;
            CopyRangeAt43AA80(position, storage.end, cleanupEnd);
        }
    }
    catch (...)
    {
        // 408DA3 的 catch 本身可抛；新块删除失败替换原异常，不包成 noexcept 析构。
        ReleaseRangeAt407E40(cleanupBegin, cleanupEnd);
        DeleteVectorAt43B450(next, nextCapacity, memory);
        throw;
    }
    // 408D1B 已撤销新范围回滚；旧 payload/退块回调后的三指针必须重新读取。
    if (storage.begin)
    {
        ReleaseRangeAt407E40(storage.begin, storage.end);
        auto* const currentCapacity = storage.capacity;
        auto* const currentBegin = storage.begin;
        DeleteVectorAt43B450(currentBegin, Distance(currentBegin, currentCapacity), memory);
    }
    storage.begin = next;
    storage.end = Advance(next, required);
    storage.capacity = Advance(next, nextCapacity);
    return Advance(storage.begin, index);
}

void AppendParamsDemoCommandAt43D350(ParamsDemoCommandVector& storage,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    auto* const end = storage.end;
    if (end != storage.capacity)
    {
        CopyReference(*end, source);
        storage.end = Advance(storage.end, 1);
        return;
    }
    static_cast<void>(InsertParamsDemoCommandAt408B70(storage, end, source, memory));
}

void ClearParamsDemoCommandsAt419F71(ParamsDemoCommandVector& storage) noexcept
{
    ReleaseRangeAt407E40(storage.begin, storage.end);
    storage.end = storage.begin;
}

void DestroyParamsDemoDataAt40DF10(ParamsDemoDataRuntime& body, IEnvironmentRootMemory& memory) noexcept
{
    auto& storage = body.commandsAt00;
    if (!storage.begin) return;
    ReleaseRangeAt407E40(storage.begin, storage.end);
    auto* const currentCapacity = storage.capacity;
    auto* const currentBegin = storage.begin;
    DeleteVectorAt43B450(currentBegin, Distance(currentBegin, currentCapacity), memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

void ReleaseParamsDemoDataAt43B7D0(Slot& reference) { ReleaseYtlReference(reference); }
void ReleaseParamsDemoEntriesAt43B890(Slot& reference) { ReleaseYtlReference(reference); }

ParamsDemoDataRuntime* RebuildParamsDemoDataAt41CF13(Slot& reference, IEnvironmentRootMemory& memory)
{
    ReleaseParamsDemoDataAt43B7D0(reference);
    auto* const body = static_cast<ParamsDemoDataRuntime*>(memory.AllocateBody(0x0C));
    if (body)
    {
        body->commandsAt00.begin = nullptr;
        body->commandsAt00.end = nullptr;
        body->commandsAt00.capacity = nullptr;
    }
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const control = AllocateControl<DataKind>(body, memory);
    PublishOwnedControl(reference, control, 0x0C, 1);
    return body;
}

void ConstructParamsDemoEntryAt40C810(ParamsDemoEntryRuntime& entry, IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(entry.nameAt00, strings);
    entry.dataAt04.baseAt00 = 0;
    entry.dataAt04.biasAt04 = 0;
    entry.dataAt04.controlAt08 = nullptr;
}

void DestroyParamsDemoEntryAt40FB50(ParamsDemoEntryRuntime& entry) noexcept
{
    ReleaseYtlReference(entry.dataAt04);
    DestroyRuntimeCStringAt40CD40(&entry.nameAt00);
}

ParamsDemoEntryRuntime* RebuildParamsDemoEntriesAt41CDCC(Slot& reference, std::uint32_t count,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ReleaseParamsDemoEntriesAt43B890(reference);
    if (!count) return nullptr;
    const auto bytes = ArrayBytes(count);
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](bytes));
    auto* const elements = cookie ? reinterpret_cast<ParamsDemoEntryRuntime*>(cookie + 1) : nullptr;
    EntryConstructionCleanup cleanup{cookie, elements, bytes};
    if (cookie)
    {
        *cookie = count;
        while (cleanup.constructed != count)
        {
            ConstructParamsDemoEntryAt40C810(*Advance(elements, cleanup.constructed), strings);
            ++cleanup.constructed;
        }
    }
    cleanup.active = false; // 41CE38：后续当前 control 删除/新 control 分配均无 raw-array 回滚。
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    auto* const control = AllocateControl<EntriesKind>(elements, memory);
    PublishOwnedControl(reference, control, 0x10, count);
    return elements;
}

void* ResolveParamsDemoCommand(const Slot& reference) noexcept { return Resolve(reference, 0); }
ParamsDemoDataRuntime* ResolveParamsDemoDataAt4A52A0(const Slot& reference) noexcept
{
    return static_cast<ParamsDemoDataRuntime*>(Resolve(reference, 0));
}
ParamsDemoEntryRuntime* ResolveParamsDemoEntry(const Slot& reference, std::uint32_t index) noexcept
{
    return static_cast<ParamsDemoEntryRuntime*>(Resolve(reference, index));
}
std::uint32_t ParamsDemoReferenceCount(const Slot& reference) noexcept
{
    return reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 : 0u;
}

ParamsDemoStorage::~ParamsDemoStorage() noexcept { ReleaseParamsDemoEntriesAt43B890(storage); }
ParamsDemoStorage::ParamsDemoStorage(ParamsDemoStorage&& other) noexcept
    : storage(std::exchange(other.storage, Slot{})) {}
ParamsDemoStorage& ParamsDemoStorage::operator=(ParamsDemoStorage&& other)
{
    if (this != &other)
    {
        Release();
        storage = std::exchange(other.storage, Slot{});
    }
    return *this;
}
void ParamsDemoStorage::Release() { ReleaseParamsDemoEntriesAt43B890(storage); }
std::uint32_t ParamsDemoStorage::size() const noexcept { return ParamsDemoReferenceCount(storage); }
ParamsDemoEntryRuntime* ParamsDemoStorage::Resolve(std::uint32_t index) noexcept
{
    return ResolveParamsDemoEntry(storage, index);
}
const ParamsDemoEntryRuntime* ParamsDemoStorage::Resolve(std::uint32_t index) const noexcept
{
    return ResolveParamsDemoEntry(storage, index);
}
}
