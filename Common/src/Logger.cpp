#include <Logging/Logger.h>

#include <Windows.h>
#include <limits>

#ifndef KAGUYA_ENABLE_LOGGING
#error KAGUYA_ENABLE_LOGGING must be set by the build configuration.
#endif
#if KAGUYA_ENABLE_LOGGING != 0 && KAGUYA_ENABLE_LOGGING != 1
#error KAGUYA_ENABLE_LOGGING must be 0 or 1.
#endif

#if KAGUYA_ENABLE_LOGGING
#include <Logging/WinUtf8FileSink.h>
#include <spdlog/logger.h>

#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <cwctype>
#endif

namespace KaGuYa::Logging
{
#if KAGUYA_ENABLE_LOGGING
namespace
{
std::mutex g_mutex;
std::wstring g_moduleName = L"Unknown";
std::shared_ptr<spdlog::logger> g_logger;
std::set<std::string> g_reportedStubs;

spdlog::level::level_enum ReadConfiguredLevel() noexcept
{
    std::array<wchar_t, 32> value{};
    const DWORD length = GetEnvironmentVariableW(L"KAGUYA_LOG_LEVEL",
        value.data(), static_cast<DWORD>(value.size()));
    if (length == 0)
    {
        return spdlog::level::info;
    }
    if (length >= value.size())
    {
        OutputDebugStringW(L"[KaGuYa logging] KAGUYA_LOG_LEVEL 过长，日志已关闭。\n");
        return spdlog::level::off;
    }
    std::wstring level(value.data(), length);
    std::transform(level.begin(), level.end(), level.begin(),
        [](const wchar_t character) { return static_cast<wchar_t>(std::towlower(character)); });
    if (level == L"trace") return spdlog::level::trace;
    if (level == L"info") return spdlog::level::info;
    if (level == L"warning" || level == L"warn") return spdlog::level::warn;
    if (level == L"error") return spdlog::level::err;
    if (level == L"off") return spdlog::level::off;
    OutputDebugStringW(L"[KaGuYa logging] KAGUYA_LOG_LEVEL 值无效，日志已关闭。\n");
    return spdlog::level::off;
}

spdlog::level::level_enum ToSpdlogLevel(const Level level) noexcept
{
    switch (level)
    {
    case Level::Trace: return spdlog::level::trace;
    case Level::Info: return spdlog::level::info;
    case Level::Warning: return spdlog::level::warn;
    case Level::Error: return spdlog::level::err;
    }
    return spdlog::level::info;
}

std::wstring CurrentModuleDirectory()
{
    HMODULE module = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&g_moduleName),
        &module);

    std::array<wchar_t, 32768> path{};
    const DWORD length = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
    if (length == 0 || length >= path.size())
    {
        return L".";
    }
    std::wstring result(path.data(), length);
    const auto separator = result.find_last_of(L"\\/");
    return separator == std::wstring::npos ? L"." : result.substr(0, separator);
}

std::string NarrowModuleName(const std::wstring& name)
{
    if (name.empty())
    {
        return "Unknown";
    }
    const int size = WideCharToMultiByte(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), nullptr, 0, nullptr, nullptr);
    if (size <= 0)
    {
        return "Unknown";
    }
    std::string result(static_cast<std::size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, name.data(), static_cast<int>(name.size()), result.data(), size, nullptr, nullptr);
    return result;
}

std::shared_ptr<spdlog::logger> EnsureLoggerLocked()
{
    if (g_logger)
    {
        return g_logger;
    }

    try
    {
        // 运行时off也必须在创建sink之前生效，不能留下空日志目录或文件。
        const auto configuredLevel = ReadConfiguredLevel();
        if (configuredLevel == spdlog::level::off)
        {
            return {};
        }
        const std::wstring path = CurrentModuleDirectory() + L"\\Logs\\KaGuYa\\" + g_moduleName + L".log";
        auto sink = std::make_shared<WinUtf8FileSinkMt>(path, false);
        auto logger = std::make_shared<spdlog::logger>(NarrowModuleName(g_moduleName), std::move(sink));
        logger->set_pattern("%Y-%m-%d %H:%M:%S.%e [%P:%t] [%-5l] %v");
        // 高频绘制 trace 会在主线程同步写盘；仅在显式诊断时通过环境变量开启。
        logger->set_level(configuredLevel);
        logger->flush_on(spdlog::level::err);
        logger->set_error_handler([](const std::string& message)
        {
            OutputDebugStringA(("[KaGuYa logging] " + message + "\n").c_str());
        });
        g_logger = std::move(logger);
    }
    catch (...)
    {
        OutputDebugStringA("[KaGuYa logging] 无法创建日志文件。\n");
    }
    return g_logger;
}
}

