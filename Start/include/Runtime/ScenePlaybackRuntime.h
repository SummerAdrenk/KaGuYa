#pragma once

#include <Runtime/LoadAfterSelectionScene.h>
#include <Runtime/ScenePlaybackScriptServices.h>
#include <Windows.h>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IScenePlaybackButton
{
public:
    virtual ~IScenePlaybackButton() = default;
    virtual std::int32_t ProcessSelectionAtVslot04() = 0;
    virtual std::int32_t ItemCountAt3C() const = 0;
    virtual void RenderDirtyRegionsAtVslot34() = 0;
    virtual void CompositeIntoAtVslot38(
        const LoadAfterTransitionSourceRef& target, const RECT* clip) = 0;
    virtual void ResetGroupAt4C1260(std::int32_t invalidate) = 0;
};

class IScenePlaybackButtonFactory
{
public:
    virtual ~IScenePlaybackButtonFactory() = default;
    virtual std::shared_ptr<IScenePlaybackButton> CreateButtonAt49F270(std::int32_t group) = 0;
};

class IScenePlaybackHostServices
{
public:
    virtual ~IScenePlaybackHostServices() = default;
    virtual LoadAfterTransitionSourceRef LoadTransitionSource(const char* name) = 0;
    virtual LoadAfterTransitionSourceRef CreateAuxiliarySourceAt40C2E0() = 0;
    virtual bool InitializeAuxiliarySourceAt415010(
        const LoadAfterTransitionSourceRef& source) = 0;
    virtual void InvokeAt4703D0(std::int32_t mode) = 0;
    virtual void QueueTitleAfterConstructionFailureAt47E180() = 0;
    virtual std::uint32_t ReadRegistrySlot2DwordAt04() const = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2DwordAt14(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt24(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt25(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt26(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt27(std::uint8_t value) = 0;
    virtual void WriteRootByteAt218(std::uint8_t value) = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;
    virtual std::int32_t ReadSceneControllerDwordAt40() const = 0;
    virtual void WriteSceneControllerDwordAt40(std::int32_t value) = 0;
    virtual bool ShouldAccelerateTransitionAt4AA970() = 0;
    virtual void RenderTransition(LoadAfterTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrameAt4B02E0(std::int32_t argument) = 0;
    virtual void InitializeEntrySubmissionAt4BCCC0() = 0;
    virtual std::string_view ReadCurrentSceneName() const = 0;
    virtual std::string_view ReadReturnFallbackSceneNameAt4ADAC0() const = 0;
    virtual bool ReadTitleReturnGateAt4B0740() const = 0;
    virtual void ClearCurrentSceneName() = 0;
    virtual void PushActiveSceneAt4A9030(const char* name) = 0;
    virtual void QueuePendingSceneAt47F920(const char* name) = 0;
    virtual std::int32_t ReadExitRouteCountAt47B7B0() const = 0;
    virtual LoadAfterTransitionSourceRef CapturePrimarySceneResourceAt4ACDD0() = 0;
    virtual std::int32_t ReadTransitionSourceValueAt04(
        const LoadAfterTransitionSourceRef& source) const = 0;
    virtual void StopSoundManagerEntryNamedAt10() = 0;
    virtual void ClearSoundManagerEntriesAt4384D0() = 0;
    virtual void StopSoundEntryNamedByRegistrySlot2At30() = 0;
    // 三个调用不能合并；失败时保留前面调用已经完成的副作用。
    virtual void InvokeGroup0At4C4330() = 0;
    virtual void InvokeGroup0At4C4250() = 0;
    virtual void InvokeRootAt4AC490() = 0;
    virtual void InvokeAt4AA890(std::int32_t argument) = 0;
    virtual void WriteRootServiceDwordAt10(std::uint32_t value) = 0;
    virtual void ShowReplayError(const char* messageCp932) = 0;
};

// 兼容已有集中宿主；生产构造也可分别传入窗口、按钮、脚本三个有明确归属的对象。
class IScenePlaybackServices : public IScenePlaybackHostServices,
    public IScenePlaybackButtonFactory, public IScenePlaybackScriptServices
{
public:
    ~IScenePlaybackServices() override = default;
};

// 49EC90 / 4B7CE0 / 4A6C60。Draw 虚槽 4B5E20 为空；Run 不额外提交画面。
class ScenePlaybackRuntime final : public ISceneTask
{
public:
    explicit ScenePlaybackRuntime(IScenePlaybackServices& services);
    // 两个依赖均借用根 owner；脚本适配器必须比场景活得更久。
    ScenePlaybackRuntime(IScenePlaybackServices& services,
        IScenePlaybackScriptServices& scriptServices);
    ScenePlaybackRuntime(IScenePlaybackHostServices& services,
        IScenePlaybackButtonFactory& buttons, IScenePlaybackScriptServices& scriptServices);
    ~ScenePlaybackRuntime() override;
    void Run(std::int32_t argument) override;
    std::optional<std::uint32_t> Phase() const noexcept { return phase_; }
    std::int32_t CurrentGroup() const noexcept { return group_; }
    std::int32_t CurrentSegment() const noexcept { return segment_; }
    const LoadAfterTransitionOperation& Transition() const noexcept { return operation_; }

private:
    IScenePlaybackButton& CurrentButton();
    void AdvanceTransition();
    void BeginListTransition();
    void PollList();
    void BeginCapturedBlackTransition();
    void PrepareReplay();
    bool ResetOrContinueReplay();

    IScenePlaybackHostServices* services_;
    IScenePlaybackScriptServices* scriptServices_;
    std::optional<std::uint32_t> phase_;
    std::uint32_t savedRegistryValue_ = 0;
    std::int32_t group_ = 0;
    std::int32_t exitRoute_ = 0;
    std::int32_t selectedItem_ = 0;
    std::int32_t segment_ = 0;
    std::vector<std::shared_ptr<IScenePlaybackButton>> buttons_;
    LoadAfterTransitionOperation operation_;
    std::int32_t transitionStep_ = 0;
    LoadAfterTransitionSourceRef black_;
    LoadAfterTransitionSourceRef captured_;
    LoadAfterTransitionSourceRef auxiliary_;
};
}
