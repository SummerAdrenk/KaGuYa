#include <Runtime/SoundSlotVectorNative.h>
#include <cstring>
#include <stdexcept>
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = SoundManagerNativeSlot8h;
constexpr std::uint32_t Maximum = 0x1FFFFFFFU;
std::uintptr_t Address(const void* p) noexcept { return reinterpret_cast<std::uintptr_t>(p); }
Slot* Advance(Slot* p, std::uint32_t count) noexcept { return reinterpret_cast<Slot*>(Address(p) + count * 8U); }
std::uint32_t Count(Slot* begin, Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(Address(end) - Address(begin));
    return (bytes >> 3) | ((bytes & 0x80000000U) ? 0xE0000000U : 0U); // 原signed SAR3后做unsigned比较。
}
Slot* Allocate(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity > Maximum ? 0xFFFFFFFFU : capacity * 8U;
    if (bytes < 0x1000U) return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto extra = bytes + 0x23U;
    auto* raw = memory.AllocateBody(extra <= bytes ? 0xFFFFFFFFU : extra);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23U) & 0xFFFFFFE0U;
    std::memcpy(reinterpret_cast<void*>(aligned - 4U), &raw, 4);
    return reinterpret_cast<Slot*>(aligned);
}
void DeleteStorage(Slot* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    void* raw = begin;
    if (bytes >= 0x1000U)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(begin) - 4U), 4);
        bytes += 0x23U;
        if (Address(begin) - Address(raw) - 4U > 0x1FU) _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}
Slot* ConstructAt466870(Slot* next, std::uint32_t count, IRuntimeStringManagerSource& strings)
{
    for (; count; --count, next = Advance(next, 1))
    {
        next->handleAt00 = 0; // 466880先写handle；GetNil失败时此槽name尚未发布。
        InitializeEmptyRuntimeCString(next->nameAt04, strings);
    }
    return next; // 原无本地前缀析构，只有正常返回后才更新caller的完成指针。
}
void CopyCurrentRange(SoundManagerNativeBody50h& body, Slot* destination)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    auto* begin = current.slotsBeginAt04;
    auto* const end = current.slotsEndAt08;
    struct Prefix final
    {
        Slot* begin;
        Slot* completed;
        bool active = true;
        ~Prefix() noexcept { if (active) DestroySoundSlotRangeAt48FC60(begin, completed); }
    } prefix{destination, destination}; // 596140 -> 45FC20，5EC868 flags5。
    for (; begin != end; begin = Advance(begin, 1))
    {
        prefix.completed->handleAt00 = begin->handleAt00;
        auto* const oldHeader = reinterpret_cast<ATL::CStringData*>(
            const_cast<char*>(begin->nameAt04.GetString())) - 1;
        auto* const header = CloneRuntimeCStringDataAt414540(oldHeader);
        const char* const characters = reinterpret_cast<const char*>(header + 1);
        // Clone成功后原生指针才进入已构造前缀；不经CString赋值。
        std::memcpy(static_cast<void*>(&prefix.completed->nameAt04), &characters, 4);
        prefix.completed = Advance(prefix.completed, 1);
    }
    prefix.active = false;
}
void ReleaseCurrentStorage(SoundManagerNativeBody50h& body, IEnvironmentRootMemory& memory)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    auto* const begin = current.slotsBeginAt04;
    if (!begin) return;
    auto* const end = current.slotsEndAt08;
    DestroySoundSlotRangeAt48FC60(begin, end);
    const auto capacity = Address(current.slotsCapacityAt0C);
    auto* const after = current.slotsBeginAt04;
    DeleteStorage(after, static_cast<std::uint32_t>(capacity - Address(after)) & 0xFFFFFFF8U, memory);
}
}
void DestroySoundSlotRangeAt48FC60(Slot* begin, Slot* end)
{
    for (; begin != end; begin = Advance(begin, 1)) DestroyRuntimeCStringAt40CD40(&begin->nameAt04);
}
void ResizeSoundSlotsAt4574C0(SoundManagerNativeBody50h& body, std::uint32_t count,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    auto* const oldEnd = current.slotsEndAt08;
    auto* const oldBegin = current.slotsBeginAt04;
    const auto size = Count(oldBegin, oldEnd);
    const auto capacity = Count(oldBegin, current.slotsCapacityAt0C);
    if (count <= capacity)
    {
        if (count > size) current.slotsEndAt08 = ConstructAt466870(oldEnd, count - size, strings);
        else if (count < size)
        {
            auto* const nextEnd = Advance(oldBegin, count);
            DestroySoundSlotRangeAt48FC60(nextEnd, oldEnd);
            current.slotsEndAt08 = nextEnd;
        }
        return;
    }
    if (count > Maximum) throw std::length_error("vector<T> too long");
    const auto half = capacity >> 1;
    const auto grown = capacity > Maximum - half ? count : capacity + half;
    const auto nextCapacity = grown < count ? count : grown;
    auto* const next = Allocate(nextCapacity, memory);
    auto* const tail = Advance(next, size);
    auto* completedTail = tail;
    try
    {
        completedTail = ConstructAt466870(tail, count - size, strings);
        CopyCurrentRange(body, next); // 尾构造全部成功后才读取当前旧begin/end。
    }
    catch (...)
    {
        // 4576D0普通catch；清理/删除异常替换原异常，不加自动重试或noexcept。
        DestroySoundSlotRangeAt48FC60(tail, completedTail);
        DeleteStorage(next, nextCapacity * 8U, memory);
        throw;
    }
    ReleaseCurrentStorage(body, memory); // 457607撤新块资格；旧Free失败不能回滚next。
    current.slotsBeginAt04 = next;
    current.slotsEndAt08 = Advance(next, count);
    current.slotsCapacityAt0C = Advance(next, nextCapacity);
}
void DestroySoundSlotsAt4627C0(SoundManagerNativeBody50h& body, IEnvironmentRootMemory& memory) noexcept
{
    auto& current = static_cast<volatile SoundManagerNativeBody50h&>(body);
    if (!current.slotsBeginAt04) return; // 原null begin不清end/capacity。
    ReleaseCurrentStorage(body, memory);
    current.slotsBeginAt04 = nullptr;
    current.slotsEndAt08 = nullptr;
    current.slotsCapacityAt0C = nullptr;
}
}
