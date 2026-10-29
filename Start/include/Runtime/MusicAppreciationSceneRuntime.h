#pragma once

#include <Runtime/LoadAfterSelectionScene.h>
#include <Runtime/SceneControllerRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
using MusicAppreciationTransitionSourceRef = LoadAfterTransitionSourceRef;
using MusicAppreciationTransitionOperation = LoadAfterTransitionOperation;

enum class MusicAppreciationPhase : std::uint32_t
{
    BeginEntryTransition = 0,
    AnimateEntryTransition = 1,
    EnableInteraction = 2,
    PollInteraction = 3,
    BeginExitTransition = 4,
    AnimateExitTransition = 5,
    QueueExitScene = 6,
    BeginMovieTransition = 7,
    AnimateMovieTransition = 8,
    WaitForExternalMovieCompletion = 9,
    ResetAfterMovie = 10,
};

class IMusicAppreciationButton
{
public:
    virtual ~IMusicAppreciationButton() = default;

    // CSoundBtn 虚表 +0x04/+0x08 以及对象 +0x3C/+0x68/+0x6C。
    [[nodiscard]] virtual std::int32_t ProcessSelectionAtVslot04() = 0;
    // 0x4B4570 以 retn 0x10 返回：四个 LONG 位于调用栈上，RECT 是按值参数。
    virtual void InvalidateAtVslot08(RECT rect) = 0;
    [[nodiscard]] virtual std::int32_t ItemCountAt3C() const = 0;
    [[nodiscard]] virtual std::int32_t SelectedItemAt68() const = 0;
    virtual void WriteSelectedItemAt68(std::int32_t value) = 0;
    [[nodiscard]] virtual std::uint8_t MovieSelectionPendingAt6C() const = 0;
    virtual void WriteMovieSelectionPendingAt6C(std::uint8_t value) = 0;
    virtual void ClearSelectedItemStateAt04And20(std::int32_t index) = 0;
};

using MusicAppreciationButtonRef =
    std::shared_ptr<IMusicAppreciationButton>;

class IMusicAppreciationSceneServices
{
public:
    virtual ~IMusicAppreciationSceneServices() = default;

    [[nodiscard]] virtual MusicAppreciationTransitionSourceRef
        LoadTransitionSource(const char* resourceNameCp932) = 0;
    virtual void PrepareSceneControllerAt4703D0(std::int32_t mode) = 0;
    [[nodiscard]] virtual MusicAppreciationButtonRef
        CreateMusicAppreciationButton() = 0;
    [[nodiscard]] virtual MusicAppreciationTransitionSourceRef
        CapturePrimarySceneResourceAt4ACDD0() = 0;

    virtual void StopMusicEntryAt4AA890(std::int32_t index) = 0;
    virtual void ResetMusicEntriesAt4AA780() = 0;
    virtual void ClearCurrentSceneName() = 0;
    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt24(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt25(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt26(std::uint8_t value) = 0;
    virtual void ClearRegistrySlot2StringAt2C() = 0;
    virtual void InvokeSoundProcedureAt4383B0() = 0;
    virtual void WriteRootByteAt218(std::uint8_t value) = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;

    [[nodiscard]] virtual bool ShouldAccelerateTransitionAt4AA970() = 0;
    virtual void RenderTransition(
        MusicAppreciationTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrameAt4B02E0(std::int32_t argument) = 0;

    [[nodiscard]] virtual std::string_view ReadCurrentSceneName() const = 0;
    [[nodiscard]] virtual std::string_view ReadReturnSourceSceneName() const = 0;
    [[nodiscard]] virtual std::string_view ReadReturnFallbackSceneName() const = 0;
    [[nodiscard]] virtual bool ReadTitleReturnGateAt4B0740() const = 0;
    virtual void PushActiveSceneAt4A9030(const char* sceneNameCp932) = 0;
    virtual void QueuePendingSceneAt47F920(const char* sceneNameCp932) = 0;
    virtual void SynchronizeButtonAt4ABB40(
        const MusicAppreciationButtonRef& button) = 0;
    virtual void ReadGraphicsTargetSize(
        std::int32_t& width,
        std::int32_t& height) const = 0;
};

// ModeSoundScene::CModeSound（0x4A0730/0x4B8C90）的 0..10 阶段状态机。
// bg_black.bgd 装载失败时，原构造函数返回一个 phase 尚未写入的部分对象；这里用
// 空 optional 精确表达该事实，Run 会显式报错，不制造可继续运行的默认 phase。
class MusicAppreciationSceneRuntime final : public ISceneTask
{
public:
    explicit MusicAppreciationSceneRuntime(
        IMusicAppreciationSceneServices& services);

    void Run(std::int32_t argument) override;

    [[nodiscard]] bool IsOperational() const noexcept;
    [[nodiscard]] std::optional<MusicAppreciationPhase> Phase() const noexcept;
    [[nodiscard]] std::int32_t ExitRoute() const noexcept;
    [[nodiscard]] const MusicAppreciationTransitionOperation& Transition() const noexcept;
    [[nodiscard]] const MusicAppreciationButtonRef& Button() const noexcept;

    // phase 9 在 0x4B8C90 中是严格空分支。生产回调只有在外部播放链确认完成后
    // 才能把原对象 +0x04 写为 10；本入口只表达该已存在的外部写边界。
    void EnterResetAfterExternalMovieCompletion();

private:
    void Update();
    void InitializeTransition(
        std::int32_t selector,
        const MusicAppreciationTransitionSourceRef& source);
    void AdvanceTransition();
    void PollInteraction();
    void QueueExitScene();
    void ResetAfterMovie();
    [[nodiscard]] std::optional<std::int32_t> ResolveReturnRoute() const;

    IMusicAppreciationSceneServices* services_ = nullptr;
    std::optional<MusicAppreciationPhase> phase_;
    MusicAppreciationButtonRef button_;
    MusicAppreciationTransitionOperation transition_;
    std::int32_t transitionStep_ = 0;
    MusicAppreciationTransitionSourceRef blackTransitionSource_;
    MusicAppreciationTransitionSourceRef primarySceneResource_;
    std::int32_t exitRoute_ = 0;
};
}
