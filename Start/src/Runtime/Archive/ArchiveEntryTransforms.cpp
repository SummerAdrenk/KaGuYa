#include <Runtime/ArchiveEntryTransforms.h>
#include <Runtime/ArchiveEntryCodecs.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/ArchiveLzssRuntime.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveReaderNative.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ParamsByteArray.h>
#include <Runtime/ParamsRawBlobInstaller.h>

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] ByteArrayReferenceRuntime& CurrentReaderBytes(const ArchiveReaderReference& reader)
{
    auto* const body = reader.Get();
    if (!body) throw std::logic_error("Archive transform reader is null (safe boundary)");
    auto* const bytes = body->BytesAt4172C0();
    if (!bytes) throw std::logic_error("Archive transform byte slot is null (safe boundary)");
    return *bytes;
}

[[nodiscard]] std::span<std::uint8_t> BoundedCodecBytes(const ByteArrayReferenceRuntime& bytes)
{
    const auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(
        bytes.RawSlot().controlAt08);
    // 内部codec尚为连续byte解析；不把任意stride结构解释成同一字节流。
    if (control && control->strideAt0C != 1)
        throw std::out_of_range("Archive codec requires byte stride one (safe boundary)");
    return BorrowBoundedByteArrayCopySpan(bytes);
}

void TransformNativeLzss(const ArchiveReaderReference& reader)
{
    const auto initial = BoundedCodecBytes(CurrentReaderBytes(reader));
    if (initial.size() < 4)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssHeaderTruncated,
            "LINK LZSS payload is shorter than its size field");
    const std::uint32_t outputSize = static_cast<std::uint32_t>(initial[0]) |
        (static_cast<std::uint32_t>(initial[1]) << 8) |
        (static_cast<std::uint32_t>(initial[2]) << 16) |
        (static_cast<std::uint32_t>(initial[3]) << 24);

    // 484094/4D4808先建立state4；412700控制块失败的无control正文仍不自动回收。
    ByteArrayReferenceRuntime temporary;
    temporary.AllocateAt412700(outputSize);
    // 48409F/4D4813再次解析outer；原CLzss owner取得在此与getter之间，尚未迁移。
    auto* const decodingReader = reader.Get();
    if (!decodingReader) throw std::logic_error("Archive LZSS source reader is null (safe boundary)");
    auto* const source = decodingReader->BytesAt4172C0();
    if (!source) throw std::logic_error("Archive LZSS source slot is null (safe boundary)");
    const auto destination = BoundedCodecBytes(temporary);
    const auto encoded = BoundedCodecBytes(*source);
    DecodeArchiveLzssPayload(std::as_bytes(encoded), std::as_writable_bytes(destination));
    // 4841B0/4D4924重新取得目标；465810复制增持后40CF00才减持临时，不是move。
    CurrentReaderBytes(reader) = temporary;
}

void RequireCoveredAccess(std::span<const std::uint8_t> extent, std::span<const std::uint8_t> access)
{
    const auto begin = reinterpret_cast<std::uintptr_t>(extent.data());
    const auto at = reinterpret_cast<std::uintptr_t>(access.data());
    if (at < begin || at - begin > extent.size() || access.size() > extent.size() - (at - begin))
        throw std::out_of_range("Archive BMR access exceeds its captured byte extent (safe boundary)");
}

class BmrInputGuard final : public IBmrInputReadGuard
{
public:
    explicit BmrInputGuard(std::span<const std::uint8_t> extent) noexcept : extent_(extent) {}
    void ValidateRead(std::span<const std::uint8_t> access) override
    {
        // 原payload已经缓存；只验缓存范围，不重读旧reader/control或以换槽猜测存活。
        // 保证借用内存仍活是调用前提，guard既不增持，也不把另一份有效旧持有拒绝掉。
        RequireCoveredAccess(extent_, access);
    }
private:
    std::span<const std::uint8_t> extent_;
};

