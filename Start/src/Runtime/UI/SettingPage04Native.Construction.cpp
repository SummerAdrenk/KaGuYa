#include "SettingPage04Native.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace SettingPage04NativeDetail;
namespace
{
void InitializeContainer(Container& p,Common* parent,int id,unsigned kind)
{
    ConstructGuiDialogAt4E0BE0(&p.common,parent,id);p.dialogsAt5C={};p.storedAt68=1;
    p.common.vptrAt00=kind==2||kind==5?&SettingPage04ManagedNativeVtable(kind).common.common.common:&SettingPage04ContainerNativeVtable(kind).common.common;
}
struct ContainerUnwind final {Common* p;bool active=true;~ContainerUnwind() noexcept {if(active)DestroyGuiDialogIntoNativeAt4F7B80(p);}};
}
Container* ConstructSettingPage04Block00NativeAt4E8590(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) Container;InitializeContainer(*p,parent,id,0);ContainerUnwind unwind{&p->common};RECT bounds{};
    Populate(*p,SettingOtherBlockFor(SettingOtherBlock::Startup),nullptr,bounds);
    p->boundsAt6C=bounds;unwind.active=false;return p;
}
SettingPage04Block01NativeBody94h* ConstructSettingPage04Block01NativeAt4E8900(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) SettingPage04Block01NativeBody94h;InitializeContainer(p->container,parent,id,1);
    ContainerUnwind core{&p->container.common};p->textAt7C={};RefLife text{p->textAt7C};p->managerAt88={};RefLife manager{p->managerAt88};
    RECT bounds{};NewManager(p->managerAt88);
    Populate(p->container,SettingOtherBlockFor(SettingOtherBlock::Name),&p->managerAt88,bounds);
    p->container.boundsAt6C=bounds;manager.active=text.active=core.active=false;return p;
}
SettingPage04ManagedNativeBody88h* ConstructSettingPage04Block02NativeAt4E8EA0(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) SettingPage04ManagedNativeBody88h;InitializeContainer(p->container,parent,id,2);
    ContainerUnwind core{&p->container.common};p->managerAt7C={};RefLife manager{p->managerAt7C};RECT bounds{};
    NewManager(p->managerAt7C);
    const auto& profile=SettingOtherBlockFor(SettingOtherBlock::Effects);
    Populate(p->container,profile,&p->managerAt7C,bounds);
    p->container.boundsAt6C=bounds;
    for(const auto& slider:profile.sliders)
        if(auto* c=FindControl(&p->container.common,slider.eventAt00))
            SliderTable(c).setRangeAt5C(c,profile.sliderRange[0],profile.sliderRange[1]);
    manager.active=core.active=false;return p;
}
TextBlock* ConstructSettingPage04Block03NativeAt4E9450(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) TextBlock;ConstructGuiDialogAt4E0BE0(&p->block.common,parent,id);
    p->block.fieldAt5C=1;p->block.managerAt74={};p->block.common.vptrAt00=&SettingPage04Block03NativeVtable().common;
    struct Base final {Common* p;bool active=true;~Base() noexcept{if(active)DestroyRadio(p);}} base{&p->block.common};
    p->surfacesAt80={};
    struct Vector final{GuiControlSurfaceReferences& r;bool active=true;~Vector() noexcept{if(active)DestroyGuiControlSurfaceReferencesAt43EAA0(r);}} vector{p->surfacesAt80};
    RECT bounds{};p->block.boundsAt64={};if(BuildBlock03Text(*p,bounds))p->block.boundsAt64=bounds;
    vector.active=base.active=false;return p;
}
Container* ConstructSettingPage04Block04NativeAt4E9960(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) Container;InitializeContainer(*p,parent,id,4);ContainerUnwind unwind{&p->common};RECT bounds{};
    Populate(*p,SettingOtherBlockFor(SettingOtherBlock::SceneChoices),nullptr,bounds);
    p->boundsAt6C=bounds;unwind.active=false;return p;
}
SettingPage04ManagedNativeBody88h* ConstructSettingPage04Block05NativeAt4E9B90(void* raw,Common* parent,int id)
{
    auto* p=::new(raw) SettingPage04ManagedNativeBody88h;InitializeContainer(p->container,parent,id,5);
    ContainerUnwind core{&p->container.common};p->managerAt7C={};RefLife manager{p->managerAt7C};RECT bounds{};
    NewManager(p->managerAt7C);
    Populate(p->container,SettingOtherBlockFor(SettingOtherBlock::Countdown),&p->managerAt7C,bounds);
    p->container.boundsAt6C=bounds;manager.active=core.active=false;return p;
}
Parent* ConstructSettingPage04NativeAt4F2FE0(void* raw,Common* parent,int id)
{
    const auto& profile=CurrentSettingOtherProfile();
    if(profile.blocks.empty()||!profile.background)
        throw std::logic_error("This game has no Other settings page");
    auto* p=::new(raw) Parent;ConstructGuiDialogAt4E0BE0(&p->common,parent,id);p->dialogsAt5C={};p->footerAt78={};
    p->common.vptrAt00=&SettingPage04NativeVtable().common.common;
    struct Unwind final{Parent* p;bool active=true;~Unwind() noexcept{if(active)DestroySettingPage04NativeAt4F9840(p);}} unwind{p};
    RECT bounds{};constexpr unsigned sizes[]{0x7C,0x94,0x88,0x8C,0x7C,0x88};auto& s=RequireSettingPage04NativeServices();
    for(const auto& block:profile.blocks)
    {
        const auto kind=static_cast<unsigned>(block.kind);
        auto* childRaw=s.memory.AllocateBody(sizes[kind]);Common* created=nullptr;
        {
            struct Raw final{void* p;unsigned n;IEnvironmentRootMemory& m;bool active=true;~Raw() noexcept{if(active)m.DeleteBody(p,n);}} cleanup{childRaw,sizes[kind],s.memory};
            if(childRaw)switch(kind)
            {
            case 0:created=&ConstructSettingPage04Block00NativeAt4E8590(childRaw,&p->common,block.dialogId)->common;break;
            case 1:created=&ConstructSettingPage04Block01NativeAt4E8900(childRaw,&p->common,block.dialogId)->container.common;break;
            case 2:created=&ConstructSettingPage04Block02NativeAt4E8EA0(childRaw,&p->common,block.dialogId)->container.common;break;
            case 3:created=&ConstructSettingPage04Block03NativeAt4E9450(childRaw,&p->common,block.dialogId)->block.common;break;
            case 4:created=&ConstructSettingPage04Block04NativeAt4E9960(childRaw,&p->common,block.dialogId)->common;break;
            case 5:created=&ConstructSettingPage04Block05NativeAt4E9B90(childRaw,&p->common,block.dialogId)->container.common;break;
            }
            cleanup.active=false;
        }
        Fresh child;ConstructDialogRef(child.slot,created,kind,sizes[kind]);RefLife life{child.slot};auto* captured=Read(child.slot.controlAt08);
        GuiDialogIntoBaseVtable().appendDialogAt8C(&p->common,Copy(child.slot));RECT out;
        auto* returned=kind==3?RTable(created).boundsAt8C(created,&out):CTable(created).boundsAt94(created,&out);
        ::UnionRect(&bounds,&bounds,returned);life.Captured(captured);
    }
    Fresh footer;auto* returned=CreateGuiStaticAt505800(footer.slot,&p->common,*profile.background,s.loader,s.surfaces,s.strings);
    {
        RefLife life{footer.slot};if(&p->footerAt78!=returned){ReleaseYtlReference(p->footerAt78);CopyGuiClickedEventReference(p->footerAt78,*returned);}life.Normal();
    }
    auto* selected=Resolve<Control>(p->footerAt78);RECT out;auto* rectangle=ControlTable(selected).rectAt3C(selected,&out);::UnionRect(&bounds,&bounds,rectangle);
    p->boundsAt68=bounds;unwind.active=false;return p;
}
}
