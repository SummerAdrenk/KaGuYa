#include "TitleDialogNative.Internal.h"

#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local TitleDialogNativeServices* activeServices; }

const TitleDialogNativeVtable36& TitleDialogNativeVtable() noexcept
{
    static const auto table = []
    {
        TitleDialogNativeVtable36 value;
        value.common = GuiDialogBaseVtable();
        using namespace TitleDialogNativeDetail;
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, entry) value.common.field = reinterpret_cast<decltype(value.common.field)>(&entry)
        SLOT(dirtyAt00, Dirty);
        SLOT(deletingDestructorAt04, Delete);
        SLOT(activeInputAt38, ActiveInput);
        SLOT(resetInputAt3C, ResetInput);
        SLOT(beginAt44, Begin);
        SLOT(pollAt48, Poll);
        SLOT(endAt4C, End);
        SLOT(resultAt50, Result);
        SLOT(childChangedAt88, ChildChanged);
#undef SLOT
        value.redrawAt8C = reinterpret_cast<decltype(value.redrawAt8C)>(&Redraw);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
__declspec(noinline) TitleDialogNativeScope::TitleDialogNativeScope(TitleDialogNativeServices& services) noexcept
    : previous_(activeServices), surfaces_(services.surfaces), graphics_(services.graphics),
      input_(services.input), screen_(services.screen), transit_(services.transit)
{
    if(auto* name=services.source.NameRegistrationServices())name_.emplace(*name);
    activeServices = &services;
}
__declspec(noinline) TitleDialogNativeScope::~TitleDialogNativeScope() noexcept
{ activeServices = previous_; }
__declspec(noinline) TitleDialogNativeServices& RequireTitleDialogNativeServices()
{
    if (!activeServices) throw std::logic_error("TitleDialog native services are not bound");
    return *activeServices;
}
TitleDialogNativeBodyD8h* ConstructTitleDialogNativeAt4F4530(void* storage, TitleDialogNativeServices& services)
{
    TitleDialogNativeScope scope(services);
    return ConstructTitleDialogNativeAt4F4530(storage);
}
TitleDialogNativeBodyD8h* DeleteTitleDialogNativeAt4FE690(TitleDialogNativeBodyD8h* body,
    std::uint32_t flags) noexcept
{
    body->common.vptrAt00 = &TitleDialogNativeVtable().common;
    ReleaseYtlReference(body->clickedManagerAtC8);
    ReleaseYtlReference(body->canvasAt84);
    ReleaseYtlReference(body->blackAt78);
    DestroyGuiDialogAt4F7AC0(&body->common);
    if (flags & 1u) ::operator delete(body, std::size_t{0xD8});
    return body;
}
namespace TitleDialogNativeDetail
{
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common* body, void*)
{ return ResolveGuiDirtyRectReference(body->dirtyAt50); }
Common* __fastcall Delete(Common* body, void*, std::uint32_t flags) noexcept
{ DeleteTitleDialogNativeAt4FE690(&Dialog(body), flags); return body; }
}
}
