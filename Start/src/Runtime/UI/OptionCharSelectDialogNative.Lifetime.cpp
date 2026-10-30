#include "OptionCharSelectDialogNative.Internal.h"

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local OptionCharSelectDialogNativeDependencies* activeDependencies;
}
const OptionCharSelectDialogNativeVtable38& OptionCharSelectDialogNativeVtable() noexcept
{
    static const auto table = []
    {
        // 26个同址槽直接继承；原9个覆写槽单独绑定，绝不扩展到+98。
        OptionCharSelectDialogNativeVtable38 value;
        value.common = GuiDialogBaseVtable();
        using namespace OptionCharSelectNativeDetail;
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, entry) value.common.field = reinterpret_cast<decltype(value.common.field)>(&entry)
        SLOT(dirtyAt00, Dirty);
        SLOT(deletingDestructorAt04, Delete);
        SLOT(upperInputAt34, UpperInput);
        SLOT(activeInputAt38, ActiveInput);
        SLOT(resetInputAt3C, ResetInput);
        SLOT(beginAt44, Begin);
        SLOT(pollAt48, Poll);
        SLOT(endAt4C, End);
        SLOT(resultAt50, Result);
#undef SLOT
        value.countAt8C = reinterpret_cast<decltype(value.countAt8C)>(&Count);
        value.resetAt90 = reinterpret_cast<decltype(value.resetAt90)>(&Reset);
        value.attachDirtyAt94 = reinterpret_cast<decltype(value.attachDirtyAt94)>(&AttachDirty);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
__declspec(noinline) OptionCharSelectDialogNativeScope::OptionCharSelectDialogNativeScope(
    OptionCharSelectDialogNativeDependencies& dependencies) noexcept
    : previous_(activeDependencies), surfaces_(dependencies.surfaces), graphics_(dependencies.graphics),
      input_(dependencies.input), screen_(dependencies.screen)
{ activeDependencies = &dependencies; }
__declspec(noinline) OptionCharSelectDialogNativeScope::~OptionCharSelectDialogNativeScope() noexcept
{ activeDependencies = previous_; }
__declspec(noinline) OptionCharSelectDialogNativeDependencies& RequireOptionCharSelectDialogNativeDependencies()
{
    if (!activeDependencies) throw std::logic_error("OptionCharSelect native source is not bound");
    return *activeDependencies;
}
OptionCharSelectDialogNativeBody8Ch* DeleteOptionCharSelectDialogAt4FCEB0(
    OptionCharSelectDialogNativeBody8Ch* body, std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &OptionCharSelectDialogNativeVtable().common;
    // 4FCEB0 state0/EHFlags5：异常终止；payload回调后重读+64，保留+60。
    ReleaseYtlReference(body->clickedManagerAt5C);
    DestroyGuiDialogAt4F7AC0(&body->common);
    if (flags & 1) ::operator delete(body, std::size_t{0x8C});
    return body;
}
namespace OptionCharSelectNativeDetail
{
GuiDirtyRectNativeBody80h* __fastcall Dirty(Body* body, void*)
{ return ResolveGuiDirtyRectReference(body->dirtyAt50); }
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ DeleteOptionCharSelectDialogAt4FCEB0(&Dialog(body), flags); return body; }
}
}
