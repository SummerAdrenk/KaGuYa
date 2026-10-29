#include <Runtime/SceneControllerNative.h>

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
static_assert(sizeof(std::uintptr_t) == 4);
constexpr std::uint32_t MaximumNames = 0x3FFFFFFFu;
constexpr std::uint32_t MaximumTasks = 0x15555555u;

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

template<class T>
T* Advance(T* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<T*>(Address(pointer) + count * sizeof(T));
}

std::int32_t ByteDistance(const void* begin, const void* end) noexcept
{
    return std::bit_cast<std::int32_t>(Address(end) - Address(begin));
}

std::uint32_t NameDistance(const void* begin, const void* end) noexcept
{
    // 453B67/453B6A/453B83：SAR 2，不把负差改成无符号除法。
    return static_cast<std::uint32_t>(ByteDistance(begin, end) >> 2);
}

std::uint32_t TaskDistance(const void* begin, const void* end) noexcept
{
    // 4092FF..409318 / 461845..461850：有符号 /12，向零截断。
    return static_cast<std::uint32_t>(ByteDistance(begin, end) / 12);
}

std::uint32_t Grow(std::uint32_t capacity, std::uint32_t required,
    std::uint32_t maximum) noexcept
{
    // 453B95 / 40934F：溢出分支退到 required，不饱和为 max_size。
    const auto grown = capacity > maximum - capacity / 2u ? required : capacity + capacity / 2u;
    return (std::max)(required, grown);
}

template<class T>
T* AllocateVector(std::uint32_t capacity, std::uint32_t maximum, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= maximum ? capacity * static_cast<std::uint32_t>(sizeof(T)) : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<T*>(memory.AllocateBody(bytes)) : nullptr;
    // 453BBF..453BE1 / 40937E..4093A0：仅大块检查 null；小块直接交给后续写入。
    const auto expanded = bytes + 0x23u;
    void* const raw = memory.AllocateBody(expanded > bytes ? expanded : 0xFFFFFFFFu);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<T*>(aligned);
}

void DeleteVector(void* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        // 453C63 / 4094A8 / 43B527 / 46185E / 466B0E：还原 raw，校验 unsigned 距离。
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(begin) - 4u), sizeof(raw));
        bytes += 0x23u;
        if (Address(begin) - Address(raw) - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes); // 53FBFB：兼容全局 delete 的最终地址及 sized-delete 参数。
}

void CopyReference(Slot& target, const Slot& source)
{
    target.baseAt00 = source.baseAt00;
    target.biasAt04 = source.biasAt04;
    target.controlAt08 = source.controlAt08;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}

void CopyRangeAt43AC30(Slot* begin, Slot* end, Slot* target)
{
    for (; begin != end; begin = Advance(begin, 1), target = Advance(target, 1))
        CopyReference(*target, *begin);
    ReleaseSceneTaskRangeNativeAt407FF0(target, target);
}
}

const char** InsertSceneNameNativeAt453B50(EnvironmentVectorStorage<const char*>& storage,
    const char** position, const char* const* source, IEnvironmentRootMemory& memory)
{
    const auto index = NameDistance(storage.begin, position);
    const auto size = NameDistance(storage.begin, storage.end);
    if (size == MaximumNames) throw std::length_error("vector<T> too long"); // 453CA4 -> 490E30
    const auto required = size + 1u;
    const auto nextCapacity = Grow(NameDistance(storage.begin, storage.capacity), required, MaximumNames);
    auto* const next = AllocateVector<const char*>(nextCapacity, MaximumNames, memory);
    // 453BF9..453C0B：分配完成才解引用 source；先写插入项，允许旧区间内别名。
    *Advance(next, index) = *source;
    if (position == storage.end)
        std::memmove(next, storage.begin, Address(storage.end) - Address(storage.begin));
    else
    {
        std::memmove(next, storage.begin, Address(position) - Address(storage.begin));
        std::memmove(Advance(next, index + 1u), position, Address(storage.end) - Address(position));
    }
    if (storage.begin)
        DeleteVector(storage.begin, (Address(storage.capacity) - Address(storage.begin)) & ~3u, memory);
    // 453C7F..453C8F：删除旧块成功后才发布；删除异常不回收新块。
    storage.begin = next;
    storage.end = Advance(next, required);
    storage.capacity = Advance(next, nextCapacity);
    return Advance(storage.begin, index);
}

