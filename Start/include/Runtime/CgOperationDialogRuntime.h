#pragma once

#include <Runtime/CgDisplayResourceRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceCompositionApi;

// 523480/5231C0/522F80 的按值参数已由调用方增持。
// 正常尾释放不清字段且抛出即 terminate；异常展开调用 40D2F0(flags=1)，清 base/control。
class CgOperationSurfaceArgument final
{
public:
    explicit CgOperationSurfaceArgument(YtlReferenceSlotRuntime& argument) noexcept
        : argument_(argument) {}
    ~CgOperationSurfaceArgument() noexcept;
    CgOperationSurfaceArgument(const CgOperationSurfaceArgument&) = delete;
    CgOperationSurfaceArgument& operator=(const CgOperationSurfaceArgument&) = delete;
    void FinishNormally() noexcept;
private:
    YtlReferenceSlotRuntime& argument_;
    bool unwinding_ = true;
};

class ICgOperationDialogEventTarget
{
public:
    virtual ~ICgOperationDialogEventTarget() = default;

    // GuiControl vslot +0x20/+0x24。0x532170 切换 active control 时
    // 先令旧对象失焦，再令新对象获焦。
    virtual void GainFocusAtVslot20() = 0;
    virtual void LoseFocusAtVslot24() = 0;
};

using CgOperationDialogEventTargetRef =
    std::shared_ptr<ICgOperationDialogEventTarget>;

class ICgOperationDialogPrimaryControl
{
public:
    virtual ~ICgOperationDialogPrimaryControl() = default;

    virtual void OpenAtVslot44() = 0;
    [[nodiscard]] virtual std::int32_t StateAtVslot20() const = 0;
    [[nodiscard]] virtual std::int32_t UpdateAtVslot48() = 0;
    [[nodiscard]] virtual std::int32_t EventAtVslot50() const = 0;
    [[nodiscard]] virtual std::int32_t WriteAtVslot1C(
        std::int32_t value) = 0;
    virtual void InvokeAtVslot68() = 0;
    virtual void ApplySelectionAt537010(std::int32_t selection) = 0;
    [[nodiscard]] virtual RECT* ReadRectAtVslot8C(RECT* output) const = 0;
    // 消费 argument 的一次增持；正常返回后调用方不得再次释放该槽。
    virtual void CompositeAtVslot5C(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip) = 0;
};

using CgOperationDialogPrimaryControlRef =
    std::shared_ptr<ICgOperationDialogPrimaryControl>;

class ICgOperationDialogPopupControl
{
public:
    virtual ~ICgOperationDialogPopupControl() = default;

    virtual void OpenAtVslot44() = 0;
    [[nodiscard]] virtual std::int32_t StateAtVslot20() const = 0;
    [[nodiscard]] virtual std::int32_t WriteAtVslot1C(std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t UpdateAtVslot48() = 0;
    [[nodiscard]] virtual std::int32_t EventAtVslot50() const = 0;
    [[nodiscard]] virtual std::int32_t WriteAtVslot24(
        std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t CurrentPageAt94() const = 0;
    [[nodiscard]] virtual std::int32_t PageCountAt88And8C() const = 0;
    virtual void SetPageAt527500(std::int32_t page) = 0;
    virtual void ResetPagesAt5021C0() = 0;
    virtual void DrawSelectionAt4FEAE0(
        std::int32_t itemIndex,
        std::int32_t available) = 0;
    [[nodiscard]] virtual CgOperationDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) = 0;
    virtual void InvokeAtVslot68() = 0;
    virtual void SubmitAtVslot30(
        CgOperationDialogEventTargetRef value) = 0;
    [[nodiscard]] virtual RECT* ReadRectAtVslot8C(RECT* output) const = 0;
    virtual void CompositeAtVslot5C(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip) = 0;
};

using CgOperationDialogPopupControlRef =
    std::shared_ptr<ICgOperationDialogPopupControl>;

class ICgOperationDialogServices
{
public:
    virtual ~ICgOperationDialogServices() = default;

    virtual void SetDialogStateAtVslot24(std::int32_t value) = 0;
    [[nodiscard]] virtual bool ReadCloseInputAt47F4E0() = 0;
    [[nodiscard]] virtual bool ReadOpaqueCloseStateAt52C613() = 0;
    [[nodiscard]] virtual CgOperationDialogEventTargetRef
        ResolveEventTargetAt53DDE0(
        const CgOperationDialogEventTargetRef& target,
        std::int32_t index) = 0;
    virtual void SubmitRectAtVslot64(const RECT& rect) = 0;
};

// CGOperationDialog 的 +0x44/+0x48/+0x50 生产状态对象。
// 事件整数及未知字段继续使用地址/虚槽命名，避免为尚未证明的业务语义命名。
class CgOperationDialogRuntime final
    : public ICgOperationDialogResourceControl
{
public:
    CgOperationDialogRuntime(
        ICgOperationDialogServices& services,
        CgOperationDialogPrimaryControlRef primary,
        CgOperationDialogPopupControlRef popup,
        const RECT& dialogRectAt68);

    void SetSelectionValueAt7C(std::int32_t value) noexcept override;
    void OpenAtVslot44() override;

    void CompositeAt523480(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip);
    [[nodiscard]] std::int32_t StateAtVslot20() const noexcept;
    [[nodiscard]] std::int32_t WriteAtVslot1C(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t WriteAtVslot24(std::int32_t value);
    [[nodiscard]] std::int32_t StateAt38() const noexcept;

    [[nodiscard]] std::int32_t UpdateAtVslot48();
    [[nodiscard]] std::int32_t ResultAtVslot50() const noexcept;
    [[nodiscard]] std::int32_t SelectionAt7C() const noexcept;
    [[nodiscard]] const RECT& DialogRectAt68() const noexcept;
    void BeginSelectionAt502110();
    void AppendAvailabilityAt4FEA00(bool available);
    void SetSelectionSlotsAt80(std::vector<std::int32_t> slots);
    [[nodiscard]] const std::vector<std::int32_t>&
        SelectionSlotsAt80() const noexcept;

private:
    void ProcessPrimaryControl();
    void ProcessPopupControl();
    void MoveSelectionForward();
    void MoveSelectionBackward();
    void ApplyPrimarySelectionAndSubmitPopup(std::int32_t selection);

    ICgOperationDialogServices* services_ = nullptr;
    CgOperationDialogPrimaryControlRef primary_;
    CgOperationDialogPopupControlRef popup_;
    std::int32_t stateAt34_ = 1;
    std::int32_t stateAt38_ = 1;
    RECT dialogRectAt68_{};
    std::int32_t resultAt78_ = -1;
    std::int32_t selectionAt7C_ = 0;
    std::vector<std::int32_t> selectionSlotsAt80_;
};
}
