#include <Runtime/LoadAfterSelectionNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/GameSceneRoutes.h>
#include <bit>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void ILoadAfterSelectionNativeSource::RestorePrimaryGauge()
{ throw std::logic_error("Load-after gauge restoration source is absent"); }
namespace
{
using Body=LoadAfterSelectionNativeBody58h;
using Table=LoadAfterSelectionNativeVtable4;
thread_local LoadAfterSelectionNativeServices* current=nullptr;
LoadAfterSelectionNativeServices& Services()
{if(!current)throw std::logic_error("LoadAfter native call requires external services");return *current;}
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
// 4C3630：晚读vptr、保留Draw的EAX高位/调用者ESI及ret4，不能经C++便利包装。
__declspec(naked) std::uint8_t __fastcall RawRun(Body*,void*,std::int32_t)
{
    __asm {
        push esi
        mov esi, ecx
        mov eax, [esi]
        call dword ptr [eax+8]
        mov eax, [esi]
        mov ecx, esi
        call dword ptr [eax+0Ch]
        mov al, 1
        pop esi
        ret 4
    }
}
Body* __fastcall Delete(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteLoadAfterSelectionNativeAt4A6A40(body,flags);}
void __fastcall Update(Body* body,void*){UpdateLoadAfterSelectionNativeAt4B6380(*body,Services());}
// 4B5E20显式C3；普通C++空函数不能保证EAX不变。
__declspec(naked) void __fastcall EmptyDraw(Body*,void*){__asm {_emit 0xC3}}
std::uint8_t __fastcall PureRun(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteTask(Body* body,void*,std::uint32_t flags) noexcept;
struct RetiredTable final{decltype(Table::runAt00) run;decltype(Table::deleteAt04) destroy;};
const RetiredTable retired{reinterpret_cast<decltype(RetiredTable::run)>(&PureRun),
    reinterpret_cast<decltype(RetiredTable::destroy)>(&DeleteTask)};
void Retire(Body& body) noexcept{body.vptrAt00=reinterpret_cast<const Table*>(&retired);}
Body* __fastcall DeleteTask(Body* body,void*,std::uint32_t flags) noexcept
{Retire(*body);if(flags&1u)::operator delete(body,std::size_t{4});return body;}
const Table methods{reinterpret_cast<decltype(Table::runAt00)>(&RawRun),
    reinterpret_cast<decltype(Table::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),
    reinterpret_cast<decltype(Table::drawAt0C)>(&EmptyDraw)};
struct TaskUnwind final{Body& body;bool armed=true;~TaskUnwind() noexcept{if(armed)Retire(body);}};
struct RefUnwind final
{YtlReferenceSlotRuntime& ref;bool armed=true;~RefUnwind() noexcept{if(armed)ReleaseYtlReference(ref);}};
void ResetPacket(TransitOperation& op) noexcept
{
    // 4C57F0的写序；不是赋值现代强引用包，也不清拥有Ref。
    auto& write=static_cast<volatile TransitOperation&>(op);
    write.destination=0;write.previous=0;write.source=nullptr;write.mask=0;
    write.y=0;write.x=0;write.clip=nullptr;write.opcode=-1;
    write.targetKind=0;write.current=0;write.target=255;write.range=0;
}
void Begin(Body& body,YtlReferenceSlotRuntime& ref,std::int32_t selector,ILoadAfterSelectionNativeSource& source)
{
    ResetPacket(body.operationAt24);body.operationAt24.opcode=selector;
    body.operationAt24.source=ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(ref);
    const auto step=source.ReadStepAt497900();
    // 4B6400先inc状态，再保存497900返回的EAX；捕获抛出时状态不推进。
    ++body.phaseAt04;body.stepAt54=step;
}
void Animate(Body& body,ILoadAfterSelectionNativeSource& source)
{
    auto& op=body.operationAt24;
    if(!source.ReadGateAt6F()||source.AccelerateAt4AA970())
    {op.current=Add(Read(op.current),256);if(Read(op.current)>=Read(op.target))op.current=Read(op.target);}
    if(Read(op.opcode)==-1||Read(op.current)>=Read(op.target))++body.phaseAt04;
    source.RenderAt4C39E0(op);
    // Render回调可改packet/step；不能用绘制前的缓存替代当前值。
    op.current=Add(Read(op.current),Read(body.stepAt54));
    if(Read(op.current)>=Read(op.target))op.current=Read(op.target);
    source.CommitAt4B02E0();
}
constexpr char script[]="\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67\x8F\x88\x97\x9D";
constexpr char map[]="\x83\x7D\x83\x62\x83\x76\x8F\x88\x97\x9D";
void RestoreNormal(Body& body,std::int32_t route,ILoadAfterSelectionNativeSource& source)
{
    const auto marker=route==3?3:route==4?1:route==5?0:2;
    source.WriteSceneMarkerAt38(marker);source.RestorePatternsAt4C14E0();
    if(route!=3)
    {
        source.RebuildTextAt4C1650(1);
        if(HasPrimaryGauge())source.RestorePrimaryGauge();
    }
    source.PropagatePatternsAt4C4250();source.DrawRegisteredAt4AC490();
    YtlReferenceSlotRuntime temporary;
    source.CaptureScreenAt4ACDD0(temporary);RefUnwind temporaryCleanup{temporary};
    CopyAssignGraphicsSurfaceReferenceAt465870(body.backupAt14,temporary);
    // 4B66C6先撤caller资格，再调40D2F0；后者重读当前control并清base/control。
    temporaryCleanup.armed=false;
    try{ReleaseYtlReference(temporary);}
    catch(...){std::terminate();}
    source.ClearScreenAt4AA890();source.ClearCommandAt437680();
    Begin(body,body.backupAt14,58,source);
}
}
LoadAfterSelectionNativeScope::LoadAfterSelectionNativeScope(LoadAfterSelectionNativeServices& services) noexcept
    :previous_(current),surfaces_(services.surfaces){current=&services;}
LoadAfterSelectionNativeScope::~LoadAfterSelectionNativeScope() noexcept{current=previous_;}
const Table& LoadAfterSelectionNativeMethods() noexcept{return methods;}
Body* ConstructLoadAfterSelectionNativeAt4AB47B(Body& body,LoadAfterSelectionNativeServices& services)
{
    LoadAfterSelectionNativeScope scope(services);body.vptrAt00=&methods;TaskUnwind task{body};
    body.blackAt08={};RefUnwind black{body.blackAt08};
    body.backupAt14={};RefUnwind backup{body.backupAt14};ResetPacket(body.operationAt24);
    if(services.source.LoadBlackAt480000(body.blackAt08,"bg_black.bgd"))
    {
        body.savedModeAt20=services.source.ReadModeAt04();services.source.WriteModeAt04(0);
        body.phaseAt04=0;services.source.WriteRootByteAt218(1);services.source.WriteRootByteAt219(1);
    }
    // false仍返回原58h，phase/+20/+54保持未写；不得抛现代构造失败或补齐默认状态。
    backup.armed=black.armed=task.armed=false;return &body;
}
Body* NewLoadAfterSelectionNativeAt4AB45F(LoadAfterSelectionNativeServices& services)
{
    LoadAfterSelectionNativeScope scope(services);
    auto* raw=services.memory.AllocateBody(0x58);if(!raw)return nullptr;
    struct RawUnwind final
    {void* raw;IEnvironmentRootMemory& memory;bool armed=true;~RawUnwind() noexcept{if(armed)memory.DeleteBody(raw,0x58);}}
        cleanup{raw,services.memory};
    auto* result=ConstructLoadAfterSelectionNativeAt4AB47B(*::new(raw) Body,services);cleanup.armed=false;return result;
}
void DestroyLoadAfterSelectionNativeAt4A6A40(Body& body) noexcept
{ReleaseYtlReference(body.backupAt14);ReleaseYtlReference(body.blackAt08);Retire(body);}
Body* DeleteLoadAfterSelectionNativeAt4A6A40(Body* body,std::uint32_t flags) noexcept
{DestroyLoadAfterSelectionNativeAt4A6A40(*body);if(flags&1u)::operator delete(body,std::size_t{0x58});return body;}
std::uint8_t RunLoadAfterSelectionNativeAt4C3630(Body& body,std::int32_t,LoadAfterSelectionNativeServices& services)
{
    LoadAfterSelectionNativeScope scope(services);
    Read(body.vptrAt00)->updateAt08(&body);Read(body.vptrAt00)->drawAt0C(&body);return 1;
}
void UpdateLoadAfterSelectionNativeAt4B6380(Body& body,LoadAfterSelectionNativeServices& services)
{
    LoadAfterSelectionNativeScope scope(services);auto& source=services.source;
    switch(Read(body.phaseAt04))
    {
    case 0:Begin(body,body.blackAt08,57,source);break;
    case 1:case 3:Animate(body,source);break;
    case 2:
    {
        source.WriteRootSubobjectDwordAt10(1);source.StopMusicAt4383B0();source.ClearVoicesAt4384D0();
        source.StopMappedEffectsAt413270();const auto route=source.RestoreScriptAt4D4CC0();
        switch(route)
        {
        case 3:case 4:case 5:case 14:RestoreNormal(body,route,source);break;
        case 9:case 13:
            source.WriteModeAt04(1);source.ClearScenesAt4C1460();source.ClearScriptFlagsAt4B6766();
            source.PushActiveAt4A9030(script);
            source.PushActiveAt4A9030(route==9?map:GameFixedActionSceneName(ScrVmFixedAction::Code13).data());break;
        default:source.ResetAndQueueTitleAt47E180();break;
        }
        break;
    }
    case 4:source.WriteModeAt04(1);source.QueueAt47F920(script);break;
    default:break;
    }
}
}
