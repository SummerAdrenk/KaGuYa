#pragma once
#include <Runtime/PreWindowInitializationNative.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
struct RootPreWindowInitializationNativeDependencies final
{
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    IEnvironmentPathApi& paths;
    IStartFileApi& files;
    const FileTransactionContext* transaction;
    IGraphicsWindowModeApi& graphics;
    IWindowModePlatformApi& windows;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    decltype(&::MessageBoxA) messageBox;
};
// 入口receiver固定，不随注册/分配/显示回调换根；Env仍在原叶处独立捕获。
class RootPreWindowInitializationNativeServices final : public IPreWindowInitializationNativeServices
{
public:
    RootPreWindowInitializationNativeServices(RootRuntimePayload&,
        RootPreWindowInitializationNativeDependencies) noexcept;
    void RegisterEnvironmentArchivesAt47E2B0() override;
    bool PrepareEnvironmentPersistenceAt480230() override;
    void WriteEnvironmentPersistenceAt488740() override;
    void InstallWindowModeAt47791D() override;
    const SaveDataEnvironmentArchive& SaveDataAtSlot1() override;
    ClickInfoArchive& ClickInfoAtSlot5() override;
    void BuildInitializationErrorTextAt479EA0(RuntimeCString&) override;
    int MessageBoxA(HWND, const char*, const char*, UINT) override;
private:
    RootRuntimePayload& selected_;
    RootPreWindowInitializationNativeDependencies dependencies_;
    EnvironmentRegistryRuntime registry_;
};
}
