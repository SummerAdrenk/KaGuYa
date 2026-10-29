#pragma once

#include <Runtime/LoadAfterSelectionScene.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/SceneControllerRuntime.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace KaGuYa::Start::Runtime
{
using CgDisplayTransitionSourceRef = LoadAfterTransitionSourceRef;
using CgDisplayTransitionOperation = LoadAfterTransitionOperation;

enum class CgDisplayPhase : std::uint32_t
{
    BeginEntryTransition = 0,
    AnimateEntryTransition = 1,
    EnableListInteraction = 2,
    PollListInteraction = 3,
    BeginExitTransition = 4,
    AnimateExitTransition = 5,
    QueueExitScene = 6,
    BeginSelectedCgTransition = 7,
    AnimateSelectedCgTransition = 8,
    PrepareSelectedCg = 9,
    AnimateCgPresentationTransition = 10,
    EnableCgOperation = 11,
    PollCgOperation = 12,
    WaitForCgOperationClose = 13,
    BeginCgReturnTransition = 14,
    AnimateCgReturnTransition = 15,
    ResetToList = 16,
};

// CModeCGdisp+0x18..+0x34。字段业务用途尚未全部由消费者证明，
// 因而除坐标和映射索引外保留原 offset 名，不擅自赋予业务名称。
struct CgDisplaySelectionState final
{
    std::int32_t countAt18 = 0;
    std::int32_t valueAt1C = 0;
    std::int32_t xAt20 = 0;
    std::int32_t yAt24 = 0;
    std::int32_t selectedMappingIndexAt28 = 0;
    std::vector<std::pair<std::int32_t, std::int32_t>> mappingPairsAt2C;
    std::vector<bool> availability;

    void Clear();
};

class ICgDisplayButton
{
public:
    virtual ~ICgDisplayButton() = default;

    [[nodiscard]] virtual std::int32_t ProcessSelectionAtVslot04() = 0;
    [[nodiscard]] virtual std::int32_t ItemCountAt3C() const = 0;
    // CCGdispBtn vslot +0x34 (0x4ABE80) 会渲染当前页及其脏区，随后提交 InvalidatedRect。
    virtual void RenderDirtyRegionsAtVslot34() = 0;
    // CCGdispBtn vslot +0x38 (0x4A80B0) 会立即把当前页和控件合成到目标对象。
    virtual void CompositeIntoAtVslot38(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) = 0;
    // 4B6ECC/4B73DE：0x4C1260 的 this 是当前 CCGdispBtn，而不是 ModeCGdisp 根对象。
    virtual void ResetGroupAt4C1260(std::int32_t invalidateFullScreen) = 0;
};

using CgDisplayButtonRef = std::shared_ptr<ICgDisplayButton>;

class ICgDisplayWindow
{
public:
    virtual ~ICgDisplayWindow() = default;

    [[nodiscard]] virtual std::size_t DrawAtVslot00(
        std::int32_t argument) = 0;
    [[nodiscard]] virtual std::size_t BlitAtVslot04(
        GraphicsSurfaceDestination destination,
        std::int32_t argument) = 0;
    [[nodiscard]] virtual RECT* CopyClipRectAtVslot0C(RECT* output) const = 0;
    [[nodiscard]] virtual bool QueryAtVslot10(
        std::uint32_t first,
        std::uint32_t second) const noexcept = 0;
    virtual void DeactivateAtVslot18() noexcept = 0;
    virtual void WritePositionAt04And08(
        std::int32_t x,
        std::int32_t y) = 0;
    [[nodiscard]] virtual std::uint8_t ActiveByteAt54() const = 0;
    virtual void BeginAt4AA860() = 0;
    virtual void UpdateAt4BD050() = 0;
    // 4B71A1/4B7995：这两个调用的 this 均为 CCGDispWin。
    virtual void InvokeAt4C17F0(std::int32_t value) = 0;
    virtual void InvokeAt4C13A0(std::int32_t value) = 0;
    // CCGDispWin 虚表 +0x14（0x4A7470）只推进动画并提交 old/new dirty rect。
    virtual void UpdateAnimationAndDirtyRectsAtVslot14() = 0;
};

using CgDisplayWindowRef = std::shared_ptr<ICgDisplayWindow>;

class ICgOperationWindow
{
public:
    virtual ~ICgOperationWindow() = default;

    [[nodiscard]] virtual std::int32_t PollCommandAt4BD590() = 0;
    // 0x4A5240 解析 CGOpeWin；0x502110 作用在其唯一 Dialog，
    // 0x4C22B0 则直接作用在 CGOpeWin 本体。
    virtual void BeginSelectionAt502110() = 0;
    virtual void AppendAvailabilityAt4FEA00(bool available) = 0;
    virtual void SubmitWindowVisibilityAt4C22B0(std::uint8_t value) = 0;
    [[nodiscard]] virtual std::int32_t DialogSelectionValueAt7C() const = 0;
    // 0x4C2690 命中时会调用关联对象的虚槽 +0x08，不能声明为 const 查询。
    [[nodiscard]] virtual bool PollDialogCompletionAt4C2690() = 0;
    // 0x4A8E40 只写 CGOpeWin+0x0C，不经过 0x4C22B0 的 Layer 提交。
    virtual void SetActiveAt0CWithoutLayerSubmission(bool active) noexcept = 0;
    [[nodiscard]] virtual bool ActiveAt0C() const noexcept = 0;
    // 0x4B7C24：写 CGOpeWin+0x0C，读取 CGOpeWin 的矩形，再提交给
    // 它在 +0x10 保存的父 Layer。
    virtual void ReactivateWindowAndLayerAt4B7C24() = 0;
};

using CgOperationWindowRef = std::shared_ptr<ICgOperationWindow>;

class ICgDisplaySceneServices
{
public:
    virtual ~ICgDisplaySceneServices() = default;

    virtual void ClearCurrentSceneName() = 0;
    [[nodiscard]] virtual CgDisplayTransitionSourceRef LoadTransitionSource(
        const char* resourceNameCp932) = 0;
    [[nodiscard]] virtual CgDisplayTransitionSourceRef
        CreateAuxiliaryListSourceAt40C2E0() = 0;
    [[nodiscard]] virtual bool InitializeAuxiliaryListSourceAt415010(
        const CgDisplayTransitionSourceRef& source) = 0;
    virtual void InvokeAt4703D0WithNull() = 0;
    [[nodiscard]] virtual CgDisplayButtonRef CreateCgDisplayButton(
        std::int32_t index) = 0;
    [[nodiscard]] virtual CgDisplayWindowRef CreateCgDisplayWindow() = 0;
    [[nodiscard]] virtual CgOperationWindowRef CreateCgOperationWindow() = 0;
    virtual void QueueTitleAfterConstructionFailureAt47E180() = 0;

    virtual void WriteRegistrySlot2DwordAt04(std::uint32_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt09(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt24(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt25(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt26(std::uint8_t value) = 0;
    virtual void WriteRegistrySlot2ByteAt27(std::uint8_t value) = 0;
    virtual void WriteRootByteAt218(std::uint8_t value) = 0;
    virtual void WriteRootByteAt219(std::uint8_t value) = 0;

    [[nodiscard]] virtual bool ShouldAccelerateTransitionAt4AA970() = 0;
    virtual void RenderTransition(CgDisplayTransitionOperation& operation) = 0;
    virtual void CommitTransitionFrameAt4B02E0(std::int32_t argument) = 0;
    virtual void InitializeEntrySubmissionAt4BCCC0() = 0;

    [[nodiscard]] virtual std::string_view ReadCurrentSceneName() const = 0;
    [[nodiscard]] virtual std::string_view ReadReturnSourceSceneName() const = 0;
    [[nodiscard]] virtual std::string_view ReadReturnFallbackSceneName() const = 0;
    [[nodiscard]] virtual bool ReadTitleReturnGateAt4B0740() const = 0;
    virtual void PushActiveSceneAt4A9030(const char* sceneNameCp932) = 0;
    virtual void QueuePendingSceneAt47F920(const char* sceneNameCp932) = 0;
    [[nodiscard]] virtual std::int32_t ReadExitRouteCountAt47B7B0() const = 0;

    [[nodiscard]] virtual bool BuildCgSelectionAt4AD780(
        std::int32_t buttonIndex,
        std::int32_t itemIndex,
        CgDisplaySelectionState& state) = 0;
    virtual void FinalizeCgSelectionAt47E940() = 0;
    virtual void WriteCgSelectionOwnerByteAt0C(std::uint8_t value) = 0;
    [[nodiscard]] virtual bool PrepareSelectedCgAt4A84E0(
        const CgDisplaySelectionState& state,
        const CgDisplayWindowRef& displayWindow) = 0;

    [[nodiscard]] virtual CgDisplayTransitionSourceRef
        CapturePrimarySceneResourceAt4ACDD0() = 0;
    [[nodiscard]] virtual std::int32_t ReadTransitionSourceValueAt04(
        const CgDisplayTransitionSourceRef& source) const = 0;
    virtual void SubmitCgDisplayWindowRectToParentDirtyRects(
        const CgDisplayWindowRef& displayWindow) = 0;
    virtual void CompositeAuxiliaryListSource(
        const CgDisplayTransitionSourceRef& source) = 0;
    // 4AA780 动态解析 Graphics.dll!ClearDirtyRect，不属于声音系统。
    virtual void ClearDirtyRectsAt4AA780() = 0;

    virtual void InvokeCgOperationCommand4At4BCD70() = 0;
    [[nodiscard]] virtual bool ReadCgCloseInputAt47F4E0(
        std::int32_t value) const = 0;

    virtual void InvokeCgOwnerVslot08() = 0;
    [[nodiscard]] virtual bool ReadRegistrySlot1ByteAt6D() const = 0;
    virtual void DrawCgGlobalLayerAtVslot18() = 0;
};

// ModeCGdispScene::CModeCGdisp（0x49E3B0/0x4B6A60/0x4B5970）的
// 0..16 阶段生产外壳。具体 GUI 和 0x4A84E0 资源构建链由强类型 services
// 提供；本对象保留原场景自己拥有的状态、映射表与非回滚顺序。
class CgDisplaySceneRuntime final : public ISceneTask
{
public:
    explicit CgDisplaySceneRuntime(ICgDisplaySceneServices& services);
    ~CgDisplaySceneRuntime() override;

    void Run(std::int32_t argument) override;

    [[nodiscard]] bool IsOperational() const noexcept;
    [[nodiscard]] std::optional<CgDisplayPhase> Phase() const noexcept;
    [[nodiscard]] std::int32_t CurrentButtonIndex() const noexcept;
    [[nodiscard]] std::int32_t ExitRoute() const noexcept;
    [[nodiscard]] bool DisplayEnabled() const noexcept;
    [[nodiscard]] const CgDisplaySelectionState& SelectionState() const noexcept;
    [[nodiscard]] const CgDisplayTransitionOperation& Transition() const noexcept;

private:
    [[nodiscard]] CgDisplayButtonRef& CurrentButton();
    [[nodiscard]] const CgDisplayButtonRef& CurrentButton() const;
    void Update();
    void Draw();
    void InitializeTransition(
        std::int32_t selector,
        const CgDisplayTransitionSourceRef& source);
    void AdvanceTransition();
    void PollListInteraction();
    void BeginCapturedBlackTransition();
    void PrepareSelectedCg();
    void PollCgOperation();
    void WaitForCgOperationClose();
    void ResetToList();
    void QueueExitScene();
    [[nodiscard]] std::optional<std::int32_t> ResolveReturnRoute() const;

    ICgDisplaySceneServices* services_ = nullptr;
    std::optional<CgDisplayPhase> phase_;
    bool displayEnabled_ = false;
    std::vector<CgDisplayButtonRef> buttons_;
    CgDisplayWindowRef displayWindow_;
    CgOperationWindowRef operationWindow_;
    std::int32_t currentButtonIndex_ = 0;
    std::int32_t exitRoute_ = 0;
    CgDisplaySelectionState selection_;
    CgDisplayTransitionOperation transition_;
    std::int32_t transitionStep_ = 0;
    CgDisplayTransitionSourceRef blackTransitionSource_;
    CgDisplayTransitionSourceRef capturedSceneSource_;
    CgDisplayTransitionSourceRef auxiliaryListSource_;
};
}
