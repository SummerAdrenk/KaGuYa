#include "GraffitiPageNative.Internal.h"
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/GraffitiCountMultiNative.h>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GraffitiPageNativeBody20h;
using Table = GraffitiPageNativeVtable;
thread_local GraffitiPageNativeServices* active = nullptr;

std::uint8_t __fastcall PureRun(TaskNativeBody4h*, void*, std::int32_t)
{ _purecall(); std::terminate(); }
TaskNativeBody4h* __fastcall DeleteTask(TaskNativeBody4h* body, void*, std::uint32_t flags) noexcept;
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const TaskNativeVtable retired{
    reinterpret_cast<decltype(TaskNativeVtable::runAt00)>(&PureRun),
    reinterpret_cast<decltype(TaskNativeVtable::deleteAt04)>(&DeleteTask)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
TaskNativeBody4h* __fastcall DeleteTask(TaskNativeBody4h* body, void*, std::uint32_t flags) noexcept
{
    // 469A70：已退役ITask只删除4字节，不重放派生Ref释放。
    body->vptrAt00 = &retired;
    if (flags & 1u) ::operator delete(body, std::size_t{4});
    return body;
}
std::uint8_t __fastcall Run(Body* body, void*, std::int32_t argument)
{ return RunGraffitiPageNativeAt4C3630(*body, argument, GraffitiPageNativeDetail::RequireServices()); }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteGraffitiPageNativeAt4A6920(body, flags); }
void __fastcall Update(Body* body, void*)
{ UpdateGraffitiPageNativeAt4B6130(*body, GraffitiPageNativeDetail::RequireServices()); }
void __fastcall Draw(Body* body, void*)
{ DrawGraffitiPageNativeAt4B5860(*body, GraffitiPageNativeDetail::RequireServices()); }
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::runAt00)>(&Run),
    reinterpret_cast<decltype(Table::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),
    reinterpret_cast<decltype(Table::drawAt0C)>(&Draw)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// 两个不同的原类型控制表；不在18h控制块内附加Source/owner。
template<bool Weight> class DialogControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DialogControl(GuiDialogNativeBody5Ch* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GuiDialogNativeBody5Ch*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DialogControl<false>) == 0x18 && sizeof(DialogControl<true>) == 0x18);

struct TaskUnwind final
{
    Body& body;
    bool armed = true;
    ~TaskUnwind() noexcept { if (armed) GraffitiPageNativeDetail::RetireTask(body); }
};
struct RefUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool armed = true;
    // 原4A3310的5F0B40 EHFlags=5；区别于ctor中的普通inline释放可抛。
    ~RefUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
};
struct RawUnwind final
{
    void* raw;
    std::size_t size;
    IEnvironmentRootMemory& memory;
    bool armed = true;
    ~RawUnwind() noexcept { if (armed) memory.DeleteBody(raw, size); }
};
GuiDialogNativeBody5Ch* NewDialog(bool weight, GraffitiPageNativeServices& services)
{
    // 源PE 49DED7 push B8h、49DF83 stride B8h；不是请求中提到的A8h。
    const std::size_t size = weight ? 0xB8 : 0xB4;
    auto* raw = services.memory.AllocateBody(size);
    if (!raw) return nullptr;
    // 59C803/59C815仅在4E13A0/4F4BC0未完成时删raw；返回后立即撤销资格。
    RawUnwind unwind{raw, size, services.memory};
    auto* dialog = weight ? services.dialogs.ConstructWeightAt4F4BC0(raw) :
        services.dialogs.ConstructLikableAt4E13A0(raw);
    unwind.armed = false;
    return dialog;
}
std::uint32_t& SlotDword(EnvironmentRootStorage* root, std::uint32_t index)
{
    // 49DD45/49DD96直接解引用；空/越界仍访问+4，不改为现代异常或默认0。
    auto* archive = ResolveEnvironmentArchiveAt417990(*root, index);
    return *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive) + 4u);
}
void PublishMulti(YtlReferenceSlotRuntime& slot, GraffitiCountDialogNativeServices& count,
    IEnvironmentRootMemory& memory)
{
    auto* dialog = NewGraffitiCountMultiNative(count);
    try
    {
        void* raw = memory.AllocateBody(sizeof(DialogControl<false>));
        if (!raw) throw std::bad_alloc{};
        auto* control = ::new(raw) DialogControl<false>(dialog);
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = sizeof(GraffitiCountMultiNativeBody);
        control->countAt10 = 1;
        slot = {reinterpret_cast<std::uintptr_t>(dialog), 0, control};
    }
    catch (...)
    {
        dialog->vptrAt00->deletingDestructorAt04(dialog, 1);
        throw;
    }
}
}
namespace GraffitiPageNativeDetail
{
GraffitiPageNativeServices& RequireServices()
{
    if (!active) throw std::logic_error("Graffiti native Source scope is not bound");
    return *active;
}
void RetireTask(Body& body) noexcept { body.vptrAt00 = reinterpret_cast<const Table*>(&retired); }
void PublishDialog(YtlReferenceSlotRuntime& slot, GuiDialogNativeBody5Ch* dialog, bool weight,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!dialog) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(dialog);
    auto* raw = memory.AllocateBody(0x18);
    EnvironmentObjectReferenceControl* control = nullptr;
    if (raw) control = weight ? static_cast<EnvironmentObjectReferenceControl*>(::new(raw) DialogControl<true>(dialog)) :
        static_cast<EnvironmentObjectReferenceControl*>(::new(raw) DialogControl<false>(dialog));
    slot.controlAt08 = control;
    // 49DEBA/49DF6F对null也写+04；volatile保留故障与部分发布顺序。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = weight ? 0xB8 : 0xB4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
}
GraffitiPageNativeScope::GraffitiPageNativeScope(GraffitiPageNativeServices& services) noexcept
    : previous_(active) { active = &services; }
