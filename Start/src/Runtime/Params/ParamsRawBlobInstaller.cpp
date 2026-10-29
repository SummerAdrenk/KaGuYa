#include <Runtime/ParamsRawBlobInstaller.h>
#include <Runtime/BmrDecoderRuntime.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::size_t BmrHeaderSize = 0x14;
constexpr std::uint16_t FirstInternalNode = 0x100;
constexpr std::uint16_t InvalidNode = std::numeric_limits<std::uint16_t>::max();
constexpr std::size_t MaximumInternalNodeCount = 0xFF;

[[nodiscard]] std::uint32_t ReadDword(const std::span<const std::uint8_t> source,
                                     const std::size_t offset) noexcept
{
    return static_cast<std::uint32_t>(source[offset]) |
           (static_cast<std::uint32_t>(source[offset + 1]) << 8u) |
           (static_cast<std::uint32_t>(source[offset + 2]) << 16u) |
           (static_cast<std::uint32_t>(source[offset + 3]) << 24u);
}

[[nodiscard]] bool HasBmrMagic(const std::span<const std::uint8_t> source) noexcept
{
    return source.size() >= 3 && source[0] == 'B' && source[1] == 'M' && source[2] == 'R';
}

[[noreturn]] void ThrowInstallError(const ParamsRawBlobInstallError error, const char* message)
{
    throw ParamsRawBlobInstallException(error, message);
}

class MsbBitReader final
{
public:
    explicit MsbBitReader(const std::span<const std::uint8_t> bytes) noexcept : bytes_(bytes) {}

    [[nodiscard]] std::uint32_t ReadBit(const ParamsRawBlobInstallError exhaustedError)
    {
        if (bitsRemaining_ == 0)
        {
            if (byteIndex_ >= bytes_.size())
                ThrowInstallError(exhaustedError, "BMR Huffman bit stream ended unexpectedly");
            currentByte_ = bytes_[byteIndex_++];
            bitsRemaining_ = 8;
        }

        --bitsRemaining_;
        return (currentByte_ >> bitsRemaining_) & 1u;
    }

    [[nodiscard]] std::uint32_t ReadByte()
    {
        std::uint32_t value = 0;
        for (int index = 0; index < 8; ++index)
            value = (value << 1u) | ReadBit(ParamsRawBlobInstallError::HuffmanLeafExhausted);
        return value;
    }

private:
    std::span<const std::uint8_t> bytes_;
    std::size_t byteIndex_ = 0;
    int bitsRemaining_ = 0;
    std::uint32_t currentByte_ = 0;
};

struct HuffmanNode
{
    std::uint16_t left = InvalidNode;
    std::uint16_t right = InvalidNode;
};

[[nodiscard]] std::uint16_t BuildHuffmanTree(
    MsbBitReader& reader,
    std::array<HuffmanNode, MaximumInternalNodeCount>& nodes,
    std::size_t& nodeCount)
{
    if (reader.ReadBit(ParamsRawBlobInstallError::HuffmanTreeExhausted) == 0)
        return static_cast<std::uint16_t>(reader.ReadByte());

    // 43E760 在分配内部节点 0x1FF 前抛出；合法内部节点编号严格为 0x100..0x1FE。
    if (nodeCount >= nodes.size())
        ThrowInstallError(
            ParamsRawBlobInstallError::HuffmanNodeLimit,
            "BMR Huffman tree contains more than 255 internal nodes");

    const auto node = static_cast<std::uint16_t>(FirstInternalNode + nodeCount++);
    HuffmanNode& entry = nodes[node - FirstInternalNode];
    entry.left = BuildHuffmanTree(reader, nodes, nodeCount);
    entry.right = BuildHuffmanTree(reader, nodes, nodeCount);
    return node;
}

void DecodeHuffmanInto(
    const std::span<const std::uint8_t> encoded,
    const std::span<std::uint8_t> output,
    IBmrDestinationAccessGuard* destinationGuard = nullptr)
{
    MsbBitReader reader(encoded);
    std::array<HuffmanNode, MaximumInternalNodeCount> nodes{};
    std::size_t nodeCount = 0;
    const std::uint16_t root = BuildHuffmanTree(reader, nodes, nodeCount);

    if (destinationGuard) destinationGuard->ValidateAccess(output);
    for (std::uint8_t& value : output)
    {
        std::uint16_t symbol = root;
        while (symbol >= FirstInternalNode)
        {
            const std::size_t nodeIndex = symbol - FirstInternalNode;
            if (nodeIndex >= nodeCount)
                ThrowInstallError(
                    ParamsRawBlobInstallError::HuffmanNodeLimit,
                    "BMR Huffman tree references an invalid internal node");
            const std::uint32_t branch =
                reader.ReadBit(ParamsRawBlobInstallError::HuffmanPayloadExhausted);
            symbol = branch == 0 ? nodes[nodeIndex].left : nodes[nodeIndex].right;
        }
        value = static_cast<std::uint8_t>(symbol);
    }
}

