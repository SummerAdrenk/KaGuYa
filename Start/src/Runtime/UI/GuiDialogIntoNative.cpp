#include "GuiDialogIntoNative.Internal.h"

#include <new>

namespace KaGuYa::Start::Runtime
{
const GuiDialogIntoNativeVtable37& GuiDialogIntoBaseVtable() noexcept
{
    // 逐槽ASM核见工作文档；仅相同原目标复用已有函数指针，不复制基类核。
    static const auto table = []() noexcept
    {
        using namespace GuiDialogIntoNativeDetail;
        GuiDialogIntoNativeVtable37 result{GuiDialogBaseVtable(), {}, {}};
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
        // noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
        // 按值栈参数 YtlReferenceSlotRuntime 为 12 字节；无聚合返回或新增隐藏参数。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define INTO_SLOT(name, entry) result.common.name = reinterpret_cast<decltype(result.common.name)>(&entry)
        INTO_SLOT(deletingDestructorAt04, Delete);
        INTO_SLOT(setDirtyAt18, SetDirty);
        INTO_SLOT(beginAt44, Begin);
        INTO_SLOT(pollAt48, Poll);
        INTO_SLOT(endAt4C, End);
        INTO_SLOT(compositeAt5C, Composite);
        INTO_SLOT(drawAt60, Draw);
#undef INTO_SLOT
        result.appendDialogAt8C = reinterpret_cast<decltype(result.appendDialogAt8C)>(&Append);
        result.findDialogAt90 = reinterpret_cast<decltype(result.findDialogAt90)>(&Find);
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return result;
    }();
    return table;
}

GuiDialogIntoNativeBody68h* ConstructGuiDialogIntoNativeAt4E0BE0(void* storage,
    GuiDialogNativeBody5Ch* parent, std::int32_t id)
{
    auto* body = ::new(storage) GuiDialogIntoNativeBody68h;
    // 失败由已有4E0BE0清common；+5C尚未获得清理资格，必须保留原字节。
    ConstructGuiDialogAt4E0BE0(&body->common, parent, id);
    body->dialogsAt5C = {};
    body->common.vptrAt00 = &GuiDialogIntoBaseVtable().common;
    return body;
}

void DestroyGuiDialogIntoNativeAt4F7B80(GuiDialogNativeBody5Ch* body) noexcept
{
    body->vptrAt00 = &GuiDialogIntoBaseVtable().common;
    DestroyNestedGuiDialogReferencesAt4F6DC0(GuiDialogIntoNativeDetail::Dialogs(*body));
    DestroyGuiDialogAt4F7AC0(body);
}

namespace GuiDialogIntoNativeDetail
{
Common* __fastcall Delete(Common* body, void*, std::uint32_t flags) noexcept
{
    DestroyGuiDialogIntoNativeAt4F7B80(body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x68});
    return body;
}
}
}