GraffitiPageNativeScope::~GraffitiPageNativeScope() noexcept { active = previous_; }
const Table& GraffitiPageNativeMethods() noexcept { return methods; }
const TaskNativeVtable& GraffitiPageRetiredTaskMethods() noexcept { return retired; }
Body* ConstructGraffitiPageNativeAt49DCA0(Body& body, GraffitiPageNativeServices& services)
{
    GraffitiPageNativeScope scope(services);
    auto* count = services.dialogs.CountDialogServices();
    if (!count || !count->resources) throw std::logic_error("Count: game profile is not bound");
    ValidateCountResourceProfile(*count->resources);
    if (count->resources->kind == CountResourceKind::None)
        throw std::logic_error("Count: this game has no graffiti count scene");
    GraffitiCountDialogNativeScope countScope(*count);
    body.vptrAt00 = &methods;
    TaskUnwind task{body}; // 59C7F0 state0 -> ITask退役。
    ::new(static_cast<void*>(&body.dialogAt08)) YtlReferenceSlotRuntime{};
    RefUnwind ref{body.dialogAt08}; // 59C7F8 state1 -> Ref释放，再展开task。
    body.drawGateAt1C = 1;
    body.savedSlot2At18 = GraffitiPageNativeDetail::Read(SlotDword(services.source.CaptureEnvironmentAt4180C0(), 2));
    SlotDword(services.source.CaptureEnvironmentAt4180C0(), 2) = 0;
    body.phaseAt04 = 0;
    if (count->resources->kind == CountResourceKind::MultiGauge)
    {
        // 16:488970直接选择多计量条，不把prog_param00当模式。
        PublishMulti(body.dialogAt08, *count, services.memory);
        ref.armed = false;
        task.armed = false;
        return &body;
    }
    // 49DDA4即使slot0为空也继续查询；这个接收者不负责读参数值。
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*services.source.CaptureEnvironmentAt4180C0(), 0));
    const auto receiver = services.source.CaptureParamsRootAfter413BE0();
    const auto index = ResolveScriptNamedValueNativeAt498020("prog_param00", services.names, services.strings);
    auto& params = receiver.paramsAt47BDB0(receiver.root);
    const bool weight = ReadScriptValueAt4C73A0(params, index, services.errors) != 0;
    GraffitiPageNativeDetail::PublishDialog(body.dialogAt08, NewDialog(weight, services), weight, services.memory);
    ref.armed = false;
    task.armed = false;
    return &body;
}
Body* NewGraffitiPageNative(GraffitiPageNativeServices& services)
{
    auto* raw = services.memory.AllocateBody(0x20);
    if (!raw) return nullptr;
    RawUnwind unwind{raw, 0x20, services.memory};
    auto* body = ConstructGraffitiPageNativeAt49DCA0(*::new(raw) Body, services);
    unwind.armed = false;
    return body;
}
void DestroyGraffitiPageNativeAt4A6920(Body& body) noexcept
{
    // 5F0BE4 EHFlags=5：释放抛出终止；回调期间保持当前vptr，结束才退役。
    ReleaseYtlReference(body.dialogAt08);
    GraffitiPageNativeDetail::RetireTask(body);
}
Body* DeleteGraffitiPageNativeAt4A6920(Body* body, std::uint32_t flags) noexcept
{
    DestroyGraffitiPageNativeAt4A6920(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x20});
    return body;
}
}
