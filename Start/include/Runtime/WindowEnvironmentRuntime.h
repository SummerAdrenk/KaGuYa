#pragma once

#include <Platform/WindowCreation.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsRuntimeProfile;

// params 解析完成后的窗口环境；不是 498280 前半段的默认环境构造。
struct WindowEnvironmentRuntime
{
    Platform::ParamsGameProfile gameProfile;
    Platform::StartExecutableProfile executableProfile;
    DWORD fullscreenStyle;
    HWND window = nullptr;
};

// 只复制已解析配置并安装 18 号 PE 的常量，不读取文件或创建窗口。
[[nodiscard]] WindowEnvironmentRuntime BuildSample18WindowEnvironment(
    const ParamsRuntimeProfile& params);
}
