#include <Runtime/SaveDataRecordVector.h>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::uint32_t MaximumRecords = 0x0FFFFFFFu;
std::uintptr_t Address(const void* value) noexcept { return reinterpret_cast<std::uintptr_t>(value); }
std::uint32_t Count(const void* begin, const void* end) noexcept
{
    const std::uint32_t bytes = Address(end) - Address(begin);
    return (bytes >> 4u) | ((bytes & 0x80000000u) ? 0xF0000000u : 0u);
}
SaveDataRecordAt10* Advance(SaveDataRecordAt10* begin, std::uint32_t count) noexcept
{ return reinterpret_cast<SaveDataRecordAt10*>(Address(begin) + count * 16u); }

void DeleteStorage(SaveDataRecordStorage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    void* raw = storage.begin;
    std::uint32_t bytes = (Address(storage.capacity) - Address(storage.begin)) & 0xFFFFFFF0u;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&raw, reinterpret_cast<const void*>(Address(storage.begin) - 4u), sizeof(raw));
        bytes += 0x23u;
        if (Address(storage.begin) - Address(raw) - 4u > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
    }
    memory.DeleteVector(raw, bytes);
}
}
void ResizeSaveDataRecordsAt457A60(SaveDataRecordStorage& storage, std::uint32_t count,
    IEnvironmentRootMemory& memory)
{
    const auto size = Count(storage.begin, storage.end);
    const auto capacity = Count(storage.begin, storage.capacity);
    if (count <= capacity)
    {
        if (count > size) std::memset(storage.end, 0, (count - size) * 16u);
        if (count != size) storage.end = Advance(storage.begin, count);
        return;
    }
    if (count > MaximumRecords) throw std::length_error("vector<T> too long");
    const auto half = capacity >> 1u;
    const auto growth = capacity > MaximumRecords - half ? count : capacity + half;
    const auto nextCapacity = growth < count ? count : growth;
    const auto bytes = nextCapacity * 16u;
    void* raw;
    if (bytes >= 0x1000u)
    {
        const auto extended = bytes + 0x23u;
        raw = memory.AllocateBody(extended <= bytes ? 0xFFFFFFFFu : extended);
        if (!raw) _invalid_parameter_noinfo_noreturn();
        const auto aligned = (Address(raw) + 0x23u) & 0xFFFFFFE0u;
        std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
        raw = reinterpret_cast<void*>(aligned);
    }
    else raw = bytes ? memory.AllocateBody(bytes) : nullptr;
    // 小块分配返回null在原函数随后越界写；这里明确终止，不制造空记录成功。
    if (!raw) _invalid_parameter_noinfo_noreturn();
    auto* next = static_cast<SaveDataRecordAt10*>(raw);
    std::memset(Advance(next, size), 0, (count - size) * 16u);
    // 457B27在分配和尾段初始化之后才重读旧范围；不能缓存旧begin/end。
    const auto copyBytes = Address(storage.end) - Address(storage.begin);
    if (copyBytes) std::memmove(next, storage.begin, copyBytes);
    // 原函数没有EH回收新块；旧释放失败时保留旧三指针，不提前发布或回滚。
    DeleteStorage(storage, memory);
    storage.begin = next;
    storage.end = Advance(next, count);
    storage.capacity = Advance(next, nextCapacity);
}
void DestroySaveDataRecordsAt461590(SaveDataRecordStorage& storage, IEnvironmentRootMemory& memory)
{
    if (!storage.begin) return;
    DeleteStorage(storage, memory);
    storage = {};
}
SaveDataRecordVector::~SaveDataRecordVector() noexcept { DestroySaveDataRecordsAt461590(storage_); }
std::uint32_t SaveDataRecordVector::size() const noexcept { return Count(storage_.begin, storage_.end); }
bool SaveDataRecordVector::operator==(const SaveDataRecordVector& other) const noexcept
{
    if (size() != other.size()) return false;
    for (std::uint32_t i = 0; i < size(); ++i) if ((*this)[i] != other[i]) return false;
    return true;
}
}
