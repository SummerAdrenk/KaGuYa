#pragma once
#include <Runtime/ArchiveResourceNameRuntime.h>

#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ResourceCatalog.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/ArchiverInfoReference.h>

#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include <ranges>

namespace KaGuYa::Start::Runtime
{
class IFileInfoProgress;
class EnvironmentInstalledFiles;
class ArchiverInfoRuntime;
// 每次调用对应458D8C、458DD0或458FC7的一次Environment/path取源；不保活旧表。
class IInstalledFileTableSource
{
public:
    virtual ~IInstalledFileTableSource() = default;
    [[nodiscard]] virtual const EnvironmentInstalledFiles& CaptureInstalledTableAt4180C0() = 0;
};
struct InstallTableArchive final
{
    std::string file;
    std::string media;
};

// 生产入口借用原12-byte Ref容器；工具入口仍可使用值数组。不缓存安装表副本。
class InstallTableCatalogSource final
{
public:
    template<std::ranges::contiguous_range R>
        requires std::same_as<std::ranges::range_value_t<R>, InstallTableArchive>
    InstallTableCatalogSource(const R& values) noexcept : values_(values) {}
    InstallTableCatalogSource(const EnvironmentInstalledFiles& references) noexcept : references_(&references) {}
    InstallTableCatalogSource(IInstalledFileTableSource& source) noexcept : source_(&source) {}
    [[nodiscard]] std::size_t size() const;
    [[nodiscard]] InstallTableArchive ReadAt(std::size_t index) const;
    [[nodiscard]] ArchiveRawResourceNameRuntime CaptureFileNameAt458DD0(std::uint32_t index) const;
    [[nodiscard]] const char* ReadMediaAt458FC7(std::uint32_t index) const;
    [[nodiscard]] const void* Source() const noexcept;
private:
    [[nodiscard]] const EnvironmentInstalledFiles* CaptureReferences() const;
    std::span<const InstallTableArchive> values_;
    const EnvironmentInstalledFiles* references_ = nullptr;
    IInstalledFileTableSource* source_ = nullptr;
};

// 文件名CString已在调用前捕获；正文new先于media取源，不使用提前求值的make_shared。
[[nodiscard]] ArchiverInfoReference CreateArchiverInfoAt458FAB(
    const RuntimeCString& fileName, InstallTableCatalogSource source, std::uint32_t index,
    const char* title = nullptr, const YtlReferenceSlotRuntime& progress = {},
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

struct InstalledArchiveMetadata final
{
    std::string file;
    std::string media;
    std::string nameSpace;
    std::uint32_t archiveVersion = 0;
    std::uint32_t fileCount = 0;
    std::uint32_t maximumDataLength = 0;
};

struct InstallTableCatalogBuildResult final
{
    ResourceCatalog catalog;
    std::vector<InstalledArchiveMetadata> archives;
    std::uint32_t totalFileCount = 0;
    std::uint32_t maximumDataLength = 0;
    std::uint32_t cacheHitCount = 0;
    std::uint32_t directScanCount = 0;
    bool cacheRewritten = false;
};

// 458B60 只按 params InstallTable 的顺序建表；目录中未列出的 ARC 不得自动加入。
[[nodiscard]] InstallTableCatalogBuildResult BuildCatalogFromInstallTable(
    std::span<const InstallTableArchive> installTable,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);

// 458B60/47FA10：缓存可用时恢复 fileinfo；任一表项失效即在构建完成后重写整份 finfo.ini。
[[nodiscard]] InstallTableCatalogBuildResult BuildCatalogFromInstallTableWithCache(
    std::span<const InstallTableArchive> installTable,
    const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api);
// FileManager同源进度：只在首DWORD后增加一次范围，选中树及直扫按各自原顺序回调。
[[nodiscard]] InstallTableCatalogBuildResult BuildCatalogFromInstallTableWithCache(
    std::span<const InstallTableArchive> installTable,
    const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    IFileInfoProgress& progress);

// 458B60：逐项写入owner的目录；异常不回滚已发布项。459399的状态写点在关读流后、写缓存前。
// 调用方提供已清理的目录和原+0C状态；本入口不捕获异常，也不创建消息owner。
void BuildCatalogFromInstallTableWithCacheInto(
    InstallTableCatalogBuildResult& destination,
    std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable,
    const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext,
    IStartFileApi& api,
    IFileInfoProgress& progress);
void BuildCatalogFromInstallTableWithCacheInto(
    InstallTableCatalogBuildResult& destination, std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable, const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext, IStartFileApi& api,
    const char* capturedTitle, const YtlReferenceSlotRuntime& progress);

// 458D33..4593A8：仅操作已构造的+04树和+0C状态；不构造诊断vector/string，
// 不分配/接管第二个map，异常保留原位部分状态。sentinel仍由458BB5在catch外创建。
void BuildNativeCatalogFromInstallTableWithCacheInto(
    ArchiveNamespaceMapRuntime& directoryAt04, std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable, const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext, IStartFileApi& api,
    const char* capturedTitle, const YtlReferenceSlotRuntime& progress);
}
