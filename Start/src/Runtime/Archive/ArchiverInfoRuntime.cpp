#include <Runtime/ArchiverInfoRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/FileStatusRuntime.h>
#include <Runtime/FileInfoProgressReference.h>

#include <mbstring.h>

#include <ctime>
#include <memory>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
ResourceFolderEntry SnapshotDirectory(const ArchiveDirectoryRuntime& source)
{
    ResourceFolderEntry result;
    for (const auto& reference : source.References())
    {
        const auto* node = ResolveArchiveNode(reference);
        if (!node) throw std::logic_error("fileinfo snapshot has no node payload");
        if (node->IsFile())
        {
            const auto& file = static_cast<const ArchiveFileNodeRuntime&>(*node);
            result.childOrder.push_back({ResourceChildKind::File, result.files.size()});
            result.files.push_back({std::string(RuntimeStringView(file.NameAt04())),
                std::string(RuntimeStringView(file.TitleAt08())), file.offsetAt10, file.lengthAt18, file.flagsAt1C});
        }
        else
        {
            const auto& folder = static_cast<const ArchiveFolderNodeRuntime&>(*node);
            auto children = SnapshotDirectory(folder.ChildrenAt08());
            children.name.assign(RuntimeStringView(folder.NameAt04()));
            result.childOrder.push_back({ResourceChildKind::Folder, result.folders.size()});
            result.folders.push_back(std::move(children));
        }
    }
    return result;
}
}

const ArchiveQueryState& ArchiverInfoRuntime::PublishQueryAt38(const ArchiveFileNodeRuntime& file) noexcept
{
    // 416FF2..417084 / 43D186..43D212：成功才逐字段覆盖；miss/不可用不清此结果区。
    queryAt38_.archivePath = pathAt10_.GetString();
    queryAt38_.archiveVersion = versionAt18_;
    queryAt38_.offset = file.offsetAt10;
    queryAt38_.length = file.lengthAt18;
    queryAt38_.flags = file.flagsAt1C;
    return queryAt38_;
}

ArchiverInfoRuntime::ArchiverInfoRuntime(std::string path, std::string media)
    : ArchiverInfoRuntime(InstalledNamesAt45905C{}, path.c_str(), media.c_str()) {}

ArchiverInfoRuntime::ArchiverInfoRuntime(InstalledNamesAt45905C,
    const char* path, const char* media, const char* title, const YtlReferenceSlotRuntime& progress,
    IRuntimeStringManagerSource& strings)
{
    struct PrefixGuard final
    {
        RuntimeCString* title;
        ArchiveDirectoryRuntime* directory;
        RuntimeCString* path;
        unsigned complete = 0;
        ~PrefixGuard() noexcept
        {
            // 596457/59644C/596444：前缀在展开期间逆序释放，不能延迟到catch内。
            if (complete >= 3) DestroyRuntimeCStringAt40CD40(path);
            if (complete >= 2) std::destroy_at(directory);
            if (complete >= 1) DestroyRuntimeCStringAt40CD40(title);
        }
    } prefix{&titleAt00_, &directoryAt04_, &pathAt10_};
    ConstructArchiveResourceNameAt40B530(&titleAt00_, title, strings);
    prefix.complete = 1;
    std::construct_at(&directoryAt04_);
    prefix.complete = 2;
    ConstructArchiveResourceNameAt40B530(&pathAt10_, path, strings);
    prefix.complete = 3;
    ConstructArchiveResourceNameAt40B530(&mediaAt14_, media, strings);
    prefix.complete = 0;
    versionAt18_ = 0xFFFFFFFFu;
    countAt30_ = 0;
    maximumAt34_ = 0;
    loadedAt58_ = 0;
    std::construct_at(&progressAt5C_, progress);
    // 4590CB：完整拷贝三槽后才增持；1C、时间/大小和查询区不在构造写集中。
    if (progressAt5C_.controlAt08) ++progressAt5C_.controlAt08->referenceCountAt04;
}

ArchiverInfoRuntime::~ArchiverInfoRuntime() noexcept
{
    // 462A80/EHFlags5：手工union成员按原逆序释放，异常终止。
    ReleaseYtlReference(progressAt5C_);
    DestroyRuntimeCStringAt40CD40(&mediaAt14_);
    DestroyRuntimeCStringAt40CD40(&pathAt10_);
    std::destroy_at(&directoryAt04_);
    DestroyRuntimeCStringAt40CD40(&titleAt00_);
}

void ArchiverInfoRuntime::ReleaseProgressAt4591EF()
{
    ReleaseYtlReference(progressAt5C_);
}

