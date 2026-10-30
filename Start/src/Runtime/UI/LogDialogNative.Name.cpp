#include "LogDialogNative.Internal.h"
#include <cerrno>
#include <cstdlib>
#include <mbstring.h>
#include <limits>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
char* NameChars(const RuntimeCString& value) noexcept
{return const_cast<char*>(value.GetString());}
ATL::CStringData* NameHeader(const char* chars) noexcept
{return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(chars)-16u);}
char* NameData(ATL::CStringData* header) noexcept
{return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(header)+16u);}
char* NameOffset(char* chars,int offset) noexcept
{return reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(chars)+static_cast<std::uint32_t>(offset));}
int NameDistance(const char* first,const char* second) noexcept
{return std::bit_cast<int>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(first)-reinterpret_cast<std::uintptr_t>(second)));}
void PublishName(RuntimeCString& value,char* chars) noexcept
{
    // 仅发布已有引用的原生指针；Clone及旧串释放仍由调用方负责。
    std::memcpy(static_cast<void*>(&value),&chars,sizeof(chars));
}
ATL::IAtlStringMgr* CloneNameManager(ATL::IAtlStringMgr* manager)
{
    using Clone=ATL::IAtlStringMgr*(__thiscall*)(ATL::IAtlStringMgr*);
    const std::byte* table;Clone clone;
    // 按原生vptr读取Clone虚槽，不经C++虚调用去虚拟化。
    std::memcpy(&table,static_cast<const void*>(manager),sizeof(table));std::memcpy(&clone,table+16,sizeof(clone));
    return clone(manager);
}
ATL::IAtlStringMgr* NameSubstringManager(ATL::CStringData* cached,IRuntimeStringManagerSource& strings)
{
    // 53C49C/53C552：只从缓存EBX选manager；Clone不能改换后续复制的ESI。
    if(auto* manager=Read(cached->pStringMgr))if(auto* clone=CloneNameManager(manager))return clone;
    // 原global为空时仍先读其vptr；不能用nil或成功manager补位。
    return CloneNameManager(strings.CaptureAt417360());
}
int MoveNameTrim(char* destination,std::uint32_t capacity,const char* source,std::uint32_t bytes)
{
    if(!bytes)return 0;
    // 53C674/53C690的内联错误分支不清目标；43D9B0的错误清零政策不能提前套用。
    if(!source){errno=EINVAL;_invalid_parameter_noinfo();return EINVAL;}
    if(capacity<bytes){errno=ERANGE;_invalid_parameter_noinfo();return ERANGE;}
    return MoveRuntimeBytesAt43D9B0(destination,capacity,source,bytes);
}
void CheckNameMove(int error)
{
    // 53C6B5 -> 4136D0：0/80正常返回，22/34映射E_INVALIDARG。
    if(error==0||error==80)return;
    if(error==EINVAL||error==ERANGE)throw ATL::CAtlException(E_INVALIDARG);
    if(error==ENOMEM)throw ATL::CAtlException(E_OUTOFMEMORY);
    throw ATL::CAtlException(E_FAIL);
}
}
void FormatName(RuntimeCString* out,RuntimeCString& input)
{
    auto& strings=RequireLogDialogNativeServices().strings;InitializeEmptyRuntimeCString(*out,strings);
    struct Result final{RuntimeCString* p;bool active=true;~Result() noexcept{if(active)DestroyRuntimeCStringAt40CD40(p);}} result{out};
    SetRuntimeCStringAt437680(*out,NamePrefix(),2);
    auto* initial=NameChars(input);bool flatten=false;
    if(Read(NameHeader(initial)->nDataLength)>0)
    {
        auto* newline=::_mbschr(reinterpret_cast<const unsigned char*>(initial),10);
        // 53C3D2在Find返回后重读input槽，而后面的循环使用独立缓存ESI。
        if(newline)flatten=NameDistance(reinterpret_cast<const char*>(newline),NameChars(input))>=0;
    }
    if(flatten)
    {
        CStringLife flattened;flattened.Empty();SetRuntimeCStringAt437680(flattened.value,"",0);
        CStringLife remaining;
        auto* esi=NameData(CloneRuntimeCStringDataAt414540(NameHeader(NameChars(input))));
        PublishName(remaining.value,esi);remaining.active=true; // 53C436/439，state4取得资格。
        while(::_mbscmp(reinterpret_cast<const unsigned char*>(esi),reinterpret_cast<const unsigned char*>("")))
        {
            auto* newline=Read(NameHeader(esi)->nDataLength)>0?::_mbschr(reinterpret_cast<const unsigned char*>(esi),10):nullptr;
            const auto index=newline?NameDistance(reinterpret_cast<const char*>(newline),esi):-1;
            if(index<0)
            {
                AppendRuntimeCStringAt413550(flattened.value,esi,Read(NameHeader(esi)->nDataLength));
                SetRuntimeCStringAt437680(remaining.value,"",0);
                esi=NameChars(remaining.value);continue; // 唯一对应53C6F5的槽重读。
            }
            auto* ebx=NameHeader(esi); // 53C483：跨left Clone/Append/Free保留旧header。
            CStringLife left;
            if(index>=Read(ebx->nDataLength))
                PublishName(left.value,NameData(CloneRuntimeCStringDataAt414540(ebx)));
            else
            {
                auto* manager=NameSubstringManager(ebx,strings);
                ConstructRuntimeCStringAt40B490(&left.value,esi,index,manager);
            }
            left.active=true; // 40B490失败后不补收其部分发布，只有正常返回才进入state5。
            auto* leftChars=NameChars(left.value);auto* leftHeader=NameHeader(leftChars);
            AppendRuntimeCStringAt413550(flattened.value,leftChars,Read(leftHeader->nDataLength));
            left.NormalCaptured(leftHeader);

            // 53C500..536的Mid已内联；count不是INT_MAX，源chars也不能重读remaining槽。
            const auto rawStart=Add(index,1);
            const auto start=rawStart<0?0:rawStart;
            const auto difference=Sub(Read(NameHeader(esi)->nDataLength),rawStart);
            auto count=difference<0?0:difference;
            if(Sub((std::numeric_limits<int>::max)(),start)<count)
                throw ATL::CAtlException(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
            const auto length=Read(NameHeader(esi)->nDataLength);
            if(Add(count,start)>length)count=Sub(length,start);
            if(start>length)count=0;
            CStringLife right;
            if(start==0&&count==length)
                PublishName(right.value,NameData(CloneRuntimeCStringDataAt414540(ebx)));
            else
            {
                auto* manager=NameSubstringManager(ebx,strings);
                ConstructRuntimeCStringAt40B490(&right.value,NameOffset(esi,start),count,manager);
            }
            right.active=true;auto* rightChars=NameChars(right.value);auto* edi=NameHeader(rightChars);
            if(edi!=ebx)
            {
                // 53C593..5CC：赋值资格使用旧EBX，不借410220重新选destination。
                const auto oldRefs=Read(ebx->nRefs);
                if(oldRefs<0)
                {
                    SetRuntimeCStringAt437680(remaining.value,rightChars,Read(edi->nDataLength));
                    esi=NameChars(remaining.value);
                }
                else
                {
                    auto* incomingManager=Read(edi->pStringMgr);
                    auto* oldManager=Read(ebx->pStringMgr);
                    if(incomingManager==oldManager)
                    {
                        auto* next=CloneRuntimeCStringDataAt414540(edi);
                        ReleaseRuntimeCStringHeader(ebx); // Clone后的旧Free失败：不补收next，不提前发布。
                        esi=NameData(next);PublishName(remaining.value,esi);
                    }
                    else
                    {
                        SetRuntimeCStringAt437680(remaining.value,rightChars,Read(edi->nDataLength));
                        esi=NameChars(remaining.value);
                    }
                }
            }
            // 正常state6->4释放缓存EDI；若前面的赋值抛出，right EH仍清当前槽。
            right.NormalCaptured(edi);

            auto* begin=reinterpret_cast<const unsigned char*>(esi);auto* first=begin;
            while(*first&&::_mbschr(reinterpret_cast<const unsigned char*>(TrimCharacters()),*first))first=::_mbsinc(first);
            if(first!=begin)
            {
                // 53C635..65C：先读缓存ESI的length/alloc/refs；仅42D590返回才刷新ESI。
                const auto requested=Read(NameHeader(esi)->nDataLength);
                const auto removed=NameDistance(reinterpret_cast<const char*>(first),esi);
                if(requested<0)throw ATL::CAtlException(E_INVALIDARG);
                const auto allocation=Read(NameHeader(esi)->nAllocLength);
                const auto references=Read(NameHeader(esi)->nRefs);
                if((Sub(1,references)|Sub(allocation,requested))<0)
                {
                    PrepareRuntimeCStringWriteAt42D590(remaining.value,requested);
                    esi=NameChars(remaining.value);
                }
                const auto currentLength=Read(NameHeader(esi)->nDataLength);
                auto* source=NameOffset(esi,removed);const auto next=Sub(currentLength,removed);
                CheckNameMove(MoveNameTrim(esi,static_cast<std::uint32_t>(Add(currentLength,1)),source,
                    static_cast<std::uint32_t>(Add(next,1))));
                if(next<0||next>Read(NameHeader(esi)->nAllocLength))throw ATL::CAtlException(E_INVALIDARG);
                NameHeader(esi)->nDataLength=next;*NameOffset(esi,next)=0;
            }
        }
        auto* remainingHeader=NameHeader(esi); // 53C6FD没有槽重读；ESI继续跨input赋值保留。
        AssignRuntimeCStringAt410220(input,flattened.value);
        remaining.NormalCaptured(remainingHeader);flattened.Normal();
    }
    auto* finalChars=NameChars(input);
    AppendRuntimeCStringAt413550(*out,finalChars,Read(NameHeader(finalChars)->nDataLength));
    AppendRuntimeCStringAt413550(*out,NameSuffix(),2);result.active=false;
}
bool CreateFont(const std::array<int,4>& descriptor)
{
    auto& s=RequireLogDialogNativeServices();GraphicsTextNativeFontParameters p;
    InitializeGraphicsTextFontNativeAt40C6E0(p);p.fontAt00.lfHeight=Sub(0,descriptor[0]);p.fontAt00.lfWeight=descriptor[1];
    ::lstrcpynA(p.fontAt00.lfFaceName,s.source.FontFaceAt61A684(),32);
    p.renderModeAt3E=static_cast<std::uint8_t>((Read(Field<std::uint8_t>(Registry(1,0x53C7DC),0x61))!=0)+1);
    p.textColorAt40=0xFFFFFF;
    if(Read(Field<std::uint8_t>(Registry(1,0x53C83A),0x60)))
    {p.shadowColorAt44=0;p.shadowXAt4C=1;p.shadowYAt50=2;}
    else{p.shadowColorAt44=0xFFFFFFFFu;p.shadowXAt4C=p.shadowYAt50=0;}
    p.lineSpacingAt3D=static_cast<std::uint8_t>(descriptor[3]);p.characterSpacingAt3C=static_cast<std::uint8_t>(descriptor[2]);
    p.verticalAt55=0;return s.text.CreateFontAt4388B0(&p);
}
}
