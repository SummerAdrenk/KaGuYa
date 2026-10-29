#include <Runtime/RootMusicSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/MusicResourceProfile.h>

namespace KaGuYa::Start::Runtime
{
bool RootMusicSceneNativeSource::LoadSurfaceAt480000(void* p,YtlReferenceSlotRuntime& out,const char* name)
{return LoadGuiControlSurfaceReferenceAt480000(out,p,name,dependencies_.pictures,dependencies_.graphics,dependencies_.strings);}
YtlReferenceSlotRuntime* RootMusicSceneNativeSource::CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;}
void RootMusicSceneNativeSource::ClearScreenAfter4182F0(unsigned,const RECT* r){dependencies_.graphics.ClearScreenIfLoaded(r);}
void RootMusicSceneNativeSource::ClearDirtyAfter4182F0(unsigned){dependencies_.graphics.ClearDirtyRectIfLoaded();}
void RootMusicSceneNativeSource::InvalidateAfter4182F0(unsigned,const RECT* r)
{auto* fixed=dependencies_.graphics.CaptureAt4182F0();dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*fixed,r);}
void RootMusicSceneNativeSource::RenderAt449DD0(void* p,TransitOperation& operation)
{TransitGraphicsScope scope(dependencies_.graphics);static_cast<void>(static_cast<TransitRuntime*>(p)->DispatchRawAt449DD0(operation));}
void RootMusicSceneNativeSource::DrawSurfaceAt441980(GraphicsSurfaceNativeBody* p,int x,int y,const RECT* clip)
{
    const auto handle=static_cast<volatile GraphicsSurfaceNativeBody&>(*p).handleAt04;
    if(handle)static_cast<void>(dependencies_.graphics.SurfaceDrawRaw(handle,x,y,nullptr,nullptr,clip,0));
}
void RootMusicSceneNativeSource::InvalidateRectAfter4182F0(const RECT* r){dependencies_.graphics.InvalidatedRectIfLoaded(r);}
void RootMusicSceneNativeSource::StopSoundAt4383B0(void* p,unsigned slot)
{dependencies_.root.NativeSoundServices().Wait(static_cast<SoundManagerNativeBody50h*>(p)).StopAt4383B0(slot);}
void RootMusicSceneNativeSource::PlayMusicAt4BCCC0(void* p,RuntimeCString& mapping,const char* name,int value,int loop)
{static_cast<void>(dependencies_.root.NativeSoundServices().PlayMusicCapturedAt4BCCC0(static_cast<SoundManagerNativeBody50h*>(p),mapping,name,value,loop));}
void RootMusicSceneNativeSource::PlayCueAt4BCD70(void* p,const char* name,int slot,int value,std::uint8_t check)
{static_cast<void>(dependencies_.root.NativeSoundServices().PlayCueAt4BCD70(static_cast<SoundManagerNativeBody50h*>(p),name,std::uint32_t(slot),value,check));}
const char* RootMusicSceneNativeSource::ExitNameAt61A56C()
{
    // 01–15原CRT为キャンセル音.sed；16–18使用带命名空间的系统音。
    const auto* configured=GameMusicResourceProfile().exitCue;
    return configured?configured:dependencies_.exitNameAt61A56C.GetString();
}
}
