#include "SaveLoadDialogNative.Internal.h"
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/SaveLoadPageSecondNative.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;using Common=GuiDialogNativeBody5Ch;
struct Temporary
{
    union{Slot slot;};bool armed=false;
    Temporary() noexcept{}
    ~Temporary() noexcept{if(armed)ReleaseYtlReference(slot);}
    void Find(Common& parent,int id)
    {
        const auto& table=*reinterpret_cast<const SaveLoadDialogNativeVtable37*>(parent.vptrAt00);
        table.findDialogAt90(&parent,&slot,id);armed=true;
    }
    void Finish() noexcept
    {armed=false;SaveLoadDialogNativeDetail::ReleaseWithoutClearing(slot);}
};
template<unsigned Kind,unsigned Size>
class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(Common* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {if(auto* body=static_cast<Common*>(payloadAt14))body->vptrAt00->deletingDestructorAt04(body,1);}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
template<unsigned Kind,unsigned Size>
void Create(Temporary& out,Common& parent,int id,ISaveLoadDialogNativePageFactory& factory)
{
    auto* raw=::operator new(Size);Common* created=nullptr;
    {
        struct RawUnwind{void* raw;bool armed=true;~RawUnwind() noexcept{if(armed)::operator delete(raw,std::size_t{Size});}} cleanup{raw};
        if(raw)
        {
            if constexpr(Kind==0)created=factory.ConstructPage00At4ED190(raw,&parent,id);
            if constexpr(Kind==1)created=factory.ConstructPage01At4EE390(raw,&parent,id);
            if constexpr(Kind==2)created=factory.ConstructSystemAt4F0C80(raw,&parent,id);
            if constexpr(Kind==3)created=factory.ConstructPopupAt4EF660(raw,&parent,id);
        }
        cleanup.armed=false; // 原完整正文没有随后new18失败的raw补删资格。
    }
    ReleaseYtlReference(out.slot);
    if(created)
    {
        if(out.slot.controlAt08)out.slot.controlAt08->DeleteControlAtVslot04(1);
        out.slot.biasAt04=0;out.slot.baseAt00=reinterpret_cast<std::uintptr_t>(created);
        auto* controlRaw=::operator new(0x18);
        auto* control=controlRaw?::new(controlRaw)Control<Kind,Size>(created):nullptr;
        out.slot.controlAt08=control;
        *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
        out.slot.controlAt08->owningAt08=1;
        static_cast<EnvironmentObjectReferenceControl*>(out.slot.controlAt08)->strideAt0C=Size;
        static_cast<EnvironmentObjectReferenceControl*>(out.slot.controlAt08)->countAt10=1;
    }
    Slot argument;CopyGuiClickedEventReference(argument,out.slot);
    const auto& table=*reinterpret_cast<const SaveLoadDialogNativeVtable37*>(parent.vptrAt00);
    table.appendDialogAt8C(&parent,argument); // caller不再补消费callee按值参数。
}
void WriteIfPresent(Common& parent,int id,int enabled)
{
    Temporary child;child.Find(parent,id);
    if(child.slot.baseAt00)
    {auto* body=ResolveEnvironmentObjectReference<Common>(child.slot);body->vptrAt00->writeEnabledAt24(body,enabled);}
    child.Finish();
}
void SetPageButtons(Common& system,int mode)
{
    // 535160独立查控件14/15/16，按!=1/2/3写state44，不写page保存或enabled字段。
    for(int id=14;id!=17;++id)
    {
        Temporary child;system.vptrAt00->findChildAt14(&system,&child.slot,id);child.armed=true;
        if(child.slot.baseAt00)
        {
            auto* body=ResolveGuiControl(child.slot);
            if(!CurrentSaveLoadResourceProfile().page.tabPositions.empty())body->vptrAt00->writeEnabledAt4C(body,mode!=id-13);
            else body->vptrAt00->writeStateAt44(body,mode!=id-13);
        }
        child.Finish();
    }
}
}
void InitializeSaveLoadDialogPageNativeAt5330A0(SaveLoadDialogNativeBody184h& body,int mode,ISaveLoadDialogNativePageFactory& factory)
{
    auto& parent=body.common;
    Temporary first;first.Find(parent,2);
    if(!mode)
    {
        if(!first.slot.baseAt00)Create<0,sizeof(SaveLoadPageSecondNativeBodyACh)>(first,parent,2,factory);
        auto* selected=ResolveEnvironmentObjectReference<Common>(first.slot);selected->vptrAt00->writeEnabledAt24(selected,1);
        first.Finish();
        for(int id=3;id!=6;++id)WriteIfPresent(parent,id,0);
        {
            Temporary system;system.Find(parent,0);
            if(!system.slot.baseAt00)Create<2,0x80>(system,parent,0,factory);
            selected=ResolveEnvironmentObjectReference<Common>(system.slot);selected->vptrAt00->writeEnabledAt24(selected,1);system.Finish();
        }
        WriteIfPresent(parent,1,0);
        {
            Temporary popup;popup.Find(parent,6);
            if(!popup.slot.baseAt00)Create<3,0xC0>(popup,parent,6,factory);
            selected=ResolveEnvironmentObjectReference<Common>(popup.slot);selected->vptrAt00->writeEnabledAt24(selected,0);popup.Finish();
        }
        WriteIfPresent(parent,7,0);
        return;
    }
    if(first.slot.baseAt00)
    {auto* selected=ResolveEnvironmentObjectReference<Common>(first.slot);selected->vptrAt00->writeEnabledAt24(selected,0);}
    first.Finish();
    for(int id=3;id!=6;++id)
    {
        Temporary page;page.Find(parent,id);
        if(!page.slot.baseAt00)Create<1,sizeof(SaveLoadPageSecondNativeBodyACh)>(page,parent,id,factory);
        auto* selected=ResolveEnvironmentObjectReference<Common>(page.slot);selected->vptrAt00->writeEnabledAt24(selected,id-3==mode-1);page.Finish();
    }
    WriteIfPresent(parent,0,0);
    {
        Temporary system;system.Find(parent,1);
        if(!system.slot.baseAt00)Create<2,0x80>(system,parent,1,factory);
        auto* selected=ResolveEnvironmentObjectReference<Common>(system.slot);selected->vptrAt00->writeEnabledAt24(selected,1);
        SetPageButtons(*selected,mode);system.Finish();
    }
    WriteIfPresent(parent,6,0);
    {
        Temporary popup;popup.Find(parent,7);
        if(!popup.slot.baseAt00)Create<3,0xC0>(popup,parent,7,factory);
        auto* selected=ResolveEnvironmentObjectReference<Common>(popup.slot);selected->vptrAt00->writeEnabledAt24(selected,0);popup.Finish();
    }
}
void OpenSaveLoadDialogChildrenNativeAt532560(SaveLoadDialogNativeBody184h& body)
{
    for(auto* at=body.dialogsAt5C.begin;at!=body.dialogsAt5C.end;++at)
    {auto* selected=ResolveEnvironmentObjectReference<Common>(*at);selected->vptrAt00->beginAt44(selected);}
}
}
