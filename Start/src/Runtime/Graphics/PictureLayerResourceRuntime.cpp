#include <Runtime/PictureLayerResourceRuntime.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/ArchiveLzssRuntime.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[noreturn]] void ThrowPictureLayerError(
    const PictureLayerResourceError error,
    const std::size_t offset,
    const char* const message)
{
    throw PictureLayerResourceException(error, offset, message);
}

class ByteCursor final
{
public:
    explicit ByteCursor(
        const std::span<const std::byte> bytes,
        const std::size_t sourceOffset = 0u) noexcept
        : bytes_(bytes), sourceOffset_(sourceOffset)
    {
    }

    [[nodiscard]] std::size_t Offset() const noexcept
    {
        return sourceOffset_ + offset_;
    }

    [[nodiscard]] std::span<const std::byte> Remaining() const noexcept
    {
        return bytes_.subspan(offset_);
    }

    [[nodiscard]] std::uint8_t ReadU8()
    {
        Require(1u, PictureLayerResourceError::PayloadTruncated,
            "PL payload is truncated");
        return std::to_integer<std::uint8_t>(bytes_[offset_++]);
    }

    [[nodiscard]] std::uint16_t ReadU16()
    {
        return ReadLittleEndian<std::uint16_t>();
    }

    [[nodiscard]] std::uint32_t ReadU32()
    {
        return ReadLittleEndian<std::uint32_t>();
    }

    [[nodiscard]] std::int32_t ReadI32()
    {
        return std::bit_cast<std::int32_t>(ReadU32());
    }

    [[nodiscard]] std::span<const std::byte> ReadBytes(const std::size_t count)
    {
        Require(count, PictureLayerResourceError::PayloadTruncated,
            "PL payload is truncated");
        const auto result = bytes_.subspan(offset_, count);
        offset_ += count;
        return result;
    }

private:
    template <typename Value>
    [[nodiscard]] Value ReadLittleEndian()
    {
        Require(sizeof(Value), PictureLayerResourceError::PayloadTruncated,
            "PL payload is truncated");
        Value value{};
        std::memcpy(&value, bytes_.data() + offset_, sizeof(value));
        offset_ += sizeof(value);
        return value;
    }

    void Require(
        const std::size_t count,
        const PictureLayerResourceError error,
        const char* const message) const
    {
        if (offset_ > bytes_.size() || count > bytes_.size() - offset_)
            ThrowPictureLayerError(error, Offset(), message);
    }

    std::span<const std::byte> bytes_;
    std::size_t sourceOffset_ = 0;
    std::size_t offset_ = 0;
};

[[nodiscard]] std::uint32_t ReadU32At(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::size_t sourceOffset,
    const char* const message)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint32_t))
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::PayloadTruncated,
            sourceOffset + offset,
            message);
    }
    std::uint32_t value{};
    std::memcpy(&value, bytes.data() + offset, sizeof(value));
    return value;
}

void RequireRange(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::size_t count,
    const std::size_t sourceOffset,
    const char* const message)
{
    if (offset > bytes.size() || count > bytes.size() - offset)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::PayloadTruncated,
            sourceOffset + offset,
            message);
    }
}

[[nodiscard]] std::size_t CheckedMultiply(
    const std::size_t lhs,
    const std::size_t rhs,
    const std::size_t offset)
{
    if (lhs != 0u && rhs > (std::numeric_limits<std::size_t>::max)() / lhs)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::ArithmeticOverflow,
            offset,
            "PL size arithmetic overflows size_t");
    }
    return lhs * rhs;
}

[[nodiscard]] std::size_t CheckedPixelCount(
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t channels,
    const std::size_t offset)
{
    if (width < 0 || height < 0 || channels < 0)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidGeometry,
            offset,
            "PL frame geometry or channel count is negative");
    }
    const std::size_t area = CheckedMultiply(
        static_cast<std::size_t>(width),
        static_cast<std::size_t>(height), offset);
    return CheckedMultiply(area, static_cast<std::size_t>(channels), offset);
}

[[nodiscard]] std::size_t CheckedAdd(
    const std::size_t lhs,
    const std::size_t rhs,
    const std::size_t offset)
{
    if (rhs > (std::numeric_limits<std::size_t>::max)() - lhs)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::ArithmeticOverflow,
            offset,
            "PL offset arithmetic overflows size_t");
    }
    return lhs + rhs;
}

[[nodiscard]] std::int32_t AddI32Wrapping(
    const std::int32_t lhs,
    const std::int32_t rhs) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(lhs) +
        std::bit_cast<std::uint32_t>(rhs));
}

[[nodiscard]] std::vector<std::byte> CopyBytes(
    const std::span<const std::byte> bytes)
{
    return {bytes.begin(), bytes.end()};
}

void AddDifference(
    std::vector<std::byte>& destination,
    const std::span<const std::byte> previous)
{
    for (std::size_t index = 0; index < destination.size(); ++index)
    {
        const auto current = std::to_integer<std::uint8_t>(destination[index]);
        const auto prior = std::to_integer<std::uint8_t>(previous[index]);
        destination[index] = static_cast<std::byte>(
            static_cast<std::uint8_t>(current + prior));
    }
}

class MsbBitReader final
{
public:
    MsbBitReader(
        const std::span<const std::byte> bytes,
        const std::size_t sourceOffset) noexcept
        : bytes_(bytes), sourceOffset_(sourceOffset)
    {
    }

    [[nodiscard]] std::uint32_t ReadBit(
        const PictureLayerResourceError error,
        const char* const message)
    {
        if (bitsRemaining_ == 0u)
        {
            if (byteOffset_ >= bytes_.size())
                ThrowPictureLayerError(error, sourceOffset_ + byteOffset_, message);
            current_ = std::to_integer<std::uint8_t>(bytes_[byteOffset_++]);
            bitsRemaining_ = 8u;
        }
        --bitsRemaining_;
        return (current_ >> bitsRemaining_) & 1u;
    }

