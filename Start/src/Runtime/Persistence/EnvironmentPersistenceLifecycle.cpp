#include <Runtime/EnvironmentPersistenceLifecycle.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/PreWindowInitialization.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/EnvironmentPostWindowPersistenceNative.h>
#include <Runtime/EnvironmentPreWindowPersistenceNative.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class Archive>
Archive& CurrentArchive(EnvironmentStartupRuntime& environment, const std::size_t index)
{
    auto* body = static_cast<Archive*>(environment.Archives().CaptureArchiveAt4180C0(static_cast<std::uint32_t>(index)));
    if (!body)
        throw std::logic_error("shutdown persistence requires the current archive slot");
    // 原4887B7/488F57/4892C8/48931F只解析Ref，不增持；这里只借用到本次Serialize返回。
    // 重入换根不能销毁尚在执行的归档正文；不靠额外shared_ptr改变其释放时序。
    return *body;
}

void WriteCurrentPath(EnvironmentStartupRuntime& environment, const SerializeBuffer& serializer,
    const char* SystemDataEnvironmentArchive::* fileName,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    // Env 4887EA/488831、Secret 489350/489397、Global 488F8A/488FD1：
    // Serialize完成之后先重取slot0，再重取Environment+48的路径正文，不提前复制文件名。
    const auto& system = CurrentArchive<SystemDataEnvironmentArchive>(environment, 0);
    const auto* paths = environment.PathsAt48();
    if (!paths)
        throw std::logic_error("shutdown persistence requires the current Environment path owner");
    const RuntimeCStringConcatenation path(paths->saveAt10, system.*fileName);
    // 原466760不补分隔符；本项拼接结果独立存活到488C10写入/关闭返回，不带到下一项。
    static_cast<void>(WriteWholePersistenceFile(path.GetString(), serializer.Bytes(), transaction, files));
}

void WriteCurrentSecret(EnvironmentStartupRuntime& environment,
    const IEnvironmentKeyByteSource& keys,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    SerializeBuffer serializer(keys);
    CurrentArchive<SecretCgArchive>(environment, 3).Serialize(serializer); // 4892A6..4892F8
    CurrentArchive<SecretScArchive>(environment, 4).Serialize(serializer); // 4892FD..48934D
    WriteCurrentPath(environment, serializer, &SystemDataEnvironmentArchive::stringAt2C, transaction, files);
}

void WriteCurrentGlobal(EnvironmentStartupRuntime& environment,
    const IEnvironmentKeyByteSource& keys,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    SerializeBuffer serializer(keys);
    CurrentArchive<GlobalParamsArchive>(environment, 7).Serialize(serializer); // 488F35..488F87
    WriteCurrentPath(environment, serializer, &SystemDataEnvironmentArchive::stringAt30, transaction, files);
}

void WriteCurrentEnvironment(EnvironmentStartupRuntime& environment,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    SerializeBuffer serializer;
    CurrentArchive<SaveDataEnvironmentArchive>(environment, 1).Serialize(serializer); // 488795..4887E7
    WriteCurrentPath(environment, serializer, &SystemDataEnvironmentArchive::stringAt28, transaction, files);
}
}

EnvironmentPersistenceLifecycle::EnvironmentPersistenceLifecycle(
    EnvironmentPersistenceCoordinator& persistence,
    SecretCgArchive& secretCg,
    SecretScArchive& secretSc,
    GlobalParamsArchive& globalParams,
    const IEnvironmentKeyTableSource& keyBytes) noexcept
    : persistence_(persistence),
      secretCg_(secretCg),
      secretSc_(secretSc),
      globalParams_(globalParams),
      keyBytes_(keyBytes)
{
}

EnvironmentPersistenceLifecycle::EnvironmentPersistenceLifecycle(
    EnvironmentPersistenceCoordinator& persistence, SecretCgArchive& secretCg,
    SecretScArchive& secretSc, GlobalParamsArchive& globalParams,
    const IEnvironmentKeyTableSource& keyBytes, PreWindowLifecycleState& rootState) noexcept
    : EnvironmentPersistenceLifecycle(persistence, secretCg, secretSc, globalParams, keyBytes)
{
    rootState_ = &rootState;
}

