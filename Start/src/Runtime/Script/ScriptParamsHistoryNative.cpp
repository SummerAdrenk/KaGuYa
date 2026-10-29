#include <Runtime/ScriptParamsHistoryNative.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::uint32_t MaximumCount = 0x3FFFFFFFu;
static_assert(sizeof(void*) == 4);
static_assert(sizeof(ScriptDwordVectorStorage) == 12);

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

std::uint32_t ByteDistance(const void* begin, const void* end) noexcept
{
    return static_cast<std::uint32_t>(Address(end) - Address(begin));
}

std::uint32_t Distance(const void* begin, const void* end) noexcept
{
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(ByteDistance(begin, end)) >> 2);
}

std::uint32_t* Advance(std::uint32_t* begin, std::uint32_t count) noexcept
{
    return reinterpret_cast<std::uint32_t*>(Address(begin) + count * 4u);
}

std::uint32_t* AllocateAt491120(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumCount ? capacity * 4u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
    {
        if (!bytes) return nullptr;
        auto* result = static_cast<std::uint32_t*>(memory.AllocateBody(bytes));
        // 与现有DWORD容器相同：原小块null随后非法写入，此处明确终止而非空成功。
        if (!result) _invalid_parameter_noinfo_noreturn();
        return result;
    }
    const auto extended = bytes + 0x23u;
    void* raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<std::uint32_t*>(aligned);
}

void DeleteAt48F6C0(std::uint32_t* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
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
}

void ResizeScriptParamsHistoryAt457300(ScriptDwordVectorStorage& history, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    auto& storage = history.storage;
    auto* const oldEnd = storage.end;
    const auto oldSize = Distance(storage.begin, oldEnd);
    const auto oldCapacity = Distance(storage.begin, storage.capacity);
    if (count > oldCapacity)
    {
        if (count > MaximumCount) throw std::length_error("vector<T> too long");
        const auto grown = oldCapacity > MaximumCount - oldCapacity / 2u ?
            count : oldCapacity + oldCapacity / 2u;
        const auto capacity = (std::max)(count, grown);
        auto* const next = AllocateAt491120(capacity, memory);
        // 457361..457383：先按缓存oldSize清新增尾部，再按回调后的begin/end复制。
        if (count != oldSize) std::memset(Advance(next, oldSize), 0, (count - oldSize) * 4u);
        auto* const currentBegin = storage.begin;
        const auto copyBytes = ByteDistance(currentBegin, storage.end);
        if (copyBytes) std::memmove(next, currentBegin, copyBytes);

        // 48F6C0无SEH回滚。删除回调后只发布，不追删回调替换进来的第三块。
        if (storage.begin)
            DeleteAt48F6C0(storage.begin, ByteDistance(storage.begin, storage.capacity) & ~3u, memory);
        storage.begin = next;
        storage.end = Advance(next, count);
        storage.capacity = Advance(next, capacity);
        return;
    }
    if (count > oldSize)
    {
        const auto bytes = (count - oldSize) * 4u;
        std::memset(oldEnd, 0, bytes);
        storage.end = reinterpret_cast<std::uint32_t*>(Address(oldEnd) + bytes);
    }
    else if (count < oldSize)
    {
        storage.end = Advance(storage.begin, count);
    }
}

SerializeBuffer& SerializeScriptParamsHistoryAt4661B0(SerializeBuffer& archive,
    ScriptDwordVectorStorage& history, IEnvironmentRootMemory& memory)
{
    // 4661C0仅决定外层分支；每次scalar仍由适配器读取自己的当前mode。
    if (archive.IsWriting())
    {
        auto count = history.Size();
        archive.SerializeSlot58Dword(count);
        const auto signedCount = std::bit_cast<std::int32_t>(count);
        for (std::int32_t index = 0; index < signedCount; ++index)
            archive.SerializeSlot3CDword(history.storage.begin[index]);
    }
    else
    {
        std::uint32_t count;
        archive.SerializeSlot58Dword(count);
        ResizeScriptParamsHistoryAt457300(history, count, memory);
        const auto signedCount = std::bit_cast<std::int32_t>(count);
        // 466224：逐项重取begin，先固定本项地址，再进入scalar；不复制临时元素。
        for (std::int32_t index = 0; index < signedCount; ++index)
            archive.SerializeSlot3CDword(history.storage.begin[index]);
    }
    return archive;
}

void AppendScriptParamsHistoryAt4C81F0(ScriptDwordVectorStorage& history,
    std::uint32_t capturedFormat, IEnvironmentRootMemory& memory)
{
    // 4C6D70与453A70同指令顺序，共用491120分配及48F6C0发布。
    history.Push(capturedFormat, memory);
}
}
