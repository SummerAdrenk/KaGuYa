#include <Runtime/GraphicsSurfaceNativeApi.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
void ClearCapturedModule(GraphicsModuleNativeBody8h* module, GraphicsSurfaceHandle handle,
    std::uint32_t color, const RECT* rect, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32)
{
    using Function = void (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceClear", 12, strings, win32));
    function(handle, color, rect);
}
}
GraphicsSurfaceNativeApi::GraphicsSurfaceNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32)
{
}

bool GraphicsSurfaceNativeApi::CaptureModule(GraphicsModuleNativeBody8h*& module)
{
    auto* const root = owner_.CaptureAt4182F0();
    // 原叶只检查Ref.base；不把空root、非零base解析成null、或HMODULE=0改为空模块返回。
    if (!root->moduleAt00.baseAt00)
    {
        module = nullptr;
        return false;
    }
    module = ResolveGraphicsModuleReference(root->moduleAt00);
    return true;
}

GraphicsSurfaceHandle GraphicsSurfaceNativeApi::CreateSurface(std::int32_t width,
    std::int32_t height, std::int32_t hasAlpha, std::int32_t clearOnCreate)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = GraphicsSurfaceHandle (__cdecl*)(std::int32_t, std::int32_t,
        std::int32_t, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceCreate", 13, strings_, win32_));
    // 4150DF..4150ED：四个DWORD原样传入，alpha/clear不按现代bool归一。
    return function(width, height, hasAlpha, clearOnCreate);
}

void GraphicsSurfaceNativeApi::ReleaseSurface(GraphicsSurfaceHandle handle)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(GraphicsSurfaceHandle);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceRelease", 14, strings_, win32_));
    // 43269D的handle门控属于24h调用者；这里不再拒绝0，也不清调用者字段。
    function(handle);
}

bool GraphicsSurfaceNativeApi::LoadBitmap(GraphicsSurfaceHandle handle,
    const void* fileHeader, const void* bitmapInfo, const void* pixels, std::uint32_t flags)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, const void*, const void*,
        const void*, std::uint32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceLoadBMP", 14, strings_, win32_));
    // 432A0D..432A1A：handle/fileHeader/bitmapInfo/pixels/flags，业务结果仅取AL。
    return function(handle, fileHeader, bitmapInfo, pixels, flags) != 0;
}

bool GraphicsSurfaceNativeApi::LoadAlp(GraphicsSurfaceHandle handle, std::uint8_t mode,
    std::int32_t width, std::int32_t height, const void* pixels)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t,
        std::int32_t, std::int32_t, const void*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceLoadALP", 14, strings_, win32_));
    // 432A4A/432AF5只定义mode低byte；现接口传入byte，按一个DWORD栈槽交给DLL。
    return function(handle, static_cast<std::uint32_t>(mode), width, height, pixels) != 0;
}

void GraphicsSurfaceNativeApi::LoadAnimationPixels(GraphicsSurfaceHandle handle,
    std::int32_t width, std::int32_t height, std::int32_t channels, const void* pixels)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(GraphicsSurfaceHandle, std::int32_t,
        std::int32_t, std::int32_t, const void*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceLoadANM", 14, strings_, win32_));
    // 44878A..448799五参；本接口沿原帧装载调用者不消费AL，不追加成功判断。
    function(handle, width, height, channels, pixels);
}

std::byte* GraphicsSurfaceNativeApi::GetSurfacePointerRaw(GraphicsSurfaceHandle handle)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return nullptr;
    using Function = std::byte* (__cdecl*)(GraphicsSurfaceHandle);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "GetSurfacePtr", 13, strings_, win32_));
    return function(handle);
}

std::int32_t GraphicsSurfaceNativeApi::GetSurfacePitchRaw(GraphicsSurfaceHandle handle)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::int32_t (__cdecl*)(GraphicsSurfaceHandle);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "GetSurfacePitch", 15, strings_, win32_));
    return function(handle);
}

void GraphicsSurfaceNativeApi::SurfaceClearRaw(GraphicsSurfaceHandle handle,
    std::uint32_t color, const RECT* rect)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    ClearCapturedModule(module, handle, color, rect, strings_, win32_);
}

void GraphicsSurfaceNativeApi::ClearSurfaceBodyAt46F745(const GraphicsSurfaceNativeBody& body)
{
    auto* const root = owner_.CaptureAt4182F0();
    // 46F74C/46F751：先取根，再读已固定正文；CString回调之后不得再重读参数。
    const auto color = body.clearColorAt20;
    const auto handle = body.handleAt04;
    if (!root->moduleAt00.baseAt00) return;
    auto* const module = ResolveGraphicsModuleReference(root->moduleAt00);
    ClearCapturedModule(module, handle, color, nullptr, strings_, win32_);
}

std::int32_t GraphicsSurfaceNativeApi::SurfaceDrawRaw(GraphicsSurfaceHandle source,
    std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
    const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t,
        std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceDraw", 11, strings_, win32_));
    return static_cast<std::int32_t>(function(source, x, y, size, sourceRect, clip, anchor));
}

std::int32_t GraphicsSurfaceNativeApi::SurfaceBlitRaw(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* size,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceBlt", 10, strings_, win32_));
    return static_cast<std::int32_t>(function(destination, source, x, y, size, sourceRect, clip, anchor));
}

void GraphicsSurfaceNativeApi::SurfaceCopyBlitRaw(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* size,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle,
        std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceCopyBlt", 14, strings_, win32_));
    function(destination, source, x, y, size, sourceRect, clip, anchor);
}
}
