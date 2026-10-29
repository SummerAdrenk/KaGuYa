#include <Runtime/FreeScrollNativeDrawing.h>
#include <Runtime/GraphicsRootNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

namespace KaGuYa::Start::Runtime
{
FreeScrollNativeCanvasCopy::FreeScrollNativeCanvasCopy(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32) {}

void FreeScrollNativeCanvasCopy::CopyProcessedAt4AEFED(std::int32_t destination,
    std::int32_t source, std::int32_t x, std::int32_t y, const RECT* sourceRect)
{
    auto* root = owner_.CaptureAt4182F0();
    if (!root->moduleAt00.baseAt00) return;
    auto* module = ResolveGraphicsModuleReference(root->moduleAt00);
    union NameStorage
    {
        NameStorage() noexcept {}
        ~NameStorage() noexcept {}
        RuntimeCString value;
    } name;
    // 418680：40B530构造后才读取固定module.H；resolver返回后重读CString槽。
    ConstructArchiveResourceNameAt40B530(&name.value, "SurfaceCopyBlt", strings_);
    const char* characters = name.value.GetString();
    const HMODULE handle = module->moduleAt04;
    const FARPROC address = win32_.GetProcAddress(handle, characters);
    DestroyRuntimeCStringAt40CD40(&name.value);
    // 原调用忽略返回值；resolver/Free失败不重试，也不改走显式长度协议。
    using Function = void(__cdecl*)(std::int32_t, std::int32_t, std::int32_t,
        std::int32_t, const SIZE*, const RECT*, const RECT*, std::int32_t);
    reinterpret_cast<Function>(address)(destination, source, x, y, nullptr, sourceRect, nullptr, 0);
}
}
