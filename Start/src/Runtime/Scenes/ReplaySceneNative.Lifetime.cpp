#include "ReplaySceneNative.Internal.h"
#include <Runtime/SceneTaskRunNative.h>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ReplaySceneNativeDetail;
thread_local ReplaySceneNativeServices* current=nullptr;
void __fastcall Update(Body* body,void*){UpdateReplaySceneNativeAt4B7CE0(*body,RequireReplaySceneNativeServices());}
Body* __fastcall Delete(Body* body,void*,unsigned flags)noexcept{return DeleteReplaySceneNativeAt4A6C60(body,flags);}
std::uint8_t __fastcall Pure(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept;
struct Retired final{decltype(ReplaySceneNativeVtable4::runAt00) run;decltype(ReplaySceneNativeVtable4::deleteAt04) destroy;};
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
const ReplaySceneNativeVtable4 methods{
    reinterpret_cast<decltype(ReplaySceneNativeVtable4::runAt00)>(&RunSceneTaskNativeAt4C3630),
    reinterpret_cast<decltype(ReplaySceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(ReplaySceneNativeVtable4::updateAt08)>(&Update),
    reinterpret_cast<decltype(ReplaySceneNativeVtable4::drawAt0C)>(&DrawSceneTaskNativeAt4B5E20)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept
{ReplaySceneNativeDetail::Retire(*body);if(flags&1u)::operator delete(body,std::size_t{4});return body;}
}
ReplaySceneNativeScope::ReplaySceneNativeScope(ReplaySceneNativeServices& services)noexcept
    :previous_(current),surfaces_(services.surfaces),params_(services.source),buttons_(services.buttons){current=&services;}
ReplaySceneNativeScope::~ReplaySceneNativeScope()noexcept{current=previous_;}
ReplaySceneNativeServices& RequireReplaySceneNativeServices()
{if(!current)throw std::logic_error("Replay native scene services are required");return *current;}
const ReplaySceneNativeVtable4& ReplaySceneNativeMethods()noexcept{return methods;}
void DrawReplaySceneNativeAt4B5E20(ReplaySceneNativeBody80h&)noexcept{}
namespace ReplaySceneNativeDetail
{
void Retire(Body& body)noexcept{body.vptrAt00=reinterpret_cast<const ReplaySceneNativeVtable4*>(&retired);}
ReplayButtonNativeBody84h* Group(Body& body)noexcept
{return Resolve<ReplayButtonNativeBody84h>(*Advance(Read(body.groupsAt1C.begin),Read(body.groupAt0C)));}
const ReplayButtonNativeVtable15* GroupMethods(ReplayButtonNativeBody84h& body)noexcept
{return reinterpret_cast<const ReplayButtonNativeVtable15*>(Read(body.common.vptrAt00));}
void CaptureScreen(Body& body,IReplaySceneNativeSource& source,unsigned site)
{
    auto* draw=Draw(source,site);Slot temporary;
    auto* returned=source.CaptureScreenAt4ACDD0(draw,&temporary);
    struct Prefix{Slot& slot;bool active=true;~Prefix()noexcept{if(active)ReleaseYtlReference(slot);}} prefix{temporary};
    CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt68,*returned);
    prefix.active=false;
    // 原state3/40D2F0的正常释放异常均terminate；不扩大到CopyAssign的可抛区间。
    const auto normal=[](Slot& slot)noexcept{ReleaseYtlReference(slot);};normal(temporary);
}
}
void DestroyReplaySceneNativeAt4A6C60(ReplaySceneNativeBody80h& body)noexcept
{
    ReleaseYtlReference(body.auxiliaryAt74);ReleaseYtlReference(body.capturedAt68);ReleaseYtlReference(body.blackAt5C);
    DestroySceneGroupReferencesNativeAt4A3950(body.groupsAt1C);ReplaySceneNativeDetail::Retire(body);
}
ReplaySceneNativeBody80h* DeleteReplaySceneNativeAt4A6C60(ReplaySceneNativeBody80h* body,unsigned flags)noexcept
{DestroyReplaySceneNativeAt4A6C60(*body);if(flags&1u)::operator delete(body,std::size_t{0x80});return body;}
}
