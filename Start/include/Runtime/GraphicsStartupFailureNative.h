#pragma once

#include <Runtime/RuntimeCString.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
class FileManagerRootRuntime;
class EnvironmentStartupRuntime;
class IFileInfoProgressConfirmation;

// 489C90的state7失败尾；GetNil成功才取得CString展开资格，最终仍throw int(1)。
[[noreturn]] void ReportGraphicsInitializationFailureNativeAt48AECE(
    NativeGraphicsRuntimeLibrary& graphics, FileManagerRootRuntime& fileManager,
    EnvironmentStartupRuntime& environment, IFileInfoProgressConfirmation& messages,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}
