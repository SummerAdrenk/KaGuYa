#pragma once

#include <Runtime/SystemDataEnvironmentArchive.h>
#include <variant>

namespace KaGuYa::Start::Runtime
{
class ResourceVersionRuntime;
class IResourceVersionSource;
class IResourceVersionPlatformApi;
class IMessageWindowConfigExpressionParser;

class SystemDataEnvironmentValueRuntime final : public ISystemDataEnvironmentValueSource
{
public:
    // 显式固定对象入口供工具/夹具使用；生产Startup使用下面的动态source入口。
    // 主线传 MessageWindowConfigExpressionParser；接口保留已有 parser 的可注入形状。
    // version/parser 必须比适配器长寿；不保留传入的 params node/key。
    // 此重载明确选择 ResourceVersionRuntime::QueryString 的原生 Win32 API。
    SystemDataEnvironmentValueRuntime(const ResourceVersionRuntime& version,
        IMessageWindowConfigExpressionParser& parser) noexcept;
    // 注入版本构造所用的同一个 API；不能在 LegalCopyright 查询时偷换成原生 API。
    // versionApi 也必须比适配器长寿，且只用于版本字符串查询，不用于表达式求值。
    SystemDataEnvironmentValueRuntime(const ResourceVersionRuntime& version,
        IMessageWindowConfigExpressionParser& parser, IResourceVersionPlatformApi& versionApi) noexcept;

    // 每个版本查询重新捕获来源；不跨FileVersion/ProductVersion/版权查询缓存对象。
    SystemDataEnvironmentValueRuntime(const IResourceVersionSource& source,
        IMessageWindowConfigExpressionParser& parser, IResourceVersionPlatformApi& versionApi) noexcept;

    [[nodiscard]] RuntimeCStringResult ReadVersionString(SystemDataVersionString query,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) override;
    [[nodiscard]] std::uint32_t ReadColorAt4797C0(
        RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, std::string_view keyCp932) override;
    [[nodiscard]] std::int32_t ReadIntegerAt47BBE0(
        RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, std::string_view keyCp932) override;

private:
    [[nodiscard]] const ResourceVersionRuntime& CaptureVersion() const;
    std::variant<const ResourceVersionRuntime*, const IResourceVersionSource*> version_;
    IMessageWindowConfigExpressionParser& parser_;
    IResourceVersionPlatformApi* versionApi_;
};

struct SystemDataEnvironmentRecovery final
{
    std::uint32_t handlerAddress;
    std::exception_ptr exception;
};

// 每次启动构造使用独立实例；不初始化/关闭全局 Logging，不改主线日志 owner。
// 496D7E 的 ready=1 只表示到达原构造尾，不代表没有配置错误。
// 原九个恢复分支写 Error 日志并留下可检查的异常记录，即使日志被关闭也不丢失状态。
class SystemDataEnvironmentLoggingDiagnostics final : public ISystemDataEnvironmentDiagnostics
{
public:
    void ReportRecoveredConstructorException(
        std::uint32_t handlerAddress, std::exception_ptr exception) override;
    [[nodiscard]] const std::vector<SystemDataEnvironmentRecovery>& Recoveries() const noexcept;

private:
    std::vector<SystemDataEnvironmentRecovery> recoveries_;
};
}
