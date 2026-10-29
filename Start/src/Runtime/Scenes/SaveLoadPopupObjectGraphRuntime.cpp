#include <Runtime/SaveLoadPopupObjectGraphRuntime.h>

#include <algorithm>
#include <array>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::string Cp932(const char* const value)
{
    return std::string(value);
}

[[nodiscard]] CgOperationControlResourceDescriptor ResourceDescriptor(
    const std::array<std::string, 7>& names)
{
    return CgOperationControlResourceDescriptor{names, std::nullopt};
}

[[nodiscard]] CgOperationChildDescriptor StaticDescriptor()
{
    const std::string name = Cp932(
        "\x83\x75\x83\x89\x83\x62\x83\x4E\x83\x41\x83\x45\x83\x67"
        "\x82\x50\x82\x55\x82\x4F\x2E\x62\x67\x64");
    return {CgOperationControlKind::Static, 55,
        ResourceDescriptor({name, {}, {}, name, name, {}, {}}), 7, -1};
}

[[nodiscard]] CgOperationChildDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const char* const normal,
    const char* const disabled,
    const char* const target,
    const char* const clicked)
{
    const std::string targetName = Cp932(target);
    return {CgOperationControlKind::Button, eventValue,
        ResourceDescriptor({Cp932(normal), Cp932(disabled), {}, targetName,
            targetName, Cp932(clicked), {}}), 0, -1};
}

