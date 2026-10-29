#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>

namespace KaGuYa::Start::Runtime
{
[[nodiscard]] const std::array<CgOperationChildDescriptor, 7>&
TitleDialogChildDescriptorsSample18At608358();

// 4F4530 在黑图、画布和 GameInputState 之后调用的两张普通导航表。
[[nodiscard]] std::vector<GuiDialogNavigationGroupRef>
CreateTitleDialogNavigationSample18At6083E0();

// 4F4530 的七个 child 与逐项交错绑定的 ClickedEvent。
// 这里只持有子树，不伪装成已装配 dirty/input/黑图/画布的完整 TitleDialog。
class TitleDialogChildObjectGraphRuntime final
{
public:
    TitleDialogChildObjectGraphRuntime();
    TitleDialogChildObjectGraphRuntime(ICgOperationSurfaceFactory& factory,
        ICgOperationButtonInputServices& input);
    // 分阶段构造供完整 owner 使用：失败后保留已插入 child，由外层按 EH 顺序释放。
    void PopulateChildrenAt4F4530(ICgOperationSurfaceFactory& factory,
        ICgOperationButtonInputServices& input);
    ~TitleDialogChildObjectGraphRuntime();
    TitleDialogChildObjectGraphRuntime(const TitleDialogChildObjectGraphRuntime&) = delete;
    TitleDialogChildObjectGraphRuntime& operator=(const TitleDialogChildObjectGraphRuntime&) = delete;

    const std::vector<CgOperationChildControlRef>& ChildrenInInsertionOrder() const noexcept { return children_; }
    const std::shared_ptr<CgOperationClickedEventManagerRuntime>& ClickedEventsAtC8() const noexcept { return events_; }

    // 4FE690 先释放事件 manager，4F7AC0 最后释放 child；顶层 owner 需在二者之间释放其他槽。
    void ReleaseClickedEventsAtC8() noexcept;
    void ReleaseChildrenAt0C() noexcept;

private:
    bool populationStarted_ = false;
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> events_;
};
}
