#include <Runtime/ParamsNestedStringTable.h>

#include <bit>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Inner = ParamsNestedStringTable::Inner;
using Storage = EnvironmentVectorStorage<Inner>;
constexpr std::uint32_t MaximumCount = 0x15555555u;
static_assert(sizeof(void*) == 4);

std::uintptr_t Address(const void* pointer) noexcept
{
    return reinterpret_cast<std::uintptr_t>(pointer);
}

Inner* Advance(Inner* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<Inner*>(Address(pointer) + count * 12u);
}

std::uint32_t Count(Inner* begin, Inner* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    // 40A67F/40E658：有符号 DWORD 距离除十二，向零截断；不是内层的 SAR 2。
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

std::uint32_t InnerCount(const Inner& inner) noexcept
{
    const std::uint32_t bytes = Address(inner.end) - Address(inner.begin);
    return (bytes >> 2u) | ((bytes & 0x80000000u) ? 0xC0000000u : 0u);
}

const RuntimeCString& InnerAt(const Inner& inner, std::uint32_t index) noexcept
{
    return *reinterpret_cast<const RuntimeCString*>(Address(inner.begin) + index * 4u);
}

Inner* AllocateAt40A6E6(std::uint32_t count, IEnvironmentRootMemory& memory)
{
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    const std::uint32_t bytes = count > MaximumCount ? maximum : count * 12u;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Inner*>(memory.AllocateBody(bytes)) : nullptr;
    const std::uint32_t extended = bytes + 0x23u;
    void* const raw = memory.AllocateBody(extended <= bytes ? maximum : extended);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Inner*>(aligned);
}

void DeleteStorage(Inner* begin, std::uint32_t bytes, IEnvironmentRootMemory& memory)
{
    // 40A7FA/43B71F/40E669：释放尺寸来自各自捕获或重读的容量，不重新做分配饱和。
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

void MoveRangeAt40AF70(Inner* begin, Inner* end, Inner* destination) noexcept
{
    for (; begin != end; begin = Advance(begin, 1), destination = Advance(destination, 1))
    {
        // 原码先清目的再读源，随后清源；不能用 Clone 或提前缓存整个源聚合替换此顺序。
        // 这里只搬三指针，没有分配/释放/CString 调用，不声称原 FuncInfo 是 flags5。
        destination->begin = nullptr;
        destination->end = nullptr;
        destination->capacity = nullptr;
        destination->begin = begin->begin;
        destination->end = begin->end;
        destination->capacity = begin->capacity;
        begin->begin = nullptr;
        begin->end = nullptr;
        begin->capacity = nullptr;
    }
}

void DestroyRangeAt43A620(Inner* begin, Inner* end, IEnvironmentRootMemory& memory)
{
    for (; begin != end; begin = Advance(begin, 1))
    {
        // 43A652 state0、40A7C2 state2 均先保护本次 43A700 调用，失败 terminate。
        // 复用 40DD20 的相同 C++ 终止边界；不把后续外层存储删除也放入此边界。
        DestroyRuntimeCStringVectorAt40DD20(*begin, memory);
    }
}

void ReleaseCurrentStorage(Storage& storage, IEnvironmentRootMemory& memory)
{
    auto* begin = storage.begin;
    if (!begin) return;
    auto* const end = storage.end;
    DestroyRangeAt43A620(begin, end, memory);
    // 原范围固定，存储释放却在元素回调后重读 begin/capacity；不沿用入口指针。
    begin = storage.begin;
    const std::uint32_t bytes = Count(begin, storage.capacity) * 12u;
    DeleteStorage(begin, bytes, memory);
}

class SnapshotCString final
{
public:
    SnapshotCString() { InitializeEmptyRuntimeCString(value); }
    ~SnapshotCString() noexcept { DestroyRuntimeCStringAt40CD40(&value); }
    SnapshotCString(const SnapshotCString&) = delete;
    SnapshotCString& operator=(const SnapshotCString&) = delete;
    union { RuntimeCString value; };
};

class SnapshotInner final
{
public:
    SnapshotInner() noexcept = default;
    ~SnapshotInner() noexcept { DestroyRuntimeCStringVectorAt40DD20(storage); }
    SnapshotInner(const SnapshotInner&) = delete;
    SnapshotInner& operator=(const SnapshotInner&) = delete;
    Inner storage{nullptr, nullptr, nullptr};
};

void AppendSnapshotString(Inner& destination, std::string_view bytes)
{
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params nested snapshot exceeds CString length");
    SnapshotCString text;
    // 仅现代深快照：先新写完整字节，再追加；不与来源共享非空字符正文，不导入 utf16。
    SetRuntimeCStringAt437680(text.value, bytes.data(), static_cast<int>(bytes.size()));
    AppendRuntimeCStringAt41D5E9(destination, text.value);
}
}

ParamsNestedStringTable::~ParamsNestedStringTable() noexcept
{
    Destroy();
}

void ParamsNestedStringTable::Append(const Inner& inner, IEnvironmentRootMemory& memory)
{
    auto* const position = storage.end;
    if (position != storage.capacity)
    {
        // 41D38E 固定目的槽，40B6E0 返回后却对当前 outer.end 加十二。
        CopyRuntimeCStringVectorAt40B6E0(*position, inner, memory);
        storage.end = Advance(storage.end, 1);
        return;
    }

    auto* const oldBegin = storage.begin;
    const auto index = Count(oldBegin, position);
    const auto oldSize = Count(oldBegin, storage.end);
    if (oldSize == MaximumCount) throw std::length_error("vector<T> too long");
    const auto nextSize = oldSize + 1u;
    const auto capacity = Count(oldBegin, storage.capacity);
    const auto half = capacity >> 1u;
    const auto grown = capacity > MaximumCount - half ? nextSize : capacity + half;
    const auto nextCapacity = grown < nextSize ? nextSize : grown;
    const std::uint32_t nextBytes = nextCapacity * 12u;
    auto* const next = AllocateAt40A6E6(nextCapacity, memory);
    auto* const inserted = Advance(next, index);
    auto* const afterInserted = Advance(inserted, 1);
    auto* cleanupBegin = afterInserted;
    try
    {
        // 40A767 先复制新项，源允许别名旧 inner；旧三指针在复制完成前仍保持有效。
        CopyRuntimeCStringVectorAt40B6E0(*inserted, inner, memory);
        auto* const currentEnd = storage.end;
        cleanupBegin = inserted;
        if (position == currentEnd)
        {
            MoveRangeAt40AF70(storage.begin, currentEnd, next);
        }
        else
        {
            // 保留入口插入位置；不能因 reader 最初尾插就省去回调后的分支。
            MoveRangeAt40AF70(storage.begin, position, next);
            cleanupBegin = next;
            MoveRangeAt40AF70(position, storage.end, afterInserted);
        }
    }
    catch (...)
    {
        // 5E1A64 try[0,0] -> 40A850：清登记的新 inner，再按缓存容量释放新外层块。
        // inner 清理失败终止；43B710 的存储删除可抛并替换原异常，不能整体 noexcept。
        DestroyRangeAt43A620(cleanupBegin, afterInserted, memory);
        DeleteStorage(next, nextCapacity * 12u, memory);
        throw;
    }
    // 40A7A3 已撤新块回滚资格。旧存储删除失败不回收新块，也不提前发布三指针。
    ReleaseCurrentStorage(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, nextSize);
    storage.capacity = reinterpret_cast<Inner*>(Address(next) + nextBytes);
}

void ParamsNestedStringTable::Destroy(IEnvironmentRootMemory& memory) noexcept
{
    // 40E5F0 / FuncInfo 5DFBC4：整体 flags5，包括每个 inner 以及最终外层存储删除。
    if (!storage.begin) return;
    ReleaseCurrentStorage(storage, memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

std::size_t ParamsNestedStringTable::size() const noexcept
{
    return Count(storage.begin, storage.end);
}

ParamsNestedStringTable::Inner& ParamsNestedStringTable::operator[](std::size_t index) noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

const ParamsNestedStringTable::Inner& ParamsNestedStringTable::operator[](std::size_t index) const noexcept
{
    return *Advance(storage.begin, static_cast<std::uint32_t>(index));
}

void ParamsNestedStringTable::AssignSnapshot(std::span<const std::vector<ParamsText>> values)
{
    if (values.size() > MaximumCount) throw std::length_error("Params nested snapshot exceeds vector count");
    ParamsNestedStringTable snapshot;
    for (const auto& row : values)
    {
        SnapshotInner inner;
        for (const auto& text : row) AppendSnapshotString(inner.storage, text.cp932);
        // 显式快照保留空 row；不冒充生产 reader 的 inner raw count==0 跳过分支。
        snapshot.Append(inner.storage);
    }
    *this = std::move(snapshot);
}

ParamsNestedStringTable::ParamsNestedStringTable(const ParamsNestedStringTable& other)
{
    const auto count = other.size();
    if (count > MaximumCount) throw std::length_error("Params nested snapshot exceeds vector count");
    ParamsNestedStringTable snapshot;
    for (std::size_t row = 0; row < count; ++row)
    {
        const auto& source = other[row];
        const auto size = InnerCount(source);
        if (size > 0x3FFFFFFFu) throw std::length_error("Params nested snapshot exceeds inner count");
        SnapshotInner inner;
        for (std::uint32_t column = 0; column < size; ++column)
            AppendSnapshotString(inner.storage, RuntimeStringView(InnerAt(source, column)));
        snapshot.Append(inner.storage);
    }
    storage = std::exchange(snapshot.storage, Storage{});
}

ParamsNestedStringTable& ParamsNestedStringTable::operator=(const ParamsNestedStringTable& other)
{
    if (this != &other)
    {
        ParamsNestedStringTable snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsNestedStringTable::ParamsNestedStringTable(ParamsNestedStringTable&& other) noexcept
    : storage(std::exchange(other.storage, Storage{})) {}

ParamsNestedStringTable& ParamsNestedStringTable::operator=(ParamsNestedStringTable&& other) noexcept
{
    if (this != &other)
    {
        Destroy();
        storage = std::exchange(other.storage, Storage{});
    }
    return *this;
}

bool ParamsNestedStringTable::operator==(const ParamsNestedStringTable& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t row = 0; row < count; ++row)
    {
        const auto& left = (*this)[row];
        const auto& right = other[row];
        const auto innerSize = InnerCount(left);
        if (innerSize != InnerCount(right)) return false;
        for (std::uint32_t column = 0; column < innerSize; ++column)
            if (RuntimeStringView(InnerAt(left, column)) != RuntimeStringView(InnerAt(right, column))) return false;
    }
    return true;
}
}
