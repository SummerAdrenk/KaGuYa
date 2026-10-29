#include <Runtime/EnvironmentArchiveVectorRuntime.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;
static_assert(sizeof(EnvironmentVectorStorage<Slot>) == 12);

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    // 45434F / 4970C5：先截取 DWORD 差值，再按有符号数除以 12。
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

Slot* AllocateStorage(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = memory.AllocateBody(request);
    // 4543E2：仅大块分支检查 null；不是额外的安全容量上限。
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}

void DeleteStorage(Slot* begin, std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    auto bytes = capacity * 12u;
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
    memory.DeleteVector(allocation, bytes); // 491580 / 4970F2 -> 53FBFB。
}

void CopyReference(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

void CopyRange(const Slot* begin, const Slot* end, Slot* destination) noexcept
{
    // 4902A0：逐项复制并增计数；末尾的 4523B0 参数是同一地址，范围为空。
    for (; begin != end; ++begin, ++destination) CopyReference(*destination, *begin);
}

void ReleaseRange(Slot* begin, Slot* end) noexcept
{
    // 4523B0 / 5EBBE4：活动元素释放抛错进入 terminate，不继续下一个元素。
    for (; begin != end; ++begin) ReleaseYtlReference(*begin);
}

void GrowAndInsertAt454310(EnvironmentVectorStorage<Slot>& storage, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Distance(storage.begin, storage.end);
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = Distance(storage.begin, storage.capacity);
    // 45439B：1.5 倍溢出时回退到 required，而非钳位到 MaximumReferences。
    const auto grown = capacity > MaximumReferences - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateStorage(nextCapacity, memory);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopyReference(*inserted, source);
        // 454441：分配回调可能改变 end；position/index/required 仍为分配前的值。
        if (position == storage.end)
            CopyRange(storage.begin, storage.end, next);
        else
        {
            CopyRange(storage.begin, position, next);
            cleanupBegin = next;
            CopyRange(position, storage.end, inserted + 1);
        }
    }
    catch (...)
    {
        // 5EC198 / 454543：只保护构造阶段。DWORD 复制本身没有可抛 C++ 回调。
        ReleaseRange(cleanupBegin, inserted + 1);
        DeleteStorage(next, nextCapacity, memory);
        throw;
    }
    // 4544BB 已置 state=-1；旧存储删除异常外传，不回收未发布的新范围。
    if (storage.begin)
    {
        ReleaseRange(storage.begin, storage.end);
        DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity), memory);
    }
    storage.begin = next;
    storage.end = next + required;
    storage.capacity = next + nextCapacity;
}
}

void AppendEnvironmentArchiveReferenceAt49852D(EnvironmentVectorStorage<Slot>& storage,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    auto* end = storage.end;
    if (end != storage.capacity)
    {
        CopyReference(*end, source);
        ++storage.end;
        return;
    }
    GrowAndInsertAt454310(storage, end, source, memory);
}

void ClearEnvironmentArchiveReferencesAt498312(EnvironmentVectorStorage<Slot>& storage) noexcept
{
    ReleaseRange(storage.begin, storage.end);
    storage.end = storage.begin; // 498317：回调结束后重读 begin，capacity 不变。
}

void DestroyEnvironmentArchiveReferencesAt497080(EnvironmentVectorStorage<Slot>& storage,
    IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.begin) return; // 4970AA：不改写非零的 end/capacity。
    ReleaseRange(storage.begin, storage.end);
    // 4970B6..4970F2：回调后重读 begin/capacity，成功释放存储后才清三指针。
    DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity), memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
}
