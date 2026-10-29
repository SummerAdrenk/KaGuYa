#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct GuiClickedEventNativeBody;
struct GuiClickedEventNativeVtable final
{
    std::int32_t (__thiscall* invokeAt00)(GuiClickedEventNativeBody*);
    void (__thiscall* clearAt04)(GuiClickedEventNativeBody*);
    GuiClickedEventNativeBody* (__thiscall* deletingDestructorAt08)(GuiClickedEventNativeBody*, std::uint32_t);
};
// 5BDC58：三个原字段；没有业务owner、allocator指针或另一份flag。
struct GuiClickedEventNativeBody final
{
    const GuiClickedEventNativeVtable* vptrAt00;
    std::int32_t clickedFlagAt04;
    std::int32_t eventValueAt08;
};
static_assert(sizeof(GuiClickedEventNativeBody) == 0x0C);
static_assert(alignof(GuiClickedEventNativeBody) == 4);
static_assert(offsetof(GuiClickedEventNativeBody, clickedFlagAt04) == 4);
static_assert(offsetof(GuiClickedEventNativeBody, eventValueAt08) == 8);
static_assert(std::is_trivially_destructible_v<GuiClickedEventNativeBody>);
static_assert(sizeof(GuiClickedEventNativeVtable) == 0x0C);
static_assert(offsetof(GuiClickedEventNativeVtable, invokeAt00) == 0);
static_assert(offsetof(GuiClickedEventNativeVtable, clearAt04) == 4);
static_assert(offsetof(GuiClickedEventNativeVtable, deletingDestructorAt08) == 8);
using GuiClickedEventReferences = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
static_assert(sizeof(GuiClickedEventReferences) == 0x0C);

const GuiClickedEventNativeVtable& GuiClickedEventVtable() noexcept;
// 4D7B90：manager为原三指针0Ch正文；旧槽退役后发布base，new18失败不补删正文。
void AdoptGuiClickedEventManagerAt4D7B90(YtlReferenceSlotRuntime&,GuiClickedEventReferences*);
// source为已选控件+04的借用字段，正文分配成功后才读；调用期间源须存活。
GuiClickedEventNativeBody* NewGuiClickedEventAt4EA7B3(const std::int32_t& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// fresh Ref，不release旧输出；null正文仍清三字段，非空先发布base再new18。
YtlReferenceSlotRuntime* ConstructGuiClickedEventReferenceAt4D7780(YtlReferenceSlotRuntime& output,
    GuiClickedEventNativeBody* body, std::uint8_t owning,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// Popup内联工厂：先new正文，后清输出；control初始化/发布次序与4D7780分开。
YtlReferenceSlotRuntime* CreatePopupClickedEventAt4EA7B3(YtlReferenceSlotRuntime& output,
    const std::int32_t& source, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// Tool在正文分配后才解析child Ref并读取+04；不可提前捕获child指针或ID。
YtlReferenceSlotRuntime* CreateToolClickedEventAt4E2F5A(YtlReferenceSlotRuntime& output,
    const YtlReferenceSlotRuntime& child, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
GuiClickedEventNativeBody* ResolveGuiClickedEvent(const YtlReferenceSlotRuntime& reference) noexcept;
GuiClickedEventNativeBody& RequireGuiClickedEvent(const YtlReferenceSlotRuntime& reference);
// 原逐DWORD构造复制并增持；目标须为fresh槽，不是赋值/按值参数消费。
void CopyGuiClickedEventReference(YtlReferenceSlotRuntime& output,
    const YtlReferenceSlotRuntime& source) noexcept;
void ReleaseGuiClickedEventReferenceAt4F64D0(YtlReferenceSlotRuntime& reference) noexcept;
// Popup正常局部Ref尾释放用已选ESI；payload后不重读槽control。state23异常终止。
// 4EA8B1的事件按值临时Ref释放也使用同一缓存协议；与展开4F64D0分开。
void ReleasePopupClickedEventTemporaryAt4EA9BC(YtlReferenceSlotRuntime& temporary,
    YtlReferenceControlRuntime* capturedControl) noexcept;

std::int32_t GuiClickedEventReferenceCount(const GuiClickedEventReferences& references) noexcept;
std::int32_t GuiClickedEventReferenceCapacity(const GuiClickedEventReferences& references) noexcept;
// 4D9620总是重分配，position是分配前的位置；返回发布后的begin+原index。
YtlReferenceSlotRuntime* GrowGuiClickedEventReferencesAt4D9620(GuiClickedEventReferences& references,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 通用Ref vector追加：三字段复制，借用source，不消费调用者的引用。
void AppendGuiClickedEventReference(GuiClickedEventReferences& references,
    const YtlReferenceSlotRuntime& source, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// Popup原快路使用本次ESI、bias=0；增长路用source并在返回后更新ESI。
// capturedControl由调用者从原对应选取点保存，不能在入口无条件从source重取。
void AppendPopupClickedEventReferenceAt4EA97F(GuiClickedEventReferences& references,
    const YtlReferenceSlotRuntime& source, YtlReferenceControlRuntime*& capturedControl,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 532200及50DDE0内联同序：固定迭代指针、每轮重读end并解析当前Ref，不增持。
void ClearGuiClickedEventFlagsAt532200(GuiClickedEventReferences& references);
// 50FDB0固定begin/end和本轮base，value与clear前分别重解bias/control；仅首个flag。
std::int32_t PollGuiClickedEventManagerNativeAt50FDB0(GuiClickedEventReferences*,std::int32_t clearMatched);
// 4D7F90捕获begin/end后正序释放；只处理范围，不改外层三指针头。
void ReleaseGuiClickedEventRangeAt4D7F90(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
void DestroyGuiClickedEventReferencesAt4F6D10(GuiClickedEventReferences& references,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;

// 与GuiDirtyRectNative一致：正文/control的memory分配须兼容全局sized delete；
// vector用memory.DeleteVector。没有业务服务依赖，不需要虚构一个正文内或隐藏scope owner。
}