Slot* InsertSceneTaskNativeAt4092C0(EnvironmentVectorStorage<Slot>& storage,
    Slot* position, const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = TaskDistance(storage.begin, position);
    const auto size = TaskDistance(storage.begin, storage.end);
    if (size == MaximumTasks) throw std::length_error("vector<T> too long"); // 409518 -> 43B130
    const auto required = size + 1u;
    const auto nextCapacity = Grow(TaskDistance(storage.begin, storage.capacity), required, MaximumTasks);
    auto* const next = AllocateVector<Slot>(nextCapacity, MaximumTasks, memory);
    auto* const inserted = Advance(next, index);
    auto* cleanupBegin = inserted;
    auto* const cleanupEnd = Advance(inserted, 1);
    try
    {
        // 4093DA..4093EE：分配回调后读三个源字段，增持完成后才重读旧 end。
        CopyReference(*inserted, source);
        auto* const oldEnd = storage.end;
        if (position == oldEnd)
            CopyRangeAt43AC30(storage.begin, oldEnd, next);
        else
        {
            CopyRangeAt43AC30(storage.begin, position, next);
            cleanupBegin = next; // 40945B：EH 范围只扩到已完成的前缀及插入项。
            CopyRangeAt43AC30(position, storage.end, cleanupEnd);
        }
    }
    catch (...)
    {
        // PE 5E23A0 catch -> 4094F3：43A520 正序 release，然后 43B510 删除新块。
        // 删除异常可以替换原异常；这里不是 noexcept RAII 退块。
        ReleaseSceneTaskRangeNativeAt407FF0(cleanupBegin, cleanupEnd);
        DeleteVector(next, nextCapacity * 12u, memory);
        throw;
    }
    // 40946B 撤销新块回滚资格；释放旧范围后重读 capacity/begin，不缓存退休前地址。
    if (storage.begin)
    {
        ReleaseSceneTaskRangeNativeAt407FF0(storage.begin, storage.end);
        auto* const currentCapacity = storage.capacity;
        auto* const currentBegin = storage.begin;
        DeleteVector(currentBegin, TaskDistance(currentBegin, currentCapacity) * 12u, memory);
    }
    storage.begin = next;
    storage.end = Advance(next, required);
    storage.capacity = Advance(next, nextCapacity);
    return Advance(storage.begin, index);
}

void ReleaseSceneTaskRangeNativeAt407FF0(Slot* begin, Slot* end) noexcept
{
    // PE 5E1D10 state0 -> 541E55：正常及 43A520 EH 清理均正序，释放异常立即终止。
    // 451767 与 408021..408051 完全同序：减计数、payload 后重读 control、清 control/base，保留 bias。
    for (; begin != end; begin = Advance(begin, 1)) ReleaseYtlReference(*begin);
}

void DestroySceneNameVectorNative(EnvironmentVectorStorage<const char*>& storage,
    IEnvironmentRootMemory& memory) noexcept
{
    // 466AF7..466B38，5EA2C0 flags5；begin 空时不补写 end/capacity。
    if (!storage.begin) return;
    DeleteVector(storage.begin, (Address(storage.capacity) - Address(storage.begin)) & ~3u, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

void DestroySceneTaskVectorNative(EnvironmentVectorStorage<Slot>& storage,
    IEnvironmentRootMemory& memory) noexcept
{
    // 461800，PE 5E4F80 flags5；正常释放先于退块，三指针只在退块成功后清零。
    if (!storage.begin) return;
    ReleaseSceneTaskRangeNativeAt407FF0(storage.begin, storage.end);
    auto* const currentCapacity = storage.capacity;
    auto* const currentBegin = storage.begin;
    DeleteVector(currentBegin, TaskDistance(currentBegin, currentCapacity) * 12u, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}
}
