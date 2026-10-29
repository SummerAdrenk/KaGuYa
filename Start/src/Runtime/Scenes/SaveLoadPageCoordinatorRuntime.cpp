#include <Runtime/SaveLoadPageCoordinatorRuntime.h>

#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class SaveLoadDialogControlAdapter final : public ISaveLoadDialogControl
{
public:
    SaveLoadDialogControlAdapter(
        const std::int32_t slot,
        SaveLoadDialogChildRef child)
        : slot_(slot), child_(std::move(child))
    {
        if (!child_)
            throw std::invalid_argument("SaveLoad control child is null");
        ValidateSlotType();
    }

    void InvokeAtVslot08() override
    {
        throw std::logic_error(
            "SaveLoad outer control vslot 08 is not used by 0x522350");
    }

    [[nodiscard]] RECT ReadRectAtVslot3C() const override
    {
        if (const auto system = System())
            return system->CombinedRectAt60();
        if (const auto page = Page())
            return page->Graph().CombinedRectAt60();
        return Popup()->CombinedRectAt60();
    }

    void WriteAtVslot1C(const std::int32_t value) override
    {
        if (const auto system = System())
        {
            system->WriteStateAtVslot20(value);
            return;
        }
        if (const auto page = Page())
        {
            page->WriteStateAtVslot20(value);
            return;
        }
        Popup()->WriteStateAtVslot20(value);
    }

    void WriteAtVslot24(const std::int32_t value) override
    {
        child_->WriteEnabledAtVslot24(value);
    }

    void OpenAtVslot44() override
    {
        child_->OpenAtVslot44();
    }

    [[nodiscard]] std::int32_t ReadValueAt7C() const override
    {
        if (const auto page = Page())
            return page->Graph().CurrentPageAt7C();
        if (const auto popup = Popup())
            return popup->SelectedPageAt7C();
        throw std::logic_error("SaveLoadSystem has no +0x7C page value");
    }

    void WriteValueAt7C(const std::int32_t value) override
    {
        if (const auto page = Page())
        {
            page->WriteCurrentPageAt7C(value);
            return;
        }
        if (const auto popup = Popup())
        {
            popup->WriteSelectedPageAt7C(value);
            return;
        }
        throw std::logic_error("SaveLoadSystem has no +0x7C page value");
    }

    [[nodiscard]] std::int32_t ReadValueAt84() const override
    {
        if (const auto popup = Popup())
            return popup->PendingSelectedPageAt84();
        throw std::logic_error(
            "Only SaveLoadPopup exposes the +0x84 pending page");
    }

    void NotifySelectionAtVslot8C(
        const std::int32_t previousValue,
        const std::int32_t argument1,
        const std::int32_t argument2) override
    {
        const auto page = Page();
        if (!page)
        {
            throw std::logic_error(
                "Only SaveLoadPage handles selection vslot 8C");
        }
        page->NotifySelectionAtVslot8C(
            previousValue, argument1, argument2);
    }

private:
    [[nodiscard]] std::shared_ptr<SaveLoadSystemObjectGraphRuntime>
        System() const
    {
        return slot_ <= 1
            ? std::dynamic_pointer_cast<SaveLoadSystemObjectGraphRuntime>(child_)
            : nullptr;
    }

    [[nodiscard]] std::shared_ptr<SaveLoadPageDialogChildRuntime>
        Page() const
    {
        return slot_ >= 2 && slot_ <= 5
            ? std::dynamic_pointer_cast<SaveLoadPageDialogChildRuntime>(child_)
            : nullptr;
    }

    [[nodiscard]] std::shared_ptr<SaveLoadPopupObjectGraphRuntime>
        Popup() const
    {
        return slot_ >= 6
            ? std::dynamic_pointer_cast<SaveLoadPopupObjectGraphRuntime>(child_)
            : nullptr;
    }

    void ValidateSlotType() const
    {
        if ((slot_ <= 1 && System()) ||
            (slot_ >= 2 && slot_ <= 5 && Page()) ||
            (slot_ >= 6 && slot_ <= 7 && Popup()))
        {
            return;
        }
        throw std::logic_error(
            "SaveLoad child type does not match its proven slot");
    }

    std::int32_t slot_ = -1;
    SaveLoadDialogChildRef child_;
};
}

SaveLoadPageCoordinatorRuntime::SaveLoadPageCoordinatorRuntime(
    ISaveLoadPageCoordinatorHost& host,
    ISaveLoadPageCoordinatorFactory& factory) noexcept
    : host_(&host), factory_(&factory)
{
}

