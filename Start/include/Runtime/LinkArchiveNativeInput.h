#pragma once

#include <Runtime/FileStreamRuntime.h>
#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
// 只借用原16-byte流和外部API；不另存文件位置，定位使用真实FILE_CURRENT。
class LinkArchiveNativeInput final
{
public:
    LinkArchiveNativeInput(FileStreamRuntime& stream,IStartFileApi& api) noexcept:stream_(stream),api_(api){}
    void Read(void* output,DWORD bytes,int failure=1);
    void Begin();
    void Skip(DWORD bytes);
private:
    FileStreamRuntime& stream_;
    IStartFileApi& api_;
};

// 原493B8C/493BA6的两个nil必须都构造成功后，才取得整个头部的展开资格。
class LinkArchiveNativeHeader final
{
public:
    LinkArchiveNativeHeader();
    ~LinkArchiveNativeHeader() noexcept;
    LinkArchiveNativeHeader(const LinkArchiveNativeHeader&)=delete;
    LinkArchiveNativeHeader& operator=(const LinkArchiveNativeHeader&)=delete;
    bool ReadAt486480(LinkArchiveNativeInput&);
    const RuntimeCString& Magic() const noexcept{return body_.fields.magic;}
    const RuntimeCString& EmbeddedName() const noexcept{return body_.fields.name;}
    void ReleaseNormal();
private:
    struct Fields {RuntimeCString magic,name;std::uint32_t flags;};
    static_assert(sizeof(Fields)==12);
    union Body {Body() noexcept{} ~Body() noexcept{} Fields fields;} body_;
    bool active_=false;
};

struct LinkArchiveNativeEntryFields final
{
    std::uint32_t dataLength;
    std::uint16_t sourceFlags,year;
    std::uint8_t month,day,hour,minute,second;
};

// 原486220：发布名称及数据长度前保留逐次Read/seek；状态槽不预清零。
std::uint32_t ReadLinkArchiveNativeEntryAt486220(LinkArchiveNativeInput&,bool link6,
    RuntimeCString& name,LinkArchiveNativeEntryFields&);
}
