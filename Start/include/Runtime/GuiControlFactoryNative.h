#pragma once

#include <Runtime/GuiControlNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
// 原描述符保持null与非null空串的区别，不以std::string::empty推断字段。
struct GuiStaticDescriptorNative24h final
{
    std::int32_t eventAt00;
    std::array<const char*, 7> namesAt04;
    const POINT* positionAt20;
};
struct GuiCheckBoxDescriptorNative2Ch final
{
    GuiStaticDescriptorNative24h common;
    std::int32_t hitSlotAt24;
    std::int32_t childIdAt28;
};
static_assert(sizeof(GuiStaticDescriptorNative24h) == 0x24);
static_assert(offsetof(GuiStaticDescriptorNative24h, positionAt20) == 0x20);
static_assert(sizeof(GuiCheckBoxDescriptorNative2Ch) == 0x2C);
static_assert(offsetof(GuiCheckBoxDescriptorNative2Ch, childIdAt28) == 0x28);
// Popup单组Checkbox描述符与5041D0的Button描述符字段同形；构造算法仍各自独立。
using GuiButtonDescriptorNative2Ch = GuiCheckBoxDescriptorNative2Ch;

// 5086C0无业务区参数展开保护；只在正常尾消费已拥有Ref。
void MoveGuiControlResourceAt5086C0(YtlReferenceSlotRuntime ownedGroup,
    std::int32_t x, std::int32_t y);
YtlReferenceSlotRuntime* CreateGuiStaticAt505800(YtlReferenceSlotRuntime& output,
    void* parent, const GuiStaticDescriptorNative24h& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 505570的fresh返回槽与按值Surface输入；本入口不走loader、也不捕获根。
YtlReferenceSlotRuntime* CreateGuiStaticFromSurfaceAt505570(YtlReferenceSlotRuntime& output,
    void* parent, std::int32_t event, YtlReferenceSlotRuntime ownedSurface,
    IGraphicsSurfaceApi& surfaces);
YtlReferenceSlotRuntime* CreateGuiButtonAt5041D0(YtlReferenceSlotRuntime& output,
    void* parent, const GuiButtonDescriptorNative2Ch& descriptor,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
