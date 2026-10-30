#include "SaveLoadSceneNative.Internal.h"
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadSceneNativeDetail;
namespace
{
thread_local SaveLoadSceneNativeServices* active = nullptr;
SaveLoadSceneNativeServices& Services()
{
    if (!active) throw std::logic_error("SaveLoad native call requires its external service scope");
    return *active;
}
// 原4C3630的21字节分派：晚读vptr，只覆盖AL，保留Draw留下的EAX高位和调用者ESI。
__declspec(naked) std::uint8_t __fastcall RawRun(Body*, void*, std::int32_t)
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
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteSaveLoadSceneNativeAt4A6F90(body, flags); }
void __fastcall Update(Body* body, void*)
{ UpdateSaveLoadSceneNativeAt4BA650(*body, Services()); }
// 原4B5E20只有ret；不能让现代空函数的调试序言改写EAX。
__declspec(naked) void __fastcall EmptyDraw(Body*, void*) { __asm { _emit 0xC3 } }
std::uint8_t __fastcall PureRun(Body*, void*, std::int32_t)
{ _purecall(); std::terminate(); }
Body* __fastcall DeleteTask(Body* body, void*, std::uint32_t flags) noexcept;
struct RetiredTaskTable final
{
    decltype(Table::runAt00) run;
    decltype(Table::deleteAt04) destroy;
};
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const RetiredTaskTable retired{
    reinterpret_cast<decltype(RetiredTaskTable::run)>(&PureRun),
    reinterpret_cast<decltype(RetiredTaskTable::destroy)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
Body* __fastcall DeleteTask(Body* body, void*, std::uint32_t flags) noexcept
{
    RetireTask(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{4});
    return body;
}
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::runAt00)>(&RawRun),
    reinterpret_cast<decltype(Table::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),
    reinterpret_cast<decltype(Table::drawAt0C)>(&EmptyDraw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4C60F0先判payload，再调raw dialog的slot04(1)；不删除现代C++外观。
        auto* body = static_cast<GuiDialogNativeBody5Ch*>(Read(payloadAt14));
        if (body) Read(body->vptrAt00)->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);
}
namespace SaveLoadSceneNativeDetail
{
void RetireTask(Body& body) noexcept
{ body.vptrAt00 = reinterpret_cast<const Table*>(&retired); }
void AdoptDialogAt4A1850(YtlReferenceSlotRuntime& slot, GuiDialogNativeBody5Ch* body,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    // 4A188D沿旧control分支得到ECX=0，不增加第二次拥有正文释放。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) DialogControl(body) : nullptr;
    slot.controlAt08 = control;
    // 4A18BD也发布NULL；4A18C0随后在NULL+04写故障，不改为bad_alloc。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    Read(slot.controlAt08)->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C = 0x184;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10 = 1;
}
}
SaveLoadSceneNativeScope::SaveLoadSceneNativeScope(SaveLoadSceneNativeServices& services) noexcept
    : previous_(active)
{
    if(auto* runtime=services.dialog.NativeRuntimeServices())dialog_.emplace(*runtime);
    active=&services;
}
SaveLoadSceneNativeScope::~SaveLoadSceneNativeScope() noexcept { active = previous_; }
const Table& SaveLoadSceneNativeMethods() noexcept { return methods; }
void DestroySaveLoadSceneNativeAt4A6F90(Body& body) noexcept
{
    // 5EF3E0的state0展开到terminate；Ref释放晚读control，不清bias。
    ReleaseYtlReference(body.dialogAt08);
    RetireTask(body);
}
Body* DeleteSaveLoadSceneNativeAt4A6F90(Body* body, std::uint32_t flags) noexcept
{
    DestroySaveLoadSceneNativeAt4A6F90(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x24});
    return body;
}
std::uint8_t RunSaveLoadSceneNativeAt4C3630(Body& body, std::int32_t argument, SaveLoadSceneNativeServices& services)
{
    SaveLoadSceneNativeScope scope(services);
    // 现代便利包装只承诺uint8_t返回；table内raw Run独立承担完整寄存器ABI。
    return Read(body.vptrAt00)->runAt00(&body, argument);
}
}
