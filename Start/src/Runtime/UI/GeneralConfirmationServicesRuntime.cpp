#include <Runtime/GeneralConfirmationServicesRuntime.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/SaveLoadDialogRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 包装仅适配既有调用契约；对象图、输入和模态四槽全部由实际对象执行。
class ConfirmationOwner final : public IRollingSaveConfirmation,
    public IQuickLoadConfirmation,
    public ISaveLoadReturnTitleConfirmation
{
public:
    ConfirmationOwner(std::shared_ptr<GeneralConfirmationDialogRuntime> dialog,
        IModalDialogRunnerServices& modal) noexcept : dialog_(std::move(dialog)), modal_(modal) {}
    void CaptureDisplayAt48CA30() override
    {
        dialog_->CaptureDisplayAt48CA30();
    }
    std::int32_t RunModalAt5071D0() override
    {
        // 485DE3/4860C3/51E3E2 的 caller 同步先于进入 runner，不属于 Begin。
        modal_.SynchronizeRuntimeAt413BE0();
        return dialog_->RunModalAt5071D0();
    }
    std::int32_t RunAt5071D0() override { return RunModalAt5071D0(); }
private:
    std::shared_ptr<GeneralConfirmationDialogRuntime> dialog_;
    IModalDialogRunnerServices& modal_;
};

class ReadScriptConfirmationOwner final : public IReadScriptExitDialog
{
public:
    ReadScriptConfirmationOwner(std::shared_ptr<GeneralConfirmationDialogRuntime> dialog,
        IModalDialogRunnerServices& modal, GeneralConfirmationLayerCaptureDependencies capture) noexcept
        : dialog_(std::move(dialog)), modal_(modal), capture_(capture) {}
    void CaptureDisplayAt48CA30() override
    {
        // 4B423C/4B25AE：捕获前才选manager，之后跨分配/Create回调固定receiver。
        auto manager = capture_.source.CaptureDrawLayersAt413BE0();
        // 4A8340：先分配 CSurface，再读尺寸；Create(AL)==0 不合成，返回空 Ref。
        GraphicsSurfaceReference frame(capture_.surfaces);
        CompositeGraphicsFrameAt4A8340(frame.Slot(), capture_.surfaces, capture_.sizes, manager, false);
        dialog_->AssignCapturedFrameAt48CA30(std::move(frame));
        modal_.SynchronizeRuntimeAt413BE0(); // 4B426F，后续 caller 才重新启用 Tool capture mode。
    }
    std::int32_t RunModalAt5071D0() override
    {
        modal_.SynchronizeRuntimeAt413BE0(); // 4B4296
        const auto result = dialog_->RunModalAt5071D0();
        modal_.SynchronizeRuntimeAt413BE0(); // 4B42BE，先于 caller 恢复 wait-mark。
        return result;
    }
private:
    std::shared_ptr<GeneralConfirmationDialogRuntime> dialog_;
    IModalDialogRunnerServices& modal_;
    GeneralConfirmationLayerCaptureDependencies capture_;
};
}

GeneralConfirmationServicesRuntime::GeneralConfirmationServicesRuntime(
    GeneralConfirmationDialogDependencies dependencies) noexcept : dependencies_(dependencies) {}

std::shared_ptr<GeneralConfirmationDialogRuntime> GeneralConfirmationServicesRuntime::CreateAt4E3330(
    GeneralConfirmationMode mode)
{
    return std::make_shared<GeneralConfirmationDialogRuntime>(dependencies_, mode);
}

std::int32_t GeneralConfirmationServicesRuntime::RunCloseConfirmationAt42380D()
{
    auto dialog = CreateAt4E3330(GeneralConfirmationMode::Exit);
    std::int32_t result;
    // 5E3084仅覆盖state2..3；4238B4同步及5071D0在try内，4E3330构造不在。
    try
    {
        dependencies_.modal.SynchronizeRuntimeAt413BE0();
        // 不先Capture：此caller由真实Begin在缺少截帧时执行捕获。
        result = dialog->RunModalAt5071D0();
        if (result != IDYES)
        {
            dialog.reset(); // 4238F1的state3：取消路径在try内释放。
            return result;
        }
    }
    catch (...)
    {
        // 423930 -> 423939释放引用 -> 423965转发旧过程；不补End或清root门控。
        result = IDYES;
    }
    dialog.reset(); // 42393C的state5：确认和catch路径在try外释放。
    return result;
}
std::unique_ptr<IRollingSaveConfirmation> GeneralConfirmationServicesRuntime::CreateQuickSaveConfirmationAt4E3330()
{
    return std::make_unique<ConfirmationOwner>(CreateAt4E3330(GeneralConfirmationMode::QuickSave), dependencies_.modal);
}
std::unique_ptr<IQuickLoadConfirmation> GeneralConfirmationServicesRuntime::CreateQuickLoadConfirmationAt4E3330()
{
    return std::make_unique<ConfirmationOwner>(CreateAt4E3330(GeneralConfirmationMode::QuickLoad), dependencies_.modal);
}
std::unique_ptr<IReadScriptExitDialog> GeneralConfirmationServicesRuntime::CreateExitDialogAt4E3330(
    GeneralConfirmationLayerCaptureDependencies capture)
{
    return std::make_unique<ReadScriptConfirmationOwner>(CreateAt4E3330(GeneralConfirmationMode::ReturnTitle),
        dependencies_.modal, capture);
}
std::shared_ptr<ISaveLoadReturnTitleConfirmation> GeneralConfirmationServicesRuntime::CreateReturnTitleConfirmationAt4E3330()
{
    return std::make_shared<ConfirmationOwner>(CreateAt4E3330(GeneralConfirmationMode::ReturnTitle), dependencies_.modal);
}
}
