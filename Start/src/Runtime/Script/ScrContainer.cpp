#include <Runtime/ScrContainer.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct HeaderDefinition final
{
    std::string_view text;
    ScrVersion version;
};

constexpr std::array<HeaderDefinition, 4> Headers = {{
    {"[SCR-Ver5]", ScrVersion::Version5},
    {"[SCR-Ver5.1]", ScrVersion::Version5_1},
    {"[SCR-Ver5.2]", ScrVersion::Version5_2},
    {"[SCR-Ver5.3]", ScrVersion::Version5_3},
}};

constexpr std::array<std::byte, 6> SaveMarker = {
    std::byte{'['}, std::byte{'S'}, std::byte{'A'},
    std::byte{'V'}, std::byte{'E'}, std::byte{']'}};
constexpr std::array<std::byte, 7> LayerMarker = {
    std::byte{'['}, std::byte{'L'}, std::byte{'A'}, std::byte{'Y'},
    std::byte{'E'}, std::byte{'R'}, std::byte{']'}};

[[noreturn]] void Throw(
    const ScrContainerError error,
    const std::size_t offset,
    const char* const message)
{
    throw ScrContainerException(error, offset, message);
}

[[nodiscard]] bool HasRange(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::size_t length) noexcept
{
    return offset <= bytes.size() && length <= bytes.size() - offset;
}

[[nodiscard]] std::uint32_t ReadDword(
    const std::span<const std::byte> bytes,
    const std::size_t offset) noexcept
{
    return std::to_integer<std::uint32_t>(bytes[offset]) |
           (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 8u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 16u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 3]) << 24u);
}

[[nodiscard]] bool Matches(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::span<const std::byte> marker) noexcept
{
    return HasRange(bytes, offset, marker.size()) &&
           std::equal(marker.begin(), marker.end(), bytes.begin() + offset);
}

[[nodiscard]] std::size_t CheckedTableEnd(
    const std::size_t tableOffset,
    const std::uint32_t count,
    const std::span<const std::byte> bytes,
    const ScrContainerError error,
    const char* const message)
{
    if (count > (std::numeric_limits<std::size_t>::max)() / sizeof(std::uint32_t))
        Throw(error, tableOffset, message);
    const std::size_t tableBytes = static_cast<std::size_t>(count) * sizeof(std::uint32_t);
    if (!HasRange(bytes, tableOffset, tableBytes))
        Throw(error, tableOffset, message);
    return tableOffset + tableBytes;
}
}

ScrContainerException::ScrContainerException(
    const ScrContainerError error,
    const std::size_t offset,
    const char* const message)
    : std::runtime_error(message), error_(error), offset_(offset)
{
}

ScrContainerError ScrContainerException::Error() const noexcept
{
    return error_;
}

std::size_t ScrContainerException::Offset() const noexcept
{
    return offset_;
}

ScrContainer ParseScrContainer(
    const std::int32_t scriptId,
    std::vector<std::byte> bytes)
{
    const std::span<const std::byte> source(bytes);
    const HeaderDefinition* selected = nullptr;
    for (const HeaderDefinition& candidate : Headers)
    {
        const auto headerBytes = std::span<const std::byte>(
            reinterpret_cast<const std::byte*>(candidate.text.data()), candidate.text.size());
        if (Matches(source, 0, headerBytes))
        {
            selected = &candidate;
            break;
        }
    }
    if (selected == nullptr)
        Throw(ScrContainerError::UnsupportedHeader, 0, "[SCRIPT] : error !!");

    const std::size_t headerLength = selected->text.size();
    if (!HasRange(source, headerLength, 4u))
        Throw(ScrContainerError::CodeSizeTruncated, headerLength, "SCR code size is truncated");
    const std::uint32_t codeSize = ReadDword(source, headerLength);
    const std::size_t codeOffset = headerLength + 4u;
    if (!HasRange(source, codeOffset, codeSize))
        Throw(ScrContainerError::CodeRangeOverflow, codeOffset, "SCR code section exceeds the source buffer");

    const std::size_t saveMarkerOffset = codeOffset + codeSize;
    if (!Matches(source, saveMarkerOffset, SaveMarker))
        Throw(ScrContainerError::SaveMarkerMismatch, saveMarkerOffset, "[SAVE] : error !!");
    const std::size_t saveCountOffset = saveMarkerOffset + SaveMarker.size();
    if (!HasRange(source, saveCountOffset, 4u))
        Throw(ScrContainerError::SaveCountTruncated, saveCountOffset, "SCR save count is truncated");
    const std::uint32_t saveCount = ReadDword(source, saveCountOffset);
    const std::size_t saveTableOffset = saveCountOffset + 4u;
    const std::size_t layerMarkerOffset = CheckedTableEnd(
        saveTableOffset, saveCount, source, ScrContainerError::SaveTableOverflow,
        "SCR save offset table exceeds the source buffer");

    if (!Matches(source, layerMarkerOffset, LayerMarker))
        Throw(ScrContainerError::LayerMarkerMismatch, layerMarkerOffset, "[LAYER] : error !!");
    const std::size_t layerCountOffset = layerMarkerOffset + LayerMarker.size();
    if (!HasRange(source, layerCountOffset, 4u))
        Throw(ScrContainerError::LayerCountTruncated, layerCountOffset, "SCR layer count is truncated");
    const std::uint32_t layerCount = ReadDword(source, layerCountOffset);
    const std::size_t layerTableOffset = layerCountOffset + 4u;
    const std::size_t parsedEnd = CheckedTableEnd(
        layerTableOffset, layerCount, source, ScrContainerError::LayerTableOverflow,
        "SCR layer offset table exceeds the source buffer");

    if (parsedEnd > (std::numeric_limits<std::uint32_t>::max)())
        Throw(ScrContainerError::LayerTableOverflow, layerTableOffset,
            "SCR parsed range cannot be represented by the original x86 offset fields");

    ScrContainer result;
    if (bytes.size() > 0x7FFFFFFFu)
        Throw(ScrContainerError::CodeRangeOverflow, 0, "SCR tool byte allocation exceeds signed count");
    ByteArrayReferenceRuntime input;
    input.AllocateAt412700(static_cast<std::uint32_t>(bytes.size()));
    std::memcpy(input.Data(), bytes.data(), bytes.size());
    result.LoadAt4D5BA0(scriptId, input.RawSlot());
    return result;
}
}
