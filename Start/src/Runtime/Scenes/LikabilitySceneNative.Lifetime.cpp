#include "LikabilitySceneNative.Internal.h"
#include <Runtime/SceneTaskRunNative.h>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilitySceneNativeDetail;
thread_local LikabilitySceneNativeServices* active=nullptr;
void __fastcall Update(Body* body,void*){UpdateLikabilitySceneNativeAt4B6280(*body,RequireLikabilitySceneNativeServices());}
Body* __fastcall Delete(Body* body,void*,unsigned flags)noexcept{return DeleteLikabilitySceneNativeAt4A69B0(body,flags);}
std::uint8_t __fastcall Pure(Body*,void*,int){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept;
struct Retired{decltype(LikabilitySceneNativeVtable4::runAt00) run;decltype(LikabilitySceneNativeVtable4::deleteAt04) destroy;};
const Retired retired{reinterpret_cast<decltype(Retired::run)>(&Pure),reinterpret_cast<decltype(Retired::destroy)>(&DeleteTask)};
const LikabilitySceneNativeVtable4 methods{
    reinterpret_cast<decltype(LikabilitySceneNativeVtable4::runAt00)>(&RunSceneTaskNativeAt4C3630),
    reinterpret_cast<decltype(LikabilitySceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(LikabilitySceneNativeVtable4::updateAt08)>(&Update),
    reinterpret_cast<decltype(LikabilitySceneNativeVtable4::drawAt0C)>(&DrawSceneTaskNativeAt4B5E20)};
Body* __fastcall DeleteTask(Body* body,void*,unsigned flags)noexcept
{LikabilitySceneNativeDetail::Retire(*body);if(flags&1u)::operator delete(body,std::size_t{4});return body;}
}
LikabilitySceneNativeScope::LikabilitySceneNativeScope(LikabilitySceneNativeServices& services)noexcept
    :previous_(active),gui_(services.gui){active=&services;}
LikabilitySceneNativeScope::~LikabilitySceneNativeScope()noexcept{active=previous_;}
LikabilitySceneNativeServices& RequireLikabilitySceneNativeServices()
{if(!active)throw std::logic_error("Likability native scene services are required");return *active;}
const LikabilitySceneNativeVtable4& LikabilitySceneNativeMethods()noexcept{return methods;}
namespace LikabilitySceneNativeDetail
{
void Retire(Body& body)noexcept{body.vptrAt00=reinterpret_cast<const LikabilitySceneNativeVtable4*>(&retired);}
}
void DestroyLikabilitySceneNativeAt4A69B0(LikabilitySceneNativeBody20h& body)noexcept
{ReleaseYtlReference(body.dialogAt10);LikabilitySceneNativeDetail::Retire(body);}
LikabilitySceneNativeBody20h* DeleteLikabilitySceneNativeAt4A69B0(LikabilitySceneNativeBody20h* body,unsigned flags)noexcept
{DestroyLikabilitySceneNativeAt4A69B0(*body);if(flags&1u)::operator delete(body,std::size_t{0x20});return body;}
}
