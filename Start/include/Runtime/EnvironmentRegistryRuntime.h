#pragma once

#include <Runtime/EnvironmentInstallRuntime.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SpriteAnimationInitializationSource.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/EnvironmentPersistence.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// 只借用唯一60h根：八个正文及错误数组均归该根所有，接口不缓存槽、不增持对象。
class EnvironmentRegistryRuntime final : public ISystemDataInstallSource,
    public IEnvironmentPathErrorSink, public ISpriteAnimationInitializationSource,
    public ISaveFileCatalogScanSource
{
public:
    explicit EnvironmentRegistryRuntime(EnvironmentRootOwner& owner,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory()) noexcept;
    [[nodiscard]] EnvironmentRootOwner& RootSource() const noexcept { return owner_; }
    [[nodiscard]] IRuntimeStringManagerSource& Strings() const noexcept { return strings_; }
    [[nodiscard]] IEnvironmentRootMemory& Memory() const noexcept { return memory_; }

    // 47E2B0完整入口：七项按固定顺序登记，最后验证安装项；不清旧表、不回滚。
    void RegisterAt47E2B0(ParamsRuntimeRootOwner& params,
        ISoundParamsSettingsSource& sounds, IEnvironmentPathApi& paths,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    [[nodiscard]] IEnvironmentArchiveRuntime* CaptureArchiveAt4180C0(std::uint32_t index) const;
    [[nodiscard]] std::size_t Size() const;
    [[nodiscard]] EnvironmentPersistenceCoordinator Persistence(
        const EnvironmentPathRuntime& paths, const IEnvironmentKeyByteSource& keys,
        const FileTransactionContext* transaction, IStartFileApi& files) const;
    [[nodiscard]] SystemDataEnvironmentArchive& SystemData() const;
    [[nodiscard]] SaveDataEnvironmentArchive& SaveData() const;
    [[nodiscard]] UnSaveDataRuntime& UnSaveData() const;
    [[nodiscard]] SecretCgArchive& SecretCg() const;
    [[nodiscard]] SecretScArchive& SecretSc() const;
    [[nodiscard]] ClickInfoArchive& ClickInfo() const;
    [[nodiscard]] SaveFileStateNativeArchive& SaveFileState() const;
    SaveFileStateNativeArchive& CaptureSaveFileStateAt4180C0() override { return SaveFileState(); }
    RuntimeStringBorrow CaptureSaveDirectoryAt4180C0() override;
    [[nodiscard]] GlobalParamsArchive& GlobalParams() const;
    [[nodiscard]] std::uint8_t ReadRegistrySlot1ByteAt6D() const override;

    const SystemDataInstalledFiles& CaptureSystemDataInstallFilesAt4180C0() const override;
    EnvironmentPathRuntime& CaptureInstallPathsAt4180C0() const override;
    void AppendMissingInstallFile(const char* nameCp932) override;
    void AppendInsertInstallDisc(int driveByte) override;
    // 已格式化的显式调用保留其长度；46A040生产错误使用上面的两个原格式入口。
    void AppendPathError(std::string textCp932) override;
    [[nodiscard]] RuntimeCStringResult ErrorTextAt479EA0() const;

private:
    [[nodiscard]] EnvironmentRootStorage& CaptureRoot() const;
    void AppendFormat(const char* format, ...);
    template<class T> T& Require(std::uint32_t index) const
    {
        auto* body = CaptureArchiveAt4180C0(index);
        if (!body) throw std::logic_error("Environment registry: missing archive payload (safe boundary)");
        // 原417990不检查vptr或ready；不引入variant类型门控。
        return *static_cast<T*>(body);
    }
    EnvironmentRootOwner& owner_;
    IRuntimeStringManagerSource& strings_;
    IEnvironmentRootMemory& memory_;
};
}