[[nodiscard]] CgOperationChildDescriptor PageButtonDescriptor(
    const bool loadStyle,
    const bool lower)
{
    if (!loadStyle && !lower)
    {
        return ButtonDescriptor(58,
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    }
    if (!loadStyle && lower)
    {
        return ButtonDescriptor(59,
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    }
    if (loadStyle && !lower)
    {
        return ButtonDescriptor(58,
            "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x8F\xE3\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    }
    return ButtonDescriptor(59,
        "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
        "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x49\x83\x74\x2E\x76\x72\x6D",
        "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x5E\x83\x51\x2E\x76\x72\x6D",
        "\x4C\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6\x89\xBA\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
}

struct CheckBoxDescriptors final
{
    CgOperationControlResourceDescriptor unchecked;
    CgOperationControlResourceDescriptor checked;
};

[[nodiscard]] CheckBoxDescriptors CheckBoxResourceDescriptors(
    const bool loadStyle)
{
    if (!loadStyle)
    {
        const std::string target = Cp932(
            "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x5E\x83\x51\x2E\x76\x72\x6D");
        const std::string offTarget = Cp932(
            "\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x83\x5E\x83\x51\x2E\x76\x72\x6D");
        return {
            ResourceDescriptor({Cp932("\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D"),
                {}, {}, target, target,
                Cp932("\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"), {}}),
            ResourceDescriptor({Cp932("\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x2E\x76\x72\x6D"),
                {}, {}, offTarget, offTarget, offTarget, {}})};
    }

    const std::string target = Cp932(
        "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x5E\x83\x51\x2E\x76\x72\x6D");
    const std::string offTarget = Cp932(
        "\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x83\x5E\x83\x51\x2E\x76\x72\x6D");
    return {
        ResourceDescriptor({Cp932("\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D"),
            {}, {}, target, target,
            Cp932("\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"), {}}),
        ResourceDescriptor({Cp932("\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x7B\x83\x5E\x83\x93\x83\x49\x83\x74\x2E\x76\x72\x6D"),
            {}, {}, offTarget, offTarget, offTarget, {}})};
}

[[nodiscard]] CgOperationSliderDescriptor SliderDescriptor(
    const bool loadStyle)
{
    CgOperationSliderDescriptor result;
    result.eventValueAt00 = 57;
    const std::string normal = loadStyle
        ? Cp932("\x4C\x83\x58\x83\x89\x83\x43\x83\x68\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D")
        : Cp932("\x83\x58\x83\x89\x83\x43\x83\x68\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D");
    const std::string clicked = loadStyle
        ? Cp932("\x4C\x83\x58\x83\x89\x83\x43\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D")
        : Cp932("\x83\x58\x83\x89\x83\x43\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D");
    result.resourcesAt04And20[0].resourceNamesAt04 =
        {normal, {}, {}, clicked, clicked, clicked, {}};
    // 0x606580..0x606598 是七个空指针，表示没有独立轨道图层。
    result.hitTestResourceSlotAt40 = 0;
    result.orientationAt44 = 1;
    result.reverseAt48 = 0;
    result.rectAt4C = RECT{1214, 207, 1239, 605};
    result.offsetRectAt5C = RECT{0, 0, 0, -51};
    return result;
}

[[nodiscard]] std::vector<GuiDialogNavigationMapping> NavigationMappings(
    const std::int32_t input)
{
    std::vector<GuiDialogNavigationMapping> result;
    result.reserve(11u);
    if (input == 1)
    {
        result.push_back({60, -1});
        for (std::int32_t value = 61; value <= 70; ++value)
            result.push_back({value, value - 1});
    }
    else if (input == 2)
    {
        result.push_back({70, -1});
        for (std::int32_t value = 69; value >= 60; --value)
            result.push_back({value, value + 1});
    }
    else
    {
        throw std::invalid_argument("SaveLoadPopup navigation input is invalid");
    }
    // 0x504E3A 在 key < 0 时终止；源表的 {-1, 0} 不是运行时元素。
    return result;
}

[[nodiscard]] std::vector<std::string> DigitResourceNames(
    const bool loadStyle)
{
    constexpr std::array<const char*, 3> suffixes{
        "\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
        "\x83\x5E\x83\x51\x2E\x76\x72\x6D",
        "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"};
    std::vector<std::string> result;
    result.reserve(30u);
    for (const char* const suffix : suffixes)
    {
        for (std::int32_t digit = 0; digit < 10; ++digit)
        {
            result.push_back(
                std::string(loadStyle ? "Lp" : "p") +
                static_cast<char>('0' + digit) + suffix);
        }
    }
    return result;
}

[[nodiscard]] std::array<std::string, 3> MarkerResourceNames(
    const bool loadStyle)
{
    if (loadStyle)
    {
        return {Cp932("\x4C\x83\x7C\x83\x62\x83\x76\x4E\x45\x57\x2E\x76\x72\x6D"),
            Cp932("\x4C\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x81\x96\x2E\x76\x72\x6D"),
            Cp932("\x4C\x83\x5E\x83\x51\x81\x96\x2E\x76\x72\x6D")};
    }
    return {Cp932("\x83\x7C\x83\x62\x83\x76\x4E\x45\x57\x2E\x76\x72\x6D"),
        Cp932("\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x81\x96\x2E\x76\x72\x6D"),
        Cp932("\x83\x5E\x83\x51\x81\x96\x2E\x76\x72\x6D")};
}

[[nodiscard]] RECT MovedRect(const RECT& source, const POINT origin)
{
    return RECT{origin.x, origin.y,
        origin.x + source.right - source.left,
        origin.y + source.bottom - source.top};
}

class SaveLoadPopupEventTargetRuntime final
    : public ISaveLoadDialogEventTarget
{
public:
    explicit SaveLoadPopupEventTargetRuntime(CgOperationChildControlRef child)
        : child_(std::move(child))
    {
        if (!child_)
            throw std::invalid_argument("SaveLoadPopup event target is null");
    }

    void InvokeAtVslot08() override { child_->InvokeAtVslot08(); }
    [[nodiscard]] RECT ReadRectAtVslot3C() const override
    {
        return child_->ReadRectAtVslot3C();
    }

private:
    CgOperationChildControlRef child_;
};
}

SaveLoadPopupObjectGraphRuntime::SaveLoadPopupObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationButtonInputServices& buttonInputServices,
    ICgOperationSliderInputServices& sliderInputServices,
    ISaveLoadPopupInheritedServices& inheritedServices,
    const std::int32_t styleSlot)
    : graphics_(&graphics), inheritedServices_(&inheritedServices)
{
    if (styleSlot != 6 && styleSlot != 7)
        throw std::invalid_argument("SaveLoadPopup style slot is not 6 or 7");
    const bool loadStyle = styleSlot == 7;
    children_.reserve(15u);
    eventChildren_.reserve(13u);
    popupNumbersAtA0_.reserve(11u);

    const auto staticDescriptor = StaticDescriptor();
    auto staticResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            surfaceFactory, staticDescriptor.resources);
    auto background = std::make_shared<CgOperationChildControlRuntime>(
        staticDescriptor.kind, staticDescriptor.eventValueAt04,
        staticDescriptor.childIdAt0C,
        staticDescriptor.initialResourceSlotAt24, staticResources);
    AddChild(background);

    auto bar = surfaceFactory.LoadStatic(loadStyle
        ? Cp932("\x4C\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x6F\x81\x5B\x2E\x76\x72\x6D")
        : Cp932("\x83\x7C\x83\x62\x83\x76\x83\x41\x83\x62\x83\x76\x83\x6F\x81\x5B\x2E\x76\x72\x6D"));
    if (!bar)
        throw 1;
    static_cast<void>(graphics_->BlitSurface(
        staticResources->ResourceForSlot(0)->Handle(), bar->Handle(),
        1214, 207, nullptr, nullptr, nullptr, 0));
    const RECT barRect{1214, 207, 1214 + bar->Width(), 207 + bar->Height()};
    RECT withBar{};
    ::UnionRect(&withBar, &combinedRectAt60_, &barRect);
    combinedRectAt60_ = withBar;

    sliderAt57_ = CreateCgOperationSliderAt5052C0(
        surfaceFactory, sliderInputServices, SliderDescriptor(loadStyle));
    sliderAt57_->SetRangeAt537680(0, 39);
    sliderAt57_->SetValueAt538520(0);
    AddChild(sliderAt57_);

    auto upper = PageButtonDescriptor(loadStyle, false);
    auto upperResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            surfaceFactory, upper.resources);
    auto upperChild = std::make_shared<CgOperationChildControlRuntime>(
        upper.kind, upper.eventValueAt04, upper.childIdAt0C,
        upper.initialResourceSlotAt24, std::move(upperResources));
    upperChild->BindButtonInputServicesAt4C(buttonInputServices);
    upperChild->WritePositionAt34And38(POINT{1036, 189});
    std::int32_t currentY = upperChild->ReadRectAtVslot3C().bottom;
    eventChildren_.push_back(upperChild);
    AddChild(upperChild);

    const CheckBoxDescriptors checkDescriptors =
        CheckBoxResourceDescriptors(loadStyle);
    for (std::int32_t index = 0; index < 11; ++index)
    {
        auto unchecked =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, checkDescriptors.unchecked);
        auto checked =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, checkDescriptors.checked);
        const RECT uncheckedRect = unchecked->CombinedRect();
        const RECT checkedRect = checked->CombinedRect();
        const LONG width = std::max(
            uncheckedRect.right - uncheckedRect.left,
            checkedRect.right - checkedRect.left);
        const LONG height = std::max(
            uncheckedRect.bottom - uncheckedRect.top,
            checkedRect.bottom - checkedRect.top);
        const RECT controlRect{1036, currentY, 1036 + width, currentY + height};

        auto checkBox = std::make_shared<CgOperationChildControlRuntime>(
            CgOperationControlKind::CheckBox, 60 + index, -1, 3,
            std::move(unchecked), std::move(checked));
        checkBox->BindButtonInputServicesAt4C(buttonInputServices);
        checkBox->WriteRectAt34ForConstruction(controlRect);

        SaveLoadPopupNumberRuntime number;
        number.pageValueAt00 = 60 + index;
        number.originAt08 = POINT{controlRect.left, controlRect.top};
        for (auto& surface : number.surfacesAt10)
        {
            surface = surfaceFactory.CreateBlank(width, height, true, true);
            if (!surface)
                throw 1;
        }
        popupNumbersAtA0_.push_back(std::move(number));
        eventChildren_.push_back(checkBox);
        AddChild(checkBox);
        currentY += height;
    }

    auto lower = PageButtonDescriptor(loadStyle, true);
    auto lowerResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            surfaceFactory, lower.resources);
    auto lowerChild = std::make_shared<CgOperationChildControlRuntime>(
        lower.kind, lower.eventValueAt04, lower.childIdAt0C,
        lower.initialResourceSlotAt24, std::move(lowerResources));
    lowerChild->BindButtonInputServicesAt4C(buttonInputServices);
    lowerChild->WritePositionAt34And38(POINT{1036, currentY});
    eventChildren_.push_back(lowerChild);
    AddChild(lowerChild);

    clickedEventManagerAt70_ =
        CgOperationClickedEventManagerRuntime::
            CreateForSaveLoadPopupAt4EF660(eventChildren_);

    for (const auto& name : DigitResourceNames(loadStyle))
    {
        auto surface = surfaceFactory.LoadStatic(name);
        if (!surface)
            throw 1;
        digitResourcesAt88_.push_back(std::move(surface));
    }
    for (const auto& name : MarkerResourceNames(loadStyle))
    {
        auto surface = surfaceFactory.LoadStatic(name);
        if (!surface)
            throw 1;
        markerResourcesAt94_.push_back(std::move(surface));
    }

    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(
            1, NavigationMappings(1)));
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(
            2, NavigationMappings(2)));
}

