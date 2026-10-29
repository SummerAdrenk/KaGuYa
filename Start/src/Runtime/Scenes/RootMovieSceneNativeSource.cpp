#include <Runtime/RootMovieSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentErrorRuntime.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootMovieSceneNativeSource::CaptureEnvironmentAt4180C0(unsigned)
{return dependencies_.environment.RootSource().CaptureAt4180C0();}
InputRootNativeBody24h* RootMovieSceneNativeSource::CaptureInputAt418210(unsigned){return dependencies_.input.CaptureAt418210();}
YtlReferenceSlotRuntime* RootMovieSceneNativeSource::CaptureSoundAt4183D0(unsigned)
{return const_cast<YtlReferenceSlotRuntime*>(dependencies_.sound.ModuleSource().CaptureSoundModuleReference());}
ParamsRuntimeRoot* RootMovieSceneNativeSource::CaptureParamsAt418480(unsigned){return dependencies_.params.CaptureAt418480();}
ATL::IAtlStringMgr* RootMovieSceneNativeSource::CaptureStringManagerAt417360(unsigned){return dependencies_.strings.CaptureAt417360();}
void RootMovieSceneNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootMovieSceneNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootMovieSceneNativeSource::RootMovieSlotAt48(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->MovieSlotAt48();}
YtlReferenceSlotRuntime& RootMovieSceneNativeSource::RootScenesSlotAt30(void* root,unsigned)
{return static_cast<RootRuntimePayload*>(root)->SceneManagerSlotAt30();}
void RootMovieSceneNativeSource::InstallMovieAt4A0C76(DirectMovieNativeBody38h* body,DirectMovieNativeServices& services,IEnvironmentRootMemory& memory,unsigned)
{dependencies_.root.InstallNativeDirectMovieAt4A0C76(body,services,memory);}
void RootMovieSceneNativeSource::ReleaseMovieAt4B96D7(void* root,unsigned)
{static_cast<RootRuntimePayload*>(root)->ReleaseDirectMovie();}
std::int32_t RootMovieSceneNativeSource::ResolveNamedAt498020(void*,const char* name,unsigned)
{return ResolveScriptNamedValueNativeAt498020(name,dependencies_.names,dependencies_.strings);}
ScriptParamsNativeBody* RootMovieSceneNativeSource::ParamsAt47BDB0(void* root,unsigned)
{return &static_cast<RootRuntimePayload*>(root)->ParamsAt47BDB0();}
std::int32_t RootMovieSceneNativeSource::ReadValueAt4C73A0(ScriptParamsNativeBody* body,int index,unsigned)
{return ReadScriptValueAt4C73A0(*body,index,dependencies_.errors);}
FARPROC RootMovieSceneNativeSource::GetProcAddressAt4A0EB3(HMODULE module,const char* name,unsigned)
{return dependencies_.sound.Win32Source().GetProcAddress(module,name);}
bool RootMovieSceneNativeSource::ExistsAt46CC90(void*,const char* name,unsigned)
{return EnvironmentPathExistsNativeAt46CC90(name,dependencies_.paths,dependencies_.strings);}
namespace
{
void Append(EnvironmentRootStorage& root,IRuntimeStringManagerSource& strings,IEnvironmentRootMemory& memory,const char* format,...)
{
    va_list args;va_start(args,format);
    try{AppendEnvironmentErrorVAt4769B0(root,format,args,strings,memory);}catch(...){va_end(args);throw;}
    va_end(args);
}
}
void RootMovieSceneNativeSource::AppendErrorAt4769B0(EnvironmentRootStorage* root,const char* format,const char* resource,unsigned)
{Append(*root,dependencies_.strings,dependencies_.memory,format,resource);}
void RootMovieSceneNativeSource::DisplayErrorsAt4AC730(EnvironmentRootStorage* root,unsigned)
{DisplayEnvironmentErrorsAt4AC730(*root,dependencies_.messageBox,dependencies_.strings);}
void RootMovieSceneNativeSource::AdvanceScenesAt4C1760(void* capturedRoot,void* scenes,unsigned)
{
    auto* services=static_cast<RootRuntimePayload*>(capturedRoot)->PostWindow().BorrowSceneManagerNativeServices();
    if(!services)throw std::logic_error("Captured Root has no native SceneManager services");
    SceneManagerView(static_cast<SceneManagerNativeBody48h*>(scenes),*services).PopThenActivatePendingAt4C1760();
}
void* RootMovieSceneNativeSource::CaptureGraphicsAt4182F0(unsigned){return dependencies_.graphics.CaptureAt4182F0();}
void RootMovieSceneNativeSource::InvalidateAt4B02E0(void* graphics,const RECT* clip,unsigned)
{dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*static_cast<GraphicsRootNativeBody1Ch*>(graphics),clip);}
RootMovieSceneServicesRuntime::RootMovieSceneServicesRuntime(RootMovieSceneNativeDependencies d)noexcept
    :host_(d.environment,d.root,d.messageBox),movieSource_(host_,d.hooks),movie_{movieSource_,d.hookServices},
     source_(d),scene_{source_,movie_,d.memory,d.directory}{}
}
