#include "GeneralConfirmDialogNative.Internal.h"
#include <cstdlib>
#include <exception>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace GeneralConfirmNativeDetail;
thread_local GeneralConfirmDialogNativeServices* services = nullptr;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ return DeleteGeneralConfirmDialogNativeAt4FCAC0(p, flags); }
Common* __fastcall DeleteBase(Common* p, void*, std::uint32_t flags) noexcept
{
    DestroyMembers(Dialog(p));
    if (flags & 1u) ::operator delete(p, std::size_t{0x80});
    return p;
}
std::int32_t __fastcall PureEvents(Common*, void*) { _purecall(); std::terminate(); }
GeneralConfirmDialogNativeVtable38 MakeTable()
{
    GeneralConfirmDialogNativeVtable38 table{};
    table.common = GuiDialogBaseVtable();
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, function) table.common.field = reinterpret_cast<decltype(table.common.field)>(&function)
    SLOT(deletingDestructorAt04, Delete); SLOT(activeInputAt38, ActiveInput); SLOT(resetInputAt3C, ResetInput);
    SLOT(beginAt44, Begin); SLOT(pollAt48, Poll); SLOT(endAt4C, End); SLOT(resultAt50, Result); SLOT(flushDirtyAt58, Flush);
#undef SLOT
    table.beginHookAt8C = reinterpret_cast<decltype(table.beginHookAt8C)>(&BeginHook);
    table.readEventsAt90 = reinterpret_cast<decltype(table.readEventsAt90)>(&ReadEvents);
    table.endHookAt94 = reinterpret_cast<decltype(table.endHookAt94)>(&EndHook);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
    return table;
}
class GeneralControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit GeneralControl(Common* p) noexcept : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* p = static_cast<Common*>(payloadAt14);
        if (p) p->vptrAt00->deletingDestructorAt04(p, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(GeneralControl) == 0x18);
}
GeneralConfirmDialogNativeScope::GeneralConfirmDialogNativeScope(GeneralConfirmDialogNativeServices& value) noexcept
    : GeneralConfirmDialogNativeScope(&value) {}
