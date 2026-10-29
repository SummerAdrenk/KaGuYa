#pragma once

#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/StartupScriptTransaction.h>
#include <Runtime/StartupParamsState.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IGraphicsResourceErrorDisplay;

// 只表达 0x4D53D0 对 0x417990 registry 槽 1/2/4/7 的已确认访问。
class INormalStartupRegistryState
{
public:
    virtual ~INormalStartupRegistryState() = default;

    virtual void WriteSlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteSlot2ByteAt0C(std::uint8_t value) = 0;
    virtual void WriteSlot2DwordAt18(std::uint32_t value) = 0;
    virtual void WriteSlot2DwordAt14(std::uint32_t value) = 0;
    virtual void WriteSlot2DwordAt1C(std::uint32_t value) = 0;
    virtual void WriteSlot2DwordAt20(std::uint32_t value) = 0;
    virtual void WriteSlot2ByteAt08(std::uint8_t value) = 0;
    virtual void WriteSlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteSlot2ByteAt28(std::uint8_t value) = 0;
    virtual void WriteSlot2ByteAt29(std::uint8_t value) = 0;
    [[nodiscard]] virtual std::uint16_t ReadSlot7WordAt0C() const = 0;
    [[nodiscard]] virtual const char* ReadSlot1NameAt58() const = 0;
    [[nodiscard]] virtual const char* ReadSlot1NameAt5C() const = 0;
    virtual void CallSlot4Nullsub3() = 0;
};

// 根+18所持 CDrawManager 的+14 owning数组，由47E940逐项调用虚表+18。
class INormalStartupRootOwnerSynchronizer
{
public:
    virtual ~INormalStartupRootOwnerSynchronizer() = default;
    virtual void SynchronizeAtVslot18() = 0;
};

// 0x4D53D0 normal/replay 两分支的生产服务绑定。它不拥有 registry、VFS、
// Graphics 或 CScript；generation 可拥有真实 shared 槽或借用 root 槽，借用须覆盖服务生命周期。
class NormalStartupScriptServices final
    : public IStartupScriptTransactionServices
{
public:
    NormalStartupScriptServices(
        ScrVmParamsGenerationBinding generationSlot,
        ScrScriptRuntimeState& scriptRuntime,
        const IScrResourceLookupSource& resourceNames,
        IStartupParamsState& paramsState,
        const ResourceCatalog& catalog,
        const FileTransactionContext* transactionContext,
        IStartFileApi& fileApi,
        const IEnvironmentKeyTableSource& environmentKeyBytes,
        INormalStartupRegistryState& registry,
        INormalStartupRootOwnerSynchronizer& rootOwners,
        IGraphicsRuntimeCalls& graphics,
        IGraphicsResourceErrorDisplay& errors,
        ArchiveCodecServices codecs);

    void ReplaceParamsGeneration() override;
    void WriteRegistrySlot2DwordAt04(std::uint32_t value) override;
    void WriteRegistrySlot2ByteAt0C(std::uint8_t value) override;
    void WriteRegistrySlot2DwordAt18(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt14(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt1C(std::uint32_t value) override;
    void WriteRegistrySlot2DwordAt20(std::uint32_t value) override;
    void WriteRegistrySlot2ByteAt08(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt09(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt28(std::uint8_t value) override;
    void WriteRegistrySlot2ByteAt29(std::uint8_t value) override;
    [[nodiscard]] std::int32_t ResolveScriptId(
        const char* scriptNameCp932) const override;
    [[nodiscard]] ScrScriptSwitchResult LoadScript(std::int32_t scriptId) override;
    void CopyRegistryPrologueFlag() override;
    void WriteReplayFile(std::int32_t scriptId) override;
    void WriteReplayPosition(std::int32_t entry) override;
    void SynchronizeRootOwnersAtVslot18() override;
    [[nodiscard]] const char* ReadRegistrySlot1NameAt58() const override;
    [[nodiscard]] const char* ReadRegistrySlot1NameAt5C() const override;
    void SetGraphicsTextFirstName(const char* nameCp932) override;
    void SetGraphicsTextSecondName(const char* nameCp932) override;
    void MirrorParamsDwordAt3CTo40() override;
    void CallRegistrySlot4Nullsub3() override;

private:
    IStartupParamsState& paramsState_;
    ScrScriptRuntimeState* scriptRuntime_ = nullptr;
    const IScrResourceLookupSource* resourceNames_ = nullptr;
    const ResourceCatalog* catalog_ = nullptr;
    const FileTransactionContext* transactionContext_ = nullptr;
    IStartFileApi* fileApi_ = nullptr;
    const IEnvironmentKeyTableSource* environmentKeyBytes_ = nullptr;
    INormalStartupRegistryState* registry_ = nullptr;
    INormalStartupRootOwnerSynchronizer* rootOwners_ = nullptr;
    IGraphicsRuntimeCalls* graphics_ = nullptr;
    IGraphicsResourceErrorDisplay* errors_ = nullptr;
    ArchiveCodecServices codecs_;
};
}
