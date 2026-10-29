#include <Runtime/RootMusicSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/ReadScriptInputNative.h>
#include <Runtime/SceneManagerView.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootMusicSceneNativeSource::CaptureEnvironmentAt4180C0(unsigned){return dependencies_.environment.CaptureAt4180C0();}
InputRootNativeBody24h* RootMusicSceneNativeSource::CaptureInputAt418210(unsigned){return dependencies_.input.CaptureAt418210();}
void RootMusicSceneNativeSource::EnsureRootAt413BE0(unsigned){dependencies_.root.EnsureCreatedAt413BE0();}
void* RootMusicSceneNativeSource::RootAt6198C4(unsigned){return dependencies_.root.ReadPublishedAt6198C4();}
YtlReferenceSlotRuntime& RootMusicSceneNativeSource::RootDrawSlotAt18(void* p){return static_cast<RootRuntimePayload*>(p)->DrawManagerSlotAt18();}
YtlReferenceSlotRuntime& RootMusicSceneNativeSource::RootSoundSlotAt24(void* p){return static_cast<RootRuntimePayload*>(p)->SoundManagerSlotAt24();}
YtlReferenceSlotRuntime& RootMusicSceneNativeSource::RootScenesSlotAt30(void* p){return static_cast<RootRuntimePayload*>(p)->SceneManagerSlotAt30();}
void RootMusicSceneNativeSource::WriteRootByte(void* p,unsigned offset,std::uint8_t value)
{
    auto* root=static_cast<RootRuntimePayload*>(p);
    if(offset==0x218)root->WriteByteAt218(value);
    else if(offset==0x219)root->WriteByteAt219(value);
    else throw std::logic_error("Music scene supplied unknown root byte");
}
void RootMusicSceneNativeSource::ConfigureScenesAt4703D0(void* p,int mode)
{ConfigureSceneManagerNativeAt4703D0(*static_cast<SceneManagerNativeBody48h*>(p),mode,RequireSceneManagerNativeServices());}
void RootMusicSceneNativeSource::PushSceneAt4A9030(void* p,const char* name)
{SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()).PushActiveScene(name);}
void RootMusicSceneNativeSource::QueueSceneAt47F920(void* p,const char* name)
{SceneManagerView(static_cast<SceneManagerNativeBody48h*>(p),RequireSceneManagerNativeServices()).QueueScene(name);}
bool RootMusicSceneNativeSource::AccelerateAt4AA970(void*){return ReadScriptAccelerateInputAt4AA970(dependencies_.input)!=0;}
const YtlReferenceSlotRuntime* RootMusicSceneNativeSource::PublicGuiAt47B7B0(void* p)
{return BorrowModeButtonsNativeAt47B7B0(*static_cast<SceneManagerNativeBody48h*>(p));}
void RootMusicSceneNativeSource::SynchronizeButtonAt4ABB40(void* p)
{SynchronizeMusicButtonNativeAt4ABB40(*static_cast<MusicButtonNativeBody70h*>(p));}
void* RootMusicSceneNativeSource::ButtonItemAt4ACF20(void* p,int index)
{return ResolveModeButtonChildNativeAt4ACF20(static_cast<MusicButtonNativeBody70h*>(p)->common,index);}
}
