#pragma once
#include <Platform/InputViews.h>

#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/GuiModalDialogState.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <span>

namespace KaGuYa::Start::Platform { class IInputRuntime; }

namespace KaGuYa::Start::Runtime
{
class ICgOperationSurfaceFactory;

// 605CD8 原表是五行：退出从 0 开始，不能把 mode1/2 改称退出/标题。
enum class GeneralConfirmationMode : std::int32_t
{
    Exit = 0,
    ReturnTitle = 1,
    ResetSettings = 2,
    QuickSave = 3,
    QuickLoad = 4,
};

using GeneralConfirmationDialogDependencies = GuiModalDialogDependencies;

// 4E3330 / vtable 5BE28C，独立于 4EC8A0 等保存页确认框。
class GeneralConfirmationDialogRuntime final
    : public IModalDialogRuntimeTarget,
      public IModalDialogRuntimeTargetReference
{
public:
    GeneralConfirmationDialogRuntime(GeneralConfirmationDialogDependencies dependencies,
        GeneralConfirmationMode mode);
    ~GeneralConfirmationDialogRuntime() noexcept;
    GeneralConfirmationDialogRuntime(const GeneralConfirmationDialogRuntime&) = delete;
    GeneralConfirmationDialogRuntime& operator=(const GeneralConfirmationDialogRuntime&) = delete;

    // 477C60 消费 root +1F0 后再执行 48CA30；后者仅赋值 Ref，不生成截图。
    void CaptureDisplayAt48CA30();
    void AssignCapturedFrameAt48CA30(GraphicsSurfaceReference frame);
    [[nodiscard]] std::int32_t RunModalAt5071D0();
    void BeginAtVslot44() override;
    [[nodiscard]] bool PollAtVslot48() override;
    [[nodiscard]] bool EndAtVslot4C() override;
    [[nodiscard]] std::int32_t ResultAtVslot50() const override;
    [[nodiscard]] IModalDialogRuntimeTarget* ResolveAt4A5330() override;

    void DrawAt5230B0(const RECT& clip);
    void CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip);
    void SubmitControlRectAt50FC80(const RECT& rect);
    void FlushDirtyRectsAt5267E0();
    [[nodiscard]] std::int32_t PollClickedResultAt524E60();
    [[nodiscard]] GeneralConfirmationMode ModeAt80() const noexcept;
    [[nodiscard]] std::optional<std::int32_t> PhaseAt6C() const noexcept;
    [[nodiscard]] std::span<const CgOperationChildControlRef> ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] std::span<const CgOperationClickedEventRef> ClickedEventsInInsertionOrder() const noexcept;
    [[nodiscard]] ICgOperationChildControl* ActiveControlAt4C() const noexcept;
    [[nodiscard]] const GraphicsSurfaceReference& CapturedFrameAt5C() const noexcept;

private:
    struct State;
    std::unique_ptr<State> state_;
};
}