std::int32_t SaveLoadPopupObjectGraphRuntime::StateAtVslot20() const
{
    return stateAt34_;
}

void SaveLoadPopupObjectGraphRuntime::WriteEnabledAtVslot24(
    const std::int32_t value)
{
    gateAt38_ = value;
}

std::int32_t SaveLoadPopupObjectGraphRuntime::GateAtVslot28() const
{
    return gateAt38_;
}

void SaveLoadPopupObjectGraphRuntime::UpdateAtVslot48()
{
    eventAt5C_ = -1;
    eventAt5CInitialized_ = true;
    if (inheritedServices_->ReadCancelRequestAt52FDA0())
    {
        eventAt5C_ = 53;
        return;
    }

    std::int32_t candidate = displayedFirstPageAt80_;
    inheritedServices_->UpdateGuiDialogAtVslot54();
    const std::int32_t clicked =
        clickedEventManagerAt70_->PollFirstAt50FDB0(1);
    if (clicked == 58)
    {
        --candidate;
    }
    else if (clicked == 59)
    {
        ++candidate;
    }
    else if (clicked >= 60 &&
             clicked < 60 + static_cast<std::int32_t>(popupNumbersAtA0_.size()))
    {
        const std::int32_t selection = clicked + displayedFirstPageAt80_ - 60;
        if (selection != selectedPageAt7C_)
        {
            eventAt5C_ = 54;
            pendingSelectedPageAt84_ = selection;
        }
    }
    else
    {
        const std::int32_t direction =
            inheritedServices_->ReadPageStepDirectionAtVslot78();
        if (direction > 0)
        {
            if (candidate > 0)
                --candidate;
        }
        else if (direction < 0)
        {
            if (candidate < 39)
                ++candidate;
        }
        else
        {
            candidate = inheritedServices_->ReadFallbackPageAt53DDE0Vslot60();
        }
    }

    if (displayedFirstPageAt80_ != candidate)
    {
        RefreshAt52B390(candidate, true);
        sliderAt57_->SetValueAt538520(candidate);
    }
}

