#pragma once

#include <Runtime/GuiControlFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
struct GuiMultiHitResourceNative8h final
{
    const char* name;
    std::int32_t hitSlot;
};
// Shigokare 4E3AD0：每种视觉状态分别指定alpha命中资源，不是多选勾选框。
struct GuiMultiCheckButtonDescriptorNative44h final
{
    std::int32_t event;
    std::array<GuiMultiHitResourceNative8h, 7> states;
    const POINT* position;
    std::int32_t childId;
};
static_assert(sizeof(GuiMultiCheckButtonDescriptorNative44h) == 0x44);
struct GuiMultiCheckButtonNativeBody78h final
{
    GuiButtonNativeBody5Ch button;
    std::array<std::int32_t, 7> hitSlots;
};
static_assert(sizeof(GuiMultiCheckButtonNativeBody78h) == 0x78);
static_assert(offsetof(GuiMultiCheckButtonNativeBody78h, button) == 0);
const GuiButtonNativeVtable29& GuiMultiCheckButtonMethods() noexcept;
YtlReferenceSlotRuntime* CreateGuiMultiCheckButtonNative(YtlReferenceSlotRuntime& output,
    void* parent, const GuiMultiCheckButtonDescriptorNative44h&, IGuiControlResourceLoadServices&,
    IGraphicsSurfaceApi&, IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
}
