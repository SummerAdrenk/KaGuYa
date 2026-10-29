#include <Runtime/CgOperationDialogRuntime.h>

#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
CgOperationSurfaceArgument::~CgOperationSurfaceArgument() noexcept
{
    if (unwinding_) ReleaseYtlReference(argument_);
}

void CgOperationSurfaceArgument::FinishNormally() noexcept
{
    // 5235E4/523324/523066：先进入 state1，尾释放抛出不重试。
    unwinding_ = false;
    auto* control = argument_.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control = argument_.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
}

CgOperationDialogRuntime::CgOperationDialogRuntime(
    ICgOperationDialogServices& services,
    CgOperationDialogPrimaryControlRef primary,
    CgOperationDialogPopupControlRef popup,
    const RECT& dialogRectAt68)
    : services_(&services),
      primary_(std::move(primary)),
      popup_(std::move(popup)),
      dialogRectAt68_(dialogRectAt68)
{
    if (!primary_ || !popup_)
        throw std::invalid_argument("CGOperationDialog direct child is null");
}

void CgOperationDialogRuntime::SetSelectionValueAt7C(
    const std::int32_t value) noexcept
{
    selectionAt7C_ = value;
}

void CgOperationDialogRuntime::OpenAtVslot44()
{
    // 0x50BFD0：自身、两个直接子控件、选中项提交，顺序不可交换。
    static_cast<void>(WriteAtVslot24(1));
    primary_->OpenAtVslot44();
    popup_->OpenAtVslot44();
    primary_->ApplySelectionAt537010(selectionAt7C_);
}

void CgOperationDialogRuntime::CompositeAt523480(
    IGraphicsSurfaceCompositionApi& graphics, YtlReferenceSlotRuntime& argument,
    const RECT* const destinationClip)
{
    CgOperationSurfaceArgument parameter(argument);
    if (stateAt38_ != 0)
    {
        const auto composite = [&](const auto& child)
        {
            // 5234C5：每个子对象重新读取父 mode；状态调用后不冻结子引用。
            if (stateAt34_ != 0)
            {
                YtlReferenceSlotRuntime copied;
                CopyGraphicsSurfaceReferenceAt47D740(copied, argument);
                child->CompositeAtVslot5C(graphics, copied, destinationClip);
            }
            else
            {
                const auto saved = child->StateAtVslot20();
                static_cast<void>(child->WriteAtVslot1C(0));
                YtlReferenceSlotRuntime copied;
                CopyGraphicsSurfaceReferenceAt47D740(copied, argument);
                child->CompositeAtVslot5C(graphics, copied, destinationClip);
                static_cast<void>(child->WriteAtVslot1C(saved));
            }
        };
        composite(primary_);
        composite(popup_);
    }
    parameter.FinishNormally();
}

std::int32_t CgOperationDialogRuntime::StateAtVslot20() const noexcept { return stateAt34_; }
std::int32_t CgOperationDialogRuntime::WriteAtVslot1C(const std::int32_t value) noexcept
{
    stateAt34_ = value;
    return value;
}
std::int32_t CgOperationDialogRuntime::WriteAtVslot24(const std::int32_t value)
{
    stateAt38_ = value;
    services_->SetDialogStateAtVslot24(value);
    return value;
}
std::int32_t CgOperationDialogRuntime::StateAt38() const noexcept { return stateAt38_; }

std::int32_t CgOperationDialogRuntime::UpdateAtVslot48()
{
    // 0x52C2FF 在任何子控件调用前清除上一帧结果。
    resultAt78_ = -1;
    if (primary_->StateAtVslot20())
        ProcessPrimaryControl();
    else
        ProcessPopupControl();
    return 1;
}

std::int32_t CgOperationDialogRuntime::ResultAtVslot50() const noexcept
{
    return resultAt78_;
}

std::int32_t CgOperationDialogRuntime::SelectionAt7C() const noexcept
{
    return selectionAt7C_;
}

const RECT& CgOperationDialogRuntime::DialogRectAt68() const noexcept
{
    return dialogRectAt68_;
}

void CgOperationDialogRuntime::BeginSelectionAt502110()
{
    // 0x502110：先把 +0x84 回退到 +0x80，再解析 childId 1 的 Popup。
    selectionSlotsAt80_.clear();
    popup_->ResetPagesAt5021C0();
}

void CgOperationDialogRuntime::AppendAvailabilityAt4FEA00(
    const bool available)
{
    // 4FEA00 先扩展 +0x80，再取新末项索引进入 Popup::4FEAE0。
    selectionSlotsAt80_.push_back(available ? 1 : 0);
    const std::size_t itemIndex = selectionSlotsAt80_.size() - 1u;
    if (itemIndex >
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::length_error("CGOperationDialog +0x80 vector exceeds int32 range");
    }
    popup_->DrawSelectionAt4FEAE0(
        static_cast<std::int32_t>(itemIndex), available ? 1 : 0);
}

void CgOperationDialogRuntime::SetSelectionSlotsAt80(
    std::vector<std::int32_t> slots)
{
    selectionSlotsAt80_ = std::move(slots);
}

const std::vector<std::int32_t>&
CgOperationDialogRuntime::SelectionSlotsAt80() const noexcept
{
    return selectionSlotsAt80_;
}

