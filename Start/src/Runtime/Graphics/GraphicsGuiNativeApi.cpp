#include <Runtime/GraphicsGuiNativeApi.h>
#include <Runtime/GraphicsSurfaceNativeApi.h>

namespace KaGuYa::Start::Runtime
{
GraphicsGuiNativeApi::GraphicsGuiNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32,
    GraphicsSurfaceNativeApi& surface) noexcept
    : owner_(owner), strings_(strings), win32_(win32), surface_(surface)
{
}

IGraphicsSurfaceCompositionApi& GraphicsGuiNativeApi::CompositionAt4182F0()
{
    return *this;
}

IGraphicsSurfacePixelApi& GraphicsGuiNativeApi::PixelsAt4182F0()
{
    return *this;
}

void GraphicsGuiNativeApi::ClearSurface(GraphicsSurfaceHandle handle,
    std::uint32_t color, const RECT* rect)
{
    surface_.SurfaceClearRaw(handle, color, rect);
}

bool GraphicsGuiNativeApi::DrawSurface(GraphicsSurfaceHandle source,
    std::int32_t destinationX, std::int32_t destinationY, const SIZE* destinationSize,
    const RECT* sourceRect, const RECT* destinationClip, std::int32_t anchor)
{
    // 441A75只取AL；现Composition接口为bool，GUI叶不消费返回值。
    return surface_.SurfaceDrawRaw(source, destinationX, destinationY,
        destinationSize, sourceRect, destinationClip, anchor) != 0;
}

bool GraphicsGuiNativeApi::BlitSurface(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t destinationX, std::int32_t destinationY,
    const SIZE* destinationSize, const RECT* sourceRect, const RECT* destinationClip,
    std::int32_t anchor)
{
    return surface_.SurfaceBlitRaw(destination, source, destinationX, destinationY,
        destinationSize, sourceRect, destinationClip, anchor) != 0;
}

std::uint32_t GraphicsGuiNativeApi::GetSurfacePixel(GraphicsSurfaceHandle handle,
    std::int32_t x, std::int32_t y)
{
    auto* const root = owner_.CaptureAt4182F0();
    // 50AF35..50AF55仅按base门控；非零base解析出null不改成空模块返回。
    if (!root->moduleAt00.baseAt00) return 0;
    auto* const module = ResolveGraphicsModuleReference(root->moduleAt00);
    using Function = std::uint32_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t, std::int32_t);
    // 50AF92为显式0Fh长度；缓存名称、晚读M+4，撤EH后resolver/Free均不补清理。
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "GetSurfacePixel", 15, strings_, win32_));
    return function(handle, x, y);
}
}
