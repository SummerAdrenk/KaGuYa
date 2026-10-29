#include <Runtime/InstallTableCatalogBuilder.h>
#include <Runtime/ArchiverInfoRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/EnvironmentInstalledFiles.h>

#include <Runtime/FileInfoCache.h>
#include <Runtime/FileInfoCacheStream.h>
#include <Runtime/FileInfoCacheWriterRuntime.h>
#include <Runtime/LinkArchiveIndexParser.h>
#include <Runtime/IFileInfoProgress.h>
#include <Runtime/FileInfoProgressReference.h>

#include <mbstring.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <ctime>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
const EnvironmentInstalledFiles* InstallTableCatalogSource::CaptureReferences() const
{
    return source_ ? &source_->CaptureInstalledTableAt4180C0() : references_;
}

std::size_t InstallTableCatalogSource::size() const
{
    const auto* references = CaptureReferences();
    return references ? references->size() : values_.size();
}

const void* InstallTableCatalogSource::Source() const noexcept
{
    if (source_) return source_;
    return references_ ? static_cast<const void*>(references_) : values_.data();
}

InstallTableArchive InstallTableCatalogSource::ReadAt(std::size_t index) const
{
    // 工具/诊断快照；生产构造必须分别调用下面的两个原时点。
    if (source_)
    {
        auto file = CaptureFileNameAt458DD0(static_cast<std::uint32_t>(index));
        const auto* media = ReadMediaAt458FC7(static_cast<std::uint32_t>(index));
        InstallTableArchive result{std::string(RuntimeStringView(file.Value())), media ? media : ""};
        file.ReleaseNormal();
        return result;
    }
    if (!references_) return values_[index];
    // 458F20/45900B分别判定元素范围；偏移由control+0C提供，不按sizeof乘索引。
    if (index >= references_->size()) return {};
    const auto* entry = ResolveEnvironmentInstalledFile(references_->References()[index]);
    if (!entry) throw std::logic_error("458F44: installed file reference has no payload");
    std::string file(RuntimeStringView(entry->stringAt00));
    if (index >= references_->size()) return {std::move(file), {}};
    entry = ResolveEnvironmentInstalledFile(references_->References()[index]);
    if (!entry) throw std::logic_error("45903B: installed media reference has no payload");
    return {std::move(file), std::string(RuntimeStringView(entry->stringAt04))};
}

ArchiveRawResourceNameRuntime InstallTableCatalogSource::CaptureFileNameAt458DD0(std::uint32_t index) const
{
    return ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output)
    {
        const auto* references = CaptureReferences();
        if (references && index < references->size())
        {
            const auto* entry = ResolveEnvironmentInstalledFile(references->References()[index]);
            if (!entry) throw std::logic_error("458F44: installed file reference has no payload");
            auto* const header = reinterpret_cast<ATL::CStringData*>(
                reinterpret_cast<std::uintptr_t>(entry->stringAt00.GetString()) - sizeof(ATL::CStringData));
            const auto* const cloned = CloneRuntimeCStringDataAt414540(header);
            const char* const data = reinterpret_cast<const char*>(cloned + 1);
            std::memcpy(output, &data, sizeof(data));
        }
        else
        {
            // 458F5A..458F9E：范围外也先nil后空Set，不返回现代构造的临时串。
            ConstructArchiveResourceNameAt40B530(output,
                !references && index < values_.size() ? values_[index].file.c_str() : "");
        }
    });
}

const char* InstallTableCatalogSource::ReadMediaAt458FC7(std::uint32_t index) const
{
    const auto* references = CaptureReferences();
    if (!references) return index < values_.size() ? values_[index].media.c_str() : nullptr;
    if (index >= references->size()) return nullptr; // 459055是真正的null，不是空CString。
    const auto* entry = ResolveEnvironmentInstalledFile(references->References()[index]);
    if (!entry) throw std::logic_error("459021: installed media reference has no payload");
    return entry->stringAt04.GetString(); // 裸字符地址跨越正文其他字段的构造，不复制/加Ref。
}

