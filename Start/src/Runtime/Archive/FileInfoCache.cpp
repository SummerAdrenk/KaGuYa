#include <Runtime/FileInfoCache.h>
#include <Runtime/ArchiveDirectoryRuntime.h>
#include <Runtime/IFileInfoProgress.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ArchiveTitleScannerRuntime.h>

#include <mbstring.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Byte = unsigned char;

[[noreturn]] void ThrowCacheError(
    const FileInfoCacheError error,
    const std::uint64_t offset,
    const char* const message)
{
    throw FileInfoCacheException(error, offset, message);
}

class Reader final
{
public:
    explicit Reader(const std::span<const std::byte> bytes) noexcept : bytes_(bytes) {}

    [[nodiscard]] std::size_t Position() const noexcept { return position_; }
    [[nodiscard]] std::size_t Size() const noexcept { return bytes_.size(); }
    [[nodiscard]] bool AtEnd() const noexcept { return position_ == bytes_.size(); }

    [[nodiscard]] std::span<const std::byte> Read(const std::size_t size)
    {
        if (position_ > bytes_.size() || size > bytes_.size() - position_)
            ThrowCacheError(FileInfoCacheError::Truncated, position_, "finfo.ini is truncated");
        const auto result = bytes_.subspan(position_, size);
        position_ += size;
        return result;
    }

    [[nodiscard]] std::uint8_t ReadU8()
    {
        return std::to_integer<std::uint8_t>(Read(1)[0]);
    }

    [[nodiscard]] std::uint16_t ReadU16()
    {
        const auto bytes = Read(2);
        return static_cast<std::uint16_t>(
            std::to_integer<std::uint8_t>(bytes[0]) |
            (std::to_integer<std::uint8_t>(bytes[1]) << 8u));
    }

    [[nodiscard]] std::uint32_t ReadU32()
    {
        const auto bytes = Read(4);
        return std::to_integer<std::uint8_t>(bytes[0]) |
            (std::to_integer<std::uint8_t>(bytes[1]) << 8u) |
            (std::to_integer<std::uint8_t>(bytes[2]) << 16u) |
            (std::to_integer<std::uint8_t>(bytes[3]) << 24u);
    }

