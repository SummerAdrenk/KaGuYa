#pragma once

#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/SystemDataEnvironmentLoader.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/MessageWindowConfigExpressionParser.h>
#include <Runtime/ResourceVersionRuntime.h>
#include <Runtime/MessageEnvironmentPathSource.h>
#include <Runtime/SaveFileCatalog.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 原498280的固定receiver执行体。services仅在调用期借用，不进入60h正文。
void InitializeEnvironmentAt498280(EnvironmentRootStorage& receiver, EnvironmentRootOwner& source,
    ParamsRuntimeRootOwner& params, HMODULE module, HINSTANCE previousInstance,
    LPSTR commandLine, int showCommand, IEnvironmentPathApi& pathApi,
    IResourceVersionPlatformApi& versionApi, IStartFileApi& files,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner(),
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());

// 服务装配器只持有一个原生根owner；窗口、路径、版本和归档不再各存一份。
// 498280的receiver与内部4180C0来源分开，重入时不进行事务性替换。
class EnvironmentStartupRuntime final : public IInstalledFileTableSource, public ISystemDataInstallSource,
    public IMessageEnvironmentPathSource, public IResourceVersionSource, public ISaveFileCatalogScanSource
{
public:
    explicit EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params);
    EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params, IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    // 进程槽独立登记原退出回调；本适配器只借同一owner，不再建第二根或额外锁。
    EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params, EnvironmentRootOwner& root,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ~EnvironmentStartupRuntime() noexcept = default;
    EnvironmentStartupRuntime(const EnvironmentStartupRuntime&) = delete;
    EnvironmentStartupRuntime& operator=(const EnvironmentStartupRuntime&) = delete;

    void InitializeAt498280(HMODULE module, HINSTANCE previousInstance, LPSTR commandLine, int showCommand,
        IEnvironmentPathApi& pathApi, IResourceVersionPlatformApi& versionApi,
        IStartFileApi& files, ISystemDataEnvironmentDiagnostics& diagnostics);

    [[nodiscard]] EnvironmentRegistryRuntime& Archives() noexcept { return archives_; }
    [[nodiscard]] const EnvironmentRegistryRuntime& Archives() const noexcept { return archives_; }
    [[nodiscard]] EnvironmentRootOwner& RootSource() const noexcept { return root_; }
    [[nodiscard]] IRuntimeStringManagerSource& Strings() const noexcept { return strings_; }
    [[nodiscard]] IEnvironmentRootMemory& Memory() const noexcept { return memory_; }
    [[nodiscard]] EnvironmentRootStorage& CaptureAt4180C0() const;
    // 只借稳定 root，不在装配 Sound source 时提前读取/保活某一代 GameSystem。
    [[nodiscard]] ParamsRuntimeRootOwner& ParamsRootSource() const noexcept { return params_; }
    [[nodiscard]] EnvironmentPathRuntime* PathsAt48() const
        { return ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(CaptureAt4180C0().pathsAt48); }
    [[nodiscard]] ResourceVersionRuntime* VersionAt54() const
        { return ResolveEnvironmentObjectReference<ResourceVersionRuntime>(CaptureAt4180C0().versionAt54); }
    [[nodiscard]] const ResourceVersionRuntime& CaptureResourceVersionAt4180C0() const override;
    [[nodiscard]] EnvironmentRootStorage& WindowFields() const { return CaptureAt4180C0(); }
    [[nodiscard]] const EnvironmentInstalledFiles& CaptureInstalledTableAt4180C0() override;
    [[nodiscard]] const SystemDataInstalledFiles& CaptureSystemDataInstallFilesAt4180C0() const override;
    [[nodiscard]] EnvironmentPathRuntime& CaptureInstallPathsAt4180C0() const override;
    [[nodiscard]] const RuntimeCString& CaptureMessagePrefixAt45A5D0() const override;
    [[nodiscard]] const RuntimeCString& CaptureUnreadPrefixAt43D3D0() const override;
    SaveFileStateNativeArchive& CaptureSaveFileStateAt4180C0() override { return archives_.SaveFileState(); }
    RuntimeStringBorrow CaptureSaveDirectoryAt4180C0() override { return CaptureInstallPathsAt4180C0().saveAt10; }

private:
    ParamsRuntimeRootOwner& params_;
    std::optional<EnvironmentRootCriticalSection> lock_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    std::optional<EnvironmentRootOwner> ownedRoot_;
    EnvironmentRootOwner& root_;
    EnvironmentRegistryRuntime archives_;
};
}
