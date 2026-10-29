#pragma once

#include <Runtime/CgOperationSurfaceResourceRuntime.h>
#include <Runtime/ProgramSelectionDialogRuntime.h>

#include <Windows.h>

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
struct ProgramSelectionSystemControlDescriptor final
{
    CgOperationChildDescriptor control;
    std::optional<CgOperationControlResourceDescriptor> checkedResources;
};

// 样本 18 的 0x608940/0x608E5C 四组描述表。两表资源相同，
// 事件值分别属于 CharaSelect00 与 CharaSelect01 的 ID 空间。
[[nodiscard]] const std::array<ProgramSelectionSystemControlDescriptor, 11>&
ProgramSelectionSystemDescriptorsSample18(ProgramSelectionDialogMode mode);

// CharaSelectSystemDialog 的虚槽 +0x44/+0x48 直接访问的根状态。
// 两个脚本字段分别是对象偏移 +0x18/+0x14，不能合并成推测性的“自动模式”。
class IProgramSelectionSystemDialogServices
{
public:
    virtual ~IProgramSelectionSystemDialogServices() = default;

    virtual void UpdateInheritedAtVslot54() = 0;
    [[nodiscard]] virtual std::uint32_t
        ReadScriptSlot2DwordAt18() const = 0;
    [[nodiscard]] virtual std::uint32_t
        ReadScriptSlot2DwordAt14() const = 0;
    virtual void ToggleStateAt53DDE0Offset5C() = 0;
    virtual void XorScriptSlot2DwordAt18With2() = 0;
    virtual void WriteCurrentSceneNameCp932(std::string_view value) = 0;
};

// 0x4DF420/0x4DFCA0 建立的 CharaSelectSystemDialog 对象图。
// child 与 ClickedEvent 的插入顺序必须和四组静态描述表一致。
class ProgramSelectionSystemDialogObjectGraphRuntime final
{
public:
    ProgramSelectionSystemDialogObjectGraphRuntime(
        ICgOperationSurfaceFactory& surfaceFactory,
        ICgOperationButtonInputServices& buttonInputServices,
        IProgramSelectionSystemDialogServices& services,
        ProgramSelectionDialogMode mode);

    void OpenAtVslot44();
    [[nodiscard]] std::int32_t UpdateAtVslot48();
    [[nodiscard]] std::int32_t CloseAtVslot4C() const noexcept;

    [[nodiscard]] ProgramSelectionDialogMode Mode() const noexcept;
    [[nodiscard]] const std::vector<CgOperationChildControlRef>&
        ChildrenInInsertionOrder() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
        ClickedEventsAt5C() const noexcept;
    [[nodiscard]] const RECT& CombinedRect() const noexcept;
    [[nodiscard]] std::int32_t DialogStateAt38() const noexcept;

private:
    [[nodiscard]] CgOperationChildControlRef ChildByEventValue(
        std::int32_t eventValue) const;

    ProgramSelectionDialogMode mode_ = ProgramSelectionDialogMode::Mode00;
    IProgramSelectionSystemDialogServices* services_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> clickedEventsAt5C_;
    RECT combinedRect_{};
    std::int32_t dialogStateAt38_ = 1;
};
}
