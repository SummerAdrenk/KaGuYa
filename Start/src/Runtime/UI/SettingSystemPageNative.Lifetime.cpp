#include "SettingSystemPageNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SettingSystemPageNativeDetail;
thread_local SettingSystemPageNativeServices* active = nullptr;
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ DeleteSettingSystemPageNativeAt4FE410(&Page(p), flags); return p; }
}
const GuiDialogNativeVtable35& SettingSystemPageNativeVtable() noexcept
{
    static const auto table = [] {
        using namespace SettingSystemPageNativeDetail;
        auto t = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define BIND(field, entry) t.field = reinterpret_cast<decltype(t.field)>(&entry)
        BIND(deletingDestructorAt04, Delete);
        BIND(beginAt44, Begin);
        BIND(pollAt48, Poll);
        BIND(endAt4C, End);
        BIND(resultAt50, Result);
#undef BIND
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return t;
    }();
    return table;
}
void DestroySettingSystemPageNativeAt4FE410(SettingSystemPageNativeBody7Ch* b) noexcept
{
    // 原state0终止边界：Setting表 -> manager Ref -> 5Ch基类；不调用End/不写result。
    b->common.vptrAt00 = &SettingSystemPageNativeVtable();
    ReleaseYtlReference(b->managerAt70);
    DestroyGuiDialogAt4F7AC0(&b->common);
}
SettingSystemPageNativeBody7Ch* DeleteSettingSystemPageNativeAt4FE410(
    SettingSystemPageNativeBody7Ch* b, std::uint32_t flags) noexcept
{
    DestroySettingSystemPageNativeAt4FE410(b);
    if (flags & 1u) ::operator delete(b, std::size_t{0x7C});
    return b;
}
SettingSystemPageNativeScope::SettingSystemPageNativeScope(SettingSystemPageNativeServices& s) noexcept
    : SettingSystemPageNativeScope(&s) {}
SettingSystemPageNativeScope::SettingSystemPageNativeScope(SettingSystemPageNativeServices* s) noexcept
    : previous_(active), surfaces_(s ? &s->surfaces : nullptr), graphics_(s ? &s->graphics : nullptr),
      input_(s ? &s->input : nullptr), screen_(s ? &s->screen : nullptr) { active = s; }
SettingSystemPageNativeScope::~SettingSystemPageNativeScope() noexcept { active = previous_; }
SettingSystemPageNativeServices& RequireSettingSystemPageNativeServices()
{
    if (!active) throw std::logic_error("SettingSystem native services are not bound");
    return *active;
}
}
