#include <Runtime/GraphicsScreenNativeApi.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
POINT* ReadCapturedGraphicsScreenSizeAt47CC00(GraphicsRootNativeBody1Ch& root,POINT* output,
    IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& win32)
{
    // 47CC28先保存base，再清返回缓冲；返回缓冲即使与Root别名也不得提前改base。
    const auto base=*static_cast<const volatile std::uintptr_t*>(&root.moduleAt00.baseAt00);
    output->x=0;output->y=0;
    if(!base)return output;
    const auto* control=*static_cast<YtlReferenceControlRuntime* const volatile*>(&root.moduleAt00.controlAt08);
    const auto stride=control?*reinterpret_cast<const volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control)+0x0Cu):0u;
    const auto bias=*static_cast<const volatile unsigned*>(&root.moduleAt00.biasAt04);
    auto* module=reinterpret_cast<GraphicsModuleNativeBody8h*>(base+bias*stride);
    using Function=std::uint64_t(__cdecl*)();
    const auto function=reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(module,"GetScreenSize",13,strings,win32));
    const auto value=function();
    output->x=std::bit_cast<LONG>(static_cast<unsigned>(value));
    output->y=std::bit_cast<LONG>(static_cast<unsigned>(value>>32u));
    return output;
}
void InvalidateCapturedGraphicsRectAt4B02E0(GraphicsRootNativeBody1Ch& root,const RECT* rect,
    IRuntimeStringManagerSource& strings,IGraphicsModuleNativeWin32Api& win32)
{
    if(!root.moduleAt00.baseAt00)return;
    auto* module=ResolveGraphicsModuleReference(root.moduleAt00);
    using Function=void (__cdecl*)(const RECT*);
    auto function=reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module,"InvalidatedRect",15,strings,win32));
    function(rect);
}
GraphicsScreenNativeApi::GraphicsScreenNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32)
{
}

bool GraphicsScreenNativeApi::CaptureModule(GraphicsModuleNativeBody8h*& module)
{
    auto* const root = owner_.CaptureAt4182F0();
    if (!root->moduleAt00.baseAt00) return false;
    module = ResolveGraphicsModuleReference(root->moduleAt00);
    return true;
}

SIZE GraphicsScreenNativeApi::ReadScreenSizeAt47CC00()
{
    POINT result;
    ReadScreenSizeCapturedAt47CC00(*owner_.CaptureAt4182F0(),&result);
    return {result.x,result.y};
}

const int* GraphicsScreenNativeApi::ResizeScreenIfLoaded(int width, int height, int mode)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return nullptr;
    using Function = const int* (__cdecl*)(int, int, int);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "ScreenResize", 12, strings_, win32_));
    // 43487C..43488A返回DLL的原指针，不复制成临时RECT，也不按结果更新根尺寸。
    return function(width, height, mode);
}

void GraphicsScreenNativeApi::ResetRendererIfLoaded()
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)();
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "RenderReset", 11, strings_, win32_));
    function();
}

void GraphicsScreenNativeApi::ClearScreenIfLoaded(const RECT* rect)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "ClearScreen", 11, strings_, win32_));
    function(rect);
}

void GraphicsScreenNativeApi::InvalidatedRectIfLoaded(const RECT* rect)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    // 441D25经410290测固定名字长度；441D38才读取已捕获module的HMODULE。
    using Function = void (__cdecl*)(const RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "InvalidatedRect", 15, strings_, win32_));
    function(rect);
}

void GraphicsScreenNativeApi::ClearDirtyRectIfLoaded()
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)();
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "ClearDirtyRect", 14, strings_, win32_));
    function();
}
}
