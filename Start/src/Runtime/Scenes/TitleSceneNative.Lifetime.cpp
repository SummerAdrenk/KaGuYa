#include "TitleSceneNative.Internal.h"
#include <array>
#include <bit>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace TitleSceneNativeDetail;
thread_local TitleSceneNativeServices* active=nullptr;
class DialogControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(void* body) noexcept
        :EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        // 4C6110：payload原vslot+04(1)，不是delete现代TitleDialogProductionRuntime。
        auto* object=payloadAt14;if(!object)return;
        using Delete=void* (__thiscall*)(void*,std::uint32_t);
        auto* table=*static_cast<const std::uintptr_t* const*>(object);
        reinterpret_cast<Delete>(table[1])(object,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(DialogControl)==0x18);
struct MemberUnwind final
{
    Body& body;bool armed=true;
    ~MemberUnwind() noexcept
    {if(armed){ReleaseYtlReference(body.dialogAt08);Retire(body);}}
};
struct RawUnwind final
{
    void* raw;IEnvironmentRootMemory& memory;bool armed=true;
    ~RawUnwind() noexcept{if(armed)memory.DeleteBody(raw,0xD8);}
};
}
namespace TitleSceneNativeDetail
{
TitleSceneNativeServices& ActiveServices()
{if(!active)throw std::logic_error("Title raw call requires external services");return *active;}
void* InlineArchive(EnvironmentRootStorage* root,std::uint32_t index)
{
    const auto& array=static_cast<const volatile EnvironmentRootStorage*>(root)->archivesAt30;
    const auto end=reinterpret_cast<std::uintptr_t>(array.end);
    const auto begin=reinterpret_cast<std::uintptr_t>(array.begin);
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12);
    if(count<=index)return nullptr;
    const auto* slot=reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin+index*12u);
    const auto base=slot->baseAt00;if(!base)return nullptr;
    const auto* control=static_cast<const volatile EnvironmentObjectReferenceControl*>(slot->controlAt08);
    const auto stride=control?control->strideAt0C:0u;
    return reinterpret_cast<void*>(base+slot->biasAt04*stride);
}
void AdoptDialog(Body& body,void* dialog,TitleSceneNativeServices& services)
{
    ReleaseYtlReference(body.dialogAt08);
    if(!dialog)return;
    // 4A294F先发布base，control分配失败不删已完成D8h，NULL control按原写+04故障。
    body.dialogAt08.biasAt04=0;
    body.dialogAt08.baseAt00=reinterpret_cast<std::uintptr_t>(dialog);
    auto* raw=services.memory.AllocateBody(0x18);
    auto* control=raw?::new(raw) DialogControl(dialog):nullptr;
    body.dialogAt08.controlAt08=control;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control)+4u)=1;
    body.dialogAt08.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(body.dialogAt08.controlAt08)->strideAt0C=0xD8;
    static_cast<EnvironmentObjectReferenceControl*>(body.dialogAt08.controlAt08)->countAt10=1;
}
}
TitleSceneNativeScope::TitleSceneNativeScope(TitleSceneNativeServices& services) noexcept
    :previous_(active)
{
    if(auto* calls=services.dialogs.NativeDialogServices())dialog_.emplace(*calls);
    active=&services;
}
TitleSceneNativeScope::~TitleSceneNativeScope() noexcept{active=previous_;}
Body* ConstructTitleSceneNativeAt4A2850(Body& body,TitleSceneNativeServices& services)
{
    TitleSceneNativeScope scope(services);
    body.vptrAt00=&TitleSceneNativeMethods();
    body.dialogAt08={};MemberUnwind cleanup{body};
    TitleSceneNativeDetail::View(services.source.CaptureScenesAfter413BE0()).ClearPublicAt46DB90();
    auto* raw=services.memory.AllocateBody(0xD8);void* dialog=nullptr;
    {
        RawUnwind allocation{raw,services.memory};
        if(raw)dialog=services.dialogs.ConstructAt4F4530(raw);
        // 原callee正常返回NULL仍取消state2 raw资格，不补回收原孤儿。
        allocation.armed=false;
    }
    AdoptDialog(body,dialog,services);
    auto& source=services.source;
    auto byte=[&](std::uint32_t offset,std::uint8_t value)
    {*reinterpret_cast<volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(InlineArchive(source.CaptureEnvironmentAt4180C0(),2))+offset)=value;};
    auto dword=[&](std::uint32_t offset,std::uint32_t value)
    {*reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(InlineArchive(source.CaptureEnvironmentAt4180C0(),2))+offset)=value;};
    dword(4,0);byte(0x24,1);byte(0x25,1);byte(0x26,1);byte(0x27,0);
    byte(8,0);byte(9,1);byte(0x0C,0);dword(0x18,0);dword(0x14,1);dword(0x1C,1);byte(0x29,0);byte(0x28,0);
    body.phaseAt04=0;
    source.WriteRootByteAfter413BE0(0x218,0);source.WriteRootByteAfter413BE0(0x219,1);
    source.ClearReplayAt4A2DD6();
    cleanup.armed=false;return &body;
}
void DestroyTitleSceneNativeAt4A71E0(Body& body) noexcept
{
    // 5EFB6C EHFlags5/state0->terminate。没有额外Finish或第二次成员Ref扫描。
    ReleaseYtlReference(body.dialogAt08);Retire(body);
}
Body* DeleteTitleSceneNativeAt4A71E0(Body* body,std::uint32_t flags) noexcept
{DestroyTitleSceneNativeAt4A71E0(*body);if(flags&1u)::operator delete(body,std::size_t{0x1C});return body;}
}