    [[nodiscard]] std::uint64_t ReadU64()
    {
        const std::uint64_t low = ReadU32();
        return low | (static_cast<std::uint64_t>(ReadU32()) << 32u);
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t position_ = 0;
};

[[nodiscard]] std::string CacheString(const std::span<const std::byte> bytes)
{
    const auto terminator = std::find(bytes.begin(), bytes.end(), std::byte{0});
    return std::string(
        reinterpret_cast<const char*>(bytes.data()),
        static_cast<std::size_t>(terminator - bytes.begin()));
}

[[nodiscard]] std::string ExtractTitle(const std::string& name, const bool runtime)
{
    const Byte* bytes = reinterpret_cast<const Byte*>(name.c_str());
    // 49495D -> 40C130：先取非空basename，再去扩展名；末尾点返回空title。
    if (runtime)
    {
        const Byte* const slash = _mbsrchr(bytes, '\\');
        if (slash != nullptr && slash[1] != 0) bytes = slash + 1;
    }
    const Byte* const dot = _mbsrchr(bytes, '.');
    if (dot == nullptr || dot == bytes || (runtime && dot[1] == 0))
        return {};
    return std::string(reinterpret_cast<const char*>(bytes), static_cast<std::size_t>(dot - bytes));
}

void ParseNativeCacheLevel(
    Reader reader, const std::uint32_t iterationCount, ArchiveDirectoryRuntime& parent,
    std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34, IFileInfoProgress& progress)
{
    // 49467D..494690：同层所有循环共用一个局部 Ref，成功 append 后仍保留局部引用。
    YtlReferenceSlotRuntime localRef;
    YtlReferenceControlRuntime* capturedControl = nullptr;
    try
    {
        for (std::uint32_t index = 0; index < iterationCount; ++index)
        {
            if (progress.PollCancelAt47F0B0()) throw 4;
            const auto type = reader.ReadU8();
            if (type != 0)
            {
                auto* body = CreateArchiveFileBody();
                ReplaceArchiveFileReference(localRef, body, capturedControl);
            }
            else
            {
                auto* body = CreateArchiveFolderBody();
                ReplaceArchiveFolderReference(localRef, body, capturedControl);
            }
            capturedControl = localRef.controlAt08;

            // 49483F..494911：先扩容并发布长度，再复制原始字节，最后按 NUL 收缩长度。
            // 沿用 Reader 的显式截断输入边界；不把坏输入补零或转换成空节点。
            const auto nameLength = reader.ReadU16();
            auto* buffer=GetRuntimeCStringBufferSetLengthAt417260(ResolveArchiveNode(localRef)->NameAt04(),nameLength);
            const auto nameBytes = reader.Read(nameLength);
            std::memmove(buffer, nameBytes.data(), nameLength);
            ReleaseRuntimeCStringBufferAt486B10(ResolveArchiveNode(localRef)->NameAt04());

            if (ResolveArchiveNode(localRef)->IsFile())
            {
                {
                    const char* name = ResolveArchiveNode(localRef)->NameAt04().GetString();
                    ArchiveRawResourceNameRuntime title("");
                    ScanArchiveTitleAt40C130(title.Value(),name);
                    const char* characters=title.Value().GetString();
                    auto& destination=static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->TitleAt08();
                    SetRuntimeCStringAt437680(destination,characters,characters?static_cast<int>(std::strlen(characters)):0);
                    title.ReleaseNormal();
                }
                static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->offsetAt10 = reader.ReadU64();
                static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->lengthAt18 = reader.ReadU32();
                static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->flagsAt1C = reader.ReadU16();
                ++fileCountAt30;
                if (static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->lengthAt18 > maximumAt34)
                    maximumAt34 = static_cast<ArchiveFileNodeRuntime*>(ResolveArchiveNode(localRef))->lengthAt18;
            }
            else
            {
                const auto childCount = reader.ReadU32();
                // 494ABA..494AE9：子层拿游标副本；返回后父层仍停在 childCount 后。
                ParseNativeCacheLevel(reader, childCount,
                    static_cast<ArchiveFolderNodeRuntime*>(ResolveArchiveNode(localRef))->ChildrenAt08(),
                    fileCountAt30, maximumAt34, progress);
            }
            const bool grows = parent.endAt04 == parent.capacityAt08;
            parent.Append(localRef);
            // 494B1D：仅增长分支从局部槽重取 esi；之后跨 Step/下一轮 Poll 与 body 构造保留。
            if (grows) capturedControl = localRef.controlAt08;
            // 每次调用均由外部 adapter 重解 archiver+5C；不能在此缓存 progress 正文。
            progress.StepAt46E820();
        }
    }
    catch (...)
    {
        ReleaseArchiveReferenceOnUnwind(localRef);
        throw;
    }
    ReleaseArchiveLocalReference(capturedControl);
}

void ParseNode(
    Reader& reader,
    ResourceFolderEntry& parent,
    std::uint32_t& parsedFileCount,
    std::uint32_t& maximumDataLength,
    const bool runtimeCursor = false,
    IFileInfoProgress* const progress = nullptr)
{
    // 4946A0..4946D2：取消检查早于节点首byte和边界检查，异常不转为空树。
    if (progress != nullptr && progress->PollCancelAt47F0B0()) throw 4;
    const std::uint8_t type = reader.ReadU8();
    const std::uint16_t nameLength = reader.ReadU16();
    std::string name = CacheString(reader.Read(nameLength));

    // 494650 仅将零视为目录，其他 type 值全部按文件节点处理。
    if (type != 0u)
    {
        ResourceFileEntry file;
        file.name = std::move(name);
        file.title = ExtractTitle(file.name, runtimeCursor);
        file.offset = reader.ReadU64();
        file.length = reader.ReadU32();
        file.flags = reader.ReadU16();
        if (!runtimeCursor && parsedFileCount == std::numeric_limits<std::uint32_t>::max())
        {
            ThrowCacheError(
                FileInfoCacheError::FileCountMismatch,
                reader.Position(),
                "finfo.ini file count overflows uint32");
        }
        // 494A54/494A74..494AA9：DWORD 回绕计数及无符号最大长度先于父容器插入。
        ++parsedFileCount;
        maximumDataLength = (std::max)(maximumDataLength, file.length);
        const std::size_t index = parent.files.size();
        parent.files.push_back(std::move(file));
        try
        {
            parent.childOrder.push_back({ResourceChildKind::File, index});
        }
        catch (...)
        {
            // 两个现代容器合为一次节点提交；仅撤销尚未完成的节点，不撤销 count/max。
            parent.files.pop_back();
            throw;
        }
        if (progress != nullptr) progress->StepAt46E820();
        return;
    }

    ResourceFolderEntry folder;
    folder.name = std::move(name);
    const std::uint32_t childCount = reader.ReadU32();
    // 494ABA 保存父层位置；494ADE 以值传递子层偏移，递归结束不更新父游标。
    // 工具的结构化解析仍消费完整子树，不能把它的规则代入原游戏运行时。
    Reader childReader = reader;
    for (std::uint32_t index = 0; index < childCount; ++index)
        ParseNode(runtimeCursor ? childReader : reader, folder, parsedFileCount,
            maximumDataLength, runtimeCursor, progress);
    // 494AE9 返回后才进入 494AEE 的父容器插入；子树失败时局部目录不发布。
    const std::size_t index = parent.folders.size();
    parent.folders.push_back(std::move(folder));
    try
    {
        parent.childOrder.push_back({ResourceChildKind::Folder, index});
    }
    catch (...)
    {
        parent.folders.pop_back();
        throw;
    }
    // 494B23..494B46：目录在子树完成并加入父容器后同样步进一次。
    if (progress != nullptr) progress->StepAt46E820();
}

void AppendU8(std::vector<std::byte>& output, const std::uint8_t value)
{
    output.push_back(static_cast<std::byte>(value));
}

void AppendU16(std::vector<std::byte>& output, const std::uint16_t value)
{
    AppendU8(output, static_cast<std::uint8_t>(value));
    AppendU8(output, static_cast<std::uint8_t>(value >> 8u));
}

void AppendU32(std::vector<std::byte>& output, const std::uint32_t value)
{
    AppendU16(output, static_cast<std::uint16_t>(value));
    AppendU16(output, static_cast<std::uint16_t>(value >> 16u));
}

void AppendU64(std::vector<std::byte>& output, const std::uint64_t value)
{
    AppendU32(output, static_cast<std::uint32_t>(value));
    AppendU32(output, static_cast<std::uint32_t>(value >> 32u));
}

void AppendString(std::vector<std::byte>& output, const std::string& value)
{
    const auto* const first = reinterpret_cast<const std::byte*>(value.data());
    output.insert(output.end(), first, first + value.size());
}

void SerializeNode(
    const ResourceFolderEntry& owner,
    const ResourceChildOrderEntry child,
    std::vector<std::byte>& output,
    std::uint32_t& serializedFileCount)
{
    if (child.kind == ResourceChildKind::File)
    {
        if (child.index >= owner.files.size())
        {
            ThrowCacheError(
                FileInfoCacheError::ChildOrderMismatch,
                output.size(),
                "finfo.ini file child order index is invalid");
        }
        const ResourceFileEntry& file = owner.files[child.index];
        if (file.name.size() > std::numeric_limits<std::uint16_t>::max())
        {
            ThrowCacheError(
                FileInfoCacheError::NameTooLong,
                output.size(),
                "finfo.ini node name exceeds uint16");
        }
        AppendU8(output, 1u);
        AppendU16(output, static_cast<std::uint16_t>(file.name.size()));
        AppendString(output, file.name);
        AppendU64(output, file.offset);
        AppendU32(output, file.length);
        AppendU16(output, file.flags);
        if (serializedFileCount == std::numeric_limits<std::uint32_t>::max())
        {
            ThrowCacheError(
                FileInfoCacheError::FileCountMismatch,
                output.size(),
                "finfo.ini file count overflows uint32");
        }
        ++serializedFileCount;
        return;
    }

    if (child.index >= owner.folders.size())
    {
        ThrowCacheError(
            FileInfoCacheError::ChildOrderMismatch,
            output.size(),
            "finfo.ini folder child order index is invalid");
    }
    const ResourceFolderEntry& folder = owner.folders[child.index];
    if (folder.name.size() > std::numeric_limits<std::uint16_t>::max() ||
        folder.childOrder.size() > std::numeric_limits<std::uint32_t>::max())
    {
        ThrowCacheError(
            FileInfoCacheError::NameTooLong,
            output.size(),
            "finfo.ini folder metadata exceeds field width");
    }
    if (folder.childOrder.size() != folder.folders.size() + folder.files.size())
    {
        ThrowCacheError(
            FileInfoCacheError::ChildOrderMismatch,
            output.size(),
            "finfo.ini folder child order is incomplete");
    }
    AppendU8(output, 0u);
    AppendU16(output, static_cast<std::uint16_t>(folder.name.size()));
    AppendString(output, folder.name);
    AppendU32(output, static_cast<std::uint32_t>(folder.childOrder.size()));
    for (const ResourceChildOrderEntry nested : folder.childOrder)
        SerializeNode(folder, nested, output, serializedFileCount);
}
}

