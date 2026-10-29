#include "LogSceneNative.Internal.h"
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace LogSceneNativeDetail;
void* CaptureRegistry(LogSceneNativeServices& s,unsigned site)
{
    auto* e=s.source.CaptureEnvironmentAt4180C0(site);
    // 49E1D7/49E228内联查表先end再begin，与普通417990次序不能混用。
    const auto end=reinterpret_cast<std::uintptr_t>(Read(e->archivesAt30.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(e->archivesAt30.begin));
    const auto count=static_cast<unsigned>(std::bit_cast<int>(end-begin)/12);
    return count>2?Resolve<void>(*reinterpret_cast<const Slot*>(begin+24)):nullptr;
}
struct TaskLife{Body& body;bool armed=true;~TaskLife() noexcept{if(armed)RetireTask(body);}};
struct MemberLife{Slot& slot;bool armed=true;~MemberLife() noexcept{if(armed)ReleaseYtlReference(slot);}};
struct RawLife{void* p;IEnvironmentRootMemory& memory;bool armed=true;~RawLife() noexcept{if(armed)memory.DeleteBody(p,0xDC);}};
class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(Common* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {auto* p=static_cast<Common*>(Read(payloadAt14));if(p)Read(p->vptrAt00)->deletingDestructorAt04(p,1);}
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(DialogControl)==0x18);
void Adopt(Slot& slot,Common* child,LogSceneNativeServices& s)
{
    ReleaseYtlReference(slot);if(!child)return;
    if(auto* control=Read(slot.controlAt08))control->DeleteControlAtVslot04(1);
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<std::uintptr_t>(child);
    auto* raw=s.memory.AllocateBody(0x18);auto* control=raw?::new(raw) DialogControl(child):nullptr;
    slot.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4)=1;
    Read(slot.controlAt08)->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C=0xDC;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10=1;
}
void* Root(LogSceneNativeServices& s,unsigned site)
{s.source.EnsureRootAt413BE0(site);return s.source.RootAt6198C4(site);}
}
LogSceneNativeBody20h* ConstructLogSceneNativeAt49E180(LogSceneNativeBody20h& b,LogSceneNativeServices& s)
{
    LogSceneNativeScope scope(s);b.vptrAt00=&LogSceneNativeMethods();TaskLife task{b};
    b.dialogAt10={};MemberLife member{b.dialogAt10};
    b.savedRegistryAt08=Read(RegistryValue(CaptureRegistry(s,0x49E1D2)));
    *static_cast<volatile std::uint32_t*>(&RegistryValue(CaptureRegistry(s,0x49E223)))=0;
    b.phaseAt04=0;auto* raw=s.memory.AllocateBody(0xDC);Common* child=nullptr;
    {RawLife life{raw,s.memory};if(raw)child=s.source.ConstructLogDialogAt4E23D0(raw);life.armed=false;}
    Adopt(b.dialogAt10,child,s); // control失败不补删已经完成的DC正文。
    b.savedRootByteAt1C=s.source.RootByteAt218(Root(s,0x49E33B));
    b.savedRootByteAt1D=s.source.RootByteAt219(Root(s,0x49E354));
    s.source.WriteRootByteAt218(Root(s,0x49E36D),0);
    s.source.WriteRootByteAt219(Root(s,0x49E383),0);
    member.armed=false;task.armed=false;return &b;
}
}
