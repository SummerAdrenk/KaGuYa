#include <Runtime/OpacityWindowRuntime.h>
#include <Runtime/GuiInputStateRuntime.h>

#include <cstdint>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
OpacityWindowRuntime::OpacityWindowRuntime(
    LayerList* const layerList,
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationSliderInputSource& inputServices,
    IOpacityDialogResetStateServices& resetStateServices)
    : FrameLayerItem(layerList),
      dialogAt24_(surfaceFactory, graphics, inputServices),
      resetStateServices_(&resetStateServices),
      boundsAt14_(dialogAt24_.DialogRectAt5C())
{
    // 45AEF3：基类注册后，COpacityWin+0x0C 明确清零。
    SetEnabled(false);
}

void OpacityWindowRuntime::Draw(const RECT& dirtyRect)
{
    // vslot +0x00 = 482BD0，尾调用 OpacityDialog vslot +0x60。
    dialogAt24_.DrawAt5230B0(dirtyRect);
}

void OpacityWindowRuntime::AdvanceOwnedFrame()
{
    // 5B2D70 的 +14 指向 46B230，原函数只有 RET。
}

void OpacityWindowRuntime::CompositeToSurface(
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    if (!destination.Body())
        throw std::invalid_argument("COpacityWin composite requires a native destination body");
    // 481B80..481BBB：先捕获 Dialog，再构造非 owning Ref12；即使禁用也不能省掉分配。
    auto* const dialog = &dialogAt24_;
    YtlReferenceSlotRuntime argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument,
        const_cast<GraphicsSurfaceNativeBody*>(destination.Body()), 0);
    dialog->CompositeAt5231C0(argument, destinationClip);
}

const RECT& OpacityWindowRuntime::BoundsAt14() const noexcept
{
    return boundsAt14_;
}

OpacityDialogObjectGraphRuntime& OpacityWindowRuntime::DialogAt24() noexcept
{
    return dialogAt24_;
}

const OpacityDialogObjectGraphRuntime&
OpacityWindowRuntime::DialogAt24() const noexcept
{
    return dialogAt24_;
}

std::uint8_t OpacityWindowRuntime::SetVisibleAtVslot10(
    const bool visible,
    const std::int32_t)
{
    if (Enabled() == visible)
        return 0;

    SetEnabled(visible);
    dialogAt24_.WriteEnabledAtVslot24(visible ? 1 : 0);
    LayerList* const list = RegisteredLayerList();
    if (!list)
        throw std::logic_error("OpacityWindow invalidation requires its parent LayerList");
    list->DirtyRects().Add(boundsAt14_);
    return 0;
}

void OpacityWindowRuntime::ResetAtVslot18()
{
    SetEnabled(false);
    dialogAt24_.ResetAtVslot44At50D730(*resetStateServices_);
}

bool OpacityWindowRuntime::TickAt4BE530(Platform::MouseView mouse,
    Platform::KeyboardView keyboard, IOpacityDialogUpdateServices& services)
{
    const GameGuiInputStateRuntime input(mouse, keyboard);
    if (input.ReadPressedAtVslot04(0)) return false;
    if (!mouse.PreviousPressed(1) && mouse.Pressed(1) &&
        mouse.ReleaseSuppressionCountdown() == 0) return false;
    // 4BE5F5：忽略子 Update 返回值，不按 outer enabled 门控。
    (void)dialogAt24_.UpdateAtVslot48At52DF20(services);
    PropagateDirtyAt4ABA60();
    return true;
}

void OpacityWindowRuntime::PropagateDirtyAt4ABA60()
{
    dialogAt24_.OwnDirtyRectsAtVslot00().PrepareAtVslot04();
    const auto count = dialogAt24_.OwnDirtyRectsAtVslot00().CountAtVslot18();
    if (count == 0) return;
    const auto rects = dialogAt24_.OwnDirtyRectsAtVslot00().RectsAtVslot1C();
    if (count > rects.size()) throw std::logic_error("Opacity dirty count exceeds its array");
    for (std::size_t i = 0; i != count; ++i)
    {
        auto* parent = RegisteredLayerList();
        if (!parent) throw std::logic_error("Opacity dirty propagation requires parent LayerList");
        parent->DirtyRects().Add(rects[i]);
    }
    // 4ABB2C：只有全部提交成功才 Clear，异常不得清掉待提交批次。
    dialogAt24_.OwnDirtyRectsAtVslot00().ClearAtVslot00();
}
}
