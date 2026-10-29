#include <Runtime/CgDisplayButtonGroupStateRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
CgDisplayButtonGroupStateRuntime::CgDisplayButtonGroupStateRuntime(
    ICgDisplayButtonGroupStateServices& services) noexcept
    : services_(&services)
{
}

void CgDisplayButtonGroupStateRuntime::InstallMainButtons(
    std::vector<std::shared_ptr<CgUiButtonRuntime>> buttons)
{
    if (buttons.size() > NavigationEntryCountAt6021E8)
    {
        throw std::invalid_argument(
            "CCGdispBtn main button count exceeds the 27-row navigation table");
    }
    mainButtonsAt14_ = std::move(buttons);
}

void CgDisplayButtonGroupStateRuntime::InstallPageButtons(
    std::vector<std::shared_ptr<CgUiButtonRuntime>> buttons)
{
    // 0x499DC0 只在页数大于一时构造上一页、下一页两个对象。
    if (!buttons.empty() && buttons.size() != 2u)
    {
        throw std::invalid_argument(
            "CCGdispBtn page button array must contain exactly two entries");
    }
    pageButtonsAt5C_ = std::move(buttons);
}

void CgDisplayButtonGroupStateRuntime::InstallNavigationTable(
    std::vector<std::array<std::int32_t, 4>> navigationTable)
{
    // 0x499DC0 最后把 +0x20 固定为 0x6021E8；该区恒为 27x4，
    // 不会随当前分类的 itemCount 缩短。
    if (navigationTable.size() != NavigationEntryCountAt6021E8)
    {
        throw std::invalid_argument(
            "CCGdispBtn navigation table must contain the 27 rows at 0x6021E8");
    }
    navigationTableAt20_ = std::move(navigationTable);
}

void CgDisplayButtonGroupStateRuntime::ReleasePageButtonsAt5C() noexcept
{
    pageButtonsAt5C_.clear();
}

void CgDisplayButtonGroupStateRuntime::ReleaseMainButtonsAt14() noexcept
{
    mainButtonsAt14_.clear();
}

void CgDisplayButtonGroupStateRuntime::ClearNavigationTableAt20() noexcept
{
    navigationTableAt20_.clear();
}

std::int32_t CgDisplayButtonGroupStateRuntime::AddDirtyRectAtVslot08(
    const RECT& rect)
{
    if (::IsRectEmpty(&rect) != FALSE)
        return 1;
    return services_->SubmitDirtyRectAtVslot08(rect);
}

void CgDisplayButtonGroupStateRuntime::ResetAvailableButtonsAtVslot14()
{
    for (const auto& button : mainButtonsAt14_)
    {
        if (button && button->OwnedSurfaceCountAt5C() != 0u)
            button->SetStateAt04(0);
    }
}

std::int32_t CgDisplayButtonGroupStateRuntime::SetButtonIdleAtVslot18(
    const std::int32_t index)
{
    return SetButtonStateAndInvalidate(index, 0, 4);
}

std::int32_t CgDisplayButtonGroupStateRuntime::SetButtonPressedAtVslot1C(
    const std::int32_t index)
{
    return SetButtonStateAndInvalidate(index, 1, 5);
}

std::int32_t CgDisplayButtonGroupStateRuntime::SetButtonFocusedAtVslot20(
    const std::int32_t index)
{
    return SetButtonStateAndInvalidate(index, 2, 6);
}

std::int32_t CgDisplayButtonGroupStateRuntime::ActivateButtonAtVslot24(
    const std::int32_t index)
{
    CgUiButtonRuntime* const button = ResolveMainButtonAt4ACF20(index);
    if (button == nullptr)
        return 0;

    switch (button->Configuration().inputModeAt1C)
    {
    case 0:
        button->SetStateAt04(0);
        break;
    case 1:
        button->SetStateAt04(button->StateAt04() < 4 ? 4 : 0);
        break;
    case 2:
        button->SetStateAt04(4);
        break;
    default:
        break;
    }
    return AddDirtyRectAtVslot08(button->VisibleRect());
}

std::int32_t CgDisplayButtonGroupStateRuntime::SetSelectedIndexAtVslot28(
    const std::int32_t index) noexcept
{
    selectedIndexAt04_ = index;
    inputStateAt08_ = -1;
    confirmLatchAt0C_ = 0;
    return index;
}

