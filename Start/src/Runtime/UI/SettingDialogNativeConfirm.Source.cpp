#include "SettingDialogNativeConfirm.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>

namespace KaGuYa::Start::Runtime
{
namespace{thread_local SettingDialogNativeConfirmServices* active=nullptr;}
SettingDialogNativeConfirmScope::SettingDialogNativeConfirmScope(SettingDialogNativeConfirmServices& s) noexcept
    :previous_(active),general_(s.general),names_(s.names){active=&s;}
SettingDialogNativeConfirmScope::~SettingDialogNativeConfirmScope() noexcept{active=previous_;}
SettingDialogNativeConfirmServices& RequireSettingDialogNativeConfirmServices()
{if(!active)throw std::logic_error("SettingDialog native Confirm services are not bound");return *active;}
GuiDialogNativeBody5Ch* SettingDialogNativeConfirmConstructors::GeneralAt4E3330(void* p,int mode)
{return ConstructGeneralConfirmDialogNativeAt4E3330(p,mode);}
NameRegistrationDialogNativeBody94h* SettingDialogNativeConfirmConstructors::NameAt4E4340(void* p)
{return ConstructNameRegistrationDialogNativeAt4E4340(p);}
namespace SettingDialogNativeConfirmDetail
{
void ReleaseCaptured(YtlReferenceControlRuntime* control) noexcept
{
    if(control&&--control->referenceCountAt04==0)
    {if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}
}
void ReleaseReloadRetain(Slot& slot) noexcept
{
    auto* control=Read(slot.controlAt08);if(!control)return;
    if(--control->referenceCountAt04==0)
    {
        if(control->owningAt08)control->DestroyPayloadAtVslot00();
        // 40D8DC/4F65EC均重读当前control。正常释放抛出进入terminate状态，不重试同一项。
        if(auto* current=Read(slot.controlAt08))current->DeleteControlAtVslot04(1);
    }
}
void ReleaseReloadClear(Slot& slot) noexcept
{
    if(!Read(slot.controlAt08))return;
    ReleaseReloadRetain(slot);
    slot.controlAt08=nullptr;slot.baseAt00=0; // 原不清bias，control=0入口则连base也不清。
}
void* Registry(unsigned index,unsigned site,bool ordinary)
{
    auto* root=RequireSettingDialogNativeConfirmServices().source.CaptureEnvironmentAt4180C0(site);
    if(ordinary)return ResolveEnvironmentArchiveAt417990(*root,index);
    const auto end=reinterpret_cast<std::uintptr_t>(Read(root->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(root->archivesAt30.begin));
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return index<count?Resolve<void>(*reinterpret_cast<const Slot*>(begin+12u*index)):nullptr;
}
void* ModalReceiver(unsigned site)
{
    auto& s=RequireSettingDialogNativeConfirmServices().source;s.EnsureRootAt413BE0(site);
    auto* root=s.RootAt6198C4(site);return Resolve<void>(s.RootSystemSlotAt3C(root));
}
int ModalAt5071D0(void* unusedThis,Slot& input)
{
    static_cast<void>(unusedThis); // 原5071D0不消费ECX，但caller取Root+3C的顺序/故障不能省略。
    GeneralConfirmDialogNativeModalTarget target(input); // 仅共用原35槽的四个Modal ABI，未共用确认业务。
    return ModalDialogRunnerRuntime(RequireSettingDialogNativeConfirmServices().modal).RunAt5071D0(target);
}
void FindDialog(Common* body,Slot& out,int id)
{
    auto* table=reinterpret_cast<const GuiDialogIntoNativeVtable37*>(Read(body->vptrAt00));
    table->findDialogAt90(body,&out,id); // hidden-result成功返回后caller才armed；忽略返回槽指针。
}
void ReloadPage(Common* page)
{
    using Fn=void(__thiscall*)(Common*);Fn fn;auto* table=Read(page->vptrAt00);
    std::memcpy(&fn,reinterpret_cast<const std::byte*>(table)+0x94,4);fn(page);
}
void AddBackgroundBounds(Body& b)
{
    // 保存第一次base，读width/height后用当前control/bias重解同一base；随后固定x/y。
    const auto base=Read(b.backgroundAt68.baseAt00);
    auto* first=ResolveWithBase<GraphicsSurfaceNativeBody>(b.backgroundAt68,base);
    const int width=Read(first->widthAt08),height=Read(first->heightAt0C);
    auto* second=ResolveWithBase<GraphicsSurfaceNativeBody>(b.backgroundAt68,base);
    const int x=Read(second->originXAt10),y=Read(second->originYAt14);
    auto* dirty=Read(b.common.vptrAt00)->dirtyAt00(&b.common);
    Read(dirty->vptrAt00)->addXYWHAt10(dirty,x,y,width,height);
}
}
}
