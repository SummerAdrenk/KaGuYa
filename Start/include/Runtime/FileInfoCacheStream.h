#pragma once

#include <Runtime/FileInfoCache.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/FileInfoCacheNativeHeader.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
struct FileInfoCacheRecord final
{
    FileInfoCacheArchive archive;
    std::uint32_t treeSize = 0;
    std::uint64_t treeOffset = 0;
};

// 458B60 持有同一个 finfo 流；47FA10 每次查询重新从偏移 4 扫描。
class FileInfoCacheStream final
{
public:
    FileInfoCacheStream(const char* path,
        const FileTransactionContext* transactionContext, IStartFileApi& api);
    ~FileInfoCacheStream();
    FileInfoCacheStream(const FileInfoCacheStream&) = delete;
    FileInfoCacheStream& operator=(const FileInfoCacheStream&) = delete;

    [[nodiscard]] std::uint32_t InitialProgressTotal() const noexcept;
    // 正式47FA10不构造值record，不缓存文件位置，匹配后就地读取紧随其后的tree。
    bool FindNativeAt47FA10(FileInfoCacheNativeHeader&,const RuntimeCString& archivePath);
    void ReadNativeTreeAt47FCC4(const FileInfoCacheNativeHeader&,ArchiveDirectoryRuntime&,
        std::uint32_t& fileCount,std::uint32_t& maximum,std::uint32_t& loaded,IFileInfoProgress&);
    // 以下Find/record入口为显式工具和旧测试适配，不供正式ArchiverInfo查询。
    [[nodiscard]] std::optional<FileInfoCacheRecord> Find(const std::string& fileName);
    // 47FCFE..47FD30：整段 ReadExact 成功后才归零 +34/+30，root 不清空。
    // 读取失败不改三个输出；解析失败保留已提交节点及数值副作用，不吞 progress 异常。
    // 旧record适配保留有界/可重新定位协议；正式链调用上方无影子offset的native入口。
    void ReadTreeInto(const FileInfoCacheRecord& record,
        ArchiveDirectoryRuntime& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
        IFileInfoProgress& progress);
    // 以下值树入口仅作工具用途，不供 native archiver 装载树。
    void ReadTreeInto(const FileInfoCacheRecord& record,
        ResourceFolderEntry& root, std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34,
        IFileInfoProgress* progress = nullptr);
    [[nodiscard]] FileInfoCacheArchive ReadTree(FileInfoCacheRecord record);
    [[nodiscard]] FileInfoCacheArchive ReadTree(FileInfoCacheRecord record, IFileInfoProgress& progress);
    void Close();

private:
    [[nodiscard]] FileInfoCacheArchive ReadTreeCore(FileInfoCacheRecord record, IFileInfoProgress* progress);
    [[nodiscard]] bool ReadExact(void* output, DWORD size);
    [[nodiscard]] bool Seek(std::uint64_t position);

    IStartFileApi& api_;
    ScopedFileStreamRuntime stream_;
    std::uint64_t position_ = 0;
    bool positionKnown_=false; // 仅工具record的游标，正式Find/ReadTree不使用。
    std::uint32_t initialProgressTotal_ = 100;
};
}