void CgOperationDialogRuntime::ProcessPrimaryControl()
{
    (void)primary_->UpdateAtVslot48();

    // 原函数为每个比较重新调用 +0x50，不能缓存为一次查询。
    if (primary_->EventAtVslot50() == 7)
    {
        resultAt78_ = 0;
        return;
    }
    if (primary_->EventAtVslot50() == 6)
    {
        resultAt78_ = 1;
        return;
    }
    if (primary_->EventAtVslot50() == 5)
    {
        MoveSelectionForward();
        return;
    }
    if (primary_->EventAtVslot50() == 3)
    {
        MoveSelectionBackward();
        return;
    }
    if (primary_->EventAtVslot50() != 4)
        return;

    resultAt78_ = 2;
    (void)primary_->WriteAtVslot1C(0);
    (void)popup_->WriteAtVslot24(1);
    popup_->SetPageAt527500(selectionAt7C_ / 20);
    services_->SubmitRectAtVslot64(dialogRectAt68_);
}

void CgOperationDialogRuntime::ProcessPopupControl()
{
    // 0x52C5DE 的关闭判定发生在 popup 的 +0x48 之前。
    if (services_->ReadCloseInputAt47F4E0() ||
        services_->ReadOpaqueCloseStateAt52C613())
    {
        resultAt78_ = 3;
        (void)primary_->WriteAtVslot1C(1);
        primary_->InvokeAtVslot68();
        (void)popup_->WriteAtVslot24(0);
        services_->SubmitRectAtVslot64(dialogRectAt68_);
        return;
    }

    (void)popup_->UpdateAtVslot48();
    const std::int32_t event = popup_->EventAtVslot50();
    if (event == 10)
    {
        std::int32_t page = popup_->CurrentPageAt94() - 1;
        if (page < 0)
            page = popup_->PageCountAt88And8C() - 1;
        popup_->SetPageAt527500(page);
        RECT rect{};
        (void)popup_->ReadRectAtVslot8C(&rect);
        services_->SubmitRectAtVslot64(rect);
        return;
    }
    if (event == 11)
    {
        const std::int32_t next = popup_->CurrentPageAt94() + 1;
        const std::int32_t page = next < popup_->PageCountAt88And8C()
            ? next
            : 0;
        popup_->SetPageAt527500(page);
        RECT rect{};
        (void)popup_->ReadRectAtVslot8C(&rect);
        services_->SubmitRectAtVslot64(rect);
        return;
    }
    if (event < 12 || event > 31)
        return;

    const std::uint32_t wrapped =
        static_cast<std::uint32_t>(event) +
        20u * static_cast<std::uint32_t>(popup_->CurrentPageAt94()) - 12u;
    const std::int32_t candidate = std::bit_cast<std::int32_t>(wrapped);
    if (candidate < 0)
        throw std::out_of_range("CGOperationDialog popup page produced a negative index");
    if (static_cast<std::size_t>(candidate) >= selectionSlotsAt80_.size() ||
        selectionSlotsAt80_[static_cast<std::size_t>(candidate)] == 0)
    {
        return;
    }

    if (candidate == selectionAt7C_)
    {
        if (auto target = popup_->ResolveEventTargetAtVslot14(event))
        {
            popup_->InvokeAtVslot68();
            popup_->SubmitAtVslot30({});
            const auto resolved =
                services_->ResolveEventTargetAt53DDE0(target, 0);
            popup_->SubmitAtVslot30(resolved);
        }
        services_->SubmitRectAtVslot64(dialogRectAt68_);
        return;
    }

    primary_->ApplySelectionAt537010(candidate);
    selectionAt7C_ = candidate;
    resultAt78_ = 4;
    (void)primary_->WriteAtVslot1C(1);
    primary_->InvokeAtVslot68();
    (void)popup_->WriteAtVslot24(0);
    services_->SubmitRectAtVslot64(dialogRectAt68_);
}

void CgOperationDialogRuntime::MoveSelectionForward()
{
    if (selectionSlotsAt80_.empty())
        throw std::out_of_range("CGOperationDialog +0x80 vector is empty");
    if (selectionAt7C_ < 0 ||
        static_cast<std::size_t>(selectionAt7C_) >= selectionSlotsAt80_.size())
    {
        throw std::out_of_range("CGOperationDialog +0x7C is outside +0x80 vector");
    }

    std::int32_t candidate = selectionAt7C_ + 1;
    for (;; ++candidate)
    {
        if (static_cast<std::size_t>(candidate) >= selectionSlotsAt80_.size())
            candidate = 0;
        if (candidate == selectionAt7C_)
            return;
        if (selectionSlotsAt80_[static_cast<std::size_t>(candidate)] != 0)
        {
            ApplyPrimarySelectionAndSubmitPopup(candidate);
            return;
        }
    }
}

void CgOperationDialogRuntime::MoveSelectionBackward()
{
    if (selectionSlotsAt80_.empty())
        throw std::out_of_range("CGOperationDialog +0x80 vector is empty");
    if (selectionAt7C_ < 0 ||
        static_cast<std::size_t>(selectionAt7C_) >= selectionSlotsAt80_.size())
    {
        throw std::out_of_range("CGOperationDialog +0x7C is outside +0x80 vector");
    }

    std::int32_t candidate = selectionAt7C_ - 1;
    for (;; --candidate)
    {
        if (candidate < 0)
            candidate = static_cast<std::int32_t>(selectionSlotsAt80_.size()) - 1;
        if (candidate == selectionAt7C_)
            return;
        if (selectionSlotsAt80_[static_cast<std::size_t>(candidate)] != 0)
        {
            ApplyPrimarySelectionAndSubmitPopup(candidate);
            return;
        }
    }
}

void CgOperationDialogRuntime::ApplyPrimarySelectionAndSubmitPopup(
    const std::int32_t selection)
{
    // 0x52C42E/0x52C4CF：读取并提交 Primary rect 后才更新 +0x7C/+0x78。
    primary_->ApplySelectionAt537010(selection);
    RECT rect{};
    (void)primary_->ReadRectAtVslot8C(&rect);
    services_->SubmitRectAtVslot64(rect);
    selectionAt7C_ = selection;
    resultAt78_ = 4;
}
}
