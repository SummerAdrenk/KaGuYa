#pragma once

#include <Runtime/GraphicsRootNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GuiControlNative.h>

namespace KaGuYa::Start::Runtime
{
class GraphicsSurfaceNativeApi;

// 502EB0等GUI叶的稳定借用服务；不持根、module、Surface或导出函数。
// surface必须与owner/strings/win32属于同一装配；构造及source getter均不Capture。
class GraphicsGuiNativeApi final
    : public IGuiControlGraphicsSource,
      public IGraphicsSurfaceCompositionApi,
      public IGraphicsSurfacePixelApi
{
public:
    GraphicsGuiNativeApi(GraphicsRootOwner& owner, IRuntimeStringManagerSource& strings,
        IGraphicsModuleNativeWin32Api& win32, GraphicsSurfaceNativeApi& surface) noexcept;
    ~GraphicsGuiNativeApi() override = default;
    GraphicsGuiNativeApi(const GraphicsGuiNativeApi&) = delete;
    GraphicsGuiNativeApi& operator=(const GraphicsGuiNativeApi&) = delete;

    // 仅选择接口，实际图形叶才执行一次4182F0；不在这里提前取根。
    [[nodiscard]] IGraphicsSurfaceCompositionApi& CompositionAt4182F0() override;
    [[nodiscard]] IGraphicsSurfacePixelApi& PixelsAt4182F0() override;

    // 443910/441980/4425C0：委托同一Surface代理，不增加handle或Initialized门控。
    void ClearSurface(GraphicsSurfaceHandle handle, std::uint32_t color,
        const RECT* rect) override;
    [[nodiscard]] bool DrawSurface(GraphicsSurfaceHandle source,
        std::int32_t destinationX, std::int32_t destinationY, const SIZE* destinationSize,
        const RECT* sourceRect, const RECT* destinationClip, std::int32_t anchor) override;
    [[nodiscard]] bool BlitSurface(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source,
        std::int32_t destinationX, std::int32_t destinationY, const SIZE* destinationSize,
        const RECT* sourceRect, const RECT* destinationClip, std::int32_t anchor) override;

    // 50AF10：返回完整DWORD；base非零但Resolve为0时仍进入原名称/字段访问路径。
    [[nodiscard]] std::uint32_t GetSurfacePixel(GraphicsSurfaceHandle handle,
        std::int32_t x, std::int32_t y) override;

private:
    GraphicsRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
    GraphicsSurfaceNativeApi& surface_;
};
}