class BmrOutputGuard final : public IBmrDestinationAccessGuard
{
public:
    explicit BmrOutputGuard(std::span<const std::uint8_t> extent) noexcept : extent_(extent) {}
    void ValidateAccess(std::span<const std::uint8_t> access) override
    {
        // 与原43DCF0固定目标地址一致；不把临时Ref后来换槽解释成缓存目标失效。
        RequireCoveredAccess(extent_, access);
    }
private:
    std::span<const std::uint8_t> extent_;
};

void TransformNativeBmr(const ArchiveReaderReference& reader)
{
    auto& bytes = CurrentReaderBytes(reader);
    // 明确现代codec适配：三参入口没有原共享压缩manager来源，不能假造445110全局owner。
    // 局部decoder在失败离开时清Header，区别于原共享owner保留Header到后续调用。
    // 原两次445110及其分配/重入时序、Huffman owner和算法临时Ref仍待后续整体迁移。
    BmrDecoderRuntime decoder;
    const auto input = BoundedCodecBytes(bytes);
    BmrInputGuard readGuard(input);
    std::uint32_t outputSize;
    const std::uint8_t* payload;
    try
    {
        payload = decoder.IdentifyAt445A00(input, outputSize, &readGuard);
    }
    catch (const ParamsRawBlobInstallException&)
    {
        // 保留归档工具的有界错误分类，不把现代截断检查伪称原机故障。
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::BmrMalformed,
            "LINK BMR payload is structurally invalid");
    }
    // 原miss继续使用未写Str与null输入；有界适配明确拒绝，不补零长度成功。
    if (!payload)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::BmrMagicMismatch,
            "LINK BMR payload does not begin with the required BMR signature");

    ByteArrayReferenceRuntime temporary;
    temporary.AllocateAt412700(outputSize);
    const auto destination = BoundedCodecBytes(temporary);
    BmrOutputGuard writeGuard(destination);
    // 48420F/4D4983缓存原payload，分配后不得重新识别或重新绑定当前reader作为输入。
    try
    {
        decoder.DecodeAt43DCF0({payload, input.size() - sizeof(BmrHeaderRuntime)}, destination,
            &readGuard, &writeGuard);
    }
    catch (const ParamsRawBlobInstallException&)
    {
        // 仅转换codec语义异常；412700、外层getter及465810回调不在此捕获范围内。
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::BmrMalformed,
            "LINK BMR payload is structurally invalid");
    }
    // 484258/4D49CC在解码及Header正常释放之后，再解析当前目标执行465810。
    CurrentReaderBytes(reader) = temporary;
}

ArchiveCodecManagerRuntime& CaptureCodecManager(const ArchiveCodecServices& codecs)
{
    auto* manager = codecs.managers.CaptureAt445910();
    if (!manager) throw std::logic_error("Archive codec manager is null (safe boundary)");
    return *manager;
}

void TransformNativeLzss(const ArchiveReaderReference& reader, const ArchiveCodecServices& codecs)
{
    const auto initial = BoundedCodecBytes(CurrentReaderBytes(reader));
    if (initial.size() < 4)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssHeaderTruncated,
            "LINK LZSS payload is shorter than its size field");
    const auto count = static_cast<std::uint32_t>(initial[0]) |
        (static_cast<std::uint32_t>(initial[1]) << 8) |
        (static_cast<std::uint32_t>(initial[2]) << 16) |
        (static_cast<std::uint32_t>(initial[3]) << 24);
    ByteArrayReferenceRuntime temporary;
    temporary.AllocateAt412700(count);
    // 48409F缓存reader身份；445910/工厂回调之后使用它，不重新解析outer。
    auto* const decodingReader = reader.Get();
    if (!decodingReader) throw std::logic_error("Archive LZSS source reader is null (safe boundary)");
    auto* const decoder = GetLzssAt4840B0(CaptureCodecManager(codecs));
    if (!decoder) throw std::logic_error("Archive LZSS body is null (safe boundary)");
    auto* const source = decodingReader->BytesAt4172C0();
    if (!source) throw std::logic_error("Archive LZSS source slot is null (safe boundary)");
    const auto destination = BoundedCodecBytes(temporary);
    const auto encoded = BoundedCodecBytes(*source);
    if (encoded.size() < 4)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::LzssHeaderTruncated,
            "LINK LZSS payload is shorter than its size field");
    decoder->DecodeAt445750(encoded.subspan(4), destination);
    CurrentReaderBytes(reader) = temporary;
}

