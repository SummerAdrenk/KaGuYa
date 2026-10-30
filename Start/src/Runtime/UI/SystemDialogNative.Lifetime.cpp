#include "SystemDialogNative.Internal.h"
#include <cstdlib>
#include <exception>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SystemDialogNativeDetail;
thread_local SystemDialogNativeServices* activeServices;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSystemDialogNativeAt4FE5F0(&Dialog(p), flags); return p; }
Common* __fastcall DeleteModal(Common* p, void*, std::uint32_t flags) noexcept
{
    DestroySystemDialogNativeAt4F80D0(&Dialog(p));
    if (flags & 1u) ::operator delete(p, std::size_t{0x80}); // 原Modal 4FCB60。
    return p;
}
void __fastcall BeginBase(Common* p, void*) { BeginModalAt50CF50(p); }
std::int32_t __fastcall EndBase(Common* p, void*) { return EndModalAt508E80(p); }
[[noreturn]] std::int32_t __fastcall Pure(Common*, void*) { _purecall(); std::terminate(); }
SystemDialogNativeVtable38 MakeModal() noexcept
{
    SystemDialogNativeVtable38 t;
    // 原表全部继承槽与5BDCB8相同，只有以下Modal业务槽不同；不遗漏非常用槽。
    t.common = GuiDialogBaseVtable();
    // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
    // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
    // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(f, e) t.common.f = reinterpret_cast<decltype(t.common.f)>(&e)
    BIND(deletingDestructorAt04, DeleteModal);
    BIND(activeInputAt38, ActiveInput);
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
}
}
const SystemDialogNativeVtable38& SystemDialogNativeModalVtable() noexcept
{ static const auto table = MakeModal(); return table; }
const SystemDialogNativeVtable38& SystemDialogNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = MakeModal();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        t.common.deletingDestructorAt04 = reinterpret_cast<decltype(t.common.deletingDestructorAt04)>(&Delete);
        t.common.beginAt44 = reinterpret_cast<decltype(t.common.beginAt44)>(&Begin);
        t.common.endAt4C = reinterpret_cast<decltype(t.common.endAt4C)>(&End);
        t.common.processInputAt54 = reinterpret_cast<decltype(t.common.processInputAt54)>(&ProcessInput);
        t.pollExtraAt90 = reinterpret_cast<decltype(t.pollExtraAt90)>(&PollExtra);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
__declspec(noinline) SystemDialogNativeScope::SystemDialogNativeScope(SystemDialogNativeServices& s) noexcept
    : SystemDialogNativeScope(&s) {}
__declspec(noinline) SystemDialogNativeScope::SystemDialogNativeScope(SystemDialogNativeServices* s) noexcept
    : previous_(activeServices), surfaces_(s?&s->surfaces:nullptr), graphics_(s?&s->graphics:nullptr),
      input_(s?&s->input:nullptr), screen_(s?&s->screen:nullptr)
{ activeServices = s; }
__declspec(noinline) SystemDialogNativeScope::~SystemDialogNativeScope() noexcept
{ activeServices = previous_; }
__declspec(noinline) SystemDialogNativeServices& RequireSystemDialogNativeServices()
{
    if (!activeServices) throw std::logic_error("SystemDialog native services are not bound");
    return *activeServices;
}
void DestroySystemDialogNativeAt4F80D0(Body* p) noexcept
{
    p->common.vptrAt00 = &SystemDialogNativeModalVtable().common;
    DestroyGuiClickedEventReferencesAt4F6D10(p->eventsAt74);
    ReleaseYtlReference(p->backgroundAt5C);
    DestroyGuiDialogAt4F7AC0(&p->common);
}
Body* DeleteSystemDialogNativeAt4FE5F0(Body* p, std::uint32_t flags) noexcept
{
    DestroySystemDialogNativeAt4F80D0(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x88});
    return p;
}
namespace SystemDialogNativeDetail
{
void __fastcall Noop(Common*, void*) {} // 5086B0、51C100均为真实ret。
std::int32_t __fastcall Result(Common* p, void*) { return Read(Dialog(p).resultAt68); }
std::int32_t __fastcall PollExtra(Common* p, void*) { return Read(Dialog(p).resultAt68) != -1; }
void __fastcall ResetInput(Common* p, void*)
{
    Dialog(p).keyboardLatchAt70 = 0;
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}
}
}
