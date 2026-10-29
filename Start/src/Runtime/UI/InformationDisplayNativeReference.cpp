#include <Runtime/InformationDisplayNativeReference.h>
#include <Runtime/SystemDialogNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(SystemInformationDisplayNativeBody14h* body) noexcept
        :EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        // 原表5B37B0 -> 492C60：从control+14取父，只调用其唯一删除槽(1)。
        auto* body=static_cast<SystemInformationDisplayNativeBody14h*>(payloadAt14);
        if(body)body->vptrAt00->deleteAt00(body,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control)==0x18);
void ReleaseWithoutClearing(YtlReferenceSlotRuntime& slot)
{
    auto* control=slot.controlAt08;
    if(control&&--control->referenceCountAt04==0)
    {
        if(control->owningAt08)control->DestroyPayloadAtVslot00();
        // payload回调可能改变根槽，48A685/40D1DC重新读取当前control。
        if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
}
void CreateAndInstallInformationDisplayAt48A64A(YtlReferenceSlotRuntime& slot,
    SystemInformationDisplayNativeServices& services)
{
    SystemDialogNativeScope scope(services.dialog);
    auto* raw=services.memory.AllocateBody(0x14);
    SystemInformationDisplayNativeBody14h* body=nullptr;
    {
        struct Unwind
        {void* raw;IEnvironmentRootMemory& memory;bool active=true;
            ~Unwind() noexcept{if(active)memory.DeleteBody(raw,0x14);}} unwind{raw,services.memory};
        if(raw)body=ConstructSystemInformationDisplayNativeAt4DC4C0(*::new(raw)SystemInformationDisplayNativeBody14h,services);
        unwind.active=false; // state0C已撤；完成14h不得因旧槽释放/new18失败补删。
    }
    if(slot.controlAt08)
    {
        ReleaseWithoutClearing(slot);
        slot.controlAt08=nullptr;slot.baseAt00=0;
    }
    if(!body)return;
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    auto* storage=services.memory.AllocateBody(0x18);
    auto* control=storage?::new(storage)Control(body):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    slot.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C=0x14;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10=1;
}
void ReleaseInformationDisplayReferenceAt40D1A0(YtlReferenceSlotRuntime& slot,
    SystemDialogNativeServices* services) noexcept
{
    if(!slot.controlAt08)return;
    SystemDialogNativeScope scope(services);
    ReleaseWithoutClearing(slot);
    slot.controlAt08=nullptr;slot.baseAt00=0;
}
}