void TransformNativeBmr(const ArchiveReaderReference& reader, const ArchiveCodecServices& codecs)
{
    auto& source = CurrentReaderBytes(reader);
    auto* const identifying = GetBmrAt445110(CaptureCodecManager(codecs));
    if (!identifying) throw std::logic_error("Archive BMR body is null (safe boundary)");
    // 4841F7在getter/codec创建后才解析先前缓存的byte Ref；不能提前构造span。
    const auto input = BoundedCodecBytes(source);
    BmrInputGuard readGuard(input);
    std::uint32_t outputSize;
    const auto* const payload = identifying->IdentifyAt445A00(input, outputSize, &readGuard);
    if (!payload)
        throw ArchiveEntryDecodeException(ArchiveEntryDecodeError::BmrMagicMismatch,
            "LINK BMR payload does not begin with the required BMR signature");
    ByteArrayReferenceRuntime temporary;
    temporary.AllocateAt412700(outputSize);
    auto* const decoding = GetBmrAt445110(CaptureCodecManager(codecs));
    if (!decoding) throw std::logic_error("Archive BMR decoding body is null (safe boundary)");
    const auto destination = BoundedCodecBytes(temporary);
    BmrOutputGuard writeGuard(destination);
    decoding->DecodeAt43DCF0({payload, input.size() - sizeof(BmrHeaderRuntime)}, destination,
        codecs.managers, codecs.errors, &readGuard, &writeGuard);
    // Header清理之后，484258重新取目标；codec/reader均无额外保活或身份缓存。
    CurrentReaderBytes(reader) = temporary;
}

[[noreturn]] void ThrowTransformError(
    const ArchiveEntryDecodeError error,
    const char* const message)
{
    throw ArchiveEntryDecodeException(error, message);
}

void RequireRange(
    const std::span<const std::byte> bytes,
    const std::size_t offset,
    const std::size_t length)
{
    if (offset > bytes.size() || length > bytes.size() - offset)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::EncryptedPayloadMalformed,
            "encrypted LINK image payload contains an out-of-range field");
    }
}

[[nodiscard]] std::uint16_t ReadWord(
    const std::span<const std::byte> bytes,
    const std::size_t offset)
{
    RequireRange(bytes, offset, 2u);
    return static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(bytes[offset])) |
           static_cast<std::uint16_t>(
               std::to_integer<std::uint8_t>(bytes[offset + 1]) << 8u);
}

[[nodiscard]] std::uint32_t ReadDword(
    const std::span<const std::byte> bytes,
    const std::size_t offset)
{
    RequireRange(bytes, offset, 4u);
    return std::to_integer<std::uint32_t>(bytes[offset]) |
           (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 8u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 16u) |
           (std::to_integer<std::uint32_t>(bytes[offset + 3]) << 24u);
}

[[nodiscard]] std::size_t CheckedAdd(
    const std::size_t first,
    const std::size_t second)
{
    if (second > (std::numeric_limits<std::size_t>::max)() - first)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::EncryptedPayloadMalformed,
            "encrypted LINK image offset overflows the host size type");
    }
    return first + second;
}

[[nodiscard]] std::size_t CheckedMultiply(
    const std::size_t first,
    const std::size_t second)
{
    if (first != 0u && second > (std::numeric_limits<std::size_t>::max)() / first)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::EncryptedPayloadMalformed,
            "encrypted LINK image dimensions overflow the host size type");
    }
    return first * second;
}

