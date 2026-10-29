#include <Runtime/SecretCgMappingVector.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Mapping = SecretCgMappingRuntime;
using Storage = SecretCgMappingVectorStorage;
using Reference = YtlReferenceSlotRuntime;
using References = EnvironmentVectorStorage<Reference>;
constexpr std::uint32_t MaximumMappings = 0x0FFFFFFFu;
constexpr std::uint32_t MaximumReferences = 0x15555555u;
static_assert(sizeof(void*) == 4 && sizeof(Reference) == 12);
static_assert(sizeof(ATL::CStringData) == 16);
static_assert(offsetof(ATL::CStringData, nRefs) == 12);

std::uintptr_t Address(const void* value) noexcept
{
    return reinterpret_cast<std::uintptr_t>(value);
}

template<class T>
T* Advance(T* value, std::uint32_t count) noexcept
{
    return reinterpret_cast<T*>(Address(value) + count * sizeof(T));
}

std::uint32_t MappingCount(const Mapping* begin, const Mapping* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    // 457831/457834：DWORD 差值 SAR 4 后作无符号比较，不使用 C++ 指针相减。
    return (bytes >> 4u) | ((bytes & 0x80000000u) ? 0xF0000000u : 0u);
}

std::uint32_t ReferenceCount(const Reference* begin, const Reference* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

void* AllocateStorage(std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    // 45786F..4578C2 / 43B240：标量分配，1000h 起使用原 32-byte 对齐头。
    if (bytes < 0x1000u) return bytes ? memory.AllocateBody(bytes) : nullptr;
    const auto extended = bytes + 0x23u;
    void* raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<void*>(aligned);
}

void DeleteStorage(void* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(begin) - 4u), sizeof(raw));
        bytes += 0x23u;
        if (Address(begin) - Address(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}

template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function result;
    static_assert(sizeof(result) == sizeof(void*));
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}

ATL::CStringData* Header(const RuntimeCString& value) noexcept
{
    const char* data;
    std::memcpy(&data, &value, sizeof(data));
    return reinterpret_cast<ATL::CStringData*>(Address(data) - 16u);
}

char* Data(ATL::CStringData* header) noexcept
{
    return reinterpret_cast<char*>(Address(header) + 16u);
}

void Publish(RuntimeCString& destination, ATL::CStringData* header) noexcept
{
    char* data = Data(header);
    std::memcpy(&destination, &data, sizeof(data));
}

ATL::CStringData* CloneDataAt414540(ATL::CStringData* source)
{
    // 457915调用同一个414540；仅克隆，不释放/发布已有目标串。
    return CloneRuntimeCStringDataAt414540(source);
}

void DestroyReferencesAt43A760(References& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    // 已逐指令核对 407D20 与 ReleaseYtlReference；回调后重读 control，bias 不清。
    // 407D20 state=0 -> 541E55：Ref 回调异常终止，不可外传。
    const auto release = [](Reference* begin, Reference* end) noexcept {
        for (; begin != end; begin = Advance(begin, 1)) ReleaseYtlReference(*begin);
    };
    release(storage.begin, storage.end);
    // 43A774..43A791：析构回调后重读存储。此函数自身不把 DeleteVector 设为 noexcept。
    const auto bytes = ReferenceCount(storage.begin, storage.capacity) * 12u;
    DeleteStorage(storage.begin, bytes, memory);
    storage = {};
}

void CopyReferencesAt457FD0(References& destination, const References& source,
    IEnvironmentRootMemory& memory)
{
    destination = {};
    const auto count = ReferenceCount(source.begin, source.end);
    if (!count) return;
    if (count > MaximumReferences) throw std::length_error("vector<T> too long");
    auto* next = static_cast<Reference*>(AllocateStorage(count * 12u, memory));
    destination.begin = next;
    destination.end = next;
    destination.capacity = Advance(next, count);
    try
    {
        // 458056..45805D 在分配回调后取范围；40AEE0 顺序复制三 DWORD，再非原子增持。
        auto* begin = source.begin;
        auto* const end = source.end;
        auto* output = destination.begin;
        for (; begin != end; begin = Advance(begin, 1), output = Advance(output, 1))
        {
            output->baseAt00 = begin->baseAt00;
            output->biasAt04 = begin->biasAt04;
            output->controlAt08 = begin->controlAt08;
            if (output->controlAt08) ++output->controlAt08->referenceCountAt04;
        }
        destination.end = output;
    }
    catch (...)
    {
        // 45807E 普通 catch：按 destination 当前 end 清理，再重抛；并无元素复制回调。
        DestroyReferencesAt43A760(destination, memory);
        throw;
    }
}

