#pragma once

#include <string>
#include <string_view>

namespace KaGuYa::Logging
{
enum class Level
{
    Trace,
    Info,
    Warning,
    Error
};

// Initialize 只登记模块名；首次写日志时才创建文件，避免在 DllMain 中执行磁盘 I/O。
void Initialize(std::wstring_view moduleName) noexcept;
void Shutdown() noexcept;
void Write(Level level, std::string_view message) noexcept;
void WriteFormat(Level level, const char* format, ...) noexcept;
void WriteNotImplementedOnce(const char* functionName) noexcept;

// 原引擎的窄字符串 ABI 使用指定 Windows 代码页；日志落盘前必须显式转换为 UTF-8。
// 转换失败时返回 false，不以系统代码页或原始字节兜底。
bool TryCodePageToUtf8(std::string_view text, unsigned int codePage, std::string& result) noexcept;
}
