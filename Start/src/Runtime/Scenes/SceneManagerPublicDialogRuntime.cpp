#include <Runtime/SceneManagerPublicDialogRuntime.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SceneManagerPublicDialogRuntime::SceneManagerPublicDialogRuntime(
    ISceneManagerPublicDialogFactory& factory) noexcept
    : factory_(&factory)
{
}

void SceneManagerPublicDialogRuntime::ConfigureAt4703D0(
    const std::int32_t mode)
{
    // 4703F7..47041F：新 CModeSelectButton 必须先完整构造；构造抛出时
    // CSceneManager 中原有的 +0x20 引用保持不变。
    auto nextModeSelectButton =
        factory_->CreateModeSelectButtonAt45AC40(mode);
    if (!nextModeSelectButton)
        throw std::runtime_error("Mode-select button factory returned null");

    // 47041F..4704DA：构造成功后才释放旧引用，安装新引用后立即调用 +0x04。
    // +0x04 抛出时新对象仍属于 CSceneManager，不能回滚为旧对象。
    modeSelectButtonAt20_ = std::move(nextModeSelectButton);
    modeSelectButtonAt20_->ResetButtonsAt487470();

    if (mode == 2)
    {
        // 4704DD..47052A：模式 2 只释放已有字符选择 Dialog，不调用 +0x44。
        optionCharSelectDialogAt2C_.reset();
        return;
    }

    // 47052D..4705F7：模式 0/1 共享同一个 Dialog；仅在槽为空时构造。
    // 构造异常发生在安装之前，当前模式按钮保持已提交状态。
    if (!optionCharSelectDialogAt2C_)
    {
        auto dialog = factory_->CreateOptionCharSelectDialogAt4E4A20();
        if (!dialog)
            throw std::runtime_error("Option-char dialog factory returned null");
        optionCharSelectDialogAt2C_ = std::move(dialog);
    }

    // 4705F7..47061A：+0x44 抛出时 Dialog 仍保留在 +0x2C 引用中。
    optionCharSelectDialogAt2C_->InitializeAt50D930();
}

const std::shared_ptr<ISceneManagerModeSelectButton>&
SceneManagerPublicDialogRuntime::ModeSelectButtonOwnerAt20() const noexcept
{
    return modeSelectButtonAt20_;
}

void SceneManagerPublicDialogRuntime::ClearAt46DB90() noexcept
{
    // 原 46DB93..46DBF2 顺序释放两个 YTL 引用；外部仍持有引用时不强行销毁。
    modeSelectButtonAt20_.reset();
    optionCharSelectDialogAt2C_.reset();
}

const std::shared_ptr<ISceneManagerOptionCharSelectDialog>&
SceneManagerPublicDialogRuntime::OptionCharSelectDialogOwnerAt2C() const noexcept
{
    return optionCharSelectDialogAt2C_;
}
}
