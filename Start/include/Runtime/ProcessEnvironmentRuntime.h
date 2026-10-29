#pragma once

#include <Runtime/EnvironmentStartupRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 唯一进程6198C0/6198C8来源。getter不替换来源，不预先创建或保活某代正文。
ParamsRuntimeRootOwner& ProcessParamsRuntimeRootOwner() noexcept;
EnvironmentStartupRuntime& ProcessEnvironmentStartupRuntime() noexcept;
// 原生默认调用点的nullable路径解析，不添加会被外层业务catch吞掉的C++安全异常。
EnvironmentPathRuntime* CaptureProcessEnvironmentPathsAt4180C0();
}
