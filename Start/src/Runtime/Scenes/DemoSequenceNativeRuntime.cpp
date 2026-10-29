#include <Runtime/DemoSequenceNativeRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local DemoSequenceNativeServices* active=nullptr;
class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(DemoSequenceNativeBodyA4h* body) noexcept
        :EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        if (!payloadAt14) return;
        if (!active) throw std::logic_error("Demo sequence Ref destruction requires its external scope");
        DeleteDemoSequenceNativeAt4C5FB0(static_cast<DemoSequenceNativeBodyA4h*>(payloadAt14),*active);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override {RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control)==0x18);
}
DemoSequenceNativeScope::DemoSequenceNativeScope(DemoSequenceNativeServices& services) noexcept
    :previous_(active),graphics_(services.surfaces){active=&services;}
DemoSequenceNativeScope::~DemoSequenceNativeScope() noexcept{active=previous_;}
void AdoptDemoSequenceNativeAt49B644(YtlReferenceSlotRuntime& target,DemoSequenceNativeBodyA4h* body,IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(target);
    if (!body) return;
    if (target.controlAt08) target.controlAt08->DeleteControlAtVslot04(1);
    target.biasAt04=0;target.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    auto* raw=memory.AllocateBody(0x18);auto* control=raw?::new(raw) Control(body):nullptr;
    target.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    target.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(target.controlAt08)->strideAt0C=0xA4;
    static_cast<EnvironmentObjectReferenceControl*>(target.controlAt08)->countAt10=1;
}
DemoSequenceNativeRuntime::DemoSequenceNativeRuntime(DemoSequenceNativeServices& services):services_(services)
{
    DemoSequenceNativeScope scope(services_);
    AdoptDemoSequenceNativeAt49B644(reference_,NewDemoSequenceNativeAt49B624(services_),services_.memory);
}
DemoSequenceNativeRuntime::~DemoSequenceNativeRuntime() noexcept
{DemoSequenceNativeScope scope(services_);ReleaseYtlReference(reference_);}
DemoSequenceNativeBodyA4h* DemoSequenceNativeRuntime::Body() const noexcept
{return ResolveEnvironmentObjectReference<DemoSequenceNativeBodyA4h>(reference_);}
void DemoSequenceNativeRuntime::BindDemoDataAt4C2220(const YtlReferenceSlotRuntime& source)
{DemoSequenceNativeScope scope(services_);BindDemoSequenceNativeAt4C2220(*Body(),source);}
std::int32_t DemoSequenceNativeRuntime::Run(std::uint8_t completion)
{DemoSequenceNativeScope scope(services_);return RunDemoSequenceNativeAt4BDAE0(*Body(),completion,services_);}
bool DemoSequenceNativeRuntime::AdvanceTransition()
{DemoSequenceNativeScope scope(services_);return AdvanceDemoTransitionNativeAt4C38A0(*Body(),services_);}
void DemoSequenceNativeRuntime::UpdateSpritesAndRedraw()
{DemoSequenceNativeScope scope(services_);UpdateDemoSpritesNativeAt4A7690(*Body(),services_);}
std::unique_ptr<ILogoDemoSequenceRuntime> DemoSequenceNativeFactory::CreateLogoDemoSequence()
{return std::make_unique<DemoSequenceNativeRuntime>(services_);}
std::unique_ptr<IEndingDemoSequenceRuntime> DemoSequenceNativeFactory::CreateEndingDemoSequence()
{return std::make_unique<DemoSequenceNativeRuntime>(services_);}
}
