#include <Runtime/ParamsByteArray.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t SnapshotCount(std::size_t count)
{
    // 现代导入不能截成负signed数量而把非空快照误当成412700清旧请求。
    if (count > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("Params byte snapshot exceeds signed array count");
    return static_cast<std::uint32_t>(count);
}
}

std::span<std::uint8_t> BorrowBoundedByteArrayCopySpan(const ByteArrayReferenceRuntime& bytes)
{
    auto* const address = bytes.DataAt(0);
    const auto& slot = bytes.RawSlot();
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    if (!control) return {address, 0};
    const auto length = static_cast<std::uint64_t>(control->countAt10) * control->strideAt0C;
    const auto begin = reinterpret_cast<std::uintptr_t>(control->payloadAt14);
    const auto resolved = reinterpret_cast<std::uintptr_t>(address);
    const auto end = static_cast<std::uint64_t>(begin) + length;
    if (!address || resolved < begin || resolved > end || end > (std::numeric_limits<std::uintptr_t>::max)())
        throw std::out_of_range("Byte array reference has no verifiable contiguous extent");
    return {address, static_cast<std::size_t>(end - resolved)};
}

ParamsByteArray::ParamsByteArray(std::initializer_list<std::byte> values)
{
    AssignSnapshot(std::span<const std::byte>(values.begin(), values.size()));
}

ParamsByteArray& ParamsByteArray::operator=(std::initializer_list<std::byte> values)
{
    AssignSnapshot(std::span<const std::byte>(values.begin(), values.size()));
    return *this;
}

void ParamsByteArray::AssignSnapshot(std::span<const std::byte> values)
{
    ParamsByteArray snapshot;
    snapshot.storage.AllocateAt412700(SnapshotCount(values.size()));
    if (!values.empty()) std::memcpy(snapshot.storage.Data(), values.data(), values.size());
    *this = std::move(snapshot);
}

ParamsByteArray::ParamsByteArray(const ParamsByteArray& other)
{
    ParamsByteArray snapshot;
    const auto count = SnapshotCount(other.size());
    snapshot.storage.AllocateAt412700(count);
    // 深快照按原Ref stride/bias取每个字节，不把非连续Ref伪装成连续span。
    for (std::uint32_t index = 0; index < count; ++index)
        snapshot[index] = other[index];
    storage = std::move(snapshot.storage);
}

ParamsByteArray& ParamsByteArray::operator=(const ParamsByteArray& other)
{
    if (this != &other)
    {
        ParamsByteArray snapshot(other);
        *this = std::move(snapshot);
    }
    return *this;
}

ParamsByteArray::ParamsByteArray(ParamsByteArray&& other) noexcept
    : storage(std::move(other.storage)) {}

ParamsByteArray& ParamsByteArray::operator=(ParamsByteArray&& other)
{
    if (this != &other) storage = std::move(other.storage);
    return *this;
}

std::byte* ParamsByteArray::Resolve(std::uint32_t index) noexcept
{
    return reinterpret_cast<std::byte*>(storage.DataAt(index));
}

const std::byte* ParamsByteArray::Resolve(std::uint32_t index) const noexcept
{
    return reinterpret_cast<const std::byte*>(storage.DataAt(index));
}

std::byte& ParamsByteArray::operator[](std::size_t index)
{
    return const_cast<std::byte&>(std::as_const(*this)[index]);
}

const std::byte& ParamsByteArray::operator[](std::size_t index) const
{
    const auto* value = Resolve(static_cast<std::uint32_t>(index));
    if (!value) throw std::out_of_range("Params byte reference resolves to null");
    return *value;
}

std::vector<std::byte> ParamsByteArray::SnapshotBytes() const
{
    const auto count = SnapshotCount(size());
    std::vector<std::byte> result(count);
    for (std::uint32_t index = 0; index < count; ++index) result[index] = (*this)[index];
    return result;
}

bool ParamsByteArray::operator==(const ParamsByteArray& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::size_t index = 0; index < count; ++index)
        if ((*this)[index] != other[index]) return false;
    return true;
}
}
