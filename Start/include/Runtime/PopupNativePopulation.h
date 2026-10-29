#pragma once

#include <Runtime/GuiControlChildrenNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/PopupMenuSample18Data.h>

namespace KaGuYa::Start::Runtime
{
// 480000只捕获已选receiver；非空目标直接复用，同一24h正文交给4800A0。
bool LoadPopupStampAt480000(YtlReferenceSlotRuntime& output, void* capturedReceiver,
    const char* name, IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 只实现4EA0D1..4EAAE6的原资源区间；factory和GameInput由A4h构造函数拥有。
// parent须为真实GuiDialog正文，不能传现代ICgOperationButtonInputServices地址。
// 失败不撤销已追加children/events；外层构造展开负责释放该前缀。
void PopulatePopupNativeControlsAt4EA0D1(void* parent,
    GuiControlChildrenStorage& children, GuiClickedEventReferences& events,
    std::span<const GuiStaticDescriptorNative24h, 12> statics,
    std::span<const GuiCheckBoxDescriptorNative2Ch, 9> checkBoxes,
    const PopupMenuCheckStampData& stamps,
    IGuiControlResourceLoadServices& loader, IGraphicsSurfaceApi& surfaces,
    IGraphicsSurfaceCopyApi& copy, IGuiControlGraphicsSource& graphics,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
