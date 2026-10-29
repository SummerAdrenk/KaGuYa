#pragma once

#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ResourceCatalog.h>
#include <Runtime/ScrContainer.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/ScrVmParamsGenerationBinding.h>
#include <Runtime/ScrResourceLoader.h>
#include <Runtime/ScrResourceNameSource.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 显式快照/工具入口；生产当前Pattern来源使用ParamsResourceNameRuntime。
// 4AD1B0的null与合法空串由receiver查询区分，旧NameAt便利入口仍返回空串引用。
class ScrResourceNameTable final : public IScrResourceLookupSource, public IScrResourceNameRootSource
{
public:
    explicit ScrResourceNameTable(std::vector<std::string> names);

    [[nodiscard]] const std::string& NameAt(std::int32_t scriptId) const noexcept;
    [[nodiscard]] std::int32_t FindId(const std::string& resourceName) const noexcept;
    [[nodiscard]] std::size_t Size() const noexcept;
    [[nodiscard]] ScrResourceNameReceiver CapturePatternNames() const noexcept override;
    [[nodiscard]] ScrResourceNameRootReceiver CaptureParamsForNames() const noexcept override;
    [[nodiscard]] std::int32_t FindResourceIdAt47A020(const char* nameCp932) const override;

private:
    static ScrResourceNameReceiver CaptureRootPattern(const void* receiver) noexcept;
    static const char* FindName(const void* receiver, std::int32_t id) noexcept;
    std::vector<std::string> names_;
};

enum class ScrScriptSwitchError : std::uint8_t
{
    None,
    MissingResourceName,
    ResourceLoadFailed,
    ContainerParseFailed,
    UnexpectedFailure,
};

struct ScrScriptSwitchResult final
{
    bool success = false;
    ScrScriptSwitchError error = ScrScriptSwitchError::UnexpectedFailure;
    std::optional<ScrResourceLoadError> resourceError;
    std::optional<ScrContainerError> containerError;
    std::size_t containerErrorOffset = 0;
    // 与原 MessageBoxA 链一致，文本保存为 CP932 字节串。
    std::string message;
    // 原生4D3C90已执行错误显示；现代值入口仍交由caller显示message。
    bool failureAlreadyReported=false;
};

// 对应 CScript+0xA0 脚本状态和 CParams 当前脚本/PC 槽位之间的切换关系。
class ScrScriptRuntimeState final
{
public:
    // 旧构造仅供独立夹具：拥有prefix-only正文，不代替root的完整47CD00构造。
    ScrScriptRuntimeState();
    explicit ScrScriptRuntimeState(
        ScrVmParamsGenerationBinding generationSlot);
    ScrScriptRuntimeState(ScriptNativeBody& body, ScriptNativePublication& publication,
        ScrVmParamsGenerationBinding generationSlot);
    ScrScriptRuntimeState(ScrScriptRuntimeState&& other) noexcept;
    ScrScriptRuntimeState(const ScrScriptRuntimeState&) = delete;
    ScrScriptRuntimeState& operator=(const ScrScriptRuntimeState&) = delete;

    [[nodiscard]] const ScrContainer* LoadedScript() const noexcept;
    // 原CScript+A0的唯一正文；原生handler不经LoadedScript的有效性门槛预判。
    [[nodiscard]] ScrContainer& NativeParserAtA0() noexcept { return body_->parserAtA0; }
    [[nodiscard]] const ScrContainer& NativeParserAtA0() const noexcept { return body_->parserAtA0; }
    [[nodiscard]] ScriptNativeBody& NativeBody() noexcept { return *body_; }
    [[nodiscard]] const ScriptNativeBody& NativeBody() const noexcept { return *body_; }
    [[nodiscard]] ScriptNativePublication& Publication() noexcept { return *publication_; }
    [[nodiscard]] const ScriptNativePublication& Publication() const noexcept { return *publication_; }
    [[nodiscard]] std::int32_t LoadedScriptId() const noexcept;
    [[nodiscard]] std::int32_t EnvironmentScriptId() const;
    [[nodiscard]] std::uint32_t ProgramCounter() const;
    // 只借当前generation的+14；引用不能跨换代保存。
    [[nodiscard]] std::uint32_t& ProgramCounterSlotAt14() { return generationSlot_->Current().programCounter; }
    [[nodiscard]] std::uint32_t StagedProgramCounter() const noexcept;
    [[nodiscard]] std::string_view CurrentTitle() const;
    [[nodiscard]] RuntimeCString& NativeTitleAt48CF90();
    [[nodiscard]] bool SaveCommandGate() const;
    [[nodiscard]] bool IsSaveCommandGatePublished() const noexcept;
    [[nodiscard]] std::optional<bool> SaveCommandGateState() const noexcept;
    [[nodiscard]] std::int32_t CurrentSaveSlot() const;
    [[nodiscard]] bool UsesParamsGenerationSlot(
        const ScrVmParamsGenerationBinding& generationSlot) const noexcept;

    // CParams+0x14 保存已提交 PC；CScript+0xB0 是 handler 可覆盖的 staged PC。
    void SetProgramCounter(std::uint32_t programCounter);
    void SetStagedProgramCounter(std::uint32_t programCounter) noexcept;
    void ApplyProgramCounterTargetAt4D6E90(std::int32_t target);
    void CommitStagedProgramCounter();
    void SetCurrentTitle(std::string_view title);
    void SetSaveCommandGate(bool enabled) noexcept;
    void SetCurrentSaveSlot(std::int32_t slot);

    // 0x481720 -> 0x45B020/0x46CE30：先构造新 generation，再替换根强引用。
    // CScript 已解析正文、staged PC 与 save-command gate 不在该 generation 内。
    void ReplaceParamsGeneration();

    // 对应 0x4D3C90。该入口吸收异常并返回 false；调用者可以原样显示 message。
    [[nodiscard]] ScrScriptSwitchResult LoadScript(
        std::int32_t scriptId,
        const IScrResourceNameSource& resourceNames,
        const ResourceCatalog& catalog,
        const FileTransactionContext* transactionContext,
        IStartFileApi& api,
        const IEnvironmentKeyTableSource& environmentKeyBytes,
        ArchiveCodecServices codecs);

private:
    friend class ScrVmCore;
    // root+1C0借用不得跨payload生存期；稳定view每次取Current，不缓存正文或伪造shared所有权。
    ScrVmParamsGenerationBinding generationSlot_;
    // 夹具正文先于其publication及CParams拥有槽销毁；生产仅借用外部正文和记录。
    ScriptNativePublication fixturePublication_;
    std::unique_ptr<ScriptNativeBody> fixtureBody_;
    ScriptNativeBody* body_ = nullptr;
    ScriptNativePublication* publication_ = &fixturePublication_;
};
}
