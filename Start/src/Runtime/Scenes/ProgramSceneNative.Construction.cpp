#include "ProgramSceneNative.Internal.h"
#include <Runtime/ProgramProfileDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSceneNativeDetail;
template<int Mode>class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* p)noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00()override
    {auto* p=static_cast<GuiDialogNativeBody5Ch*>(Read(payloadAt14));if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(unsigned flags)override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(DialogControl<0>)==0x18&&sizeof(DialogControl<1>)==0x18);
char invalidMode[]="\x83\x5F\x83\x43\x83\x41\x83\x8D\x83\x4F\x82\xCC\x8E\xED\x97\xDE\x82\xAA\x91\xCE\x89\x9E\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB9\x82\xF1\x81\x49";
}
namespace ProgramSceneNativeDetail
{
void* InlineArchiveZero(EnvironmentRootStorage* env)noexcept
{
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.begin));
    const auto end=reinterpret_cast<std::uintptr_t>(Read(env->archivesAt30.end));
    if(std::bit_cast<int>(unsigned(end-begin))/12==0)return nullptr;
    auto& slot=*reinterpret_cast<Slot*>(begin);const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto bias=Read(slot.biasAt04);return reinterpret_cast<void*>(base+bias*(control?Read(control->strideAt0C):0u));
}
void AdoptDialog(Slot& slot,GuiDialogNativeBody5Ch* created,int mode,IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);if(!created)return;
    if(auto* old=Read(slot.controlAt08))old->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(created);
    auto* raw=memory.AllocateBody(0x18);EnvironmentObjectReferenceControl* control=nullptr;
    if(raw)control=mode==0?static_cast<EnvironmentObjectReferenceControl*>(::new(raw)DialogControl<0>(created)):
        static_cast<EnvironmentObjectReferenceControl*>(::new(raw)DialogControl<1>(created));
    slot.controlAt08=control;
    *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4)=1;
    Read(slot.controlAt08)->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C=0xEC;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10=1;
}
void AdoptConfiguredDialog(Slot& slot,GuiDialogNativeBody5Ch* created,IEnvironmentRootMemory& memory)
{
    if (!created) throw std::bad_alloc();
    struct Guard final
    {
        GuiDialogNativeBody5Ch* p;
        ~Guard() noexcept { if (p) p->vptrAt00->deletingDestructorAt04(p,1); }
    } guard{created};
    auto* raw=memory.AllocateBody(sizeof(DialogControl<1>));
    if (!raw) throw std::bad_alloc();
    auto* control=::new(raw) DialogControl<1>(created);
    control->referenceCountAt04=1;control->owningAt08=1;
    control->strideAt0C=static_cast<unsigned>(ProgramProfileDialogStorageSize());control->countAt10=1;
    slot={reinterpret_cast<std::uintptr_t>(created),0,control};
    guard.p=nullptr;
}
}
ProgramSceneNativeBody20h* ConstructProgramSceneNativeAt4A11E0(Body& b,ProgramSceneNativeServices& services)
{
    ProgramSceneNativeScope scope(services);auto& source=services.source;
    b.vptrAt00=&ProgramSceneNativeMethods();b.dialogAt08={};
    struct Members final{Body& body;bool active=true;~Members()noexcept{if(active){ReleaseYtlReference(body.dialogAt08);Retire(body);}}} members{b};
    const auto& profile=services.resources?*services.resources:CurrentProgramResourceProfile();
    b.modeAt1C=0;b.profileAt18=nullptr;
    if (profile.selection==ProgramModeSelection::NamedParameter)
    {
        auto* archive=InlineArchiveZero(source.CaptureEnvironmentAt4180C0(0x4A1233));
        auto* root=Root(source,0x4A127F,0x4A1284);
        const auto index=source.ResolveNamedAt498020(archive,profile.modeVariable,0x4A1291);
        b.modeAt1C=source.ReadValueAt4C73A0(source.ParamsAt47BDB0(root,0x4A1299),index,0x4A12A0);
    }
    const auto modeValue=Read(b.modeAt1C);
    if(profile.modes.size()==2 && profile.modes[0].implementation==ProgramDialogImplementation::MamatomoTraining &&
        profile.modes[1].implementation==ProgramDialogImplementation::MamatomoTrip && modeValue!=0 && modeValue!=1)
        throw static_cast<char*>(invalidMode); // Preserve the existing 18 failure contract.
    const auto& selected=SelectProgramMode(profile,modeValue);
    b.modeAt1C=selected.scriptValue;b.profileAt18=&selected;
    const bool configured=selected.implementation==ProgramDialogImplementation::Configured;
    const auto size=configured?ProgramProfileDialogStorageSize():std::size_t{0xEC};
    auto* raw=services.memory.AllocateBody(size);
    struct Raw final{void* p;std::size_t size;IEnvironmentRootMemory& memory;bool active=true;~Raw()noexcept{if(active)memory.DeleteBody(p,size);}} cleanup{raw,size,services.memory};
    GuiDialogNativeBody5Ch* gui=nullptr;
    if(raw)
    {
        if(configured)gui=source.ConstructProfileDialog(raw,selected);
        else if(selected.implementation==ProgramDialogImplementation::MamatomoTraining)gui=source.ConstructDialog00At4DE1E0(raw);
        else if(selected.implementation==ProgramDialogImplementation::MamatomoTrip)gui=source.ConstructDialog01At4DF0E0(raw);
        else throw static_cast<char*>(invalidMode);
    }
    cleanup.active=false;
    if(configured)AdoptConfiguredDialog(b.dialogAt08,gui,services.memory);
    else AdoptDialog(b.dialogAt08,gui,b.modeAt1C,services.memory);
    Field<unsigned>(Archive(source,2,0x4A1414),0x18)&=2;
    Field<std::uint8_t>(Archive(source,2,0x4A1426),0x0C)=0;
    b.savedAt14=Read(Field<unsigned>(Archive(source,2,0x4A1438),0x14));
    Field<unsigned>(Archive(source,2,0x4A144C),0x14)&=2;
    auto* root=Root(source,0x4A1461,0x4A1466);source.ParamsAt47BDB0(root,0x4A146C)->messageFormatIndexAt60=-1;
    root=Root(source,0x4A147D,0x4A1482);auto* script=source.ScriptAt47CD00(root,0x4A1488);
    script->commandAt04.flags=0;script->commandAt04.reload[0]=0;
    b.phaseAt04=0;members.active=false;return &b;
}
}
