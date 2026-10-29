#pragma once
#include <Runtime/MovieSceneNativeBody.h>
#include <Runtime/MovieSceneNativeScope.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/InputRootNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;
struct ScriptParamsNativeBody;

class IMovieSceneNativeSource
{
public:
    virtual ~IMovieSceneNativeSource()=default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site)=0;
    virtual InputRootNativeBody24h* CaptureInputAt418210(std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime* CaptureSoundAt4183D0(std::uint32_t site)=0;
    virtual ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t site)=0;
    virtual ATL::IAtlStringMgr* CaptureStringManagerAt417360(std::uint32_t site)=0;
    virtual void EnsureRootAt413BE0(std::uint32_t site)=0;
    virtual void* RootAt6198C4(std::uint32_t site)=0;
    // 只借本次receiver的槽，不Ensure、AddRef、换域或缓存电影正文。
    virtual YtlReferenceSlotRuntime& RootMovieSlotAt48(void* root,std::uint32_t site)=0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void* root,std::uint32_t site)=0;
    // RootRuntimeOwner::InstallNativeDirectMovieAt4A0C76包含原来唯一一次Ensure。
    virtual void InstallMovieAt4A0C76(DirectMovieNativeBody38h*,DirectMovieNativeServices&,
        IEnvironmentRootMemory&,std::uint32_t site)=0;
    // 使用被捕获Root既有的电影域/服务，不再次Ensure。
    virtual void ReleaseMovieAt4B96D7(void* root,std::uint32_t site)=0;
    virtual std::int32_t ResolveNamedAt498020(void* archive0,const char* name,std::uint32_t site)=0;
    virtual ScriptParamsNativeBody* ParamsAt47BDB0(void* root,std::uint32_t site)=0;
    virtual std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*,std::int32_t index,std::uint32_t site)=0;
    virtual FARPROC GetProcAddressAt4A0EB3(HMODULE,const char* name,std::uint32_t site)=0;
    virtual bool ExistsAt46CC90(void* paths,const char* path,std::uint32_t site)=0;
    virtual void AppendErrorAt4769B0(EnvironmentRootStorage*,const char* format,
        const char* resource,std::uint32_t site)=0;
    virtual void DisplayErrorsAt4AC730(EnvironmentRootStorage*,std::uint32_t site)=0;
    virtual void AdvanceScenesAt4C1760(void* capturedRoot,void* scenes,std::uint32_t site)=0;
    virtual void* CaptureGraphicsAt4182F0(std::uint32_t site)=0;
    virtual void InvalidateAt4B02E0(void* graphics,const RECT*,std::uint32_t site)=0;
};

struct MovieSceneNativeServices final
{
    IMovieSceneNativeSource& source;
    DirectMovieNativeServices& movie;
    IEnvironmentRootMemory& memory;
    // 原61A5B8已初始化的进程CString，只借用，不复制或每次构造重建。
    const RuntimeCString& movieDirectoryAt61A5B8;
};

MovieSceneNativeServices& RequireMovieSceneNativeServices();
const MovieSceneNativeVtable4& MovieSceneNativeMethods()noexcept;
MovieSceneNativeBody0Ch* ConstructMovieSceneNativeAt4A0B00(MovieSceneNativeBody0Ch&,MovieSceneNativeServices&);
void UpdateMovieSceneNativeAt4B9520(MovieSceneNativeBody0Ch&,MovieSceneNativeServices&);
void DestroyMovieSceneNativeAt4A6EA0(MovieSceneNativeBody0Ch&)noexcept;
MovieSceneNativeBody0Ch* DeleteMovieSceneNativeAt4A6EA0(MovieSceneNativeBody0Ch*,std::uint32_t)noexcept;

struct MovieSceneNativeEhEvidence final
{
    std::uint32_t functionVA,funcInfoVA;
    std::array<std::uint32_t,9> funcInfo;
    std::span<const std::uint32_t> unwind;
};
std::span<const std::uint32_t,4> MovieSceneNativeOriginalTableSample18()noexcept;
std::span<const MovieSceneNativeEhEvidence,1> MovieSceneNativeEhSample18()noexcept;
const char* MovieSceneMissingFileFormatSample18()noexcept;
}