ArchiverInfoReference CreateArchiverInfoAt458FAB(
    const RuntimeCString& fileName, InstallTableCatalogSource source, std::uint32_t index,
    const char* title, const YtlReferenceSlotRuntime& progress, IRuntimeStringManagerSource& strings)
{
    const char* const file = fileName.GetString(); // 原ESI在正文分配前捕获。
    // 458FAB分配先于media晚取；分离placement避免/sdl清零原本未写的元数据/查询区。
    void* storage = ::operator new(sizeof(ArchiverInfoRuntime));
    if (!storage) return {}; // 458FC1 ->4590D3：空正文分配不读取media。
    ArchiverInfoRuntime* body;
    try
    {
        body = ::new (storage) ArchiverInfoRuntime(ArchiverInfoRuntime::InstalledNamesAt45905C{},
            file, source.ReadMediaAt458FC7(index), title, progress, strings);
    }
    catch (...)
    {
        ::operator delete(storage); // 原正文构造清理态覆盖media读取和三个CString构造。
        throw;
    }
    // 4590DA先退出正文清理态；后续control分配抛出不会回收这个完整正文。
    return ArchiverInfoReference::AdoptAt4590DA(body);
}

namespace
{
using Byte = unsigned char;

[[nodiscard]] std::string ArchiveNamespace(const std::string& path)
{
    // CStringToTitle(40C130) 先取最后一个反斜线后的文件名，再取最后一个点之前的 title。
    const Byte* fileName = reinterpret_cast<const Byte*>(path.c_str());
    if (const Byte* const separator = _mbsrchr(fileName, '\\'))
    {
        if (separator[1] != 0)
            fileName = separator + 1;
    }

    const Byte* const dot = _mbsrchr(fileName, '.');
    if (dot == nullptr || dot[1] == 0)
        return {};
    return std::string(
        reinterpret_cast<const char*>(fileName),
        static_cast<std::size_t>(dot - fileName));
}

[[nodiscard]] std::string ArchiveFileName(const std::string& path)
{
    const Byte* fileName = reinterpret_cast<const Byte*>(path.c_str());
    if (const Byte* const separator = _mbsrchr(fileName, '\\'))
    {
        if (separator[1] != 0)
            fileName = separator + 1;
    }
    return reinterpret_cast<const char*>(fileName);
}

struct DiskArchiveMetadata final
{
    std::int64_t lastWriteTime = 0;
    std::uint64_t fileSize = 0;
};

[[nodiscard]] std::int64_t FileTimeToOriginalLocalTime(const FILETIME& source)
{
    FILETIME localFileTime{};
    SYSTEMTIME systemTime{};
    if (FileTimeToLocalFileTime(&source, &localFileTime) == FALSE ||
        FileTimeToSystemTime(&localFileTime, &systemTime) == FALSE)
    {
        throw std::runtime_error("archive FILETIME conversion failed");
    }
    if (systemTime.wYear < 1900u)
        return 0;

    std::tm value{};
    value.tm_sec = systemTime.wSecond;
    value.tm_min = systemTime.wMinute;
    value.tm_hour = systemTime.wHour;
    value.tm_mday = systemTime.wDay;
    value.tm_mon = systemTime.wMonth - 1;
    value.tm_year = systemTime.wYear - 1900;
    value.tm_isdst = -1;
    const __time64_t converted = _mktime64(&value);
    if (converted == -1)
        throw std::runtime_error("archive local timestamp conversion failed");
    return converted;
}

[[nodiscard]] DiskArchiveMetadata QueryDiskArchiveMetadata(
    const std::string& path,
    IStartFileApi& api)
{
    WIN32_FIND_DATAA data{};
    const HANDLE findHandle = api.FindFirst(path.c_str(), &data);
    if (findHandle == INVALID_HANDLE_VALUE)
        throw 1;
    if (api.FindCloseHandle(findHandle) == FALSE)
        throw 2;
    return {
        FileTimeToOriginalLocalTime(data.ftLastWriteTime),
        (static_cast<std::uint64_t>(data.nFileSizeHigh) << 32u) |
            data.nFileSizeLow};
}

[[nodiscard]] bool SameMinute(
    const std::int64_t first,
    const std::int64_t second)
{
    std::tm firstLocal{};
    std::tm secondLocal{};
    const __time64_t firstValue = first;
    const __time64_t secondValue = second;
    if (_localtime64_s(&firstLocal, &firstValue) != 0 ||
        _localtime64_s(&secondLocal, &secondValue) != 0)
    {
        throw std::runtime_error("archive cache timestamp conversion failed");
    }
    return firstLocal.tm_year == secondLocal.tm_year &&
        firstLocal.tm_mon == secondLocal.tm_mon &&
        firstLocal.tm_mday == secondLocal.tm_mday &&
        firstLocal.tm_hour == secondLocal.tm_hour &&
        firstLocal.tm_min == secondLocal.tm_min;
}

[[nodiscard]] std::uint32_t MaximumDataLength(const ResourceFolderEntry& folder)
{
    std::uint32_t maximum = 0;
    for (const ResourceFileEntry& file : folder.files)
        maximum = (std::max)(maximum, file.length);
    for (const ResourceFolderEntry& child : folder.folders)
        maximum = (std::max)(maximum, MaximumDataLength(child));
    return maximum;
}

class ScopedFile final
{
public:
    ScopedFile(IStartFileApi& api, const HANDLE file) noexcept : api_(api), file_(file) {}
    ~ScopedFile()
    {
        if (file_ != INVALID_HANDLE_VALUE)
            api_.Close(file_);
    }

