#pragma once

#include <Runtime/RestoredScriptTransaction.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>
#include <Runtime/ScrResourceNameSource.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsRuntimeCalls;
class IGraphicsResourceErrorDisplay;
class INormalStartupRootOwnerSynchronizer;
class SaveDataEnvironmentArchive;
class ScrVmCore;
class ScrVmExecutableProfile;
class ScrVmValueTable;
class UnSaveDataRuntime;

// 引用实际恢复后的对象，不缓存 generation/人名/存档位置/已装载 SCR。
// rootOwners 仅复用 47E940 的窄同步边界；不借用 NormalStartup 的 Reset/registry 宿主。
struct RestoredScriptServicesDependencies final
{
    ScrVmParamsGenerationBinding generation;
    ScrScriptRuntimeState& script;
    ScrVmValueTable& values;
    // 仅转发稳定 source；4D4CF9 读恢复 ID 后，4D4CFE 的 LoadScript 才捕获 Pattern。
    const IScrResourceNameSource& resourceNames;
    const ScrVmExecutableProfile& profile;
    const ResourceCatalog& catalog;
    const FileTransactionContext* transaction;
    IStartFileApi& files;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    const SaveDataEnvironmentArchive& save;
    UnSaveDataRuntime& unsave;
    INormalStartupRootOwnerSynchronizer& rootOwners;
    IGraphicsRuntimeCalls& graphics;
    const ScrVmCore& vm;
    // 4D3EE6 的 MessageBox 叶，生产可复用 Win32DemoResourceErrorDisplay。
    IGraphicsResourceErrorDisplay& errors;
};

// 4D4CC0 的具体服务。事务逐项提交顺序仍由 RestoredScriptTransaction 拥有。
class RestoredScriptServicesRuntime final : public IRestoredScriptTransactionServices
{
public:
    explicit RestoredScriptServicesRuntime(RestoredScriptServicesDependencies dependencies);

    [[nodiscard]] std::int32_t ReadRestoredScriptId() const override;
    [[nodiscard]] ScrScriptSwitchResult LoadScript(std::int32_t scriptId) override;
    void SynchronizeRootOwnersAtVslot18() override;
    [[nodiscard]] const char* ReadRegistrySlot1NameAt58() const override;
    [[nodiscard]] const char* ReadRegistrySlot1NameAt5C() const override;
    void SetGraphicsTextFirstName(const char* nameCp932) override;
    void SetGraphicsTextSecondName(const char* nameCp932) override;
    void WriteRegistrySlot2ByteAt0C(std::uint8_t value) override;
    void WriteRegistrySlot2DwordAt18(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt14(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt1C(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt20(std::uint32_t value) override;
    void WriteRegistrySlot2ByteAt08(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt09(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt28(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt29(std::uint8_t value) override;
    void WriteScriptVariableReplay(std::int32_t value) override;
    [[nodiscard]] std::int32_t ReadRestoredInstructionIndex() const override;
    [[nodiscard]] std::int32_t ReadLoadedInstructionCount() const override;
    [[nodiscard]] std::int32_t ReadLoadedInstructionProgramCounter(std::int32_t instructionIndex) const override;
    void WriteEnvironmentProgramCounter(std::int32_t programCounter) override;
    [[nodiscard]] std::int32_t DispatchCurrentOpcode() override;

private:
    [[nodiscard]] const ScrContainer& LoadedScript() const;
    RestoredScriptServicesDependencies dependencies_;
};
}
