#include "LogDialogNative.Internal.h"
#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SpriteNativeStorage.h>
#include <cstdlib>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local LogDialogNativeServices* current=nullptr;
using namespace LogDialogNativeDetail;
class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Common* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<Common*>(payloadAt14);if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(std::uint32_t f) override{RetireEnvironmentObjectControl(this,f);}
};
class RowControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit RowControl(Row* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<Row*>(payloadAt14);if(p)Read(p->vptrAt00)->deleteAt00(p,1);}
    void DeleteControlAtVslot04(std::uint32_t f) override{RetireEnvironmentObjectControl(this,f);}
};
static_assert(sizeof(DialogControl)==0x18&&sizeof(RowControl)==0x18);
}
LogDialogNativeScope::LogDialogNativeScope(LogDialogNativeServices& s) noexcept
    :previous_(current),surfaces_(s.surfaces),graphics_(s.graphics),input_(s.input),screen_(s.screen){current=&s;}
LogDialogNativeScope::~LogDialogNativeScope() noexcept{current=previous_;}
LogDialogNativeServices& RequireLogDialogNativeServices()
{if(!current)throw std::logic_error("Log native services are required");return *current;}
GuiDialogNativeBody5Ch* LogDialogNativeConstruction::ConstructLogDialogAt4E23D0(void* raw)
{LogDialogNativeScope scope(services_);return ConstructLogDialogNativeAt4E23D0(raw);}
namespace LogDialogNativeDetail
{
void ReleaseCaptured(YtlReferenceControlRuntime* c)
{if(c&&--c->referenceCountAt04==0){if(c->owningAt08)c->DestroyPayloadAtVslot00();c->DeleteControlAtVslot04(1);}}
void RefLife::Normal(bool clear)
{
    active=false;auto* c=Read(slot.controlAt08);if(!c)return;
    if(--c->referenceCountAt04==0)
    {if(c->owningAt08)c->DestroyPayloadAtVslot00();if(auto* now=Read(slot.controlAt08))now->DeleteControlAtVslot04(1);}
    if(clear){slot.controlAt08=nullptr;slot.baseAt00=0;}
}
void RefLife::Captured(YtlReferenceControlRuntime* c,bool clear)
{
    active=false;if(!c)return;ReleaseCaptured(c);
    if(clear){slot.controlAt08=nullptr;slot.baseAt00=0;}
}
void CStringLife::Clone(const RuntimeCString& source)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())-16);
    auto* cloned=CloneRuntimeCStringDataAt414540(header);
    auto* chars=reinterpret_cast<char*>(cloned)+16;
    // Clone引用只按原生指针表示发布，保持active的清理资格切换。
    std::memcpy(static_cast<void*>(&value),&chars,4);active=true;
}
void* Root(std::uint32_t site)
{auto& s=RequireLogDialogNativeServices().source;s.EnsureRootAt413BE0(site);return s.RootAt6198C4(site);}
void* Registry(unsigned index,std::uint32_t site,bool ordinary)
{
    auto* env=RequireLogDialogNativeServices().source.CaptureEnvironmentAt4180C0(site);
    if(ordinary)return ResolveEnvironmentArchiveAt417990(*env,index);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.begin)); // 六个原inline站点均end-first；ordinary仍走417990。
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<Slot*>(begin+index*12u)):nullptr;
}
ScriptParamsNativeBody* Parameters(std::uint32_t site)
{auto& s=RequireLogDialogNativeServices().source;auto* root=Root(site);return s.ParamsAt47BDB0(root);}
SoundManagerNativeBody50h* Sound(std::uint32_t site)
{auto& s=RequireLogDialogNativeServices().source;auto* root=Root(site);return Resolve<SoundManagerNativeBody50h>(s.RootSoundSlotAt24(root));}
void AdoptDialog(Slot& slot,Common* p,std::uint32_t size)
{
    ReleaseYtlReference(slot);if(!p)return;
    if(auto* old=Read(slot.controlAt08))old->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=RequireLogDialogNativeServices().memory.AllocateBody(0x18);
    auto* c=raw?::new(raw) DialogControl(p):nullptr;slot.controlAt08=c;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(c)+4u)=1;
    c->owningAt08=1;c->strideAt0C=size;c->countAt10=1;
}
void AdoptRow(Slot& slot,Row* p)
{
    slot={};if(!p)return;slot.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    auto* raw=RequireLogDialogNativeServices().memory.AllocateBody(0x18);
    auto* c=raw?::new(raw) RowControl(p):nullptr;slot.controlAt08=c;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(c)+4u)=1;
    c->owningAt08=1;c->strideAt0C=0x30;c->countAt10=1;
}
void AppendRow(EnvironmentVectorStorage<Slot>& rows,const Slot& source)
{AppendGuiControlChildReference(rows,source);}
void DestroyRows(EnvironmentVectorStorage<Slot>& rows) noexcept
{DestroyGuiControlChildrenAt4F6C60(rows);}
void ResetRow(Row& p)
{
    p.formatAt10=-1;p.historyAt0C=-1;p.eventAt14=-1;
    ClearSpriteNameVectorAt43F240(p.voicesAt18);
    auto* surface=Resolve<GraphicsSurfaceNativeBody>(p.surfaceAt24);
    const auto handle=Read(surface->handleAt04);
    if(handle)RequireLogDialogNativeServices().drawing.SurfaceClearRaw(handle,Read(surface->clearColorAt20),nullptr);
}
}
void DestroyLogDialogNativeAt4F7EC0(LogDialogNativeBodyDCh* p) noexcept
{
    p->common.vptrAt00=&LogDialogNativeVtable();
    ReleaseYtlReference(p->reservedAt9C);ReleaseYtlReference(p->backgroundAt90);
    ReleaseYtlReference(p->scrollAt84);ReleaseYtlReference(p->textAt78);
    DestroyGuiDialogAt4F7AC0(&p->common);
}
LogDialogNativeBodyDCh* DeleteLogDialogNativeAt4FCA50(LogDialogNativeBodyDCh* p,std::uint32_t flags) noexcept
{DestroyLogDialogNativeAt4F7EC0(p);if(flags&1u)::operator delete(p,std::size_t{0xDC});return p;}
LogTextDialogNativeBodyA0h* DeleteLogTextDialogNativeAt4FC920(LogTextDialogNativeBodyA0h* p,std::uint32_t flags) noexcept
{
    using namespace LogDialogNativeDetail;p->common.vptrAt00=&LogTextDialogNativeVtable().common;
    ReleaseYtlReference(p->frameAt94);ReleaseYtlReference(p->voicesAt78);DestroyRows(p->rowsAt68);
    ReleaseYtlReference(p->eventsAt5C);DestroyGuiDialogAt4F7AC0(&p->common);
    if(flags&1u)::operator delete(p,std::size_t{0xA0});return p;
}
LogScrollDialogNativeBody80h* DeleteLogScrollDialogNativeAt4FE140(LogScrollDialogNativeBody80h* p,std::uint32_t flags) noexcept
{
    p->common.vptrAt00=&LogScrollDialogNativeVtable().common;ReleaseYtlReference(p->eventsAt74);
    DestroyGuiDialogAt4F7AC0(&p->common);if(flags&1u)::operator delete(p,std::size_t{0x80});return p;
}
LogTextNativeBody30h* DeleteLogTextNativeAt4FE870(LogTextNativeBody30h* p,std::uint32_t flags) noexcept
{
    p->vptrAt00=&LogTextNativeVtable();ReleaseYtlReference(p->surfaceAt24);
    DestroyRuntimeCStringVectorAt40DD20(p->voicesAt18);
    if(flags&1u)::operator delete(p,std::size_t{0x30});return p;
}
}
