#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class CgUiButtonRuntime;

// CModeSelectButton 的公共边界。样本 18 的虚表除析构外仅公开 +0x04；
// 具体对象内部固定持有四个 CGUIButton，由后续 GUI 生产实现负责。
class ISceneManagerModeSelectButton
{
public:
    virtual ~ISceneManagerModeSelectButton() = default;
    virtual void ResetButtonsAt487470() = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<CgUiButtonRuntime>>
        AcquireButtonsAt47B7B0() = 0;
};

// OptionCharSelectDialog 在 CSceneManager 切换模式时只调用虚槽 +0x44。
// 不把完整 GuiDialog 虚表压缩成这个接口；这里仅表达 0x4703D0 的可达边界。
class ISceneManagerOptionCharSelectDialog
{
public:
    virtual ~ISceneManagerOptionCharSelectDialog() = default;
    virtual void InitializeAt50D930() = 0;
};

class ISceneManagerPublicDialogFactory
{
public:
    virtual ~ISceneManagerPublicDialogFactory() = default;

    [[nodiscard]] virtual std::shared_ptr<ISceneManagerModeSelectButton>
        CreateModeSelectButtonAt45AC40(std::int32_t mode) = 0;
    [[nodiscard]] virtual std::shared_ptr<ISceneManagerOptionCharSelectDialog>
        CreateOptionCharSelectDialogAt4E4A20() = 0;
};

// CSceneManager+0x20..+0x34 的两份非数组引用及 0x4703D0 的切换规则。
// mode 0/1 保留并重新打开字符选择 Dialog；mode 2 将其释放。
class SceneManagerPublicDialogRuntime final
{
public:
    explicit SceneManagerPublicDialogRuntime(
        ISceneManagerPublicDialogFactory& factory) noexcept;

    void ConfigureAt4703D0(std::int32_t mode);
    // 4A28D4 进入标题前调用：先释放 +20，再释放 +2C，不创建替代对象。
    void ClearAt46DB90() noexcept;

    [[nodiscard]] const std::shared_ptr<ISceneManagerModeSelectButton>&
        ModeSelectButtonOwnerAt20() const noexcept;
    [[nodiscard]] const std::shared_ptr<ISceneManagerOptionCharSelectDialog>&
        OptionCharSelectDialogOwnerAt2C() const noexcept;

private:
    ISceneManagerPublicDialogFactory* factory_ = nullptr;
    std::shared_ptr<ISceneManagerModeSelectButton> modeSelectButtonAt20_;
    std::shared_ptr<ISceneManagerOptionCharSelectDialog>
        optionCharSelectDialogAt2C_;
};
}
