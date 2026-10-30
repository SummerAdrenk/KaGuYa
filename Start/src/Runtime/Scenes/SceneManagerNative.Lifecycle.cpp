#include "SceneManagerNative.Internal.h"
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SceneManagerNativeBody48h;
using namespace SceneManagerNativeDetail;
thread_local SceneManagerNativeServices* current=nullptr;
template<OwnedBody kind>class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(void* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* body=payloadAt14;if(!body)return;
        // 五个payload原492D60/492BC0/492C90/492F40/492D10均先检查payload空。
        if constexpr(kind==OwnedBody::Scheduler)
        {
            DestroyTaskControllerNativeAt4644A0(*static_cast<TaskControllerNativeBody18h*>(body),
                RequireSceneManagerNativeServices().scheduler);
            ::operator delete(body,std::size_t{0x18});
        }
        else if constexpr(kind==OwnedBody::Controller)
        {
            auto* task=reinterpret_cast<TaskNativeBody4h*>(body);task->vptrAt00->deleteAt04(task,1);
        }
        else if constexpr(kind==OwnedBody::Mode)
        {
            auto* mode=static_cast<ModeSelectButtonNativeBody10h*>(body);mode->vptrAt00->deleteAt00(mode,1);
        }
        else if constexpr(kind==OwnedBody::Dialog)
        {
            auto* dialog=reinterpret_cast<GuiDialogNativeBody5Ch*>(body);dialog->vptrAt00->deletingDestructorAt04(dialog,1);
        }
        else
        {
            auto* manager=static_cast<Body*>(body);manager->vptrAt00->deleteAt00(manager,1);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control<OwnedBody::Scheduler>)==0x18);
