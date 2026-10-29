#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>
#include <Runtime/ProgramSelectionDialogRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IProgramSelectionMainValueSource
{
public:
    virtual ~IProgramSelectionMainValueSource() = default;

    [[nodiscard]] virtual std::int32_t ReadNamedValueAt4C73A0(
        std::string_view nameCp932) const = 0;
    [[nodiscard]] virtual std::int32_t WriteNamedValueAt4C8220(
        std::string_view nameCp932,
        std::int32_t value) = 0;
};

class IProgramSelectionMainDirtyRectSink
{
public:
    virtual ~IProgramSelectionMainDirtyRectSink() = default;
    virtual void SubmitRectAtVslot64(const RECT& rect) = 0;
};

enum class ProgramSelectionMainSoundAt4BCD70 : std::uint8_t
{
    SelectOrExitAt61A658,
    TrainingConfirmAt61A65C,
    WeightDecreaseAt61A670,
    StressIncreaseAt61A674,
};

// CharaSelect00/01Dialog01 的继承虚槽。这里仅抽出对象图外部持有的
// Dialog/导航状态；child 重置和事件轮询仍由本对象按原顺序执行。
class IProgramSelectionMainDialogServices
{
public:
    virtual ~IProgramSelectionMainDialogServices() = default;

    virtual void WriteDialogEnabledAtVslot24(std::int32_t value) = 0;
    virtual void PrepareNavigationAtVslot68() = 0;
    virtual void UpdateInheritedAtVslot54() = 0;
    virtual void SubmitPrimaryBoundsAtVslot64() = 0;
    virtual void DisableCapturedPrimaryLayerAt52CF2E() = 0;
    [[nodiscard]] virtual bool
        ShouldForceAnimationCompletionAt51BB50() = 0;
    virtual void EmitMappedSoundAt4BCD70(
        ProgramSelectionMainSoundAt4BCD70 sound) = 0;
};

struct DietTrainingPartsState final
{
    std::int32_t characterIndex = 0;
    std::int32_t stressLevel = 0;
    std::int32_t trainingLevel = 0;
    std::int32_t weight = 0;
    std::int32_t remainingCount = 0;
    bool showTrainingEffect = false;
    bool showStatusIncrease = false;
    bool showStatusDecrease = false;
    bool submitDirtyRect = false;
};

// CharaSelect00Dialog01::parts(4F55A0/5279C0)。该对象持有三张角色状态画布，
// 以及构成状态画布所需的 24 张样本 18 固定资源。
class DietTrainingPartsRuntime final
{
public:
    DietTrainingPartsRuntime(
        ICgOperationSurfaceFactory& factory,
        IGraphicsSurfaceCompositionApi& graphics,
        IProgramSelectionMainDirtyRectSink* dirtyRectSink = nullptr);

    void ComposeAt5279C0(const DietTrainingPartsState& state);

    [[nodiscard]] const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
        OutputSurfacesAt00() const noexcept;
    [[nodiscard]] const RECT& CombinedRectAt18() const noexcept;

private:
    [[nodiscard]] const RuntimeGraphicsSurface& OverlayAt(
        std::size_t index) const;
    void BlitAtResourceOrigin(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source);
    void BlitAtAbsolutePosition(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source,
        std::int32_t x,
        std::int32_t y);

    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    IProgramSelectionMainDirtyRectSink* dirtyRectSink_ = nullptr;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> outputSurfacesAt00_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> stressResourcesAt0C_;
    RECT stressRectAt18_{};
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> overlayResourcesAt28_;
    RECT combinedRectAt34_{};
    std::shared_ptr<RuntimeGraphicsSurface> trainingEffectAt44_;
    std::shared_ptr<RuntimeGraphicsSurface> statusIncreaseAt50_;
    std::shared_ptr<RuntimeGraphicsSurface> statusDecreaseAt5C_;
    std::shared_ptr<RuntimeGraphicsSurface> trainingCountBaseAt68_;
};

[[nodiscard]] std::span<const CgOperationChildDescriptor>
ProgramSelectionMainDescriptorsSample18(ProgramSelectionDialogMode mode);

