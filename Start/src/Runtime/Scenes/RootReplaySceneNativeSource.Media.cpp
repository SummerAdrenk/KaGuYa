#include <Runtime/RootReplaySceneNativeSource.h>

namespace KaGuYa::Start::Runtime
{
bool RootReplaySceneNativeSource::LoadSurfaceAt480000(void* p,YtlReferenceSlotRuntime& out,const char* name)
{return common_.LoadSurfaceAt480000(p,out,name);}
YtlReferenceSlotRuntime* RootReplaySceneNativeSource::CaptureScreenAt4ACDD0(void* p,YtlReferenceSlotRuntime* out)
{return common_.CaptureScreenAt4ACDD0(p,out);}
SIZE RootReplaySceneNativeSource::ReadScreenSizeAfter4182F0(unsigned site){return common_.ReadScreenSizeAfter4182F0(site);}
void RootReplaySceneNativeSource::ClearDirtyAfter4182F0(unsigned site){common_.ClearDirtyAfter4182F0(site);}
void RootReplaySceneNativeSource::InvalidateAfter4182F0(unsigned site,const RECT* rect){common_.InvalidateAfter4182F0(site,rect);}
void RootReplaySceneNativeSource::RenderAt449DD0(void* p,TransitOperation& op){common_.RenderAt449DD0(p,op);}
void RootReplaySceneNativeSource::DrawPageAfter4182F0(GraphicsSurfaceNativeBody* p,const RECT* rect){common_.DrawPageAfter4182F0(p,rect);}
void RootReplaySceneNativeSource::InvalidatedRectAt4AC316(const RECT* rect){common_.InvalidatedRectAt4AC316(rect);}
std::uint8_t RootReplaySceneNativeSource::LoadButtonAt480930(void* p,CgUiButtonNativeBody6Ch* button,const CgUiButtonRecordNative28h* row,int position)
{return common_.LoadButtonAt480930(p,button,row,position);}
void RootReplaySceneNativeSource::PlayMusicAt4BCCC0(void* p,RuntimeCString& map,const char* name,int value,int loop)
{common_.PlayMusicAt4BCCC0(p,map,name,value,loop);}
void RootReplaySceneNativeSource::PlayCueAt4BCD70(void* p,const char* name,int index,int value,std::uint8_t gate)
{common_.PlayCueAt4BCD70(p,name,index,value,gate);}
void RootReplaySceneNativeSource::OpenReaderAt483D90(ArchiveReaderReference& out,const char* name,unsigned site)
{common_.OpenReaderAt483D90(out,name,site);}
bool RootReplaySceneNativeSource::LoadSpriteAt480650(SpriteNativeBodyA0h& sprite,const ArchiveReaderReference& reader,std::uint8_t mode,unsigned arg,unsigned site)
{return common_.LoadSpriteAt480650(sprite,reader,mode,arg,site);}
bool RootReplaySceneNativeSource::LoadSpriteMemberAt4B0840(SpriteNativeBodyA0h& sprite,int member,const ArchiveReaderReference& reader,std::uint8_t mode,unsigned site)
{return common_.LoadSpriteMemberAt4B0840(sprite,member,reader,mode,site);}
}
