#include <Runtime/TitleDialogInputRuntime.h>
#include <algorithm>
#include <map>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
TitleDialogInputRuntime::TitleDialogInputRuntime(ICgOperationButtonInputServices& input) noexcept
    : input_(&input)
{
}

void TitleDialogInputRuntime::ResetAt510840()
{
    confirmLatch_ = 0;
    SubmitActiveAt532170(nullptr);
    navigationLatch_ = 0;
}

void TitleDialogInputRuntime::SubmitActiveAt532170(ICgOperationChildControl* control)
{
    if (active_ == control) return;
    if (active_) active_->LoseFocusAtVslot24();
    if (control) control->GainFocusAtVslot20();
    active_ = control;
}

std::int32_t TitleDialogInputRuntime::ProcessConfirmAt511500()
{
    if (!active_) throw std::logic_error("Title confirmation input has no active control");
    const auto mode = active_->InputModeAt08();
    if (mode != 0 && mode != 2) return 0;
    if (!confirmLatch_) throw std::logic_error("Title confirmation input was not initialized");
    const auto previous = *confirmLatch_;
    if (previous != 0)
    {
        if (!ReadInputAtVslot6C(5))
        {
            // 5115BE..5115E2：先触发绑定事件，再清 +C4；返回值仍为进入时的锁存。
            active_->InvokeReferencedObjectAt3CAtVslot00();
            confirmLatch_ = 0;
        }
    }
    else if (ReadInputAtVslot70(5))
    {
        confirmLatch_ = 1;
        active_->WriteAtVslot70(1);
        return 1;
    }
    return previous;
}

void TitleDialogInputRuntime::SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    auto* const child = dynamic_cast<ICgOperationChildControl*>(&control);
    if (!child) throw std::invalid_argument("Title active object is not a child control");
    SubmitActiveAt532170(child);
}

std::int32_t TitleDialogInputRuntime::ProcessNavigationAt51E740(
    std::span<const CgOperationChildControlRef> children,
    std::span<const GuiDialogNavigationGroupRef> navigation, ITitleDialogInputNotifications& notifications)
{
    if (notifications.ReadPointerMovedAtVslot7C() || ReadPressAtVslot80())
    {
        ResetAt510840();
        return 0;
    }
    // Title +40=511610 固定为零，+38=511500 则必须在导航之前执行。
    if (active_ && ProcessConfirmAt511500() != 0) return 1;
    const auto unchanged = navigationLatch_ != 0 ? 2 : 0;
    const auto find = [children](std::int32_t id)
    {
        const auto found = std::ranges::find_if(children,
            [id](const auto& child) { return child && child->EventValueAt04() == id; });
        if (found == children.end()) throw 1;
        return *found;
    };
    for (const auto& group : navigation)
    {
        if (!group) throw std::logic_error("Title navigation group is null");
        if (!ReadInputAtVslot70(group->InputValueAt04())) continue;
        if (active_)
        {
            const auto initial = active_->EventValueAt04();
            auto candidate = initial;
            for (;;)
            {
                candidate = group->NextAtVslot00(candidate);
                if (candidate < 0 || candidate == initial) break;
                auto child = find(candidate);
                const auto state = child->VisualStateAtVslot38();
                if (state == 1 || state == 2) continue;
                for (const auto& item : children) item->InvokeAtVslot08();
                SubmitActiveAt532170(child.get());
                navigationLatch_ = 1;
                return 3;
            }
        }
        else
        {
            for (const auto id : group->InitialCandidatesAt08())
            {
                auto child = find(id);
                const auto state = child->VisualStateAtVslot38();
                if (state == 1 || state == 2) continue;
                SubmitActiveAt532170(child.get());
                navigationLatch_ = 1;
                return 3;
            }
        }
        // 原函数选定第一组后结束搜索；不能改成失败后再试另一方向。
        return unchanged;
    }
    return unchanged;
}

void TitleDialogInputRuntime::UpdateAt518C30(std::int32_t modeAt34, std::int32_t enabledAt38,
    std::span<const CgOperationChildControlRef> children,
    std::span<const GuiDialogNavigationGroupRef> navigation, ITitleDialogInputNotifications& notifications)
{
    if (modeAt34 == 0 || enabledAt38 == 0) return;
    std::map<std::int32_t, std::int32_t> before;
    for (const auto& child : children)
    {
        if (!child) throw std::logic_error("Title dialog child is null");
        before.insert({child->EventValueAt04(), child->VisualStateAtVslot38()});
    }
    if (ProcessNavigationAt51E740(children, navigation, notifications) == 0)
    {
        for (const auto& child : children)
            if (child->ProcessAtVslot14() == 0) (void)child->ProcessAtVslot18();
    }
    for (const auto& child : children)
    {
        bool changed = before[child->EventValueAt04()] != child->VisualStateAtVslot38();
        if (!changed && child->InputModeAt08() == 5) changed = child->ReadModeFiveChangedAtVslot6C() != 0;
        if (!changed) continue;
        notifications.SubmitChildRectAt50FC80(child->ReadRectAtVslot3C());
        notifications.NotifyChildChangedAt518AE0(child);
    }
}

bool TitleDialogInputRuntime::ReadInputAtVslot6C(std::int32_t value) { return input_->ReadInputAtVslot6C(value); }
bool TitleDialogInputRuntime::ReadInputAtVslot70(std::int32_t value) { return input_->ReadInputAtVslot70(value); }
POINT TitleDialogInputRuntime::ReadPointerAtVslot74() { return input_->ReadPointerAtVslot74(); }
bool TitleDialogInputRuntime::ReadPressAtVslot80() { return input_->ReadPressAtVslot80(); }
bool TitleDialogInputRuntime::ReadReleaseAtVslot84() { return input_->ReadReleaseAtVslot84(); }
std::uint32_t TitleDialogInputRuntime::ReadSurfacePixelAtGetSurfacePixel(
    GraphicsSurfaceHandle handle, std::int32_t x, std::int32_t y)
{
    return input_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}
}
