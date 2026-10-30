#include "SaveLoadSystemPageNative.Internal.h"
#include <Runtime/SaveLoadPageSecondNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadSystemPageNativeDetail;
thread_local SaveLoadSystemPageNativeServices* activeServices;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSaveLoadSystemPageNativeAt4FE0A0(&Page(p), flags); return p; }
}
const GuiDialogNativeVtable35& SaveLoadSystemPageNativeVtable() noexcept
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
        BIND(deletingDestructorAt04, Delete);
        BIND(resetInputAt3C, ResetInput);
        BIND(priorityInputAt40, PriorityInput);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(endAt4C, End);
        BIND(resultAt50, Result);
        BIND(compositeAt5C, Composite);
        BIND(drawAt60, Draw);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
namespace SaveLoadSystemPageNativeDetail
{
const GuiDialogNativeVtable35& PageBaseVtable() noexcept
{
    // 两类都退化到原5BE460/7Ch表，不能各留一份可能分叉的基类实现。
    return SaveLoadPageBaseNativeVtable();
}
}
void DestroySaveLoadSystemPageNativeAt4F93F0(SaveLoadSystemPageNativeBody80h* p) noexcept
{
    p->common.vptrAt00 = &SaveLoadSystemPageNativeDetail::PageBaseVtable();
    ReleaseYtlReference(p->managerAt70); // manager先退役，children仍持event一次。
    DestroyGuiDialogAt4F7AC0(&p->common);
}
SaveLoadSystemPageNativeBody80h* DeleteSaveLoadSystemPageNativeAt4FE0A0(
    SaveLoadSystemPageNativeBody80h* p, std::uint32_t flags) noexcept
{
    DestroySaveLoadSystemPageNativeAt4F93F0(p);
    if (flags & 1u) ::operator delete(p, std::size_t{0x80});
    return p;
}
__declspec(noinline) SaveLoadSystemPageNativeScope::SaveLoadSystemPageNativeScope(
    SaveLoadSystemPageNativeServices& s) noexcept : SaveLoadSystemPageNativeScope(&s) {}
__declspec(noinline) SaveLoadSystemPageNativeScope::SaveLoadSystemPageNativeScope(
    SaveLoadSystemPageNativeServices* s) noexcept
    : previous_(activeServices), surfaces_(s ? &s->surfaces : nullptr),
      graphics_(s ? &s->graphics : nullptr), input_(s ? &s->input : nullptr),
      screen_(s ? &s->screen : nullptr)
{ activeServices = s; }
__declspec(noinline) SaveLoadSystemPageNativeScope::~SaveLoadSystemPageNativeScope() noexcept
{ activeServices = previous_; }
__declspec(noinline) SaveLoadSystemPageNativeServices& RequireSaveLoadSystemPageNativeServices()
{
    if (!activeServices) throw std::logic_error("SaveLoadSystem native services are not bound");
    return *activeServices;
}
}
