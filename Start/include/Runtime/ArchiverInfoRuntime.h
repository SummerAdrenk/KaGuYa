#pragma once

#include <Runtime/FileInfoCacheStream.h>
#include <Runtime/LinkArchiveIndexParser.h>
#include <Runtime/ArchiveDirectoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 458FAB/459061：唯一 0x68 正文；工具快照按需导出，不保留同步的值目录。
#pragma pack(push, 4)
class ArchiverInfoRuntime final
{
public:
    ArchiverInfoRuntime(std::string archivePath, std::string media);
    ~ArchiverInfoRuntime() noexcept;
    struct InstalledNamesAt45905C final {};
    // media 已是借用裸地址，必须跨标题、path 两次构造后才读取。
    ArchiverInfoRuntime(InstalledNamesAt45905C, const char* archivePath, const char* media,
        const char* title = nullptr, const YtlReferenceSlotRuntime& progress = {},
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ArchiverInfoRuntime(const ArchiverInfoRuntime&) = delete;
    ArchiverInfoRuntime& operator=(const ArchiverInfoRuntime&) = delete;
    ArchiverInfoRuntime(ArchiverInfoRuntime&&) = delete;
    ArchiverInfoRuntime& operator=(ArchiverInfoRuntime&&) = delete;

    // 0=局部catch失败，1=缓存命中，2=直接扫描。失败不回滚已经写入的成员。
    [[nodiscard]] int LoadInfoAt47FA10(FileInfoCacheStream& cache,
        const FileTransactionContext* transaction, IStartFileApi& files, IFileInfoProgress& progress);
    // 生产入口使用正文内的同源 progress Ref，每次回调重新解析。
    [[nodiscard]] int LoadInfoAt47FA10(FileInfoCacheStream& cache,
        const FileTransactionContext* transaction, IStartFileApi& files);
    void ReleaseProgressAt4591EF();

    [[nodiscard]] int IsAvailableAt43B2B0(IStartFileApi& files) const;
    [[nodiscard]] const ArchiveQueryState& PublishQueryAt38(const ArchiveFileNodeRuntime& file) noexcept;

    // 显式工具/测试快照；生产查询及 finfo 写回不得调用。
    [[nodiscard]] LinkArchiveIndex Index() const;
    [[nodiscard]] ArchiveDirectoryRuntime& DirectoryAt04() noexcept { return directoryAt04_; }
    [[nodiscard]] const ArchiveDirectoryRuntime& DirectoryAt04() const noexcept { return directoryAt04_; }
    [[nodiscard]] const RuntimeCString& TitleAt00() const noexcept { return titleAt00_; }
    [[nodiscard]] const RuntimeCString& PathAt10() const noexcept { return pathAt10_; }
    [[nodiscard]] const RuntimeCString& MediaAt14() const noexcept { return mediaAt14_; }
    [[nodiscard]] std::uint32_t VersionAt18() const noexcept { return versionAt18_; }
    [[nodiscard]] std::int64_t LastWriteTimeAt20() const noexcept { return lastWriteTimeAt20_; }
    [[nodiscard]] std::uint64_t FileSizeAt28() const noexcept { return fileSizeAt28_; }
    [[nodiscard]] std::uint32_t LoadedAt58() const noexcept { return loadedAt58_; }
    [[nodiscard]] std::uint32_t CountAt30() const noexcept { return countAt30_; }
    [[nodiscard]] std::uint32_t MaximumAt34() const noexcept { return maximumAt34_; }
    [[nodiscard]] YtlReferenceSlotRuntime& ProgressAt5C() noexcept { return progressAt5C_; }

private:
    // 40B530的nil/赋值和458B60 EH状态按原时点手工构造，避免成员提前初始化。
    union { RuntimeCString titleAt00_; };
    union { ArchiveDirectoryRuntime directoryAt04_; };
    union { RuntimeCString pathAt10_; };
    union { RuntimeCString mediaAt14_; };
    std::uint32_t versionAt18_;
    std::byte unwrittenAt1C_[4];
    std::int64_t lastWriteTimeAt20_;
    std::uint64_t fileSizeAt28_;
    std::uint32_t countAt30_;
    std::uint32_t maximumAt34_;
    ArchiveQueryState queryAt38_;
    std::uint32_t loadedAt58_;
    union { YtlReferenceSlotRuntime progressAt5C_; };
    friend struct ArchiverInfoRuntimeLayout;
};
#pragma pack(pop)
static_assert(sizeof(ArchiverInfoRuntime) == 0x68);
}
