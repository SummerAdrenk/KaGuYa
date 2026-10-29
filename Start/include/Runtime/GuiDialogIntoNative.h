#pragma once

#include <Runtime/GuiDialogNative.h>

namespace KaGuYa::Start::Runtime
{
// 元素为 dialog Ref12；类型相同不表示可按控件50h解析其正文。
using GuiDialogChildrenStorage = GuiControlChildrenStorage;
static_assert(sizeof(GuiDialogChildrenStorage) == 0x0C);
static_assert(alignof(GuiDialogChildrenStorage) == 4);
static_assert(offsetof(GuiDialogChildrenStorage, begin) == 0);
static_assert(offsetof(GuiDialogChildrenStorage, end) == 4);
static_assert(offsetof(GuiDialogChildrenStorage, capacity) == 8);

// 只建立原存储的寿命，不值初始化common或提前清+5C；无附加owner。
struct GuiDialogIntoNativeBody68h final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    GuiDialogIntoNativeBody68h() noexcept {}
};
static_assert(sizeof(GuiDialogIntoNativeBody68h) == 0x68);
static_assert(alignof(GuiDialogIntoNativeBody68h) == 4);
static_assert(offsetof(GuiDialogIntoNativeBody68h, common) == 0);
static_assert(offsetof(GuiDialogIntoNativeBody68h, dialogsAt5C) == 0x5C);
static_assert(std::is_standard_layout_v<GuiDialogIntoNativeBody68h>);
static_assert(std::is_trivially_destructible_v<GuiDialogIntoNativeBody68h>);

// PE 5BDD48..5BDDD8共37槽；5BDDDC的5DA0D8为下一类RTTI，不入表。
struct GuiDialogIntoNativeVtable37 final
{
    GuiDialogNativeVtable35 common;
    std::int32_t (__thiscall* appendDialogAt8C)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime);
    YtlReferenceSlotRuntime* (__thiscall* findDialogAt90)(GuiDialogNativeBody5Ch*,
        YtlReferenceSlotRuntime* freshOutput, std::int32_t id);
};
static_assert(sizeof(GuiDialogIntoNativeVtable37) == 0x94);
static_assert(alignof(GuiDialogIntoNativeVtable37) == 4);
static_assert(offsetof(GuiDialogIntoNativeVtable37, common) == 0);
static_assert(offsetof(GuiDialogIntoNativeVtable37, appendDialogAt8C) == 0x8C);
static_assert(offsetof(GuiDialogIntoNativeVtable37, findDialogAt90) == 0x90);
static_assert(std::is_standard_layout_v<GuiDialogIntoNativeVtable37>);

const GuiDialogIntoNativeVtable37& GuiDialogIntoBaseVtable() noexcept;
// 4E0BE0只构造common；成功后执行各派生构造内联的清+5C并安装Into表。
// 可传派生正文首地址；本函数不分配68h正文，也不添加第二个ownedObject。
GuiDialogIntoNativeBody68h* ConstructGuiDialogIntoNativeAt4E0BE0(void* storage,
    GuiDialogNativeBody5Ch* parent, std::int32_t id);
void DestroyGuiDialogIntoNativeAt4F7B80(GuiDialogNativeBody5Ch* body) noexcept;

std::int32_t NestedGuiDialogReferenceCount(const GuiDialogChildrenStorage&) noexcept;
std::int32_t NestedGuiDialogReferenceCapacity(const GuiDialogChildrenStorage&) noexcept;
// 借用source；允许source别名旧元素以及中间插入，不消费调用方Ref。
YtlReferenceSlotRuntime* GrowNestedGuiDialogReferencesAt4D97A0(GuiDialogChildrenStorage&,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source);
void AppendNestedGuiDialogReference(GuiDialogChildrenStorage&, const YtlReferenceSlotRuntime& source);
void ReleaseNestedGuiDialogReferencesRangeAt4D8020(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
// 供派生正常尾直接销毁+5C；不改vptr，不触及common，也不跳入完整析构。
void DestroyNestedGuiDialogReferencesAt4F6DC0(GuiControlChildrenStorage&) noexcept;
}