    [[nodiscard]] std::uint32_t ReadBits(
        const std::uint32_t count,
        const PictureLayerResourceError error,
        const char* const message)
    {
        std::uint32_t result = 0u;
        for (std::uint32_t index = 0; index < count; ++index)
            result = (result << 1u) | ReadBit(error, message);
        return result;
    }

    [[nodiscard]] std::size_t Offset() const noexcept
    {
        return sourceOffset_ + byteOffset_;
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t sourceOffset_ = 0;
    std::size_t byteOffset_ = 0;
    std::uint32_t bitsRemaining_ = 0;
    std::uint32_t current_ = 0;
};

struct HuffmanNode final
{
    std::int32_t left = 0;
    std::int32_t right = 0;
};

[[nodiscard]] std::int32_t ReadHuffmanNode(
    MsbBitReader& bits,
    std::array<HuffmanNode, 255>& nodes,
    std::int32_t& nextInternal)
{
    if (bits.ReadBit(
            PictureLayerResourceError::HuffmanBitstreamExhausted,
            "PL Huffman tree bitstream is truncated") == 0u)
    {
        return static_cast<std::int32_t>(bits.ReadBits(
            8u,
            PictureLayerResourceError::HuffmanBitstreamExhausted,
            "PL Huffman leaf is truncated"));
    }

    // 43E760：内部 token 从 256 开始，达到 511 时抛错。
    if (nextInternal >= 511)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidHuffmanTree,
            bits.Offset(),
            "PL Huffman tree has more than 255 internal nodes");
    }
    const std::int32_t token = nextInternal++;
    HuffmanNode& node = nodes[static_cast<std::size_t>(token - 256)];
    node.left = ReadHuffmanNode(bits, nodes, nextInternal);
    node.right = ReadHuffmanNode(bits, nodes, nextInternal);
    return token;
}

[[nodiscard]] std::vector<std::byte> DecodeHuffman(
    const std::span<const std::byte> stream,
    const std::size_t decodedSize,
    const std::size_t sourceOffset)
{
    MsbBitReader bits(stream, sourceOffset);
    std::array<HuffmanNode, 255> nodes{};
    std::int32_t nextInternal = 256;
    const std::int32_t root = ReadHuffmanNode(bits, nodes, nextInternal);
    std::vector<std::byte> output(decodedSize);
    for (std::byte& value : output)
    {
        std::int32_t token = root;
        while (token >= 256)
        {
            const HuffmanNode& node =
                nodes[static_cast<std::size_t>(token - 256)];
            token = bits.ReadBit(
                PictureLayerResourceError::HuffmanBitstreamExhausted,
                "PL Huffman payload is truncated") != 0u
                ? node.right
                : node.left;
        }
        value = static_cast<std::byte>(static_cast<std::uint8_t>(token));
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodeInterleavedRleDifference(
    const std::span<const std::byte> packed,
    const std::span<const std::byte> previous,
    const std::uint8_t step,
    const std::size_t sourceOffset)
{
    if (step == 0u || static_cast<std::size_t>(step) > previous.size())
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidRleStep,
            sourceOffset,
            "PL10 RLE step cannot address the decoded frame");
    }

    std::size_t source = 0u;
    const auto readPacked = [&]() -> std::uint8_t
    {
        if (source >= packed.size())
        {
            ThrowPictureLayerError(
                PictureLayerResourceError::PayloadTruncated,
                sourceOffset + source,
                "PL10 RLE payload is truncated");
        }
        return std::to_integer<std::uint8_t>(packed[source++]);
    };

    std::vector<std::byte> output(previous.size());
    for (std::size_t lane = 0u; lane < step; ++lane)
    {
        std::uint8_t previousDifference = readPacked();
        output[lane] = static_cast<std::byte>(static_cast<std::uint8_t>(
            std::to_integer<std::uint8_t>(previous[lane]) + previousDifference));
        std::size_t destination = lane + step;
        while (destination < output.size())
        {
            std::uint8_t difference = readPacked();
            output[destination] = static_cast<std::byte>(static_cast<std::uint8_t>(
                std::to_integer<std::uint8_t>(previous[destination]) + difference));
            destination += step;

            if (difference == previousDifference)
            {
                std::uint32_t count = readPacked();
                if ((count & 0x80u) != 0u)
                    count = readPacked() + ((count & 0x7Fu) << 8u) + 128u;

                const std::size_t remaining = destination < output.size()
                    ? 1u + (output.size() - 1u - destination) / step
                    : 0u;
                if (count > remaining)
                {
                    ThrowPictureLayerError(
                        PictureLayerResourceError::DecodedSizeMismatch,
                        sourceOffset + source,
                        "PL10 RLE run writes past the decoded frame");
                }
                while (count-- != 0u)
                {
                    output[destination] = static_cast<std::byte>(
                        static_cast<std::uint8_t>(
                            std::to_integer<std::uint8_t>(previous[destination]) +
                            difference));
                    destination += step;
                }
                if (destination < output.size())
                {
                    difference = readPacked();
                    output[destination] = static_cast<std::byte>(
                        static_cast<std::uint8_t>(
                            std::to_integer<std::uint8_t>(previous[destination]) +
                            difference));
                    destination += step;
                }
            }
            previousDifference = difference;
        }
    }
    return output;
}

void UndoMoveToFront(std::vector<std::byte>& bytes)
{
    std::array<std::uint8_t, 256> table{};
    for (std::size_t index = 0u; index < table.size(); ++index)
        table[index] = static_cast<std::uint8_t>(index);

    for (std::byte& encoded : bytes)
    {
        const std::uint8_t index = std::to_integer<std::uint8_t>(encoded);
        const std::uint8_t value = table[index];
        for (std::size_t cursor = index; cursor > 0u; --cursor)
            table[cursor] = table[cursor - 1u];
        table[0] = value;
        encoded = static_cast<std::byte>(value);
    }
}