[[nodiscard]] std::size_t ReadImageSize(
    const std::span<const std::byte> bytes,
    const std::size_t dimensionsOffset,
    const bool hasStoredBytesPerPixel)
{
    const std::size_t width = ReadDword(bytes, dimensionsOffset);
    const std::size_t height = ReadDword(bytes, CheckedAdd(dimensionsOffset, 4u));
    const std::size_t bytesPerPixel = hasStoredBytesPerPixel
        ? ReadDword(bytes, CheckedAdd(dimensionsOffset, 8u))
        : 4u;
    return CheckedMultiply(CheckedMultiply(width, height), bytesPerPixel);
}

void XorRegionNew(
    const std::span<std::byte> region,
    const ArchiveKeyView key)
{
    const auto keyByte = [&key](const std::size_t offset) -> std::uint8_t
    {
        return key.storage[key.index + offset];
    };
    const std::size_t fullBlocks = region.size() >> 5u;
    for (std::size_t block = 0; block < fullBlocks; ++block)
    {
        const std::uint32_t byteOffset = static_cast<std::uint32_t>(block) * 32u;
        const std::size_t keyOffset = byteOffset % key.count;
        const std::size_t dataOffset = block * 32u;
        for (std::size_t index = 0; index < 32u; ++index)
            region[dataOffset + index] ^= static_cast<std::byte>(keyByte(keyOffset + index));
    }

    const std::size_t remainderOffset = fullBlocks * 32u;
    const std::size_t remainder = region.size() - remainderOffset;
    const std::uint32_t keyCycleOffset = static_cast<std::uint32_t>(fullBlocks) * 32u;
    const std::size_t keyOffset = keyCycleOffset % key.count;
    const std::size_t completeDwords = remainder >> 2u;
    for (std::size_t index = 0; index < completeDwords * 4u; ++index)
        region[remainderOffset + index] ^= static_cast<std::byte>(keyByte(keyOffset + index));

    const std::size_t tailLength = remainder & 3u;
    const std::size_t tailOffset = remainderOffset + completeDwords * 4u;
    const std::size_t tailKeyOffset = keyOffset + completeDwords * 4u;
    for (std::size_t index = 0; index < tailLength; ++index)
    {
        region[tailOffset + index] ^=
            static_cast<std::byte>(keyByte(tailKeyOffset + tailLength - 1u - index));
    }
}

void XorRegionOld(
    const std::span<std::byte> region,
    const ArchiveKeyView key)
{
    const auto keyByte = [&key](const std::size_t offset) -> std::uint8_t
    {
        return key.storage[key.index + offset];
    };
    const std::uint32_t keyBlockCount = static_cast<std::uint32_t>(key.count >> 5u);
    const std::size_t fullBlocks = region.size() >> 5u;
    for (std::size_t block = 0; block < fullBlocks; ++block)
    {
        const std::uint32_t dwordIndex =
            (static_cast<std::uint32_t>(block) * 8u) % keyBlockCount;
        const std::size_t keyOffset = static_cast<std::size_t>(dwordIndex) * 4u;
        const std::size_t dataOffset = block * 32u;
        for (std::size_t index = 0; index < 32u; ++index)
            region[dataOffset + index] ^= static_cast<std::byte>(keyByte(keyOffset + index));
    }

    const std::size_t remainderOffset = fullBlocks * 32u;
    const std::size_t remainder = region.size() - remainderOffset;
    const std::uint32_t dwordIndex =
        (static_cast<std::uint32_t>(fullBlocks) * 8u) % keyBlockCount;
    const std::size_t keyOffset = static_cast<std::size_t>(dwordIndex) * 4u;
    const std::size_t completeDwords = remainder >> 2u;
    for (std::size_t index = 0; index < completeDwords * 4u; ++index)
        region[remainderOffset + index] ^= static_cast<std::byte>(keyByte(keyOffset + index));

    const std::size_t tailLength = remainder & 3u;
    const std::size_t tailOffset = remainderOffset + completeDwords * 4u;
    const std::size_t tailKeyOffset = keyOffset + completeDwords * 4u;
    for (std::size_t index = 0; index < tailLength; ++index)
    {
        region[tailOffset + index] ^=
            static_cast<std::byte>(keyByte(tailKeyOffset + tailLength - 1u - index));
    }
}

