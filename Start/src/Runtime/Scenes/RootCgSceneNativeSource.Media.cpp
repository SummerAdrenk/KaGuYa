#include <Runtime/RootCgSceneNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <Runtime/SpriteResourceNativeLoad.h>
#include <Runtime/SpriteFileGroupNativeLoad.h>

namespace KaGuYa::Start::Runtime
{
bool RootCgSceneNativeSource::LoadSurfaceAt480000(void* p,YtlReferenceSlotRuntime& out,const char* name)
{return LoadGuiControlSurfaceReferenceAt480000(out,p,name,dependencies_.pictures,dependencies_.graphics,dependencies_.strings);}
YtlReferenceSlotRuntime* RootCgSceneNativeSource::CaptureScreenAt4ACDD0(void*,YtlReferenceSlotRuntime* out)
{CaptureGraphicsScreenAt4ACDD0(*out,dependencies_.graphics,dependencies_.graphics,dependencies_.graphics);return out;}
SIZE RootCgSceneNativeSource::ReadScreenSizeAfter4182F0(unsigned){return dependencies_.graphics.ReadScreenSizeAt47CC00();}
void RootCgSceneNativeSource::ClearDirtyAfter4182F0(unsigned){dependencies_.graphics.ClearDirtyRectIfLoaded();}
void RootCgSceneNativeSource::InvalidateAfter4182F0(unsigned,const RECT* rect)
{auto* root=dependencies_.graphics.CaptureAt4182F0();dependencies_.graphics.InvalidatedRectCapturedAt4B02E0(*root,rect);}
void RootCgSceneNativeSource::RenderAt449DD0(void* p,TransitOperation& operation)
{TransitGraphicsScope scope(dependencies_.graphics);static_cast<void>(static_cast<TransitRuntime*>(p)->DispatchRawAt449DD0(operation));}
void RootCgSceneNativeSource::DrawPageAfter4182F0(GraphicsSurfaceNativeBody* p,const RECT* rect)
{
    const auto handle=static_cast<const volatile GraphicsSurfaceNativeBody&>(*p).handleAt04;
    static_cast<void>(dependencies_.graphics.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,rect,0));
}
void RootCgSceneNativeSource::InvalidatedRectAt4AC316(const RECT* rect){dependencies_.graphics.InvalidatedRectIfLoaded(rect);}
std::uint8_t RootCgSceneNativeSource::LoadButtonAt480930(void* p,CgUiButtonNativeBody6Ch* body,const CgUiButtonRecordNative28h* record,int position)
{return records_.LoadAt480930(p,body,record,position);}
void RootCgSceneNativeSource::PlayMusicAt4BCCC0(void* p,RuntimeCString& mapping,const char* name,int value,int loop)
{static_cast<void>(dependencies_.root.NativeSoundServices().PlayMusicCapturedAt4BCCC0(static_cast<SoundManagerNativeBody50h*>(p),mapping,name,value,loop));}
void RootCgSceneNativeSource::PlayCueAt4BCD70(void* p,const char* name,int slot,int value,std::uint8_t check)
{static_cast<void>(dependencies_.root.NativeSoundServices().PlayCueAt4BCD70(static_cast<SoundManagerNativeBody50h*>(p),name,std::uint32_t(slot),value,check));}
void RootCgSceneNativeSource::OpenReaderAt483D90(ArchiveReaderReference& fresh,const char* name,unsigned)
{
    ArchiveReaderFactoryRuntime factory(archives_,dependencies_.transaction,dependencies_.files,dependencies_.readers,dependencies_.codecs);
    const auto transfer=[&]()
    {auto slot=factory.DetachReaderSlot();fresh=ArchiveReaderReference::Adopt(slot);};
    // false不另造空reader；异常也转交已发布的原Ref，外层4A84E0按取得的FI资格退役。
    try{static_cast<void>(factory.LoadBorrowedAt483D90(name,dependencies_.key));}
    catch(...){transfer();throw;}
    transfer();
}
bool RootCgSceneNativeSource::LoadSpriteAt480650(SpriteNativeBodyA0h& sprite,const ArchiveReaderReference& reader,
    std::uint8_t initialization,unsigned argument,unsigned)
{return LoadSpriteArchiveNativeAt480650(sprite,reader,initialization,argument,pictures_,dependencies_.graphics,dependencies_.strings,&pictureServices_.inputMode);}
bool RootCgSceneNativeSource::LoadSpriteMemberAt4B0840(SpriteNativeBodyA0h& sprite,int member,const ArchiveReaderReference& reader,
    std::uint8_t initialization,unsigned)
{return LoadSpriteFileGroupNativeAt4B0840(sprite,member,reader,initialization,pictures_,dependencies_.graphics,dependencies_.strings,&pictureServices_.inputMode);}
}
