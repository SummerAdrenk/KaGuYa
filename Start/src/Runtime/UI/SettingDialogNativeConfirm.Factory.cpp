#include "SettingDialogNativeConfirm.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingDialogNativeConfirmDetail;
class GeneralControl18h final:public EnvironmentObjectReferenceControl
{
public:
    explicit GeneralControl18h(Common* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<Common*>(payloadAt14);if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(unsigned flags) override{RetireEnvironmentObjectControl(this,flags);}
};
class NameControl18h final:public EnvironmentObjectReferenceControl
{
public:
    explicit NameControl18h(NameRegistrationDialogNativeBody94h* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<NameRegistrationDialogNativeBody94h*>(payloadAt14);if(p)Read(p->common.vptrAt00)->deletingDestructorAt04(&p->common,1);}
    void DeleteControlAtVslot04(unsigned flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(GeneralControl18h)==0x18&&sizeof(NameControl18h)==0x18);
struct RawLife final
{
    void* p;unsigned size;IEnvironmentRootMemory& memory;bool armed=true;
    ~RawLife() noexcept{if(armed)memory.DeleteBody(p,size);}
};
}
YtlReferenceSlotRuntime* CreateSettingDialogGeneralConfirmNativeReference(Slot& out,int mode)
{
    auto& s=RequireSettingDialogNativeConfirmServices();void* raw=s.memory.AllocateBody(0x84);
    RawLife life{raw,0x84,s.memory};auto* body=raw?s.constructors.GeneralAt4E3330(raw,mode):nullptr;
    life.armed=false; // 51E55E/51B7DD撤raw后才4515E0；control失败不补删84h正文。
    out.baseAt00=0;out.biasAt04=0;out.controlAt08=nullptr;
    if(body)
    {
        out.baseAt00=reinterpret_cast<std::uintptr_t>(body);void* c=s.memory.AllocateBody(0x18);
        auto* control=c?::new(c) GeneralControl18h(body):nullptr;out.controlAt08=control;
        *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4)=1;
        out.controlAt08->owningAt08=1;static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C=0x84;
        static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10=1;
    }
    return &out;
}
YtlReferenceSlotRuntime* CreateSettingDialogNameConfirmNativeReference(Slot& out)
{
    auto& s=RequireSettingDialogNativeConfirmServices();void* raw=s.memory.AllocateBody(0x94);
    RawLife life{raw,0x94,s.memory};auto* body=raw?s.constructors.NameAt4E4340(raw):nullptr;
    life.armed=false; // 521B29撤raw后才4D78D0；94h/84h各自control类型与stride，不能合并。
    out.baseAt00=0;out.biasAt04=0;out.controlAt08=nullptr;
    if(body)
    {
        out.baseAt00=reinterpret_cast<std::uintptr_t>(body);void* c=s.memory.AllocateBody(0x18);
        auto* control=c?::new(c) NameControl18h(body):nullptr;out.controlAt08=control;
        *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4)=1;
        out.controlAt08->owningAt08=1;static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C=0x94;
        static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10=1;
    }
    return &out;
}
}