[[nodiscard]] std::vector<std::byte> UndoBurrowsWheeler(
    const std::span<const std::byte> input,
    const std::int32_t key,
    const std::size_t sourceOffset)
{
    if (input.empty())
    {
        if (key != 0)
        {
            ThrowPictureLayerError(
                PictureLayerResourceError::InvalidBwtKey,
                sourceOffset,
                "BMR BWT key is invalid for an empty payload");
        }
        return {};
    }
    if (key < 0 || static_cast<std::size_t>(key) >= input.size())
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBwtKey,
            sourceOffset,
            "BMR BWT key is outside the decoded payload");
    }

    std::array<std::size_t, 256> frequencies{};
    for (const std::byte value : input)
        ++frequencies[std::to_integer<std::uint8_t>(value)];
    for (std::size_t index = 1u; index < frequencies.size(); ++index)
        frequencies[index] += frequencies[index - 1u];

    std::vector<std::size_t> distribution(input.size());
    for (std::size_t index = input.size(); index-- > 0u;)
    {
        const std::uint8_t value = std::to_integer<std::uint8_t>(input[index]);
        distribution[--frequencies[value]] = index;
    }

    std::vector<std::byte> output(input.size());
    std::size_t position = static_cast<std::size_t>(key);
    for (std::byte& value : output)
    {
        position = distribution[position];
        value = input[position];
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodeRleToSize(
    const std::span<const std::byte> input,
    const std::size_t finalSize,
    const std::uint8_t step,
    const std::size_t sourceOffset)
{
    if (step == 0u || static_cast<std::size_t>(step) > finalSize)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidRleStep,
            sourceOffset,
            "BMR RLE step cannot address the output");
    }
    std::size_t source = 0u;
    const auto readByte = [&]() -> std::uint8_t
    {
        if (source >= input.size())
        {
            ThrowPictureLayerError(
                PictureLayerResourceError::PayloadTruncated,
                sourceOffset + source,
                "BMR RLE payload is truncated");
        }
        return std::to_integer<std::uint8_t>(input[source++]);
    };

    std::vector<std::byte> output(finalSize);
    for (std::size_t lane = 0u; lane < step; ++lane)
    {
        std::uint8_t previous = readByte();
        output[lane] = static_cast<std::byte>(previous);
        std::size_t destination = lane + step;
        while (destination < output.size())
        {
            std::uint8_t current = readByte();
            output[destination] = static_cast<std::byte>(current);
            destination += step;
            if (current == previous)
            {
                std::uint32_t count = readByte();
                if ((count & 0x80u) != 0u)
                    count = readByte() + ((count & 0x7Fu) << 8u) + 128u;
                const std::size_t remaining = destination < output.size()
                    ? 1u + (output.size() - 1u - destination) / step
                    : 0u;
                if (count > remaining)
                {
                    ThrowPictureLayerError(
                        PictureLayerResourceError::DecodedSizeMismatch,
                        sourceOffset + source,
                        "BMR RLE run writes past its declared output");
                }
                while (count-- != 0u)
                {
                    output[destination] = static_cast<std::byte>(current);
                    destination += step;
                }
                if (destination < output.size())
                {
                    current = readByte();
                    output[destination] = static_cast<std::byte>(current);
                    destination += step;
                }
            }
            previous = current;
        }
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodeBmr(
    const std::span<const std::byte> payload,
    const std::size_t sourceOffset)
{
    if (payload.size() < 20u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBmrHeader,
            sourceOffset,
            "PL20 BMR header is truncated");
    }
    if (payload[0] != static_cast<std::byte>('B') ||
        payload[1] != static_cast<std::byte>('M') ||
        payload[2] != static_cast<std::byte>('R'))
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBmrHeader,
            sourceOffset,
            "PL20 mode 3 payload has no BMR magic");
    }

    const std::uint8_t step = std::to_integer<std::uint8_t>(payload[3]);
    const std::int32_t finalSizeRaw = std::bit_cast<std::int32_t>(
        ReadU32At(payload, 4u, sourceOffset, "BMR final size is truncated"));
    const std::int32_t key = std::bit_cast<std::int32_t>(
        ReadU32At(payload, 8u, sourceOffset, "BMR BWT key is truncated"));
    const std::int32_t intermediateSizeRaw = std::bit_cast<std::int32_t>(
        ReadU32At(payload, 12u, sourceOffset, "BMR intermediate size is truncated"));
    const std::int32_t huffmanSizeRaw = std::bit_cast<std::int32_t>(
        ReadU32At(payload, 16u, sourceOffset, "BMR Huffman size is truncated"));
    if (finalSizeRaw < 0 || intermediateSizeRaw < 0 || huffmanSizeRaw < 0)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBmrHeader,
            sourceOffset + 4u,
            "BMR contains a negative size");
    }

    const auto huffmanSize = static_cast<std::size_t>(huffmanSizeRaw);
    RequireRange(payload, 20u, huffmanSize, sourceOffset,
        "BMR Huffman stream is truncated");
    std::vector<std::byte> transformed = DecodeHuffman(
        payload.subspan(20u, huffmanSize),
        static_cast<std::size_t>(intermediateSizeRaw),
        sourceOffset + 20u);
    UndoMoveToFront(transformed);
    std::vector<std::byte> decoded = UndoBurrowsWheeler(
        transformed, key, sourceOffset + 8u);

    const std::size_t finalSize = static_cast<std::size_t>(finalSizeRaw);
    if (step != 0u)
        return DecodeRleToSize(decoded, finalSize, step, sourceOffset + 3u);
    if (decoded.size() != finalSize)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::DecodedSizeMismatch,
            sourceOffset + 4u,
            "BMR decoded size differs from its final size");
    }
    return decoded;
}

