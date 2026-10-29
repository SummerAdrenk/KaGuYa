#include "SettingDialogNativeApply.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
namespace{thread_local SettingDialogNativeApplyServices* active=nullptr;}
SettingDialogNativeApplyScope::SettingDialogNativeApplyScope(SettingDialogNativeApplyServices& s) noexcept
    :previous_(active),messageWindow_(s.messageWindow){active=&s;}
SettingDialogNativeApplyScope::~SettingDialogNativeApplyScope() noexcept{active=previous_;}
SettingDialogNativeApplyServices& RequireSettingDialogNativeApplyServices()
{if(!active)throw std::logic_error("SettingDialog native Apply services are not bound");return *active;}
namespace SettingDialogNativeApplyDetail
{
void* Registry(unsigned index,unsigned site,bool inlined,ArchiveOrder order)
{
    auto& source=RequireSettingDialogNativeApplyServices().source;
    auto* p=inlined?source.CaptureEnvironmentInlineAt6198C8(site):source.CaptureEnvironmentAt4180C0(site);
    if(order==ArchiveOrder::Ordinary)return ResolveEnvironmentArchiveAt417990(*p,index);
    std::uintptr_t end,begin;
    if(order==ArchiveOrder::EndFirst){end=reinterpret_cast<std::uintptr_t>(Read(p->archivesAt30.end));begin=reinterpret_cast<std::uintptr_t>(Read(p->archivesAt30.begin));}
    else{begin=reinterpret_cast<std::uintptr_t>(Read(p->archivesAt30.begin));end=reinterpret_cast<std::uintptr_t>(Read(p->archivesAt30.end));}
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<const Slot*>(begin+12u*index)):nullptr;
}
Params* SavedParams(unsigned site,bool inlined,ArchiveOrder order)
{return Resolve<Params>(Field<Slot>(Registry(1,site,inlined,order),0x1C));}
Sound* RootSound(unsigned site)
{
    auto& s=RequireSettingDialogNativeApplyServices().source;s.EnsureRootAt413BE0(site);
    auto* fixed=s.RootAt6198C4(site);return Resolve<Sound>(s.RootSoundSlotAt24(fixed));
}
DrawManagerNativeBody2Ch* RootDraw(unsigned site)
{
    auto& s=RequireSettingDialogNativeApplyServices().source;s.EnsureRootAt413BE0(site);
    auto* fixed=s.RootAt6198C4(site);return Resolve<DrawManagerNativeBody2Ch>(s.RootDrawSlotAt18(fixed));
}
FileManagerNative28h* RootFiles(unsigned site)
{
    auto& s=RequireSettingDialogNativeApplyServices().source;s.EnsureRootAt413BE0(site);
    auto* fixed=s.RootAt6198C4(site);return Resolve<FileManagerNative28h>(s.RootFileSlotAt00(fixed));
}
ParamsGameSystem* GameSystem(unsigned site,bool inlined)
{
    auto& source=RequireSettingDialogNativeApplyServices().source;
    auto* root=inlined?source.CaptureParamsInlineAt6198C0(site):source.CaptureParamsAt418480(site);
    return Resolve<ParamsGameSystem>(GameSystemSlot(root));
}
RuntimeCString* SnapshotAt(Body& b,unsigned index) noexcept
{return reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(Read(b.snapshotNamesAt84.begin))+index*4u);}
int SnapshotCount(Body& b) noexcept
{const auto begin=reinterpret_cast<std::uintptr_t>(Read(b.snapshotNamesAt84.begin));const auto end=reinterpret_cast<std::uintptr_t>(Read(b.snapshotNamesAt84.end));return std::bit_cast<int>(end-begin)>>2;}
void AssignMapping(RuntimeCString& destination,const RuntimeCString& source)
{
    // 51D50A/51DF04原内联410220：固定src/dst槽，Clone先于旧Free，失败不补新clone回收。
    AssignRuntimeCStringAt410220(destination,source);
}
bool NamesDifferInsensitive(const RuntimeCString& current,const char* original)
{
    if(!original)throw ATL::CAtlException(E_FAIL);
    return _mbsicmp(reinterpret_cast<const unsigned char*>(current.GetString()),reinterpret_cast<const unsigned char*>(original))!=0;
}
void SetDefaultName(const char* held,bool first,unsigned site)
{
    auto& s=RequireSettingDialogNativeApplyServices();auto* root=s.source.CaptureGraphicsAt4182F0(site);
    const auto base=Read(root->moduleAt00.baseAt00);if(!base)return; // 原模块base门，不加Initialized/handle门。
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(root->moduleAt00.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    auto* module=reinterpret_cast<GraphicsModuleNativeBody8h*>(base+Read(root->moduleAt00.biasAt04)*stride);
    // 418680内含40B530(GetNil/Set的constructor EH)，正文完成后GetProc/normal Free不再armed。
    // 两个固定导出名分别16/17字节；复用正式CString核，不跳过它的分配/释放副作用。
    const auto function=ResolveGraphicsModuleExportWithCString(module,first?"SetTextFirstName":"SetTextSecondName",
        first?16:17,s.strings,s.graphicsWin32);
    using Function=void(__cdecl*)(const char*);reinterpret_cast<Function>(function)(held);
}
}
}