FileInfoCacheException::FileInfoCacheException(
    const FileInfoCacheError error,
    const std::uint64_t offset,
    const char* const message)
    : std::runtime_error(message), error_(error), offset_(offset)
{
}

FileInfoCacheError FileInfoCacheException::Error() const noexcept
{
    return error_;
}

std::uint64_t FileInfoCacheException::Offset() const noexcept
{
    return offset_;
}

ResourceFolderEntry ParseFileInfoCacheTree(
    const std::span<const std::byte> bytes, const std::uint32_t fileNodeCount)
{
    Reader reader(bytes);
    ResourceFolderEntry root;
    std::uint32_t parsedFileCount = 0;
    std::uint32_t maximumDataLength = 0;
    while (!reader.AtEnd())
        ParseNode(reader, root, parsedFileCount, maximumDataLength);
    if (parsedFileCount != fileNodeCount)
        ThrowCacheError(FileInfoCacheError::FileCountMismatch, 0,
            "finfo.ini tree file count differs from archive header");
    return root;
}

void ParseRuntimeFileInfoCacheTreeInto(
    const std::span<const std::byte> bytes, const std::uint32_t iterationCount,
    ArchiveDirectoryRuntime& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
    IFileInfoProgress& progress)
{
    ParseNativeCacheLevel(Reader(bytes), iterationCount, root, fileCountAt30, maximumAt34, progress);
}

