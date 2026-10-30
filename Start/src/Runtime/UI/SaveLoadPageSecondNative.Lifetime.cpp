#include "SaveLoadPageSecondNative.Internal.h"
#include <array>
#include <cstdlib>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageSecondNativeDetail;
thread_local SaveLoadPageSecondNativeServices* activeServices;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSaveLoadPageSecondNativeAt4FDFB0(&Page(p), flags); return p; }
Common* __fastcall DeleteBase(Common* p, void*, std::uint32_t flags) noexcept
{
    p->vptrAt00 = &SaveLoadPageBaseNativeVtable();
    ReleaseYtlReference(Page(p).managerAt70);
    DestroyGuiDialogAt4F7AC0(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x7C}); // 4FDF10，不用于Second表。
    return p;
}
[[noreturn]] std::int32_t __fastcall PureChange(Common*, void*, std::int32_t, std::int32_t, std::int32_t)
{ _purecall(); std::terminate(); }
void __fastcall Begin(Common* p, void*) { BeginSaveLoadPageSecondNativeAt50EB50(p); }
std::int32_t __fastcall Poll(Common* p, void*) { return PollSaveLoadPageSecondNativeAt52F8B0(p); }
std::int32_t __fastcall End(Common* p, void*) { return EndSaveLoadPageSecondNativeAt5094C0(p); }
std::int32_t __fastcall Result(Common* p, void*) { return ResultSaveLoadPageSecondNativeAt50ABC0(p); }
std::int32_t __fastcall Active(Common* p, void*) { return ActiveInputSaveLoadPageSecondNativeAt511080(p); }
void __fastcall Reset(Common* p, void*) { ResetInputSaveLoadPageSecondNativeAt510790(p); }
std::int32_t __fastcall Priority(Common* p, void*) { return PriorityInputSaveLoadPageSecondNativeAt511620(p); }
void __fastcall Process(Common* p, void*) { ProcessInputSaveLoadPageSecondNativeAt519CE0(p); }
void __fastcall Composite(Common* p, void*, Slot r, const RECT* clip)
{ CompositeSaveLoadPageBaseNativeAt5249C0(p, r, clip); }
void __fastcall Draw(Common* p, void*, const RECT* clip) { DrawSaveLoadPageBaseNativeAt524960(p, clip); }
constexpr std::array<std::uint32_t, 36> originalSlots{{
    0x509ED0,0x4FDFB0,0x5025A0,0x50A5D0,0x4FF020,0x509D20,0x536310,0x536500,
    0x50FF40,0x538660,0x510630,0x500800,0x532170,0x51E740,0x511080,0x510790,
    0x511620,0x50EB50,0x52F8B0,0x5094C0,0x50ABC0,0x519CE0,0x525FB0,0x5249C0,
    0x524960,0x50FC80,0x522E90,0x510030,0x5100E0,0x50A6A0,0x50B920,0x510220,
    0x510170,0x5102B0,0x518A80,0x541C5F}};
}
const GuiDialogNativeVtable35& SaveLoadPageBaseNativeVtable() noexcept
{
    static const auto table = []
    {
        auto t = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(field, entry) t.field = reinterpret_cast<decltype(t.field)>(&entry)
        BIND(deletingDestructorAt04, DeleteBase);
        BIND(endAt4C, End);
        BIND(resultAt50, Result);
        BIND(compositeAt5C, Composite);
        BIND(drawAt60, Draw);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t; // +44/+48保留GuiDialog原purecall。
    }();
    return table;
}
const SaveLoadPageSecondNativeVtable36& SaveLoadPageSecondNativeVtable() noexcept
{
    static const auto table = []
    {
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
        SaveLoadPageSecondNativeVtable36 t{SaveLoadPageBaseNativeVtable(),
            reinterpret_cast<decltype(SaveLoadPageSecondNativeVtable36::changePageAt8C)>(&PureChange)};
#define BIND(field, entry) t.inheritedGui35.field = reinterpret_cast<decltype(t.inheritedGui35.field)>(&entry)
        BIND(deletingDestructorAt04, Delete);
        BIND(activeInputAt38, Active);
        BIND(resetInputAt3C, Reset);
        BIND(priorityInputAt40, Priority);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(processInputAt54, Process);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
std::span<const std::uint32_t, 36> SaveLoadPageSecondNativeOriginalSlots() noexcept { return originalSlots; }
GuiDialogNativeBody5Ch* ConstructSaveLoadPageSecondNativeAt4EF5E0(void* rawAC, Common* parent, std::int32_t id)
{
    auto* b = ::new(rawAC) Body;
    ConstructGuiDialogAt4E0BE0(&b->common, parent, id);
    b->managerAt70 = {};
    b->common.vptrAt00 = &SaveLoadPageSecondNativeVtable().inheritedGui35;
    b->canvasAt80 = {};
    b->surfacesAt8C = {};
    b->hoverSurface = {};
    return &b->common;
}
void DestroySaveLoadPageSecondNativeAt4F9470(Body* b) noexcept
{
    b->common.vptrAt00 = &SaveLoadPageSecondNativeVtable().inheritedGui35;
    ReleaseYtlReference(b->hoverSurface);
    DestroyGuiControlSurfaceReferencesAt43EAA0(b->surfacesAt8C);
    ReleaseYtlReference(b->canvasAt80);
    b->common.vptrAt00 = &SaveLoadPageBaseNativeVtable();
    ReleaseYtlReference(b->managerAt70);
    DestroyGuiDialogAt4F7AC0(&b->common);
}
Body* DeleteSaveLoadPageSecondNativeAt4FDFB0(Body* b, std::uint32_t flags) noexcept
{
    DestroySaveLoadPageSecondNativeAt4F9470(b);
    if (flags & 1u) ::operator delete(b, sizeof(Body));
    return b;
}
__declspec(noinline) SaveLoadPageSecondNativeScope::SaveLoadPageSecondNativeScope(SaveLoadPageSecondNativeServices& s) noexcept
    : SaveLoadPageSecondNativeScope(&s) {}
__declspec(noinline) SaveLoadPageSecondNativeScope::SaveLoadPageSecondNativeScope(SaveLoadPageSecondNativeServices* s) noexcept
    : previous_(activeServices), surfaces_(s ? &s->surfaces : nullptr), graphics_(s ? &s->graphics : nullptr),
      input_(s ? &s->input : nullptr), screen_(s ? &s->screen : nullptr)
{ activeServices = s; }
__declspec(noinline) SaveLoadPageSecondNativeScope::~SaveLoadPageSecondNativeScope() noexcept { activeServices = previous_; }
__declspec(noinline) SaveLoadPageSecondNativeServices& RequireSaveLoadPageSecondNativeServices()
{
    if (!activeServices) throw std::logic_error("SaveLoadPageSecond native services are not bound");
    return *activeServices;
}
}
