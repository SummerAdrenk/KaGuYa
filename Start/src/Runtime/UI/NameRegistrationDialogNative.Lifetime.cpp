#include "NameRegistrationDialogNative.Internal.h"
#include <new>
#include <cstdlib>
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local NameRegistrationDialogNativeServices* activeServices;
using namespace NameRegistrationDialogNativeDetail;
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common* p, void*)
{ return ResolveGuiDirtyRectReference(p->dirtyAt50); }
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteNameRegistrationDialogNativeAt4FCDE0(&Dialog(p), flags); return p; }
Common* __fastcall DeleteModal(Common* p, void*, std::uint32_t flags) noexcept
{
    DestroyNameRegistrationDialogNativeAt4F80D0(&Dialog(p));
    if (flags & 1u) ::operator delete(p, std::size_t{0x80});
    return p;
}
void __fastcall BeginBase(Common* p, void*) { BeginModal(p); }
std::int32_t __fastcall EndBase(Common* p, void*)
{
    ReleaseYtlReference(Dialog(p).backgroundAt5C);
    auto& s = RequireNameRegistrationDialogNativeServices().source;
    s.RootModalFlagAt1B8(Root(s)) = 0;
    return 1;
}
std::int32_t __fastcall ActiveBase(Common* p, void*)
{
    if (p->activeAt4C->typeAt08 != 0) return 0;
    auto& b = Dialog(p);
    const auto latch = b.keyboardLatchAt70;
    if (!latch)
    {
        if (!p->vptrAt00->pressedAt70(p, 5)) return 0;
        b.keyboardLatchAt70 = 1;
        auto* child = p->activeAt4C;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot event = Copy(p->activeAt4C->eventAt3C);
        LocalReference lifetime{event};
        if (event.baseAt00)
        { auto& e = RequireGuiClickedEvent(event); (void)e.vptrAt00->invokeAt00(&e); }
        b.keyboardLatchAt70 = 0;
        lifetime.Reload();
    }
    return latch;
}
[[noreturn]] std::int32_t __fastcall Pure(Common*, void*) { _purecall(); std::terminate(); }
// 析构退化到ModalDialog：与本派生表共用叶槽，业务槽在析构期间不提供派生实现。
const NameRegistrationDialogNativeVtable38& DestructionTable() noexcept
{
    static const auto table = []
    {
        NameRegistrationDialogNativeVtable38 t;
        t.common = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(f, e) t.common.f = reinterpret_cast<decltype(t.common.f)>(&e)
        BIND(dirtyAt00, Dirty);
        BIND(deletingDestructorAt04, DeleteModal);
        BIND(activeInputAt38, ActiveBase);
        BIND(resetInputAt3C, ResetInput);
        BIND(beginAt44, BeginBase);
        BIND(pollAt48, Poll);
        BIND(endAt4C, EndBase);
        BIND(resultAt50, Result);
        BIND(flushDirtyAt58, FlushDirty);
#undef BIND
        t.beginExtraAt8C = reinterpret_cast<decltype(t.beginExtraAt8C)>(&Noop);
        t.pollExtraAt90 = reinterpret_cast<decltype(t.pollExtraAt90)>(&Pure);
        t.endExtraAt94 = reinterpret_cast<decltype(t.endExtraAt94)>(&Noop);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
class ModalTarget final : public IModalDialogRuntimeTargetReference, private IModalDialogRuntimeTarget
{
public:
    explicit ModalTarget(const Slot& slot) noexcept : slot_(slot) {}
    IModalDialogRuntimeTarget* ResolveAt4A5330() override
    {
        selected_ = ResolveEnvironmentObjectReference<Common>(slot_);
        if (!selected_) throw std::logic_error("5071D0 null NameRegist target");
        return this;
    }
private:
    void BeginAtVslot44() override { selected_->vptrAt00->beginAt44(selected_); }
    bool PollAtVslot48() override { return selected_->vptrAt00->pollAt48(selected_) != 0; }
    bool EndAtVslot4C() override { return selected_->vptrAt00->endAt4C(selected_) != 0; }
    std::int32_t ResultAtVslot50() const override { return selected_->vptrAt00->resultAt50(selected_); }
    const Slot& slot_;
    Common* selected_ = nullptr;
};
}
const NameRegistrationDialogNativeVtable38& NameRegistrationDialogNativeVtable() noexcept
{
    static const auto table = []
    {
        NameRegistrationDialogNativeVtable38 t;
        t.common = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(f, e) t.common.f = reinterpret_cast<decltype(t.common.f)>(&e)
        BIND(dirtyAt00, Dirty);
        BIND(deletingDestructorAt04, Delete);
        BIND(upperInputAt34, UpperInput);
        BIND(activeInputAt38, ActiveInput);
        BIND(resetInputAt3C, ResetInput);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(endAt4C, End);
        BIND(resultAt50, Result);
        BIND(processInputAt54, ProcessInput);
        BIND(flushDirtyAt58, FlushDirty);
#undef BIND
        t.beginExtraAt8C = reinterpret_cast<decltype(t.beginExtraAt8C)>(&Noop);
        t.pollExtraAt90 = reinterpret_cast<decltype(t.pollExtraAt90)>(&PollExtra);
        t.endExtraAt94 = reinterpret_cast<decltype(t.endExtraAt94)>(&Noop);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
__declspec(noinline) NameRegistrationDialogNativeScope::NameRegistrationDialogNativeScope(
    NameRegistrationDialogNativeServices& s) noexcept
    : previous_(activeServices), surfaces_(s.surfaces), graphics_(s.graphics), input_(s.input), screen_(s.screen)
{
    edits_.emplace(s.source.NativeEditMaps(),s.source.NativeEditThunks());
    activeServices = &s;
}
__declspec(noinline) NameRegistrationDialogNativeScope::~NameRegistrationDialogNativeScope() noexcept
{ activeServices = previous_; }
__declspec(noinline) NameRegistrationDialogNativeServices& RequireNameRegistrationDialogNativeServices()
{
    if (!activeServices) throw std::logic_error("NameRegist native services are not bound");
    return *activeServices;
}
NameRegistrationDialogNativeBody94h* ConstructNameRegistrationDialogNativeAt4E4340(
    void* storage, NameRegistrationDialogNativeServices& services)
{
    NameRegistrationDialogNativeScope scope(services);
    return ConstructNameRegistrationDialogNativeAt4E4340(storage);
}
void DestroyNameRegistrationDialogNativeAt4F80D0(Body* p) noexcept
{
    // 4F80F8/4FCE08：先退化表，正序释放events，再background，最后5Ch基类。
    // 98h edits是root集合借用，不在这里Remove；CtorEH也不得调用业务End。
    p->common.vptrAt00 = &DestructionTable().common;
    DestroyGuiClickedEventReferencesAt4F6D10(p->eventsAt74);
    ReleaseYtlReference(p->backgroundAt5C);
    DestroyGuiDialogAt4F7AC0(&p->common);
}
Body* DeleteNameRegistrationDialogNativeAt4FCDE0(Body* p, std::uint32_t flags) noexcept
{
    DestroyNameRegistrationDialogNativeAt4F80D0(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x94});
    return p;
}
std::int32_t RunNameRegistrationDialogNativeAt5071D0(void*, const Slot* target,
    IModalDialogRunnerServices& modal)
{
    (void)RequireNameRegistrationDialogNativeServices();
    if (!target) throw std::logic_error("5071D0 missing borrowed Ref address");
    ModalTarget view(*target);
    return ModalDialogRunnerRuntime(modal).RunAt5071D0(view);
}
namespace NameRegistrationDialogNativeDetail
{
void __fastcall Noop(Common*, void*) {} // PE 5086B0/51C100实际ret，不是缺叶默认成功。
std::int32_t __fastcall Result(Common* p, void*) { return Dialog(p).resultAt68; }
void __fastcall ResetInput(Common* p, void*)
{
    Dialog(p).keyboardLatchAt70 = 0;
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
}
}