template<OwnedBody kind>EnvironmentObjectReferenceControl* NewControl(void* body,SceneManagerNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x18);
    return raw?::new(raw) Control<kind>(body):nullptr;
}
Body* __fastcall Deleting(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteSceneManagerNativeAt469300(body,flags,RequireSceneManagerNativeServices());}
TaskNativeBody4h* __fastcall Create(SceneFactoryNativeBody4h*,void*,const char* name)
{return RequireSceneManagerNativeServices().factory.CreateAt4AAF90(name);}
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const SceneManagerNativeVtable1 methods{reinterpret_cast<decltype(SceneManagerNativeVtable1::deleteAt00)>(&Deleting)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const SceneFactoryNativeVtable factoryMethods{reinterpret_cast<decltype(SceneFactoryNativeVtable::createAt00)>(&Create)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
struct RefUnwind final
{YtlReferenceSlotRuntime& slot;bool armed=true;~RefUnwind() noexcept{if(armed)ReleaseYtlReference(slot);}};
struct CStringUnwind final
{RuntimeCString& value;bool armed=true;~CStringUnwind() noexcept{if(armed)DestroyRuntimeCStringAt40CD40(&value);}};
}
void SceneManagerNativeDetail::Replace(YtlReferenceSlotRuntime& slot,void* body,OwnedBody kind,
    std::uint32_t stride,SceneManagerNativeServices& services)
{
    ReleaseYtlReference(slot);if(!body)return;
    if(auto* control=Read(slot.controlAt08))control->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    EnvironmentObjectReferenceControl* control=nullptr;
    switch(kind)
    {
    case OwnedBody::Scheduler:control=NewControl<OwnedBody::Scheduler>(body,services);break;
    case OwnedBody::Controller:control=NewControl<OwnedBody::Controller>(body,services);break;
    case OwnedBody::Mode:control=NewControl<OwnedBody::Mode>(body,services);break;
    case OwnedBody::Dialog:control=NewControl<OwnedBody::Dialog>(body,services);break;
    case OwnedBody::Manager:control=NewControl<OwnedBody::Manager>(body,services);break;
    }
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    Read(slot.controlAt08)->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C=stride;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10=1;
}
SceneManagerNativeScope::SceneManagerNativeScope(SceneManagerNativeServices& services) noexcept
    :previous_(current),controller_(services.controller),mode_(services.mode),dialog_(services.dialog),
    tasks_(services.factory.TaskServices()){current=&services;}
SceneManagerNativeScope::~SceneManagerNativeScope() noexcept{current=previous_;}
SceneManagerNativeServices& RequireSceneManagerNativeServices()
{if(!current)throw std::logic_error("SceneManager native call has no bound services");return *current;}
const SceneManagerNativeVtable1& SceneManagerNativeVtable() noexcept{return methods;}
Body* ConstructSceneManagerNativeAt45B890(Body& body,SceneManagerNativeServices& services)
{
    SceneManagerNativeScope scope(services);body.vptrAt00=&methods;
    body.schedulerAt04={};RefUnwind scheduler{body.schedulerAt04};
    body.controllerAt10={};RefUnwind controller{body.controllerAt10};
    InitializeEmptyRuntimeCString(body.commandAt1C,services.strings);CStringUnwind command{body.commandAt1C};
    body.modeAt20={};RefUnwind mode{body.modeAt20};
    body.dialogAt2C={};RefUnwind dialog{body.dialogAt2C};
    // 原第二次独立417360/GetNil；不克隆command或缓存第一次manager。
    InitializeEmptyRuntimeCString(body.demoNameAt3C,services.strings);CStringUnwind demo{body.demoNameAt3C};
    auto* task=NewTaskControllerNativeAt45B960(services.scheduler);
    Replace(body.schedulerAt04,task,OwnedBody::Scheduler,0x18,services);
    auto* scene=NewSceneControllerNativeAt45BA52(services.controller);
    Replace(body.controllerAt10,scene,OwnedBody::Controller,0x28,services);
    auto* raw=services.memory.AllocateBody(4);
    auto* factory=static_cast<SceneFactoryNativeBody4h*>(raw);if(factory)factory->vptrAt00=&factoryMethods;
    AdoptSceneFactoryNativeAt45BB78(*Resolve<SceneControllerNativeBody28h>(body.controllerAt10),factory,services.controller);
    auto* controllerBody=reinterpret_cast<TaskNativeBody4h*>(Resolve<SceneControllerNativeBody28h>(body.controllerAt10));
    auto* schedulerBody=Resolve<TaskControllerNativeBody18h>(body.schedulerAt04);
    RegisterTaskNativeAt46A330(*schedulerBody,controllerBody,1,services.scheduler);
    SetRuntimeCStringAt437680(body.commandAt1C,"",0);body.fieldAt44=0;
    demo.armed=dialog.armed=mode.armed=command.armed=controller.armed=scheduler.armed=false;return &body;
}
Body* NewSceneManagerNativeAt48ABE1(SceneManagerNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x48);if(!raw)return nullptr;
    struct Unwind final
    {void* raw;IEnvironmentRootMemory& memory;bool armed=true;~Unwind() noexcept{if(armed)memory.DeleteBody(raw,0x48);}} unwind{raw,services.memory};
    auto* body=ConstructSceneManagerNativeAt45B890(*::new(raw) Body,services);unwind.armed=false;return body;
}
void DestroySceneManagerNativeAt463AF0(Body& body,SceneManagerNativeServices& services) noexcept
{
    SceneManagerNativeScope scope(services);body.vptrAt00=&methods;
    DestroyRuntimeCStringAt40CD40(&body.demoNameAt3C);ReleaseYtlReference(body.dialogAt2C);
    ReleaseYtlReference(body.modeAt20);DestroyRuntimeCStringAt40CD40(&body.commandAt1C);
    ReleaseYtlReference(body.controllerAt10);ReleaseYtlReference(body.schedulerAt04);
}
Body* DeleteSceneManagerNativeAt469300(Body* body,std::uint32_t flags,SceneManagerNativeServices& services) noexcept
{
    DestroySceneManagerNativeAt463AF0(*body,services);
    if(flags&1u)::operator delete(body,std::size_t{0x48});return body;
}
void AdoptSceneManagerNativeAt48AC01(YtlReferenceSlotRuntime& slot,Body* body,SceneManagerNativeServices& services)
{SceneManagerNativeScope scope(services);Replace(slot,body,OwnedBody::Manager,0x48,services);}
}
