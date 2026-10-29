#include <Runtime/ScrLayerTable.h>

#include <bit>
#include <limits>
#include <span>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[noreturn]] void Throw(
    const ScrLayerTableError error,
    const std::size_t offset,
    const char* const message)
{
    throw ScrLayerTableException(error, offset, message);
}

[[nodiscard]] bool HasRange(
    const std::size_t rangeEnd,
    const std::size_t offset,
    const std::size_t size) noexcept
{
    return offset <= rangeEnd && size <= rangeEnd - offset;
}

[[nodiscard]] std::uint16_t ReadWord(
    const std::span<const std::byte> bytes,
    const std::size_t offset) noexcept
{
    return static_cast<std::uint16_t>(
        std::to_integer<std::uint16_t>(bytes[offset]) |
        (std::to_integer<std::uint16_t>(bytes[offset + 1u]) << 8u));
}

[[nodiscard]] std::uint32_t ReadDword(
    const std::span<const std::byte> bytes,
    const std::size_t offset) noexcept
{
    return std::to_integer<std::uint32_t>(bytes[offset]) |
           (std::to_integer<std::uint32_t>(bytes[offset + 1u]) << 8u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 2u]) << 16u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 3u]) << 24u);
}
}

ScrLayerTableException::ScrLayerTableException(
    const ScrLayerTableError error,
    const std::size_t offset,
    const char* const message)
    : std::runtime_error(message), error_(error), offset_(offset)
{
}

ScrLayerTableError ScrLayerTableException::Error() const noexcept
{
    return error_;
}

std::size_t ScrLayerTableException::Offset() const noexcept
{
    return offset_;
}

std::optional<ScrLayerRecord> FindScrLayerRecord(
    const ScrContainer& container,
    const std::int32_t layerIndex,
    const std::int32_t entryIndex)
{
    if (layerIndex < 0 || entryIndex < 0)
        return std::nullopt;
    if (static_cast<std::uint32_t>(layerIndex) >= container.LayerCount())
    {
        Throw(ScrLayerTableError::LayerIndexOutOfRange, container.LayerTableOffset(),
            "SCR layer index exceeds the parsed layer table");
    }

    const std::span<const std::byte> bytes = container.Bytes();
    const std::size_t tailOffset = container.ParsedEndOffset();
    const std::uint32_t relativeGroupOffset = container.LayerOffsetAt(
        static_cast<std::uint32_t>(layerIndex));
    if (relativeGroupOffset > (std::numeric_limits<std::size_t>::max)() - tailOffset)
    {
        Throw(ScrLayerTableError::LayerOffsetOutOfRange, tailOffset,
            "SCR layer group offset overflows the host address range");
    }
    const std::size_t groupOffset = tailOffset + relativeGroupOffset;
    if (!HasRange(bytes.size(), groupOffset, 1u))
    {
        Throw(ScrLayerTableError::LayerOffsetOutOfRange, groupOffset,
            "SCR layer group starts outside the source buffer");
    }

    std::size_t groupEnd = bytes.size();
    const std::uint32_t nextLayerIndex = static_cast<std::uint32_t>(layerIndex) + 1u;
    if (nextLayerIndex < container.LayerCount())
    {
        const std::uint32_t nextRelativeOffset = container.LayerOffsetAt(nextLayerIndex);
        if (nextRelativeOffset <= relativeGroupOffset)
        {
            Throw(ScrLayerTableError::LayerOffsetOrderInvalid, groupOffset,
                "SCR layer group offsets are not strictly increasing");
        }
        if (nextRelativeOffset > (std::numeric_limits<std::size_t>::max)() - tailOffset)
        {
            Throw(ScrLayerTableError::LayerOffsetOutOfRange, tailOffset,
                "SCR next layer group offset overflows the host address range");
        }
        groupEnd = tailOffset + nextRelativeOffset;
        if (groupEnd > bytes.size())
        {
            Throw(ScrLayerTableError::LayerOffsetOutOfRange, groupEnd,
                "SCR next layer group starts outside the source buffer");
        }
    }

    const std::uint8_t entryCount = std::to_integer<std::uint8_t>(bytes[groupOffset]);
    if (static_cast<std::uint32_t>(entryIndex) >= entryCount)
        return std::nullopt;

    std::size_t recordOffset = groupOffset + 1u;
    for (std::int32_t index = 0; index < entryIndex; ++index)
    {
        if (!HasRange(groupEnd, recordOffset, 2u))
        {
            Throw(ScrLayerTableError::RecordHeaderTruncated, recordOffset,
                "SCR layer record size is truncated");
        }
        const std::uint16_t recordSize = ReadWord(bytes, recordOffset);
        if (!HasRange(groupEnd, recordOffset, recordSize))
        {
            Throw(ScrLayerTableError::RecordRangeOverflow, recordOffset,
                "SCR layer record exceeds its group boundary");
        }
        recordOffset += recordSize;
    }

    constexpr std::size_t FixedRecordSize = 14u;
    if (!HasRange(groupEnd, recordOffset, FixedRecordSize))
    {
        Throw(ScrLayerTableError::RecordHeaderTruncated, recordOffset,
            "SCR selected layer record is truncated");
    }

    ScrLayerRecord result;
    result.recordSize = ReadWord(bytes, recordOffset);
    result.valueAt00 = ReadDword(bytes, recordOffset + 2u);
    result.valueAt04 = std::to_integer<std::uint8_t>(bytes[recordOffset + 6u]);
    result.valueAt08 = std::bit_cast<std::int16_t>(ReadWord(bytes, recordOffset + 7u));
    result.valueAt0C = std::bit_cast<std::int16_t>(ReadWord(bytes, recordOffset + 9u));
    result.valueAt10 = std::to_integer<std::uint8_t>(bytes[recordOffset + 11u]);
    result.valueAt18 = std::bit_cast<std::int8_t>(
        std::to_integer<std::uint8_t>(bytes[recordOffset + 12u]));

    const std::uint8_t extraCount =
        std::to_integer<std::uint8_t>(bytes[recordOffset + 13u]);
    const std::size_t extraBytes = static_cast<std::size_t>(extraCount) * 4u;
    const std::size_t requiredRecordSize = FixedRecordSize + extraBytes;
    if (result.recordSize < requiredRecordSize ||
        !HasRange(groupEnd, recordOffset, requiredRecordSize))
    {
        Throw(ScrLayerTableError::RecordPayloadOverflow, recordOffset,
            "SCR layer record extra table exceeds the declared record or group boundary");
    }
    if (!HasRange(groupEnd, recordOffset, result.recordSize))
    {
        Throw(ScrLayerTableError::RecordRangeOverflow, recordOffset,
            "SCR selected layer record exceeds its group boundary");
    }

    result.extrasAt1C.reserve(extraCount);
    for (std::size_t index = 0; index < extraCount; ++index)
        result.extrasAt1C.push_back(ReadDword(bytes, recordOffset + 14u + index * 4u));
    return result;
}
}
