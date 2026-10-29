#include "PropertyPageTextNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace {thread_local PropertyPageTextNativeServices* current;}
RootPropertyPageTextNativeSource::RootPropertyPageTextNativeSource(EnvironmentRootOwner& e,GraphicsRootOwner& g)noexcept
    :environment_(e),graphics_(g){}
EnvironmentRootStorage* RootPropertyPageTextNativeSource::CaptureEnvironmentAt4180C0(){return environment_.CaptureAt4180C0();}
GraphicsRootNativeBody1Ch* RootPropertyPageTextNativeSource::CaptureGraphicsAt4182F0(){return graphics_.CaptureAt4182F0();}
PropertyPageTextNativeScope::PropertyPageTextNativeScope(PropertyPageTextNativeServices& s)noexcept
    :previous_(current),endpoints_(s.endpoints),surfaces_(s.surfaces){current=&s;}
PropertyPageTextNativeScope::~PropertyPageTextNativeScope()noexcept{current=previous_;}
PropertyPageTextNativeServices& RequirePropertyPageTextNativeServices()
{if(!current)throw std::logic_error("Win32 property Text/MessWin requires explicit native services");return *current;}
}
namespace KaGuYa::Start::Runtime::PropertyPageTextDetail
{
void* InlineArchive(PropertyPageTextNativeServices& s,DWORD index,RefOrder refOrder,EnvOrder envOrder)
{
    auto* root=s.source.CaptureEnvironmentAt4180C0();
    YtlReferenceSlotRuntime* begin;DWORD end,subtractBegin;
    if(envOrder==EnvOrder::BeginEndBegin)
    {
        begin=Read(root->archivesAt30.begin);end=reinterpret_cast<DWORD>(Read(root->archivesAt30.end));
        subtractBegin=reinterpret_cast<DWORD>(Read(root->archivesAt30.begin));
    }
    else
    {
        end=reinterpret_cast<DWORD>(Read(root->archivesAt30.end));begin=Read(root->archivesAt30.begin);
        subtractBegin=reinterpret_cast<DWORD>(begin);
    }
    const auto count=static_cast<DWORD>(std::bit_cast<LONG>(end-subtractBegin)/12);
    if(index>=count)return nullptr;
    return Resolve<void>(begin[index],refOrder);
}
void* LookupArchive(PropertyPageTextNativeServices& s,DWORD index)
{return ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(),index);}
FARPROC GraphicsExport(const char* name,int length,PropertyPageTextNativeServices& s,bool& present)
{
    auto* root=s.source.CaptureGraphicsAt4182F0();
    const DWORD base=Read(root->moduleAt00.baseAt00);present=base!=0;
    if(!present)return nullptr;
    const auto* control=Read(root->moduleAt00.controlAt08);
    const DWORD stride=control?Read(Field<DWORD>(const_cast<YtlReferenceControlRuntime*>(control),0x0C)):0;
    const DWORD bias=Read(root->moduleAt00.biasAt04);
    auto* module=reinterpret_cast<GraphicsModuleNativeBody8h*>(base+bias*stride);
    return ResolveGraphicsModuleExportWithCString(module,name,length,s.strings,s.exports);
}
void ReleaseImageReference(YtlReferenceSlotRuntime& slot)
{
    auto* c=Read(slot.controlAt08);if(!c)return;
    if(--c->referenceCountAt04==0)
    {
        if(c->owningAt08)c->DestroyPayloadAtVslot00();
        c=Read(slot.controlAt08);if(c)c->DeleteControlAtVslot04(1);
    }
    slot.controlAt08=nullptr;slot.baseAt00=0; // bias不清；control空时连base也不清。
}
}
