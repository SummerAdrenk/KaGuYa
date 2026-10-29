#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveEntryTransforms.h>
#include <Runtime/ParamsRawBlobInstaller.h>

#include <array>
#include <limits>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
class ScopedFile final
{
public:
    ScopedFile(const HANDLE file, IStartFileApi& api) noexcept
        : file_(file), api_(api)
    {
    }

    ~ScopedFile()
    {
        if (file_ != INVALID_HANDLE_VALUE)
            api_.Close(file_);
    }

    ScopedFile(const ScopedFile&) = delete;
    ScopedFile& operator=(const ScopedFile&) = delete;

private:
    HANDLE file_ = INVALID_HANDLE_VALUE;
    IStartFileApi& api_;
};

[[noreturn]] void ThrowDecodeError(
    const ArchiveEntryDecodeError error,
    const char* const message)
{
    throw ArchiveEntryDecodeException(error, message);
}

[[nodiscard]] std::uint32_t ReadDword(
    const std::span<const std::byte> source,
    const std::size_t offset) noexcept
{
    return std::to_integer<std::uint32_t>(source[offset]) |
           (std::to_integer<std::uint32_t>(source[offset + 1]) << 8u) |
           (std::to_integer<std::uint32_t>(source[offset + 2]) << 16u) |
           (std::to_integer<std::uint32_t>(source[offset + 3]) << 24u);
}

class LzssBitReader final
{
public:
    explicit LzssBitReader(const std::span<const std::byte> bytes) noexcept
        : bytes_(bytes)
    {
    }

    [[nodiscard]] std::uint32_t ReadBits(const unsigned count)
    {
        std::uint32_t value = 0;
        for (unsigned index = 0; index < count; ++index)
        {
            if (mask_ == 0)
            {
                if (byteOffset_ >= bytes_.size())
                {
                    ThrowDecodeError(
                        ArchiveEntryDecodeError::LzssBitstreamExhausted,
                        "LINK LZSS bit stream ended unexpectedly");
                }
                currentByte_ = std::to_integer<std::uint8_t>(bytes_[byteOffset_++]);
                mask_ = 0x80u;
            }
            value = (value << 1u) | ((currentByte_ & mask_) != 0 ? 1u : 0u);
            mask_ >>= 1u;
        }
        return value;
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t byteOffset_ = 0;
    std::uint8_t currentByte_ = 0;
    std::uint8_t mask_ = 0;
};
}

ArchiveEntryDecodeException::ArchiveEntryDecodeException(
    const ArchiveEntryDecodeError error,
    const char* const message)
    : std::runtime_error(message), error_(error)
{
}

ArchiveEntryDecodeError ArchiveEntryDecodeException::Error() const noexcept
{
    return error_;
}

static HANDLE OpenArchiveEntryFile(
    const char* const path,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.lpSecurityDescriptor = nullptr;
    securityAttributes.bInheritHandle = TRUE;

    const FileOpenRequest request{
        path,
        GENERIC_READ,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        &securityAttributes};
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
        throw 1;
    return file;
}

static void ClearReader(LoadedArchiveEntry& reader)
{
    std::vector<std::byte>().swap(reader.bytes);
    reader.resourceName.clear();
}

static void ReadArchiveEntryRangeCore(
    const HANDLE file, const std::uint32_t length,
    IStartFileApi& api, const std::string* resourceName, LoadedArchiveEntry& loaded,
    std::optional<const char*> borrowedName = std::nullopt)
{
    const bool named = resourceName || borrowedName.has_value();
    ClearReader(loaded);
    // 原reader先清buffer/name，signed length < 1跳过分配/命名/Read。
    // 无名称的原区间工具接口保持既有零长度错误契约。
    if (named && (length == 0u || length > 0x7FFFFFFFu)) return;
    loaded.bytes.resize(length);
    if (length == 0u) throw 3;
    if (resourceName) loaded.resourceName = *resourceName;
    else if (borrowedName) loaded.resourceName = *borrowedName ? *borrowedName : "";

    DWORD bytesRead = 0;
    const BOOL read = api.Read(file, loaded.bytes.data(), length, &bytesRead);
    // 431418..43143F：具名生产reader沿用原流FALSE错误码分支，工具仍严格。
    if (read != FALSE ? bytesRead != length :
        (!named || api.LastError() != ERROR_SUCCESS))
    {
        // 484040/4D47B4：Read返回失败先调用Clear；Read自身抛出则不经过这里。
        if (named) ClearReader(loaded);
        throw 3;
    }
}

