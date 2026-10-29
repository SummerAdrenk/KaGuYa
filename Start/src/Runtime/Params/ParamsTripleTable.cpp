#include <Runtime/ParamsTripleTable.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Triple = ParamsTripleTable::value_type;
using Storage = EnvironmentVectorStorage<Triple>;
constexpr std::uint32_t MaximumCount = 0x15555555u;
static_assert(sizeof(void*) == 4);

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

Triple* Advance(Triple* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<Triple*>(Address(pointer) + count * 12u);
}

std::uint32_t Distance(const Triple* begin, const Triple* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(Address(end) - Address(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

Triple* AllocateStorage(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const std::uint32_t bytes = capacity > MaximumCount ? 0xFFFFFFFFu : capacity * 12u;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Triple*>(memory.AllocateBody(bytes)) : nullptr;
    const std::uint32_t extended = bytes + 0x23u;
    void* const raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & ~std::uintptr_t{0x1F};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Triple*>(aligned);
}

void DeleteStorage(Triple* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
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

void ParamsTripleTable::ResizeAt40ACB0(std::uint32_t count, IEnvironmentRootMemory& memory)
{
    auto* const oldEnd = storage.end;
    const auto oldSize = Distance(storage.begin, oldEnd);
    const auto oldCapacity = Distance(storage.begin, storage.capacity);
    if (count > oldCapacity)
    {
        if (count > MaximumCount) throw std::length_error("vector<T> too long");
        const auto grown = oldCapacity > MaximumCount - oldCapacity / 2u ?
            count : oldCapacity + oldCapacity / 2u;
        const auto nextCapacity = (std::max)(count, grown);
        auto* const next = AllocateStorage(nextCapacity, memory);
        // 40AD7C：尾部起点使用分配前的 size；未使用的 capacity 尾部不初始化。
        if (count != oldSize)
            std::memset(Advance(next, oldSize), 0, (count - oldSize) * 12u);

        // 40AD9D：memmove 读取分配回调后的 begin/end，不能使用缓存 oldEnd。
        auto* const currentBegin = storage.begin;
        const auto copyBytes = static_cast<std::uint32_t>(Address(storage.end) - Address(currentBegin));
        if (copyBytes) std::memmove(next, currentBegin, copyBytes);
        if (storage.begin)
            DeleteStorage(storage.begin, Distance(storage.begin, storage.capacity) * 12u, memory);

        // 无新块清理资格；旧删除回调结束后只发布，不再追删回调写入的槽。
        storage.begin = next;
        storage.end = Advance(next, count);
        storage.capacity = Advance(next, nextCapacity);
        return;
    }
    if (count > oldSize)
    {
        const auto bytes = (count - oldSize) * 12u;
        std::memset(oldEnd, 0, bytes);
        storage.end = reinterpret_cast<Triple*>(Address(oldEnd) + bytes);
    }
    else if (count < oldSize)
    {
        storage.end = Advance(storage.begin, count);
    }
}

void ParamsTripleTable::DestroyAt40E6C0(IEnvironmentRootMemory& memory)
{
    auto* const begin = storage.begin;
    if (!begin) return;
    DeleteStorage(begin, Distance(begin, storage.capacity) * 12u, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

std::size_t ParamsTripleTable::size() const noexcept
{
    return Distance(storage.begin, storage.end);
}

ParamsTripleTable::value_type& ParamsTripleTable::operator[](std::size_t index) noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

const ParamsTripleTable::value_type& ParamsTripleTable::operator[](std::size_t index) const noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

void ParamsTripleTable::AssignSnapshot(std::span<const value_type> values)
{
    if (values.size() > MaximumCount) throw std::length_error("Params triple snapshot exceeds vector count");
    ParamsTripleTable snapshot;
    snapshot.ResizeAt40ACB0(static_cast<std::uint32_t>(values.size()));
    if (!values.empty()) std::memcpy(snapshot.storage.begin, values.data(), values.size_bytes());
    *this = std::move(snapshot);
}

ParamsTripleTable::~ParamsTripleTable() noexcept
{
    DestroyAt40E6C0();
}

ParamsTripleTable::ParamsTripleTable(const ParamsTripleTable& other)
{
    AssignSnapshot({other.storage.begin, other.size()});
}

ParamsTripleTable& ParamsTripleTable::operator=(const ParamsTripleTable& other)
{
    if (this != &other) AssignSnapshot({other.storage.begin, other.size()});
    return *this;
}

ParamsTripleTable::ParamsTripleTable(ParamsTripleTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsTripleTable& ParamsTripleTable::operator=(ParamsTripleTable&& other) noexcept
{
    if (this != &other)
    {
        DestroyAt40E6C0();
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

bool ParamsTripleTable::operator==(const ParamsTripleTable& other) const noexcept
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t index = 0; index < count; ++index)
        if ((*this)[index] != other[index]) return false;
    return true;
}
}
