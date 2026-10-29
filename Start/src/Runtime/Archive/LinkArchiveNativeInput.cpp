#include <Runtime/LinkArchiveNativeInput.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/LinkArchiveIndexParser.h>
#include <bit>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
char* PrepareRead(RuntimeCString& text,int length)
{return GetRuntimeCStringBufferSetLengthAt417260(text,length);}
void FinishRead(RuntimeCString& text)
{ReleaseRuntimeCStringBufferAt486B10(text);}
bool Equal(const RuntimeCString& text,const char* value)
{return _mbscmp(reinterpret_cast<const unsigned char*>(text.GetString()),reinterpret_cast<const unsigned char*>(value))==0;}
}

void LinkArchiveNativeInput::Read(void* output,DWORD bytes,int failure)
{if(!stream_.ReadExactWithApi(output,bytes,api_))throw failure;}
void LinkArchiveNativeInput::Begin()
{if(!stream_.SeekWithApi(0,FILE_BEGIN,api_))throw 1;}
void LinkArchiveNativeInput::Skip(DWORD bytes)
{static_cast<void>(stream_.SeekWithApi(bytes,FILE_CURRENT,api_));}

LinkArchiveNativeHeader::LinkArchiveNativeHeader()
{
    InitializeEmptyRuntimeCString(body_.fields.magic);
    InitializeEmptyRuntimeCString(body_.fields.name);
    active_=true;
}
LinkArchiveNativeHeader::~LinkArchiveNativeHeader() noexcept
{
    if(active_)
    {
        DestroyRuntimeCStringAt40CD40(&body_.fields.name);
        DestroyRuntimeCStringAt40CD40(&body_.fields.magic);
    }
}
void LinkArchiveNativeHeader::ReleaseNormal()
{
    // 493D13先撤整个header的EH资格，name正常Free抛出时不能擅补magic清理。
    active_=false;
    DestroyRuntimeCStringAt40CD40(&body_.fields.name);
    DestroyRuntimeCStringAt40CD40(&body_.fields.magic);
}
bool LinkArchiveNativeHeader::ReadAt486480(LinkArchiveNativeInput& input)
{
    try
    {
        auto& magic=body_.fields.magic;
        auto& name=body_.fields.name;
        input.Read(PrepareRead(magic,5),5);FinishRead(magic);
        if(Equal(magic,"LINK6"))
        {
            std::uint16_t flags;input.Read(&flags,2);body_.fields.flags=flags;
            std::uint8_t length;input.Read(&length,1);
            if(!length){SetRuntimeCStringAt437680(name,"",0);return true;}
            input.Read(PrepareRead(name,length),length);FinishRead(name);
        }
        else
        {
            input.Read(PrepareRead(name,3),3);FinishRead(name);body_.fields.flags=0;
            if(Equal(magic,"LINK4")||Equal(magic,"LINK5"))
            {std::uint16_t flags;input.Read(&flags,2);body_.fields.flags=flags;}
        }
        return true;
    }
    catch(...){return false;} // 原486480自身catch-all；caller再抛3，不回滚已写头部。
}

std::uint32_t ReadLinkArchiveNativeEntryAt486220(LinkArchiveNativeInput& input,bool link6,
    RuntimeCString& name,LinkArchiveNativeEntryFields& fields)
{
    std::uint32_t chunk;input.Read(&chunk,4,0);
    if(!chunk)return 0;
    input.Read(&fields.sourceFlags,2);input.Read(&fields.year,2);
    input.Read(&fields.month,1);input.Read(&fields.day,1);input.Read(&fields.hour,1);
    input.Read(&fields.minute,1);input.Read(&fields.second,1);
    std::uint32_t header;
    if(link6)
    {
        std::uint16_t length;input.Read(&length,2);
        ByteArrayReferenceRuntime wide;
        wide.AllocateAt412700(static_cast<unsigned>(length)+2);
        // 486339先读control count，48634B才取data；Read/转换也分别重新解Ref。
        const auto count=wide.Size();std::memset(wide.Data(),0,count);
        input.Read(wide.Data(),length);
        const auto* source=wide.Data();
        if(length&1u)
        {
            bool terminated=false;
            for(unsigned i=0;i+1<count;i+=2)if(!source[i]&&!source[i+1]){terminated=true;break;}
            if(!terminated)throw LinkArchiveFormatError(0,"LINK6 odd name has no bounded WCHAR terminator");
        }
        AssignRuntimeCStringWideAt4102D0(name,reinterpret_cast<const wchar_t*>(source));
        header=static_cast<unsigned>(length)+15;
    } // 原4863AF先释放byte Ref，之后才发布dataLength并seek。
    else
    {
        std::uint8_t length;input.Read(&length,1);input.Skip(2);
        input.Read(PrepareRead(name,length),length);FinishRead(name);
        header=static_cast<unsigned>(length)+16;
    }
    fields.dataLength=chunk-header;
    input.Skip(fields.dataLength);
    return header;
}
}