ArchiveEntryReadStream::ArchiveEntryReadStream(const ResolvedResourceEntry& entry,
    const FileTransactionContext* transactionContext, IStartFileApi& api)
    : api_(api),stream_(api)
{
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES),nullptr,TRUE};
    const FileOpenRequest request{entry.archivePath.c_str(),GENERIC_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,&security};
    if(!stream_.OpenReaderWithApi(request,transactionContext))throw 1;
    offset_=entry.offset;length_=entry.length; // 保留显式值入口原有Open后快照时点。
}

ArchiveEntryReadStream::ArchiveEntryReadStream(const ResourceReadQuery& query,
    const FileTransactionContext* transactionContext, IStartFileApi& api)
    : api_(api),stream_(api),query_(&query)
{
    // 483E0A/4D453A捕获path后，42BAE0才分配文件正文/control；不提前取offset/length。
    const char* const path=query.Path();
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES),nullptr,TRUE};
    const FileOpenRequest request{path,GENERIC_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,&security};
    if(!stream_.OpenReaderWithApi(request,transactionContext))throw 1;
}

// 48430B/4D4A7F的真实Ref析构发生在变换及R+08复制之后，catch清reader之前。
ArchiveEntryReadStream::~ArchiveEntryReadStream()=default;

LoadedArchiveEntry ArchiveEntryReadStream::Read(const std::string& resourceName)
{
    LoadedArchiveEntry reader;
    ReadInto(resourceName, reader);
    return reader;
}

void ArchiveEntryReadStream::ReadInto(const std::string& resourceName, LoadedArchiveEntry& reader)
{
    SeekToEntry();
    ReadCurrentRangeInto(resourceName, reader);
}

void ArchiveEntryReadStream::SeekToEntry()
{
    // 483E68/483FB4：打开后才取offset，seek之后再取length；两次读取不能提前合成快照。
    if(!stream_.Native().SeekWithApi(query_?query_->Offset():offset_,FILE_BEGIN,api_))throw 2;
}

void ArchiveEntryReadStream::ReadCurrentRangeInto(const std::string& resourceName, LoadedArchiveEntry& reader)
{
    ReadArchiveEntryRangeCore(stream_.Native().HandleAtVslot00(),query_?query_->Length():length_,api_,&resourceName,reader);
}

void ArchiveEntryReadStream::ReadCurrentBorrowedRangeInto(const char* resourceName, LoadedArchiveEntry& reader)
{
    ReadArchiveEntryRangeCore(stream_.Native().HandleAtVslot00(),query_?query_->Length():length_,api_,nullptr,reader,
        std::optional<const char*>{resourceName});
}

static void ReadArchiveEntryNativeCore(FileStreamRuntime& stream, const std::uint32_t length,
    IStartFileApi& api, const std::string* name, const char* borrowedName,
    const ArchiveReaderReference& reference)
{
    // 483FB4..FC9：调用方已缓存length；Clear前才固定本次reader，之后不改用回调后的外层槽。
    auto* const reader = reference.Get();
    // 483FC5按本次reader的当前虚表调用Clear；空reader到原read0故障，不转成业务catch可吞异常。
    reader->ClearAt4327B0();
    if (length >= 1u && length <= 0x7FFFFFFFu)
    {
        reader->RawBytesAt04().AllocateAt412700(length);
        // 483FDD..484004：分配成功后才取名称和首NUL长度；不提前复制逻辑名。
        const char* const characters = name ? name->c_str() : borrowedName;
        const auto count = characters ? std::strlen(characters) : 0u;
        if (count > 0x7FFFFFFFu) throw std::length_error("archive reader name exceeds x86 CString length");
        SetRuntimeCStringAt437680(reader->RawNameAt10(), characters, static_cast<int>(count));
        // Set的管理器回调可替换byte Ref，缓存的length不变，目标指针必须在此重解。
        if(!stream.ReadExactWithApi(reader->RawBytesAt04().Data(),length,api))reader->ClearAt4327B0();
    }
    // 484046..51：Read返回false走Clear后仍调用Ready，不能提前throw或检查vector长度。
    if (!reader->ReadyAt419360()) throw 3;
}

void ArchiveEntryReadStream::ReadCurrentRangeInto(const std::string& resourceName,
    const ArchiveReaderReference& reader)
{
    ReadArchiveEntryNativeCore(stream_.Native(),query_?query_->Length():length_,api_,&resourceName,nullptr,reader);
}

