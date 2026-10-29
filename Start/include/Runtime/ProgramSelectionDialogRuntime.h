#pragma once

#include <Runtime/ProgramSelectionSceneRuntime.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
enum class ProgramSelectionDialogMode : std::int32_t
{
    Mode00 = 0,
    Mode01 = 1,
};

enum class ProgramSelectionDialogFrameSlot : std::int32_t
{
    At98 = 0,
    AtA4 = 1,
};

// 0x4C57F0 初始化的 0x30 字节 operation，对应对象 +0xB0..+0xDC。
struct ProgramSelectionDialogTransitionOperation
{
    std::int32_t valueAt00 = 0;
    std::uint32_t sourceAt04 = 0;
    std::int32_t valueAt08 = 0;
    std::int32_t valueAt0C = 0;
    std::int32_t valueAt10 = 0;
    std::int32_t valueAt14 = 0;
    std::int32_t valueAt18 = 0;
    std::int32_t selectorAt1C = -1;
    std::int32_t valueAt20 = 0;
    std::int32_t progressAt24 = 0;
    std::int32_t maximumAt28 = 255;
    std::int32_t valueAt2C = 0;

    void Reset() noexcept;
};

// 这些字段逐项对应 CharaSelect00/01Dialog 的已确认对象偏移。
// +0x98 更新函数会直接改写它们，因此以一个显式状态块穿过服务边界。
struct ProgramSelectionDialogState
{
    std::int32_t stateAt68 = -1;
    std::int32_t substateAt6C = 0;
    std::int32_t pendingStateAt70 = 0;
    std::int32_t resultAt94 = -1;
    ProgramSelectionDialogTransitionOperation transitionAtB0;
    std::int32_t transitionStepAtE0 = 0;
};

class IProgramSelectionDialogCoreServices
{
public:
    virtual ~IProgramSelectionDialogCoreServices() = default;

    virtual void WriteDialogEnabledAtVslot24(std::int32_t value) = 0;
    virtual void ResetBaseChildrenAt522E90() = 0;
    virtual void BeginRegisteredChildrenAtVslot44() = 0;

    // 0x51C120/0x51C4E0 的直接依赖。状态机和 operation 由 runtime 持有，
    // services 只暴露原函数确实访问的根对象、资源对象和 DLL 边界。
    virtual void CompositeFrameAtA4AtVslot5C(std::int32_t argument) = 0;
    [[nodiscard]] virtual std::uint32_t ResolveFrameSourceAt444770(
        ProgramSelectionDialogFrameSlot slot,
        std::int32_t index) = 0;
    [[nodiscard]] virtual std::int32_t ReadRegistryTimeAt51C120() = 0;
    [[nodiscard]] virtual std::string ReadTimeSoundNameAt61A664(
        std::uint32_t index) = 0;
    [[nodiscard]] virtual std::int32_t
        ReadProgramSelectionValueAtVslot14() = 0;
    virtual void WriteScriptSlot2StringAt2C(const std::string& value) = 0;
    virtual void ApplyTimeSoundAt42BFE0(
        const std::string& name,
        std::int32_t value,
        std::int32_t argument) = 0;
    virtual void StopProgramSelectionSoundsAt4384D0() = 0;
    virtual void ApplyScriptSlot2SoundStopListAt413270() = 0;
    virtual void WriteScriptSlot2DwordAt04(std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t ResolveTransitionStepAt497900() = 0;
    [[nodiscard]] virtual bool PollTransitionAccelerationAt4C3430() = 0;
    [[nodiscard]] virtual bool PollTransitionAccelerationAt4AA970() = 0;
    virtual void SynchronizeRuntimeAt413BE0() = 0;
    virtual void RenderTransitionAt449DD0(
        ProgramSelectionDialogTransitionOperation& operation) = 0;
    virtual void CommitTransitionAt4B02E0(std::int32_t argument) = 0;

    [[nodiscard]] virtual bool ReadChildAtVslot20(std::int32_t childId) = 0;
    virtual void InvokeChildAtVslot48(std::int32_t childId) = 0;
    [[nodiscard]] virtual bool ReadChildAtVslot28(std::int32_t childId) = 0;
    [[nodiscard]] virtual bool ReadChildAtVslot48(std::int32_t childId) = 0;
    virtual void InvokeChildAtVslot4C(std::int32_t childId) = 0;
    [[nodiscard]] virtual std::int32_t ReadChildDwordAt90(
        std::int32_t childId) = 0;
    [[nodiscard]] virtual std::int32_t ReadChildDwordAt80(
        std::int32_t childId) = 0;

    virtual void ProcessDialogAtVslot58() = 0;
    [[nodiscard]] virtual ProgramSelectionReference
        CreateSelectionReferenceFromChildZeroAt503AA0() = 0;
    virtual void WriteProgramResultAt509340(std::int32_t result) = 0;

    [[nodiscard]] virtual std::uint32_t
        ReadSystemInformationSlot2DwordAt14() = 0;
    virtual void WriteChildDwordAt5C(
        std::int32_t childId,
        std::int32_t value) = 0;
    virtual void InvokeChildAtVslot44(
        std::int32_t childId,
        std::int32_t value) = 0;
    virtual void ApplyDialogRectToRootAt530E40() = 0;
};

// 两种角色选择框共享的外层状态机。模式差异只保留在原虚表确实不同的槽位。
class ProgramSelectionDialogRuntime final : public IProgramSelectionDialog
{
public:
    ProgramSelectionDialogRuntime(
        ProgramSelectionDialogMode mode,
        IProgramSelectionDialogCoreServices& services);

    void BeginAtVslot44() override;
    [[nodiscard]] ProgramSelectionReference ReadReferenceAtVslot9C() override;
    [[nodiscard]] bool ReadBusyAtVslot48() const override;
    void CloseAtVslot4C() override;
    void ReopenAtVslotA4() override;

    [[nodiscard]] ProgramSelectionDialogMode Mode() const noexcept;
    [[nodiscard]] const ProgramSelectionDialogState& State() const noexcept;

private:
    void AdvanceTransitionAtVslot98() const;
    void CompleteSelectedChildAtVslot94() const;

    ProgramSelectionDialogMode mode_ = ProgramSelectionDialogMode::Mode00;
    IProgramSelectionDialogCoreServices* services_ = nullptr;
    mutable ProgramSelectionDialogState state_{};
};
}
