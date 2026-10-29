#include <Runtime/ReplaySelectionNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <atlbase.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T* Add(T* value,std::uint32_t bytes)noexcept
{return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(value)+bytes);}
template<class T>T* Resolve(const Slot& slot)noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<const EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(slot.biasAt04)*stride);
}
int Distance(const void* first,const void* last)noexcept
{return std::bit_cast<int>(std::uint32_t(reinterpret_cast<std::uintptr_t>(last)-reinterpret_cast<std::uintptr_t>(first)))/12;}
class SelectionControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit SelectionControl(void* body)noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00()override{if(payloadAt14)::operator delete(payloadAt14,std::size_t{8});}
    void DeleteControlAtVslot04(unsigned flags)override{RetireEnvironmentObjectControl(this,flags);}
};
struct ReturnGuard final
{
    Slot& value;bool active=true;
    ~ReturnGuard()noexcept{if(active)ReleaseReplaySelectionNativeAt4A2F90(value);}
};
const ParamsGameSystem* System(IReplaySelectionParamsNativeSource& source,unsigned site)
{return Resolve<ParamsGameSystem>(source.CaptureParamsAt418480(site)->gameSystemAt04.Native());}
const ParamsRegistSceneGroupRuntime* Group(const ParamsGameSystem* system,unsigned offset)
{return Resolve<ParamsRegistSceneGroupRuntime>(*Add(Read(system->registScene.storage.begin),offset));}
const ParamsRegistSceneItemRuntime* Item(const ParamsGameSystem* system,unsigned group,unsigned item)
{return Resolve<ParamsRegistSceneItemRuntime>(*Add(Read(Group(system,group)->itemsAt04.storage.begin),item));}
void CopyReference(Slot& target,const Slot& source)
{
    if(&target==&source)return;
    // 4ADD4B普通释放可抛；临时Ref处于state1，不能扩成全函数terminate。
    ReleaseYtlReference(target);
    target.baseAt00=Read(source.baseAt00);target.biasAt04=Read(source.biasAt04);target.controlAt08=Read(source.controlAt08);
    if(auto* control=Read(target.controlAt08))++control->referenceCountAt04;
}
}
void ReleaseReplaySelectionNativeAt4A2F90(Slot& value)noexcept{ReleaseYtlReference(value);}
Slot* FindReplayLabelNativeAt4AD300(Slot& out,const ParamsSceneLabelNative& table,const char* name,IEnvironmentRootMemory& memory)
{
    out.baseAt00=0;out.biasAt04=0;out.controlAt08=nullptr;
    auto* cursor=Read(table.labelsAt04.begin);ReturnGuard returned{out};
    while(cursor!=Read(table.labelsAt04.end))
    {
        auto* entry=Resolve<ParamsSceneLabelEntryRuntime>(*cursor);
        if(!name)throw ATL::CAtlException(E_FAIL); // 原4AD384在源Ref解析之后、name字段读取之前。
        const auto* characters=Read(*reinterpret_cast<const char* const*>(&entry->nameAt00));
        if(_mbsicmp(reinterpret_cast<const unsigned char*>(characters),reinterpret_cast<const unsigned char*>(name)))
        {cursor=Add(cursor,12);continue;}
        ReleaseYtlReference(out);
        // 8字节只有随后两次字段复制才写；控制块失败保留已发布base，不补删原孤儿。
        auto* body=memory.AllocateBody(8);
        if(auto* control=Read(out.controlAt08))control->DeleteControlAtVslot04(1);
        out.biasAt04=0;out.baseAt00=reinterpret_cast<std::uintptr_t>(body);
        auto* raw=memory.AllocateBody(0x18);auto* control=raw?::new(raw)SelectionControl(body):nullptr;
        out.controlAt08=control;
        *reinterpret_cast<volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
        Read(out.controlAt08)->owningAt08=1;
        static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->strideAt0C=8;
        static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08))->countAt10=1;
        entry=Resolve<ParamsSceneLabelEntryRuntime>(*cursor);
        auto* destination=Resolve<ReplaySelectionNativeBody8h>(out);
        const auto file=Read(entry->value1At04);
        static_cast<volatile ReplaySelectionNativeBody8h*>(destination)->fileIndexAt00=std::bit_cast<int>(file);
        entry=Resolve<ParamsSceneLabelEntryRuntime>(*cursor);
        const auto base=Read(out.baseAt00);
        auto* current=base?static_cast<EnvironmentObjectReferenceControl*>(Read(out.controlAt08)):nullptr;
        const auto bias=base?Read(out.biasAt04):0u;
        const auto stride=current?Read(current->strideAt0C):0u;
        const auto value=Read(entry->value2At08);
        destination=base?reinterpret_cast<ReplaySelectionNativeBody8h*>(base+bias*stride):nullptr;
        static_cast<volatile ReplaySelectionNativeBody8h*>(destination)->entryAt04=std::bit_cast<int>(value);
        break;
    }
    returned.active=false;return &out;
}
Slot* FindReplaySelectionNativeAt4ADAE0(Slot& out,IReplaySelectionParamsNativeSource& source,int category,int item,int segment,IEnvironmentRootMemory& memory)
{
    out.baseAt00=0;out.biasAt04=0;out.controlAt08=nullptr;ReturnGuard returned{out};
    const auto* first=System(source,0x4ADB34);
    auto* end=Read(first->registScene.storage.end);auto* begin=Read(first->registScene.storage.begin);
    if(category>=Distance(begin,end)){returned.active=false;return &out;}
    const auto* second=System(source,0x4ADB81);const auto groupOffset=unsigned(category)*12u;
    const auto* group=Group(second,groupOffset);
    end=Read(group->itemsAt04.storage.end);begin=Read(group->itemsAt04.storage.begin);
    if(item>=Distance(begin,end)){returned.active=false;return &out;}
    const auto* third=System(source,0x4ADBFB);const auto itemOffset=unsigned(item)*12u;
    auto* cursor=Read(Item(third,groupOffset,itemOffset)->scenesAt00.begin);
    unsigned ordinal=0;
    for(;;)
    {
        const auto* current=Item(System(source,0x4ADC80),groupOffset,itemOffset);
        if(cursor==Read(current->scenesAt00.end))break;
        if(std::bit_cast<int>(ordinal)!=segment){cursor=Add(cursor,4);++ordinal;continue;}
        const auto* characters=Read(*reinterpret_cast<const char* const*>(cursor));
        const auto* root=source.CaptureParamsAt418480(0x4ADD0E);
        const auto* labels=Resolve<ParamsSceneLabelNative>(root->sceneLabelsAt1C.Native());
        Slot temporary;FindReplayLabelNativeAt4AD300(temporary,*labels,characters,memory);
        ReturnGuard temporaryGuard{temporary};
        CopyReference(out,temporary);
        temporaryGuard.active=false;
        // 4ADD94 state2 ->541E55；固定入场control，payload后重读临时槽，不清临时字段。
        const auto normal=[](Slot& value)noexcept
        {
            auto* control=Read(value.controlAt08);
            if(control&&--control->referenceCountAt04==0)
            {
                if(control->owningAt08)control->DestroyPayloadAtVslot00();
                if(auto* current=Read(value.controlAt08))current->DeleteControlAtVslot04(1);
            }
        };
        normal(temporary);break;
    }
    returned.active=false;return &out;
}
}
