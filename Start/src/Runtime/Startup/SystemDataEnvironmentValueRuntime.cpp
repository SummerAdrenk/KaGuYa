#include <Runtime/SystemDataEnvironmentValueRuntime.h>

#include <Logging/Logger.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/ResourceVersionRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SystemDataEnvironmentValueRuntime::SystemDataEnvironmentValueRuntime(
    const ResourceVersionRuntime& version, IMessageWindowConfigExpressionParser& parser) noexcept
    : version_(&version), parser_(parser), versionApi_(nullptr)
{
}

SystemDataEnvironmentValueRuntime::SystemDataEnvironmentValueRuntime(
    const ResourceVersionRuntime& version, IMessageWindowConfigExpressionParser& parser,
    IResourceVersionPlatformApi& versionApi) noexcept
    : version_(&version), parser_(parser), versionApi_(&versionApi)
{
}

SystemDataEnvironmentValueRuntime::SystemDataEnvironmentValueRuntime(
    const IResourceVersionSource& source, IMessageWindowConfigExpressionParser& parser,
    IResourceVersionPlatformApi& versionApi) noexcept
    : version_(&source), parser_(parser), versionApi_(&versionApi)
{
}

const ResourceVersionRuntime& SystemDataEnvironmentValueRuntime::CaptureVersion() const
{
    if (const auto* fixed = std::get_if<const ResourceVersionRuntime*>(&version_)) return **fixed;
    return std::get<const IResourceVersionSource*>(version_)->CaptureResourceVersionAt4180C0();
}

RuntimeCStringResult SystemDataEnvironmentValueRuntime::ReadVersionString(
    const SystemDataVersionString query, IRuntimeStringManagerSource& strings)
{
    // 版本缺资源时返回空串的判定仅由真实 ResourceVersionRuntime 完成。
    // 497E00 的 API 异常/未知 length 必须外传，不把失败替换成空版权字符串。
    switch (query)
    {
    case SystemDataVersionString::FileVersionAt497920: return CaptureVersion().FileVersionNative(strings);
    case SystemDataVersionString::ProductVersionAt497A60: return CaptureVersion().ProductVersionNative(strings);
    case SystemDataVersionString::LegalCopyrightAt497E00:
        if (versionApi_) return CaptureVersion().QueryStringNative("LegalCopyright", *versionApi_, strings);
        return CaptureVersion().QueryStringNative("LegalCopyright", strings);
    }
    throw std::invalid_argument("CSysData version string query is invalid");
}

std::uint32_t SystemDataEnvironmentValueRuntime::ReadColorAt4797C0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const std::string_view keyCp932)
{
    return ReadSettingNodeRgbColorAt4797C0(errorCString, tag, keyCp932, parser_);
}

std::int32_t SystemDataEnvironmentValueRuntime::ReadIntegerAt47BBE0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const std::string_view keyCp932)
{
    return ReadSettingNodeIntegerAt47BBE0(errorCString, tag, keyCp932, parser_);
}

void SystemDataEnvironmentLoggingDiagnostics::ReportRecoveredConstructorException(
    const std::uint32_t handlerAddress, std::exception_ptr exception)
{
    if (!exception) throw std::invalid_argument("CSysData recovery requires the original exception");
    // 5ED500..5ED588 的 catch 恢复必须可追溯；记录分配失败直接外传，不吞成成功。
    recoveries_.push_back({handlerAddress, exception});
    // 不假定任意异常 what()/char* 的编码；保留 exception_ptr 供主线检查完整原因。
    KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
        "CSysData constructor recovered at 0x%06X; original retained-value path, "
        "not a clean initialization; recovery count=%zu",
        static_cast<unsigned>(handlerAddress), recoveries_.size());
}

const std::vector<SystemDataEnvironmentRecovery>&
SystemDataEnvironmentLoggingDiagnostics::Recoveries() const noexcept
{
    return recoveries_;
}
}
