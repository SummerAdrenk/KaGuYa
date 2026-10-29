#include <Runtime/RuntimeBitVector.h>

#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Storage = RuntimeBitVectorStorage;
static_assert(sizeof(void*) == 4);
constexpr std::uint32_t MaximumBits = 0x7FFFFFFFu;
constexpr std::uint32_t MaximumWords = 0x3FFFFFFFu;

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

std::uint32_t* Advance(std::uint32_t* pointer, std::uint32_t words) noexcept
{
    return reinterpret_cast<std::uint32_t*>(Address(pointer) + words * 4u);
}

std::uint32_t WordCount(const std::uint32_t* begin, const std::uint32_t* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    // 45771C/45771F：DWORD 相减再 SAR，包含空指针和 x86 回绕语义。
    return (bytes >> 2u) | ((bytes & 0x80000000u) ? 0xC0000000u : 0u);
}

struct BitCursor final
{
    std::uint32_t* word;
    std::uint32_t bit;
};

BitCursor Offset(BitCursor cursor, std::uint32_t bits) noexcept
{
    // 466700/48FF28：负偏移跨字时向下取整，不能直接无符号右移。
    const auto sum = cursor.bit + bits;
    const auto words = (bits & 0x80000000u) && cursor.bit < 0u - bits
        ? 0u - ((~sum >> 5u) + 1u) : sum >> 5u;
    return {Advance(cursor.word, words), sum & 31u};
}

bool Same(BitCursor left, BitCursor right) noexcept
{
    return left.word == right.word && left.bit == right.bit;
}

void Previous(BitCursor& cursor) noexcept
{
    if (cursor.bit) --cursor.bit;
    else { cursor.word = Advance(cursor.word, 0xFFFFFFFFu); cursor.bit = 31; }
}

void Next(BitCursor& cursor) noexcept
{
    if (cursor.bit < 31) ++cursor.bit;
    else { cursor.word = Advance(cursor.word, 1); cursor.bit = 0; }
}

void Write(BitCursor cursor, bool value) noexcept
{
    const auto mask = 1u << cursor.bit;
    if (value) *cursor.word |= mask;
    else *cursor.word &= ~mask;
}

std::uint32_t* AllocateAt4910B0(std::uint32_t words, IEnvironmentRootMemory& memory)
{
    const auto bytes = words > MaximumWords ? 0xFFFFFFFFu : words * 4u;
    if (bytes < 0x1000u)
        return bytes ? static_cast<std::uint32_t*>(memory.AllocateBody(bytes)) : nullptr;
    const auto extended = bytes + 0x23u;
    void* raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<std::uint32_t*>(aligned);
}

void ReleaseStorage(Storage& storage, IEnvironmentRootMemory& memory)
{
    auto* const begin = storage.beginAt00;
    if (!begin) return;
    std::uint32_t bytes = (Address(storage.capacityAt08) - Address(begin)) & 0xFFFFFFFCu;
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

void ResizeWordsAt457700(Storage& storage, std::uint32_t words, IEnvironmentRootMemory& memory)
{
    auto* const oldBegin = storage.beginAt00;
    auto* const oldEnd = storage.endAt04;
    const auto size = WordCount(oldBegin, oldEnd);
    const auto capacity = WordCount(oldBegin, storage.capacityAt08);
    if (words <= capacity)
    {
        if (words > size)
        {
            auto* next = oldEnd;
            for (auto remaining = words - size; remaining; --remaining, next = Advance(next, 1))
                *next = 0;
            storage.endAt04 = next;
        }
        else if (words < size) storage.endAt04 = Advance(oldBegin, words);
        return;
    }

    if (words > MaximumWords) throw std::length_error("vector<T> too long");
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumWords - half ? words : capacity + half;
    const auto nextCapacity = grown < words ? words : grown;
    auto* const next = AllocateAt4910B0(nextCapacity, memory);
    auto* tail = Advance(next, size);
    for (auto remaining = words - size; remaining; --remaining, tail = Advance(tail, 1))
        *tail = 0;

    // 457784：分配回调后重读旧范围；先填尾，再复制。原函数没有新块 EH 清理。
    auto* const source = storage.beginAt00;
    const std::uint32_t bytes = Address(storage.endAt04) - Address(source);
    if (bytes) std::memmove(next, source, bytes);

    // 48F660：旧块释放正常返回后才逐一提交；失败时新块不回收，count 尚未更新。
    ReleaseStorage(storage, memory);
    storage.beginAt00 = next;
    storage.endAt04 = Advance(next, words);
    storage.capacityAt08 = Advance(next, nextCapacity);
}

void GrowAt48FDF0(Storage& storage, std::uint32_t index, std::uint32_t added,
    bool value, IEnvironmentRootMemory& memory)
{
    const auto current = storage.countAt0C;
    if (MaximumBits - current < added) throw std::length_error("vector<bool> too long");
    ResizeWordsAt457700(storage, (current + added + 31u) >> 5u, memory);
    if (!storage.countAt0C) storage.countAt0C = added;
    else
    {
        auto source = Offset({storage.beginAt00, 0}, storage.countAt0C);
        storage.countAt0C += added;
        auto destination = Offset({storage.beginAt00, 0}, storage.countAt0C);
        const auto first = Offset({storage.beginAt00, 0}, index);
        // 普通 resize 在旧尾插入，无搬移；回调改写 count 时仍按原 helper 倒序搬移。
        while (!Same(first, source))
        {
            Previous(source);
            Previous(destination);
            Write(destination, (*source.word & (1u << source.bit)) != 0);
        }
    }
    auto cursor = Offset({storage.beginAt00, 0}, index);
    const auto end = Offset(cursor, added);
    for (; !Same(cursor, end); Next(cursor)) Write(cursor, value);
}

void ShrinkAt493480(Storage& storage, std::uint32_t count)
{
    // 494C66 总是擦除 [新尾, 旧尾)，493548 的尾段搬移循环不执行。
    if (count > MaximumBits) throw std::length_error("vector<bool> too long");
    const auto words = (count + 31u) >> 5u;
    auto* const begin = storage.beginAt00;
    if (words < WordCount(begin, storage.endAt04))
        storage.endAt04 = Advance(begin, words);
    storage.countAt0C = count;
    const auto bits = count & 31u;
    if (bits) *Advance(begin, words - 1u) &= (1u << bits) - 1u;
}
}

void ResizeRuntimeBitVectorAt494BD0(Storage& storage, std::uint32_t count,
    bool value, IEnvironmentRootMemory& memory)
{
    const auto previous = storage.countAt0C;
    if (previous < count) GrowAt48FDF0(storage, previous, count - previous, value, memory);
    else if (previous > count) ShrinkAt493480(storage, count);
}

void DestroyRuntimeBitVectorAt462850(Storage& storage, IEnvironmentRootMemory& memory) noexcept
{
    if (!storage.beginAt00) return;
    ReleaseStorage(storage, memory);
    storage.beginAt00 = nullptr;
    storage.endAt04 = nullptr;
    storage.capacityAt08 = nullptr;
}
}
