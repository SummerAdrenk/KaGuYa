#pragma once

#include <Runtime/ResourceCatalog.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IFileInfoProgress;
class ArchiveDirectoryRuntime;
enum class FileInfoCacheError
{
    Truncated,
    TreeSizeOutOfRange,
    TreeNodeOutOfRange,
    FileCountMismatch,
    NameTooLong,
    TreeSizeOverflow,
    ChildOrderMismatch,
};

class FileInfoCacheException final : public std::runtime_error
{
public:
    FileInfoCacheException(
        FileInfoCacheError error,
        std::uint64_t offset,
        const char* message);

    [[nodiscard]] FileInfoCacheError Error() const noexcept;
    [[nodiscard]] std::uint64_t Offset() const noexcept;

private:
    FileInfoCacheError error_;
    std::uint64_t offset_ = 0;
};

// 工具值模型：仅供离线校验、转换和测试，不承载 native archiver 的目录所有权。
struct FileInfoCacheArchive final
{
    std::string archiveFileName;
    std::int64_t lastWriteTime = 0;
    std::uint64_t archiveFileSize = 0;
    std::uint32_t archiveVersion = 0;
    std::uint32_t fileNodeCount = 0;
    ResourceFolderEntry root;
};

struct FileInfoCache final
{
    std::uint32_t totalFileCount = 0;
    std::vector<FileInfoCacheArchive> archives;
};

struct RuntimeFileInfoCacheTree final
{
    ResourceFolderEntry root;
    std::uint32_t fileCount = 0;
    std::uint32_t maximumDataLength = 0;
};

// 工具入口：489060/488250/48F160 与 48F300/48F410 的无填充小端格式。
[[nodiscard]] FileInfoCache ParseFileInfoCache(std::span<const std::byte> bytes);
[[nodiscard]] ResourceFolderEntry ParseFileInfoCacheTree(
    std::span<const std::byte> bytes, std::uint32_t fileNodeCount);
// 494650 的 native 入口：本层复用一个 Ref，直接追加原目录，不经工具值树中转。
// progress 的每次重取由调用方 adapter 负责；异常不回滚已发布的节点及 count/max。
// 仍沿用逐字段 safe bounds：截断输入抛 FileInfoCacheException，不模拟原越界内存读取。
void ParseRuntimeFileInfoCacheTreeInto(
    std::span<const std::byte> bytes, std::uint32_t iterationCount,
    ArchiveDirectoryRuntime& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
    IFileInfoProgress& progress);
// 以下值树重载仅作工具用途：追加到现有 root，累加 +30/+34，不归零或回滚。
// 三个输出须引用独立且存活的 owner 状态；回调可以观察，但不得重入修改这些输出。
// 未完成目录尚未插入父容器，其子文件产生的 count/max 仍保留；异常原样传播。
void ParseRuntimeFileInfoCacheTreeInto(
    std::span<const std::byte> bytes, std::uint32_t iterationCount,
    ResourceFolderEntry& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
    IFileInfoProgress* progress = nullptr);
// 工具值树按 494650 的本层次数读，不要求耗尽树字节；不模拟 native 分配/Ref 时序。
[[nodiscard]] RuntimeFileInfoCacheTree ParseRuntimeFileInfoCacheTree(
    std::span<const std::byte> bytes, std::uint32_t iterationCount);
// 工具兼容重载；实际 FileManager 必须使用上面的 native 原位入口。
[[nodiscard]] RuntimeFileInfoCacheTree ParseRuntimeFileInfoCacheTree(
    std::span<const std::byte> bytes, std::uint32_t iterationCount, IFileInfoProgress& progress);
[[nodiscard]] std::vector<std::byte> SerializeFileInfoCache(const FileInfoCache& cache);
}
