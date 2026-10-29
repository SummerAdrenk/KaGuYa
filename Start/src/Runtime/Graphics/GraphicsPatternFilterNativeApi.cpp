#include <Runtime/GraphicsPatternFilterNativeApi.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

namespace KaGuYa::Start::Runtime
{
GraphicsPatternFilterNativeApi::GraphicsPatternFilterNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32), surfaces_(owner, strings, win32)
{
}

bool GraphicsPatternFilterNativeApi::CaptureModule(GraphicsModuleNativeBody8h*& module)
{
    auto* const root = owner_.CaptureAt4182F0();
    if (!root->moduleAt00.baseAt00) return false;
    // 资格取base，不检查解析后地址；回绕为0仍进入原来的字段访问。
    module = ResolveGraphicsModuleReference(root->moduleAt00);
    return true;
}

void GraphicsPatternFilterNativeApi::SurfaceFilterIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceFilter", 13, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceFilter2IfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceFilter2", 14, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceFlushIfLoaded(GraphicsSurfaceHandle destination, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceFlush", 12, strings_, win32_));
    static_cast<void>(function(destination, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle, std::int32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceBlur", 11, strings_, win32_));
    static_cast<void>(function(destination, source, radius, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceGaussBlurIfLoaded(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source, std::int32_t radius, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, GraphicsSurfaceHandle, std::int32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceGaussBlur", 16, strings_, win32_));
    static_cast<void>(function(destination, source, radius, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceAddColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceAddColor", 15, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceSubColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceSubColor", 15, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceCopyDrawIfLoaded(GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* destinationSize, const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t, std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceCopyDraw", 15, strings_, win32_));
    static_cast<void>(function(source, x, y, destinationSize, sourceRect, clip, anchor));
}

void GraphicsPatternFilterNativeApi::SurfaceClearIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    // 443067 -> 443910共用同一原生叶，不在委托前多取一次根。
    surfaces_.SurfaceClearRaw(destination, color, clip);
}

void GraphicsPatternFilterNativeApi::SurfaceMulAlphaIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceMulAlpha", 15, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceAddSubSurfaceIfLoaded(GraphicsSurfaceHandle destination, std::uintptr_t sourceObjectAddress, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uintptr_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceAddSubSurface", 20, strings_, win32_));
    static_cast<void>(function(destination, sourceObjectAddress, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceMosaicIfLoaded(GraphicsSurfaceHandle destination, std::int32_t size, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceMosaic", 13, strings_, win32_));
    static_cast<void>(function(destination, size, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceMulColorIfLoaded(GraphicsSurfaceHandle destination, std::uint32_t color, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SurfaceMulColor", 15, strings_, win32_));
    static_cast<void>(function(destination, color, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceFlipIfLoaded(GraphicsSurfaceHandle destination, std::int32_t flags, const RECT* clip)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::int32_t, const RECT*);
    // 44335C -> 418680使用40B530而非显式长度构造；后者只负责自己的构造EH。
    // 418680没有名称清理EH：resolver抛出时不能追加减持或吞掉异常。
    union NameStorage
    {
        NameStorage() noexcept {}
        ~NameStorage() noexcept {}
        RuntimeCString value;
    } name;
    ConstructArchiveResourceNameAt40B530(&name.value, "SurfaceFlip", strings_);
    const HMODULE handle = module->moduleAt04;
    const auto function = reinterpret_cast<Function>(
        win32_.GetProcAddress(handle, name.value.GetString()));
    auto* const header = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(name.value.GetString()) - sizeof(ATL::CStringData));
    ReleaseRuntimeCStringHeader(header);
    static_cast<void>(function(destination, flags, clip));
}

void GraphicsPatternFilterNativeApi::SurfaceCopyBlitRaw(GraphicsSurfaceHandle destination,
    GraphicsSurfaceHandle source, std::int32_t x, std::int32_t y, const SIZE* destinationSize,
    const RECT* sourceRect, const RECT* clip, std::int32_t anchor)
{
    surfaces_.SurfaceCopyBlitRaw(destination, source, x, y, destinationSize, sourceRect, clip, anchor);
}
}
