#pragma once

#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;

// 4B3CE4/4B3D12等两次Ensure分别取源；捕获与发布不得共用预先缓存的root。
class IRootTransitionFrameSource : public IDrawManagerLayerSource
{
public:
    virtual ~IRootTransitionFrameSource() = default;
    virtual YtlReferenceSlotRuntime& CaptureFrameSlotAt413BE0() = 0;
};

class RootTransitionFrameSource final : public IRootTransitionFrameSource
{
public:
    RootTransitionFrameSource(RootRuntimeOwner& root, IGraphicsSurfaceApi& surfaces) noexcept
        : root_(root), surfaces_(surfaces) {}
    DrawLayersView CaptureDrawLayersAt413BE0() override;
    YtlReferenceSlotRuntime& CaptureFrameSlotAt413BE0() override;
private:
    RootRuntimeOwner& root_;
    IGraphicsSurfaceApi& surfaces_;
};

class IGuiModalRootSource : public IRootTransitionFrameSource
{
public:
    virtual GraphicsSurfaceReference CaptureScreenAt413BE0() = 0;
    virtual std::uint32_t& ModalGateAt413BE0() = 0;
};

// 50CF50/508E80/50DCA0各自在原Ensure位置取当前根；构造不创建根或缓存payload。
class RootGuiModalSource final : public IGuiModalRootSource
{
public:
    RootGuiModalSource(RootRuntimeOwner& root, IGraphicsSurfaceApi& surfaces,
        INativeScreenCaptureServices& capture) noexcept
        : root_(root), capture_(capture), transition_(root, surfaces) {}
    DrawLayersView CaptureDrawLayersAt413BE0() override;
    YtlReferenceSlotRuntime& CaptureFrameSlotAt413BE0() override;
    GraphicsSurfaceReference CaptureScreenAt413BE0() override;
    std::uint32_t& ModalGateAt413BE0() override;
private:
    RootRuntimeOwner& root_;
    INativeScreenCaptureServices& capture_;
    RootTransitionFrameSource transition_;
};

void CaptureRootQuickOperationFrame(IRootTransitionFrameSource& source,
    IGraphicsSurfaceApi& surfaces, IGraphicsScreenSizeApi& sizes);
}
