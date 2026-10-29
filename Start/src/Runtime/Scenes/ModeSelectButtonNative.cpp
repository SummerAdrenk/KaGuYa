#include <Runtime/ModeSelectButtonNative.h>
#include <Runtime/GameBuildProfile.h>
#include "../UI/CgUiButtonNative.Internal.h"
#include <new>
#include <cstdlib>
#include <stdexcept>

extern "C" int __cdecl _purecall();
namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=ModeSelectButtonNativeBody10h;
using Button=CgUiButtonNativeBody6Ch;
using namespace CgUiButtonNativeDetail;
class ArrayControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit ArrayControl(Button* payload) noexcept:EnvironmentObjectReferenceControl(payload,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* payload=static_cast<Button*>(payloadAt14);if(!payload)return;
        auto* raw=reinterpret_cast<std::byte*>(payload)-4;
        if(Read(*reinterpret_cast<std::uint32_t*>(raw)))payload->vptrAt00->deleteAt00(payload,3);
        else std::free(raw); // 491DA7：零cookie走原free_base，不调用数组虚槽。
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(ArrayControl)==0x18);
Body* __fastcall Delete(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteModeSelectButtonNativeAt468E50(body,flags);}
Body* __fastcall BaseDelete(Body* body,void*,std::uint32_t flags) noexcept;
void __fastcall Reset(Body* body,void*){ResetModeSelectButtonNativeAt487470(*body);}
void __fastcall PureReset(Body*,void*){_purecall();}
const ModeSelectButtonNativeVtable2 methods{
    reinterpret_cast<decltype(ModeSelectButtonNativeVtable2::deleteAt00)>(&Delete),
    reinterpret_cast<decltype(ModeSelectButtonNativeVtable2::resetAt04)>(&Reset)};
const ModeSelectButtonNativeVtable2 baseMethods{
    reinterpret_cast<decltype(ModeSelectButtonNativeVtable2::deleteAt00)>(&BaseDelete),
    reinterpret_cast<decltype(ModeSelectButtonNativeVtable2::resetAt04)>(&PureReset)};
Body* __fastcall BaseDelete(Body* body,void*,std::uint32_t flags) noexcept
{
    DestroyOptionButtonBaseNativeAt464920(*body);
    if(flags&1u)::operator delete(body,std::size_t{0x10});return body;
}
struct ArrayUnwind final
{
    std::byte* raw;unsigned completed=0;bool armed=true;
    ~ArrayUnwind() noexcept
    {
        if(!armed)return;
        auto* first=reinterpret_cast<Button*>(raw+4);
        for(auto at=completed;at;--at)DestroyCgUiButtonNativeAt463160(first[at-1]);
        ::operator delete[](raw,std::size_t{0x1B4});
    }
};
Button* NewArray()
{
    auto* raw=static_cast<std::byte*>(::operator new[](0x1B4));if(!raw)return nullptr;
    ArrayUnwind unwind{raw};*reinterpret_cast<std::uint32_t*>(raw)=4;
    auto* first=reinterpret_cast<Button*>(raw+4);
    for(unsigned at=0;at<4;++at)
    {ConstructCgUiButtonNativeAt459530(*::new(static_cast<void*>(first+at)) Button);++unwind.completed;}
    unwind.armed=false;return first;
}
void PublishArray(Body& body,Button* first,ModeSelectButtonNativeServices& services)
{
    ClearModeSelectButtonNativeAt486AD0(body);body.buttonsAt04.biasAt04=0;
    body.buttonsAt04.baseAt00=reinterpret_cast<std::uintptr_t>(first);
    auto* raw=services.memory.AllocateBody(0x18);
    auto* control=raw?::new(raw) ArrayControl(first):nullptr;
    body.buttonsAt04.controlAt08=control;
    // 45AD32先发布control，null随后仍在地址4写refs。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08))->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08))->strideAt0C=0x6C;
    static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08))->countAt10=4;
}
Button* ButtonAt(Body& body,std::int32_t index) noexcept
{
    const auto base=Read(body.buttonsAt04.baseAt00);if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<Button*>(base+(Read(body.buttonsAt04.biasAt04)+std::uint32_t(index))*stride);
}
}
ModeSelectButtonNativeScope::ModeSelectButtonNativeScope(ModeSelectButtonNativeServices& services) noexcept
    :surface_(services.surfaces),button_(services.button){}
