#pragma once

#include <Runtime/GraphicsSurfaceNativeApi.h>
#include <Runtime/PatternPixelFilterRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 442900的原生模块调用边界；只借稳定服务，每次操作重新取当前根，不增持Ref。
class GraphicsPatternFilterNativeApi final : public IGraphicsPatternFilterCalls
{
public:
    GraphicsPatternFilterNativeApi(GraphicsRootOwner& owner,
        IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept;

    void SurfaceFilterIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceFilter2IfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceFlushIfLoaded(GraphicsSurfaceHandle destination, const RECT* clip) override;
    void SurfaceBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) override;
    void SurfaceGaussBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip) override;
    void SurfaceAddColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceSubColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceCopyDrawIfLoaded(GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* destinationSize, const RECT* sourceRect, const RECT* clip, std::int32_t anchor) override;
    void SurfaceClearIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceMulAlphaIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceAddSubSurfaceIfLoaded(GraphicsSurfaceHandle destination, std::uintptr_t sourceObjectAddress, const RECT* clip) override;
    void SurfaceMosaicIfLoaded(GraphicsSurfaceHandle destination, std::int32_t size, const RECT* clip) override;
    void SurfaceMulColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip) override;
    void SurfaceFlipIfLoaded(GraphicsSurfaceHandle destination, std::int32_t flags, const RECT* clip) override;
    void SurfaceCopyBlitRaw(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* destinationSize, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) override;

private:
    [[nodiscard]] bool CaptureModule(GraphicsModuleNativeBody8h*& module);
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
    GraphicsSurfaceNativeApi surfaces_;
};
}
