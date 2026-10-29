#include "SettingSceneNative.Internal.h"
#include <bit>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace SettingSceneNativeDetail;
namespace
{
thread_local SettingSceneNativeServices* active = nullptr;
// 4C3630 原分派：重读 vptr，只覆盖 AL，保留 Draw 的 EAX 高位，callee 弹一个参数。
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
__declspec(naked) void __fastcall EmptyDraw(Body*, void*) { __asm { _emit 0xC3 } }
void __fastcall Update(Body* body, void*)
{ UpdateSettingSceneNativeAt4BA800(*body, RequireSettingSceneNativeServices()); }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteSettingSceneNativeAt4A7020(body, flags); }
std::uint8_t __fastcall PureRun(Body*, void*, std::int32_t)
{ _purecall(); std::terminate(); }
struct RetiredTaskTable final
{
    decltype(Table::runAt00) run;
    decltype(Table::deleteAt04) destroy;
};
Body* __fastcall DeleteTask(Body*, void*, std::uint32_t) noexcept;
const RetiredTaskTable retired{
    reinterpret_cast<decltype(RetiredTaskTable::run)>(&PureRun),
    reinterpret_cast<decltype(RetiredTaskTable::destroy)>(&DeleteTask)};
static_assert(sizeof(RetiredTaskTable) == 8);
Body* __fastcall DeleteTask(Body* body, void*, std::uint32_t flags) noexcept
{
    RetireTask(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{4});
    return body;
}
const Table methods{
    reinterpret_cast<decltype(Table::runAt00)>(&RawRun),
    reinterpret_cast<decltype(Table::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),
    reinterpret_cast<decltype(Table::drawAt0C)>(&EmptyDraw)};
class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* child) noexcept
        : EnvironmentObjectReferenceControl(child, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4C6100 与 Info 原桥相同：只读 payload，判空后调用真实 GUI 删除槽 +04(1)。
        auto* child = static_cast<GuiDialogNativeBody5Ch*>(Read(payloadAt14));
        if (child) Read(child->vptrAt00)->deletingDestructorAt04(child, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl) == 0x18);
struct TaskUnwind final
{
    Body& body;
    bool armed = true;
    ~TaskUnwind() noexcept { if (armed) RetireTask(body); }
};
struct MemberUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool armed = true;
    ~MemberUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
};
struct RawUnwind final
{
    void* raw;
    IEnvironmentRootMemory& memory;
    std::size_t bytes;
    bool armed = true;
    ~RawUnwind() noexcept { if (armed) memory.DeleteBody(raw, bytes); }
};
void* CaptureConstructorRegistry(SettingSceneNativeServices& services)
{
    auto* system = services.source.CaptureEnvironmentAt4180C0();
    // 4A1A57..9D 的内联 lookup 先 end 后 begin；不能改成 417990 的相反读序。
    const auto end = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.end));
    const auto begin = reinterpret_cast<std::uintptr_t>(Read(system->archivesAt30.begin));
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(end - begin) / 12);
    if (count <= 2u) return nullptr;
    return Resolve<void>(*reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + 0x18u));
}
void Adopt(YtlReferenceSlotRuntime& slot, GuiDialogNativeBody5Ch* child, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!child) return;
    if (auto* old = Read(slot.controlAt08)) old->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(child);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) DialogControl(child) : nullptr;
    slot.controlAt08 = control;
    // 4A1C34：保留 null control+04 故障；不转换 bad_alloc 或返回成功空 Ref。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    Read(slot.controlAt08)->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->strideAt0C = 0xAC;
    static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08))->countAt10 = 1;
}
}
namespace SettingSceneNativeDetail
{
void RetireTask(Body& body) noexcept { body.vptrAt00 = reinterpret_cast<const Table*>(&retired); }
}
SettingSceneNativeScope::SettingSceneNativeScope(SettingSceneNativeServices& services) noexcept
    : previous_(active)
{
    if(auto* runtime=services.source.NativeRuntimeServices())dialog_.emplace(*runtime);
    active=&services;
}
SettingSceneNativeScope::~SettingSceneNativeScope() noexcept { active = previous_; }
SettingSceneNativeServices& RequireSettingSceneNativeServices()
{
    if (!active) throw std::logic_error("Setting native scene requires its external service scope");
    return *active;
}
const Table& SettingSceneNativeMethods() noexcept { return methods; }
Body* ConstructSettingSceneNativeAt4A1A00(Body& body, SettingSceneNativeServices& services)
{
    SettingSceneNativeScope scope(services);
    body.vptrAt00 = &methods;
    TaskUnwind task{body}; // state0 -> 59CD60 -> ITask 退化；不恢复注册值。
    body.dialogAt14 = {};
    MemberUnwind member{body.dialogAt14}; // state1 -> 59CD68 -> Ref14 释放。
    body.savedRegistryAt08 = Read(RegistryWord(CaptureConstructorRegistry(services), 4));
    body.savedFlagsAt0C = Read(RegistryWord(CaptureConstructorRegistry(services), 0x14));
    *static_cast<volatile std::uint32_t*>(&RegistryWord(CaptureConstructorRegistry(services), 4)) = 0;
    auto& flags = RegistryWord(CaptureConstructorRegistry(services), 0x14);
    *static_cast<volatile std::uint32_t*>(&flags) = Read(flags) & 2u;
    body.phaseAt04 = 0;
    auto* raw = services.memory.AllocateBody(0xAC);
    GuiDialogNativeBody5Ch* child = nullptr;
    {
        RawUnwind cleanup{raw, services.memory, 0xAC}; // state2 仅保护 0ACh 构造调用。
        if (raw) child = services.source.ConstructSettingDialogAt4F1BF0(raw);
        cleanup.armed = false;
    }
    // GUI 完成后已回 state1；new18 抛出留下孤立 GUI，原 EH 不补删它。
    Adopt(body.dialogAt14, child, services.memory);
    member.armed = false;
    task.armed = false;
    return &body;
}
Body* NewSettingSceneNativeAt4AB649(SettingSceneNativeServices& services)
{
    auto* raw = services.memory.AllocateBody(0x20);
    if (!raw) return nullptr;
    RawUnwind cleanup{raw, services.memory, 0x20};
    auto* body = ConstructSettingSceneNativeAt4A1A00(*::new(raw) Body, services);
    cleanup.armed = false;
    return body;
}
void DestroySettingSceneNativeAt4A7020(Body& body) noexcept
{
    // 5F0A88 state0 -> terminate：先 Ref，正常返回后才 ITask 退化，不写 phase/备份/bias。
    ReleaseYtlReference(body.dialogAt14);
    RetireTask(body);
}
Body* DeleteSettingSceneNativeAt4A7020(Body* body, std::uint32_t flags) noexcept
{
    DestroySettingSceneNativeAt4A7020(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x20});
    return body;
}
std::uint8_t RunSettingSceneNativeAt4C3630(Body& body, std::int32_t argument, SettingSceneNativeServices& services)
{
    SettingSceneNativeScope scope(services);
    return Read(body.vptrAt00)->runAt00(&body, argument);
}
}