void ConvertRange(
    const std::span<std::byte> bytes,
    const std::size_t offset,
    const std::size_t length,
    const ArchiveKeyView key,
    const bool useNewCipher)
{
    RequireRange(bytes, offset, length);
    std::span<std::byte> region(bytes.data() + offset, length);
    if (useNewCipher)
        XorRegionNew(region, key);
    else
        XorRegionOld(region, key);
}

void ConvertAn(
    const std::span<std::byte> bytes,
    const ArchiveKeyView key)
{
    const std::span<const std::byte> view(bytes);
    const std::uint16_t version = ReadWord(view, 2u);
    if (version == 0x3030u || version == 0x3031u)
    {
        const std::size_t listCount = ReadWord(view, 20u);
        const std::size_t imageTableOffset = CheckedAdd(24u, CheckedMultiply(listCount, 4u));
        std::size_t imageCount = ReadWord(view, imageTableOffset);
        std::size_t cursor = CheckedAdd(imageTableOffset, 2u);
        while (imageCount-- > 0u)
        {
            const std::size_t dimensionsOffset = CheckedAdd(cursor, 8u);
            const bool hasStoredBytesPerPixel = version == 0x3031u;
            const std::size_t imageSize = ReadImageSize(
                view, dimensionsOffset, hasStoredBytesPerPixel);
            const std::size_t dataOffset = CheckedAdd(
                dimensionsOffset, hasStoredBytesPerPixel ? 12u : 8u);
            ConvertRange(bytes, dataOffset, imageSize, key, true);
            cursor = CheckedAdd(dataOffset, imageSize);
        }
        return;
    }

    if (version != 0x3032u && version != 0x3132u)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::UnsupportedAnVersion,
            "encrypted AN payload uses an unsupported version");
    }

    std::size_t cursor = 8u;
    std::size_t commandCount = ReadWord(view, 4u);
    while (commandCount-- > 0u)
    {
        RequireRange(view, cursor, 1u);
        const std::uint8_t command = std::to_integer<std::uint8_t>(view[cursor++]);
        std::size_t argumentSize = 0;
        if (command == 1u)
            argumentSize = 8u;
        else if (command >= 2u && command <= 5u)
            argumentSize = 4u;
        else if (command != 0u)
        {
            ThrowTransformError(
                ArchiveEntryDecodeError::UnsupportedAnCommand,
                "encrypted AN payload contains an unsupported command");
        }
        RequireRange(view, cursor, argumentSize);
        cursor += argumentSize;
    }

    const std::size_t tableCount = ReadWord(view, cursor);
    cursor = CheckedAdd(cursor, 2u);
    cursor = CheckedAdd(cursor, CheckedMultiply(tableCount, 8u));
    RequireRange(view, cursor, 0u);

    bool singlePictureLayout = false;
    if (version == 0x3132u)
    {
        RequireRange(view, cursor, 7u);
        constexpr std::byte PictureMarker[] = {
            std::byte{'['}, std::byte{'P'}, std::byte{'I'}, std::byte{'C'}, std::byte{']'}};
        if (!std::equal(std::begin(PictureMarker), std::end(PictureMarker), view.begin() + cursor))
        {
            ThrowTransformError(
                ArchiveEntryDecodeError::MissingAnPictureMarker,
                "encrypted AN21 payload is missing its [PIC] marker");
        }
        cursor += 5u;
        const std::uint16_t pictureVersion = ReadWord(view, cursor);
        if (pictureVersion != 0x3030u && pictureVersion != 0x3031u)
        {
            ThrowTransformError(
                ArchiveEntryDecodeError::MissingAnPictureMarker,
                "encrypted AN21 payload has an unsupported [PIC] version");
        }
        singlePictureLayout = pictureVersion == 0x3031u;
        cursor += 2u;
    }

    const std::size_t imageCount = ReadWord(view, cursor);
    if (imageCount == 0u)
        return;
    std::size_t recordOffset = CheckedAdd(cursor, 18u);
    const std::size_t processCount = singlePictureLayout ? 1u : imageCount;
    for (std::size_t index = 0; index < processCount; ++index)
    {
        const std::size_t dimensionsOffset = CheckedAdd(recordOffset, 8u);
        const std::size_t imageSize = ReadImageSize(view, dimensionsOffset, true);
        const std::size_t dataOffset = CheckedAdd(dimensionsOffset, 12u);
        ConvertRange(bytes, dataOffset, imageSize, key, true);
        recordOffset = CheckedAdd(dataOffset, imageSize);
    }
}

