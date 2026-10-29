#pragma once
#include <Runtime/MovieSceneNative.h>
#include <Runtime/MovieDirectoryNativeGlobals.h>
#include <Runtime/DirectMovieHostRuntime.h>
#include <Runtime/EnvironmentPathExistsNative.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeSoundRuntimeLibrary.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class EnvironmentStartupRuntime;
class IScrVmCommandErrorSink;
struct RootMovieSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentStartupRuntime& environment;
    ParamsRuntimeRootOwner& params;
    NativeInputRuntime& input;
    NativeSoundRuntimeLibrary& sound;
    NativeGraphicsRuntimeLibrary& graphics;
    IEnvironmentPathApi& paths;
    IScrVmCommandErrorSink& errors;
    WinHookRootNativeOwner& hooks;
    WinHookNativeServices& hookServices;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    const RuntimeCString& directory=ProcessMovieDirectoryNative();
};
class RootMovieSceneNativeSource final:public IMovieSceneNativeSource
{
public:
    explicit RootMovieSceneNativeSource(RootMovieSceneNativeDependencies d)noexcept:dependencies_(d){}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(unsigned)override;
    InputRootNativeBody24h* CaptureInputAt418210(unsigned)override;
    YtlReferenceSlotRuntime* CaptureSoundAt4183D0(unsigned)override;
    ParamsRuntimeRoot* CaptureParamsAt418480(unsigned)override;
    ATL::IAtlStringMgr* CaptureStringManagerAt417360(unsigned)override;
    void EnsureRootAt413BE0(unsigned)override;
    void* RootAt6198C4(unsigned)override;
    YtlReferenceSlotRuntime& RootMovieSlotAt48(void*,unsigned)override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*,unsigned)override;
    void InstallMovieAt4A0C76(DirectMovieNativeBody38h*,DirectMovieNativeServices&,IEnvironmentRootMemory&,unsigned)override;
    void ReleaseMovieAt4B96D7(void*,unsigned)override;
    std::int32_t ResolveNamedAt498020(void*,const char*,unsigned)override;
    ScriptParamsNativeBody* ParamsAt47BDB0(void*,unsigned)override;
    std::int32_t ReadValueAt4C73A0(ScriptParamsNativeBody*,std::int32_t,unsigned)override;
    FARPROC GetProcAddressAt4A0EB3(HMODULE,const char*,unsigned)override;
    bool ExistsAt46CC90(void*,const char*,unsigned)override;
    void AppendErrorAt4769B0(EnvironmentRootStorage*,const char*,const char*,unsigned)override;
    void DisplayErrorsAt4AC730(EnvironmentRootStorage*,unsigned)override;
    void AdvanceScenesAt4C1760(void*,void*,unsigned)override;
    void* CaptureGraphicsAt4182F0(unsigned)override;
    void InvalidateAt4B02E0(void*,const RECT*,unsigned)override;
private:
    RootMovieSceneNativeDependencies dependencies_;
};
// 所有依赖及本服务束必须长于Root movie和逃逸末Ref；构造不Capture或创建电影。
class RootMovieSceneServicesRuntime final
{
public:
    explicit RootMovieSceneServicesRuntime(RootMovieSceneNativeDependencies)noexcept;
    MovieSceneNativeServices& Scene()noexcept{return scene_;}
    DirectMovieNativeServices& Movie()noexcept{return movie_;}
    RootMovieSceneNativeSource& Source()noexcept{return source_;}
private:
    DirectMovieHostRuntime host_;
    DirectMovieNativeSource movieSource_;
    DirectMovieNativeServices movie_;
    RootMovieSceneNativeSource source_;
    MovieSceneNativeServices scene_;
};
}
