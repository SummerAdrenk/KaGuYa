#include <Runtime/EditWindowNativeCreation.h>
#include <Runtime/YtlReferenceArrayRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class EditControlRef final:public EnvironmentObjectReferenceControl
{
public:
    explicit EditControlRef(EditWindowNativeBody98h* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* body=static_cast<EditWindowNativeBody98h*>(payloadAt14);
        if(body)body->outerAt00.vptrAt00->deleteAt04(&body->outerAt00,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(EditControlRef)==0x18);
}
EditWindowNativeBody98h* EditWindowNativeCreationRuntime::CreateAt46FA30(EditWindowNativeReferences& refs,
    DWORD disable,DWORD alignment,std::uintptr_t borderArgument)
{
    void* raw=::operator new(0x98);
    auto* body=raw?::new(raw)EditWindowNativeBody98h:nullptr;
    if(body)InitializeEditWindowNativeStorageAt46FA74(*body);
    YtlReferenceSlotRuntime local{};YtlReferenceControlRuntime* captured=nullptr;
    if(body)
    {
        local.baseAt00=reinterpret_cast<std::uintptr_t>(body);
        void* storage=::operator new(0x18);
        auto* control=storage?::new(storage)EditControlRef(body):nullptr;
        control->referenceCountAt04=1;control->owningAt08=1;control->strideAt0C=0x98;
        local.controlAt08=control;control->countAt10=1;captured=control;
    }
    body->alignmentAt80=alignment; // 原46FBCC在state0资格之前写；NULL正文是原访问故障。
    const auto style=borderArgument?0x40800000u:0x40000000u;
    struct Unwind final
    {
        YtlReferenceSlotRuntime& local;bool active=true;
        ~Unwind() noexcept{if(active)ReleaseYtlReference(local);}
    } lifetime{local};
    const auto parent=dependencies_.environment.CaptureWindowAt4180C0();
    CreateEndpoint(body->outerAt00,dependencies_.windowClass,style,parent);
    // 46FC37/3A直接从临时Ref base读取+04，不提前按bias/stride解析窗口。
    const auto selected=reinterpret_cast<EditWindowNativeBody98h*>(local.baseAt00);
    static_cast<void>(dependencies_.windows.EnableWindow(selected->outerAt00.windowAt04,disable==0));
    auto* position=refs.end;
    if(position==refs.capacity)
    {
        static_cast<void>(GrowEditWindowNativeReferencesAt452E60(refs,position,&local));
        captured=local.controlAt08; // 原46FD17只在增长路更新ESI。
    }
    else
    {
        position->baseAt00=local.baseAt00;position->biasAt04=0;position->controlAt08=captured;
        if(captured)++captured->referenceCountAt04;
        refs.end=reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(refs.end)+12u);
    }
    auto* result=local.baseAt00?reinterpret_cast<EditWindowNativeBody98h*>(local.baseAt00+
        local.biasAt04*(captured?static_cast<EnvironmentObjectReferenceControl*>(captured)->strideAt0C:0u)):nullptr;
    lifetime.active=false;
    // 原state1正常尾用缓存ESI，不在payload删除后重读local.control。
    struct Finish final
    {
        YtlReferenceControlRuntime* control;
        ~Finish() noexcept
        {
            if(control&&--control->referenceCountAt04==0)
            {if(control->owningAt08)control->DestroyPayloadAtVslot00();control->DeleteControlAtVslot04(1);}
        }
    } finish{captured};
    return result;
}
}
