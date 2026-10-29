#pragma once

#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ResourceCatalog.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
class IFileInfoProgress;
class ArchiveDirectoryRuntime;
struct LinkArchiveHeaderMetadata final
{
    std::string magic;
    std::uint32_t version = 0;
    std::uint16_t flags = 0;
    std::string embeddedArchiveName;
    std::uint64_t headerSize = 0;
};

struct LinkArchiveIndex final
{
    LinkArchiveHeaderMetadata header;
    ResourceArchiveEntry archive;
    std::uint32_t fileCount = 0;
    std::uint32_t maximumDataLength = 0;
};

class ILinkArchiveIndexCompletion
{
public:
    virtual ~ILinkArchiveIndexCompletion() = default;
    // scanner成功后、流关闭前调用；主线在此先写outer version，再写loaded。
    // 此回调的异常不属于493B40的catch，由主线outer LoadInfo处理。
    virtual void OnCompleted(const LinkArchiveIndex& index) = 0;
};

class LinkArchiveFormatError final : public std::runtime_error
{
public:
    LinkArchiveFormatError(std::uint64_t offset, const char* message);

    [[nodiscard]] std::uint64_t Offset() const noexcept;

private:
    std::uint64_t offset_ = 0;
};

// 只解析 LINK 的目录和 fileinfo；条目正文仍由后续 reader 按 offset/length/flags 读取。
[[nodiscard]] LinkArchiveIndex ParseLinkArchiveIndex(
    std::span<const std::byte> bytes,
    std::string nameSpace,
    std::string archivePath);

// 工具路径按需读取目录字段并跨过正文，保留严格格式检查及异常接口。
[[nodiscard]] LinkArchiveIndex ParseLinkArchiveIndexFile(
    const char* archivePath,
    std::string nameSpace,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);
// 带进度的真实扫描路径；包括终止条目前的取消检查。
[[nodiscard]] LinkArchiveIndex ParseLinkArchiveIndexFile(
    const char* archivePath,
    std::string nameSpace,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    IFileInfoProgress& progress);

// 生产直扫：open失败抛int1且不改index；成功打开后先清max、再清count，不清root。
// scanner原位发布header/tree/count/max，catch-all后抛int5，不回滚；流在展开中关闭。
// 成功才更新archiveVersion并通知completion；返回/抛错时由FileHandle关闭流。
// 带路径条目仅在scanner局部暂存，失败时销毁；已进入root的目录/文件保留。
void LoadRuntimeLinkArchiveIndexFileInto(
    const char* archivePath,
    std::string nameSpace,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    LinkArchiveIndex& index,
    IFileInfoProgress* progress = nullptr,
    ILinkArchiveIndexCompletion* completion = nullptr);

// 47FE60..47FF14：直接写原archiver目录及四个标量，没有值树/结果快照中转。
void LoadRuntimeLinkArchiveDirectoryFileInto(
    const char* archivePath, const FileTransactionContext* transactionContext, IStartFileApi& api,
    ArchiveDirectoryRuntime& directory, std::uint32_t& countAt30, std::uint32_t& maximumAt34,
    std::uint32_t& versionAt18, std::uint32_t& loadedAt58, IFileInfoProgress& progress);
}