void SaveLoadPopupObjectGraphRuntime::EndAtVslot4C()
{
    // vtable +0x4C = 0x5094C0，无状态写入。
}

std::int32_t SaveLoadPopupObjectGraphRuntime::EventAtVslot50() const
{
    if (!eventAt5CInitialized_)
        throw std::logic_error("SaveLoadPopup +0x5C has not been initialized");
    return eventAt5C_;
}

SaveLoadDialogEventTargetRef
SaveLoadPopupObjectGraphRuntime::ResolveEventTargetAtVslot14(
    const std::int32_t event)
{
    const auto child = ChildByEventValue(event);
    return child
        ? std::make_shared<SaveLoadPopupEventTargetRuntime>(child)
        : SaveLoadDialogEventTargetRef{};
}

void SaveLoadPopupObjectGraphRuntime::OpenAtVslot44()
{
    clickedEventManagerAt70_->ClearAllAt532200();
    for (const auto& child : children_)
        child->InvokeAtVslot08();
    if (selectedPageAt7C_ == displayedFirstPageAt80_)
        displayedFirstPageAt80_ = -1;
    RefreshAt52B390(selectedPageAt7C_, false);
    sliderAt57_->SetValueAt538520(selectedPageAt7C_);
    inheritedServices_->OpenGuiDialogAtVslot3C();
    valueAtB0_ = -1;
}

