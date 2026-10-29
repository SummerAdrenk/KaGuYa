#pragma once
#include <Platform/InputViews.h>

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Platform/Input.h>

#include <Windows.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// OpacityWin::COpacityWin(0x45AE70) 的生产 Layer owner。
// 原对象在 CDrawManager owning Layer 数组中位于索引 6；现代 C++ 对象
// 不伪造原 0x30 字节布局，但保留同一注册、子对象与可观察调用顺序。
class OpacityWindowRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override
        { return SetVisibleAtVslot10(requested != 0, secondArgument); }
    void AdvanceOwnedFrame() override;
    OpacityWindowRuntime(
        LayerList* layerList,
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationSliderInputSource& inputServices,
        IOpacityDialogResetStateServices& resetStateServices);

    OpacityWindowRuntime(const OpacityWindowRuntime&) = delete;
    OpacityWindowRuntime& operator=(const OpacityWindowRuntime&) = delete;

    void Draw(const RECT& dirtyRect) override;
    void CompositeToSurface(
        GraphicsSurfaceDestination destination, const RECT* destinationClip) override;

    [[nodiscard]] const RECT& BoundsAt14() const noexcept;
    [[nodiscard]] OpacityDialogObjectGraphRuntime& DialogAt24() noexcept;
    [[nodiscard]] const OpacityDialogObjectGraphRuntime&
        DialogAt24() const noexcept;

    // 487230：第二个参数未被读取；只有状态实际变化时才同步 Dialog
    // 并向父 LayerList 提交整个 bounds。
    [[nodiscard]] std::uint8_t SetVisibleAtVslot10(
        bool visible,
        std::int32_t unusedSecondArgument);

    // 47EE40：不提交旧矩形，直接隐藏外层并重置内层 Dialog。
    void ResetAtVslot18();
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAtVslot18(); }
    [[nodiscard]] bool TickAt4BE530(Platform::MouseView mouse,
        Platform::KeyboardView keyboard, IOpacityDialogUpdateServices& services);
    void PropagateDirtyAt4ABA60();

private:
    OpacityDialogObjectGraphRuntime dialogAt24_;
    IOpacityDialogResetStateServices* resetStateServices_ = nullptr;
    RECT boundsAt14_{};
};
}
