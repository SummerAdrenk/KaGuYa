#pragma once

#include <Runtime/GeneralConfirmationDialogRuntime.h>
#include <Runtime/QuickLoadRuntime.h>
#include <Runtime/RollingSaveRuntime.h>

namespace KaGuYa::Start::Runtime
{
class IReadScriptExitDialog;
class ISaveLoadReturnTitleConfirmation;
class DrawManagerLayerRuntime;

struct GeneralConfirmationLayerCaptureDependencies final
{
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenSizeApi& sizes;
    IRootTransitionFrameSource& source;
};

// 持有引用 bundle，不生成另一套 root/input/Graphics；每次创建真实强持有的模态对象。
// 返回的适配器包含 caller 在 capture/run 周围的同步；外层不要重复添加这几次同步。
class GeneralConfirmationServicesRuntime final
    : public IRollingSaveConfirmationServices,
      public IQuickLoadConfirmationServices
{
public:
    explicit GeneralConfirmationServicesRuntime(GeneralConfirmationDialogDependencies dependencies) noexcept;
    [[nodiscard]] std::unique_ptr<IRollingSaveConfirmation> CreateQuickSaveConfirmationAt4E3330() override;
    [[nodiscard]] std::unique_ptr<IQuickLoadConfirmation> CreateQuickLoadConfirmationAt4E3330() override;
    // 4B4183 原调用传 1；沿用现有 ExitDialog 接口名，但资源仍是返回标题。
    // 4B4256 的 4A8340(0) 合成 manager 注册图层，不是消费 root +1F0。
    [[nodiscard]] std::unique_ptr<IReadScriptExitDialog> CreateExitDialogAt4E3330(
        GeneralConfirmationLayerCaptureDependencies capture);
    [[nodiscard]] std::shared_ptr<ISaveLoadReturnTitleConfirmation> CreateReturnTitleConfirmationAt4E3330();
    [[nodiscard]] std::shared_ptr<GeneralConfirmationDialogRuntime> CreateAt4E3330(GeneralConfirmationMode mode);
    // 42380D：构造在原try范围之外；仅模态执行异常按关闭路径继续转发。
    [[nodiscard]] std::int32_t RunCloseConfirmationAt42380D();

private:
    GeneralConfirmationDialogDependencies dependencies_;
};
}