void ArchiveEntryReadStream::ReadCurrentBorrowedRangeInto(const char* resourceName,
    const ArchiveReaderReference& reader)
{
    ReadArchiveEntryNativeCore(stream_.Native(),query_?query_->Length():length_,api_,nullptr,resourceName,reader);
}

LoadedArchiveEntry ExportArchiveReader(const ArchiveReaderReference& reference)
{
    auto* const reader = reference.Get();
    if (!reader) throw std::logic_error("archive reader export has no body");
    const char* const name = reader->NameAt4179F0();
    LoadedArchiveEntry result;
    if (name) result.resourceName.assign(name);
    const auto bytes = ArchiveReaderPayloadView(reference);
    result.bytes.assign(bytes.begin(), bytes.end());
    return result;
}

std::vector<std::byte> ReadArchiveEntryRange(const ResolvedResourceEntry& entry,
    const FileTransactionContext* transactionContext, IStartFileApi& api)
{
    const HANDLE file = OpenArchiveEntryFile(entry.archivePath.c_str(), transactionContext, api);
    const ScopedFile closeFile(file, api);
    if (api.SeekAbsolute(file, entry.offset) == FALSE) throw 2;
    LoadedArchiveEntry reader;
    ReadArchiveEntryRangeCore(file, entry.length, api, nullptr, reader);
    return std::move(reader.bytes);
}

LoadedArchiveEntry ReadNamedArchiveEntryRange(const ResolvedResourceEntry& entry,
    const std::string& resourceName, const FileTransactionContext* transactionContext, IStartFileApi& api)
{
    ArchiveEntryReadStream stream(entry, transactionContext, api);
    return stream.Read(resourceName);
}

std::vector<std::byte> DecodeArchiveLzssPayload(
    const std::span<const std::byte> payload)
{
    if (payload.size() < sizeof(std::uint32_t))
    {
        ThrowDecodeError(
            ArchiveEntryDecodeError::LzssHeaderTruncated,
            "LINK LZSS payload is shorter than its size field");
    }

    const std::uint32_t outputSize = ReadDword(payload, 0);
    if (outputSize == 0u ||
        outputSize > static_cast<std::uint32_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        ThrowDecodeError(
            ArchiveEntryDecodeError::LzssOutputSizeInvalid,
            "LINK LZSS output size is not a positive x86 allocation size");
    }

    std::vector<std::byte> output(outputSize);
    DecodeArchiveLzssPayload(payload, output);
    return output;
}

std::vector<std::byte> DecodeArchiveBmrPayload(
    const std::span<const std::byte> payload)
{
    if (payload.size() < 3u ||
        payload[0] != std::byte{'B'} ||
        payload[1] != std::byte{'M'} ||
        payload[2] != std::byte{'R'})
    {
        ThrowDecodeError(
            ArchiveEntryDecodeError::BmrMagicMismatch,
            "LINK BMR payload does not begin with the required BMR signature");
    }

    try
    {
        const auto bytes = std::span<const std::uint8_t>(
            reinterpret_cast<const std::uint8_t*>(payload.data()), payload.size());
        InstalledParamsRawBlob decoded = ParamsRawBlobInstaller::Install(bytes);
        if (decoded.storageKind != ParamsRawBlobStorageKind::Bmr)
        {
            ThrowDecodeError(
                ArchiveEntryDecodeError::BmrMagicMismatch,
                "LINK BMR branch did not produce a BMR decoding result");
        }

        std::vector<std::byte> output(decoded.bytes.size());
        for (std::size_t index = 0; index < decoded.bytes.size(); ++index)
            output[index] = static_cast<std::byte>(decoded.bytes[index]);
        return output;
    }
    catch (const ParamsRawBlobInstallException&)
    {
        ThrowDecodeError(
            ArchiveEntryDecodeError::BmrMalformed,
            "LINK BMR payload is structurally invalid");
    }
}

std::optional<LoadedArchiveEntry> LoadArchiveEntry(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    const std::span<const std::uint8_t> key)
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog, transactionContext, api, memory, ToolArchiveCodecsTag{});
    if (!factory.Load(resourceName, key)) return std::nullopt;
    return ExportArchiveReader(factory.Reader());
}

std::optional<LoadedArchiveEntry> LoadArchiveEntry(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes)
{
    ArchiveReaderMemoryApi memory;
    ArchiveReaderFactoryRuntime factory(catalog, transactionContext, api, memory, ToolArchiveCodecsTag{});
    if (!factory.Load(resourceName, environmentKeyBytes)) return std::nullopt;
    return ExportArchiveReader(factory.Reader());
}
}