[[nodiscard]] std::vector<std::byte> DecodeLzss(
    const std::span<const std::byte> payload,
    const std::size_t sourceOffset)
{
    if (payload.size() < 4u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidLzssStream,
            sourceOffset,
            "PL20 LZSS block has no decoded-size field");
    }
    const std::int32_t decodedSizeRaw = std::bit_cast<std::int32_t>(
        ReadU32At(payload, 0u, sourceOffset, "LZSS decoded size is truncated"));
    if (decodedSizeRaw < 0)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidLzssStream,
            sourceOffset,
            "PL20 LZSS decoded size is negative");
    }

    const std::size_t decodedSize = static_cast<std::size_t>(decodedSizeRaw);
    std::vector<std::byte> output(decodedSize);
    std::array<std::byte, 0x1000> window{};
    std::size_t destination = 0u;
    std::size_t windowPosition = 1u;
    MsbBitReader bits(payload.subspan(4u), sourceOffset + 4u);

    for (;;)
    {
        if (bits.ReadBit(
                PictureLayerResourceError::InvalidLzssStream,
                "PL20 LZSS stream is truncated") != 0u)
        {
            if (destination >= output.size())
            {
                ThrowPictureLayerError(
                    PictureLayerResourceError::DecodedSizeMismatch,
                    sourceOffset,
                    "PL20 LZSS writes past its decoded size");
            }
            const auto value = static_cast<std::byte>(bits.ReadBits(
                8u,
                PictureLayerResourceError::InvalidLzssStream,
                "PL20 LZSS literal is truncated"));
            output[destination++] = value;
            window[windowPosition] = value;
            windowPosition = (windowPosition + 1u) & 0xFFFu;
            continue;
        }

        const std::uint32_t source = bits.ReadBits(
            12u,
            PictureLayerResourceError::InvalidLzssStream,
            "PL20 LZSS reference is truncated");
        if (source == 0u)
            break;
        const std::uint32_t length = bits.ReadBits(
            4u,
            PictureLayerResourceError::InvalidLzssStream,
            "PL20 LZSS length is truncated") + 2u;
        if (length > output.size() - destination)
        {
            ThrowPictureLayerError(
                PictureLayerResourceError::DecodedSizeMismatch,
                sourceOffset,
                "PL20 LZSS reference writes past its decoded size");
        }
        for (std::uint32_t index = 0u; index < length; ++index)
        {
            const std::byte value = window[(source + index) & 0xFFFu];
            output[destination++] = value;
            window[windowPosition] = value;
            windowPosition = (windowPosition + 1u) & 0xFFFu;
        }
    }

    if (destination != output.size())
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::DecodedSizeMismatch,
            sourceOffset,
            "PL20 LZSS terminated before filling its decoded size");
    }
    return output;
}

class BlockControlReader final
{
public:
    BlockControlReader(
        const std::span<const std::byte> bytes,
        const std::size_t sourceOffset) noexcept
        : bytes_(bytes), sourceOffset_(sourceOffset)
    {
    }