// 4DCEB0/4DE520 的 Main 子树。样本 18 的固定表只在本对象内使用，
// 不提升为其他 params 系游戏的通用布局规则。
class ProgramSelectionMainDialogObjectGraphRuntime final
{
public:
    ProgramSelectionMainDialogObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ICgOperationButtonInputServices& buttonInputServices,
        IProgramSelectionMainValueSource& values,
        IProgramSelectionMainDialogServices& services,
        ProgramSelectionDialogMode mode,
        IProgramSelectionMainDirtyRectSink* dirtyRectSink = nullptr);

    void OpenAtVslot44();
    [[nodiscard]] std::int32_t UpdateAtVslot48();
    [[nodiscard]] std::int32_t CloseAtVslot4C();
    void RefreshMode00At5009C0(std::int32_t interactive);

    [[nodiscard]] ProgramSelectionDialogMode Mode() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
        ClickedEventsAt5C() const noexcept;
    [[nodiscard]] const std::vector<GuiDialogNavigationGroupRef>&
        NavigationGroupsAt24() const noexcept;
    [[nodiscard]] const RECT& CombinedRect() const noexcept;
    [[nodiscard]] const std::shared_ptr<DietTrainingPartsRuntime>&
        DietPartsAt74() const noexcept;
    [[nodiscard]] std::int32_t ResultAt80() const noexcept;
    [[nodiscard]] std::int32_t ResultAt90() const noexcept;
    [[nodiscard]] std::int32_t SelectedCharacterAt8C() const noexcept;
    void WriteSelectedTrainingAtBC(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t ResolveTrainingRecordAt50B470(
        std::int32_t trainingChoice) const;
    [[nodiscard]] std::int32_t CalculateTrainingAt50BCC0();
    [[nodiscard]] std::int32_t WeightBeforeAtA0() const noexcept;
    [[nodiscard]] std::int32_t WeightAfterAtA4() const noexcept;
    [[nodiscard]] std::int32_t StressBeforeAtA8() const noexcept;
    [[nodiscard]] std::int32_t StressAfterAtAC() const noexcept;
    [[nodiscard]] std::int32_t TrainingLevelAtB0() const noexcept;
    [[nodiscard]] std::int32_t SelectedTrainingAtBC() const noexcept;
    [[nodiscard]] std::int32_t PhaseAt98() const noexcept;
    [[nodiscard]] std::int32_t AnimationStateAt9C() const noexcept;
    [[nodiscard]] std::int32_t AnimationDelayAtB4() const noexcept;
    [[nodiscard]] std::int32_t AnimationCounterAtB8() const noexcept;

private:
    void AddChild(const CgOperationChildControlRef& child);
    void ApplyMode01AvailabilityAt501880();
    [[nodiscard]] CgOperationChildControlRef FindChildByEventValue(
        std::int32_t eventValue) const noexcept;
    [[nodiscard]] CgOperationChildControlRef ChildByEventValue(
        std::int32_t eventValue) const;
    void ComposeMode00At5279C0(
        std::int32_t stress,
        std::int32_t weight,
        bool showTrainingEffect,
        bool showStatusIncrease,
        bool showStatusDecrease,
        bool submitDirtyRect);
    void AdvanceMode00AnimationAt51BB50();

    ProgramSelectionDialogMode mode_ = ProgramSelectionDialogMode::Mode00;
    IProgramSelectionMainValueSource* values_ = nullptr;
    IProgramSelectionMainDialogServices* services_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> clickedEventsAt5C_;
    std::vector<GuiDialogNavigationGroupRef> navigationGroupsAt24_;
    RECT combinedRect_{};
    std::shared_ptr<DietTrainingPartsRuntime> dietPartsAt74_;
    std::int32_t resultAt80_ = -1;
    std::int32_t selectedCharacterAt8C_ = 0;
    std::int32_t resultAt90_ = -1;
    std::int32_t phaseAt98_ = 0;
    std::int32_t animationStateAt9C_ = 0;
    std::int32_t weightBeforeAtA0_ = 0;
    std::int32_t weightAfterAtA4_ = 0;
    std::int32_t stressBeforeAtA8_ = 0;
    std::int32_t stressAfterAtAC_ = 0;
    std::int32_t trainingLevelAtB0_ = 0;
    std::int32_t animationDelayAtB4_ = 0;
    std::int32_t animationCounterAtB8_ = 0;
    std::int32_t selectedTrainingAtBC_ = 0;
};
}
