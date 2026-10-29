#include <Runtime/ToolWindowLayerRuntime.h>

#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScrVmValueTable.h>
#include <Logging/Logger.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
ToolWindowLayerRuntime::ToolWindowLayerRuntime(LayerList* parent,
    IToolWindowDialogFactory& factory, const ScrVmValueTable& values,
    const ScrVmExecutableProfile& profile)
    : FrameLayerItem(parent), values_(&values), profile_(&profile)
{
    ConstructDialogs(factory);
}

ToolWindowLayerRuntime::ToolWindowLayerRuntime(LayerList* parent,
    IToolWindowDialogFactory& factory, IToolWindowReplaySource& replay)
    : FrameLayerItem(parent), replay_(&replay)
{
    ConstructDialogs(factory);
}

void ToolWindowLayerRuntime::ConstructDialogs(IToolWindowDialogFactory& factory)
{
    SetEnabledByteAt0C(0);
    std::int32_t mode = 0;
    try
    {
        for (; mode != 2; ++mode)
            dialogs_.push_back(factory.CreateDialogAt4E27F0(mode));
        state_.selectedAt3C = -1;
        state_.valueAt30 = std::uint8_t{0};
    }
    catch (...)
    {
        // 45A5A0 只清 RECT；保留已经 append 的 child、父注册与未初始化字段。
        state_.boundsAt14 = {};
        Logging::WriteFormat(Logging::Level::Error,
            "CToolWin 45A4B0 failed at dialog mode %d; partial owner retained", mode);
    }
}

ToolWindowLayerRuntime::~ToolWindowLayerRuntime()
{
    // 468DE0 -> 461960 -> 452560：按正序逐项释放，再由基类取消父注册。
    for (auto& dialog : dialogs_) dialog.reset();
}

std::shared_ptr<IToolWindowDialog> ToolWindowLayerRuntime::SelectedDialogAt417B50()
{
    const auto value = replay_ ? replay_->ReadReplayAt417B50()
        : values_->ReadSigned(profile_->ResolveNamedValue("replay"));
    const auto selected = value != 0 ? 1 : 0;
    if (state_.selectedAt3C.value() != selected)
    {
        const auto& dialog = dialogs_.at(static_cast<std::size_t>(selected));
        if (!dialog) throw std::logic_error("CToolWin 417B50 selected a null GuiToolDialog");
        // 417C01/417C06：重置成功以后才更新 selector；不能先置位或吞掉异常。
        dialog->ResetControlsAt532480();
        state_.selectedAt3C = selected;
    }
    return dialogs_.at(static_cast<std::size_t>(selected));
}

std::shared_ptr<IToolWindowDialog> ToolWindowLayerRuntime::RequireSelectedDialog()
{
    auto dialog = SelectedDialogAt417B50();
    if (!dialog) throw std::logic_error("CToolWin selected a null GuiToolDialog receiver");
    return dialog;
}

void ToolWindowLayerRuntime::SetEnabledAt418F70(std::uint8_t value)
{
    SetEnabledByteAt0C(value);
    {
        auto dialog = RequireSelectedDialog();
        dialog->SetEnabledAtVslot24(value);
    }
    // 第二次 417B50 必须重新读取 replay，不复用第一次的 child 或 RECT。
    auto dialog = RequireSelectedDialog();
    auto* parent = RegisteredLayerList();
    if (!parent) throw std::logic_error("CToolWin 418F70 has no parent dirty receiver");
    parent->DirtyRects().Add(dialog->BoundsAt60());
}

std::uint8_t ToolWindowLayerRuntime::SetRequestedEnabledAtVslot10(
    std::uint8_t requested, std::int32_t)
{
    if ((requested != 0) == Enabled()) return 0;
    SetEnabledAt418F70(requested != 0 ? 1 : 0);
    return 1;
}

void ToolWindowLayerRuntime::ResetAt47EDA0()
{
    SetEnabledByteAt0C(0);
    RequireSelectedDialog()->ResetAtVslot44();
}

void ToolWindowLayerRuntime::Draw(const RECT& dirty)
{
    RequireSelectedDialog()->DrawAtVslot60(dirty);
}

void ToolWindowLayerRuntime::CompositeToSurface(
    GraphicsSurfaceDestination destination, const RECT* clip)
{
    if (!destination.Body())
        throw std::invalid_argument("CToolWin composite requires a native destination body");
    // 481AFA..481B3F：先取得并保活选中的 Dialog，再分配借用 control；
    // Surface 参数交给 +5C 消费，Dialog 临时引用最后才释放。
    auto dialog = RequireSelectedDialog();
    YtlReferenceSlotRuntime argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument,
        const_cast<GraphicsSurfaceNativeBody*>(destination.Body()), 0);
    dialog->CompositeAtVslot5C(argument, clip);
}

void ToolWindowLayerRuntime::AdvanceOwnedFrame()
{
    // 原 vslot +14 = 46B230，仅 retn；工具输入由 4BE200 明确调度。
}
}
