#pragma once

#include <Runtime/FileInfoCache.h>
#include <Runtime/PersistenceFileAccess.h>

namespace KaGuYa::Start::Runtime
{
class ArchiverInfoRuntime;
class ArchiveNamespaceMapRuntime;
class IArchiveDirectoryWriterRuntime;

// 491200 与 488340/48F497：遍历同一 native Ref 范围，每次回调后重读 end。
[[nodiscard]] std::uint32_t TreeSizeAt491200(const ArchiveDirectoryRuntime& directory);
void SerializeChildren(IArchiveDirectoryWriterRuntime& writer, const ArchiveDirectoryRuntime& directory);

// 489060生产写端：false表示普通打开/首DWORD写入失败后返回，不是manager构造失败。
// 后续archive字段失败抛int1。与严格的内存SerializeFileInfoCache工具入口分开。
// 正式入口沿原map节点前进，每次写完再取Next及当前head，不复制节点或Ref。
[[nodiscard]] bool WriteRuntimeFileInfoCacheAt489060(
    const char* path, const ArchiveNamespaceMapRuntime& archives,
    std::uint32_t totalCount, const FileTransactionContext* transaction, IStartFileApi& files);
// 独立差分夹具入口；生产不得提前生成/排序指针视图。
[[nodiscard]] bool WriteRuntimeFileInfoCacheAt489060(
    const char* path, std::span<const ArchiverInfoRuntime* const> sortedArchives,
    std::uint32_t totalCount, const FileTransactionContext* transaction, IStartFileApi& files);
// 工具兼容入口：值模型不再用于主线写缓存。
[[nodiscard]] bool WriteRuntimeFileInfoCacheAt489060(
    const char* path, const FileInfoCache& cache,
    const FileTransactionContext* transaction, IStartFileApi& files);
}
