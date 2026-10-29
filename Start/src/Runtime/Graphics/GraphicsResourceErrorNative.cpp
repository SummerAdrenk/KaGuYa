#include <Runtime/GraphicsResourceErrorNative.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootGraphicsResourceErrorDisplayNative::RootGraphicsResourceErrorDisplayNative(
    EnvironmentRootOwner& environment, MessageBox messageBox) noexcept
    : environment_(environment), messageBox_(messageBox) {}

void RootGraphicsResourceErrorDisplayNative::Show(RuntimeCString& text)
{
    auto* environment = environment_.CaptureAt4180C0(); // 481605；固定本次Env，不再取第二根。
    // 原是三个有序DWORD读取；不能让优化器改变非法地址分支的首个访问位置。
    using Chars = const char*;
    const char* caption = *reinterpret_cast<Chars const volatile*>(&environment->titleAt10); // 48160C。
    const char* message = *reinterpret_cast<Chars const volatile*>(&text); // 48160F：Capture后才读caller槽。
    const HWND window = reinterpret_cast<HWND>(*static_cast<const volatile std::uint32_t*>(&environment->windowAt2C)); // 481612。
    static_cast<void>(messageBox_(window, message, caption, 0x10u)); // 481615忽略返回，异常仍外传。
}
}
