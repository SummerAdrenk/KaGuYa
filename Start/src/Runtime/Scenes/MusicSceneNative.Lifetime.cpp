#include "MusicSceneNative.Internal.h"
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace MusicSceneNativeDetail;
thread_local MusicSceneNativeServices* current=nullptr;
__declspec(naked) std::uint8_t __fastcall Run(Body*,void*,std::int32_t)
{
    __asm {
        push esi
        mov esi,ecx
        mov eax,[esi]
        call dword ptr[eax+8]
        mov eax,[esi]
        mov ecx,esi
        call dword ptr[eax+0Ch]
        mov al,1
        pop esi
        ret 4
    }
}
__declspec(naked) void __fastcall DrawEmpty(Body*,void*){__asm{_emit 0xC3}}
void __fastcall Update(Body* p,void*){UpdateMusicSceneNativeAt4B8C90(*p,RequireMusicSceneNativeServices());}
Body* __fastcall Delete(Body* p,void*,unsigned flags) noexcept{return DeleteMusicSceneNativeAt4A6DA0(p,flags);}
std::uint8_t __fastcall Pure(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body*,void*,unsigned) noexcept;
struct Retired final{decltype(MusicSceneNativeVtable4::runAt00) run;decltype(MusicSceneNativeVtable4::deleteAt04) destroy;};
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Retired retired{reinterpret_cast<decltype(Retired::run)>(&Pure),reinterpret_cast<decltype(Retired::destroy)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MusicSceneNativeVtable4 methods{
    reinterpret_cast<decltype(MusicSceneNativeVtable4::runAt00)>(&Run),reinterpret_cast<decltype(MusicSceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(MusicSceneNativeVtable4::updateAt08)>(&Update),reinterpret_cast<decltype(MusicSceneNativeVtable4::drawAt0C)>(&DrawEmpty)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* p,void*,unsigned flags) noexcept
{MusicSceneNativeDetail::Retire(*p);if(flags&1)::operator delete(p,std::size_t{4});return p;}
}
MusicSceneNativeScope::MusicSceneNativeScope(MusicSceneNativeServices& s) noexcept:previous_(current),surfaces_(s.surfaces)
{if(auto* buttons=s.buttons.NativeButtonServices())button_.emplace(*buttons);current=&s;}
MusicSceneNativeScope::~MusicSceneNativeScope() noexcept{current=previous_;}
MusicSceneNativeServices& RequireMusicSceneNativeServices()
{if(!current)throw std::logic_error("Music native scene services are required");return *current;}
const MusicSceneNativeVtable4& MusicSceneNativeMethods() noexcept{return methods;}
namespace MusicSceneNativeDetail
{
void Retire(Body& p) noexcept{p.vptrAt00=reinterpret_cast<const MusicSceneNativeVtable4*>(&retired);}
void RefLife::NormalTerminate() noexcept
{
    active=false;auto* control=Read(slot.controlAt08);
    if(control&&--control->referenceCountAt04==0)
    {if(control->owningAt08)control->DestroyPayloadAtVslot00();if(auto* now=Read(slot.controlAt08))now->DeleteControlAtVslot04(1);}
}
void* Root(IMusicSceneNativeSource& s,std::uint32_t site){s.EnsureRootAt413BE0(site);return s.RootAt6198C4(site);}
void* Scenes(IMusicSceneNativeSource& s,std::uint32_t site){return Resolve<void>(s.RootScenesSlotAt30(Root(s,site)));}
void* Draw(IMusicSceneNativeSource& s,std::uint32_t site){return Resolve<void>(s.RootDrawSlotAt18(Root(s,site)));}
void* Registry(IMusicSceneNativeSource& s,unsigned index,unsigned site,bool inlineLookup)
{
    auto* env=s.CaptureEnvironmentAt4180C0(site);if(!inlineLookup)return ResolveEnvironmentArchiveAt417990(*env,index);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.begin));
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<Slot*>(begin+index*12u)):nullptr;
}
}
void DestroyMusicSceneNativeAt4A6DA0(MusicSceneNativeBody64h& p) noexcept
{ReleaseYtlReference(p.backupAt54);ReleaseYtlReference(p.blackAt48);ReleaseYtlReference(p.buttonAt08);MusicSceneNativeDetail::Retire(p);}
MusicSceneNativeBody64h* DeleteMusicSceneNativeAt4A6DA0(MusicSceneNativeBody64h* p,unsigned flags) noexcept
{DestroyMusicSceneNativeAt4A6DA0(*p);if(flags&1)::operator delete(p,std::size_t{0x64});return p;}
}