SaveLoadDialogChildRef SaveLoadPageCoordinatorRuntime::Resolve(
    const std::int32_t slot)
{
    return host_->ResolveChildAtVslot90(slot);
}

void SaveLoadPageCoordinatorRuntime::WriteEnabledIfPresent(
    const std::int32_t slot,
    const std::int32_t value)
{
    if (const auto child = Resolve(slot))
        child->WriteEnabledAtVslot24(value);
}

SaveLoadDialogChildRef SaveLoadPageCoordinatorRuntime::InsertCreated(
    const SaveLoadDialogChildRef& child)
{
    if (!child)
        throw std::logic_error("SaveLoad page factory returned a null child");
    host_->InsertChildAtVslot8C(child);
    return child;
}

void SaveLoadPageCoordinatorRuntime::InitializePageAt5330A0(
    const std::int32_t mode)
{
    if (mode == 0)
    {
        auto page = Resolve(2);
        if (!page)
            page = InsertCreated(factory_->CreatePage00At4ED190(2));
        page->WriteEnabledAtVslot24(1);

        for (std::int32_t slot = 3; slot <= 5; ++slot)
            WriteEnabledIfPresent(slot, 0);

        auto systemChild = Resolve(0);
        if (!systemChild)
            systemChild = InsertCreated(factory_->CreateSystemAt4F0C80(false));
        systemChild->WriteEnabledAtVslot24(1);

        WriteEnabledIfPresent(1, 0);

        auto popup = Resolve(6);
        if (!popup)
            popup = InsertCreated(factory_->CreatePopupAt4EF660(6));
        popup->WriteEnabledAtVslot24(0);

        WriteEnabledIfPresent(7, 0);
        return;
    }

    WriteEnabledIfPresent(2, 0);

    for (std::int32_t slot = 3; slot <= 5; ++slot)
    {
        auto page = Resolve(slot);
        if (!page)
            page = InsertCreated(factory_->CreatePage01At4EE390(slot));
        page->WriteEnabledAtVslot24(slot == mode + 2 ? 1 : 0);
    }

    WriteEnabledIfPresent(0, 0);

    auto systemChild = Resolve(1);
    if (!systemChild)
        systemChild = InsertCreated(factory_->CreateSystemAt4F0C80(true));
    systemChild->WriteEnabledAtVslot24(1);

    const auto system =
        std::dynamic_pointer_cast<ISaveLoadPageModeSystem>(systemChild);
    if (!system)
        throw std::logic_error("SaveLoad slot 1 is not SaveLoadSystem");
    system->ApplyPageModeAt535160(mode);

    WriteEnabledIfPresent(6, 0);

    auto popup = Resolve(7);
    if (!popup)
        popup = InsertCreated(factory_->CreatePopupAt4EF660(7));
    popup->WriteEnabledAtVslot24(0);
}

SaveLoadDialogChildObjectGraphRuntime::
SaveLoadDialogChildObjectGraphRuntime(
    SaveLoadDialogChildObjectGraphDependencies dependencies)
    : dependencies_(dependencies),
      visualResources_(dependencies_.surfaceFactory),
      coordinator_(*this, *this)
{
    childrenInInsertionOrder_.reserve(8u);
}

void SaveLoadDialogChildObjectGraphRuntime::InitializePageAt5330A0(
    const std::int32_t mode)
{
    coordinator_.InitializePageAt5330A0(mode);
}

void SaveLoadDialogChildObjectGraphRuntime::OpenChildrenAt532560()
{
    // 0x532560 读取 +0x5C 容器的当前顺序，并逐项调用虚槽 +0x44。
    for (const auto& child : childrenInInsertionOrder_)
        child->OpenAtVslot44();
}

const SaveLoadDialogVisualResourcesRuntime&
SaveLoadDialogChildObjectGraphRuntime::VisualResources() const noexcept
{
    return visualResources_;
}

const std::array<SaveLoadDialogChildRef, 8>&
SaveLoadDialogChildObjectGraphRuntime::Slots() const noexcept
{
    return slots_;
}

const std::vector<SaveLoadDialogChildRef>&
SaveLoadDialogChildObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return childrenInInsertionOrder_;
}

void SaveLoadDialogChildObjectGraphRuntime::
ReleaseVisualResourcesAt4F9030() noexcept
{
    visualResources_.ReleaseAt4F9030();
}