void ParseRuntimeFileInfoCacheTreeInto(
    const std::span<const std::byte> bytes, const std::uint32_t iterationCount,
    ResourceFolderEntry& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
    IFileInfoProgress* const progress)
{
    Reader reader(bytes);
    // 494690/494B49..494B53：循环次数来自 header，真实文件数在文件分支单独累计。
    for (std::uint32_t index = 0; index < iterationCount; ++index)
        ParseNode(reader, root, fileCountAt30, maximumAt34, true, progress);
}

RuntimeFileInfoCacheTree ParseRuntimeFileInfoCacheTree(
    const std::span<const std::byte> bytes, const std::uint32_t iterationCount)
{
    RuntimeFileInfoCacheTree result;
    ParseRuntimeFileInfoCacheTreeInto(
        bytes, iterationCount, result.root, result.fileCount, result.maximumDataLength);
    return result;
}

RuntimeFileInfoCacheTree ParseRuntimeFileInfoCacheTree(
    const std::span<const std::byte> bytes, const std::uint32_t iterationCount,
    IFileInfoProgress& progress)
{
    RuntimeFileInfoCacheTree result;
    ParseRuntimeFileInfoCacheTreeInto(
        bytes, iterationCount, result.root, result.fileCount, result.maximumDataLength, &progress);
    return result;
}

