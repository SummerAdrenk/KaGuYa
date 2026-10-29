#include <Runtime/FileInfoCacheNativeHeader.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
FileInfoCacheNativeHeader::FileInfoCacheNativeHeader()
{
    InitializeEmptyRuntimeCString(storage_.fields.nameAt00);
    active_=true; // 原只初始化name，其它标量跨次Read保留，不预清零。
}
FileInfoCacheNativeHeader::~FileInfoCacheNativeHeader() noexcept
{if(active_)DestroyRuntimeCStringAt40CD40(&storage_.fields.nameAt00);}
void FileInfoCacheNativeHeader::ReleaseNormal()
{active_=false;DestroyRuntimeCStringAt40CD40(&storage_.fields.nameAt00);}

bool FileInfoCacheNativeHeader::ReadAt486780(FileStreamRuntime& stream,IStartFileApi& api)
{
    auto& f=storage_.fields;
    try
    {
        std::uint8_t length;
        if(!stream.ReadExactWithApi(&length,1,api))throw 1;
        auto* buffer=GetRuntimeCStringBufferSetLengthAt417260(f.nameAt00,length);
        if(!stream.ReadExactWithApi(buffer,length,api))throw 1;
        ReleaseRuntimeCStringBufferAt486B10(f.nameAt00);
        if(!stream.ReadExactWithApi(&f.lastWriteAt08,8,api)||
            !stream.ReadExactWithApi(&f.fileSizeAt10,8,api)||
            !stream.ReadExactWithApi(&f.versionAt18,4,api)||
            !stream.ReadExactWithApi(&f.countAt1C,4,api)||
            !stream.ReadExactWithApi(&f.treeBytesAt20,4,api))throw 1;
        return true;
    }
    catch(...){return false;} // 4868CA仅保护头部读取，不保护caller的seek/比较/树读取。
}

namespace
{
void FormatMinute(RuntimeCString& text,std::int64_t& time,ISaveLoadTimestampNativeSource& source)
{
    std::tm minuteTm,hourTm,dayTm,monthTm;
    const int minute=source.LocalTime64At5697BB(&minuteTm,&time)==0?minuteTm.tm_min:-1;
    const int hour=source.LocalTime64At5697BB(&hourTm,&time)==0?hourTm.tm_hour:-1;
    const int day=source.LocalTime64At5697BB(&dayTm,&time)==0?dayTm.tm_mday:0;
    const int month=source.LocalTime64At5697BB(&monthTm,&time)==0?
        std::bit_cast<int>(static_cast<unsigned>(monthTm.tm_mon)+1u):0;
    const int year=SaveLoadTimestampYearNativeAt47E120(&time,source);
    FormatRuntimeCStringAt417130(text,"%d-%d-%d %d:%d",year,month,day,hour,minute);
}
}

bool MatchFileInfoMetadataAt47FB67(std::int64_t diskTime,std::uint64_t diskSize,
    const FileInfoCacheNativeFields& cached,ISaveLoadTimestampNativeSource& source)
{
    // 47FB7F/47FB89：缓存时间只取低DWORD并以高DWORD=0组成快照。
    std::int64_t cachedTime=static_cast<std::uint32_t>(cached.lastWriteAt08);
    const auto nil=[](RuntimeCString* text){InitializeEmptyRuntimeCString(*text);};
    ArchiveRawResourceNameRuntime disk(ArchiveRawResourceNameRuntime::ProducerTag{},nil);
    FormatMinute(disk.Value(),diskTime,source);
    ArchiveRawResourceNameRuntime cache(ArchiveRawResourceNameRuntime::ProducerTag{},nil);
    FormatMinute(cache.Value(),cachedTime,source);
    const char* const second=cache.Value().GetString(); // 47FC48，Str2先判空。
    if(!second)throw ATL::CAtlException(E_FAIL);
    const char* const first=disk.Value().GetString();
    const bool same=_mbsicmp(reinterpret_cast<const unsigned char*>(first),
        reinterpret_cast<const unsigned char*>(second))==0&&
        static_cast<std::uint32_t>(diskSize)==static_cast<std::uint32_t>(cached.fileSizeAt10)&&
        (cached.fileSizeAt10>>32)==0;
    cache.ReleaseNormalCached(second);
    disk.ReleaseNormalCached(first);
    return same;
}
}
