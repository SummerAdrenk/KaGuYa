#include "SaveLoadPageNative.Internal.h"
#include <Runtime/SaveLoadPageSlotNative.h>
#include <Runtime/SaveLoadResourceProfile.h>
#include <Runtime/SaveFileCatalog.h>
#include <algorithm>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageNativeDetail;
template<unsigned Size>
class FactoryControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit FactoryControl(SaveFileDeleteFactoryNativeBody0Ch* body) noexcept
        :EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* body=static_cast<SaveFileDeleteFactoryNativeBody0Ch*>(payloadAt14);
        if(body)body->vptrAt00->deletingDestructorAt00(body,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
template<unsigned Size>
void PublishFactory(Slot& output,SaveFileDeleteFactoryNativeBody0Ch* body)
{
    ReleaseYtlReference(output);
    if(!body)return;
    output.baseAt00=reinterpret_cast<std::uintptr_t>(body);output.biasAt04=0;
    auto* raw=::operator new(0x18);auto* control=raw?::new(raw)FactoryControl<Size>(body):nullptr;
    output.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    output.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C=Size;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10=1;
}
struct RawUnwind
{
    void* raw;std::size_t size;bool armed=true;
    ~RawUnwind() noexcept{if(armed)::operator delete(raw,size);}
};
void RegularFactory(Slot& output,std::int32_t page,SaveLoadPageNativeServices& services)
{
    auto* raw=::operator new(0x0C);SaveFileDeleteFactoryNativeBody0Ch* body=nullptr;
    {
        RawUnwind lifetime{raw,0x0C};
        if(raw)
        {body=::new(raw)SaveFileDeleteFactoryNativeBody0Ch;ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(*body,page,services.factory);}
        lifetime.armed=false;
    }
    PublishFactory<0x0C>(output,body);
}
void RollingFactory(Slot& output,bool automatic,SaveLoadPageNativeServices& services)
{
    auto* raw=::operator new(0x10);SaveFileDeleteFactoryNativeRollingBody10h* body=nullptr;
    {
        RawUnwind lifetime{raw,0x10};
        if(raw)
        {body=::new(raw)SaveFileDeleteFactoryNativeRollingBody10h;ConstructRollingSaveFileDeleteFactoryNativeAt45B800(*body,automatic,services.factory);}
        lifetime.armed=false;
    }
    PublishFactory<0x10>(output,body?&body->base:nullptr);
}
}
std::int32_t ChangeSaveLoadPage00NativeAt5339C0(Common* common,std::int32_t page,std::int32_t notify,std::int32_t force)
{
    auto& body=Page(common);
    if(CurrentSaveLoadResourceProfile().catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)
        page=std::clamp(page,0,static_cast<int>(CaptureSave(RequireSaveLoadPageNativeServices())->RegularPages().size())-1);
    if(!force&&Read(body.pageAt7C)==page)return 0;
    auto& services=RequireSaveLoadPageNativeServices();
    DrawSaveLoadPageNumbers(body,page,services);
    Slot factory{};LocalReference factoryLifetime{factory};
    RegularFactory(factory,SaveFilePhysicalPageNumber(*CaptureSave(services),page),services);auto* const factoryControl=factory.controlAt08;
    const auto latestPage=CaptureSave(services)->regularLatestPageAt18;
    for(std::uint32_t slot=0;slot<static_cast<unsigned>(CurrentSaveLoadResourceProfile().catalog.slotsPerPage);++slot)
    {
        const auto marker=latestPage==page?(CaptureSave(services)->regularLatestItemAt1C==std::int32_t(slot)):false;
        Slot ownedFactory=Copy(factory); // 原factory实参先复制，随后page参数才构造。
        LocalReference factoryArgument{ownedFactory};
        const auto& selected=Read(CaptureSave(services)->regularPagesAt20.begin)[page];
        Slot ownedPage=Copy(selected);factoryArgument.armed=false;
        RenderSaveLoadPageSlotNativeAt52A470(body,page,slot,ownedPage,ownedFactory,marker,services.slots);
    }
    factoryLifetime.Captured(factoryControl,false);
    WriteSaveLoadPageButtons(*common,page);body.pageAt7C=page;
    RefreshSaveLoadPageHoverNative(body,services.slots,true);
    if(notify)NotifySaveLoadPage(*common,services);
    // 原EAX为最后叶的偶然结果，所有已核caller忽略；不把它定义为成功标志。
    return 0;
}
std::int32_t ChangeSaveLoadPage01NativeAt5345A0(Common* common,std::int32_t page,std::int32_t notify,std::int32_t force)
{
    auto& body=Page(common);
    if(common->vptrAt00->idAt0C(common)==3&&CurrentSaveLoadResourceProfile().catalog.kind==SaveLoadCatalogKind::ExistingPhysicalPages)
        page=std::clamp(page,0,static_cast<int>(CaptureSave(RequireSaveLoadPageNativeServices())->RegularPages().size())-1);
    if(!force&&Read(body.pageAt7C)==page)return 0;
    auto& services=RequireSaveLoadPageNativeServices();
    const auto id=common->vptrAt00->idAt0C(common);
    if(id==3)DrawSaveLoadPageNumbers(body,page,services);
    Slot selectedPage{},factory{};LocalReference pageLifetime{selectedPage},factoryLifetime{factory};
    bool pageIsLatest=true;const std::int32_t* latestItem;
    YtlReferenceControlRuntime* pageControl;YtlReferenceControlRuntime* factoryControl;
    // 原534B95第二次查询决定factory；入口5345EA的id只供画布和尾部按钮判断。
    const auto factoryId=common->vptrAt00->idAt0C(common);
    if(factoryId==4)
    {
        CopyGuiClickedEventReference(selectedPage,CaptureSave(services)->quickPageAt2C);
        pageControl=selectedPage.controlAt08;
        RollingFactory(factory,false,services);factoryControl=factory.controlAt08;
        latestItem=&CaptureSave(services)->quickLatestItemAt04;
    }
    else if(factoryId==5)
    {
        CopyGuiClickedEventReference(selectedPage,CaptureSave(services)->autoPageAt38);
        pageControl=selectedPage.controlAt08;
        RollingFactory(factory,true,services);factoryControl=factory.controlAt08;
        latestItem=&CaptureSave(services)->autoLatestItemAt08;
    }
    else
    {
        CopyGuiClickedEventReference(selectedPage,Read(CaptureSave(services)->regularPagesAt20.begin)[page]);
        pageControl=selectedPage.controlAt08;
        RegularFactory(factory,SaveFilePhysicalPageNumber(*CaptureSave(services),page),services);
        factoryControl=factory.controlAt08;
        pageIsLatest=CaptureSave(services)->regularLatestPageAt18==page;
        latestItem=&CaptureSave(services)->regularLatestItemAt1C;
    }
    for(std::uint32_t slot=0;slot<static_cast<unsigned>(CurrentSaveLoadResourceProfile().catalog.slotsPerPage);++slot)
    {
        const auto physicalSlot=CurrentSaveLoadResourceProfile().catalog.rollingNewestFirst&&(id==4||id==5)?
            SaveFileRollingSlot(*CaptureSave(services),id==5,static_cast<int>(slot)):static_cast<int>(slot);
        const auto marker=pageIsLatest&&Read(*latestItem)==physicalSlot;
        Slot ownedFactory=Copy(factory);LocalReference argument{ownedFactory};
        Slot ownedPage=Copy(selectedPage);argument.armed=false;
        RenderSaveLoadPageSlotNativeAt52A470(body,page,slot,ownedPage,ownedFactory,marker,services.slots);
    }
    factoryLifetime.Captured(factoryControl,false);pageLifetime.Captured(pageControl,false);
    if(id==3)WriteSaveLoadPageButtons(*common,page);
    body.pageAt7C=page;RefreshSaveLoadPageHoverNative(body,services.slots,true);
    if(notify)NotifySaveLoadPage(*common,services);
    return 0;
}
}
