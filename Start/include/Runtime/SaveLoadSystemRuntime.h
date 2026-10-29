#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/SaveLoadDialogRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ISaveLoadSystemInheritedServices
{
public:
    virtual ~ISaveLoadSystemInheritedServices() = default;

    // 0x52FDA0 的全局输入状态判断不属于 SaveLoadSystem 对象本身。
    [[nodiscard]] virtual bool ReadCancelRequestAt52FDA0() = 0;
    virtual void UpdateGuiDialogAtVslot54() = 0;
    virtual void ResetInputAtVslot68() = 0;
    [[nodiscard]] virtual bool
        ReadRootByteAt218AfterSynchronizeAt413BE0() = 0;
};

[[nodiscard]] std::span<const CgOperationChildDescriptor>
SaveLoadSystemChildDescriptorsSample18At603DB0(bool saveMode) noexcept;

class ISaveLoadPageModeSystem : public ISaveLoadDialogChild
{
public:
    virtual ~ISaveLoadPageModeSystem() = default;

    // 0x535160：事件 14/15/16 对应控件的 +0x44 分别接收 mode!=1/2/3。
    virtual void ApplyPageModeAt535160(std::int32_t mode) = 0;
};

using SaveLoadPageModeSystemRef =
    std::shared_ptr<ISaveLoadPageModeSystem>;

// GuiControl::GuiSaveLoadDialog::SaveLoadSystem，构造入口 0x4F0C80。
// 两种模式分别严格持有 4/7 个 Button 与同序 ClickedEvent。
class SaveLoadSystemObjectGraphRuntime final : public ISaveLoadPageModeSystem
{
public:
    SaveLoadSystemObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        ICgOperationButtonInputServices& buttonInputServices,
        ISaveLoadSystemInheritedServices& inheritedServices,
        bool saveMode);

    [[nodiscard]] std::int32_t StateAtVslot20() const override;
    void WriteEnabledAtVslot24(std::int32_t value) override;
    [[nodiscard]] std::int32_t GateAtVslot28() const override;
    void OpenAtVslot44() override;
    void UpdateAtVslot48() override;
    void EndAtVslot4C() override;
    [[nodiscard]] std::int32_t EventAtVslot50() const override;
    [[nodiscard]] SaveLoadDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) override;
    void ApplyPageModeAt535160(std::int32_t mode) override;

    void WriteStateAtVslot20(std::int32_t value) noexcept;
    void ClearChildrenAtVslot08() noexcept;

    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
        ClickedEventManagerAt70() const noexcept;
    [[nodiscard]] const RECT& CombinedRectAt60() const noexcept;

private:
    ISaveLoadSystemInheritedServices* inheritedServices_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::vector<SaveLoadDialogEventTargetRef> eventTargets_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime>
        clickedEventManagerAt70_;
    RECT combinedRectAt60_{};
    std::int32_t stateAt34_ = 1;
    std::int32_t gateAt38_ = 1;
    bool eventAt5CInitialized_ = false;
    std::int32_t eventAt5C_ = -1;
};
}
