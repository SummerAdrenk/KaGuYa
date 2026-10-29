#include <Runtime/RootReplaySceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsResourceNameRuntime.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>

namespace KaGuYa::Start::Runtime
{
RootReplaySceneNativeSource::RootReplaySceneNativeSource(RootReplaySceneNativeDependencies d)noexcept
    :dependencies_(d),common_(d.common){}
const ParamsRuntimeRoot* RootReplaySceneNativeSource::CaptureParamsAt418480(unsigned site){return common_.CaptureParamsAt418480(site);}
EnvironmentRootStorage* RootReplaySceneNativeSource::CaptureEnvironmentAt4180C0(unsigned site){return common_.CaptureEnvironmentAt4180C0(site);}
InputRootNativeBody24h* RootReplaySceneNativeSource::CaptureInputAt418210(unsigned site){return common_.CaptureInputAt418210(site);}
EnvironmentRootStorage*& RootReplaySceneNativeSource::EnvironmentPublicationAt6198C8(unsigned site){return common_.EnvironmentPublicationAt6198C8(site);}
void RootReplaySceneNativeSource::EnsureRootAt413BE0(unsigned site){common_.EnsureRootAt413BE0(site);}
void* RootReplaySceneNativeSource::RootAt6198C4(unsigned site){return common_.RootAt6198C4(site);}
YtlReferenceSlotRuntime& RootReplaySceneNativeSource::RootDrawSlotAt18(void* p){return common_.RootDrawSlotAt18(p);}
YtlReferenceSlotRuntime& RootReplaySceneNativeSource::RootSoundSlotAt24(void* p){return common_.RootSoundSlotAt24(p);}
YtlReferenceSlotRuntime& RootReplaySceneNativeSource::RootScenesSlotAt30(void* p){return common_.RootScenesSlotAt30(p);}
YtlReferenceSlotRuntime& RootReplaySceneNativeSource::RootInformationSlotAt3C(void* p)
{return static_cast<RootRuntimePayload*>(p)->InformationDisplaySlotAt3C();}
ReplayScriptNativeReceiver RootReplaySceneNativeSource::ScriptAt47CD00(void* p)
{
    auto& root=*static_cast<RootRuntimePayload*>(p);auto* script=root.ScriptAt47CD00();
    return {script,&root.ScriptPublication(),&root}; // getter回调换根时仍属于caller固定的旧root。
}
void RootReplaySceneNativeSource::WriteRootByte(void* p,unsigned offset,std::uint8_t value){common_.WriteRootByte(p,offset,value);}
void RootReplaySceneNativeSource::ConfigureScenesAt4703D0(void* p,int value){common_.ConfigureScenesAt4703D0(p,value);}
void RootReplaySceneNativeSource::PushSceneAt4A9030(void* p,const char* name){common_.PushSceneAt4A9030(p,name);}
void RootReplaySceneNativeSource::QueueSceneAt47F920(void* p,const char* name){common_.QueueSceneAt47F920(p,name);}
void RootReplaySceneNativeSource::QueueTitleAfterConstructorAt47E180(void* p){common_.QueueTitleAfterConstructorAt47E180(p);}
bool RootReplaySceneNativeSource::AccelerateAt4AA970(void* p){return common_.AccelerateAt4AA970(p);}
const char* RootReplaySceneNativeSource::CueNameAt(unsigned address){return common_.CueNameAt(address);}
const char* RootReplaySceneNativeSource::PatternNameAt4AD1B0(const ParamsPatternNative* p,int id)
{return ReadParamsPatternNameNativeAt4AD1B0(p,id);}
void RootReplaySceneNativeSource::StopMusicAt4383B0(void* p,unsigned slot)
{dependencies_.common.root.NativeSoundServices().Wait(static_cast<SoundManagerNativeBody50h*>(p)).StopAt4383B0(slot);}
void RootReplaySceneNativeSource::ClearVoicesAt4384D0(void* p)
{static_cast<void>(dependencies_.common.root.NativeSoundServices().ClearVoicesCapturedAt4384D0(static_cast<SoundManagerNativeBody50h*>(p)));}
void RootReplaySceneNativeSource::StopMappedEffectsAt413270(void* p,EnvironmentVectorStorage<RuntimeCString>& mapping)
{dependencies_.common.root.NativeSoundServices().StopEffectsCapturedAt413270(static_cast<SoundManagerNativeBody50h*>(p),&mapping);}
void RootReplaySceneNativeSource::RestorePatternsAt4C4330(void* p)
{DrawLayersView(static_cast<DrawManagerNativeBody2Ch*>(p)).WindowZero().RestorePatternsAt4C4330(dependencies_.common.graphics,&dependencies_.patterns);}
void RootReplaySceneNativeSource::PropagatePatternsAt4C4250(void* p)
{DrawLayersView(static_cast<DrawManagerNativeBody2Ch*>(p)).WindowZero().PropagateDirtyRectsAt4C4250();}
void RootReplaySceneNativeSource::DrawRegisteredAt4AC490(void* p)
{DrawLayersView(static_cast<DrawManagerNativeBody2Ch*>(p)).RegisteredLayers().RenderDirtyRects(dependencies_.common.graphics);}
void RootReplaySceneNativeSource::ClearScreenAfter4182F0(unsigned,const RECT* rect)
{dependencies_.common.graphics.ClearScreenIfLoaded(rect);}
void RootReplaySceneNativeSource::ShowReplayError(EnvironmentRootStorage* root,const char* text)
{
    const auto* title=root->titleAt10.GetString();const auto window=reinterpret_cast<HWND>(std::uintptr_t(root->windowAt2C));
    static_cast<void>(dependencies_.messageBox(window,text,title,MB_ICONERROR));
}
}
