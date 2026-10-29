#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// GuiDialog/Popup+0C的唯一三指针正文；元素为原child Ref12，不持现代控件owner。
using GuiControlChildrenStorage = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(GuiControlChildrenStorage) == 0x0C);
static_assert(alignof(GuiControlChildrenStorage) == 4);
static_assert(offsetof(GuiControlChildrenStorage, begin) == 0);
static_assert(offsetof(GuiControlChildrenStorage, end) == 4);
static_assert(offsetof(GuiControlChildrenStorage, capacity) == 8);
static_assert(std::is_trivially_copyable_v<GuiControlChildrenStorage>);

std::int32_t GuiControlChildReferenceCount(const GuiControlChildrenStorage& children) noexcept;
std::int32_t GuiControlChildReferenceCapacity(const GuiControlChildrenStorage& children) noexcept;

// 4D93B0：支持中间插入；分配之后才读source，旧范围释放后才发布三指针。
YtlReferenceSlotRuntime* GrowGuiControlChildrenAt4D93B0(GuiControlChildrenStorage& children,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source);
// Popup内联快路复制增持，满时转4D93B0；借用source，不消费调用方拥有的Ref。
void AppendGuiControlChildReference(GuiControlChildrenStorage& children,
    const YtlReferenceSlotRuntime& source);

// 范围入口固定begin/end；payload后重读当前元素control，成功只清base/control。
// child资源释放依赖由外部scope提供，不进入三指针正文；释放异常必须终止。
void ReleaseGuiControlChildrenRangeAt4D7F00(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
// begin为空不改end/capacity；范围释放后重读当前头，按capacity释放存储后才清头。
void DestroyGuiControlChildrenAt4F6C60(GuiControlChildrenStorage& children) noexcept;
}