void Initialize(const std::wstring_view moduleName) noexcept
{
    try
    {
        std::scoped_lock lock(g_mutex);
        if (g_moduleName != moduleName)
        {
            g_logger.reset();
            g_reportedStubs.clear();
            g_moduleName.assign(moduleName);
        }
    }
    catch (...)
    {
    }
}

void Shutdown() noexcept
{
    try
    {
        std::shared_ptr<spdlog::logger> logger;
        {
            std::scoped_lock lock(g_mutex);
            logger = std::move(g_logger);
            g_reportedStubs.clear();
        }
        if (logger)
        {
            logger->set_level(spdlog::level::off);
            logger->flush();
        }
    }
    catch (...)
    {
    }
}

void Write(const Level level, const std::string_view message) noexcept
{
    try
    {
        std::shared_ptr<spdlog::logger> logger;
        {
            std::scoped_lock lock(g_mutex);
            logger = EnsureLoggerLocked();
        }
        if (logger)
        {
            logger->log(ToSpdlogLevel(level), spdlog::string_view_t(message.data(), message.size()));
        }
    }
    catch (...)
    {
    }
}

void WriteFormat(const Level level, const char* const format, ...) noexcept
{
    if (format == nullptr)
    {
        return;
    }

    std::array<char, 2048> buffer{};
    va_list arguments;
    va_start(arguments, format);
    const int length = std::vsnprintf(buffer.data(), buffer.size(), format, arguments);
    va_end(arguments);
    if (length > 0)
    {
        Write(level, std::string_view(buffer.data(), static_cast<std::size_t>((std::min)(length, static_cast<int>(buffer.size() - 1)))));
    }
}

void WriteNotImplementedOnce(const char* const functionName) noexcept
{
    try
    {
        const std::string name = functionName == nullptr ? "<null>" : functionName;
        bool first = false;
        {
            std::scoped_lock lock(g_mutex);
            first = g_reportedStubs.emplace(name).second;
        }
        if (first)
        {
            WriteFormat(Level::Warning, "%s: ABI 已保留，行为尚未实现。", name.c_str());
        }
    }
    catch (...)
    {
    }
}

#else
// 只关闭日志实现；调用接口和参数求值不变，游戏错误分支不受影响。
void Initialize(std::wstring_view) noexcept {}
void Shutdown() noexcept {}
void Write(Level, std::string_view) noexcept {}
void WriteFormat(Level, const char*, ...) noexcept {}
void WriteNotImplementedOnce(const char*) noexcept {}
#endif

// 编码转换是独立能力，日志关闭时仍保留原返回值和失败语义。
bool TryCodePageToUtf8(const std::string_view text, const unsigned int codePage,
    std::string& result) noexcept
{
    result.clear();
    if (text.empty())
    {
        return true;
    }
    if (text.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
    {
        return false;
    }

    try
    {
        const int sourceLength = static_cast<int>(text.size());
        const int wideLength = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS,
            text.data(), sourceLength, nullptr, 0);
        if (wideLength <= 0)
        {
            return false;
        }

        std::wstring wide(static_cast<std::size_t>(wideLength), L'\0');
        if (MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS,
            text.data(), sourceLength, wide.data(), wideLength) != wideLength)
        {
            return false;
        }

        const int utf8Length = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
            wide.data(), wideLength, nullptr, 0, nullptr, nullptr);
        if (utf8Length <= 0)
        {
            return false;
        }

        result.resize(static_cast<std::size_t>(utf8Length));
        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
            wide.data(), wideLength, result.data(), utf8Length, nullptr, nullptr) != utf8Length)
        {
            result.clear();
            return false;
        }
        return true;
    }
    catch (...)
    {
        result.clear();
        return false;
    }
}
}
