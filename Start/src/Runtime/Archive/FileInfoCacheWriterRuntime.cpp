#include <Runtime/FileInfoCacheWriterRuntime.h>
#include <Runtime/ArchiveDirectoryRuntime.h>
#include <Runtime/ArchiverInfoRuntime.h>
#include <Runtime/ArchiveNamespaceMapRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/FileStreamRuntime.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NativeCacheOutput final : public IArchiveDirectoryWriterRuntime
{
public:
    NativeCacheOutput(FileStreamRuntime& stream, IStartFileApi& files) noexcept
        : stream_(stream), files_(files) {}
    NativeCacheOutput(const NativeCacheOutput&) = delete;
    NativeCacheOutput& operator=(const NativeCacheOutput&) = delete;

    std::int32_t Write(const void* bytes, std::uint32_t count) override
    {
        // 43A1A0：每次经真实流解析Ref/HANDLE，不预清actual，也不检查short write。
        return stream_.WriteWithApi(bytes, count, files_);
    }
    void Required(const void* bytes, DWORD count, int error)
    {
        if (!Write(bytes, count)) throw error;
    }
    template<class T> void Required(const T& value, int error)
    {
        Required(&value, sizeof(value), error);
    }
private:
    FileStreamRuntime& stream_;
    IStartFileApi& files_;
};

bool WriteArchiveHeaderAt48F160(NativeCacheOutput& output, const RuntimeCString& name,
    std::int64_t lastWriteTime, std::uint64_t fileSize, std::uint32_t version,
    std::uint32_t count, std::uint32_t treeSize)
{
    try
    {
        const auto nameLength = static_cast<std::uint8_t>(name.GetLength());
        output.Required(nameLength, 1);
        output.Required(name.GetString(), nameLength, 1);
        output.Required(lastWriteTime, 1);
        output.Required(fileSize, 1);
        output.Required(version, 1);
        output.Required(count, 1);
        output.Required(treeSize, 1);
        return true;
    }
    catch (...)
    {
        return false; // 48F24C/48F252：先返回0，由4883DA再抛int1展开name。
    }
}

bool WriteArchiveAt488250(NativeCacheOutput& output, const ArchiverInfoRuntime& archive)
{
    try
    {
        ArchiveRawResourceNameRuntime name(ArchiveRawResourceNameRuntime::ProducerTag{},
            [](RuntimeCString* value) { InitializeEmptyRuntimeCString(*value); });
        {
            const char* path = archive.PathAt10().GetString();
            ArchiveRawResourceNameRuntime fileName("");
            ScanArchiveFileNameAt45D950(fileName.Value(), path);
            const char* characters = fileName.Value().GetString();
            const int length = characters ? static_cast<int>(std::strlen(characters)) : 0;
            SetRuntimeCStringAt437680(name.Value(), characters, length);
            fileName.ReleaseNormal(); // 4882DA..4882F7：正常Free前撤scanner清理资格。
        }
        // 4882FA..488330：只暂存原头字段，绝不构造工具 FileInfoCache 或目录快照。
        // count 早于树大小虚调用；version 晚于该调用，不能合并为提前取值的结构体。
        const auto lastWriteTime = archive.LastWriteTimeAt20();
        const auto fileSize = archive.FileSizeAt28();
        const auto count = archive.CountAt30();
        const auto treeSize = TreeSizeAt491200(archive.DirectoryAt04());
        const auto version = archive.VersionAt18();
        if (!WriteArchiveHeaderAt48F160(output, name.Value(), lastWriteTime,
            fileSize, version, count, treeSize)) throw 1;
        SerializeChildren(output, archive.DirectoryAt04());
        name.ReleaseNormal(); // 488382..488398：正常Free可被本层catch，不走noexcept析构。
        return true;
    }
    catch (...)
    {
        return false; // 4883B4/4883BA：先结束本层catch，由489229抛int1。
    }
}

// 以下值树辅助函数仅服务工具兼容入口。
class ToolCacheOutput final : public IArchiveDirectoryWriterRuntime
{
public:
    ToolCacheOutput(IStartFileApi& files, HANDLE handle) noexcept : files_(files), handle_(handle) {}
    ~ToolCacheOutput() noexcept { files_.Close(handle_); }
    ToolCacheOutput(const ToolCacheOutput&) = delete;
    ToolCacheOutput& operator=(const ToolCacheOutput&) = delete;

    std::int32_t Write(const void* bytes, std::uint32_t count) override
    {
        DWORD actual = 0;
        return files_.Write(handle_, bytes, count, &actual) != FALSE || files_.LastError() == ERROR_SUCCESS;
    }
    void Required(const void* bytes, DWORD count, int error)
    {
        if (!Write(bytes, count)) throw error;
    }
    template<class T> void Required(const T& value, int error)
    {
        Required(&value, sizeof(value), error);
    }
private:
    IStartFileApi& files_;
    HANDLE handle_;
};

std::uint32_t TreeSizeAt491200(const ResourceFolderEntry& folder)
{
    std::uint32_t size = 0;
    for (const auto child : folder.childOrder)
    {
        if (child.kind == ResourceChildKind::File)
        {
            // 494C80/494C90按完整CString长度计算，不使用实际写出的截断WORD长度。
            size += static_cast<std::uint32_t>(folder.files.at(child.index).name.size()) + 17u;
        }
        else if (child.kind == ResourceChildKind::Folder)
        {
            const auto& nested = folder.folders.at(child.index);
            size += static_cast<std::uint32_t>(nested.name.size()) + 7u;
            size += TreeSizeAt491200(nested);
        }
        else throw std::logic_error("invalid runtime fileinfo child kind");
    }
    return size;
}