std::int32_t
CgDisplayButtonGroupStateRuntime::GetSelectedOrActiveIndexAtVslot2C() const noexcept
{
    if (selectedIndexAt04_ >= 0)
        return selectedIndexAt04_;

    for (std::size_t index = 0; index < mainButtonsAt14_.size(); ++index)
    {
        const auto& button = mainButtonsAt14_[index];
        if (!button || button->OwnedSurfaceCountAt5C() == 0u)
            continue;
        if (button->StateAt04() == 1 || button->StateAt04() == 2)
            return static_cast<std::int32_t>(index);
    }
    return selectedIndexAt04_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::MoveSelectionAtVslot30()
{
    std::int32_t target = GetSelectedOrActiveIndexAtVslot2C();
    static_cast<void>(SetButtonIdleAtVslot18(target));

    if (target < 0)
    {
        const std::int32_t first = currentPageAt6C_ * itemsPerPageAt70_;
        const std::int32_t end = first + itemsPerPageAt70_;
        for (std::size_t index = 0; index < mainButtonsAt14_.size(); ++index)
        {
            const std::int32_t signedIndex = static_cast<std::int32_t>(index);
            if (signedIndex < first || signedIndex >= end)
                continue;
            const auto& button = mainButtonsAt14_[index];
            if (button && button->OwnedSurfaceCountAt5C() != 0u)
            {
                target = signedIndex;
                break;
            }
        }
    }
    else if (!navigationTableAt20_.empty())
    {
        // 0x4A9D90 只在 +0x08 为 0..3 时调用本槽；其它值说明调用边界已坏。
        if (inputStateAt08_ < 0 || inputStateAt08_ >= 4)
            throw std::out_of_range("CCGdispBtn navigation direction is outside 0..3");
        if (static_cast<std::size_t>(target) >= navigationTableAt20_.size())
            throw std::out_of_range("CCGdispBtn current navigation index is outside table");

        std::int32_t next = navigationTableAt20_[static_cast<std::size_t>(target)]
            [static_cast<std::size_t>(inputStateAt08_)];
        if (next >= 0)
        {
            std::size_t traversed = 0;
            for (;;)
            {
                target = next;
                if (target < 0 ||
                    static_cast<std::size_t>(target) >= navigationTableAt20_.size() ||
                    static_cast<std::size_t>(target) >= mainButtonsAt14_.size())
                {
                    // 样本 18 的 108 项均为 0..26；原代码若到达这里会越界读静态区。
                    throw std::out_of_range("CCGdispBtn navigation target is outside table");
                }

                const auto& button = mainButtonsAt14_[static_cast<std::size_t>(target)];
                if (button && button->OwnedSurfaceCountAt5C() != 0u)
                    break;

                if (++traversed > navigationTableAt20_.size())
                {
                    throw std::runtime_error(
                        "CCGdispBtn navigation cycle contains no available button");
                }
                next = navigationTableAt20_[static_cast<std::size_t>(target)]
                    [static_cast<std::size_t>(inputStateAt08_)];
            }
        }
    }

    static_cast<void>(SetSelectedIndexAtVslot28(target));
    return SetButtonPressedAtVslot1C(target);
}

void CgDisplayButtonGroupStateRuntime::ResetGroupAt4C1260(
    const bool invalidateFullScreen)
{
    // 原函数按 itemCount 遍历主按钮，同时清 +0x04 与 +0x20。
    for (const auto& button : mainButtonsAt14_)
    {
        if (!button)
            continue;
        button->SetStateAt04(0);
        button->SetInputLatchAt20(false);
    }

    services_->ResetCategorySelectorAtVslot90();
    for (const auto& button : pageButtonsAt5C_)
    {
        if (!button)
            continue;
        button->SetStateAt04(0);
        button->SetInputLatchAt20(false);
    }

    releaseGateStateAt10_ = 0;
    static_cast<void>(SetSelectedIndexAtVslot28(-1));
    if (invalidateFullScreen)
        static_cast<void>(AddDirtyRectAtVslot08(services_->ReadScreenRectAt4180C0()));
}

void CgDisplayButtonGroupStateRuntime::SetInputStateAt08(
    const std::int32_t value) noexcept
{
    inputStateAt08_ = value;
}

void CgDisplayButtonGroupStateRuntime::SetConfirmLatchAt0C(
    const std::int32_t value) noexcept
{
    confirmLatchAt0C_ = value;
}

void CgDisplayButtonGroupStateRuntime::SetReleaseGateStateAt10(
    const std::int32_t value) noexcept
{
    releaseGateStateAt10_ = value;
}

void CgDisplayButtonGroupStateRuntime::SetCurrentPageAt6C(
    const std::int32_t value) noexcept
{
    currentPageAt6C_ = value;
}

void CgDisplayButtonGroupStateRuntime::SetCategoryIndexAt74(
    const std::int32_t value) noexcept
{
    categoryIndexAt74_ = value;
}

std::int32_t CgDisplayButtonGroupStateRuntime::SelectedIndexAt04() const noexcept
{
    return selectedIndexAt04_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::InputStateAt08() const noexcept
{
    return inputStateAt08_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::ConfirmLatchAt0C() const noexcept
{
    return confirmLatchAt0C_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::ReleaseGateStateAt10() const noexcept
{
    return releaseGateStateAt10_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::CurrentPageAt6C() const noexcept
{
    return currentPageAt6C_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::ItemsPerPageAt70() const noexcept
{
    return itemsPerPageAt70_;
}

std::int32_t CgDisplayButtonGroupStateRuntime::CategoryIndexAt74() const noexcept
{
    return categoryIndexAt74_;
}

std::size_t CgDisplayButtonGroupStateRuntime::ItemCountAt3C() const noexcept
{
    return mainButtonsAt14_.size();
}

const std::vector<std::array<std::int32_t, 4>>&
CgDisplayButtonGroupStateRuntime::NavigationTableAt20() const noexcept
{
    return navigationTableAt20_;
}

const std::vector<std::shared_ptr<CgUiButtonRuntime>>&
CgDisplayButtonGroupStateRuntime::MainButtonsAt14() const noexcept
{
    return mainButtonsAt14_;
}

const std::vector<std::shared_ptr<CgUiButtonRuntime>>&
CgDisplayButtonGroupStateRuntime::PageButtonsAt5C() const noexcept
{
    return pageButtonsAt5C_;
}

CgUiButtonRuntime*
CgDisplayButtonGroupStateRuntime::ResolveMainButtonAt4ACF20(
    const std::int32_t index) const noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= mainButtonsAt14_.size())
        return nullptr;
    return mainButtonsAt14_[static_cast<std::size_t>(index)].get();
}

std::int32_t CgDisplayButtonGroupStateRuntime::SetButtonStateAndInvalidate(
    const std::int32_t index,
    const std::int32_t lowState,
    const std::int32_t highState)
{
    CgUiButtonRuntime* const button = ResolveMainButtonAt4ACF20(index);
    if (button == nullptr)
        return 0;
    button->SetStateAt04(button->StateAt04() < 4 ? lowState : highState);
    return AddDirtyRectAtVslot08(button->VisibleRect());
}
}
