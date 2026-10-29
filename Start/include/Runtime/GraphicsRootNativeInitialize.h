#pragma once

#include <Runtime/GraphicsRootNative.h>

namespace KaGuYa::Start::Runtime
{
class EnvironmentPathRuntime;

// 44088B..4409A9只借当前Environment的路径正文，不提前复制library CString。
class IGraphicsInitializationPathsNativeSource
{
public:
    virtual ~IGraphicsInitializationPathsNativeSource() = default;
    virtual EnvironmentPathRuntime* CapturePathsAt4180C0() = 0;
};
IGraphicsInitializationPathsNativeSource& DefaultGraphicsInitializationPathsNativeSource() noexcept;

// 440770固定root；DLL的六参cdecl调用只检查AL。configuration保持调用方借用地址。
// 失败尾只普通释放当前module，不补ExitSystem，也不回滚已提交的窗口/尺寸。
[[nodiscard]] bool InitializeGraphicsRootNativeAt440770(GraphicsRootNativeBody1Ch& root,
    std::uint32_t renderMode, HWND window, std::int32_t width, std::int32_t height,
    const std::uint8_t* configuration, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32,
    IGraphicsInitializationPathsNativeSource& paths);
}
