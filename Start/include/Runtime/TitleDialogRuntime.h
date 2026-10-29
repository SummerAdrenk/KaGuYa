#pragma once

#include <Runtime/TitleMenuDispatchRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/CgDisplayButtonGroupRuntime.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
class ITitleDialogExternalServices
{
public:
    virtual ~ITitleDialogExternalServices() = default;
    virtual void InvalidatedRect(const RECT& rect) = 0;
    virtual bool ReadInputAt4C3430() = 0;
    virtual bool ReadInputAt4AA970() = 0;
    virtual void RenderTransitionAt449DD0(LoadAfterTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrameAt4B02E0(std::int32_t argument) = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual std::int32_t ReadRegistrySlot1BgmVolumeAtVslot14() = 0;
    virtual const char* ReadTitleBgmNameAt61A654() = 0;
    virtual void WriteRegistrySlot2BgmNameAt2C(const char* name) = 0;
    virtual void PlayBgmAt42BFE0(const char* name, std::int32_t volume, std::int32_t loop) = 0;
    virtual void ClearSoundManagerAt4384D0() = 0;
    virtual void StopSoundNamedByRegistrySlot2At30() = 0;
    virtual bool ReadInputAt47F6D0() = 0;
    virtual bool ReadSceneManagerAt485CE0() = 0;
    virtual void PlayConfirmSoundAt4BCD70() = 0;
    virtual std::uint32_t ReadObjectAt50A620DwordAt1C() = 0;
    // 522D7E 为 [eax+54h]；伪代码的 +84 是十进制，不是 0x84。
    virtual std::uint8_t ReadRegistrySlot1ByteAt54() = 0;
    virtual std::shared_ptr<IModalDialogRuntimeTargetReference> CreateConfirmationAt4E4340() = 0;
    virtual void PrepareModalAt4AD230() = 0;
    virtual std::int32_t RunModalAt5071D0(IModalDialogRuntimeTargetReference& target) = 0;
    virtual void WriteCurrentSceneName(const char* name) = 0;
    virtual void PostCloseMessage() = 0;
};

// 状态核心的内部 Dialog 槽由生产 owner 提供，不能再转发给根服务占位。
class ITitleDialogServices : public ITitleDialogExternalServices
{
public:
    virtual void ClearClickedEventsAt532200() = 0;
    virtual void WriteDialogEnabledAt538660(std::int32_t value) = 0;
    virtual void ResetInputAt510840() = 0;
    virtual void ResetChildrenAt522E90() = 0;
    virtual void UpdateGuiDialogAt518C30() = 0;
    virtual std::int32_t PollClickedEventAt50FDB0(std::int32_t clear) = 0;
    virtual ICgDisplayButtonGroupDirtyRects& OwnDirtyRectsAtVslot00() = 0;
    virtual void DrawChildrenAt5230B0(const RECT& clip) = 0;
    virtual void CompositeChildrenAt5231C0(const LoadAfterTransitionSourceRef& target,
        const RECT* clip) = 0;
};

// TitleDialog vtable 5BFE38 的状态核心。资源/子控件构造由独立 owner 提供，
// 不把尚未初始化的 +5C/+D4 伪造为可运行状态。
class TitleDialogRuntime final : public ITitleDialogRuntime
{
public:
    TitleDialogRuntime(ITitleDialogServices& services,
        LoadAfterTransitionSourceRef blackAt78, LoadAfterTransitionSourceRef canvasAt84,
        const RECT& rectAt68) noexcept;
    void InvokeVslot44() override;
    bool Run() override;
    std::uint32_t SelectionResult() const override;
    void RedrawAllAt530E60();

    std::optional<std::int32_t> PhaseAt5C() const noexcept { return phase_; }
    std::int32_t TransitionPhaseAt60() const noexcept { return transitionPhase_; }
    const LoadAfterTransitionOperation& TransitionAt90() const noexcept { return transition_; }

private:
    void AdvanceTransitionAt51CC70();
    void ProcessSelectionAt522BD0();
    void DrawDirtyAt525FB0();

    ITitleDialogServices* services_;
    LoadAfterTransitionSourceRef black_;
    LoadAfterTransitionSourceRef canvas_;
    RECT rect_;
    std::optional<std::int32_t> phase_;
    std::optional<std::uint32_t> selection_;
    std::int32_t transitionPhase_ = 0;
    std::int32_t nextPhase_ = 0;
    std::int32_t step_ = 0;
    LoadAfterTransitionOperation transition_;
};
}
