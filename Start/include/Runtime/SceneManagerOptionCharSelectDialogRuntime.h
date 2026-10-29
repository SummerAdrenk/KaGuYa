#pragma once

#include <Runtime/CgDisplayButtonGroupRuntime.h>
#include <Runtime/SceneManagerPublicDialogRuntime.h>

#include <Windows.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class ISceneManagerOptionCharSelectDialogServices
{
public:
    virtual ~ISceneManagerOptionCharSelectDialogServices() = default;

    virtual void ResetDialogBaseAt510750() = 0;
    virtual void WriteDialogEnabledAtVslot24(std::int32_t value) = 0;
    virtual void ProcessGenericDialogAtVslot54() = 0;
    virtual void ClearClickedEventsAt532200() = 0;
    [[nodiscard]] virtual std::int32_t PollClickedEventAt50FDB0(
        std::int32_t clearMatchedEvent) = 0;

    [[nodiscard]] virtual bool HasChildAtVslot14(
        std::int32_t childId) const = 0;
    virtual void ResetChildAtVslot08(std::int32_t childId) = 0;
    virtual void WriteChildEnabledAtVslot4C(
        std::int32_t childId,
        std::int32_t value) = 0;
    virtual void WriteChildPositionAt34And38(
        std::int32_t childId,
        const POINT& position) = 0;
    virtual void WriteChildStateAtVslot44(
        std::int32_t childId,
        std::int32_t value) = 0;

    [[nodiscard]] virtual ICgDisplayButtonGroupDirtyRects&
        OwnDirtyRectsAtVslot00() = 0;
    virtual void DrawChildrenAt5230B0(const RECT& clip) = 0;
    virtual void CompositeChildrenAt5231C0(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) = 0;
};

// 样本 18 OptionCharSelectDialog 的选择状态核心。
// 虚表由 RTTI CompleteObjectLocator 0x5D9F64 恢复，表地址 0x5BFECC；
// 本类型只实现已由 0x50D930/0x52E100/0x53AF00/0x532240/0x512180
// 证明的业务槽，不以其他按钮类近似替代 0x4E4A20 创建的 GuiControl::Button。
class SceneManagerOptionCharSelectDialogRuntime final
    : public ISceneManagerOptionCharSelectDialog,
      public ICgDisplayButtonGroupSelector
{
public:
    explicit SceneManagerOptionCharSelectDialogRuntime(
        ISceneManagerOptionCharSelectDialogServices& services) noexcept;

    void InitializeAt50D930() override;
    [[nodiscard]] std::int32_t ProcessAtVslot48() override;
    [[nodiscard]] std::int32_t SelectionAtVslot50() const override;
    void AttachDirtyRectsAtVslot94(
        ICgDisplayButtonGroupDirtyRects& dirtyRects) override;
    void ResetAtVslot90() override;
    void DrawAtVslot60(const RECT& clip) override;
    void CompositeAtVslot5C(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) override;

    [[nodiscard]] std::int32_t StateAt68() const noexcept;
    [[nodiscard]] std::int32_t FirstVisibleChildAt80() const noexcept;
    [[nodiscard]] std::int32_t FocusedChildAt84() const noexcept;

private:
    void ApplyFiveButtonWindowAt53AF00();

    ISceneManagerOptionCharSelectDialogServices* services_ = nullptr;
    std::int32_t fieldAt4C_ = 0;
    std::int32_t stateAt68_ = -1;
    std::int32_t firstVisibleChildAt80_ = 0;
    std::int32_t focusedChildAt84_ = 0;
    std::int32_t selectionAt88_ = -1;
};
}