    [[nodiscard]] bool ReadBit()
    {
        if (byteOffset_ >= bytes_.size())
        {
            ThrowPictureLayerError(
                PictureLayerResourceError::BlockControlExhausted,
                sourceOffset_ + byteOffset_,
                "PL30 block control stream is exhausted");
        }
        const std::uint8_t value = std::to_integer<std::uint8_t>(bytes_[byteOffset_]);
        const bool result = (value & (1u << (7u - bitOffset_))) != 0u;
        if (++bitOffset_ == 8u)
        {
            bitOffset_ = 0u;
            ++byteOffset_;
        }
        return result;
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t sourceOffset_ = 0u;
    std::size_t byteOffset_ = 0u;
    std::uint8_t bitOffset_ = 0u;
};

void CopyImageBlock(
    const std::span<const std::byte> source,
    std::span<std::byte> destination,
    const std::size_t imageWidth,
    const std::size_t channels,
    const std::size_t blockX,
    const std::size_t blockY,
    const std::size_t blockWidth,
    const std::size_t blockHeight)
{
    const std::size_t rowBytes = blockWidth * channels;
    for (std::size_t row = 0u; row < blockHeight; ++row)
    {
        const std::size_t offset =
            ((blockY + row) * imageWidth + blockX) * channels;
        std::memmove(destination.data() + offset, source.data() + offset, rowBytes);
    }
}

[[nodiscard]] std::vector<std::byte> DecodeBlockConvert(
    const std::span<const std::byte> payload,
    const std::span<const std::byte> previous,
    const std::int32_t widthRaw,
    const std::int32_t heightRaw,
    const std::int32_t channelsRaw,
    const std::uint8_t blockSizeRaw,
    const std::size_t sourceOffset)
{
    if (blockSizeRaw == 0u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBlockSize,
            sourceOffset,
            "PL30 block size is zero");
    }
    const std::size_t pixelCount = CheckedPixelCount(
        widthRaw, heightRaw, channelsRaw, sourceOffset);
    if (previous.size() != pixelCount)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::DecodedSizeMismatch,
            sourceOffset,
            "PL30 previous frame size is inconsistent");
    }
    RequireRange(payload, 0u, 10u, sourceOffset,
        "PL30 block-convert header is truncated");

    const std::size_t controlSize =
        std::to_integer<std::uint8_t>(payload[4]) |
        (static_cast<std::size_t>(std::to_integer<std::uint8_t>(payload[5])) << 8u);
    const std::size_t tableSizeOffset = CheckedAdd(6u, controlSize, sourceOffset);
    RequireRange(payload, 6u, controlSize, sourceOffset,
        "PL30 block control stream is truncated");
    const std::size_t tableSize = ReadU32At(
        payload, tableSizeOffset, sourceOffset,
        "PL30 block table-size field is truncated");
    const std::size_t tableBase = CheckedAdd(tableSizeOffset, 4u, sourceOffset);
    const std::size_t rawBase = CheckedAdd(tableBase, tableSize, sourceOffset);
    RequireRange(payload, tableBase, tableSize, sourceOffset,
        "PL30 block table is truncated");
    RequireRange(payload, rawBase, 0u, sourceOffset,
        "PL30 raw pixel area is outside the payload");

    const std::size_t width = static_cast<std::size_t>(widthRaw);
    const std::size_t height = static_cast<std::size_t>(heightRaw);
    const std::size_t channels = static_cast<std::size_t>(channelsRaw);
    const std::size_t blockSize = blockSizeRaw;
    const std::size_t maskBytes = CheckedMultiply(
        blockSize, blockSize, sourceOffset) >> 3u;
    const std::size_t bitRowStride = blockSize < 8u ? blockSize : 0u;
    std::vector<std::byte> output(pixelCount);
    BlockControlReader control(payload.subspan(6u, controlSize), sourceOffset + 6u);
    std::size_t tableCursor = tableBase;
    std::size_t rawCursor = rawBase;

    for (std::size_t blockY = 0u; blockY < height; blockY += blockSize)
    {
        const std::size_t blockHeight = std::min(blockSize, height - blockY);
        for (std::size_t blockX = 0u; blockX < width; blockX += blockSize)
        {
            const std::size_t blockWidth = std::min(blockSize, width - blockX);
            std::size_t commandIndex = 0u;
            while (commandIndex < 3u && control.ReadBit())
                ++commandIndex;
            const std::uint8_t command =
                std::to_integer<std::uint8_t>(payload[commandIndex]);

            switch (command)
            {
            case 0u:
                CopyImageBlock(previous, output, width, channels,
                    blockX, blockY, blockWidth, blockHeight);
                break;

            case 1u:
            {
                const std::size_t rowBytes = CheckedMultiply(
                    blockWidth, channels, sourceOffset + rawCursor);
                const std::size_t byteCount = CheckedMultiply(
                    rowBytes, blockHeight, sourceOffset + rawCursor);
                RequireRange(payload, rawCursor, byteCount, sourceOffset,
                    "PL30 raw block is truncated");
                for (std::size_t row = 0u; row < blockHeight; ++row)
                {
                    const std::size_t destination =
                        ((blockY + row) * width + blockX) * channels;
                    std::memmove(output.data() + destination,
                        payload.data() + rawCursor + row * rowBytes,
                        rowBytes);
                }
                rawCursor += byteCount;
                break;
            }

            case 2u:
                RequireRange(payload, tableCursor, maskBytes, sourceOffset,
                    "PL30 mask table is truncated");
                for (std::size_t row = 0u; row < blockHeight; ++row)
                {
                    for (std::size_t column = 0u; column < blockWidth; ++column)
                    {
                        const std::size_t tableBit = column + row * blockSize;
                        const std::size_t tableByte = tableBit >> 3u;
                        const std::size_t bit =
                            (column + row * bitRowStride) & 7u;
                        if (tableByte >= maskBytes)
                        {
                            ThrowPictureLayerError(
                                PictureLayerResourceError::PayloadTruncated,
                                sourceOffset + tableCursor + tableByte,
                                "PL30 mask bit addresses outside its table");
                        }
                        const std::size_t destination =
                            ((blockY + row) * width + blockX + column) * channels;
                        const bool replace =
                            (std::to_integer<std::uint8_t>(
                                payload[tableCursor + tableByte]) &
                             (1u << (7u - bit))) != 0u;
                        if (replace)
                        {
                            RequireRange(payload, rawCursor, channels, sourceOffset,
                                "PL30 mask-selected pixel is truncated");
                            std::memmove(output.data() + destination,
                                payload.data() + rawCursor, channels);
                            rawCursor += channels;
                        }
                        else
                        {
                            std::memmove(output.data() + destination,
                                previous.data() + destination, channels);
                        }
                    }
                }
                tableCursor += maskBytes;
                break;

            case 3u:
            {
                RequireRange(payload, tableCursor, 1u, sourceOffset,
                    "PL30 sparse table count is truncated");
                const std::size_t sparseCount =
                    std::to_integer<std::uint8_t>(payload[tableCursor++]);
                RequireRange(payload, tableCursor, sparseCount, sourceOffset,
                    "PL30 sparse table is truncated");
                std::size_t sparseIndex = 0u;
                for (std::size_t row = 0u; row < blockHeight; ++row)
                {
                    for (std::size_t column = 0u; column < blockWidth; ++column)
                    {
                        const std::size_t compactIndex = column + row * blockSize;
                        const std::size_t destination =
                            ((blockY + row) * width + blockX + column) * channels;
                        const bool replace = sparseIndex < sparseCount &&
                            std::to_integer<std::uint8_t>(
                                payload[tableCursor + sparseIndex]) == compactIndex;
                        if (replace)
                        {
                            RequireRange(payload, rawCursor, channels, sourceOffset,
                                "PL30 sparse-selected pixel is truncated");
                            std::memmove(output.data() + destination,
                                payload.data() + rawCursor, channels);
                            rawCursor += channels;
                            ++sparseIndex;
                        }
                        else
                        {
                            std::memmove(output.data() + destination,
                                previous.data() + destination, channels);
                        }
                    }
                }
                tableCursor += sparseCount;
                break;
            }

            default:
                // 445300 的 default 分支不写目标、不推进 table/raw 游标，也不抛错。
                break;
            }
        }
    }
    return output;
}

[[nodiscard]] PictureLayerRect ReadGlobalRect(ByteCursor& cursor)
{
    return PictureLayerRect{
        cursor.ReadI32(), cursor.ReadI32(), cursor.ReadI32(), cursor.ReadI32()};
}

[[nodiscard]] PictureLayerFrame MakeFrame(
    const std::int32_t fileX,
    const std::int32_t fileY,
    const std::int32_t surfaceX,
    const std::int32_t surfaceY,
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t channels,
    const bool createWithAlpha,
    std::vector<std::byte> pixels)
{
    return PictureLayerFrame{
        fileX, fileY, surfaceX, surfaceY,
        width, height, channels, createWithAlpha, std::move(pixels)};
}