void EnvironmentPersistenceLifecycle::LoadSecretAfterParamsKeyInstallation()
{
    // 489C90:48A16A..48A1DC：CG 从 0 起算，Scene 从 RawBlob 的半长处起算。
    secretCg_.baseKeyOffsetAt08 = 0;
    secretSc_.SetKeyBaseAtVslot14(static_cast<std::uint32_t>(keyBytes_.RawByteCount() / 2));
    static_cast<void>(persistence_.TryLoadSecret());

    // 原函数无论文件读取成功还是失败，都会在 sub_481220 返回后设置 +0x1AD。
    if (rootState_)
        rootState_->ByteAt1AD() = 1;
    else
        writeGates_.secretAt1AD = true;
}

void EnvironmentPersistenceLifecycle::LoadGlobalAfterIntermediateInitialization()
{
    // 489C90:48A225..48A345：Global 的低、高清单字节索引分别为 0 与半长。
    globalParams_.lowKeyOffsetAt04 = 0;
    globalParams_.highKeyOffsetAt08 =
        static_cast<std::uint32_t>(keyBytes_.RawByteCount() / 2);
    static_cast<void>(persistence_.TryLoadGlobal());

    // 与 Secret 相同，文件缺失不阻止 +0x1B3 成为退出写回门控。
    if (rootState_)
        rootState_->ByteAt1B3() = 1;
    else
        writeGates_.globalAt1B3 = true;
}

void EnvironmentPersistenceLifecycle::WriteOnShutdown()
{
    // 477620:4777A8..4777C9：Secret、Global 按门控写，Env 最后无条件写。
    if (WriteGates().secretAt1AD)
        static_cast<void>(persistence_.WriteSecret());
    if (WriteGates().globalAt1B3)
        static_cast<void>(persistence_.WriteGlobal());
    static_cast<void>(persistence_.WriteEnvironment());
}

void EnvironmentPersistenceLifecycle::WriteCurrentOnShutdownAt4777A8(
    EnvironmentStartupRuntime& environment, const PreWindowLifecycleState& rootState,
    const IEnvironmentKeyByteSource& keys,
    const FileTransactionContext* transaction, IStartFileApi& files)
{
    // 4777A8读byte+1AD，Secret返回后4777B6才读byte+1B3；不先构造固定输入coordinator。
    if (rootState.ByteAt1AD() != 0)
        WriteCurrentSecret(environment, keys, transaction, files);
    if (rootState.ByteAt1B3() != 0)
        WriteCurrentGlobal(environment, keys, transaction, files);
    WriteCurrentEnvironment(environment, transaction, files); // 4777C4无条件写env.ini
    // 5EC9FC/5ECA30/5EC994只有局部展开清理：bool失败忽略，异常外传并停止后续项。
}
void EnvironmentPersistenceLifecycle::WriteCurrentOnShutdownNativeAt4777A8(
    EnvironmentStartupRuntime& environment, PreWindowLifecycleState& rootState,
    FileManagerRootRuntime& keys, const FileTransactionContext* transaction, IStartFileApi& files)
{
    EnvironmentPostWindowPersistenceNativeServices services{environment, keys, rootState, files,
        transaction, environment.Strings(), environment.Memory()};
    if (rootState.ByteAt1AD() != 0)
        static_cast<void>(WriteSecretEnvironmentNativeAt489250(services));
    // 4777B6必须在Secret及其正常清理返回之后读，回调改变gate要在此生效。
    if (rootState.ByteAt1B3() != 0)
        static_cast<void>(WriteGlobalEnvironmentNativeAt488EE0(services));
    static_cast<void>(WritePreWindowEnvironmentAt488740(environment.Archives(), files, transaction));
}

EnvironmentPersistenceWriteGates
EnvironmentPersistenceLifecycle::WriteGates() const noexcept
{
    if (rootState_)
        return {rootState_->ByteAt1AD() != 0, rootState_->ByteAt1B3() != 0};
    return writeGates_;
}
}