void ConvertPl(
    const std::span<std::byte> bytes,
    const ArchiveKeyView key)
{
    const std::span<const std::byte> view(bytes);
    const std::uint16_t version = ReadWord(view, 2u);
    if (version != 0x3030u && version != 0x3031u)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::UnsupportedPlVersion,
            "encrypted PL payload uses an unsupported version");
    }

    const std::size_t imageCount = ReadWord(view, 4u);
    if (imageCount == 0u)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::EmptyPlPictureTable,
            "encrypted PL payload contains no pictures");
    }

    std::size_t recordOffset = 22u;
    const std::size_t processCount = version == 0x3031u ? 1u : imageCount;
    for (std::size_t index = 0; index < processCount; ++index)
    {
        const std::size_t dimensionsOffset = CheckedAdd(recordOffset, 8u);
        const std::size_t imageSize = ReadImageSize(view, dimensionsOffset, true);
        const std::size_t dataOffset = CheckedAdd(dimensionsOffset, 12u);
        ConvertRange(bytes, dataOffset, imageSize, key, true);
        recordOffset = CheckedAdd(dataOffset, imageSize);
    }
}

void ValidateArchiveKey(const ArchiveKeyView key)
{
    if (key.storage.empty() || key.count == 0u)
        ThrowTransformError(ArchiveEntryDecodeError::KeyEmpty, "LINK key table is empty");
    if ((key.count & 31u) != 0u ||
        key.index > key.storage.size() || key.count > key.storage.size() - key.index)
    {
        ThrowTransformError(
            ArchiveEntryDecodeError::KeyLengthInvalid,
            "LINK key table length is not divisible by 32");
    }
}

void ConvertArchiveKeyInPlace(const std::span<std::byte> output, const ArchiveKeyView key,
    const std::uint32_t archiveVersion, const bool runtimeCatch)
{
    const std::span<const std::byte> view(output);
    const bool useNewCipher = archiveVersion == 5u || archiveVersion == 6u;

    if (output.size() >= 0x36u && ReadWord(view, 0u) == 0x4D42u)
    {
        const std::size_t pixelOffset = ReadDword(view, 10u);
        RequireRange(view, pixelOffset, 0u);
        ConvertRange(output, pixelOffset, output.size() - pixelOffset, key, useNewCipher);
        return;
    }

    if (output.size() >= 2u && ReadWord(view, 0u) == 0x5041u)
    {
        const std::uint16_t version = ReadWord(view, 2u);
        const std::size_t dataOffset =
            version == 0x322Du || version == 0x332Du ? 24u : 12u;
        RequireRange(view, dataOffset, 0u);
        ConvertRange(output, dataOffset, output.size() - dataOffset, key, useNewCipher);
        return;
    }

    // 0x46E200 在旧 LINK3/4 上只识别 BMP/AP，其他签名保持原样。
    if (!useNewCipher)
        return;

    // 46E34F..46E76E / EH 5E8F28：只有新版 AN/PL 处于内部捕获范围，BMP/AP 不在内。
    try
    {
        if (output.size() >= 2u && ReadWord(view, 0u) == 0x4E41u)
            ConvertAn(output, key);
        else if (output.size() >= 2u && ReadWord(view, 0u) == 0x4C50u)
            ConvertPl(output, key);
    }
    catch (const ArchiveEntryDecodeException& error)
    {
        if (!runtimeCatch) throw;
        // 原版显式 int 异常分别位于46E782/46E797/46E7AC/46E7C1/46E7D6/46E7EB。
        // 范围检查是现代安全约束，不能把它或分配错误映射成这些原版 int。
        switch (error.Error())
        {
        case ArchiveEntryDecodeError::UnsupportedAnVersion:
        case ArchiveEntryDecodeError::UnsupportedAnCommand:
        case ArchiveEntryDecodeError::MissingAnPictureMarker:
        case ArchiveEntryDecodeError::UnsupportedPlVersion:
        case ArchiveEntryDecodeError::EmptyPlPictureTable:
            return;
        default:
            throw;
        }
    }
}
}