void SaveLoadPopupObjectGraphRuntime::RefreshAt52B390(
    const std::int32_t firstPage,
    const bool submitDirty)
{
    const std::int32_t clampedFirst = firstPage + 11 > 50 ? 39 : firstPage;
    const SaveFileCatalog& catalog =
        inheritedServices_->SaveFileCatalogAt4180C0();
    if (catalog.RegularPages().size() < 50u)
    {
        throw std::out_of_range(
            "SaveLoadPopup catalog has fewer than fifty regular pages");
    }

    constexpr std::array<POINT, 3> digitPositions{
        POINT{99, 4}, POINT{79, 4}, POINT{59, 4}};
    for (std::size_t index = 0; index < popupNumbersAtA0_.size(); ++index)
    {
        auto& number = popupNumbersAtA0_[index];
        for (const auto& surface : number.surfacesAt10)
        {
            graphics_->ClearSurface(
                surface->Handle(), surface->ClearColor(), nullptr);
        }

        const std::int32_t page =
            clampedFirst + static_cast<std::int32_t>(index);
        std::int32_t value = page + 1;
        for (std::size_t place = 0; place < 3u; ++place)
        {
            const std::int32_t digit = value % 10;
            value /= 10;
            for (std::size_t state = 0; state < 3u; ++state)
            {
                DrawWhole(*number.surfacesAt10[state],
                    *digitResourcesAt88_.at(state * 10u +
                        static_cast<std::size_t>(digit)),
                    digitPositions[place]);
            }
        }

        auto checkBox = ConcreteChildByEventValue(
            60 + static_cast<std::int32_t>(index));
        checkBox->WriteCheckBoxStateAt5037D0(
            selectedPageAt7C_ == page ? 1 : 0);

        if (catalog.RegularPages()[static_cast<std::size_t>(page)]
                .availabilityAt00 != 0)
        {
            const std::size_t marker =
                selectedPageAt7C_ == page ? 2u : 1u;
            for (const auto& surface : number.surfacesAt10)
            {
                DrawWhole(*surface, *markerResourcesAt94_[marker], POINT{31, 7});
            }
        }
        if (catalog.regularLatestPageAt18 == page)
        {
            for (const auto& surface : number.surfacesAt10)
            {
                DrawWhole(*surface, *markerResourcesAt94_[0], POINT{129, 1});
            }
        }
        number.pageValueAt00 = page;
    }

    displayedFirstPageAt80_ = clampedFirst;
    ConcreteChildByEventValue(58)->WriteAtVslot44(clampedFirst != 0 ? 1 : 0);
    ConcreteChildByEventValue(59)->WriteAtVslot44(clampedFirst != 39 ? 1 : 0);
    if (submitDirty)
        inheritedServices_->SubmitDirtyRectAtVslot64(combinedRectAt60_);
}

void SaveLoadPopupObjectGraphRuntime::DrawAt524A90(
    const RECT* const destinationClip)
{
    if (gateAt38_ != 0)
    {
        for (const auto& child : children_)
            static_cast<void>(child->DrawAtVslot0C(*graphics_, destinationClip));
    }
    if (GateAtVslot28() == 0)
        return;

    for (const auto& number : popupNumbersAtA0_)
    {
        const auto checkBox = ConcreteChildByEventValue(number.pageValueAt00 -
            displayedFirstPageAt80_ + 60);
        const std::int32_t state = checkBox->VisualStateAtVslot38();
        const bool checked = checkBox->CheckBoxStateAt5C() != 0;
        std::size_t surfaceIndex = 0;
        if (state == 0)
            surfaceIndex = checked ? 2u : 0u;
        else if (state == 3 || state == 4)
            surfaceIndex = checked ? 2u : 1u;
        else if (state == 5)
            surfaceIndex = 2u;
        else
            continue;
        static_cast<void>(graphics_->DrawSurface(
            number.surfacesAt10[surfaceIndex]->Handle(),
            number.originAt08.x, number.originAt08.y,
            nullptr, nullptr, destinationClip, 0));
    }
}

