#pragma once

#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ResourceCatalog.h>
#include <Runtime/FileStreamRuntime.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReference;
enum class ArchiveEntryDecodeError
{
    LzssHeaderTruncated,
    LzssOutputSizeInvalid,
    LzssBitstreamExhausted,
    LzssOutputOverflow,
    LzssOutputSizeMismatch,
    LzssUninitializedWindowReference,
    BmrMagicMismatch,
    BmrMalformed,
    KeyEmpty,
    KeyLengthInvalid,
    EncryptedPayloadMalformed,
    UnsupportedAnVersion,
    UnsupportedAnCommand,
    MissingAnPictureMarker,
    UnsupportedPlVersion,
    EmptyPlPictureTable
};

class ArchiveEntryDecodeException final : public std::runtime_error
{
public:
    ArchiveEntryDecodeException(ArchiveEntryDecodeError error, const char* message);

    [[nodiscard]] ArchiveEntryDecodeError Error() const noexcept;

private:
    ArchiveEntryDecodeError error_;
};

struct LoadedArchiveEntry final
{
    // 显式离线值结果，不是CFileReader正文；生产消费者持有借用的reader Ref。
    std::string resourceName;
    std::vector<std::byte> bytes;
};

// 483D90/4D44C0 的局部文件流必须跨过正文变换，不能随范围读取提前关闭。
// 内部持有原16-byte CStreamFile；factory reader Ref/scratch的归属仍由外层工厂管理。
class ArchiveEntryReadStream final
{
public:
    ArchiveEntryReadStream(const ResolvedResourceEntry& entry,
        const FileTransactionContext* transactionContext, IStartFileApi& api);
    ArchiveEntryReadStream(const ResourceReadQuery& query,
        const FileTransactionContext* transactionContext, IStartFileApi& api);
    ~ArchiveEntryReadStream();
    ArchiveEntryReadStream(const ArchiveEntryReadStream&) = delete;
    ArchiveEntryReadStream& operator=(const ArchiveEntryReadStream&) = delete;

    [[nodiscard]] LoadedArchiveEntry Read(const std::string& resourceName);
    // Read抛出时保留已分配/命名/写入的状态；普通失败先清，再由工厂处理失败。
    void ReadInto(const std::string& resourceName, LoadedArchiveEntry& reader);
    // 工厂在 Seek 和读取 length 之间发布新 reader；独立 ReadInto 仍组合这两步。
    void SeekToEntry();
    void ReadCurrentRangeInto(const std::string& resourceName, LoadedArchiveEntry& reader);
    // 483FCF：buffer 分配成功后才读取捕获的名称指针，不缓存提前测出的长度。
    void ReadCurrentBorrowedRangeInto(const char* resourceName, LoadedArchiveEntry& reader);
    // 原生生产入口先缓存length，再解析并固定本次reader，Read后判断同一正文Ready。
    void ReadCurrentRangeInto(const std::string& resourceName, const ArchiveReaderReference& reader);
    void ReadCurrentBorrowedRangeInto(const char* resourceName, const ArchiveReaderReference& reader);

private:
    IStartFileApi& api_;
    ScopedFileStreamRuntime stream_;
    std::uint64_t offset_ = 0;
    std::uint32_t length_ = 0;
    const ResourceReadQuery* query_ = nullptr;
};

// 工具专用深导出；不能在生产decoder入口用DTO代替真实虚getter。
[[nodiscard]] LoadedArchiveEntry ExportArchiveReader(const ArchiveReaderReference& reader);

// 对应 0x483D90 的物理 ARC 区间读取阶段；失败码保持原函数的 1/2/3。
[[nodiscard]] std::vector<std::byte> ReadArchiveEntryRange(
    const ResolvedResourceEntry& entry,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);

// 483FCF/4D4749的具名reader：正有符号长度才分配、写原逻辑名称并Read。
[[nodiscard]] LoadedArchiveEntry ReadNamedArchiveEntryRange(
    const ResolvedResourceEntry& entry,
    const std::string& resourceName,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);

// 对应 0x484057..0x4841C7 与 0x445750；只处理 flags bit 0 的正文格式。
[[nodiscard]] std::vector<std::byte> DecodeArchiveLzssPayload(
    std::span<const std::byte> payload);

// 目标已由原临时byte Ref分配；晚取payload的长度DWORD不再决定输出大小。
void DecodeArchiveLzssPayload(std::span<const std::byte> payload,
    std::span<std::byte> destination);

// 对应 0x4841E7..0x48426F 与 0x445A00/0x43DCF0；EntryFlags 决定进入此分支。
[[nodiscard]] std::vector<std::byte> DecodeArchiveBmrPayload(
    std::span<const std::byte> payload);

// 一次性值结果适配口；生产消费者需让 ArchiveReaderFactoryRuntime 覆盖消费作用域。
// catch 清理再次抛出的异常继续外传；buffer/key 内部 Ref 身份仍有未闭合范围。
[[nodiscard]] std::optional<LoadedArchiveEntry> LoadArchiveEntry(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    std::span<const std::uint8_t> key);

// 489C90 -> 479FF0 -> 47CB00：生产入口只接受 params RawBlob 安装后的 Environment key 表。
[[nodiscard]] std::optional<LoadedArchiveEntry> LoadArchiveEntry(
    const ResourceCatalog& catalog,
    const std::string& resourceName,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    const IEnvironmentKeyTableSource& environmentKeyBytes);
}
