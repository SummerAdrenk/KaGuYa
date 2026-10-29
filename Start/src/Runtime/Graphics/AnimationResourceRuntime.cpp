#include <Runtime/AnimationResourceRuntime.h>
#include <Runtime/AnimationResourceGroupRuntime.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/ArchiveHuffmanRuntime.h>
#include <Runtime/ArchiveLzssRuntime.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsRawBlobInstaller.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[noreturn]] void ThrowAnimationError(
    const AnimationResourceError error,
    const char* const message)
{
    throw AnimationResourceException(error, message);
}

class AnimationReader final
{
public:
    explicit AnimationReader(const std::span<const std::byte> bytes) noexcept
        : bytes_(bytes)
    {
    }

    template <typename Value>
    [[nodiscard]] Value Read()
    {
        const std::span<const std::byte> encoded = ReadBytes(
            sizeof(Value), AnimationResourceError::HeaderTruncated,
            "Animation resource structure is truncated");
        Value value{};
        std::memcpy(&value, encoded.data(), sizeof(value));
        return value;
    }

    [[nodiscard]] std::int32_t ReadSignedDword()
    {
        return std::bit_cast<std::int32_t>(Read<std::uint32_t>());
    }

    [[nodiscard]] std::span<const std::byte> ReadBytes(
        const std::size_t count,
        const AnimationResourceError error =
            AnimationResourceError::FramePayloadTruncated,
        const char* const message =
            "Animation frame pixel payload is truncated")
    {
        if (position_ > bytes_.size() || count > bytes_.size() - position_)
            ThrowAnimationError(error, message);
        const std::span<const std::byte> result = bytes_.subspan(position_, count);
        position_ += count;
        return result;
    }

    [[nodiscard]] std::size_t Position() const noexcept { return position_; }
    [[nodiscard]] std::span<const std::byte> Remaining() const noexcept
    { return bytes_.subspan(position_); }

private:
    std::span<const std::byte> bytes_;
    std::size_t position_ = 0;
};

[[nodiscard]] std::uint8_t ByteValue(const std::byte value) noexcept
{
    return std::to_integer<std::uint8_t>(value);
}

[[nodiscard]] std::uint16_t ReadWordAt(
    const std::span<const std::byte> bytes,
    const std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < 2u)
        ThrowAnimationError(
            AnimationResourceError::CompressedPayloadMalformed,
            "Animation compressed payload has a truncated word");
    return static_cast<std::uint16_t>(ByteValue(bytes[offset])) |
        static_cast<std::uint16_t>(ByteValue(bytes[offset + 1u]) << 8u);
}

[[nodiscard]] std::uint32_t ReadDwordAt(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const AnimationResourceError error =
        AnimationResourceError::CompressedPayloadMalformed,
    const char* const message =
        "Animation compressed payload has a truncated dword")
{
    if (offset > bytes.size() || bytes.size() - offset < 4u)
        ThrowAnimationError(error, message);
    return static_cast<std::uint32_t>(ByteValue(bytes[offset])) |
        (static_cast<std::uint32_t>(ByteValue(bytes[offset + 1u])) << 8u) |
        (static_cast<std::uint32_t>(ByteValue(bytes[offset + 2u])) << 16u) |
        (static_cast<std::uint32_t>(ByteValue(bytes[offset + 3u])) << 24u);
}

[[nodiscard]] std::size_t CheckedFramePayloadSize(
    const std::int32_t width,
    const std::int32_t height,
    const std::int32_t channels)
{
    if (width <= 0 || height <= 0 || channels <= 0)
    {
        ThrowAnimationError(
            AnimationResourceError::InvalidFramePayloadShape,
            "Animation frame dimensions or channel count are not positive");
    }

    const std::uint64_t size = static_cast<std::uint64_t>(width) *
        static_cast<std::uint64_t>(height) *
        static_cast<std::uint64_t>(channels);
    if (size > (std::numeric_limits<std::size_t>::max)())
    {
        ThrowAnimationError(
            AnimationResourceError::FramePayloadSizeOverflow,
            "Animation frame pixel payload size overflows size_t");
    }
    return static_cast<std::size_t>(size);
}

[[nodiscard]] std::vector<std::byte> CopyBytes(
    const std::span<const std::byte> bytes)
{
    return {bytes.begin(), bytes.end()};
}

[[nodiscard]] std::vector<std::byte> AddBytewiseModulo(
    const std::span<const std::byte> previous,
    const std::span<const std::byte> delta)
{
    if (previous.size() != delta.size())
    {
        ThrowAnimationError(
            AnimationResourceError::DecompressedSizeMismatch,
            "Animation differential frame size does not match its previous frame");
    }

    std::vector<std::byte> output(previous.size());
    for (std::size_t index = 0; index < output.size(); ++index)
    {
        output[index] = static_cast<std::byte>(
            static_cast<std::uint8_t>(ByteValue(previous[index]) +
                ByteValue(delta[index])));
    }
    return output;
}

void SetSourcePixels(
    AnimationReader& reader,
    AnimationFrameDescriptor& frame,
    const std::size_t absoluteBase)
{
    frame.pixelLength = CheckedFramePayloadSize(
        frame.width, frame.height, frame.channels);
    frame.pixelStorage = AnimationFramePixelStorage::SourceResource;
    frame.pixelOffset = absoluteBase + reader.Position();
    static_cast<void>(reader.ReadBytes(frame.pixelLength));
}

void SetDecodedPixels(
    AnimationFrameDescriptor& frame,
    std::vector<std::byte> pixels)
{
    const std::size_t expected = CheckedFramePayloadSize(
        frame.width, frame.height, frame.channels);
    if (pixels.size() != expected)
    {
        ThrowAnimationError(
            AnimationResourceError::DecompressedSizeMismatch,
            "Animation decoder did not produce the declared frame size");
    }
    frame.pixelStorage = AnimationFramePixelStorage::DecodedBytes;
    frame.pixelOffset = 0;
    frame.pixelLength = expected;
    frame.decodedPixels = std::move(pixels);
}

class MsbBitReader final
{
public:
    explicit MsbBitReader(const std::span<const std::byte> bytes) noexcept
        : bytes_(bytes)
    {
    }

    [[nodiscard]] std::uint32_t ReadBit(
        const AnimationResourceError error,
        const char* const message)
    {
        if (mask_ == 0u)
        {
            if (byteOffset_ >= bytes_.size())
                ThrowAnimationError(error, message);
            current_ = ByteValue(bytes_[byteOffset_++]);
            mask_ = 0x80u;
        }
        const std::uint32_t bit = (current_ & mask_) != 0u ? 1u : 0u;
        mask_ >>= 1u;
        return bit;
    }

