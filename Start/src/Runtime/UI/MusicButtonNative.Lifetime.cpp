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
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        result.deleteAt00=reinterpret_cast<decltype(result.deleteAt00)>(&Delete);
        result.processAt04=reinterpret_cast<decltype(result.processAt04)>(&Process);return result;
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
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
