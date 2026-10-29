#include <Runtime/GraphicsTransitNativeApi.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
FARPROC ResolveTransitExportWithNameAt40B530(GraphicsModuleNativeBody8h* module,
    const char* exportName, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32)
{
    union NameStorage
    {
        NameStorage() noexcept {}
        ~NameStorage() noexcept {}
        RuntimeCString value;
    } name;
    // 44EC2B/44ECAB/44ED2B等内联418680同类路径；40B530只负责构造期间的EH。
    ConstructArchiveResourceNameAt40B530(&name.value, exportName, strings);
    const char* const characters = name.value.GetString();
    const HMODULE handle = module->moduleAt04;
    const FARPROC function = win32.GetProcAddress(handle, characters);
    // 44EC3C/44ECBC/44ED3C等在resolver后重读本地槽；不能复用缓存旧chars的helper。
    // resolver抛出不补清理；正常Free抛出也不重试，且不调用已缓存的F。
    DestroyRuntimeCStringAt40CD40(&name.value);
    return function;
}
}

GraphicsTransitNativeApi::GraphicsTransitNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32)
{
}

bool GraphicsTransitNativeApi::CaptureModule(GraphicsModuleNativeBody8h*& module)
{
    auto* const root = owner_.CaptureAt4182F0();
    if (!root->moduleAt00.baseAt00)
    {
        module = nullptr;
        return false;
    }
    // 固定M但不增持；不以M/H/F为0代替原来只检查base的分支。
    module = ResolveGraphicsModuleReference(root->moduleAt00);
    return true;
}

std::uint8_t GraphicsTransitNativeApi::TransitCopyBlit(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* size,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceCopyBlt", 14, strings_, win32_));
    // 443AD4..443AE9保留AL；不能转发到丢弃返回值的SurfaceCopyBlitRaw。
    return function(destination, source, x, y, size, sourceRect, clip, anchor);
}

std::uint8_t GraphicsTransitNativeApi::TransitCopyDraw(GraphicsSurfaceHandle source,
    std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
    const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t,
        std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceCopyDraw", strings_, win32_));
    return function(source, x, y, size, sourceRect, clip, anchor);
}

std::uint8_t GraphicsTransitNativeApi::TransitBlendBlit(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, std::int32_t alpha,
    const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        std::int32_t, std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceBlendBlt", strings_, win32_));
    // 448D26 -> 44EC65：alpha在x/y之后，完整DWORD，不截byte或钳位。
    return function(destination, source, x, y, alpha, size, sourceRect, clip, anchor);
}

std::uint8_t GraphicsTransitNativeApi::TransitBlendDraw(GraphicsSurfaceHandle source,
    std::int32_t x, std::int32_t y, std::int32_t alpha, const SIZE* size,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t,
        std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceBlendDraw", strings_, win32_));
    return function(source, x, y, alpha, size, sourceRect, clip, anchor);
}

std::uint8_t GraphicsTransitNativeApi::TransitBlit(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* size,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceBlt", strings_, win32_));
    return function(destination, source, x, y, size, sourceRect, clip, anchor);
}

std::uint8_t GraphicsTransitNativeApi::TransitDraw(GraphicsSurfaceHandle source,
    std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
    const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t,
        std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceDraw", strings_, win32_));
    return function(source, x, y, size, sourceRect, clip, anchor);
}

void GraphicsTransitNativeApi::TransitUniversalBlit(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle mask, const std::int32_t* thresholds, std::int32_t x, std::int32_t y,
    const SIZE* size, const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        const std::int32_t*, std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceUniversalBlt", strings_, win32_));
    // 44EE2C..44EE47为九参/24h；操作码20由DLL内部1000E578压14h，不能重复加第十参。
    function(destination, mask, thresholds, x, y, size, sourceRect, clip, anchor);
}

void GraphicsTransitNativeApi::TransitMosaic(GraphicsSurfaceHandle destination,
    std::uint32_t size, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveTransitExportWithNameAt40B530(
        module, "SurfaceMosaic", strings_, win32_));
    // 44DCB3..44DCBC传递先前计算的完整DWORD；代理不添加size>=2条件。
    function(destination, size, clip);
}
}