void SaveLoadPopupObjectGraphRuntime::CompositeAt5249C0(
    const GraphicsSurfaceHandle destination,
    const RECT* const destinationClip)
{
    if (gateAt38_ == 0)
        return;
    for (const auto& child : children_)
    {
        static_cast<void>(child->CompositeAtVslot10(
            *graphics_, destination, destinationClip));
    }
}

const std::vector<CgOperationChildControlRef>&
SaveLoadPopupObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::vector<CgOperationChildControlRef>&
SaveLoadPopupObjectGraphRuntime::EventChildrenInInsertionOrder() const noexcept
{
    return eventChildren_;
}

const std::vector<SaveLoadPopupNumberRuntime>&
SaveLoadPopupObjectGraphRuntime::PopupNumbersAtA0() const noexcept
{
    return popupNumbersAtA0_;
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadPopupObjectGraphRuntime::DigitResourcesAt88() const noexcept
{
    return digitResourcesAt88_;
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadPopupObjectGraphRuntime::MarkerResourcesAt94() const noexcept
{
    return markerResourcesAt94_;
}

const std::vector<GuiDialogNavigationGroupRef>&
SaveLoadPopupObjectGraphRuntime::NavigationGroupsAt24() const noexcept
{
    return navigationGroupsAt24_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
SaveLoadPopupObjectGraphRuntime::ClickedEventManagerAt70() const noexcept
{
    return clickedEventManagerAt70_;
}

const std::shared_ptr<CgOperationSliderRuntime>&
SaveLoadPopupObjectGraphRuntime::SliderAt57() const noexcept
{
    return sliderAt57_;
}

const RECT& SaveLoadPopupObjectGraphRuntime::CombinedRectAt60() const noexcept
{
    return combinedRectAt60_;
}

std::int32_t
SaveLoadPopupObjectGraphRuntime::SelectedPageAt7C() const noexcept
{
    return selectedPageAt7C_;
}

std::int32_t
SaveLoadPopupObjectGraphRuntime::DisplayedFirstPageAt80() const noexcept
{
    return displayedFirstPageAt80_;
}

std::int32_t
SaveLoadPopupObjectGraphRuntime::PendingSelectedPageAt84() const noexcept
{
    return pendingSelectedPageAt84_;
}

void SaveLoadPopupObjectGraphRuntime::WriteSelectedPageAt7C(
    const std::int32_t value) noexcept
{
    selectedPageAt7C_ = value;
}

void SaveLoadPopupObjectGraphRuntime::WriteStateAtVslot20(
    const std::int32_t value) noexcept
{
    stateAt34_ = value;
}

void SaveLoadPopupObjectGraphRuntime::AddChild(
    const CgOperationChildControlRef& child)
{
    if (!child)
        throw std::invalid_argument("SaveLoadPopup child is null");
    const RECT rect = child->ReadRectAtVslot3C();
    RECT combined{};
    ::UnionRect(&combined, &combinedRectAt60_, &rect);
    combinedRectAt60_ = combined;
    children_.push_back(child);
}

CgOperationChildControlRef
SaveLoadPopupObjectGraphRuntime::ChildByEventValue(
    const std::int32_t event) const
{
    const auto found = std::ranges::find_if(children_,
        [event](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == event;
        });
    return found == children_.end() ? nullptr : *found;
}

std::shared_ptr<CgOperationChildControlRuntime>
SaveLoadPopupObjectGraphRuntime::ConcreteChildByEventValue(
    const std::int32_t event) const
{
    auto child = std::dynamic_pointer_cast<CgOperationChildControlRuntime>(
        ChildByEventValue(event));
    if (!child)
        throw std::logic_error("SaveLoadPopup concrete child is missing");
    return child;
}

void SaveLoadPopupObjectGraphRuntime::DrawWhole(
    RuntimeGraphicsSurface& destination,
    const RuntimeGraphicsSurface& source,
    const POINT& point)
{
    static_cast<void>(graphics_->BlitSurface(
        destination.Handle(), source.Handle(), point.x, point.y,
        nullptr, nullptr, nullptr, 0));
}
}
