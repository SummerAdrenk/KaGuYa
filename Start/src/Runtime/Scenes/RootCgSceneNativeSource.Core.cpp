#include <Runtime/RootCgSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/NativeInputRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ReadScriptInputNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
RootCgSceneNativeSource::RootCgSceneNativeSource(RootCgSceneNativeDependencies d) noexcept
    :dependencies_(d),archives_(d.root),records_(d.root,d.environment,d.files,d.key,d.codecs,d.graphics,d.messages,d.strings,d.transaction),
     pictureServices_{d.graphics,d.strings,d.graphics.RootSource(),d.graphics.Win32Source(),d.codecs,{},ArchiveCodecInputMode::Bounded},pictures_(pictureServices_){}
const ParamsRuntimeRoot* RootCgSceneNativeSource::CaptureParamsAt418480(unsigned){return dependencies_.params.CaptureAt418480();}
EnvironmentRootStorage* RootCgSceneNativeSource::CaptureEnvironmentAt4180C0(unsigned){return dependencies_.environment.RootSource().CaptureAt4180C0();}
InputRootNativeBody24h* RootCgSceneNativeSource::CaptureInputAt418210(unsigned){return dependencies_.input.CaptureAt418210();}
EnvironmentRootStorage*& RootCgSceneNativeSource::EnvironmentPublicationAt6198C8(unsigned)
{return dependencies_.environment.RootSource().PublicationSlot();}
EnvironmentRootStorage* RootCgSceneNativeSource::CaptureEnvironmentInlineAt6198C8(unsigned)
{return dependencies_.environment.RootSource().CaptureAt4180C0();}
void RootCgSceneNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootCgSceneNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootCgSceneNativeSource::RootDrawSlotAt18(void* p){return static_cast<RootRuntimePayload*>(p)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootCgSceneNativeSource::RootSoundSlotAt24(void* p){return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();}
YtlReferenceSlotRuntime& RootCgSceneNativeSource::RootScenesSlotAt30(void* p){return static_cast<RootRuntimePayload*>(p)->SceneManagerSlotAt30();}
SceneManagerNativeBody48h* RootCgSceneNativeSource::CaptureScenesAt413BE0(unsigned site)
{
    EnsureRootAt413BE0(site);auto* root=RootAt6198C4(site+5u);
    return ResolveEnvironmentObjectReference<SceneManagerNativeBody48h>(RootScenesSlotAt30(root));
}
void RootCgSceneNativeSource::WriteRootByte(void* p,unsigned offset,std::uint8_t value)
{
    auto* root=static_cast<RootRuntimePayload*>(p);
    if(offset==0x218)root->WriteByteAt218(value);
    else if(offset==0x219)root->WriteByteAt219(value);
    else throw std::logic_error("CG scene supplied unknown Root byte");
}
void RootCgSceneNativeSource::ConfigureScenesAt4703D0(void* p,int value)
{ConfigureSceneManagerNativeAt4703D0(*static_cast<SceneManagerNativeBody48h*>(p),value,RequireSceneManagerNativeServices());}
void RootCgSceneNativeSource::PushSceneAt4A9030(void* p,const char* name)
{SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()).PushActiveScene(name);}
void RootCgSceneNativeSource::QueueSceneAt47F920(void* p,const char* name)
{SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()).QueueScene(name);}
void RootCgSceneNativeSource::QueueTitleAfterConstructorAt47E180(void* p)
{
    // 固定入口receiver；47E180内部独立取Root重置Params/文字，再清场景并排队标题。
    dependencies_.recovery.ResetAndQueueTitleAt47E180(SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()));
}
bool RootCgSceneNativeSource::AccelerateAt4AA970(void*)
{return ReadScriptAccelerateInputAt4AA970(dependencies_.input.RootSource())!=0;}
const char* RootCgSceneNativeSource::CueNameAt(unsigned address)
{
    switch(address)
    {
    case 0x61A564:return dependencies_.confirmAt61A564.GetString();
    case 0x61A568:return dependencies_.focusAt61A568.GetString();
    case 0x61A56C:return dependencies_.exitAt61A56C.GetString();
    default:throw std::logic_error("CG button supplied unknown original cue address");
    }
}
}