    [[nodiscard]] std::uint32_t ReadBits(
        const unsigned count,
        const AnimationResourceError error,
        const char* const message)
    {
        std::uint32_t value = 0;
        for (unsigned index = 0; index < count; ++index)
            value = (value << 1u) | ReadBit(error, message);
        return value;
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t byteOffset_ = 0;
    std::uint8_t current_ = 0;
    std::uint8_t mask_ = 0;
};

constexpr std::uint16_t FirstHuffmanInternalNode = 0x100u;
constexpr std::uint16_t InvalidHuffmanNode = 0xFFFFu;
constexpr std::size_t MaximumHuffmanInternalNodes = 0xFFu;

struct HuffmanNode final
{
    std::uint16_t left = InvalidHuffmanNode;
    std::uint16_t right = InvalidHuffmanNode;
};

[[nodiscard]] std::uint16_t ReadHuffmanTree(
    MsbBitReader& bits,
    std::array<HuffmanNode, MaximumHuffmanInternalNodes>& nodes,
    std::size_t& nodeCount)
{
    constexpr auto error = AnimationResourceError::CompressedPayloadMalformed;
    if (bits.ReadBit(error, "Animation Huffman tree is truncated") == 0u)
    {
        return static_cast<std::uint16_t>(
            bits.ReadBits(8u, error, "Animation Huffman leaf is truncated"));
    }
    if (nodeCount >= nodes.size())
        ThrowAnimationError(error, "Animation Huffman tree has too many internal nodes");

    const std::uint16_t node = static_cast<std::uint16_t>(
        FirstHuffmanInternalNode + nodeCount++);
    HuffmanNode& entry = nodes[node - FirstHuffmanInternalNode];
    entry.left = ReadHuffmanTree(bits, nodes, nodeCount);
    entry.right = ReadHuffmanTree(bits, nodes, nodeCount);
    return node;
}

[[nodiscard]] std::vector<std::byte> DecodeHuffmanBytes(
    const std::span<const std::byte> encoded,
    const std::size_t outputSize)
{
    MsbBitReader bits(encoded);
    std::array<HuffmanNode, MaximumHuffmanInternalNodes> nodes{};
    std::size_t nodeCount = 0;
    const std::uint16_t root = ReadHuffmanTree(bits, nodes, nodeCount);
    std::vector<std::byte> output(outputSize);
    for (std::byte& value : output)
    {
        std::uint16_t symbol = root;
        while (symbol >= FirstHuffmanInternalNode)
        {
            const std::size_t index = symbol - FirstHuffmanInternalNode;
            if (index >= nodeCount)
            {
                ThrowAnimationError(
                    AnimationResourceError::CompressedPayloadMalformed,
                    "Animation Huffman stream references an invalid tree node");
            }
            const std::uint32_t branch = bits.ReadBit(
                AnimationResourceError::CompressedPayloadMalformed,
                "Animation Huffman payload is truncated");
            symbol = branch == 0u ? nodes[index].left : nodes[index].right;
        }
        value = static_cast<std::byte>(symbol);
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodeInterleavedRle(
    const std::span<const std::byte> encoded,
    const std::size_t outputSize,
    const std::uint8_t step)
{
    if (step == 0u || static_cast<std::size_t>(step) > outputSize)
    {
        ThrowAnimationError(
            AnimationResourceError::CompressedPayloadMalformed,
            "Animation RLE interleave step is outside the output frame");
    }

    std::vector<std::byte> output(outputSize);
    std::size_t source = 0;
    const auto readByte = [&]() -> std::uint8_t
    {
        if (source >= encoded.size())
        {
            ThrowAnimationError(
                AnimationResourceError::CompressedPayloadMalformed,
                "Animation RLE payload is truncated");
        }
        return ByteValue(encoded[source++]);
    };

    for (std::size_t lane = 0; lane < step; ++lane)
    {
        std::uint8_t previousValue = readByte();
        output[lane] = static_cast<std::byte>(previousValue);
        std::size_t destination = lane + step;
        while (destination < output.size())
        {
            std::uint8_t value = readByte();
            output[destination] = static_cast<std::byte>(value);
            destination += step;
            if (value == previousValue)
            {
                std::size_t repeatCount = readByte();
                if ((repeatCount & 0x80u) != 0u)
                {
                    repeatCount = readByte() +
                        ((repeatCount & 0x7Fu) << 8u) + 128u;
                }
                for (std::size_t repeat = 0; repeat < repeatCount; ++repeat)
                {
                    if (destination >= output.size())
                    {
                        ThrowAnimationError(
                            AnimationResourceError::CompressedPayloadMalformed,
                            "Animation RLE run exceeds the output frame");
                    }
                    output[destination] = static_cast<std::byte>(value);
                    destination += step;
                }
                if (destination < output.size())
                {
                    value = readByte();
                    output[destination] = static_cast<std::byte>(value);
                    destination += step;
                }
            }
            previousValue = value;
        }
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodePictureLzss(
    const std::span<const std::byte> payload,
    const std::size_t expectedSize)
{
    const std::uint32_t declaredSize = ReadDwordAt(
        payload, 0u, AnimationResourceError::CompressedPayloadMalformed,
        "PIC20 LZSS payload has no unpacked-size dword");
    if (declaredSize != expectedSize)
    {
        ThrowAnimationError(
            AnimationResourceError::DecompressedSizeMismatch,
            "PIC20 LZSS unpacked size differs from the frame size");
    }

    std::vector<std::byte> output(expectedSize);
    std::array<std::byte, 0x1000> window{};
    std::array<bool, 0x1000> initialized{};
    std::size_t outputOffset = 0;
    std::size_t writePosition = 1u;
    MsbBitReader bits(payload.subspan(4u));
    const auto writeByte = [&](const std::byte value)
    {
        if (outputOffset >= output.size())
        {
            ThrowAnimationError(
                AnimationResourceError::CompressedPayloadMalformed,
                "PIC20 LZSS stream writes past the frame size");
        }
        output[outputOffset++] = value;
        window[writePosition] = value;
        initialized[writePosition] = true;
        writePosition = (writePosition + 1u) & 0xFFFu;
    };

    for (;;)
    {
        if (bits.ReadBit(
                AnimationResourceError::CompressedPayloadMalformed,
                "PIC20 LZSS bit stream is truncated") != 0u)
        {
            writeByte(static_cast<std::byte>(bits.ReadBits(
                8u, AnimationResourceError::CompressedPayloadMalformed,
                "PIC20 LZSS literal is truncated")));
            continue;
        }

        const std::size_t offset = bits.ReadBits(
            12u, AnimationResourceError::CompressedPayloadMalformed,
            "PIC20 LZSS match offset is truncated");
        if (offset == 0u)
            break;
        const std::size_t length = bits.ReadBits(
            4u, AnimationResourceError::CompressedPayloadMalformed,
            "PIC20 LZSS match length is truncated") + 2u;
        for (std::size_t index = 0; index < length; ++index)
        {
            const std::size_t source = (offset + index) & 0xFFFu;
            if (!initialized[source])
            {
                ThrowAnimationError(
                    AnimationResourceError::CompressedPayloadMalformed,
                    "PIC20 LZSS references an unwritten dictionary byte");
            }
            writeByte(window[source]);
        }
    }
    if (outputOffset != output.size())
    {
        ThrowAnimationError(
            AnimationResourceError::DecompressedSizeMismatch,
            "PIC20 LZSS terminated before filling the frame");
    }
    return output;
}

[[nodiscard]] std::vector<std::byte> DecodePictureBmr(
    const std::span<const std::byte> payload,
    const std::size_t expectedSize)
{
    try
    {
        const auto source = std::span<const std::uint8_t>(
            reinterpret_cast<const std::uint8_t*>(payload.data()),
            payload.size());
        InstalledParamsRawBlob installed = ParamsRawBlobInstaller::Install(source);
        if (installed.storageKind != ParamsRawBlobStorageKind::Bmr)
        {
            ThrowAnimationError(
                AnimationResourceError::CompressedPayloadMalformed,
                "PIC20 mode 3 payload does not begin with BMR");
        }
        if (installed.bytes.size() != expectedSize)
        {
            ThrowAnimationError(
                AnimationResourceError::DecompressedSizeMismatch,
                "PIC20 BMR output size differs from the frame size");
        }
        std::vector<std::byte> output(installed.bytes.size());
        for (std::size_t index = 0; index < output.size(); ++index)
            output[index] = static_cast<std::byte>(installed.bytes[index]);
        return output;
    }
    catch (const ParamsRawBlobInstallException&)
    {
        ThrowAnimationError(
            AnimationResourceError::CompressedPayloadMalformed,
            "PIC20 BMR payload is malformed");
    }
}

void DecodePicture30Block(
    const std::span<const std::byte> payload,
    const std::span<const std::byte> previous,
    const std::span<std::byte> output,
    const std::int32_t widthValue,
    const std::int32_t heightValue,
    const std::int32_t channelsValue,
    const std::uint8_t blockSizeValue)
{
    if (blockSizeValue == 0u)
    {
        ThrowAnimationError(
            AnimationResourceError::BlockPayloadMalformed,
            "PIC30 block size is zero");
    }
    const std::size_t expected = CheckedFramePayloadSize(
        widthValue, heightValue, channelsValue);
    if (previous.size() != expected || output.size() != expected)
    {
        ThrowAnimationError(
            AnimationResourceError::DecompressedSizeMismatch,
            "PIC30 frame buffer size is invalid");
    }
    if (payload.size() < 10u)
    {
        ThrowAnimationError(
            AnimationResourceError::BlockPayloadMalformed,
            "PIC30 block payload header is truncated");
    }

    std::array<std::uint8_t, 4> operationMap{};
    for (std::size_t index = 0; index < operationMap.size(); ++index)
        operationMap[index] = ByteValue(payload[index]);

    const std::size_t commandByteCount = ReadWordAt(payload, 4u);
    if (commandByteCount > payload.size() - 6u ||
        payload.size() - 6u - commandByteCount < 4u)
    {
        ThrowAnimationError(
            AnimationResourceError::BlockPayloadMalformed,
            "PIC30 block command section is truncated");
    }
    const std::size_t auxiliarySizeOffset = 6u + commandByteCount;
    const std::size_t auxiliaryByteCount = ReadDwordAt(
        payload, auxiliarySizeOffset,
        AnimationResourceError::BlockPayloadMalformed,
        "PIC30 block auxiliary-size field is truncated");
    const std::size_t auxiliaryOffset = auxiliarySizeOffset + 4u;
    if (auxiliaryByteCount > payload.size() - auxiliaryOffset)
    {
        ThrowAnimationError(
            AnimationResourceError::BlockPayloadMalformed,
            "PIC30 block auxiliary section is truncated");
    }
    const std::span<const std::byte> auxiliary =
        payload.subspan(auxiliaryOffset, auxiliaryByteCount);
    const std::span<const std::byte> literals =
        payload.subspan(auxiliaryOffset + auxiliaryByteCount);
    MsbBitReader commands(payload.subspan(6u, commandByteCount));

    const std::size_t width = static_cast<std::size_t>(widthValue);
    const std::size_t height = static_cast<std::size_t>(heightValue);
    const std::size_t channels = static_cast<std::size_t>(channelsValue);
    const std::size_t blockSize = blockSizeValue;
    const std::size_t fixedMaskBytes = (blockSize * blockSize) >> 3u;
    // 445313..445324：>=8 的块只按列选 bit，字节位置仍按完整块行宽计算。
    const std::size_t maskBitRowStride = blockSize < 8u ? blockSize : 0u;
    std::size_t auxiliaryCursor = 0;
    std::size_t literalCursor = 0;

    const auto copyLiteralPixel = [&](const std::size_t destination)
    {
        if (literalCursor > literals.size() ||
            channels > literals.size() - literalCursor)
        {
            ThrowAnimationError(
                AnimationResourceError::BlockPayloadMalformed,
                "PIC30 literal section is truncated");
        }
        std::memcpy(
            output.data() + destination,
            literals.data() + literalCursor,
            channels);
        literalCursor += channels;
    };

    for (std::size_t blockY = 0; blockY < height; blockY += blockSize)
    {
        const std::size_t blockHeight = std::min(blockSize, height - blockY);
        for (std::size_t blockX = 0; blockX < width; blockX += blockSize)
        {
            const std::size_t blockWidth = std::min(blockSize, width - blockX);
            std::size_t unaryIndex = 0;
            while (unaryIndex < 3u && commands.ReadBit(
                       AnimationResourceError::BlockPayloadMalformed,
                       "PIC30 block command bit stream is truncated") != 0u)
            {
                ++unaryIndex;
            }
            const std::uint8_t operation = operationMap[unaryIndex];

            // 44545C -> 4456D7：未知映射只推进块，保留本次目标旧字节，不消费辅助/raw。
            if (operation > 3u)
                continue;
            if (operation == 0u)
            {
                for (std::size_t y = 0; y < blockHeight; ++y)
                {
                    const std::size_t destination =
                        ((blockY + y) * width + blockX) * channels;
                    std::memcpy(output.data() + destination,
                        previous.data() + destination, blockWidth * channels);
                }
                continue;
            }
            if (operation == 1u)
            {
                for (std::size_t y = 0; y < blockHeight; ++y)
                {
                    for (std::size_t x = 0; x < blockWidth; ++x)
                    {
                        const std::size_t destination =
                            ((blockY + y) * width + blockX + x) * channels;
                        copyLiteralPixel(destination);
                    }
                }
                continue;
            }
            if (operation == 2u)
            {
                for (std::size_t y = 0; y < blockHeight; ++y)
                {
                    for (std::size_t x = 0; x < blockWidth; ++x)
                    {
                        const std::size_t index = y * blockSize + x;
                        const std::size_t maskOffset =
                            auxiliaryCursor + index / 8u;
                        if (maskOffset >= auxiliary.size())
                        {
                            ThrowAnimationError(
                                AnimationResourceError::BlockPayloadMalformed,
                                "PIC30 block mask exceeds the auxiliary section");
                        }
                        const std::size_t destination =
                            ((blockY + y) * width + blockX + x) * channels;
                        const std::size_t maskBit = (y * maskBitRowStride + x) & 7u;
                        if ((ByteValue(auxiliary[maskOffset]) &
                                (0x80u >> maskBit)) != 0u)
                        {
                            copyLiteralPixel(destination);
                        }
                        else
                        {
                            std::memcpy(output.data() + destination,
                                previous.data() + destination, channels);
                        }
                    }
                }
                if (fixedMaskBytes > auxiliary.size() - auxiliaryCursor)
                {
                    ThrowAnimationError(
                        AnimationResourceError::BlockPayloadMalformed,
                        "PIC30 fixed block mask is truncated");
                }
                auxiliaryCursor += fixedMaskBytes;
                continue;
            }

            if (auxiliaryCursor >= auxiliary.size())
            {
                ThrowAnimationError(
                    AnimationResourceError::BlockPayloadMalformed,
                    "PIC30 sparse block count is truncated");
            }
            const std::size_t sparseCount = ByteValue(auxiliary[auxiliaryCursor]);
            if (sparseCount > auxiliary.size() - auxiliaryCursor - 1u)
            {
                ThrowAnimationError(
                    AnimationResourceError::BlockPayloadMalformed,
                    "PIC30 sparse block index list is truncated");
            }
            const std::span<const std::byte> indices =
                auxiliary.subspan(auxiliaryCursor + 1u, sparseCount);
            for (const std::byte index : indices)
            {
                if (ByteValue(index) >= blockSize * blockSize)
                {
                    ThrowAnimationError(
                        AnimationResourceError::BlockPayloadMalformed,
                        "PIC30 sparse block index is outside the fixed block");
                }
            }
            std::size_t sparseCursor = 0;
            for (std::size_t y = 0; y < blockHeight; ++y)
            {
                for (std::size_t x = 0; x < blockWidth; ++x)
                {
                    const std::size_t fixedIndex = y * blockSize + x;
                    const std::size_t destination =
                        ((blockY + y) * width + blockX + x) * channels;
                    if (sparseCursor < indices.size() &&
                        fixedIndex == ByteValue(indices[sparseCursor]))
                    {
                        copyLiteralPixel(destination);
                        ++sparseCursor;
                    }
                    else
                    {
                        std::memcpy(output.data() + destination,
                            previous.data() + destination, channels);
                    }
                }
            }
            auxiliaryCursor += 1u + sparseCount;
        }
    }
}

void ReadCanvas(AnimationReader& reader, ParsedAnimationResource& output)
{
    output.canvasX = reader.ReadSignedDword();
    output.canvasY = reader.ReadSignedDword();
    output.canvasWidth = reader.ReadSignedDword();
    output.canvasHeight = reader.ReadSignedDword();
}

struct ToolAnimationCodecs final
{
    bool NeedsPreviousSnapshot(bool) const noexcept { return true; }
    void ApplyPicture20Origin(AnimationFrameDescriptor&, const ParsedAnimationResource&) const noexcept {}
    // 历史工具PIC20夹具未含447596越过的DWORD，不混入生产wire格式。
    void ReadPicture20Reserved(AnimationReader&) const noexcept {}
    void ValidateMode(std::uint16_t mode) const
    {
        if (mode != 3u && mode != 4u)
            ThrowAnimationError(AnimationResourceError::UnsupportedCompressionMode,
                "PIC20 compression mode is not 3 or 4");
    }
    void BeginDifference(std::span<const std::byte>) const noexcept {}
    void Finish() const noexcept {}
    std::vector<std::byte> Difference(AnimationReader& reader,
        std::span<const std::byte> previous) const
    {
        const auto size = reader.Read<std::uint32_t>();
        return AddBytewiseModulo(previous, DecodeHuffmanBytes(reader.ReadBytes(size), previous.size()));
    }
    std::vector<std::byte> Frame(AnimationReader& reader, std::uint16_t mode,
        std::uint32_t packedSize, std::size_t expected) const
    {
        const auto payload = reader.ReadBytes(packedSize);
        return mode == 3 ? DecodePictureBmr(payload, expected) : DecodePictureLzss(payload, expected);
    }
};

class NativeAnimationCodecs final
{
public:
    explicit NativeAnimationCodecs(const ArchiveCodecServices& services) noexcept : services_(services)
    { std::construct_at(&scratch_); }
    ~NativeAnimationCodecs() noexcept { if (active_) std::destroy_at(&scratch_); }
    bool NeedsPreviousSnapshot(bool huffman) const noexcept { return !huffman; }
    void ApplyPicture20Origin(AnimationFrameDescriptor& frame, const ParsedAnimationResource& canvas) const noexcept
    {
        // 4476F7..447732是DWORD相加，不能触发C++有符号溢出或饱和裁剪。
        frame.originX = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(frame.originX) +
            static_cast<std::uint32_t>(canvas.canvasX));
        frame.originY = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(frame.originY) +
            static_cast<std::uint32_t>(canvas.canvasY));
    }

    void BeginDifference(std::span<const std::byte> first)
    {
        if (first.size() > (std::numeric_limits<std::uint32_t>::max)() / 2u)
            ThrowAnimationError(AnimationResourceError::FramePayloadSizeOverflow,
                "PIC11 double buffer exceeds the byte Ref count (safe boundary)");
        // 447117保留首帧原输入；44714E只申请2*N，不将首帧复制到临时Ref。
        previous_ = first;
        scratch_.AllocateAt412700(static_cast<std::uint32_t>(first.size() * 2u));
        const auto available = ScratchAt(scratch_.RawSlot().baseAt00);
        if (first.size() > available.size())
            ThrowAnimationError(AnimationResourceError::DecompressedSizeMismatch,
                "PIC11 output exceeds the byte Ref extent (safe boundary)");
        next_ = available.first(first.size());
    }

    void ReadPicture20Reserved(AnimationReader& reader) const
    {
        // 447594..59C越过canvas.height和额外DWORD；ReadCanvas已消费前者。
        static_cast<void>(reader.Read<std::uint32_t>());
    }
    void ValidateMode(std::uint16_t) const noexcept {}

    std::vector<std::byte> Difference(AnimationReader& reader, std::span<const std::byte>)
    {
        // 4473E8..F8缓存目标、前帧、输入指针；448437取manager后才读长度DWORD。
        const auto payload = AsInput(reader.Remaining());
        auto* const decoder = GetHuffmanAt43DE24(Capture());
        if (!decoder) throw std::logic_error("AN PIC11 Huffman body is null (safe boundary)");
        decoder->DecodeAt43DE24(payload, next_, nullptr, nullptr, services_.errors);
        for (std::size_t index = 0; index < previous_.size(); ++index)
            next_[index] = static_cast<std::uint8_t>(next_[index] + ByteValue(previous_[index]));
        // 4473FD在转换成功后重读同一前缀；失败不提交这段压缩输入。
        const auto size = reader.Read<std::uint32_t>();
        static_cast<void>(reader.ReadBytes(size));
        auto result = CopyBytes(std::as_bytes(next_));
        previous_ = std::as_bytes(next_);
        differenceOffset_ ^= previous_.size();
        const auto available = ScratchAt(scratch_.RawSlot().baseAt00);
        if (differenceOffset_ > available.size() || previous_.size() > available.size() - differenceOffset_)
            ThrowAnimationError(AnimationResourceError::DecompressedSizeMismatch,
                "PIC11 next frame exceeds the byte Ref extent (safe boundary)");
        next_ = available.subspan(differenceOffset_, previous_.size());
        return result;
    }

    std::vector<std::byte> Frame(AnimationReader& reader, std::uint16_t mode,
        std::uint32_t packedSize, std::size_t expected)
    {
        std::uintptr_t cachedBase;
        if (mode == 3)
        {
            const auto payload = AsInput(reader.Remaining());
            auto* const identifying = GetBmrAt445110(Capture());
            if (!identifying) throw std::logic_error("AN PIC20 BMR body is null (safe boundary)");
            std::uint32_t count;
            const auto* const input = identifying->IdentifyAt445A00(payload, count);
            if (!input)
                ThrowAnimationError(AnimationResourceError::CompressedPayloadMalformed,
                    "PIC20 BMR Identify did not produce an input (safe boundary)");
            ResizeScratch(count);
            cachedBase = scratch_.RawSlot().baseAt00;
            const auto output = ScratchAt(cachedBase);
            // 4477A5先缓存目标，4477A7独立取源；第三次取Huffman仍用同一services。
            auto* const decoding = GetBmrAt445110(Capture());
            if (!decoding) throw std::logic_error("AN PIC20 decoding BMR body is null (safe boundary)");
            decoding->DecodeAt43DCF0({input, payload.size() - 16}, output,
                services_.managers, services_.errors);
        }
        else if (mode == 4)
        {
            const auto count = ReadDwordAt(reader.Remaining(), 0);
            ResizeScratch(count);
            cachedBase = scratch_.RawSlot().baseAt00;
            const auto output = ScratchAt(cachedBase);
            // 447809先获取共享CLzss，4478F0再读当前输入；不重置窗口或要求写满。
            auto& manager = Capture();
            const bool cold = manager.lzssAt18.baseAt00 == 0;
            auto* const decoder = GetLzssAt4840B0(manager);
            // 447810选择冷路径；4478A3在control分配之后重读EDI，仅影响后面的上传。
            // 解码目标EBX已在447807缓存，不能随这个base一起改绑，也不能扩大到热路径。
            if (cold) cachedBase = scratch_.RawSlot().baseAt00;
            if (!decoder) throw std::logic_error("AN PIC20 LZSS body is null (safe boundary)");
            decoder->DecodeAt445750(AsInput(reader.Remaining()).subspan(4), output);
        }
        else throw 4; // 4477BF/4479AC：帧字段及packedSize已消费，不提前报错。
        // 4478FB重读control/bias，正文base仍来自分配后的EDI；正常尾声缓存此control。
        normalControl_ = scratch_.RawSlot().controlAt08;
        const auto output = ScratchAt(cachedBase);
        if (expected > output.size())
            ThrowAnimationError(AnimationResourceError::DecompressedSizeMismatch,
                "PIC20 pixel upload exceeds decoded allocation (safe boundary)");
        auto result = CopyBytes(std::as_bytes(output.first(expected)));
        static_cast<void>(reader.ReadBytes(packedSize));
        return result;
    }

    void Finish() noexcept
    {
        // 447487/447970进入terminate态。先撤资格，正常用cached control，展开用当前槽。
        auto* const control = previous_.empty() ? normalControl_ : scratch_.RawSlot().controlAt08;
        active_ = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }

private:
    static std::span<const std::uint8_t> AsInput(std::span<const std::byte> bytes) noexcept
    { return {reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size()}; }
    ArchiveCodecManagerRuntime& Capture()
    {
        auto* const manager = services_.managers.CaptureAt445910();
        if (!manager) throw std::logic_error("AN codec manager is null (safe boundary)");
        return *manager;
    }
    std::span<std::uint8_t> ScratchAt(std::uintptr_t base) const
    {
        const auto& slot = scratch_.RawSlot();
        const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
        if (!base) return {};
        if (!control || control->strideAt0C != 1 || slot.biasAt04 > control->countAt10)
            throw std::out_of_range("AN scratch requires a bounded byte Ref (safe boundary)");
        return {reinterpret_cast<std::uint8_t*>(base) + slot.biasAt04, control->countAt10 - slot.biasAt04};
    }
    void ResizeScratch(std::uint32_t count)
    {
        if (!scratch_.RawSlot().baseAt00 || scratch_.Size() != count)
            scratch_.AllocateAt412700(count);
    }
    const ArchiveCodecServices& services_;
    union { ByteArrayReferenceRuntime scratch_; };
    bool active_ = true;
    YtlReferenceControlRuntime* normalControl_ = nullptr;
    std::span<const std::byte> previous_;
    std::span<std::uint8_t> next_;
    std::size_t differenceOffset_ = 0;
};

void ParsePicture00(
    AnimationReader& reader,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase)
{
    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
        return;
    ReadCanvas(reader, output);
    output.frames.reserve(frameCount);
    for (std::uint16_t index = 0; index < frameCount; ++index)
    {
        AnimationFrameDescriptor frame;
        frame.originX = reader.ReadSignedDword();
        frame.originY = reader.ReadSignedDword();
        frame.width = reader.ReadSignedDword();
        frame.height = reader.ReadSignedDword();
        frame.channels = reader.ReadSignedDword();
        SetSourcePixels(reader, frame, absoluteBase);
        output.frames.push_back(std::move(frame));
    }
}

void ParsePicture01(
    AnimationReader& reader,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase)
{
    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
        return;
    ReadCanvas(reader, output);
    output.pictureChannels = reader.ReadSignedDword();
    const std::size_t frameSize = CheckedFramePayloadSize(
        output.canvasWidth, output.canvasHeight, output.pictureChannels);
    output.frames.reserve(frameCount);

    AnimationFrameDescriptor first;
    first.width = output.canvasWidth;
    first.height = output.canvasHeight;
    first.channels = output.pictureChannels;
    first.pixelLength = frameSize;
    first.pixelStorage = AnimationFramePixelStorage::SourceResource;
    first.pixelOffset = absoluteBase + reader.Position();
    const std::span<const std::byte> firstPixels = reader.ReadBytes(frameSize);
    std::vector<std::byte> previous = CopyBytes(firstPixels);
    output.frames.push_back(std::move(first));

    for (std::uint16_t index = 1; index < frameCount; ++index)
    {
        const std::span<const std::byte> delta = reader.ReadBytes(frameSize);
        std::vector<std::byte> current = AddBytewiseModulo(previous, delta);
        AnimationFrameDescriptor frame;
        frame.width = output.canvasWidth;
        frame.height = output.canvasHeight;
        frame.channels = output.pictureChannels;
        SetDecodedPixels(frame, std::move(current));
        previous = frame.decodedPixels;
        output.frames.push_back(std::move(frame));
    }
}

template<class Codecs>
void ParsePicture10Or11(
    AnimationReader& reader,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase,
    const bool huffman,
    Codecs& codecs)
{
    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
        return;
    ReadCanvas(reader, output);
    const std::int32_t frameX = reader.ReadSignedDword();
    const std::int32_t frameY = reader.ReadSignedDword();
    const std::int32_t frameWidth = reader.ReadSignedDword();
    const std::int32_t frameHeight = reader.ReadSignedDword();
    const std::int32_t channels = reader.ReadSignedDword();
    const std::size_t frameSize = CheckedFramePayloadSize(
        frameWidth, frameHeight, channels);
    output.pictureChannels = channels;
    output.frames.reserve(frameCount);

    AnimationFrameDescriptor first;
    first.originX = frameX;
    first.originY = frameY;
    first.width = frameWidth;
    first.height = frameHeight;
    first.channels = channels;
    first.pixelLength = frameSize;
    first.pixelStorage = AnimationFramePixelStorage::SourceResource;
    first.pixelOffset = absoluteBase + reader.Position();
    const std::span<const std::byte> firstPixels = reader.ReadBytes(frameSize);
    const bool rememberPrevious = codecs.NeedsPreviousSnapshot(huffman);
    std::vector<std::byte> previous;
    if (rememberPrevious) previous = CopyBytes(firstPixels);
    output.frames.push_back(std::move(first));
    if (huffman)
    {
        codecs.BeginDifference(firstPixels);
        output.pictureReservedWord = reader.Read<std::uint16_t>();
    }

    for (std::uint16_t index = 1; index < frameCount; ++index)
    {
        std::vector<std::byte> current;
        if (huffman)
        {
            current = codecs.Difference(reader, previous);
        }
        else
        {
            const std::uint8_t step = reader.Read<std::uint8_t>();
            const std::uint32_t packedSize = reader.Read<std::uint32_t>();
            const auto delta = DecodeInterleavedRle(
                reader.ReadBytes(packedSize), frameSize, step);
            current = AddBytewiseModulo(previous, delta);
        }
        AnimationFrameDescriptor frame;
        frame.originX = frameX;
        frame.originY = frameY;
        frame.width = frameWidth;
        frame.height = frameHeight;
        frame.channels = channels;
        SetDecodedPixels(frame, std::move(current));
        if (rememberPrevious) previous = frame.decodedPixels;
        output.frames.push_back(std::move(frame));
    }
    if (huffman) codecs.Finish();
}

template<class Codecs>
void ParsePicture20(
    AnimationReader& reader,
    ParsedAnimationResource& output,
    Codecs& codecs)
{
    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
        return;
    ReadCanvas(reader, output);
    codecs.ReadPicture20Reserved(reader);
    output.pictureCompressionMode = reader.Read<std::uint16_t>();
    codecs.ValidateMode(output.pictureCompressionMode);
    output.frames.reserve(frameCount);
    for (std::uint16_t index = 0; index < frameCount; ++index)
    {
        AnimationFrameDescriptor frame;
        frame.originX = reader.ReadSignedDword();
        frame.originY = reader.ReadSignedDword();
        frame.width = reader.ReadSignedDword();
        frame.height = reader.ReadSignedDword();
        frame.channels = reader.ReadSignedDword();
        codecs.ApplyPicture20Origin(frame, output);
        const std::size_t expected = CheckedFramePayloadSize(
            frame.width, frame.height, frame.channels);
        const std::uint32_t packedSize = reader.Read<std::uint32_t>();
        auto pixels = codecs.Frame(reader, output.pictureCompressionMode, packedSize, expected);
        SetDecodedPixels(frame, std::move(pixels));
        output.frames.push_back(std::move(frame));
    }
    codecs.Finish();
}

void ParsePicture30(
    AnimationReader& reader,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase)
{
    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
        return;
    ReadCanvas(reader, output);
    // 447A54..447A69：height 后直接 channels DWORD、block BYTE，没有 reserved BYTE。
    output.pictureChannels = reader.ReadSignedDword();
    output.pictureBlockSize = reader.Read<std::uint8_t>();
    const std::size_t frameSize = CheckedFramePayloadSize(
        output.canvasWidth, output.canvasHeight, output.pictureChannels);
    if (output.pictureBlockSize == 0u)
    {
        ThrowAnimationError(
            AnimationResourceError::BlockPayloadMalformed,
            "PIC30 block size is zero");
    }
    output.frames.reserve(frameCount);

    AnimationFrameDescriptor first;
    first.width = output.canvasWidth;
    first.height = output.canvasHeight;
    first.channels = output.pictureChannels;
    first.pixelLength = frameSize;
    first.pixelStorage = AnimationFramePixelStorage::SourceResource;
    first.pixelOffset = absoluteBase + reader.Position();
    const std::span<const std::byte> firstPixels = reader.ReadBytes(frameSize);
    std::span<const std::byte> previous = firstPixels;
    output.frames.push_back(std::move(first));

    ByteArrayReferenceRuntime scratch;
    std::size_t targetOffset = 0;
    for (std::uint16_t index = 1; index < frameCount; ++index)
    {
        // 447BF4..447C2B：第二帧才申请未初始化的 2*N；两半轮换，不预填 previous。
        if (!scratch)
        {
            // 有界 parser 的安全拒绝，不冒充原 DWORD 溢出/非法目标访问的行为。
            if (frameSize > static_cast<std::size_t>(
                    (std::numeric_limits<std::int32_t>::max)()) / 2u)
            {
                ThrowAnimationError(AnimationResourceError::FramePayloadSizeOverflow,
                    "PIC30 double buffer exceeds signed byte-array capacity");
            }
            scratch.AllocateAt412700(static_cast<std::uint32_t>(frameSize * 2u));
        }
        const std::span<std::byte> current =
            std::as_writable_bytes(scratch.MutableBytes()).subspan(targetOffset, frameSize);
        const std::uint8_t encoded = reader.Read<std::uint8_t>();
        if (encoded == 0u)
        {
            const std::span<const std::byte> delta = reader.ReadBytes(frameSize);
            for (std::size_t byte = 0; byte < frameSize; ++byte)
                current[byte] = static_cast<std::byte>(ByteValue(previous[byte]) + ByteValue(delta[byte]));
        }
        else
        {
            const std::uint32_t packedSize = reader.Read<std::uint32_t>();
            DecodePicture30Block(
                reader.ReadBytes(packedSize), previous, current,
                output.canvasWidth, output.canvasHeight,
                output.pictureChannels, output.pictureBlockSize);
        }
        AnimationFrameDescriptor frame;
        frame.width = output.canvasWidth;
        frame.height = output.canvasHeight;
        frame.channels = output.pictureChannels;
        // 此接口仍交付现代解析结果；逐帧快照不作为原生 44h 的 Surface/Ref 装载证明。
        SetDecodedPixels(frame, CopyBytes(current));
        output.frames.push_back(std::move(frame));
        previous = current;
        targetOffset ^= frameSize; // 447EDB..447EE7。
    }
}

void ValidateNewControlReferences(const ParsedAnimationResource& output, bool embeddedFrameBounds)
{
    const std::int32_t commandCount =
        static_cast<std::int32_t>(output.commands.size());
    const std::int32_t branchCount =
        static_cast<std::int32_t>(output.branches.size());
    for (const AnimationBranch& branch : output.branches)
    {
        if (branch.jumpCommandIndex < 0 ||
            branch.jumpCommandIndex >= commandCount)
        {
            ThrowAnimationError(
                AnimationResourceError::InvalidCommandReference,
                "Animation branch jump target is outside the command table");
        }
    }
    for (const AnimationCommand& command : output.commands)
    {
        if (command.opcode == AnimationCommandOpcode::SetFrameMapping)
        {
            if (command.first < 0 ||
                command.first >= output.frameMappingSlotCount)
            {
                ThrowAnimationError(
                    AnimationResourceError::InvalidCommandReference,
                    "Animation frame-mapping slot is outside its table");
            }
            if (embeddedFrameBounds && command.second >= 0 &&
                command.second >= static_cast<std::int32_t>(output.frames.size()))
            {
                ThrowAnimationError(
                    AnimationResourceError::InvalidCommandReference,
                    "Animation frame-mapping index is outside the frame table");
            }
        }
        else if ((command.opcode ==
                      AnimationCommandOpcode::LoadBranchRepeatCounter ||
                     command.opcode == AnimationCommandOpcode::
                         DecrementBranchAndJumpIfPositive) &&
            (command.first < 0 || command.first >= branchCount))
        {
            ThrowAnimationError(
                AnimationResourceError::InvalidCommandReference,
                "Animation command branch index is outside the branch table");
        }
    }
}

template<class Codecs>
void ParseNewAnimation(
    AnimationReader& reader,
    const bool requiresPictureMarker,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase,
    Codecs& codecs,
    bool embeddedFrameBounds)
{
    const std::uint16_t commandCount = reader.Read<std::uint16_t>();
    if (commandCount == 0u)
    {
        ThrowAnimationError(
            AnimationResourceError::EmptyCommandTable,
            "New animation has no command");
    }
    const std::uint16_t controlWord = reader.Read<std::uint16_t>();
    output.controlFlag = (controlWord & 0x00FFu) != 0u;
    output.controlReservedByte = static_cast<std::uint8_t>(controlWord >> 8u);
    output.commands.reserve(commandCount);
    std::int32_t maximumFrameSlot = -1;
    for (std::uint16_t index = 0; index < commandCount; ++index)
    {
        AnimationCommand command;
        const std::uint8_t opcode = reader.Read<std::uint8_t>();
        if (opcode > static_cast<std::uint8_t>(
                AnimationCommandOpcode::SetAnimationResultCode))
        {
            ThrowAnimationError(
                AnimationResourceError::InvalidCommandOpcode,
                "New animation command opcode is outside 0..5");
        }
        command.opcode = static_cast<AnimationCommandOpcode>(opcode);
        switch (command.opcode)
        {
        case AnimationCommandOpcode::NoOperation:
            break;
        case AnimationCommandOpcode::SetFrameMapping:
            command.first = reader.ReadSignedDword();
            command.second = reader.ReadSignedDword();
            if (command.first < 0)
            {
                ThrowAnimationError(
                    AnimationResourceError::InvalidCommandReference,
                    "Animation frame-mapping slot is negative");
            }
            maximumFrameSlot = std::max(maximumFrameSlot, command.first);
            break;
        case AnimationCommandOpcode::WaitTicks:
        case AnimationCommandOpcode::LoadBranchRepeatCounter:
        case AnimationCommandOpcode::DecrementBranchAndJumpIfPositive:
        case AnimationCommandOpcode::SetAnimationResultCode:
            command.first = reader.ReadSignedDword();
            break;
        }
        output.commands.push_back(command);
    }
    if (maximumFrameSlot < 0)
    {
        ThrowAnimationError(
            AnimationResourceError::MissingFrameMappingCommand,
            "New animation has no SetFrameMapping command");
    }
    if (maximumFrameSlot == (std::numeric_limits<std::int32_t>::max)())
    {
        ThrowAnimationError(
            AnimationResourceError::InvalidCommandReference,
            "Animation frame-mapping slot count overflows int32");
    }
    output.frameMappingSlotCount = maximumFrameSlot + 1;

    const std::uint16_t branchCount = reader.Read<std::uint16_t>();
    output.branches.reserve(branchCount);
    for (std::uint16_t index = 0; index < branchCount; ++index)
    {
        output.branches.push_back({
            reader.ReadSignedDword(), reader.ReadSignedDword()});
    }

    if (!requiresPictureMarker)
    {
        output.pictureVersion = AnimationPictureVersion::Pic00;
    }
    else
    {
        const std::span<const std::byte> marker = reader.ReadBytes(
            5u, AnimationResourceError::MissingPictureMarker,
            "AN21/AN30 picture marker is truncated");
        constexpr std::array<std::byte, 5> expectedMarker{
            std::byte{'['}, std::byte{'P'}, std::byte{'I'},
            std::byte{'C'}, std::byte{']'}};
        if (!std::equal(marker.begin(), marker.end(), expectedMarker.begin()))
        {
            ThrowAnimationError(
                AnimationResourceError::MissingPictureMarker,
                "AN21/AN30 picture marker is not [PIC]");
        }
        switch (reader.Read<std::uint16_t>())
        {
        case 0x3030u: output.pictureVersion = AnimationPictureVersion::Pic00; break;
        case 0x3130u: output.pictureVersion = AnimationPictureVersion::Pic01; break;
        case 0x3031u: output.pictureVersion = AnimationPictureVersion::Pic10; break;
        case 0x3131u: output.pictureVersion = AnimationPictureVersion::Pic11; break;
        case 0x3032u: output.pictureVersion = AnimationPictureVersion::Pic20; break;
        case 0x3033u: output.pictureVersion = AnimationPictureVersion::Pic30; break;
        default:
            ThrowAnimationError(
                AnimationResourceError::UnsupportedPictureVersion,
                "AN21/AN30 [PIC] version is not accepted by Start.exe:4460D0");
        }
    }

    switch (output.pictureVersion)
    {
    case AnimationPictureVersion::Pic00:
        ParsePicture00(reader, output, absoluteBase);
        break;
    case AnimationPictureVersion::Pic01:
        ParsePicture01(reader, output, absoluteBase);
        break;
    case AnimationPictureVersion::Pic10:
        ParsePicture10Or11(reader, output, absoluteBase, false, codecs);
        break;
    case AnimationPictureVersion::Pic11:
        ParsePicture10Or11(reader, output, absoluteBase, true, codecs);
        break;
    case AnimationPictureVersion::Pic20:
        ParsePicture20(reader, output, codecs);
        break;
    case AnimationPictureVersion::Pic30:
        ParsePicture30(reader, output, absoluteBase);
        break;
    case AnimationPictureVersion::None:
        ThrowAnimationError(
            AnimationResourceError::UnsupportedPictureVersion,
            "New animation has no selected picture version");
    }
    // 02:441050/4415E0允许零内嵌图片，4FE7D0随后才装载各PLT；不能提前拒绝外部引用。
    ValidateNewControlReferences(output, embeddedFrameBounds);
    output.consumedBytes = absoluteBase + reader.Position();
}

void ParseLegacyAnimation(
    AnimationReader& reader,
    const bool readsFrameChannels,
    ParsedAnimationResource& output,
    const std::size_t absoluteBase)
{
    ReadCanvas(reader, output);

    const std::uint16_t pairCount = reader.Read<std::uint16_t>();
    if (pairCount == 0u)
    {
        ThrowAnimationError(
            AnimationResourceError::EmptyCommandTable,
            "Legacy animation has no control pair");
    }

    const std::uint16_t controlWord = reader.Read<std::uint16_t>();
    output.controlFlag = (controlWord & 0x00FFu) != 0u;
    output.controlReservedByte = static_cast<std::uint8_t>(controlWord >> 8u);
    output.commands.reserve(static_cast<std::size_t>(pairCount) * 2u);
    for (std::uint16_t index = 0; index < pairCount; ++index)
    {
        const std::uint16_t waitTicks = reader.Read<std::uint16_t>();
        const std::uint16_t rawFrameIndex = reader.Read<std::uint16_t>();
        output.commands.push_back({AnimationCommandOpcode::SetFrameMapping,
            0, rawFrameIndex == 0xFFFFu ? -1 : rawFrameIndex});
        output.commands.push_back({AnimationCommandOpcode::WaitTicks,
            static_cast<std::int32_t>(waitTicks), 0});
    }
    output.frameMappingSlotCount = 1;

    const std::uint16_t frameCount = reader.Read<std::uint16_t>();
    if (frameCount == 0u)
    {
        ThrowAnimationError(
            AnimationResourceError::EmptyFrameTable,
            "Legacy animation has no image frame");
    }

    output.frames.reserve(frameCount);
    for (std::uint16_t index = 0; index < frameCount; ++index)
    {
        AnimationFrameDescriptor frame;
        frame.originX = reader.ReadSignedDword();
        frame.originY = reader.ReadSignedDword();
        frame.width = reader.ReadSignedDword();
        frame.height = reader.ReadSignedDword();
        frame.channels = readsFrameChannels ? reader.ReadSignedDword() : 4;
        SetSourcePixels(reader, frame, absoluteBase);
        output.frames.push_back(std::move(frame));
    }
    output.consumedBytes = absoluteBase + reader.Position();
}
}

std::span<const std::byte> AnimationFrameDescriptor::Pixels(
    const std::span<const std::byte> sourceResource) const
{
    if (pixelStorage == AnimationFramePixelStorage::DecodedBytes)
    {
        if (decodedPixels.size() != pixelLength)
        {
            ThrowAnimationError(
                AnimationResourceError::DecompressedSizeMismatch,
                "Animation frame decoded storage has an invalid size");
        }
        return decodedPixels;
    }
    if (pixelOffset > sourceResource.size() ||
        pixelLength > sourceResource.size() - pixelOffset)
    {
        ThrowAnimationError(
            AnimationResourceError::FramePayloadTruncated,
            "Animation frame source storage is outside the resource");
    }
    return sourceResource.subspan(pixelOffset, pixelLength);
}

AnimationResourceException::AnimationResourceException(
    const AnimationResourceError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

AnimationResourceError AnimationResourceException::Error() const noexcept
{
    return error_;
}

template<class Codecs>
static AnimationParseState ParseAnimationResourceUsing(
    const std::span<const std::byte> bytes,
    ParsedAnimationResource& output,
    Codecs& codecs,
    bool embeddedFrameBounds = true)
{
    output = {};
    if (bytes.size() < 2u)
    {
        ThrowAnimationError(
            AnimationResourceError::HeaderTruncated,
            "Animation resource has no two-byte magic");
    }

    std::uint16_t magic = 0u;
    std::memcpy(&magic, bytes.data(), sizeof(magic));
    if (magic != 0x4E41u) // "AN"
        return AnimationParseState::NotAnimation;
    if (bytes.size() < 4u)
    {
        ThrowAnimationError(
            AnimationResourceError::HeaderTruncated,
            "Animation resource version is truncated");
    }

    std::uint16_t version = 0u;
    std::memcpy(&version, bytes.data() + 2u, sizeof(version));
    constexpr std::size_t bodyOffset = 4u;
    AnimationReader reader(bytes.subspan(bodyOffset));
    switch (version)
    {
    case 0x3030u: // bytes "00"
        output.version = AnimationResourceVersion::An00;
        ParseLegacyAnimation(reader, false, output, bodyOffset);
        break;
    case 0x3031u: // bytes "10"
        output.version = AnimationResourceVersion::An10;
        ParseLegacyAnimation(reader, true, output, bodyOffset);
        break;
    case 0x3032u: // bytes "20"
        output.version = AnimationResourceVersion::An20;
        ParseNewAnimation(reader, false, output, bodyOffset, codecs, embeddedFrameBounds);
        break;
    case 0x3033u: // bytes "30"
        output.version = AnimationResourceVersion::An30;
        ParseNewAnimation(reader, true, output, bodyOffset, codecs, embeddedFrameBounds);
        break;
    case 0x3132u: // bytes "21"
        output.version = AnimationResourceVersion::An21;
        ParseNewAnimation(reader, true, output, bodyOffset, codecs, embeddedFrameBounds);
        break;
    default:
        ThrowAnimationError(
            AnimationResourceError::UnsupportedVersion,
            "Animation resource version is not accepted by Start.exe:448010");
    }
    return AnimationParseState::Success;
}

AnimationParseState ParseAnimationResource(std::span<const std::byte> bytes,
    ParsedAnimationResource& output, const ArchiveCodecServices& services)
{
    NativeAnimationCodecs codecs(services);
    return ParseAnimationResourceUsing(bytes, output, codecs);
}

AnimationParseState ParseAnimationResourceForTools(std::span<const std::byte> bytes,
    ParsedAnimationResource& output)
{
    ToolAnimationCodecs codecs;
    return ParseAnimationResourceUsing(bytes, output, codecs);
}

AnimationParseState ParseFileGroupAnimationResource(std::span<const std::byte> bytes,
    ParsedAnimationResource& output, const ArchiveCodecServices& services)
{
    NativeAnimationCodecs codecs(services);
    return ParseAnimationResourceUsing(bytes, output, codecs, false);
}

AnimationParseState ParseFileGroupAnimationResourceForTools(std::span<const std::byte> bytes,
    ParsedAnimationResource& output)
{
    ToolAnimationCodecs codecs;
    return ParseAnimationResourceUsing(bytes, output, codecs, false);
}
}