    ScopedFile(const ScopedFile&) = delete;
    ScopedFile& operator=(const ScopedFile&) = delete;

private:
    IStartFileApi& api_;
    HANDLE file_ = INVALID_HANDLE_VALUE;
};

void WriteFileInfoCache(
    const char* const path,
    const FileInfoCache& cache,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    const std::vector<std::byte> bytes = SerializeFileInfoCache(cache);
    if (bytes.size() > std::numeric_limits<DWORD>::max())
        throw std::overflow_error("finfo.ini exceeds DWORD write size");

    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;
    const FileOpenRequest request{
        path,
        GENERIC_WRITE,
        0u,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        &securityAttributes};
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
        throw 1;
    const ScopedFile owner(api, file);

    DWORD bytesWritten = 0;
    if (api.Write(
            file,
            bytes.data(),
            static_cast<DWORD>(bytes.size()),
            &bytesWritten) == FALSE)
    {
        throw 1;
    }
}

struct CacheRecordWithNamespace final
{
    std::string nameSpace;
    FileInfoCacheArchive archive;
};

struct InstalledArchiveConstruction final
{
    InstalledArchiveConstruction(InstallTableCatalogSource source, std::uint32_t index,
        const char* title, const YtlReferenceSlotRuntime& progress)
        : fileName(source.CaptureFileNameAt458DD0(index)),
          cachedFileName(fileName.Value().GetString()),
          owner(CreateArchiverInfoAt458FAB(fileName.Value(), source, index, title, progress)),
          cachedControl(owner.reference.controlAt08) {}
    ~InstalledArchiveConstruction() noexcept
    {
        if (ownerActive) std::destroy_at(&owner);
    }
    void ReleaseNormalAt459309()
    {
        // 45930C/state17保护Ref释放；45933A退state6后，cached ESI的Free才可外传。
        ownerActive = false;
        ReleaseArchiveLocalReference(cachedControl); // 原EBX跨LoadInfo/插表，两个虚槽均不重读局部Ref。
        if (cachedControl)
        {
            owner.reference.controlAt08 = nullptr;
            owner.reference.baseAt00 = 0;
        }
        fileName.ReleaseNormalCached(cachedFileName);
    }
    // 文件名共享正文保持到本轮插表/临时引用释放结束，不在LoadInfo之前换成值快照。
    ArchiveRawResourceNameRuntime fileName;
    const char* cachedFileName;
    union { ArchiverInfoReference owner; };
    YtlReferenceControlRuntime* cachedControl;
    bool ownerActive = true;
};
}

InstallTableCatalogBuildResult BuildCatalogFromInstallTable(
    const std::span<const InstallTableArchive> installTable,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api)
{
    InstallTableCatalogBuildResult result;
    result.archives.reserve(installTable.size());

    for (const InstallTableArchive& installed : installTable)
    {
        std::string nameSpace = ArchiveNamespace(installed.file);
        LinkArchiveIndex index = ParseLinkArchiveIndexFile(
            installed.file.c_str(), nameSpace, transactionContext, api);

        if (index.fileCount >
            std::numeric_limits<std::uint32_t>::max() - result.totalFileCount)
        {
            throw std::overflow_error("InstallTable total file count exceeds uint32");
        }

        InstalledArchiveMetadata metadata;
        metadata.file = installed.file;
        metadata.media = installed.media;
        metadata.nameSpace = std::move(nameSpace);
        metadata.archiveVersion = index.header.version;
        metadata.fileCount = index.fileCount;
        metadata.maximumDataLength = index.maximumDataLength;

        // ResourceCatalog 保留 458B60 的区分大小写重复 namespace 拒绝语义。
        result.catalog.AddArchive(std::move(index.archive));
        result.totalFileCount += index.fileCount;
        result.maximumDataLength = (std::max)(
            result.maximumDataLength, index.maximumDataLength);
        result.archives.push_back(std::move(metadata));
    }

    return result;
}

