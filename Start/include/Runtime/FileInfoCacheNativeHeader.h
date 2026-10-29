#pragma once
#include <Runtime/RuntimeCString.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/SaveLoadTimestampNative.h>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
struct FileInfoCacheNativeFields final
{
    RuntimeCString nameAt00;
    std::uint32_t paddingAt04;
    std::int64_t lastWriteAt08;
    std::uint64_t fileSizeAt10;
    std::uint32_t versionAt18,countAt1C,treeBytesAt20;
};
static_assert(sizeof(FileInfoCacheNativeFields)==0x28);
static_assert(offsetof(FileInfoCacheNativeFields,lastWriteAt08)==8);
static_assert(offsetof(FileInfoCacheNativeFields,treeBytesAt20)==0x20);

class FileInfoCacheNativeHeader final
{
public:
    FileInfoCacheNativeHeader();
    ~FileInfoCacheNativeHeader() noexcept;
    FileInfoCacheNativeHeader(const FileInfoCacheNativeHeader&)=delete;
    FileInfoCacheNativeHeader& operator=(const FileInfoCacheNativeHeader&)=delete;
    FileInfoCacheNativeFields& Fields() noexcept{return storage_.fields;}
    const FileInfoCacheNativeFields& Fields() const noexcept{return storage_.fields;}
    bool ReadAt486780(FileStreamRuntime&,IStartFileApi&);
    void ReleaseNormal();
private:
    union Storage {Storage() noexcept{} ~Storage() noexcept{} FileInfoCacheNativeFields fields;} storage_;
    bool active_=false;
};

// 原先格式化并比较两个分钟串，再比较文件大小；不能提前合并为两次tm转换。
bool MatchFileInfoMetadataAt47FB67(std::int64_t diskTime,std::uint64_t diskSize,
    const FileInfoCacheNativeFields&,ISaveLoadTimestampNativeSource&);
}