void DestroyMappingsAt452180(Mapping* begin, Mapping* end, IEnvironmentRootMemory& memory)
{
    for (; begin != end; begin = Advance(begin, 1))
    {
        // 4521B3 state=0 -> terminate，仅包住内层 vector；4521C4 撤销后 CString 可抛。
        const auto destroyInner = [&memory](References& references) noexcept {
            DestroyReferencesAt43A760(references, memory);
        };
        destroyInner(begin->referencesAt04);
        DestroyRuntimeCStringAt40CD40(&begin->nameAt00);
    }
}

Mapping* DefaultMappingsAt4668C0(Mapping* next, std::uint32_t count,
    IRuntimeStringManagerSource& strings)
{
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    for (; count; --count, next = Advance(next, 1))
    {
        // 466900..466934：先清整个 16 字节，Capture/GetNil 后发布串，再清内层三指针。
        // 5EB678 无前缀清理：失败前完成的元素不回滚，调用者 end 也尚未推进。
        std::memset(next, 0, sizeof(Mapping));
        auto* manager = strings.CaptureAt417360();
        if (!manager) throw ATL::CAtlException(E_FAIL);
        Publish(next->nameAt00, ManagerSlot<GetNil>(manager, 12)(manager));
        next->referencesAt04 = {};
    }
    return next;
}

void CopyMappings(Mapping* begin, Mapping* end, Mapping* destination,
    IEnvironmentRootMemory& memory)
{
    struct PrefixCleanup
    {
        Mapping* begin;
        Mapping* completed;
        IEnvironmentRootMemory& memory;
        bool active = true;
        ~PrefixCleanup() noexcept
        {
            // 596170 -> 45FBD0 / 5EC88C EHFlags=5：已完成 mapping 前缀清理失败终止。
            if (active) DestroyMappingsAt452180(begin, completed, memory);
        }
    } prefix{destination, destination, memory};
    for (; begin != end; begin = Advance(begin, 1))
    {
        auto* output = prefix.completed;
        Publish(output->nameAt00, CloneDataAt414540(Header(begin->nameAt00)));
        struct StringCleanup
        {
            RuntimeCString* value;
            bool active = true;
            ~StringCleanup() noexcept
            {
                // 596178 -> 40CD40：只有 CString 已发布而 inner copy 未完成时展开。
                if (active) DestroyRuntimeCStringAt40CD40(value);
            }
        } string{&output->nameAt00};
        CopyReferencesAt457FD0(output->referencesAt04, begin->referencesAt04, memory);
        string.active = false;
        prefix.completed = Advance(output, 1);
    }
    // 457949 state=3 仅覆盖空范围 Destroy；不存在可抛回调。随后 457957 撤销新块保护。
    prefix.active = false;
}

void ReleaseCurrentStorage(Storage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    DestroyMappingsAt452180(storage.begin, storage.end, memory);
    const std::uint32_t bytes = (Address(storage.capacity) - Address(storage.begin)) & 0xFFFFFFF0u;
    DeleteStorage(storage.begin, bytes, memory);
}
}

void ResizeSecretCgMappingVectorAt4577F0(Storage& storage, std::uint32_t count,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* const oldEnd = storage.end;
    auto* const oldBegin = storage.begin;
    const auto size = MappingCount(oldBegin, oldEnd);
    const auto capacity = MappingCount(oldBegin, storage.capacity);
    if (count <= capacity)
    {
        if (count > size)
            storage.end = DefaultMappingsAt4668C0(oldEnd, count - size, strings);
        else if (count < size)
        {
            auto* const end = Advance(oldBegin, count);
            DestroyMappingsAt452180(end, oldEnd, memory);
            storage.end = end;
        }
        return;
    }
    if (count > MaximumMappings) throw std::length_error("vector<T> too long");
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumMappings - half ? count : capacity + half;
    const auto nextCapacity = grown < count ? count : grown;
    auto* const next = static_cast<Mapping*>(AllocateStorage(nextCapacity * 16u, memory));
    auto* const tail = Advance(next, size);
    auto* completedTail = tail;
    try
    {
        completedTail = DefaultMappingsAt4668C0(tail, count - size, strings);
        // 4578ED..4578F0：所有尾段回调返回后才捕获旧范围，不能沿用入口的 begin/end。
        CopyMappings(storage.begin, storage.end, next, memory);
    }
    catch (...)
    {
        // 457A27 -> 48F7B0 普通 catch：CString Free/外层 DeleteVector 可替换原异常。
        DestroyMappingsAt452180(tail, completedTail, memory);
        // 48FDC0 仅返回 ECX；491440 才按保存的新容量释放，不能重读 storage。
        DeleteStorage(next, nextCapacity * 16u, memory);
        throw;
    }
    // 457957 已 state=-1；旧释放失败不清新块，也不发布新的三指针。
    ReleaseCurrentStorage(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, count);
    storage.capacity = Advance(next, nextCapacity);
}

void DestroySecretCgMappingVectorAt461500(Storage& storage, IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.begin) return;
    ReleaseCurrentStorage(storage, memory);
    storage = {};
}
}