ModeSelectButtonNativeScope::~ModeSelectButtonNativeScope() noexcept=default;
const ModeSelectButtonNativeVtable2& ModeSelectButtonNativeVtable() noexcept{return methods;}
const ModeSelectButtonNativeVtable2& OptionButtonBaseNativeVtable() noexcept{return baseMethods;}
Body* ConstructModeSelectButtonNativeAt45AC40(Body& body,std::int32_t mode,ModeSelectButtonNativeServices& services)
{
    ModeSelectButtonNativeScope scope(services);body.buttonsAt04={};body.vptrAt00=&methods;
    struct BaseUnwind final
    {Body& body;bool armed=true;~BaseUnwind() noexcept{if(armed)DestroyOptionButtonBaseNativeAt464920(body);}} baseUnwind{body};
    // FuncInfo5ECDB8内部catch保护state1..2；新array/control的失败按原部分发布处理。
    try
    {
        auto* first=NewArray();PublishArray(body,first,services);
        for(std::int32_t index=0;;index=CgUiButtonNativeDetail::Add(index,1))
        {
            auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08));
            const auto count=control?std::bit_cast<std::int32_t>(Read(control->countAt10)):0;
            if(index>=count)break;
            auto* button=ButtonAt(body,index);
            auto* receiver=services.source.CaptureDrawAt45AD92();
            const auto offset=(std::uint32_t(index)+std::uint32_t(mode)*4u)*40u;
            const auto* record=services.source.RecordAt601658(offset);
            // 旧版以自然资源原点放置，新版才使用记录中的显式坐标。
            if(!services.source.LoadAt480930(receiver,button,record,
                CompiledGameProfile().modeSelectExplicitPosition?1:0))throw 1;
            button=ButtonAt(body,index);button->pixelHitAt08=0;button->hitRectAt0C={};
        }
    }
    catch(...){ClearModeSelectButtonNativeAt486AD0(body);}
    baseUnwind.armed=false;return &body;
}
Body* NewModeSelectButtonNativeAt4703F9(std::int32_t mode,ModeSelectButtonNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x10);if(!raw)return nullptr;
    struct Unwind final
    {void* raw;IEnvironmentRootMemory& memory;bool armed=true;~Unwind() noexcept{if(armed)memory.DeleteBody(raw,0x10);}} unwind{raw,services.memory};
    auto* body=ConstructModeSelectButtonNativeAt45AC40(*::new(raw) Body,mode,services);unwind.armed=false;return body;
}
void ClearModeSelectButtonNativeAt486AD0(Body& body){ReleaseYtlReference(body.buttonsAt04);}
void ResetModeSelectButtonNativeAt487470(Body& body)
{
    for(std::int32_t index=0;;index=CgUiButtonNativeDetail::Add(index,1))
    {
        auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(body.buttonsAt04.controlAt08));
        const auto count=control?std::bit_cast<std::int32_t>(Read(control->countAt10)):0;
        if(index>=count)return;
        auto* button=ButtonAt(body,index);button->stateAt04=0;button->latchAt20=0;
    }
}
void DestroyOptionButtonBaseNativeAt464920(Body& body) noexcept
{body.vptrAt00=&baseMethods;ReleaseYtlReference(body.buttonsAt04);}
Body* DeleteModeSelectButtonNativeAt468E50(Body* body,std::uint32_t flags) noexcept
{
    body->vptrAt00=&methods;ReleaseYtlReference(body->buttonsAt04);
    DestroyOptionButtonBaseNativeAt464920(*body);
    if(flags&1u)::operator delete(body,std::size_t{0x10});return body;
}
}
