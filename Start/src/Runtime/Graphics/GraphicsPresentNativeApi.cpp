#include <Runtime/GraphicsPresentNativeApi.h>

namespace KaGuYa::Start::Runtime
{
GraphicsPresentNativeApi::GraphicsPresentNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32) {}

void GraphicsPresentNativeApi::RenderScreenIfLoaded()
{
    // 4836FF/5072D6：正常Leave后读取当前发布根，仅按Ref的base门控。
    auto* const root = owner_.CaptureAt4182F0();
    if (!root->moduleAt00.baseAt00) return;
    auto* const module = ResolveGraphicsModuleReference(root->moduleAt00);
    using Function = void (__cdecl*)();
    // 48375C/50733A显式11字节；固定M，晚读H，撤销清理资格后解析，Free后调用固定F。
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "RenderSceen", 11, strings_, win32_));
    function();
}
}
