#pragma once

#include <Runtime/EnvironmentPersistence.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class EnvironmentStartupRuntime;
class FileManagerRootRuntime;
struct PreWindowLifecycleState;
struct EnvironmentPersistenceWriteGates final
{
    bool secretAt1AD = false;
    bool globalAt1B3 = false;
};

class EnvironmentPersistenceLifecycle final
{
public:
    EnvironmentPersistenceLifecycle(
        EnvironmentPersistenceCoordinator& persistence,
        SecretCgArchive& secretCg,
        SecretScArchive& secretSc,
        GlobalParamsArchive& globalParams,
        const IEnvironmentKeyTableSource& keyBytes) noexcept;
    // 生产 owner 使用同一个根状态；+1AD/+1B3 分别与前置清零 WORD 重叠。
    EnvironmentPersistenceLifecycle(
        EnvironmentPersistenceCoordinator& persistence,
        SecretCgArchive& secretCg, SecretScArchive& secretSc,
        GlobalParamsArchive& globalParams, const IEnvironmentKeyTableSource& keyBytes,
        PreWindowLifecycleState& rootState) noexcept;

    void LoadSecretAfterParamsKeyInstallation();
    void LoadGlobalAfterIntermediateInitialization();
    // 固定输入入口：沿用构造时的归档引用和路径，供既有调用/测试使用。
    void WriteOnShutdown();

    // 4777A8..4777C4：生产入口逐项晚取同根；不提前固定归档、路径或两个gate。
    static void WriteCurrentOnShutdownAt4777A8(
        EnvironmentStartupRuntime& environment, const PreWindowLifecycleState& rootState,
        const IEnvironmentKeyByteSource& keys,
        const FileTransactionContext* transaction, IStartFileApi& files);
    // 正式FileManager域显式命名，不能由const/重载选择悄悄退回固定key宿主域。
    static void WriteCurrentOnShutdownNativeAt4777A8(
        EnvironmentStartupRuntime& environment, PreWindowLifecycleState& rootState,
        FileManagerRootRuntime& keys,
        const FileTransactionContext* transaction, IStartFileApi& files);

    [[nodiscard]] EnvironmentPersistenceWriteGates WriteGates() const noexcept;

private:
    EnvironmentPersistenceCoordinator& persistence_;
    SecretCgArchive& secretCg_;
    SecretScArchive& secretSc_;
    GlobalParamsArchive& globalParams_;
    const IEnvironmentKeyTableSource& keyBytes_;
    EnvironmentPersistenceWriteGates writeGates_;
    PreWindowLifecycleState* rootState_ = nullptr;
};
}
