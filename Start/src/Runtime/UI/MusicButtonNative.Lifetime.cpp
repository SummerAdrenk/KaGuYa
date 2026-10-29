#include "MusicButtonNative.Internal.h"
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local MusicButtonNativeServices* current=nullptr;
ModeButtonNativeBody50h* __fastcall Delete(ModeButtonNativeBody50h* p,void*,unsigned flags) noexcept
{return reinterpret_cast<ModeButtonNativeBody50h*>(DeleteMusicButtonNativeAt4A70B0(reinterpret_cast<MusicButtonNativeBody70h*>(p),flags));}
int __fastcall Process(ModeButtonNativeBody50h* p,void*){return ProcessMusicButtonNativeAt4BE610(*reinterpret_cast<MusicButtonNativeBody70h*>(p));}
}
MusicButtonNativeScope::MusicButtonNativeScope(MusicButtonNativeServices& s) noexcept
    :previous_(current),mode_(s.mode),dialog_(s.dialogs.NativeDialogServices()){current=&s;}
MusicButtonNativeScope::~MusicButtonNativeScope() noexcept{current=previous_;}
MusicButtonNativeServices& RequireMusicButtonNativeServices()
{if(!current)throw std::logic_error("Music button native services are required");return *current;}
const ModeButtonNativeVtable13& MusicButtonNativeVtable() noexcept
{
    static const auto table=[]
    {
        auto result=IModeBtnProcNativeVtable();
        result.deleteAt00=reinterpret_cast<decltype(result.deleteAt00)>(&Delete);
        result.processAt04=reinterpret_cast<decltype(result.processAt04)>(&Process);return result;
    }();return table;
}
MusicButtonNativeBody70h* DeleteMusicButtonNativeAt4A70B0(MusicButtonNativeBody70h* p,unsigned flags) noexcept
{
    ReleaseYtlReference(p->reservedAt5C);ReleaseYtlReference(p->dialogAt50);
    ReleaseYtlReference(p->common.publicGuiAt44);DestroyButtonBaseNativeAt4A4BB0(p->common);
    if(flags&1)::operator delete(p,std::size_t{0x70});return p;
}
namespace MusicButtonNativeDetail
{
void* Root(IMusicButtonNativeSource& s,unsigned site){s.EnsureRootAt413BE0(site);return s.RootAt6198C4(site);}
void* Scenes(IMusicButtonNativeSource& s,unsigned site){return Resolve<void>(s.RootScenesSlotAt30(Root(s,site)));}
void* Sound(IMusicButtonNativeSource& s,unsigned site){return Resolve<void>(s.RootSoundSlotAt24(Root(s,site)));}
void* Registry(IMusicButtonNativeSource& s,unsigned index,unsigned site,bool inlineLookup)
{
    auto* env=s.CaptureEnvironmentAt4180C0(site);if(!inlineLookup)return ResolveEnvironmentArchiveAt417990(*env,index);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.begin));
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<Slot*>(begin+index*12u)):nullptr;
}
}
}