GeneralConfirmDialogNativeScope::GeneralConfirmDialogNativeScope(GeneralConfirmDialogNativeServices* value) noexcept
    : previous_(services), surfaces_(value ? &value->surfaces : nullptr), graphics_(value ? &value->graphics : nullptr),
      input_(value ? &value->input : nullptr), screen_(value ? &value->screen : nullptr)
{ services = value; }
GeneralConfirmDialogNativeScope::~GeneralConfirmDialogNativeScope() noexcept { services = previous_; }
GeneralConfirmDialogNativeServices& RequireGeneralConfirmDialogNativeServices()
{
    if (!services) throw std::logic_error("GeneralConfirm native services are not bound");
    return *services;
}
const GeneralConfirmDialogNativeVtable38& GeneralConfirmDialogNativeVtable() noexcept
{ static const auto table = MakeTable(); return table; }
namespace GeneralConfirmNativeDetail
{
const GeneralConfirmDialogNativeVtable38& ModalBaseTable() noexcept
{
    static const auto table = []
    {
        auto value = MakeTable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        value.common.deletingDestructorAt04 = reinterpret_cast<decltype(value.common.deletingDestructorAt04)>(&DeleteBase);
        value.readEventsAt90 = reinterpret_cast<decltype(value.readEventsAt90)>(&PureEvents);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
void ReleaseUncleared(Slot& slot)
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
void RefLifetime::Finish(bool clear) noexcept
{
    armed = false;
    if (!slot.controlAt08) return;
    ReleaseUncleared(slot);
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void RefLifetime::FinishCaptured(YtlReferenceControlRuntime* control, bool clear) noexcept
{
    armed = false;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    if (clear) { slot.controlAt08 = nullptr; slot.baseAt00 = 0; }
}
void DestroyMembers(Body& body) noexcept
{
    // 4F80F8/4FCAE8先恢复Modal基表，再events/capture/GuiDialog；析构不补End或gate。
    body.common.vptrAt00 = &ModalBaseTable().common;
    DestroyGuiClickedEventReferencesAt4F6D10(body.eventsAt74);
    ReleaseYtlReference(body.capturedAt5C);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
}
void DestroyGeneralConfirmDialogNativeAt4F80D0(GuiDialogNativeBody5Ch* p) noexcept
{ GeneralConfirmNativeDetail::DestroyMembers(GeneralConfirmNativeDetail::Dialog(p)); }
GuiDialogNativeBody5Ch* DeleteGeneralConfirmDialogNativeAt4FCAC0(GuiDialogNativeBody5Ch* p, std::uint32_t flags) noexcept
{
    DestroyGeneralConfirmDialogNativeAt4F80D0(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x84});
    return p;
}
YtlReferenceSlotRuntime* ConstructGeneralConfirmDialogNativeReferenceAt4515E0(
    YtlReferenceSlotRuntime& output, GuiDialogNativeBody5Ch* p, std::uint8_t owning)
{
    output = {};
    if (p)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(p);
        void* raw = ::operator new(0x18);
        auto* control = raw ? ::new(raw) GeneralControl(p) : nullptr;
        output.controlAt08 = control;
        // 451622原null control写+04故障；volatile保留x86写点，不改成成功空Ref。
        *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
        output.controlAt08->owningAt08 = owning;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x84;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    }
    return &output;
}
YtlReferenceSlotRuntime* CreateGeneralConfirmDialogNativeReference(YtlReferenceSlotRuntime& output, std::int32_t mode)
{
    void* raw = ::operator new(0x84);
    GeneralConfirmNativeDetail::RawLifetime lifetime{raw, 0x84};
    auto* body = raw ? ConstructGeneralConfirmDialogNativeAt4E3330(raw, mode) : nullptr;
    lifetime.armed = false; // 485D88/423837撤raw保护后才分配Ref控制块。
    return ConstructGeneralConfirmDialogNativeReferenceAt4515E0(output, body, 1);
}
GuiDialogNativeBody5Ch& GeneralConfirmDialogNativeModalTarget::Resolve() const
{ return *ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference_); }
IModalDialogRuntimeTarget* GeneralConfirmDialogNativeModalTarget::ResolveAt4A5330()
{ return ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference_) ? this : nullptr; }
void GeneralConfirmDialogNativeModalTarget::BeginAtVslot44() { auto& p = Resolve(); p.vptrAt00->beginAt44(&p); }
bool GeneralConfirmDialogNativeModalTarget::PollAtVslot48() { auto& p = Resolve(); return p.vptrAt00->pollAt48(&p) != 0; }
bool GeneralConfirmDialogNativeModalTarget::EndAtVslot4C() { auto& p = Resolve(); return p.vptrAt00->endAt4C(&p) != 0; }
std::int32_t GeneralConfirmDialogNativeModalTarget::ResultAtVslot50() const
{ auto& p = Resolve(); return p.vptrAt00->resultAt50(&p); }
std::int32_t RunGeneralConfirmDialogNativeAt5071D0(YtlReferenceSlotRuntime& reference)
{
    GeneralConfirmDialogNativeModalTarget target(reference);
    ModalDialogRunnerRuntime runner(RequireGeneralConfirmDialogNativeServices().modal);
    return runner.RunAt5071D0(target);
}
GeneralConfirmDialogNativeSession::GeneralConfirmDialogNativeSession(GeneralConfirmDialogNativeServices& value, std::int32_t mode)
    : scope_(value)
{ CreateGeneralConfirmDialogNativeReference(reference_, mode); }
GeneralConfirmDialogNativeSession::~GeneralConfirmDialogNativeSession() noexcept { ReleaseYtlReference(reference_); }
GuiDialogNativeBody5Ch& GeneralConfirmDialogNativeSession::Body() const
{ return *ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference_); }
void GeneralConfirmDialogNativeSession::ConsumeRootCaptureAt477C60()
{ ConsumeRootGeneralConfirmDialogNativeCaptureAt477C60(&Body()); }
std::int32_t GeneralConfirmDialogNativeSession::RunAt5071D0() { return RunGeneralConfirmDialogNativeAt5071D0(reference_); }
}