void RequireDecodedPixelCount(
    const std::vector<std::byte>& pixels,
    const std::size_t expected,
    const std::size_t offset)
{
    if (pixels.size() != expected)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::DecodedSizeMismatch,
            offset,
            "PL decoded payload size differs from frame geometry");
    }
}

// 两个编译期策略把旧工具算法与生产显式依赖分开，不以空services选择local fallback。
struct ToolPictureCodecs final
{
    [[noreturn]] void NotPicture() const
    {
        ThrowPictureLayerError(PictureLayerResourceError::UnsupportedMagic, 0,
            "Picture-layer resource has no PL magic");
    }
    void BeginDifference(std::size_t) const noexcept {}
    void Finish() const noexcept {}
    std::vector<std::byte> Difference(ByteCursor& cursor, std::span<const std::byte> previous) const
    {
        const auto size = cursor.ReadU32();
        const auto offset = cursor.Offset();
        auto output = DecodeHuffman(cursor.ReadBytes(size), previous.size(), offset);
        AddDifference(output, previous);
        return output;
    }
    std::vector<std::byte> Frame(ByteCursor& cursor, std::uint16_t mode,
        std::uint32_t packedSize, std::size_t expected) const
    {
        const auto offset = cursor.Offset();
        const auto packed = cursor.ReadBytes(packedSize);
        auto output = mode == 3 ? DecodeBmr(packed, offset) : DecodeLzss(packed, offset);
        RequireDecodedPixelCount(output, expected, offset);
        return output;
    }
};

class NativePictureCodecs final
{
public:
    explicit NativePictureCodecs(const ArchiveCodecServices& services) noexcept : services_(services) {}

    [[noreturn]] void NotPicture() const { throw 3; } // 4483D3原非PL分支

    void BeginDifference(std::size_t size)
    {
        // 44714A：PL11在循环外拥有2*N字节，两个N字节区轮换；不初始化其正文。
        const auto count = CheckedMultiply(size, 2, 0);
        scratch_.AllocateAt412700(static_cast<std::uint32_t>(count));
    }

    std::vector<std::byte> Difference(ByteCursor& cursor, std::span<const std::byte> previous)
    {
        const auto payload = AsInput(cursor.Remaining());
        const auto available = scratch_.MutableBytes();
        // 448437先取owner/GetHuffman，44851F才读取固定payload的长度DWORD。
        auto* const decoder = GetHuffmanAt43DE24(Capture());
        if (!decoder) throw std::logic_error("PL11 Huffman body is null (safe boundary)");
        if (differenceOffset_ > available.size() || previous.size() > available.size() - differenceOffset_)
            ThrowPictureLayerError(PictureLayerResourceError::DecodedSizeMismatch,
                cursor.Offset(), "PL11 output exceeds decoded allocation (safe boundary)");
        auto output = available.subspan(differenceOffset_, previous.size());
        decoder->DecodeAt43DE24(payload, output, nullptr, nullptr, services_.errors);
        for (std::size_t index = 0; index < previous.size(); ++index)
            output[index] = static_cast<std::uint8_t>(output[index] +
                std::to_integer<std::uint8_t>(previous[index]));
        // 4473FD只在解码+差分成功之后重读原前缀并推进caller局部游标。
        const auto packedSize = cursor.ReadU32();
        static_cast<void>(cursor.ReadBytes(packedSize));
        differenceOffset_ ^= previous.size();
        return CopyBytes(std::as_bytes(output));
    }

    std::vector<std::byte> Frame(ByteCursor& cursor, std::uint16_t mode,
        std::uint32_t packedSize, std::size_t expected)
    {
        const auto payload = AsInput(cursor.Remaining());
        if (mode == 3)
        {
            // 44774F/4477A7两次独立取源；Identify所得input跨目标分配保持借用，
            // 第二次GetBmr可指向另一正文，不能缓存首次decoder替代晚取。
            auto* decoder = GetBmrAt445110(Capture());
            if (!decoder) throw std::logic_error("PL20 BMR body is null (safe boundary)");
            std::uint32_t count;
            const auto* const input = decoder->IdentifyAt445A00(payload, count);
            if (!input)
                ThrowPictureLayerError(PictureLayerResourceError::InvalidBmrHeader,
                    cursor.Offset(), "PL20 BMR Identify did not produce an input (safe boundary)");
            ResizeScratch(count);
            auto output = scratch_.MutableBytes();
            decoder = GetBmrAt445110(Capture());
            if (!decoder) throw std::logic_error("PL20 BMR body is null (safe boundary)");
            decoder->DecodeAt43DCF0({input, payload.size() - 16}, output,
                services_.managers, services_.errors);
        }
        else
        {
            // 4477CC先读输出长度、按需分配；447809之后才取共享LZSS。
            const auto count = ReadU32At(cursor.Remaining(), 0, cursor.Offset(),
                "PL20 LZSS output size is truncated");
            ResizeScratch(count);
            auto output = scratch_.MutableBytes();
            auto* const decoder = GetLzssAt4840B0(Capture());
            if (!decoder) throw std::logic_error("PL20 LZSS body is null (safe boundary)");
            decoder->DecodeAt445750(payload.subspan(4), output);
        }
        // 原445750只以零offset结束；同尺寸临时byteRef保留前一帧尾部，既不补0，
        // 也不要求输出量/长度DWORD等于像素数。首次未写的上传字节不在合法借用域。
        const auto output = scratch_.Bytes();
        if (expected > output.size())
            ThrowPictureLayerError(PictureLayerResourceError::DecodedSizeMismatch,
                cursor.Offset(), "PL20 pixel upload exceeds decoded allocation (safe boundary)");
        auto result = CopyBytes(std::as_bytes(output.first(expected)));
        static_cast<void>(cursor.ReadBytes(packedSize));
        return result;
    }