static void BuildNativeCatalogWithCacheCore(
    ArchiveNamespaceMapRuntime& directory, std::uint32_t& scanStatus,
    const InstallTableCatalogSource installTable, const char* fileInfoCachePath,
    const FileTransactionContext* transactionContext, IStartFileApi& api,
    const char* capturedTitle, const YtlReferenceSlotRuntime& progressReference,
    InstallTableCatalogBuildResult* diagnostics)
{
    FileInfoCacheStream cache(fileInfoCachePath, transactionContext, api);
    FileInfoProgressReferenceView progress(progressReference);
    progress.AddRangeAt46A5A0(cache.InitialProgressTotal());
    // 458D83/458D86 是本次调用局部量；不由上次诊断计数决定是否写缓存。
    bool rewrite = false;
    std::uint32_t totalFileCount = 0;
    for (std::uint32_t ordinal = 0; ; ++ordinal)
    {
        const auto count = static_cast<std::uint32_t>(installTable.size());
        if (std::bit_cast<std::int32_t>(ordinal) >= std::bit_cast<std::int32_t>(count)) break;
        InstalledArchiveConstruction runtime(installTable, ordinal, capturedTitle, progressReference);
        auto& info = *runtime.owner.get();
        auto* const capturedControl = runtime.owner.reference.controlAt08;
        const int loaded = info.LoadInfoAt47FA10(cache, transactionContext, api);
        if (loaded == 0) throw 3;
        if (loaded == 2) rewrite = true;
        if (diagnostics)
        {
            if (loaded == 2) ++diagnostics->directScanCount;
            else ++diagnostics->cacheHitCount;
        }
        ArchiveInstalledNamespaceRuntime nameSpace(runtime.cachedFileName);
        const bool duplicate = directory.Find(nameSpace.Namespace()) != directory.End();
        info.ReleaseProgressAt4591EF();
        if (duplicate) throw 4;
        totalFileCount += info.CountAt30(); // 45923D，原DWORD自然溢出。
        if (diagnostics) diagnostics->totalFileCount += info.CountAt30();
        {
            const RuntimeCStringResult pairKey([&](RuntimeCString* output)
            {
                auto* const source = reinterpret_cast<ATL::CStringData*>(
                    reinterpret_cast<std::uintptr_t>(nameSpace.Namespace().GetString()) -
                    sizeof(ATL::CStringData));
                const auto* const data = CloneRuntimeCStringDataAt414540(source);
                const char* const characters = reinterpret_cast<const char*>(data + 1);
                std::memcpy(output, &characters, sizeof(characters));
            });
            ArchiverInfoReference pairValue;
            pairValue.reference = {reinterpret_cast<std::uintptr_t>(&info), 0, capturedControl};
            if (capturedControl) ++capturedControl->referenceCountAt04;
            (void)directory.Insert(pairKey.Value(), pairValue.reference);
        }

        // 只供显式工具收据；生产传nullptr，不引入快照分配和额外失败点。
        std::optional<InstalledArchiveMetadata> metadata;
        if (diagnostics)
        {
            metadata.emplace();
            metadata->file.assign(runtime.fileName.Value().GetString(), runtime.fileName.Value().GetLength());
            metadata->media.assign(RuntimeStringView(info.MediaAt14()));
            metadata->nameSpace.assign(RuntimeStringView(nameSpace.Namespace()));
        }
        nameSpace.ReleaseNormalAt4592EA();
        if (diagnostics)
        {
            metadata->archiveVersion = info.VersionAt18();
            metadata->fileCount = info.CountAt30();
            metadata->maximumDataLength = info.MaximumAt34();
            diagnostics->maximumDataLength = (std::max)(
                diagnostics->maximumDataLength, metadata->maximumDataLength);
            diagnostics->archives.push_back(std::move(*metadata));
        }
        runtime.ReleaseNormalAt459309();
    }
    cache.Close(); // 459374：关闭异常必须先于+0C写入。
    scanStatus = 1u;
    if (rewrite)
    {
        const bool written = WriteRuntimeFileInfoCacheAt489060(
            fileInfoCachePath, directory, totalFileCount, transactionContext, api);
        if (diagnostics) diagnostics->cacheRewritten = written;
    }
}

