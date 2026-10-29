#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

#include <cstdarg>

namespace KaGuYa::Start::Runtime
{
// 显式receiver固定，不Capture全局根。临时串GetNil成功后才取得展开资格。
void AppendEnvironmentErrorVAt4769B0(EnvironmentRootStorage& receiver,
    const char* format, va_list args,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void __cdecl AppendEnvironmentErrorAt4769B0(EnvironmentRootStorage& receiver,
    _Printf_format_string_ const char* format, ...);

// outRaw必须是未构造槽；成功交出唯一CString结果，失败按当前槽展开。
// 每轮重新读取receiver.errorsAt3C，既不复制vector也不缓存初始数量。
RuntimeCString& BuildEnvironmentErrorTextAt479EA0(EnvironmentRootStorage& receiver,
    RuntimeCString& outRaw,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void DisplayEnvironmentErrorsAt4AC730(EnvironmentRootStorage&,decltype(&::MessageBoxA),
    IRuntimeStringManagerSource& =DefaultRuntimeStringManagerSource());
}