    void Finish() noexcept
    {
        // 447487/447970先撤展开资格，正常固定control；原state2是terminate边界。
        auto* const control = scratch_.RawSlot().controlAt08;
        scratch_.RawSlot() = {};
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
private:
    static std::span<const std::uint8_t> AsInput(std::span<const std::byte> bytes) noexcept
    {
        return {reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size()};
    }
    ArchiveCodecManagerRuntime& Capture()
    {
        auto* const manager = services_.managers.CaptureAt445910();
        if (!manager) throw std::logic_error("PL codec manager is null (safe boundary)");
        return *manager;
    }
    void ResizeScratch(std::uint32_t count)
    {
        if (!scratch_.RawSlot().baseAt00 || scratch_.Size() != count)
            scratch_.AllocateAt412700(count);
    }
    const ArchiveCodecServices& services_;
    ByteArrayReferenceRuntime scratch_;
    std::size_t differenceOffset_ = 0;
};

[[nodiscard]] ParsedPictureLayerResource ParsePl00(ByteCursor& cursor)
{
    ParsedPictureLayerResource result;
    result.version = PictureLayerVersion::Pl00;
    const std::uint16_t count = cursor.ReadU16();
    if (count == 0u)
        ThrowPictureLayerError(PictureLayerResourceError::EmptyFrameSet,
            cursor.Offset() - 2u, "PL00 has no frames");
    result.globalRect = ReadGlobalRect(cursor);
    result.frames.reserve(count);
    for (std::uint16_t index = 0u; index < count; ++index)
    {
        const std::int32_t x = cursor.ReadI32();
        const std::int32_t y = cursor.ReadI32();
        const std::int32_t width = cursor.ReadI32();
        const std::int32_t height = cursor.ReadI32();
        const std::int32_t channels = cursor.ReadI32();
        const std::size_t size = CheckedPixelCount(
            width, height, channels, cursor.Offset());
        result.frames.push_back(MakeFrame(
            x, y, x, y, width, height, channels, true,
            CopyBytes(cursor.ReadBytes(size))));
    }
    return result;
}

[[nodiscard]] ParsedPictureLayerResource ParsePl01(ByteCursor& cursor)
{
    ParsedPictureLayerResource result;
    result.version = PictureLayerVersion::Pl01;
    const std::uint16_t count = cursor.ReadU16();
    if (count == 0u)
        ThrowPictureLayerError(PictureLayerResourceError::EmptyFrameSet,
            cursor.Offset() - 2u, "PL01 has no frames");
    result.globalRect = ReadGlobalRect(cursor);
    const std::int32_t channels = cursor.ReadI32();
    const std::size_t size = CheckedPixelCount(
        result.globalRect.width, result.globalRect.height,
        channels, cursor.Offset());
    result.frames.reserve(count);
    std::vector<std::byte> previous = CopyBytes(cursor.ReadBytes(size));
    result.frames.push_back(MakeFrame(
        0, 0, 0, 0, result.globalRect.width, result.globalRect.height,
        channels, channels == 4, previous));

    for (std::uint16_t index = 1u; index < count; ++index)
    {
        std::vector<std::byte> current = CopyBytes(cursor.ReadBytes(size));
        AddDifference(current, previous);
        result.frames.push_back(MakeFrame(
            0, 0, 0, 0, result.globalRect.width, result.globalRect.height,
            channels, channels == 4, current));
        previous = std::move(current);
    }
    return result;
}

template<class Codecs>
[[nodiscard]] ParsedPictureLayerResource ParsePl10Or11(
    ByteCursor& cursor,
    const bool huffman, Codecs& codecs)
{
    ParsedPictureLayerResource result;
    result.version = huffman ? PictureLayerVersion::Pl11 : PictureLayerVersion::Pl10;
    const std::uint16_t count = cursor.ReadU16();
    if (count == 0u)
        ThrowPictureLayerError(PictureLayerResourceError::EmptyFrameSet,
            cursor.Offset() - 2u, huffman ? "PL11 has no frames" : "PL10 has no frames");
    result.globalRect = ReadGlobalRect(cursor);
    const std::int32_t x = cursor.ReadI32();
    const std::int32_t y = cursor.ReadI32();
    const std::int32_t width = cursor.ReadI32();
    const std::int32_t height = cursor.ReadI32();
    const std::int32_t channels = cursor.ReadI32();
    const std::size_t size = CheckedPixelCount(
        width, height, channels, cursor.Offset());
    result.frames.reserve(count);
    std::vector<std::byte> previous = CopyBytes(cursor.ReadBytes(size));
    result.frames.push_back(MakeFrame(
        x, y, x, y, width, height, channels, true, previous));

    if (huffman)
    {
        codecs.BeginDifference(size);
        result.pl11UnknownWord = cursor.ReadU16();
    }

    for (std::uint16_t index = 1u; index < count; ++index)
    {
        std::vector<std::byte> current;
        if (huffman)
        {
            current = codecs.Difference(cursor, previous);
        }
        else
        {
            const std::uint8_t step = cursor.ReadU8();
            const std::uint32_t packedSize = cursor.ReadU32();
            const std::size_t packedOffset = cursor.Offset();
            current = DecodeInterleavedRleDifference(
                cursor.ReadBytes(packedSize), previous, step, packedOffset);
        }
        result.frames.push_back(MakeFrame(
            x, y, x, y, width, height, channels, true, current));
        previous = std::move(current);
    }
    codecs.Finish();
    return result;
}

template<class Codecs>
[[nodiscard]] ParsedPictureLayerResource ParsePl20(ByteCursor& cursor, Codecs& codecs)
{
    ParsedPictureLayerResource result;
    result.version = PictureLayerVersion::Pl20;
    const std::uint16_t count = cursor.ReadU16();
    if (count == 0u)
        ThrowPictureLayerError(PictureLayerResourceError::EmptyFrameSet,
            cursor.Offset() - 2u, "PL20 has no frames");
    result.globalRect = ReadGlobalRect(cursor);
    // 447594..44759E：最后一个rect DWORD后还跳过一个未解释DWORD，然后才读mode。
    static_cast<void>(cursor.ReadBytes(4));
    result.pl20CompressionMode = cursor.ReadU16();
    if (result.pl20CompressionMode != 3u && result.pl20CompressionMode != 4u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::UnsupportedCompressionMode,
            cursor.Offset() - 2u,
            "PL20 compression mode is neither 3 nor 4");
    }

