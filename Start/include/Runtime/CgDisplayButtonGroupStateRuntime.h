#pragma once

#include <Runtime/CgUiButtonRuntime.h>

#include <Windows.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ICgDisplayButtonGroupStateServices
{
public:
    virtual ~ICgDisplayButtonGroupStateServices() = default;

    [[nodiscard]] virtual std::int32_t SubmitDirtyRectAtVslot08(
        const RECT& rect) = 0;
    virtual void ResetCategorySelectorAtVslot90() = 0;
    [[nodiscard]] virtual RECT ReadScreenRectAt4180C0() const = 0;
};

// CCGdispBtn+0x04..+0x70 中由 0x4A73F0、0x4AACD0、0x4ACD30、
// 0x4C1980、0x4B10F0、0x4C2200、0x4ACFA0 与 0x4C1260 直接证明的
// 状态内核。最终聚合 owner 直接持有本对象，不再复制一份平行状态。
class CgDisplayButtonGroupStateRuntime final
{
public:
    static constexpr std::size_t NavigationEntryCountAt6021E8 = 27u;

    explicit CgDisplayButtonGroupStateRuntime(
        ICgDisplayButtonGroupStateServices& services) noexcept;

    void InstallMainButtons(
        std::vector<std::shared_ptr<CgUiButtonRuntime>> buttons);
    void InstallPageButtons(
        std::vector<std::shared_ptr<CgUiButtonRuntime>> buttons);
    void InstallNavigationTable(
        std::vector<std::array<std::int32_t, 4>> navigationTable);
    void ReleasePageButtonsAt5C() noexcept;
    void ReleaseMainButtonsAt14() noexcept;
    void ClearNavigationTableAt20() noexcept;

    [[nodiscard]] std::int32_t AddDirtyRectAtVslot08(const RECT& rect);
    void ResetAvailableButtonsAtVslot14();
    [[nodiscard]] std::int32_t SetButtonIdleAtVslot18(std::int32_t index);
    [[nodiscard]] std::int32_t SetButtonPressedAtVslot1C(std::int32_t index);
    [[nodiscard]] std::int32_t SetButtonFocusedAtVslot20(std::int32_t index);
    [[nodiscard]] std::int32_t ActivateButtonAtVslot24(std::int32_t index);
    [[nodiscard]] std::int32_t SetSelectedIndexAtVslot28(
        std::int32_t index) noexcept;
    [[nodiscard]] std::int32_t GetSelectedOrActiveIndexAtVslot2C() const noexcept;
    [[nodiscard]] std::int32_t MoveSelectionAtVslot30();
    void ResetGroupAt4C1260(bool invalidateFullScreen);

    void SetInputStateAt08(std::int32_t value) noexcept;
    void SetConfirmLatchAt0C(std::int32_t value) noexcept;
    void SetReleaseGateStateAt10(std::int32_t value) noexcept;
    void SetCurrentPageAt6C(std::int32_t value) noexcept;
    void SetCategoryIndexAt74(std::int32_t value) noexcept;

    [[nodiscard]] std::int32_t SelectedIndexAt04() const noexcept;
    [[nodiscard]] std::int32_t InputStateAt08() const noexcept;
    [[nodiscard]] std::int32_t ConfirmLatchAt0C() const noexcept;
    [[nodiscard]] std::int32_t ReleaseGateStateAt10() const noexcept;
    [[nodiscard]] std::int32_t CurrentPageAt6C() const noexcept;
    [[nodiscard]] std::int32_t ItemsPerPageAt70() const noexcept;
    [[nodiscard]] std::int32_t CategoryIndexAt74() const noexcept;
    [[nodiscard]] std::size_t ItemCountAt3C() const noexcept;
    [[nodiscard]] const std::vector<std::array<std::int32_t, 4>>&
        NavigationTableAt20() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<CgUiButtonRuntime>>&
        MainButtonsAt14() const noexcept;
    [[nodiscard]] const std::vector<std::shared_ptr<CgUiButtonRuntime>>&
        PageButtonsAt5C() const noexcept;

private:
    [[nodiscard]] CgUiButtonRuntime* ResolveMainButtonAt4ACF20(
        std::int32_t index) const noexcept;
    [[nodiscard]] std::int32_t SetButtonStateAndInvalidate(
        std::int32_t index,
        std::int32_t lowState,
        std::int32_t highState);

    ICgDisplayButtonGroupStateServices* services_ = nullptr;
    std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtonsAt14_;
    std::vector<std::array<std::int32_t, 4>> navigationTableAt20_;
    std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtonsAt5C_;
    std::int32_t selectedIndexAt04_ = -1;
    std::int32_t inputStateAt08_ = -1;
    std::int32_t confirmLatchAt0C_ = 0;
    std::int32_t releaseGateStateAt10_ = 0;
    std::int32_t currentPageAt6C_ = 0;
    std::int32_t itemsPerPageAt70_ = 9;
    std::int32_t categoryIndexAt74_ = 0;
};
}
