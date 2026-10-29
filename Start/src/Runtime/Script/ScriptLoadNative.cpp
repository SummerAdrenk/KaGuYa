#include <Runtime/ScriptLoadNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <bit>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Factory=ScriptReaderFactoryNativeBody14h;
Factory* __fastcall Delete(Factory* body,void*,std::uint32_t flags) noexcept
{DestroyScriptReaderFactoryNativeAt4C9D20(*body);if(flags&1u)::operator delete(body,std::size_t{0x14});return body;}
const ScriptReaderFactoryNativeVtable1 table{reinterpret_cast<decltype(ScriptReaderFactoryNativeVtable1::deleteAt00)>(&Delete)};
struct FactoryUnwind final
{
    Factory& body;bool complete=false,armed=true;
    ~FactoryUnwind() noexcept
    {if(armed){if(complete)DestroyScriptReaderFactoryNativeAt4C9D20(body);else ReleaseYtlReference(body.readerAt04);}}
    void ReleaseNormal() noexcept{armed=false;DestroyScriptReaderFactoryNativeAt4C9D20(body);}
};
std::uintptr_t SaveParams(const ScriptParamsNativeBody* params)
{
    const auto& ref=static_cast<const volatile YtlReferenceSlotRuntime&>(params->saveParams2At30);
    const auto base=ref.baseAt00;if(!base)return 0;
    const auto* control=static_cast<const volatile EnvironmentObjectReferenceControl*>(ref.controlAt08);
    const auto stride=control?control->strideAt0C:0u;return base+ref.biasAt04*stride;
}
constexpr char missing[]="\x83\x74\x83\x40\x83\x43\x83\x8B\x96\xBC\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
constexpr char loadFailed[]="\x81\x77 %s \x81\x78\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49";
constexpr char notScript[]="\x81\x77 %s \x81\x78\x82\xCD\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xC5\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
ArchiveRawResourceNameRuntime Prefix(const RuntimeCString& detail,IRuntimeStringManagerSource& strings)
{
    return ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},[&](RuntimeCString* out)
    {ConstructRuntimeCStringPrefixAt4CA280(out,"LoadScript : ",detail,strings);});
}
}
const ScriptReaderFactoryNativeVtable1& ScriptReaderFactoryNativeMethods() noexcept{return table;}
void DestroyScriptReaderFactoryNativeAt4C9D20(Factory& body) noexcept
{
    body.vptrAt00=&table;ReleaseYtlReference(body.readerAt04);
    // 4C9D77已xor ECX,ECX；后继40D750内联正常尾不再读取槽，不补扫晚填引用。
}
std::uint8_t LoadScriptNativeAt4D3C90(ScriptNativeBody& receiver,std::int32_t id,ScriptLoadNativeServices& services)
{
    // 0/1资格在catch外；GetNil/初始Set失败不显示LoadScript错误框。
    ArchiveRawResourceNameRuntime detail(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* out){InitializeEmptyRuntimeCString(*out,services.strings);});
    SetRuntimeCStringAt437680(detail.Value(),"syntax error",12);
    bool success=false;
    try
    {
        const char* rawName=services.source.FindNameAt4AD1B0(id);
        auto name=ArchiveResourceNameAt40B530(rawName,services.strings);
        if(name.Value().GetLength()==0)
        {SetRuntimeCStringAt437680(detail.Value(),missing,int(sizeof(missing)-1));throw 1;}
        if(receiver.parserAtA0.scriptIdAt00!=id)
        {
            Factory factory;factory.vptrAt00=&table;factory.readerAt04={};factory.fieldAt10=-1;
            FactoryUnwind cleanup{factory};services.source.LoadAt4D44C0(factory,name.Value().GetString());cleanup.complete=true;
            if(!factory.readerAt04.baseAt00)
            {FormatRuntimeCStringAt417130(detail.Value(),loadFailed,name.Value().GetString());throw 1;}
            auto* reader=ResolveEnvironmentObjectReference<ArchiveStreamReaderRuntime>(factory.readerAt04);
            // getter返回原Ref地址；parser的原按值入口自行增持，不能复制文件payload。
            auto* bytes=reader->BytesAt4172C0();
            receiver.parserAtA0.LoadAt4D5BA0(id,bytes->RawSlot(),services.headers);
            if(!receiver.parserAtA0.bytesAt04.RawSlot().baseAt00)
            {FormatRuntimeCStringAt417130(detail.Value(),notScript,name.Value().GetString());throw 1;}
            auto* params=services.source.CaptureParamsAfter413BE0();
            *reinterpret_cast<volatile std::int32_t*>(SaveParams(params)+4u)=id;
            cleanup.ReleaseNormal();
        }
        // 同ID也读取header长度；在413BE0之前缓存，CString回调不得改变本次长度。
        const auto* text=receiver.parserAtA0.headerAt28.GetString();
        const auto length=(reinterpret_cast<const ATL::CStringData*>(text)-1)->nDataLength;
        auto* params=services.source.CaptureParamsAfter413BE0();
        params->programCounter=std::uint32_t(length)+4u;
        name.ReleaseNormal();success=true;
    }
    catch(...){/* 原5F186C覆盖state2..5，仅展开现有前缀，不替换syntax默认值。 */}
    if(success){detail.ReleaseNormal();return 1;}
    auto error=Prefix(detail.Value(),services.strings);
    const char* message=error.Value().GetString();auto* env=services.source.CaptureEnvironmentAt4180C0();
    services.source.MessageBoxAt4D3EE6(reinterpret_cast<HWND>(env->windowAt2C),message,env->titleAt10.GetString(),MB_ICONERROR);
    // 4D3EEF/EF7之间state7不撤销：任一Free失败仍展开error，再展开detail。
    error.ReleasePreservingQualification();detail.ReleasePreservingQualification();
    error.DismissQualification();detail.DismissQualification();return 0;
}
}
