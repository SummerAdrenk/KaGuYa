#include "CgSceneNative.Internal.h"
#include <Runtime/SceneTaskRunNative.h>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace CgSceneNativeDetail;
thread_local CgSceneNativeServices* current=nullptr;
void __fastcall Update(Body* p,void*){UpdateCgSceneNativeAt4B6A60(*p,RequireCgSceneNativeServices());}
void __fastcall DrawFrame(Body* p,void*){DrawCgSceneNativeAt4B5970(*p,RequireCgSceneNativeServices());}
Body* __fastcall Delete(Body* p,void*,unsigned flags) noexcept{return DeleteCgSceneNativeAt4A6C30(p,flags);}
std::uint8_t __fastcall Pure(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* p,void*,unsigned flags) noexcept;
struct Retired final{decltype(CgSceneNativeVtable4::runAt00) run;decltype(CgSceneNativeVtable4::deleteAt04) destroy;};
const Retired retired{reinterpret_cast<decltype(Retired::run)>(&Pure),reinterpret_cast<decltype(Retired::destroy)>(&DeleteTask)};
const CgSceneNativeVtable4 methods{
    reinterpret_cast<decltype(CgSceneNativeVtable4::runAt00)>(&RunSceneTaskNativeAt4C3630),reinterpret_cast<decltype(CgSceneNativeVtable4::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(CgSceneNativeVtable4::updateAt08)>(&Update),reinterpret_cast<decltype(CgSceneNativeVtable4::drawAt0C)>(&DrawFrame)};
Body* __fastcall DeleteTask(Body* p,void*,unsigned flags) noexcept
{CgSceneNativeDetail::Retire(*p);if(flags&1u)::operator delete(p,std::size_t{4});return p;}
}
CgSceneNativeScope::CgSceneNativeScope(CgSceneNativeServices& s) noexcept
    :previous_(current),surfaces_(s.surfaces),params_(s.source),buttons_(s.buttons),display_(s.display),operation_(s.operation.dialog)
{current=&s;}
CgSceneNativeScope::~CgSceneNativeScope() noexcept{current=previous_;}
CgSceneNativeServices& RequireCgSceneNativeServices()
{if(!current)throw std::logic_error("CG native scene services are required");return *current;}
const CgSceneNativeVtable4& CgSceneNativeMethods() noexcept{return methods;}
namespace CgSceneNativeDetail
{
void Retire(Body& p) noexcept{p.vptrAt00=reinterpret_cast<const CgSceneNativeVtable4*>(&retired);}
void* Root(ICgSceneNativeSource& s,unsigned site,unsigned readSite)
{s.EnsureRootAt413BE0(site);return s.RootAt6198C4(readSite?readSite:site+5u);}
void* Draw(ICgSceneNativeSource& s,unsigned site){return Resolve<void>(s.RootDrawSlotAt18(Root(s,site)));}
void* Scenes(ICgSceneNativeSource& s,unsigned site,unsigned readSite){return Resolve<void>(s.RootScenesSlotAt30(Root(s,site,readSite)));}
void* Registry(ICgSceneNativeSource& s,unsigned index,unsigned site,bool inlineLookup)
{
    auto* env=s.CaptureEnvironmentAt4180C0(site);
    if(!inlineLookup)return ResolveEnvironmentArchiveAt417990(*env,index);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.begin));
    const auto count=unsigned(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<Slot*>(begin+index*12u)):nullptr;
}
void ResetOperation(TransitOperation& operation) noexcept
{
    auto& p=static_cast<volatile TransitOperation&>(operation);
    p.destination=0;p.previous=0;p.source=nullptr;p.mask=0;p.y=0;p.x=0;
    p.clip=nullptr;p.opcode=-1;p.targetKind=0;p.current=0;p.target=255;p.range=0;
}
void ReleaseTemporaryRetained(Slot& slot) noexcept
{
    auto* c=Read(slot.controlAt08);
    if(c&&--c->referenceCountAt04==0)
    {if(c->owningAt08)c->DestroyPayloadAtVslot00();if(auto* now=Read(slot.controlAt08))now->DeleteControlAtVslot04(1);}
}
void ReleaseTemporaryCaptured(YtlReferenceControlRuntime* c) noexcept
{if(c&&--c->referenceCountAt04==0){if(c->owningAt08)c->DestroyPayloadAtVslot00();c->DeleteControlAtVslot04(1);}}
void CaptureScreen(Body& body,ICgSceneNativeSource& source,unsigned site,bool inlineNormal)
{
    auto* draw=Draw(source,site);Slot temporary;
    auto* returned=source.CaptureScreenAt4ACDD0(draw,&temporary);
    struct Unwind final{Slot& slot;bool active=true;~Unwind() noexcept{if(active)ReleaseYtlReference(slot);}} cleanup{temporary};
    CopyAssignGraphicsSurfaceReferenceAt465870(body.capturedAt84,*returned);
    cleanup.active=false;
    // 4B6B94内联清槽与4B6CC2/40D2F0均为terminate资格；不扩大到CopyAssign正常异常。
    (void)inlineNormal;
    const auto normal=[](Slot& slot) noexcept{ReleaseYtlReference(slot);};normal(temporary);
}
}
void DestroyCgSceneNativeAt4A4720(Body& body) noexcept
{
    // 4A4720一直保留派生表；只有最后4A48F0退役到ITask。
    ReleaseYtlReference(body.operationWindowAtA8);ReleaseYtlReference(body.displayWindowAt9C);
    ReleaseYtlReference(body.auxiliaryAt90);ReleaseYtlReference(body.capturedAt84);ReleaseYtlReference(body.blackAt78);
    CgSceneNativeDetail::DestroyGroups(body.groupsAt38);
    DestroyCgSelectionNativeAt4A4F40(body.selectionAt18);CgSceneNativeDetail::Retire(body);
}
Body* DeleteCgSceneNativeAt4A6C30(Body* body,unsigned flags) noexcept
{DestroyCgSceneNativeAt4A4720(*body);if(flags&1u)::operator delete(body,std::size_t{0xB4});return body;}
}