FileInfoCache ParseFileInfoCache(const std::span<const std::byte> bytes)
{
    Reader reader(bytes);
    FileInfoCache result;
    result.totalFileCount = reader.ReadU32();

    while (!reader.AtEnd())
    {
        FileInfoCacheArchive archive;
        const std::uint8_t archiveNameLength = reader.ReadU8();
        archive.archiveFileName = CacheString(reader.Read(archiveNameLength));
        archive.lastWriteTime = static_cast<std::int64_t>(reader.ReadU64());
        archive.archiveFileSize = reader.ReadU64();
        archive.archiveVersion = reader.ReadU32();
        archive.fileNodeCount = reader.ReadU32();
        const std::uint32_t treeSize = reader.ReadU32();
        const std::size_t treeOffset = reader.Position();
        const auto treeBytes = reader.Read(treeSize);

        try
        {
            archive.root = ParseFileInfoCacheTree(treeBytes, archive.fileNodeCount);
        }
        catch (const FileInfoCacheException& error)
        {
            throw FileInfoCacheException(error.Error(), treeOffset + error.Offset(), error.what());
        }
        result.archives.push_back(std::move(archive));
    }

    return result;
}

std::vector<std::byte> SerializeFileInfoCache(const FileInfoCache& cache)
{
    std::vector<std::byte> output;
    AppendU32(output, cache.totalFileCount);
    for (const FileInfoCacheArchive& archive : cache.archives)
    {
        if (archive.archiveFileName.size() > std::numeric_limits<std::uint8_t>::max())
        {
            // 原代码会截断长度并写坏后续布局；安全重写明确拒绝产生不可读缓存。
            ThrowCacheError(
                FileInfoCacheError::NameTooLong,
                output.size(),
                "finfo.ini archive file name exceeds uint8");
        }
        if (archive.root.childOrder.size() !=
            archive.root.folders.size() + archive.root.files.size())
        {
            ThrowCacheError(
                FileInfoCacheError::ChildOrderMismatch,
                output.size(),
                "finfo.ini root child order is incomplete");
        }

        std::vector<std::byte> tree;
        std::uint32_t serializedFileCount = 0;
        for (const ResourceChildOrderEntry child : archive.root.childOrder)
            SerializeNode(archive.root, child, tree, serializedFileCount);
        if (serializedFileCount != archive.fileNodeCount)
        {
            ThrowCacheError(
                FileInfoCacheError::FileCountMismatch,
                output.size(),
                "finfo.ini serialized file count differs from archive header");
        }
        if (tree.size() > std::numeric_limits<std::uint32_t>::max())
        {
            ThrowCacheError(
                FileInfoCacheError::TreeSizeOverflow,
                output.size(),
                "finfo.ini serialized tree exceeds uint32");
        }

        AppendU8(output, static_cast<std::uint8_t>(archive.archiveFileName.size()));
        AppendString(output, archive.archiveFileName);
        AppendU64(output, static_cast<std::uint64_t>(archive.lastWriteTime));
        AppendU64(output, archive.archiveFileSize);
        AppendU32(output, archive.archiveVersion);
        AppendU32(output, archive.fileNodeCount);
        AppendU32(output, static_cast<std::uint32_t>(tree.size()));
        output.insert(output.end(), tree.begin(), tree.end());
    }
    return output;
}
}