void WriteChildren(ToolCacheOutput& output, const ResourceFolderEntry& folder)
{
    for (const auto child : folder.childOrder)
    {
        if (child.kind == ResourceChildKind::File)
        {
            const auto& file = folder.files.at(child.index);
            const std::uint8_t type = 1;
            const auto length = static_cast<std::uint16_t>(file.name.size());
            // 48F300的六次独立vslot24调用及0..5异常值，不能合并为一次大块Write。
            output.Required(type, 0);
            output.Required(length, 1);
            output.Required(file.name.data(), length, 2);
            output.Required(file.offset, 3);
            output.Required(file.length, 4);
            output.Required(file.flags, 5);
        }
        else if (child.kind == ResourceChildKind::Folder)
        {
            const auto& nested = folder.folders.at(child.index);
            const std::uint8_t type = 0;
            const auto length = static_cast<std::uint16_t>(nested.name.size());
            const auto count = static_cast<std::uint32_t>(nested.childOrder.size());
            output.Required(type, 0);
            output.Required(length, 1);
            output.Required(nested.name.data(), length, 2);
            output.Required(count, 3);
            WriteChildren(output, nested);
        }
        else throw std::logic_error("invalid runtime fileinfo child kind");
    }
}

void WriteArchiveAt488250(ToolCacheOutput& output, const FileInfoCacheArchive& archive)
{
    try
    {
        const auto nameLength = static_cast<std::uint8_t>(archive.archiveFileName.size());
        const auto treeSize = TreeSizeAt491200(archive.root);
        // 48F160：BYTE名称长度，随后时间/大小/版本/文件数/树长度，保留每次写调用。
        output.Required(nameLength, 1);
        output.Required(archive.archiveFileName.data(), nameLength, 1);
        output.Required(archive.lastWriteTime, 1);
        output.Required(archive.archiveFileSize, 1);
        output.Required(archive.archiveVersion, 1);
        output.Required(archive.fileNodeCount, 1);
        output.Required(treeSize, 1);
        WriteChildren(output, archive.root);
    }
    catch (...)
    {
        // 4883B4/4883BA返回0，489229..489239随后抛int1；节点异常不穿过archive适配边界。
        throw 1;
    }
}
}

std::uint32_t TreeSizeAt491200(const ArchiveDirectoryRuntime& directory)
{
    std::uint32_t size = 0;
    for (auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
    {
        size += ResolveArchiveNode(*cursor)->SerializedSize();
        // 491245..491292：虚调用后重新解 Ref；不缓存 node，也不预先复制迭代范围。
        if (!ResolveArchiveNode(*cursor)->IsFile())
            size += TreeSizeAt491200(
                static_cast<ArchiveFolderNodeRuntime*>(ResolveArchiveNode(*cursor))->ChildrenAt08());
    }
    return size;
}

void SerializeChildren(IArchiveDirectoryWriterRuntime& writer, const ArchiveDirectoryRuntime& directory)
{
    for (auto* cursor = directory.beginAt00; cursor != directory.endAt04; ++cursor)
        ResolveArchiveNode(*cursor)->Write(writer);
}

bool WriteRuntimeFileInfoCacheAt489060(const char* path,
    const ArchiveNamespaceMapRuntime& archives, const std::uint32_t totalCount,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    if (archives.empty()) return false; // 489091的count检查先于输出打开。
    ScopedFileStreamRuntime stream(files);
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    const FileOpenRequest request{
        path, GENERIC_WRITE, 0u, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, &security};
    // 489183..489190：Open正常返回才登记EH清理，按base判断打开而非HANDLE或Open返回值。
    if (!stream.OpenReaderWithApi(request, transaction)) return false;
    NativeCacheOutput output(stream.Native(), files);
    if (!output.Write(&totalCount, sizeof(totalCount))) return false;
    for (auto* node = archives.Begin(); node != archives.End();
        node = ArchiveNamespaceMapRuntime::Next(node))
    {
        // 4891B0..4891D5：每轮从当前节点解析base/bias/stride，再调用正文写端。
        if (!WriteArchiveAt488250(output, *node->valueAt14.get())) throw 1;
    }
    return true;
}

bool WriteRuntimeFileInfoCacheAt489060(const char* path,
    const std::span<const ArchiverInfoRuntime* const> sortedArchives, const std::uint32_t totalCount,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    if (sortedArchives.empty()) return false;
    ScopedFileStreamRuntime stream(files);
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    const FileOpenRequest request{
        path, GENERIC_WRITE, 0u, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, &security};
    if (!stream.OpenReaderWithApi(request, transaction)) return false;
    NativeCacheOutput output(stream.Native(), files);
    if (!output.Write(&totalCount, sizeof(totalCount))) return false;
    // 4891B0..4891EC：按调用者提供的 namespace 顺序借用正文，不复制或重排树。
    for (const auto* archive : sortedArchives)
        if (!WriteArchiveAt488250(output, *archive)) throw 1;
    return true;
}

bool WriteRuntimeFileInfoCacheAt489060(const char* path, const FileInfoCache& cache,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    if (cache.archives.empty()) return false; // 489091：空目录不打开输出。
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    const FileOpenRequest request{
        path, GENERIC_WRITE, 0u, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, &security};
    const HANDLE handle = OpenFileUsingStartPolicy(request, transaction, files);
    if (handle == INVALID_HANDLE_VALUE) return false;
    ToolCacheOutput output(files, handle);
    if (!output.Write(&cache.totalFileCount, sizeof(cache.totalFileCount))) return false;
    for (const auto& archive : cache.archives) WriteArchiveAt488250(output, archive);
    return true;
}
}
