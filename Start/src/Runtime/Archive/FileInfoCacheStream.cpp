#include <Runtime/FileInfoCacheStream.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>

#include <mbstring.h>

#include <algorithm>
#include <array>
#include <utility>

namespace KaGuYa::Start::Runtime
{
FileInfoCacheStream::FileInfoCacheStream(const char* const path,
    const FileTransactionContext* const transactionContext, IStartFileApi& api)
    : api_(api),stream_(api)
{
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    const FileOpenRequest request{
        path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, &security};
    if(stream_.OpenReaderWithApi(request,transactionContext))
    {
        // 458C8C/458D52：成功但未写入的特殊Read沿用Buffer=2，失败仍用100。
        std::uint32_t total=2;
        if(stream_.Native().ReadExactWithApi(&total,4,api_))initialProgressTotal_=total;
    }
}

FileInfoCacheStream::~FileInfoCacheStream()=default;

void FileInfoCacheStream::Close()
{
    // 45935F..45938B是明确的Ref释放，不先伪造关闭状态；40ED70由流owner另行执行。
    stream_.Native().CloseWithApi(api_);
    positionKnown_=false;
}

std::uint32_t FileInfoCacheStream::InitialProgressTotal() const noexcept
{
    return initialProgressTotal_;
}

bool FileInfoCacheStream::ReadExact(void* const output, const DWORD size)
{
    if(!stream_.Native().IsOpenAtVslot08())return false;
    DWORD read = 0;
    const BOOL result = api_.Read(stream_.Native().HandleAtVslot00(), output, size, &read);
    position_ += read;
    // 431418..43143F：FALSE 时只检查错误码；TRUE 才比较实际读取长度。
    return result != FALSE ? read == size : api_.LastError() == ERROR_SUCCESS;
}

bool FileInfoCacheStream::Seek(const std::uint64_t position)
{
    if(!stream_.Native().IsOpenAtVslot08()||api_.SeekAbsolute(stream_.Native().HandleAtVslot00(),position)==FALSE)
        return false;
    position_ = position;
    positionKnown_=true;
    return true;
}

std::optional<FileInfoCacheRecord> FileInfoCacheStream::Find(const std::string& fileName)
{
    if (!stream_.Native().IsOpenAtVslot08() || !Seek(4))
        return std::nullopt;

    for (;;)
    {
        std::uint8_t nameLength = 0;
        std::array<char, 256> name{};
        FileInfoCacheRecord record;
        auto& archive = record.archive;
        // 486780：任一头字段短读使扫描返回 false；不解析未选中的目录树。
        try
        {
            if (!ReadExact(&nameLength, 1) || !ReadExact(name.data(), nameLength) ||
                !ReadExact(&archive.lastWriteTime, 8) ||
                !ReadExact(&archive.archiveFileSize, 8) ||
                !ReadExact(&archive.archiveVersion, 4) ||
                !ReadExact(&archive.fileNodeCount, 4) || !ReadExact(&record.treeSize, 4))
                return std::nullopt;
            archive.archiveFileName = name.data();
        }
        catch (...)
        {
            // 原 486780 的局部 catch-all；不能扩到 seek、比较或已选中的 tree。
            return std::nullopt;
        }
        record.treeOffset = position_;
        if (_mbsicmp(reinterpret_cast<const unsigned char*>(fileName.c_str()),
                reinterpret_cast<const unsigned char*>(name.data())) == 0)
            return record;
        // 47FD4A..47FD55：跳过整个 treeSize，不预读、不校验无关记录的树。
        if (!Seek(position_ + record.treeSize))
            return std::nullopt;
    }
}

FileInfoCacheArchive FileInfoCacheStream::ReadTree(FileInfoCacheRecord record)
{
    return ReadTreeCore(std::move(record), nullptr);
}

FileInfoCacheArchive FileInfoCacheStream::ReadTree(FileInfoCacheRecord record, IFileInfoProgress& progress)
{
    return ReadTreeCore(std::move(record), &progress);
}

FileInfoCacheArchive FileInfoCacheStream::ReadTreeCore(
    FileInfoCacheRecord record, IFileInfoProgress* progress)
{
    RuntimeFileInfoCacheTree parsed;
    ReadTreeInto(record, parsed.root, parsed.fileCount, parsed.maximumDataLength, progress);
    record.archive.root = std::move(parsed.root);
    record.archive.fileNodeCount = parsed.fileCount;
    return std::move(record.archive);
}

void FileInfoCacheStream::ReadTreeInto(
    const FileInfoCacheRecord& record, ArchiveDirectoryRuntime& root,
    std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34, IFileInfoProgress& progress)
{
    const std::uint32_t iterationCount = record.archive.fileNodeCount;
    if (!stream_.Native().IsOpenAtVslot08() ||
        ((!positionKnown_||position_ != record.treeOffset) && !Seek(record.treeOffset)))
        throw 3;
    // 显式record工具适配，不由正式ArchiverInfo消费；保留它的有界值协议。
    std::vector<std::byte> tree(record.treeSize);
    if (!ReadExact(tree.data(), record.treeSize))
        throw 3;
    maximumAt34 = 0;
    fileCountAt30 = 0;
    ParseRuntimeFileInfoCacheTreeInto(
        tree, iterationCount, root, fileCountAt30, maximumAt34, progress);
}

void FileInfoCacheStream::ReadTreeInto(
    const FileInfoCacheRecord& record, ResourceFolderEntry& root,
    std::uint32_t& fileCountAt30, std::uint32_t& maximumAt34, IFileInfoProgress* const progress)
{
    // 提前取出本层迭代次数，允许输出 count 引用 record.archive.fileNodeCount。
    const std::uint32_t iterationCount = record.archive.fileNodeCount;
    if (!stream_.Native().IsOpenAtVslot08() ||
        ((!positionKnown_||position_ != record.treeOffset) && !Seek(record.treeOffset)))
        throw 3;
    std::vector<std::byte> tree(record.treeSize);
    // 47FCFE..47FFB1：已选中且元数据匹配的树短读必须失败，不能改成直接扫描 ARC。
    if (!ReadExact(tree.data(), record.treeSize))
        throw 3;
    // 47FD0F/47FD1A：读失败不能触及旧数值；47FD2A 直接使用原 root，未清树。
    maximumAt34 = 0;
    fileCountAt30 = 0;
    ParseRuntimeFileInfoCacheTreeInto(
        tree, iterationCount, root, fileCountAt30, maximumAt34, progress);
}

bool FileInfoCacheStream::FindNativeAt47FA10(FileInfoCacheNativeHeader& record,const RuntimeCString& archivePath)
{
    positionKnown_=false;
    if(!stream_.Native().IsOpenAtVslot08())return false;
    auto result=stream_.Native().SeekWithApi(4,FILE_BEGIN,api_);
    ArchiveRawResourceNameRuntime scanner("");
    ScanArchiveFileNameAt45D950(scanner.Value(),archivePath.GetString());
    ArchiveRawResourceNameRuntime wanted(scanner.Value().GetString());
    scanner.ReleaseNormal();
    bool found=false;
    while(result!=-1) // 47FB10确实比较FFFFFFFF；不能把原0返回擅自解释成miss。
    {
        if(!record.ReadAt486780(stream_.Native(),api_))break;
        const auto* name=record.Fields().nameAt00.GetString();
        if(!name)throw ATL::CAtlException(E_FAIL);
        if(_mbsicmp(reinterpret_cast<const unsigned char*>(wanted.Value().GetString()),
            reinterpret_cast<const unsigned char*>(name))==0){found=true;break;}
        result=stream_.Native().SeekWithApi(record.Fields().treeBytesAt20,FILE_CURRENT,api_);
    }
    wanted.ReleaseNormal();
    return found;
}

void FileInfoCacheStream::ReadNativeTreeAt47FCC4(const FileInfoCacheNativeHeader& record,
    ArchiveDirectoryRuntime& root,std::uint32_t& count,std::uint32_t& maximum,
    std::uint32_t& loaded,IFileInfoProgress& progress)
{
    positionKnown_=false;
    ByteArrayReferenceRuntime tree;
    tree.AllocateAt412700(record.Fields().treeBytesAt20);
    const auto requested=record.Fields().treeBytesAt20;
    if(requested>tree.Size())throw FileInfoCacheException(FileInfoCacheError::TreeSizeOutOfRange,0,
        "native finfo read exceeds allocated byte Ref");
    if(!stream_.Native().ReadExactWithApi(tree.Data(),requested,api_))throw 3;
    const auto size=record.Fields().treeBytesAt20; // 47FD09先捕获，再写max/count。
    maximum=0;count=0;
    const auto* bytes=reinterpret_cast<const std::byte*>(tree.Data());
    if(size>tree.Size())throw FileInfoCacheException(FileInfoCacheError::TreeSizeOutOfRange,0,
        "native finfo tree exceeds allocated byte Ref");
    ParseRuntimeFileInfoCacheTreeInto({bytes,size},record.Fields().countAt1C,root,count,maximum,progress);
    loaded=1; // 47FD38在临时byte Ref释放之前提交，不能挪到caller返回之后。
}
}
