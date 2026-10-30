#include "MovieSceneNative.Internal.h"
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime::MovieSceneNativeDetail
{
ConstructionFrame::~ConstructionFrame()noexcept
{
    if(!active)return;
    switch(state)
    {
    case 7:DestroyRuntimeCStringAt40CD40(&alternatePath.value);[[fallthrough]];
    case 6:DestroyRuntimeCStringAt40CD40(&alternatePrefix.value);[[fallthrough]];
    case 4:
        if(pathOwned){pathOwned=false;DestroyRuntimeCStringAt40CD40(&path.value);}break;
    case 5:
        DestroyRuntimeCStringAt40CD40(&procOrPrefix.value);
        if(pathOwned){pathOwned=false;DestroyRuntimeCStringAt40CD40(&path.value);}break;
    case 3:DestroyRuntimeCStringAt40CD40(&procOrPrefix.value);break;
    case 2:
        // ConstructDirectMovieNativeAt4A0BFD已执行59CBE7对应的基表退役。
        [[fallthrough]];
    case 1:memory.DeleteBody(raw,0x38);break;
    default:break;
    }
    Retire(body);
}

namespace
{
template<class Function>Function ManagerSlot(ATL::IAtlStringMgr* manager,unsigned offset)noexcept
{return Virtual<Function>(manager,offset);}
void PublishNil(RuntimeCString& out,ATL::IAtlStringMgr* manager)
{
    using GetNil=ATL::CStringData*(__thiscall*)(ATL::IAtlStringMgr*);
    auto* nil=ManagerSlot<GetNil>(manager,12)(manager);
    const auto chars=reinterpret_cast<std::uintptr_t>(nil)+16u;
    // GetNil已提供引用；只发布原生指针，不额外构造或赋值CString。
    std::memcpy(static_cast<void*>(&out),&chars,4);
}
void BeginConcatenation(RuntimeCString& out,const RuntimeCString& prefix,
    IMovieSceneNativeSource& source,unsigned fallbackSite)
{
    using Clone=ATL::IAtlStringMgr*(__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager=Read(Header(Chars(prefix))->pStringMgr);
    if(manager)manager=ManagerSlot<Clone>(manager,16)(manager);
    if(!manager)
    {
        auto* global=source.CaptureStringManagerAt417360(fallbackSite);
        // 原4A543F/4667B0直接解全局manager；只有Clone返回空才抛E_FAIL。
        manager=ManagerSlot<Clone>(global,16)(global);
        if(!manager)throw ATL::CAtlException(E_FAIL);
    }
    PublishNil(out,manager);
}
struct TextUnwind final
{
    RuntimeCString* value;
    ~TextUnwind()noexcept{if(value)DestroyRuntimeCStringAt40CD40(value);}
};
void PrefixAt4A53F0(RuntimeCString& out,const RuntimeCString& base,const RuntimeCString& directory,
    IMovieSceneNativeSource& source)
{
    BeginConcatenation(out,base,source,0x4A543A);
    TextUnwind cleanup{&out};
    const auto* first=Chars(base);const auto* second=Chars(directory);
    const auto secondLength=std::uint32_t(Read(Header(second)->nDataLength));
    const auto firstLength=std::uint32_t(Read(Header(first)->nDataLength));
    ConcatenateRuntimeCStringAt4145E0(out,first,firstLength,second,secondLength);
    cleanup.value=nullptr;
}
void PathAt466760(RuntimeCString& out,const RuntimeCString& prefix,const char* name,
    IMovieSceneNativeSource& source)
{
    BeginConcatenation(out,prefix,source,0x4667AB);
    TextUnwind cleanup{&out};
    const auto length=name?std::uint32_t(std::strlen(name)):0u;
    const auto* first=Chars(prefix);
    const auto firstLength=std::uint32_t(Read(Header(first)->nDataLength));
    ConcatenateRuntimeCStringAt4145E0(out,first,firstLength,name,length);
    cleanup.value=nullptr;
}
void ConfigureMovie(ConstructionFrame& frame,MovieSceneNativeServices& services)
{
    auto& source=services.source;
    auto* configuration=Resolve<void>(Field<Slot>(Archive(source,1,0x4A0D12),0x1C));
    auto* mutedMovie=Movie(source,0x4A0D83,0x4A0D88,0x4A0D8E);
    using Byte=std::uint8_t*(__thiscall*)(void*);
    auto* muted=Virtual<Byte>(configuration,0x10)(configuration);
    mutedMovie->mutedAt34=Read(*muted);

    configuration=Resolve<void>(Field<Slot>(Archive(source,1,0x4A0DBB),0x1C));
    source.EnsureRootAt413BE0(0x4A0E2C);
    using Integer=std::int32_t*(__thiscall*)(void*);
    auto* volumeValue=Virtual<Integer>(configuration,0x14)(configuration);
    const auto value=Read(*volumeValue);
    auto* wrapper=Resolve<void>(*source.CaptureSoundAt4183D0(0x4A0E3A));
    long volume=-10000;
    if(wrapper)
    {
        auto* manager=source.CaptureStringManagerAt417360(0x4A0E64);
        if(!manager)throw ATL::CAtlException(E_FAIL);
        PublishNil(frame.procOrPrefix.value,manager);frame.state=3;
        SetRuntimeCStringAt437680(frame.procOrPrefix.value,"SoundCalcValueToVolume",22);
        const auto* chars=Chars(frame.procOrPrefix.value);
        const auto module=Read(Field<HMODULE>(wrapper,4));
        frame.state=0;
        const auto address=source.GetProcAddressAt4A0EB3(module,chars,0x4A0EB3);
        // ESI缓存GetProcAddress前的chars；释放不能重新读取被回调改过的CString槽。
        ReleaseRuntimeCStringHeader(Header(chars));
        using Calculate=long(__cdecl*)(std::int32_t,std::int32_t);
        volume=reinterpret_cast<Calculate>(address)(100,value);
    }
    auto* volumeMovie=Movie(source,0x4A0EE2,0x4A0EE7,0x4A0EEC);
    if(volume<-10000)volume=-10000;
    else if(volume>0)volume=0;
    volumeMovie->volumeAt30=volume;
}
}
}

namespace KaGuYa::Start::Runtime
{
MovieSceneNativeBody0Ch* ConstructMovieSceneNativeAt4A0B00(MovieSceneNativeBody0Ch& body,
    MovieSceneNativeServices& services)
{
    using namespace MovieSceneNativeDetail;
    MovieSceneNativeScope scope(services);auto& source=services.source;
    body.vptrAt00=&MovieSceneNativeMethods();
    ConstructionFrame frame{.body=body,.memory=services.memory,
        .procOrPrefix={},.path={},.alternatePrefix={},.alternatePath={}};
    body.savedModeAt08=Read(Field<std::uint32_t>(Archive(source,2,0x4A0B41),4));
    Field<volatile std::uint32_t>(Archive(source,2,0x4A0B92),4)=0;

    frame.raw=static_cast<DirectMovieNativeBody38h*>(services.memory.AllocateBody(0x38));
    auto* created=frame.raw;
    if(created)
    {
        frame.state=2;
        created=ConstructDirectMovieNativeAt4A0BFD(*::new(created) DirectMovieNativeBody38h,services.movie);
    }
    // 4A0C72在Root Ensure/旧Ref释放/control分配前撤销裸正文清理资格。
    frame.state=0;
    source.InstallMovieAt4A0C76(created,services.movie,services.memory,0x4A0C76);
    ConfigureMovie(frame,services);

    auto* archive0=Archive(source,0,0x4A0F26);
    auto* root=Root(source,0x4A0F72,0x4A0F77);
    auto* paramsRoot=source.CaptureParamsAt418480(0x4A0F7F);
    auto* pattern=Resolve<ParamsPatternNative>(paramsRoot->patternAt10.Native());
    auto* environment=source.CaptureEnvironmentAt4180C0(0x4A0FA5);
    auto* paths=Resolve<void>(environment->pathsAt48);
    const auto index=source.ResolveNamedAt498020(archive0,"prog_param00",0x4A0FD2);
    auto* params=source.ParamsAt47BDB0(root,0x4A0FDB);
    const auto resourceId=source.ReadValueAt4C73A0(params,index,0x4A0FE2);
    const auto* name=ReadParamsPatternNameNativeAt4AD1B0(pattern,resourceId);

    frame.state=4;
    PrefixAt4A53F0(frame.procOrPrefix.value,Field<RuntimeCString>(paths,8),services.movieDirectoryAt61A5B8,source);
    frame.state=5;
    PathAt466760(frame.path.value,frame.procOrPrefix.value,name,source);
    frame.pathOwned=true;
    frame.ReleaseNormal(frame.procOrPrefix.value,4);
    if(!source.ExistsAt46CC90(paths,Chars(frame.path.value),0x4A1046))
    {
        PrefixAt4A53F0(frame.alternatePrefix.value,Field<RuntimeCString>(paths,0x0C),services.movieDirectoryAt61A5B8,source);
        frame.state=6;
        PathAt466760(frame.alternatePath.value,frame.alternatePrefix.value,name,source);
        frame.state=7;
        AssignRuntimeCStringAt410220(frame.path.value,frame.alternatePath.value);
        frame.ReleaseNormal(frame.alternatePath.value,6);
        frame.ReleaseNormal(frame.alternatePrefix.value,4);
        if(!source.ExistsAt46CC90(paths,Chars(frame.path.value),0x4A10C5))
        {
            auto* errors=source.CaptureEnvironmentAt4180C0(0x4A10D4);
            source.AppendErrorAt4769B0(errors,MovieSceneMissingFileFormatSample18(),name,0x4A10DA);
            errors=source.CaptureEnvironmentAt4180C0(0x4A10E2);
            source.DisplayErrorsAt4AC730(errors,0x4A10E9);
            SetRuntimeCStringAt437680(frame.path.value,"",0);
        }
    }
    if(Read(Header(Chars(frame.path.value))->nDataLength)==0)
        RestoreAdvanceInvalidate(body,source,constructorCompletion);
    else
    {
        auto* movie=Movie(source,0x4A116A,0x4A116F,0x4A1175);
        const auto* table=Read(movie->vptrAt00);
        table->loadAt0C(movie,Chars(frame.path.value),0);
    }
    frame.ReleaseNormal(frame.path.value,0);
    frame.active=false;return &body;
}
}
