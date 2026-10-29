#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/SceneManagerOptionCharSelectDialogRuntime.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ISceneManagerOptionCharAvailabilitySample18
{
public:
    virtual ~ISceneManagerOptionCharAvailabilitySample18() = default;
    [[nodiscard]] virtual bool IsAvailableAt4E4C68(
        std::int32_t characterIndex) = 0;
};

class ISceneManagerOptionCharInheritedServices
{
public:
    virtual ~ISceneManagerOptionCharInheritedServices() = default;

    virtual void ResetDialogBaseAt510750() = 0;
    virtual void WriteDialogEnabledAt538660(std::int32_t value) = 0;
    [[nodiscard]] virtual bool ReadInputHeldAtVslot6C(
        std::int32_t eventValue) = 0;
    [[nodiscard]] virtual bool ReadInputPressedAtVslot70(
        std::int32_t eventValue) = 0;
    [[nodiscard]] virtual POINT ReadPointerAtVslot74() = 0;
    [[nodiscard]] virtual bool ReadCancelAtVslot7C() = 0;
    [[nodiscard]] virtual bool ReadPointerPressAtVslot80() = 0;
    [[nodiscard]] virtual bool ReadPointerReleaseAtVslot84() = 0;
    [[nodiscard]] virtual std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) = 0;
    [[nodiscard]] virtual ICgDisplayButtonGroupDirtyRects&
        OwnDirtyRectsAtVslot00() = 0;
    [[nodiscard]] virtual GraphicsSurfaceHandle ResolveTransitionTargetHandle(
        const CgDisplayTransitionSourceRef& target) const = 0;
};

// 0x4E4A20 的完整样本 18 owner：五个 Button、各自 SurfaceGroup、
// ClickedEvent manager 与选择状态核心共处同一强所有权边界。
class SceneManagerOptionCharSelectDialogObjectGraphRuntime final
    : public ISceneManagerOptionCharSelectDialog,
      public ICgDisplayButtonGroupSelector,
      private ISceneManagerOptionCharSelectDialogServices,
      private ICgOperationButtonInputServices
{
public:
    SceneManagerOptionCharSelectDialogObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        IGraphicsSurfaceCompositionApi& graphics,
        ISceneManagerOptionCharAvailabilitySample18& availability,
        ISceneManagerOptionCharInheritedServices& inheritedServices);

    void InitializeAt50D930() override;
    [[nodiscard]] std::int32_t ProcessAtVslot48() override;
    [[nodiscard]] std::int32_t SelectionAtVslot50() const override;
    void AttachDirtyRectsAtVslot94(
        ICgDisplayButtonGroupDirtyRects& dirtyRects) override;
    void ResetAtVslot90() override;
    void DrawAtVslot60(const RECT& clip) override;
    void CompositeAtVslot5C(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) override;

    [[nodiscard]] const std::vector<std::shared_ptr<
        CgOperationChildControlRuntime>>& Buttons() const noexcept;
    [[nodiscard]] const std::shared_ptr<
        CgOperationClickedEventManagerRuntime>& ClickedEvents() const noexcept;
    [[nodiscard]] std::int32_t PagingInputLatchAt6C() const noexcept;
    [[nodiscard]] const CgOperationChildControlRef&
        ActiveControlAt4C() const noexcept;
    [[nodiscard]] std::int32_t StateAt68() const noexcept;
    [[nodiscard]] std::int32_t FocusedChildAt84() const noexcept;

private:
    void ResetDialogBaseAt510750() override;
    void WriteDialogEnabledAtVslot24(std::int32_t value) override;
    void ProcessGenericDialogAtVslot54() override;
    void ClearClickedEventsAt532200() override;
    [[nodiscard]] std::int32_t PollClickedEventAt50FDB0(
        std::int32_t clearMatchedEvent) override;
    [[nodiscard]] bool HasChildAtVslot14(
        std::int32_t childId) const override;
    void ResetChildAtVslot08(std::int32_t childId) override;
    void WriteChildEnabledAtVslot4C(
        std::int32_t childId,
        std::int32_t value) override;
    void WriteChildPositionAt34And38(
        std::int32_t childId,
        const POINT& position) override;
    void WriteChildStateAtVslot44(
        std::int32_t childId,
        std::int32_t value) override;
    [[nodiscard]] ICgDisplayButtonGroupDirtyRects&
        OwnDirtyRectsAtVslot00() override;
    void DrawChildrenAt5230B0(const RECT& clip) override;
    void CompositeChildrenAt5231C0(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) override;

    [[nodiscard]] CgOperationChildControlRuntime& ChildForId(
        std::int32_t childId) const;
    [[nodiscard]] CgOperationChildControlRef ResolveChildAt509D20(
        std::int32_t eventValue) const;
    void SubmitActiveControlAt532170(CgOperationChildControlRef control);
    [[nodiscard]] std::int32_t ProcessUpperInputAt51F8E0();
    [[nodiscard]] std::int32_t ProcessBaseUpperInputAt51E740();
    [[nodiscard]] bool ProcessPagingReleaseAt510EB0();

    void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) override;
    [[nodiscard]] bool ReadInputAtVslot6C(
        std::int32_t eventValue) override;
    [[nodiscard]] bool ReadInputAtVslot70(
        std::int32_t eventValue) override;
    [[nodiscard]] POINT ReadPointerAtVslot74() override;
    [[nodiscard]] bool ReadPressAtVslot80() override;
    [[nodiscard]] bool ReadReleaseAtVslot84() override;
    [[nodiscard]] std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) override;

    ISceneManagerOptionCharInheritedServices* inheritedServices_ = nullptr;
    CgOperationDialogRenderHostRuntime renderHost_;
    std::vector<std::shared_ptr<CgOperationChildControlRuntime>> buttons_;
    std::vector<CgOperationChildControlRef> childRefs_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> clickedEvents_;
    std::shared_ptr<SceneManagerOptionCharSelectDialogRuntime> dialog_;
    CgOperationChildControlRef activeControlAt4C_;
    std::int32_t pagingInputLatchAt6C_ = 0;
};
}