static void BuildCatalogFromInstallTableWithCacheCore(
    InstallTableCatalogBuildResult& result,
    const InstallTableCatalogSource installTable,
    const char* const fileInfoCachePath,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    IFileInfoProgress* progress)
{
    FileInfoCacheStream cache(fileInfoCachePath, transactionContext, api);
    if (progress != nullptr) progress->AddRangeAt46A5A0(cache.InitialProgressTotal());

    result.archives.reserve(installTable.size());
    std::vector<CacheRecordWithNamespace> rebuiltRecords;
    rebuiltRecords.reserve(installTable.size());

    // 显式值目录工具入口；原生装载由上方逐次取源的循环承担。
    for (std::size_t ordinal = 0; ordinal < installTable.size(); ++ordinal)
    {
        InstallTableArchive installed;
        std::string nameSpace;
        ResourceArchiveEntry archive;
        std::uint32_t fileCount = 0;
        std::uint32_t maximumDataLength = 0;
        FileInfoCacheArchive rebuilt;
        {
            installed = installTable.ReadAt(ordinal);
            // 工具入口保留原抛错接口，不将完整结果式工具偷偷改成生产catch/部分状态契约。
            const DiskArchiveMetadata disk = QueryDiskArchiveMetadata(installed.file, api);
            const std::string fileName = ArchiveFileName(installed.file);
            nameSpace = ArchiveNamespace(installed.file);
            auto cached = cache.Find(fileName);
            // 47FB7F/47FB8C 将缓存时间低 DWORD 零扩展；47FC67..77 只比较磁盘大小低位，
            // 同时要求缓存大小高位为零。保留此原始判定，不擅自改成完整 uint64 相等。
            if (cached &&
                SameMinute(disk.lastWriteTime,
                    static_cast<std::uint32_t>(cached->archive.lastWriteTime)) &&
                static_cast<std::uint32_t>(cached->archive.archiveFileSize) ==
                    static_cast<std::uint32_t>(disk.fileSize) &&
                (cached->archive.archiveFileSize >> 32u) == 0u)
            {
                rebuilt = progress != nullptr
                    ? cache.ReadTree(std::move(*cached), *progress) : cache.ReadTree(std::move(*cached));
                archive.nameSpace = nameSpace;
                archive.archivePath = installed.file;
                archive.archiveVersion = rebuilt.archiveVersion;
                archive.root = rebuilt.root;
                fileCount = rebuilt.fileNodeCount;
                maximumDataLength = MaximumDataLength(archive.root);
                ++result.cacheHitCount;
            }
            else
            {
                LinkArchiveIndex index = progress != nullptr
                    ? ParseLinkArchiveIndexFile(installed.file.c_str(), nameSpace, transactionContext, api, *progress)
                    : ParseLinkArchiveIndexFile(installed.file.c_str(), nameSpace, transactionContext, api);
                fileCount = index.fileCount;
                maximumDataLength = index.maximumDataLength;
                archive = std::move(index.archive);

                rebuilt.archiveFileName = fileName;
                rebuilt.lastWriteTime = disk.lastWriteTime;
                rebuilt.archiveFileSize = disk.fileSize;
                rebuilt.archiveVersion = archive.archiveVersion;
                rebuilt.fileNodeCount = fileCount;
                rebuilt.root = archive.root;
                ++result.directScanCount;
            }

            // 47FD80..47FDA5：命中也刷新磁盘元数据，混合命中/重扫后统一写回时不能保留旧秒值。
            rebuilt.archiveFileName = fileName;
            rebuilt.lastWriteTime = disk.lastWriteTime;
            rebuilt.archiveFileSize = disk.fileSize;
        }

        if (fileCount > std::numeric_limits<std::uint32_t>::max() - result.totalFileCount)
            throw std::overflow_error("InstallTable total file count exceeds uint32");

        InstalledArchiveMetadata metadata;
        metadata.file = installed.file;
        metadata.media = installed.media;
        metadata.nameSpace = nameSpace;
        metadata.archiveVersion = archive.archiveVersion;
        metadata.fileCount = fileCount;
        metadata.maximumDataLength = maximumDataLength;

        result.catalog.AddArchive(std::move(archive));
        result.totalFileCount += fileCount;
        result.maximumDataLength = (std::max)(result.maximumDataLength, maximumDataLength);
        result.archives.push_back(std::move(metadata));
        rebuiltRecords.push_back({std::move(nameSpace), std::move(rebuilt)});
    }

    // 45935F..4593A8：先释放读流，才允许 CREATE_ALWAYS 重写同一文件。
    // 459374经5AE408+00直接进入43C840，不是40ED70析构；关闭异常由manager构造catch保留状态0。
    cache.Close();
    if (result.directScanCount != 0u)
    {
        std::sort(
            rebuiltRecords.begin(),
            rebuiltRecords.end(),
            [](const CacheRecordWithNamespace& first, const CacheRecordWithNamespace& second)
            {
                return _mbscmp(
                    reinterpret_cast<const Byte*>(first.nameSpace.c_str()),
                    reinterpret_cast<const Byte*>(second.nameSpace.c_str())) < 0;
            });
        FileInfoCache outputCache;
        outputCache.totalFileCount = result.totalFileCount;
        outputCache.archives.reserve(rebuiltRecords.size());
        for (CacheRecordWithNamespace& record : rebuiltRecords)
            outputCache.archives.push_back(std::move(record.archive));
        WriteFileInfoCache(fileInfoCachePath, outputCache, transactionContext, api);
        result.cacheRewritten = true;
    }

}

