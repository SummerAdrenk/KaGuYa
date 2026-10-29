#pragma once

#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/PreWindowInitialization.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class IEnvironmentInitializationDisplay;

// 4778B0服务只借用真实根；不建立profile、variant归档表或窗口字段副本。
// WindowMode槽仍由外层运行根持有，尚不代表21Ch根或wWinMain已经装配完成。
class EnvironmentRootPreWindowServices final : public IPreWindowServices
{
public:
    EnvironmentRootPreWindowServices(EnvironmentRootOwner& environment, ParamsRuntimeRootOwner& params,
        IEnvironmentPathApi& paths, IStartFileApi& files, const FileTransactionContext* transaction,
        IGraphicsWindowModeApi& graphics, IWindowModePlatformApi& windowApi,
        YtlReferenceSlotRuntime& windowModeOwner, WindowModeRuntimeServices& windowModeServices,
        IEnvironmentInitializationDisplay& display) noexcept;

    void RegisterEnvironmentArchives(const std::array<EnvironmentArchiveKind, 7>& order) override;
    bool PrepareEnvironmentPersistence() override;
    void WriteEnvironmentPersistence() override;
    void InstallWindowModeManager() override;
    const SaveDataEnvironmentArchive& SaveDataAtSlot1() const override;
    ClickInfoArchive& MutableClickInfoAtSlot5() override;
    std::string BuildInitializationErrorText() override;
    void ShowInitializationError(std::string_view text, std::string_view caption, unsigned type) override;

private:
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
    EnvironmentRegistryRuntime registry_;
    IEnvironmentPathApi& paths_;
    IStartFileApi& files_;
    const FileTransactionContext* transaction_;
    IGraphicsWindowModeApi& graphics_;
    IWindowModePlatformApi& windowApi_;
    YtlReferenceSlotRuntime& windowModeOwner_;
    WindowModeRuntimeServices& windowModeServices_;
    IEnvironmentInitializationDisplay& display_;
};
}
