#include <Runtime/SystemInformationDisplayNative.h>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SystemInformationDisplayNativeBody14h;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
Body* __fastcall Delete(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteSystemInformationDisplayNativeAt4FC080(body,flags);}
const SystemInformationDisplayNativeVtable1 methods{
    reinterpret_cast<decltype(SystemInformationDisplayNativeVtable1::deleteAt00)>(&Delete)};
class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(GuiDialogNativeBody5Ch* child) noexcept
        :EnvironmentObjectReferenceControl(child,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        // 53D9E0：正文+04(1)，不借现代SystemMenuDialog的C++删除。
        auto* child=static_cast<GuiDialogNativeBody5Ch*>(Read(payloadAt14));
        if(child)Read(child->vptrAt00)->deletingDestructorAt04(child,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control)==0x18);
struct RefUnwind final
{YtlReferenceSlotRuntime& slot;bool armed=true;~RefUnwind() noexcept{if(armed)ReleaseYtlReference(slot);}};
struct RawUnwind final
{void* raw;IEnvironmentRootMemory& memory;bool armed=true;~RawUnwind() noexcept{if(armed)memory.DeleteBody(raw,0x88);}};
GuiDialogNativeBody5Ch* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<GuiDialogNativeBody5Ch*>(base+Read(slot.biasAt04)*stride);
}
// 只有调用期借用的Ref地址。Resolve后每个实际虚调用仍重新解析同一槽，不保活旧child。
class ModalTarget final:public IModalDialogRuntimeTargetReference,private IModalDialogRuntimeTarget
{
public:
    explicit ModalTarget(const YtlReferenceSlotRuntime& slot) noexcept:slot_(slot){}
    IModalDialogRuntimeTarget* ResolveAt4A5330() override{return this;}
private:
    void BeginAtVslot44() override{auto* child=Resolve(slot_);Read(child->vptrAt00)->beginAt44(child);}
    bool PollAtVslot48() override{auto* child=Resolve(slot_);return Read(child->vptrAt00)->pollAt48(child)!=0;}
    bool EndAtVslot4C() override{auto* child=Resolve(slot_);return Read(child->vptrAt00)->endAt4C(child)!=0;}
    std::int32_t ResultAtVslot50() const override{auto* child=Resolve(slot_);return Read(child->vptrAt00)->resultAt50(child);}
    const YtlReferenceSlotRuntime& slot_;
};
}
const SystemInformationDisplayNativeVtable1& SystemInformationDisplayNativeMethods() noexcept{return methods;}
Body* ConstructSystemInformationDisplayNativeAt4DC4C0(Body& body,SystemInformationDisplayNativeServices& services)
{
    body.vptrAt00=&methods;body.dialogAt04={};RefUnwind member{body.dialogAt04};
    auto* raw=services.memory.AllocateBody(0x88);GuiDialogNativeBody5Ch* child=nullptr;
    {
        RawUnwind cleanup{raw,services.memory};
        if(raw)child=services.source.ConstructSystemDialogAt4F41A0(raw);
        cleanup.armed=false;
    }
    ReleaseYtlReference(body.dialogAt04);
    if(child)
    {
        body.dialogAt04.biasAt04=0;body.dialogAt04.baseAt00=reinterpret_cast<std::uintptr_t>(child);
        auto* controlRaw=services.memory.AllocateBody(0x18);
        auto* control=controlRaw?::new(controlRaw) Control(child):nullptr;
        body.dialogAt04.controlAt08=control;
        *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
        body.dialogAt04.controlAt08->owningAt08=1;
        static_cast<EnvironmentObjectReferenceControl*>(body.dialogAt04.controlAt08)->strideAt0C=0x88;
        static_cast<EnvironmentObjectReferenceControl*>(body.dialogAt04.controlAt08)->countAt10=1;
    }
    body.suppressedAt10=0;member.armed=false;return &body;
}
void DestroySystemInformationDisplayNativeAt4F74E0(Body& body) noexcept
{body.vptrAt00=&methods;ReleaseYtlReference(body.dialogAt04);}
Body* DeleteSystemInformationDisplayNativeAt4FC080(Body* body,std::uint32_t flags) noexcept
{DestroySystemInformationDisplayNativeAt4F74E0(*body);if(flags&1u)::operator delete(body,std::size_t{0x14});return body;}
std::int32_t QuerySystemInformationDisplayNativeAt5105C0(Body& body,SystemInformationDisplayNativeServices& services)
{
    // 两级门控短路；父失败不能调用5104A0而清child的84。
    if(services.source.ReadRootModalGateAfter413BE0()!=0)return 0;
    if(Read(body.suppressedAt10)!=0)return 0;
    return services.source.TriggerSystemDialogAt5104A0(Resolve(body.dialogAt04));
}
std::int32_t RunSystemInformationDisplayNativeAt5075B0(Body& body,SystemInformationDisplayNativeServices& services)
{
    return RunNativeGuiModalReferenceAt5071D0(&body.dialogAt04,services.source.ModalServices());
}
std::int32_t RunNativeGuiModalReferenceAt5071D0(const YtlReferenceSlotRuntime* slot,IModalDialogRunnerServices& services)
{
    ModalTarget target(*slot);ModalDialogRunnerRuntime runner(services);
    return runner.RunAt5071D0(target);
}
}