InstallTableCatalogBuildResult BuildCatalogFromInstallTableWithCache(
    std::span<const InstallTableArchive> installTable, const char* path,
    const FileTransactionContext* transaction, IStartFileApi& api)
{
    InstallTableCatalogBuildResult result;
    BuildCatalogFromInstallTableWithCacheCore(result, installTable, path, transaction, api, nullptr);
    return result;
}

InstallTableCatalogBuildResult BuildCatalogFromInstallTableWithCache(
    std::span<const InstallTableArchive> installTable, const char* path,
    const FileTransactionContext* transaction, IStartFileApi& api, IFileInfoProgress& progress)
{
    InstallTableCatalogBuildResult result;
    BuildCatalogFromInstallTableWithCacheCore(result, installTable, path, transaction, api, &progress);
    return result;
}

void BuildCatalogFromInstallTableWithCacheInto(
    InstallTableCatalogBuildResult& destination, std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable, const char* path,
    const FileTransactionContext* transaction, IStartFileApi& api, IFileInfoProgress& progress)
{
    const YtlReferenceSlotRuntime borrowed{reinterpret_cast<std::uintptr_t>(&progress), 0, nullptr};
    BuildCatalogFromInstallTableWithCacheInto(destination, scanStatusAt0C, installTable,
        path, transaction, api, nullptr, borrowed);
}

void BuildCatalogFromInstallTableWithCacheInto(
    InstallTableCatalogBuildResult& destination, std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable, const char* path,
    const FileTransactionContext* transaction, IStartFileApi& api,
    const char* title, const YtlReferenceSlotRuntime& progress)
{
    // 独立审计入口也先构造同一map；FileManager生产入口已在catch外完成此步。
    destination.catalog.InitializeNativeMap(api);
    BuildNativeCatalogWithCacheCore(destination.catalog.NativeMap(), scanStatusAt0C,
        installTable, path, transaction, api, title, progress, &destination);
}

void BuildNativeCatalogFromInstallTableWithCacheInto(
    ArchiveNamespaceMapRuntime& directoryAt04, std::uint32_t& scanStatusAt0C,
    InstallTableCatalogSource installTable, const char* path,
    const FileTransactionContext* transaction, IStartFileApi& api,
    const char* title, const YtlReferenceSlotRuntime& progress)
{
    BuildNativeCatalogWithCacheCore(directoryAt04, scanStatusAt0C,
        installTable, path, transaction, api, title, progress, nullptr);
}
}