std::vector<std::byte> ApplyArchiveKeyConvert(
    const std::span<const std::byte> payload,
    const ArchiveKeyView key,
    const std::uint32_t archiveVersion)
{
    ValidateArchiveKey(key);
    std::vector<std::byte> output(payload.begin(), payload.end());
    ConvertArchiveKeyInPlace(output, key, archiveVersion, false);
    return output;
}

std::vector<std::byte> ApplyArchiveKeyConvert(
    const std::span<const std::byte> payload,
    const std::span<const std::uint8_t> key,
    const std::uint32_t archiveVersion)
{
    return ApplyArchiveKeyConvert(payload, ArchiveKeyView{key, 0u, key.size()}, archiveVersion);
}

std::vector<std::byte> TransformArchiveEntryPayload(
    const ResolvedResourceEntry& entry,
    const std::span<const std::byte> payload,
    const std::span<const std::uint8_t> key)
{
    if ((entry.flags & 1u) != 0u)
        return DecodeArchiveLzssPayload(payload);
    if ((entry.flags & 2u) != 0u)
        return DecodeArchiveBmrPayload(payload);
    if ((entry.flags & 4u) != 0u)
        return ApplyArchiveKeyConvert(payload, key, entry.archiveVersion);
    return {payload.begin(), payload.end()};
}

void TransformArchiveEntryPayloadInPlace(
    const ResolvedResourceEntry& entry,
    std::vector<std::byte>& payload,
    const std::span<const std::uint8_t> key)
{
    if ((entry.flags & 1u) != 0u)
        payload = DecodeArchiveLzssPayload(payload);
    else if ((entry.flags & 2u) != 0u)
        payload = DecodeArchiveBmrPayload(payload);
    else if ((entry.flags & 4u) != 0u)
    {
        const ArchiveKeyView view{key, 0u, key.size()};
        ValidateArchiveKey(view);
        ConvertArchiveKeyInPlace(payload, view, entry.archiveVersion, true);
    }
}

void TransformArchiveEntryPayloadInPlace(
    const ResolvedResourceEntry& entry,
    const ArchiveReaderReference& reader,
    const std::span<const std::uint8_t> key)
{
    if ((entry.flags & 1u) != 0u)
        TransformNativeLzss(reader);
    else if ((entry.flags & 2u) != 0u)
        TransformNativeBmr(reader);
    else if ((entry.flags & 4u) != 0u)
    {
        const ArchiveKeyView view{key, 0u, key.size()};
        ValidateArchiveKey(view);
        auto& bytes = CurrentReaderBytes(reader);
        if (!bytes.RawSlot().baseAt00) return; // 46E231..235只看base，空正文不访问字符。
        const auto extent = BoundedCodecBytes(bytes);
        const auto count = bytes.Size();
        if (count > extent.size())
            throw std::out_of_range("Archive key conversion exceeds the byte extent (safe boundary)");
        ConvertArchiveKeyInPlace(std::as_writable_bytes(extent.first(count)), view, entry.archiveVersion, true);
    }
}

void TransformArchiveEntryPayloadInPlace(const ResolvedResourceEntry& entry,
    const ArchiveReaderReference& reader, std::span<const std::uint8_t> key,
    const ArchiveCodecServices& codecs)
{
    if ((entry.flags & 1u) != 0u) TransformNativeLzss(reader, codecs);
    else if ((entry.flags & 2u) != 0u) TransformNativeBmr(reader, codecs);
    // 仅key/无变换共用旧函数，不会进入它的显式工具压缩分支。
    else TransformArchiveEntryPayloadInPlace(entry, reader, key);
}
}
