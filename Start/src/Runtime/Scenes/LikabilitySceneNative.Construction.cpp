#include "LikabilitySceneNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LikabilitySceneNativeDetail;
class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(GuiDialogNativeBody5Ch* p)noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00()override
    {auto* p=static_cast<GuiDialogNativeBody5Ch*>(Read(payloadAt14));if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(unsigned flags)override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control)==0x18);
}
LikabilitySceneNativeBody20h* ConstructLikabilitySceneNativeAt49DFB0(LikabilitySceneNativeBody20h& body,LikabilitySceneNativeServices& services)
{
    LikabilitySceneNativeScope scope(services);auto& source=services.source;
    body.vptrAt00=&LikabilitySceneNativeMethods();body.dialogAt10={};
    struct Members{Body& body;bool active=true;~Members()noexcept{if(active){ReleaseYtlReference(body.dialogAt10);Retire(body);}}} members{body};
    body.savedModeAt08=Read(Field<unsigned>(Mode(source,0x49E002),4));
    Field<unsigned>(Mode(source,0x49E053),4)=0;body.phaseAt04=0;
    auto* raw=services.memory.AllocateBody(sizeof(LikabilityDialogNativeBody94h));
    struct Raw{void* p;IEnvironmentRootMemory& memory;bool active=true;~Raw()noexcept{if(active)memory.DeleteBody(p,sizeof(LikabilityDialogNativeBody94h));}} cleanup{raw,services.memory};
    auto* gui=raw?source.ConstructDialogAt4E0E30(raw):nullptr;cleanup.active=false;
    ReleaseYtlReference(body.dialogAt10);
    if(gui)
    {
        if(auto* previous=Read(body.dialogAt10.controlAt08))previous->DeleteControlAtVslot04(1);
        body.dialogAt10.biasAt04=0;body.dialogAt10.baseAt00=reinterpret_cast<std::uintptr_t>(gui);
        auto* allocation=services.memory.AllocateBody(0x18);auto* control=allocation?::new(allocation)Control(gui):nullptr;
        body.dialogAt10.controlAt08=control;
        // 原49E144直接写control+4；不把null申请改成可捕获bad_alloc或成功空对象。
        *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
        Read(body.dialogAt10.controlAt08)->owningAt08=1;
        static_cast<EnvironmentObjectReferenceControl*>(Read(body.dialogAt10.controlAt08))->strideAt0C=sizeof(LikabilityDialogNativeBody94h);
        static_cast<EnvironmentObjectReferenceControl*>(Read(body.dialogAt10.controlAt08))->countAt10=1;
    }
    members.active=false;return &body;
}
}