LinkArchiveIndex ArchiverInfoRuntime::Index() const
{
    LinkArchiveIndex result;
    result.archive.archivePath.assign(RuntimeStringView(pathAt10_));
    result.archive.archiveVersion = versionAt18_;
    result.archive.root = SnapshotDirectory(directoryAt04_);
    result.fileCount = countAt30_;
    result.maximumDataLength = maximumAt34_;
    return result;
}

int ArchiverInfoRuntime::IsAvailableAt43B2B0(IStartFileApi& files) const
{
    // 43B2B5/43B2E4：入口与错误15h后均读取当前loaded；不泵消息或限次重试。
    if (loadedAt58_ == 0) return 0;
    do
    {
        // 非INVALID值均成功，包括目录属性；失败不清loaded。
        if (files.GetAttributes(PathAt10().GetString()) != INVALID_FILE_ATTRIBUTES) return 1;
        if (files.LastError() != 0x15u) return 0;
    } while (loadedAt58_ != 0);
    return 0;
}

int ArchiverInfoRuntime::LoadInfoAt47FA10(FileInfoCacheStream& cache,
    const FileTransactionContext* transaction, IStartFileApi& files, IFileInfoProgress& progress)
{
    // 显式夹具入口：借用外部progress，不制造拥有引用；生产调用下面的三参数入口。
    ReleaseYtlReference(progressAt5C_);
    progressAt5C_ = {reinterpret_cast<std::uintptr_t>(&progress), 0, nullptr};
    return LoadInfoAt47FA10(cache, transaction, files);
}

int ArchiverInfoRuntime::LoadInfoAt47FA10(FileInfoCacheStream& cache,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    try
    {
        FileInfoProgressReferenceView progress(progressAt5C_);
        loadedAt58_ = 0;
        FileStatusAt47D160 disk{};
        if (!QueryFileStatusAt47D160(PathAt10().GetString(), files, disk)) throw 2;
        FileInfoCacheNativeHeader cached;
        if (cache.FindNativeAt47FA10(cached,PathAt10()))
        {
            // 47FB67/47FB6A早于时间/大小比较；失效缓存随后扫描失败仍保留此版本。
            versionAt18_=cached.Fields().versionAt18;
            CrtSaveLoadTimestampNativeSource times;
            if(MatchFileInfoMetadataAt47FB67(disk.lastWriteTime,disk.fileSize,cached.Fields(),times))
            {
                cache.ReadNativeTreeAt47FCC4(cached,directoryAt04_,countAt30_,maximumAt34_,loadedAt58_,progress);
            }
        }

        // 47FD80..47FDA5：缓存树失败不会到达这里；直扫open失败则已经刷新元数据。
        lastWriteTimeAt20_ = disk.lastWriteTime;
        fileSizeAt28_ = disk.fileSize;
        if (loadedAt58_ != 0)
        {
            cached.ReleaseNormal();
            return 1;
        }
        LoadRuntimeLinkArchiveDirectoryFileInto(PathAt10().GetString(), transaction, files,
            directoryAt04_, countAt30_, maximumAt34_, versionAt18_, loadedAt58_, progress);
        cached.ReleaseNormal();
        return 2;
    }
    catch (...)
    {
        // 5E8B58 ->47FF4E/47FF54，局部失败仅返回0，不清版本、目录或计数。
        return 0;
    }
}

struct ArchiverInfoRuntimeLayout final
{
    static_assert(offsetof(ArchiverInfoRuntime, titleAt00_) == 0);
    static_assert(offsetof(ArchiverInfoRuntime, directoryAt04_) == 4);
    static_assert(offsetof(ArchiverInfoRuntime, pathAt10_) == 0x10);
    static_assert(offsetof(ArchiverInfoRuntime, mediaAt14_) == 0x14);
    static_assert(offsetof(ArchiverInfoRuntime, versionAt18_) == 0x18);
    static_assert(offsetof(ArchiverInfoRuntime, lastWriteTimeAt20_) == 0x20);
    static_assert(offsetof(ArchiverInfoRuntime, fileSizeAt28_) == 0x28);
    static_assert(offsetof(ArchiverInfoRuntime, countAt30_) == 0x30);
    static_assert(offsetof(ArchiverInfoRuntime, maximumAt34_) == 0x34);
    static_assert(offsetof(ArchiverInfoRuntime, queryAt38_) == 0x38);
    static_assert(offsetof(ArchiverInfoRuntime, loadedAt58_) == 0x58);
    static_assert(offsetof(ArchiverInfoRuntime, progressAt5C_) == 0x5C);
};
}