void SaveLoadDialogChildObjectGraphRuntime::ReleaseChildrenAt4F6DC0() noexcept
{
    // 4D8020 正向释放 +0x5C child vector。slots_ 只是重写版的索引，
    // 先撤销其重复强引用，确保实际对象仍按插入顺序由 vector 释放。
    for (auto& child : childrenInInsertionOrder_)
    {
        for (auto& slot : slots_)
        {
            if (slot == child)
            {
                slot.reset();
                break;
            }
        }
        child.reset();
    }
    childrenInInsertionOrder_.clear();
    for (auto& slot : slots_)
        slot.reset();
    pendingCreatedSlot_ = -1;
}

SaveLoadDialogControlRef
SaveLoadDialogChildObjectGraphRuntime::ResolveDialogControlAtVslot90(
    const std::int32_t slot)
{
    const SaveLoadDialogChildRef child = ResolveChildAtVslot90(slot);
    return child
        ? std::make_shared<SaveLoadDialogControlAdapter>(slot, child)
        : SaveLoadDialogControlRef{};
}

SaveLoadDialogChildRef
SaveLoadDialogChildObjectGraphRuntime::ResolveChildAtVslot90(
    const std::int32_t slot)
{
    if (slot < 0 || slot >= static_cast<std::int32_t>(slots_.size()))
        throw std::out_of_range("SaveLoad child slot is invalid");
    return slots_[static_cast<std::size_t>(slot)];
}

void SaveLoadDialogChildObjectGraphRuntime::InsertChildAtVslot8C(
    const SaveLoadDialogChildRef& child)
{
    if (!child)
        throw std::invalid_argument("SaveLoad inserted child is null");
    if (pendingCreatedSlot_ < 0 ||
        pendingCreatedSlot_ >= static_cast<std::int32_t>(slots_.size()))
    {
        throw std::logic_error(
            "SaveLoad child insertion has no proven factory slot");
    }

    auto& destination =
        slots_[static_cast<std::size_t>(pendingCreatedSlot_)];
    if (destination)
        throw std::logic_error("SaveLoad child slot is already occupied");
    destination = child;
    childrenInInsertionOrder_.push_back(child);
    pendingCreatedSlot_ = -1;
}

SaveLoadDialogChildRef
SaveLoadDialogChildObjectGraphRuntime::CreatePage00At4ED190(
    const std::int32_t slot)
{
    if (slot != 2)
        throw std::invalid_argument("SaveLoadPage00 slot is not 2");
    auto child = std::make_shared<SaveLoadPageDialogChildRuntime>(
        dependencies_.surfaceFactory,
        dependencies_.buttonInputServices,
        dependencies_.dialogBounds,
        SaveLoadPageKind::Save,
        dependencies_.graphics,
        visualResources_,
        dependencies_.pageServices);
    RememberCreatedSlot(slot);
    return child;
}

SaveLoadDialogChildRef
SaveLoadDialogChildObjectGraphRuntime::CreatePage01At4EE390(
    const std::int32_t slot)
{
    if (slot < 3 || slot > 5)
        throw std::invalid_argument("SaveLoadPage01 slot is outside 3..5");
    auto child = std::make_shared<SaveLoadPageDialogChildRuntime>(
        dependencies_.surfaceFactory,
        dependencies_.buttonInputServices,
        dependencies_.dialogBounds,
        static_cast<SaveLoadPageKind>(slot),
        dependencies_.graphics,
        visualResources_,
        dependencies_.pageServices);
    RememberCreatedSlot(slot);
    return child;
}

SaveLoadPageModeSystemRef
SaveLoadDialogChildObjectGraphRuntime::CreateSystemAt4F0C80(
    const bool saveMode)
{
    auto child = std::make_shared<SaveLoadSystemObjectGraphRuntime>(
        dependencies_.surfaceFactory,
        dependencies_.buttonInputServices,
        dependencies_.systemServices,
        saveMode);
    RememberCreatedSlot(saveMode ? 1 : 0);
    return child;
}

SaveLoadDialogChildRef
SaveLoadDialogChildObjectGraphRuntime::CreatePopupAt4EF660(
    const std::int32_t slot)
{
    if (slot != 6 && slot != 7)
        throw std::invalid_argument("SaveLoadPopup slot is not 6 or 7");
    auto child = std::make_shared<SaveLoadPopupObjectGraphRuntime>(
        dependencies_.surfaceFactory,
        dependencies_.graphics,
        dependencies_.buttonInputServices,
        dependencies_.sliderInputServices,
        dependencies_.popupServices,
        slot);
    RememberCreatedSlot(slot);
    return child;
}

void SaveLoadDialogChildObjectGraphRuntime::RememberCreatedSlot(
    const std::int32_t slot)
{
    if (pendingCreatedSlot_ != -1)
    {
        throw std::logic_error(
            "SaveLoad previous created child was not inserted");
    }
    pendingCreatedSlot_ = slot;
}
}