void UndoMoveToFront(const std::span<std::uint8_t> bytes) noexcept
{
    std::array<std::uint8_t, 256> dictionary{};
    for (std::size_t index = 0; index < dictionary.size(); ++index)
        dictionary[index] = static_cast<std::uint8_t>(index);

    for (std::uint8_t& indexByte : bytes)
    {
        const std::size_t index = indexByte;
        const std::uint8_t value = dictionary[index];
        for (std::size_t position = index; position > 0; --position)
            dictionary[position] = dictionary[position - 1];
        dictionary[0] = value;
        indexByte = value;
    }
}

[[nodiscard]] std::vector<std::uint8_t> UndoBurrowsWheeler(
    const std::span<const std::uint8_t> bytes,
    const std::uint32_t key,
    IBmrDestinationAccessGuard* destinationGuard = nullptr)
{
    if (bytes.empty() || key >= bytes.size())
        ThrowInstallError(ParamsRawBlobInstallError::InvalidBwtKey, "BMR BWT key is out of range");

    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    std::array<std::size_t, 256> frequencies{};
    for (const std::uint8_t value : bytes)
        ++frequencies[value];
    for (std::size_t index = 1; index < frequencies.size(); ++index)
        frequencies[index] += frequencies[index - 1];

    std::vector<std::size_t> distribution(bytes.size());
    // distribution分配会回调；其后读的仍是入参旧target，不能改绑为当前key。
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    for (std::size_t index = bytes.size(); index > 0; --index)
    {
        const std::uint8_t value = bytes[index - 1];
        distribution[--frequencies[value]] = index - 1;
    }

    std::vector<std::uint8_t> output(bytes.size());
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    std::size_t position = distribution[key];
    for (std::uint8_t& value : output)
    {
        value = bytes[position];
        position = distribution[position];
    }
    return output;
}

void DecodeRleInto(
    const std::span<const std::uint8_t> input,
    const std::span<std::uint8_t> output,
    const std::uint8_t step,
    const bool rejectOutOfRange)
{
    if (output.size() < step)
        ThrowInstallError(
            ParamsRawBlobInstallError::InvalidSizeRelationship,
            "BMR RLE step exceeds the final output size");

    std::size_t source = 0;
    const auto readByte = [&input, &source]() -> std::uint8_t
    {
        if (source >= input.size())
            ThrowInstallError(
                ParamsRawBlobInstallError::RleStreamExhausted,
                "BMR RLE stream ended unexpectedly");
        return input[source++];
    };

    for (std::size_t lane = 0; lane < step; ++lane)
    {
        std::uint8_t previous = readByte();
        output[lane] = previous;
        std::size_t destination = lane + step;
        while (destination < output.size())
        {
            std::uint8_t current = readByte();
            output[destination] = current;
            destination += step;
            if (current == previous)
            {
                std::size_t count = readByte();
                if ((count & 0x80u) != 0)
                    count = readByte() + ((count & 0x7Fu) << 8u) + 128u;

                while (count-- > 0)
                {
                    if (destination >= output.size())
                    {
                        // 原43E200不会截断游程；生产路径在第一次越界写之前明确失败。
                        if (rejectOutOfRange)
                            ThrowInstallError(ParamsRawBlobInstallError::RleDestinationOutOfRange,
                                "BMR RLE run writes past the destination buffer");
                        break;
                    }
                    output[destination] = current;
                    destination += step;
                }
                if (destination < output.size())
                {
                    current = readByte();
                    output[destination] = current;
                    destination += step;
                }
            }
            previous = current;
        }
    }
}
}

namespace BmrDecoderAlgorithms
{
void DecodeHuffman(std::span<const std::uint8_t> encoded, std::span<std::uint8_t> output,
    IBmrInputReadGuard* inputGuard, IBmrDestinationAccessGuard* destinationGuard)
{
    // 当前算法读取输入期间无分配/外部回调，故整段验证一次；未来原owner分配必须在此检查之前。
    if (inputGuard) inputGuard->ValidateRead(encoded);
    DecodeHuffmanInto(encoded, output, destinationGuard);
}

void MoveToFront(std::span<std::uint8_t> bytes) noexcept
{
    UndoMoveToFront(bytes);
}

void BurrowsWheeler(std::span<std::uint8_t> bytes, std::uint32_t key,
    IBmrDestinationAccessGuard* destinationGuard)
{
    const auto reordered = UndoBurrowsWheeler(bytes, key, destinationGuard);
    // helper返回前distribution已释放，释放回调也可能使旧target失效。
    if (destinationGuard) destinationGuard->ValidateAccess(bytes);
    std::copy(reordered.begin(), reordered.end(), bytes.begin());
}

void RunLength(std::span<const std::uint8_t> input, std::span<std::uint8_t> output, std::uint8_t step)
{
    DecodeRleInto(input, output, step, true);
}
}

