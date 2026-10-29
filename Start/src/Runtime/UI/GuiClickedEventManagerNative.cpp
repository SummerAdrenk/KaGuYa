#include <Runtime/GuiClickedEventNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class ManagerControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit ManagerControl(GuiClickedEventReferences* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        if(auto* body=static_cast<GuiClickedEventReferences*>(payloadAt14))
        {DestroyGuiClickedEventReferencesAt4F6D10(*body);::operator delete(body,std::size_t{0x0C});}
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(ManagerControl)==0x18);
}
void AdoptGuiClickedEventManagerAt4D7B90(YtlReferenceSlotRuntime& slot,GuiClickedEventReferences* body)
{
    ReleaseYtlReference(slot);
    if(!body)return;
    if(slot.controlAt08)slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(body);
    auto* raw=::operator new(0x18);
    auto* control=raw?::new(raw)ManagerControl(body):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    slot.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C=0x0C;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10=1;
}
}