    result.frames.reserve(count);
    for (std::uint16_t index = 0u; index < count; ++index)
    {
        const std::int32_t x = cursor.ReadI32();
        const std::int32_t y = cursor.ReadI32();
        const std::int32_t width = cursor.ReadI32();
        const std::int32_t height = cursor.ReadI32();
        const std::int32_t channels = cursor.ReadI32();
        const std::size_t expected = CheckedPixelCount(
            width, height, channels, cursor.Offset());
        const std::uint32_t packedSize = cursor.ReadU32();
        std::vector<std::byte> pixels = codecs.Frame(cursor, result.pl20CompressionMode,
            packedSize, expected);
        result.frames.push_back(MakeFrame(
            x, y,
            AddI32Wrapping(result.globalRect.x, x),
            AddI32Wrapping(result.globalRect.y, y),
            width, height, channels, channels == 4, std::move(pixels)));
    }
    codecs.Finish();
    return result;
}

[[nodiscard]] ParsedPictureLayerResource ParsePl30(ByteCursor& cursor)
{
    ParsedPictureLayerResource result;
    result.version = PictureLayerVersion::Pl30;
    const std::uint16_t count = cursor.ReadU16();
    if (count == 0u)
        ThrowPictureLayerError(PictureLayerResourceError::EmptyFrameSet,
            cursor.Offset() - 2u, "PL30 has no frames");
    result.globalRect = ReadGlobalRect(cursor);
    const std::int32_t channels = cursor.ReadI32();
    result.pl30BlockSize = cursor.ReadU8();
    if (result.pl30BlockSize == 0u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::InvalidBlockSize,
            cursor.Offset() - 1u,
            "PL30 block size is zero");
    }
    const std::size_t size = CheckedPixelCount(
        result.globalRect.width, result.globalRect.height,
        channels, cursor.Offset());
    result.frames.reserve(count);
    std::vector<std::byte> previous = CopyBytes(cursor.ReadBytes(size));
    result.frames.push_back(MakeFrame(
        0, 0, 0, 0, result.globalRect.width, result.globalRect.height,
        channels, channels == 4, previous));

    for (std::uint16_t index = 1u; index < count; ++index)
    {
        const std::uint8_t packedFlag = cursor.ReadU8();
        std::vector<std::byte> current;
        if (packedFlag == 0u)
        {
            current = CopyBytes(cursor.ReadBytes(size));
            AddDifference(current, previous);
        }
        else
        {
            const std::uint32_t packedSize = cursor.ReadU32();
            const std::size_t packedOffset = cursor.Offset();
            current = DecodeBlockConvert(
                cursor.ReadBytes(packedSize), previous,
                result.globalRect.width, result.globalRect.height,
                channels, result.pl30BlockSize, packedOffset);
        }
        result.frames.push_back(MakeFrame(
            0, 0, 0, 0, result.globalRect.width, result.globalRect.height,
            channels, channels == 4, current));
        previous = std::move(current);
    }
    return result;
}
}

PictureLayerResourceException::PictureLayerResourceException(
    const PictureLayerResourceError error,
    const std::size_t offset,
    const char* const message)
    : std::runtime_error(message), error_(error), offset_(offset)
{
}

PictureLayerResourceError PictureLayerResourceException::Error() const noexcept
{
    return error_;
}

std::size_t PictureLayerResourceException::Offset() const noexcept
{
    return offset_;
}

template<class Codecs>
static ParsedPictureLayerResource ParsePictureLayerWithCodecs(
    const std::span<const std::byte> bytes, Codecs& codecs)
{
    if (bytes.size() < 4u)
    {
        ThrowPictureLayerError(
            PictureLayerResourceError::HeaderTruncated,
            0u,
            "PL resource magic is truncated");
    }
    if (bytes[0] != static_cast<std::byte>('P') ||
        bytes[1] != static_cast<std::byte>('L'))
    {
        codecs.NotPicture();
    }

    ByteCursor cursor(bytes.subspan(4u), 4u);
    const std::array<std::byte, 2> version{bytes[2], bytes[3]};
    if (version == std::array{
            static_cast<std::byte>('0'), static_cast<std::byte>('0')})
        return ParsePl00(cursor);
    if (version == std::array{
            static_cast<std::byte>('0'), static_cast<std::byte>('1')})
        return ParsePl01(cursor);
    if (version == std::array{
            static_cast<std::byte>('1'), static_cast<std::byte>('0')})
        return ParsePl10Or11(cursor, false, codecs);
    if (version == std::array{
            static_cast<std::byte>('1'), static_cast<std::byte>('1')})
        return ParsePl10Or11(cursor, true, codecs);
    if (version == std::array{
            static_cast<std::byte>('2'), static_cast<std::byte>('0')})
        return ParsePl20(cursor, codecs);
    if (version == std::array{
            static_cast<std::byte>('3'), static_cast<std::byte>('0')})
        return ParsePl30(cursor);

    ThrowPictureLayerError(
        PictureLayerResourceError::UnsupportedVersion,
        2u,
        "Unsupported PL resource version");
}

ParsedPictureLayerResource ParsePictureLayerResource(std::span<const std::byte> bytes,
    const ArchiveCodecServices& services)
{
    NativePictureCodecs codecs(services);
    return ParsePictureLayerWithCodecs(bytes, codecs);
}

ParsedPictureLayerResource ParsePictureLayerResourceForTools(std::span<const std::byte> bytes)
{
    ToolPictureCodecs codecs;
    return ParsePictureLayerWithCodecs(bytes, codecs);
}
}