ParamsRawBlobInstallException::ParamsRawBlobInstallException(
    const ParamsRawBlobInstallError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

ParamsRawBlobInstallError ParamsRawBlobInstallException::Error() const noexcept
{
    return error_;
}

InstalledParamsRawBlob ParamsRawBlobInstaller::Install(
    const std::span<const std::uint8_t> source)
{
    if (!HasBmrMagic(source))
        return {ParamsRawBlobStorageKind::Plain, {source.begin(), source.end()}};

    if (source.size() < BmrHeaderSize)
        ThrowInstallError(ParamsRawBlobInstallError::HeaderTruncated, "BMR header is truncated");

    const std::size_t finalSize = ReadDword(source, 4);
    const std::uint32_t key = ReadDword(source, 8);
    const std::size_t unpackedSize = ReadDword(source, 12);
    const std::size_t huffmanSize = ReadDword(source, 16);
    if (huffmanSize > source.size() - BmrHeaderSize)
        ThrowInstallError(
            ParamsRawBlobInstallError::HuffmanStreamOutOfRange,
            "BMR Huffman stream extends past the source buffer");

    const std::uint8_t step = source[3];
    if ((step == 0 && finalSize != unpackedSize) || finalSize == 0 || unpackedSize == 0)
        ThrowInstallError(
            ParamsRawBlobInstallError::InvalidSizeRelationship,
            "BMR final and intermediate sizes are not valid for the selected RLE mode");

    std::vector<std::uint8_t> intermediate(unpackedSize);
    DecodeHuffmanInto(source.subspan(BmrHeaderSize, huffmanSize), intermediate);
    UndoMoveToFront(intermediate);
    intermediate = UndoBurrowsWheeler(intermediate, key);

    if (step == 0)
        return {ParamsRawBlobStorageKind::Bmr, std::move(intermediate)};
    std::vector<std::uint8_t> output(finalSize);
    DecodeRleInto(intermediate, output, step, false);
    return {ParamsRawBlobStorageKind::Bmr, std::move(output)};
}

ParamsRawBlobRuntimeLayout ParamsRawBlobInstaller::InspectRuntimeSource(
    const std::span<const std::uint8_t> source)
{
    if (!HasBmrMagic(source))
        return {ParamsRawBlobStorageKind::Plain, source.size()};
    if (source.size() < 16)
        ThrowInstallError(ParamsRawBlobInstallError::HeaderTruncated, "BMR header is truncated");
    return {ParamsRawBlobStorageKind::Bmr, ReadDword(source, 4)};
}

void ParamsRawBlobInstaller::InstallIntoRuntime(const std::span<const std::uint8_t> source,
    const std::span<std::uint8_t> destination)
{
    const auto layout = InspectRuntimeSource(source);
    if (destination.size() != layout.byteCount)
        ThrowInstallError(ParamsRawBlobInstallError::DestinationSizeMismatch,
            "Published key array does not match the inspected source size");
    if (layout.storageKind == ParamsRawBlobStorageKind::Plain)
    {
        std::copy(source.begin(), source.end(), destination.begin());
        return;
    }

    if (source.size() < BmrHeaderSize)
        ThrowInstallError(ParamsRawBlobInstallError::HeaderTruncated, "BMR stream length is missing");
    const std::size_t unpackedSize = ReadDword(source, 12);
    const std::size_t huffmanSize = ReadDword(source, 16);
    if (huffmanSize > source.size() - BmrHeaderSize)
        ThrowInstallError(ParamsRawBlobInstallError::HuffmanStreamOutOfRange,
            "BMR Huffman stream extends past the source buffer");
    if (unpackedSize > destination.size())
        ThrowInstallError(ParamsRawBlobInstallError::InvalidSizeRelationship,
            "BMR intermediate output exceeds the published key array");

    // 43DFD3逐字节发布Huffman；43E000原位MTF，后续失败不能恢复旧key。
    const auto intermediate = destination.first(unpackedSize);
    DecodeHuffmanInto(source.subspan(BmrHeaderSize, huffmanSize), intermediate);
    UndoMoveToFront(intermediate);
    // 43E320先完整生成BWT临时数组，再memmove回已发布前缀。
    {
        const auto reordered = UndoBurrowsWheeler(intermediate, ReadDword(source, 8));
        std::copy(reordered.begin(), reordered.end(), intermediate.begin());
    }
    if (source[3] != 0)
    {
        // 43E0BE..43E0F5复制BWT结果，RLE后续直接写目标；无RLE时尾部原样保留。
        const std::vector<std::uint8_t> encodedRle(intermediate.begin(), intermediate.end());
        DecodeRleInto(encodedRle, destination, source[3], true);
    }
}
}
