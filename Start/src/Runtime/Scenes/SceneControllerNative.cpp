#include <Runtime/SceneControllerNative.h>
#include <exception>
#include <new>
#include <stdexcept>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SceneControllerNativeBody28h;using Slot=YtlReferenceSlotRuntime;
thread_local SceneControllerNativeServices* active=nullptr;
SceneControllerNativeServices& Services()
{if(!active)throw std::logic_error("native scene controller requires its external scope");return *active;}
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
void ReleaseTask(Slot& slot) noexcept{ReleaseYtlReference(slot);}
struct RefUnwind final
{
    Slot& slot;bool armed=true;
    ~RefUnwind() noexcept{if(armed)ReleaseTask(slot);}
    void ReleaseNormal() noexcept{armed=false;ReleaseTask(slot);}
};
class TaskControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit TaskControl(TaskNativeBody4h* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* body=static_cast<TaskNativeBody4h*>(payloadAt14);if(body)body->vptrAt00->deleteAt04(body,1);}
    void DeleteControlAtVslot04(std::uint32_t flags) override;
};
class RetiredControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredControl() noexcept:EnvironmentObjectReferenceControl(nullptr,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override{_purecall();std::terminate();}
    void DeleteControlAtVslot04(std::uint32_t flags) override{if(flags&1u)::operator delete(this,std::size_t{0x14});}
};
static_assert(sizeof(TaskControl)==0x18&&sizeof(RetiredControl)==0x18);
void RetireControl(EnvironmentObjectReferenceControl* control,std::uint32_t flags)
{
    std::byte fields[20];std::memcpy(fields,reinterpret_cast<std::byte*>(control)+4,20);
    ::new(static_cast<void*>(control)) RetiredControl;
    std::memcpy(reinterpret_cast<std::byte*>(control)+4,fields,20);
    // 4117A0本次delete18；PE5ADD1C的退休虚槽412460才delete14。
    if(flags&1u)::operator delete(control,std::size_t{0x18});
}
void TaskControl::DeleteControlAtVslot04(std::uint32_t flags){RetireControl(this,flags);}
class FactoryControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit FactoryControl(SceneFactoryNativeBody4h* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    // 492BB0不判payload，原样把NULL或当前指针交sized delete4。
    void DestroyPayloadAtVslot00() override{::operator delete(payloadAt14,std::size_t{4});}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireControl(this,flags);}
};
void InstallTask(Slot& slot,TaskNativeBody4h* body,SceneControllerNativeServices& services)
{
    if(!body)return;
    if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    auto* raw=services.memory.AllocateBody(0x18);auto* control=raw?::new(raw) TaskControl(body):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    slot.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C=4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10=1;
}
void Copy(Slot& target,const Slot& source)
{
    target.baseAt00=source.baseAt00;target.biasAt04=source.biasAt04;target.controlAt08=source.controlAt08;
    if(auto* control=target.controlAt08)++control->referenceCountAt04;
}
void Append(EnvironmentVectorStorage<Slot>& vector,const Slot& source,SceneControllerNativeServices& services)
{
    auto* end=Read(vector.end);
    if(end==Read(vector.capacity))InsertSceneTaskNativeAt4092C0(vector,end,source,services.memory);
    else{Copy(*end,source);vector.end=reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(end)+12u);}
}
TaskNativeBody4h* Create(Body& body,const char* name)
{
    auto* factory=ResolveEnvironmentObjectReference<SceneFactoryNativeBody4h>(body.factoryAt1C);
    return factory->vptrAt00->createAt00(factory,name);
}
const char* PopName(Body& body)
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.begin));
    const auto count=std::bit_cast<std::int32_t>(std::uint32_t(end-begin))>>2;
    const auto* at=reinterpret_cast<const char* const*>(begin+std::uint32_t(count)*4u-4u);
    const auto* name=*at;body.pendingAt04.end=reinterpret_cast<const char**>(end-4u);return name;
}
std::uint8_t __fastcall Run(Body* body,void*,std::int32_t argument)
{return RunSceneControllerNativeAt48E100(*body,argument,Services());}
Body* __fastcall Delete(Body* body,void*,std::uint32_t flags) noexcept;
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const TaskNativeVtable methods{
    reinterpret_cast<decltype(TaskNativeVtable::runAt00)>(&Run),
    reinterpret_cast<decltype(TaskNativeVtable::deleteAt04)>(&Delete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
std::uint8_t __fastcall PureRun(TaskNativeBody4h*,void*,std::int32_t){_purecall();std::terminate();}
TaskNativeBody4h* __fastcall DeleteRetired(TaskNativeBody4h* body,void*,std::uint32_t flags) noexcept;
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const TaskNativeVtable retired{
    reinterpret_cast<decltype(TaskNativeVtable::runAt00)>(&PureRun),
    reinterpret_cast<decltype(TaskNativeVtable::deleteAt04)>(&DeleteRetired)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
TaskNativeBody4h* __fastcall DeleteRetired(TaskNativeBody4h* body,void*,std::uint32_t flags) noexcept
{body->vptrAt00=&retired;if(flags&1u)::operator delete(body,std::size_t{4});return body;}
Body* __fastcall Delete(Body* body,void*,std::uint32_t flags) noexcept
{DestroySceneControllerNative(*body,Services());if(flags&1u)::operator delete(body,std::size_t{0x28});return body;}
}
SceneControllerNativeScope::SceneControllerNativeScope(SceneControllerNativeServices& services) noexcept:previous_(active){active=&services;}
SceneControllerNativeScope::~SceneControllerNativeScope() noexcept{active=previous_;}
Body* ConstructSceneControllerNativeAt45BA63(Body& body)
{body.vptrAt00=&methods;body.pendingAt04={};body.activeAt10={};body.factoryAt1C={};return &body;}
Body* NewSceneControllerNativeAt45BA52(SceneControllerNativeServices& services)
{auto* raw=services.memory.AllocateBody(0x28);return raw?ConstructSceneControllerNativeAt45BA63(*::new(raw) Body):nullptr;}
void AdoptSceneFactoryNativeAt45BB78(Body& body,SceneFactoryNativeBody4h* factory,SceneControllerNativeServices& services)
{
    ReleaseYtlReference(body.factoryAt1C);if(!factory)return;
    auto& slot=body.factoryAt1C;if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(factory);
    auto* raw=services.memory.AllocateBody(0x18);auto* control=raw?::new(raw) FactoryControl(factory):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    slot.controlAt08->owningAt08=1;control=static_cast<FactoryControl*>(slot.controlAt08);
    control->strideAt0C=4;control->countAt10=1;
}
std::uint32_t PendingSceneCountNative(const Body& body) noexcept
{return std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.end))-reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.begin)))/4u;}
std::int32_t ActiveSceneCountNative(const Body& body) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.end))-reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.begin))))/12;}
bool HasSceneControllerWorkNativeAt4836AA(const Body& body) noexcept
{
    const auto pendingEnd=reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.end));
    const auto pendingBegin=reinterpret_cast<std::uintptr_t>(Read(body.pendingAt04.begin));
    if(std::uint32_t(pendingEnd-pendingBegin)>=4u)return true;
    const auto activeEnd=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.end));
    const auto activeBegin=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.begin));
    return std::bit_cast<std::int32_t>(std::uint32_t(activeEnd-activeBegin))/12!=0;
}
void GetCurrentTaskNativeAt481790(Body& body,Slot& output) noexcept
{
    output={};const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.begin));
    const auto end=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.end));
    const auto count=std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12;if(!count)return;
    auto* source=reinterpret_cast<Slot*>(begin+
        (std::uint32_t(count)-1u)*12u);
    if(&output!=source)Copy(output,*source);
}
void PopSceneTaskNativeAt4BCF80(Body& body)
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.begin));
    if(std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12==0)return;
    auto& slot=*reinterpret_cast<Slot*>(end-12u);
    // 4BCF80 state0->terminate，不将失败弹出视为可恢复异常。
    try{ReleaseYtlReference(slot);}catch(...){std::terminate();}
    body.activeAt10.end=reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(Read(body.activeAt10.end))-12u);
}
void QueueSceneControllerNativeAt47F966(Body& body,const char* name,SceneControllerNativeServices& services)
{
    PopSceneTaskNativeAt4BCF80(body);
    auto* end=Read(body.pendingAt04.end);
    if(end==Read(body.pendingAt04.capacity))InsertSceneNameNativeAt453B50(body.pendingAt04,end,&name,services.memory);
    else{*end=name;body.pendingAt04.end=reinterpret_cast<const char**>(reinterpret_cast<std::uintptr_t>(end)+4u);}
}
void PushSceneTaskNativeAt40B050(EnvironmentVectorStorage<Slot>& vector,TaskNativeBody4h* body,
    SceneControllerNativeServices& services)
{
    Slot temporary;InstallTask(temporary,body,services);RefUnwind cleanup{temporary};
    Append(vector,temporary,services);
    // 40B115缓存控制块，释放后不清局部；payload后也不用重读槽替代缓存。
    auto* control=temporary.controlAt08;cleanup.armed=false;
    try{if(control&&--control->referenceCountAt04==0){if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}}
    catch(...){std::terminate();}
}
void ActivatePendingSceneNativeAt4C1782(Body& body,SceneControllerNativeServices& services)
{
    PopSceneTaskNativeAt4BCF80(body);if(!PendingSceneCountNative(body))return;
    const auto* name=PopName(body);PushSceneTaskNativeAt40B050(body.activeAt10,Create(body,name),services);
}
void ClearSceneControllerNativeAt4C14A5(Body& body) noexcept
{
    ReleaseSceneTaskRangeNativeAt407FF0(body.activeAt10.begin,body.activeAt10.end);
    body.activeAt10.end=body.activeAt10.begin;body.pendingAt04.end=body.pendingAt04.begin;
}
std::uint8_t RunSceneControllerNativeAt48E100(Body& body,std::int32_t argument,SceneControllerNativeServices& services)
{
    if(PendingSceneCountNative(body)<1u&&!ActiveSceneCountNative(body))return 0;
    Slot current;GetCurrentTaskNativeAt481790(body,current);RefUnwind cleanup{current};
    if(!current.baseAt00)
    {
        const auto* name=PopName(body);auto* task=Create(body,name);
        // 48E1BD旧local的inline释放可抛；展开时仍保留其清理资格。
        ReleaseYtlReference(current);InstallTask(current,task,services);
        Slot argumentSlot;Copy(argumentSlot,current);RefUnwind argumentCleanup{argumentSlot};
        Append(body.activeAt10,argumentSlot,services);
        auto* control=argumentSlot.controlAt08;argumentCleanup.armed=false;
        try{if(control&&--control->referenceCountAt04==0){if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}}
        catch(...){std::terminate();}
    }
    auto* task=ResolveEnvironmentObjectReference<TaskNativeBody4h>(current);
    (void)task->vptrAt00->runAt00(task,argument);
    cleanup.ReleaseNormal();return 1;
}
void DestroySceneControllerNative(Body& body,SceneControllerNativeServices& services) noexcept
{
    ReleaseTask(body.factoryAt1C);
    DestroySceneTaskVectorNative(body.activeAt10,services.memory);
    DestroySceneNameVectorNative(body.pendingAt04,services.memory);
    body.vptrAt00=&retired;
}
}
