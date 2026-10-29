#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ScrVmValueTable;
class ScrVmExecutableProfile;

class IToolWindowReplaySource
{
public:
    virtual ~IToolWindowReplaySource() = default;
    virtual std::int32_t ReadReplayAt417B50() = 0;
};

// 4E27F0 的 GuiToolDialog 语义边界；不是原二进制虚表，也不提供默认成功。
class IToolWindowDialog
{
public:
    virtual ~IToolWindowDialog() = default;
    virtual void ResetControlsAt532480() = 0;
    virtual void SetEnabledAtVslot24(std::uint8_t value) = 0;
    [[nodiscard]] virtual const RECT& BoundsAt60() const = 0;
    virtual void ResetAtVslot44() = 0;
    virtual void DrawAtVslot60(const RECT& dirty) = 0;
    // 原按值 Ref12 已由调用方构造；被调用方正常/异常路径均消费一次。
    virtual void CompositeAtVslot5C(YtlReferenceSlotRuntime& argument, const RECT* clip) = 0;
};

class IToolWindowDialogFactory
{
public:
    virtual ~IToolWindowDialogFactory() = default;
    [[nodiscard]] virtual std::shared_ptr<IToolWindowDialog> CreateDialogAt4E27F0(
        std::int32_t mode) = 0;
};

// 45A4B0 仅在两个 child append 均成功后初始化 +30/+3C；+34/+38 不在构造中写入。
struct ToolWindowState final
{
    RECT boundsAt14{};
    std::optional<std::uint8_t> valueAt30;
    std::optional<std::int32_t> valueAt34;
    std::optional<std::uint8_t> valueAt38;
    std::optional<std::int32_t> selectedAt3C;
};

// CToolWin 的完整图层槽与普通/回放双 child 所有权。
// 434FB0 等工具交互另行实现；工厂必须提供真正的 GuiToolDialog 对象。
class ToolWindowLayerRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    ToolWindowLayerRuntime(LayerList* parent, IToolWindowDialogFactory& factory,
        IToolWindowReplaySource& replay);
    // 显式固定值表装配入口；原根装配必须使用上面的晚取源入口。
    ToolWindowLayerRuntime(LayerList* parent, IToolWindowDialogFactory& factory,
        const ScrVmValueTable& values, const ScrVmExecutableProfile& profile);
    ~ToolWindowLayerRuntime() override;

    [[nodiscard]] ToolWindowState& State() noexcept { return state_; }
    [[nodiscard]] const ToolWindowState& State() const noexcept { return state_; }
    [[nodiscard]] std::size_t DialogCount() const noexcept { return dialogs_.size(); }
    [[nodiscard]] std::shared_ptr<IToolWindowDialog> SelectedDialogAt417B50();
    [[nodiscard]] RECT BoundsAt479DB0() const noexcept { return state_.boundsAt14; }

    void SetEnabledAt418F70(std::uint8_t value);
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override;
    void ResetAt47EDA0();
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAt47EDA0(); }
    void Draw(const RECT& dirty) override;
    void CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* clip) override;
    void AdvanceOwnedFrame() override;

private:
    void ConstructDialogs(IToolWindowDialogFactory& factory);
    [[nodiscard]] std::shared_ptr<IToolWindowDialog> RequireSelectedDialog();
    ToolWindowState state_;
    std::vector<std::shared_ptr<IToolWindowDialog>> dialogs_;
    IToolWindowReplaySource* replay_ = nullptr;
    const ScrVmValueTable* values_ = nullptr;
    const ScrVmExecutableProfile* profile_ = nullptr;
};
}
