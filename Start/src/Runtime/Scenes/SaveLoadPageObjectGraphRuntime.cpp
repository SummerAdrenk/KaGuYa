#include <Runtime/SaveLoadPageObjectGraphRuntime.h>

#include <array>
#include <ctime>
#include <mbctype.h>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] std::string Cp932(const char* const value)
{
    return std::string(value);
}

[[nodiscard]] CgOperationChildDescriptor StaticDescriptor(
    const std::int32_t eventValue,
    const char* const resourceName)
{
    return CgOperationChildDescriptor{
        CgOperationControlKind::Static,
        eventValue,
        {{{Cp932(resourceName), {}, {}, Cp932(resourceName),
           Cp932(resourceName), {}, {}}}, std::nullopt},
        7,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const char* const normal,
    const char* const off,
    const char* const target,
    const char* const clicked,
    const std::int32_t childId)
{
    const std::string targetName = Cp932(target);
    return CgOperationChildDescriptor{
        CgOperationControlKind::Button,
        eventValue,
        {{{Cp932(normal), Cp932(off), {}, targetName, targetName,
           Cp932(clicked), {}}}, std::nullopt},
        0,
        childId};
}

[[nodiscard]] CgOperationChildDescriptor FileButtonDescriptor(
    const bool loadStyle)
{
    const auto make = [](const char* const normal, const char* const target) {
        const std::string targetName = Cp932(target);
        // 0x606310/0x6063EC：槽 1..3 为空，槽 4..6 共用 target。
        return CgOperationChildDescriptor{
            CgOperationControlKind::Button,
            25,
            {{{Cp932(normal), {}, {}, {}, targetName, targetName,
               targetName}}, std::nullopt},
            0,
            -1};
    };
    if (loadStyle)
    {
        return make(
            "\x4C\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x4C\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D");
    }
    return make(
        "\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
        "\x83\x74\x83\x40\x83\x43\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D");
}

[[nodiscard]] CgOperationChildDescriptor DeleteButtonDescriptor(
    const bool loadStyle)
{
    if (loadStyle)
    {
        return ButtonDescriptor(41,
            "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x49\x83\x74\x2E\x76\x72\x6D",
            "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x5E\x83\x51\x2E\x76\x72\x6D",
            "\x4C\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
            -1);
    }
    return ButtonDescriptor(41,
        "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
        "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x49\x83\x74\x2E\x76\x72\x6D",
        "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x5E\x83\x51\x2E\x76\x72\x6D",
        "\x83\x54\x83\x80\x83\x6C\x8D\xED\x8F\x9C\x81\x7E\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D",
        -1);
}

[[nodiscard]] std::array<CgOperationChildDescriptor, 3>
TopButtonDescriptors(const bool loadStyle)
{
    const char* const prefix = loadStyle ? "\x4C" : "";
    const auto withPrefix = [prefix](const char* const tail) {
        return std::string(prefix) + tail;
    };
    const auto make = [&withPrefix](
        const std::int32_t eventValue,
        const char* const stem,
        const std::int32_t childId) {
        const std::string normal = withPrefix(stem) +
            "\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
        const std::string off = withPrefix(stem) +
            "\x83\x49\x83\x74\x2E\x76\x72\x6D";
        const std::string target = withPrefix(stem) +
            "\x83\x5E\x83\x51\x2E\x76\x72\x6D";
        const std::string clicked = withPrefix(stem) +
            "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
        return ButtonDescriptor(eventValue, normal.c_str(), off.c_str(),
            target.c_str(), clicked.c_str(), childId);
    };
    return {{
        make(50, "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x8D\xB6", -1),
        make(51, "\x83\x5E\x83\x75\x88\xDA\x93\xAE\x89\x45", -1),
        make(52, "\x83\x79\x81\x5B\x83\x57\x95\x5C\x8E\xA6", 27),
    }};
}

[[nodiscard]] const char* BackgroundName(const SaveLoadPageKind kind)
{
    switch (kind)
    {
    case SaveLoadPageKind::Save:
        return "\x83\x5A\x81\x5B\x83\x75\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
    case SaveLoadPageKind::Load:
        return "\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
    case SaveLoadPageKind::QuickLoad:
        return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
    case SaveLoadPageKind::AutoLoad:
        return "\x83\x49\x81\x5B\x83\x67\x83\x8D\x81\x5B\x83\x68\x8A\xEE\x96\x7B\x2E\x76\x72\x6D";
    }
    throw std::invalid_argument("SaveLoadPage kind is outside 2..5");
}

constexpr std::array<POINT, 8> kFilePositions{{
    {159, 215}, {430, 215}, {701, 215}, {972, 215},
    {159, 418}, {430, 418}, {701, 418}, {972, 418},
}};

constexpr std::array<POINT, 6> kPageNumberPositions{{
    {604, 132}, {582, 132}, {560, 132},
    {710, 132}, {688, 132}, {666, 132},
}};

constexpr std::array<POINT, 13> kTimestampPositions{{
    {52, 166}, {64, 166}, {80, 166},
    {94, 166}, {106, 166}, {122, 166},
    {136, 166}, {148, 166},
    {177, 166}, {189, 166}, {202, 168},
    {209, 166}, {221, 166},
}};

constexpr SIZE kPreviewMaximumSize{236, 133};

template <std::size_t Count>
[[nodiscard]] std::vector<std::shared_ptr<RuntimeGraphicsSurface>>
LoadVisualGroup(
    ICgOperationSurfaceFactory& factory,
    const std::array<std::string, Count>& names)
{
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> result;
    result.reserve(Count);
    for (const auto& name : names)
    {
        auto resource = factory.LoadStatic(name);
        if (!resource)
            throw 1;
        result.push_back(std::move(resource));
    }
    return result;
}

[[nodiscard]] std::string TruncateSceneLabelAt4DA8E0(
    const std::string_view source)
{
    constexpr std::int32_t maximumWidth = 20;
    constexpr std::int32_t maximumLines = 1;
    std::string result;
    std::size_t position = 0;
    std::int32_t line = 0;
    std::int32_t width = 0;
    std::size_t previousCharacterBytes = 0;
    bool ellipsisWritten = false;

    while (position < source.size() && line < maximumLines)
    {
        const auto byte = static_cast<unsigned char>(source[position]);
        if (byte != '\r' && byte != '\n')
        {
            if (width >= maximumWidth)
            {
                if (!ellipsisWritten)
                {
                    result.resize(result.size() - previousCharacterBytes);
                    result.append("\x81\x63", 2);
                    ellipsisWritten = true;
                }
                ++position;
                continue;
            }

            if (_ismbblead(byte) != 0)
            {
                result.push_back(static_cast<char>(byte));
                result.push_back(
                    position + 1 < source.size() ? source[position + 1] : '\0');
                position += 2;
                width += 2;
                previousCharacterBytes = 2;
            }
            else
            {
                result.push_back(static_cast<char>(byte));
                ++position;
                ++width;
                previousCharacterBytes = 1;
            }
            continue;
        }

        result.push_back(static_cast<char>(byte));
        ++position;
        if (byte == '\n')
        {
            width = 0;
            ellipsisWritten = false;
            ++line;
        }
    }
    return result;
}

enum class LocalTimeField : std::uint8_t
{
    Year,
    Month,
    Day,
    Hour,
    Minute,
};

[[nodiscard]] std::int32_t ReadLocalTimeField(
    const std::uint64_t rawTime,
    const LocalTimeField field)
{
    const __time64_t time = static_cast<__time64_t>(rawTime);
    std::tm value{};
    if (_localtime64_s(&value, &time) != 0)
    {
        return field == LocalTimeField::Hour ||
            field == LocalTimeField::Minute ? -1 : 0;
    }

    switch (field)
    {
    case LocalTimeField::Year:
        return value.tm_year + 1900;
    case LocalTimeField::Month:
        return value.tm_mon + 1;
    case LocalTimeField::Day:
        return value.tm_mday;
    case LocalTimeField::Hour:
        return value.tm_hour;
    case LocalTimeField::Minute:
        return value.tm_min;
    }
    throw std::logic_error("Save/load local time field is invalid");
}

void BlitWhole(
    IGraphicsSurfaceCompositionApi& graphics,
    RuntimeGraphicsSurface& destination,
    const RuntimeGraphicsSurface& source,
    const POINT point)
{
    static_cast<void>(graphics.BlitSurface(
        destination.Handle(), source.Handle(),
        point.x, point.y, nullptr, nullptr, nullptr, 0));
}

[[nodiscard]] std::vector<GuiDialogNavigationMapping> NavigationMappings(
    const std::int32_t action)
{
    if (action == 3)
    {
        return {{25, 28}, {26, 25}, {27, 26}, {28, 27},
                {29, 32}, {30, 29}, {31, 30}, {32, 31}};
    }
    if (action == 4)
    {
        return {{25, 26}, {26, 27}, {27, 28}, {28, 25},
                {29, 30}, {30, 31}, {31, 32}, {32, 29}};
    }
    if (action == 1 || action == 2)
    {
        return {{25, 29}, {26, 30}, {27, 31}, {28, 32},
                {29, 25}, {30, 26}, {31, 27}, {32, 28}};
    }
    throw std::invalid_argument("SaveLoadPage navigation action is invalid");
}
}

SaveLoadDialogVisualResourcesRuntime::SaveLoadDialogVisualResourcesRuntime(
    ICgOperationSurfaceFactory& factory)
{
    const std::array<std::string, 10> pageNames{
        "page0.vrm", "page1.vrm", "page2.vrm", "page3.vrm", "page4.vrm",
        "page5.vrm", "page6.vrm", "page7.vrm", "page8.vrm", "page9.vrm"};
    const std::array<std::string, 16> fileNames{
        "f1.vrm", "f2.vrm", "f3.vrm", "f4.vrm",
        "f5.vrm", "f6.vrm", "f7.vrm", "f8.vrm",
        "Lf1.vrm", "Lf2.vrm", "Lf3.vrm", "Lf4.vrm",
        "Lf5.vrm", "Lf6.vrm", "Lf7.vrm", "Lf8.vrm"};
    const std::array<std::string, 24> dateNames{
        "y0.vrm", "y1.vrm", "y2.vrm", "y3.vrm", "y4.vrm", "y5.vrm",
        "y6.vrm", "y7.vrm", "y8.vrm", "y9.vrm",
        "\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D",
        "\x8E\xCE\x90\xFC\x2E\x76\x72\x6D",
        "Ly0.vrm", "Ly1.vrm", "Ly2.vrm", "Ly3.vrm", "Ly4.vrm", "Ly5.vrm",
        "Ly6.vrm", "Ly7.vrm", "Ly8.vrm", "Ly9.vrm",
        "\x4C\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D",
        "\x4C\x8E\xCE\x90\xFC\x2E\x76\x72\x6D"};
    const std::array<std::string, 2> noDataNames{
        "NODATA.vrm", "LNODATA.vrm"};
    const std::array<std::string, 2> fallbackNames{
        "\x96\xB3\x92\x6E\x2E\x76\x72\x6D",
        "\x4C\x96\xB3\x92\x6E\x2E\x76\x72\x6D"};
    const std::array<std::string, 2> newNames{"NEW.vrm", "LNEW.vrm"};

    pageDigitsAtF8_ = LoadVisualGroup(factory, pageNames);
    fileNumbersAt120_ = LoadVisualGroup(factory, fileNames);
    dateSymbolsAt12C_ = LoadVisualGroup(factory, dateNames);
    noDataAt138_ = LoadVisualGroup(factory, noDataNames);
    previewFallbackAt144_ = LoadVisualGroup(factory, fallbackNames);
    newMarkerAt150_ = LoadVisualGroup(factory, newNames);
}

const RuntimeGraphicsSurface& SaveLoadDialogVisualResourcesRuntime::PageDigit(
    const std::int32_t digit) const
{
    return *pageDigitsAtF8_.at(static_cast<std::size_t>(digit));
}

const RuntimeGraphicsSurface& SaveLoadDialogVisualResourcesRuntime::FileNumber(
    const std::int32_t slotIndex,
    const bool saveStyle) const
{
    const std::int32_t index = slotIndex + (saveStyle ? 0 : 8);
    return *fileNumbersAt120_.at(static_cast<std::size_t>(index));
}

const RuntimeGraphicsSurface& SaveLoadDialogVisualResourcesRuntime::DateSymbol(
    const std::int32_t symbol,
    const bool saveStyle) const
{
    const std::int32_t index = symbol + (saveStyle ? 0 : 12);
    return *dateSymbolsAt12C_.at(static_cast<std::size_t>(index));
}

const RuntimeGraphicsSurface& SaveLoadDialogVisualResourcesRuntime::NoData(
    const bool saveStyle) const
{
    return *noDataAt138_.at(saveStyle ? 0u : 1u);
}

const RuntimeGraphicsSurface&
SaveLoadDialogVisualResourcesRuntime::PreviewFallback(
    const bool saveStyle) const
{
    return *previewFallbackAt144_.at(saveStyle ? 0u : 1u);
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadDialogVisualResourcesRuntime::PreviewFallbackRef(
    const bool saveStyle) const
{
    return previewFallbackAt144_.at(saveStyle ? 0u : 1u);
}

const RuntimeGraphicsSurface& SaveLoadDialogVisualResourcesRuntime::NewMarker(
    const bool saveStyle) const
{
    return *newMarkerAt150_.at(saveStyle ? 0u : 1u);
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadDialogVisualResourcesRuntime::PageDigitsAtF8() const noexcept
{
    return pageDigitsAtF8_;
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadDialogVisualResourcesRuntime::FileNumbersAt120() const noexcept
{
    return fileNumbersAt120_;
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadDialogVisualResourcesRuntime::DateSymbolsAt12C() const noexcept
{
    return dateSymbolsAt12C_;
}

void SaveLoadDialogVisualResourcesRuntime::ReleaseAt4F9030() noexcept
{
    // 4F90E8..4F9193：五组高位 vector 逆字段顺序释放，页码 +0xF8 最后。
    newMarkerAt150_.clear();
    previewFallbackAt144_.clear();
    noDataAt138_.clear();
    dateSymbolsAt12C_.clear();
    fileNumbersAt120_.clear();
    pageDigitsAtF8_.clear();
}

SaveLoadPageObjectGraphRuntime::SaveLoadPageObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    ICgOperationButtonInputServices& buttonInputServices,
    const RECT& dialogBounds,
    const SaveLoadPageKind kind,
    const std::int32_t initialPage)
    : kind_(kind), currentPageAt7C_(initialPage)
{
    static_cast<void>(BackgroundName(kind));
    const bool fullPage =
        kind == SaveLoadPageKind::Save || kind == SaveLoadPageKind::Load;
    const bool loadStyle = kind != SaveLoadPageKind::Save;
    children_.reserve(fullPage ? 29u : 17u);
    eventChildren_.reserve(fullPage ? 19u : 8u);
    displaySurfacesAt8C_.reserve(8u);

    const auto backgroundDescriptor = StaticDescriptor(24, BackgroundName(kind));
    auto backgroundResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            surfaceFactory, backgroundDescriptor.resources);
    AddChild(std::make_shared<CgOperationChildControlRuntime>(
        backgroundDescriptor.kind, backgroundDescriptor.eventValueAt04,
        backgroundDescriptor.childIdAt0C,
        backgroundDescriptor.initialResourceSlotAt24,
        std::move(backgroundResources)));

    if (fullPage)
    {
        fullPageSurfaceAt80_ = surfaceFactory.CreateBlank(
            dialogBounds.right - dialogBounds.left,
            dialogBounds.bottom - dialogBounds.top,
            true,
            true);
        if (!fullPageSurfaceAt80_)
            throw 0;
        fullPageSurfaceAt80_->SetOrigin(dialogBounds.left, dialogBounds.top);
        auto fullPageResources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                fullPageSurfaceAt80_, 0u);
        AddChild(std::make_shared<CgOperationChildControlRuntime>(
            CgOperationControlKind::Static, 49, -1, 7,
            std::move(fullPageResources)));

        for (const auto& descriptor : TopButtonDescriptors(loadStyle))
        {
            auto child = CreateButton(
                surfaceFactory, buttonInputServices, descriptor);
            eventChildren_.push_back(child);
            AddChild(std::move(child));
        }
    }

    const auto fileDescriptor = FileButtonDescriptor(loadStyle);
    for (std::size_t index = 0; index < kFilePositions.size(); ++index)
    {
        auto descriptor = fileDescriptor;
        descriptor.eventValueAt04 += static_cast<std::int32_t>(index);
        auto child = CreateButton(
            surfaceFactory, buttonInputServices, descriptor);
        child->WritePositionAt34And38(kFilePositions[index]);
        eventChildren_.push_back(child);
        AddChild(std::move(child));
    }

    for (std::size_t index = 0; index < kFilePositions.size(); ++index)
    {
        const RECT fileRect = children_.at(
            (fullPage ? 5u : 1u) + index)->ReadRectAtVslot3C();
        auto surface = surfaceFactory.CreateBlank(
            fileRect.right - fileRect.left,
            fileRect.bottom - fileRect.top,
            true,
            true);
        // 4EDD30/4EF354：八张显示 Surface 的单项创建失败只跳过该项。
        if (!surface)
            continue;
        surface->SetOrigin(fileRect.left, fileRect.top);
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(surface, 0u);
        AddChild(std::make_shared<CgOperationChildControlRuntime>(
            CgOperationControlKind::Static,
            33 + static_cast<std::int32_t>(index),
            -1,
            7,
            std::move(resources)));
        displaySurfacesAt8C_.push_back(std::move(surface));
    }

    if (fullPage)
    {
        const auto deleteDescriptor = DeleteButtonDescriptor(loadStyle);
        for (std::size_t index = 0; index < kFilePositions.size(); ++index)
        {
            auto descriptor = deleteDescriptor;
            descriptor.eventValueAt04 += static_cast<std::int32_t>(index);
            auto child = CreateButton(
                surfaceFactory, buttonInputServices, descriptor);
            child->WritePositionAt34And38(POINT{
                kFilePositions[index].x + 10,
                kFilePositions[index].y + 140});
            eventChildren_.push_back(child);
            AddChild(std::move(child));
        }
    }

    clickedEventManagerAt70_ =
        CgOperationClickedEventManagerRuntime::CreateForSaveLoadPageAt4ED190(
            eventChildren_);
    for (const std::int32_t action : {3, 4, 1, 2})
    {
        navigationGroupsAt24_.push_back(
            std::make_shared<GuiDialogNavigationGroupRuntime>(
                action, NavigationMappings(action)));
    }
}

SaveLoadPageKind SaveLoadPageObjectGraphRuntime::Kind() const noexcept
{
    return kind_;
}

const std::vector<CgOperationChildControlRef>&
SaveLoadPageObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::vector<CgOperationChildControlRef>&
SaveLoadPageObjectGraphRuntime::EventChildrenInInsertionOrder() const noexcept
{
    return eventChildren_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
SaveLoadPageObjectGraphRuntime::ClickedEventManagerAt70() const noexcept
{
    return clickedEventManagerAt70_;
}

const std::vector<GuiDialogNavigationGroupRef>&
SaveLoadPageObjectGraphRuntime::NavigationGroupsAt24() const noexcept
{
    return navigationGroupsAt24_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadPageObjectGraphRuntime::FullPageSurfaceAt80() const noexcept
{
    return fullPageSurfaceAt80_;
}

const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>&
SaveLoadPageObjectGraphRuntime::DisplaySurfacesAt8C() const noexcept
{
    return displaySurfacesAt8C_;
}

const RECT& SaveLoadPageObjectGraphRuntime::CombinedRectAt60() const noexcept
{
    return combinedRectAt60_;
}

std::int32_t SaveLoadPageObjectGraphRuntime::CurrentPageAt7C() const noexcept
{
    return currentPageAt7C_;
}

void SaveLoadPageObjectGraphRuntime::WriteCurrentPageAt7C(
    const std::int32_t value) noexcept
{
    currentPageAt7C_ = value;
}

void SaveLoadPageObjectGraphRuntime::RefreshAtVslot8C(
    const std::int32_t pageIndex,
    const bool submitDirty,
    const bool force,
    IGraphicsSurfaceCompositionApi& graphics,
    const SaveLoadDialogVisualResourcesRuntime& visuals,
    ISaveLoadPageRefreshServices& services)
{
    if (!force && currentPageAt7C_ == pageIndex)
        return;

    const bool regular =
        kind_ == SaveLoadPageKind::Save || kind_ == SaveLoadPageKind::Load;
    if (regular)
    {
        if (!fullPageSurfaceAt80_)
            throw std::logic_error("Regular SaveLoad page has no full-page surface");
        graphics.ClearSurface(
            fullPageSurfaceAt80_->Handle(),
            fullPageSurfaceAt80_->ClearColor(),
            nullptr);
        DrawPageNumber(graphics, visuals, pageIndex + 1, 0u);
        DrawPageNumber(graphics, visuals, 50, 3u);
    }

    const SaveFileCatalog& catalog = services.SaveFileCatalogAt4180C0();
    const SaveFileCatalogPage* page = nullptr;
    SaveLoadFileFactoryOwner factory;
    std::int32_t latestItem = -1;
    std::int32_t latestPage = pageIndex;

    switch (kind_)
    {
    case SaveLoadPageKind::Save:
    case SaveLoadPageKind::Load:
        if (pageIndex < 0 ||
            static_cast<std::size_t>(pageIndex) >=
                catalog.RegularPages().size())
        {
            throw std::out_of_range("Regular SaveLoad page index is invalid");
        }
        page = &catalog.RegularPages()[static_cast<std::size_t>(pageIndex)];
        factory = services.CreateRegularFactoryAt4DCDB0(pageIndex + 1);
        latestPage = catalog.regularLatestPageAt18;
        latestItem = catalog.regularLatestItemAt1C;
        break;
    case SaveLoadPageKind::QuickLoad:
        page = &catalog.QuickPage();
        factory = services.CreateQuickFactoryAt4DC5F0();
        latestItem = catalog.quickLatestItemAt04;
        break;
    case SaveLoadPageKind::AutoLoad:
        page = &catalog.AutoPage();
        factory = services.CreateAutoFactoryAt4DA870();
        latestItem = catalog.autoLatestItemAt08;
        break;
    }

    if (!page || !factory)
        throw std::logic_error("SaveLoad page data source is null");
    if (page->Items().size() < 8u)
        throw std::out_of_range("SaveLoad catalog page has fewer than eight slots");

    for (std::int32_t slot = 0; slot < 8; ++slot)
    {
        const bool latest =
            latestPage == pageIndex && latestItem == slot;
        DrawSlotAt52A470(
            slot,
            page->Items()[static_cast<std::size_t>(slot)],
            *factory,
            latest,
            graphics,
            visuals,
            services);
    }

    if (regular)
    {
        if (const auto previous = ChildByEventValue(50))
            previous->WriteAtVslot44(pageIndex != 0 ? 1 : 0);
        if (const auto next = ChildByEventValue(51))
            next->WriteAtVslot44(pageIndex != 49 ? 1 : 0);
    }

    currentPageAt7C_ = pageIndex;
    if (submitDirty)
        services.SubmitPageRectAtVslot64(combinedRectAt60_);
}

void SaveLoadPageObjectGraphRuntime::DrawPageNumber(
    IGraphicsSurfaceCompositionApi& graphics,
    const SaveLoadDialogVisualResourcesRuntime& visuals,
    const std::int32_t pageNumber,
    const std::size_t coordinateBase)
{
    if (!fullPageSurfaceAt80_)
        throw std::logic_error("SaveLoad page number target is null");
    std::int32_t remaining = pageNumber;
    for (std::size_t index = 0; index < 3u; ++index)
    {
        const std::int32_t digit = remaining % 10;
        remaining /= 10;
        BlitWhole(
            graphics,
            *fullPageSurfaceAt80_,
            visuals.PageDigit(digit),
            kPageNumberPositions.at(coordinateBase + index));
    }
}

void SaveLoadPageObjectGraphRuntime::DrawSlotAt52A470(
    const std::int32_t slotIndex,
    const SaveFileCatalogItem& item,
    ISaveLoadFileFactory& factory,
    const bool latest,
    IGraphicsSurfaceCompositionApi& graphics,
    const SaveLoadDialogVisualResourcesRuntime& visuals,
    ISaveLoadPageRefreshServices& services)
{
    if (slotIndex < 0 ||
        static_cast<std::size_t>(slotIndex) >= displaySurfacesAt8C_.size())
    {
        throw std::out_of_range("SaveLoad display surface index is invalid");
    }
    auto& destination = *displaySurfacesAt8C_[static_cast<std::size_t>(slotIndex)];
    const bool saveStyle = kind_ == SaveLoadPageKind::Save;

    graphics.ClearSurface(
        destination.Handle(), destination.ClearColor(), nullptr);
    BlitWhole(
        graphics, destination, visuals.FileNumber(slotIndex, saveStyle),
        POINT{14, 167});

    bool available = false;
    if (item.Available())
    {
        const std::array<std::int32_t, 5> values{
            ReadLocalTimeField(item.LastWriteTime(), LocalTimeField::Year) % 100,
            ReadLocalTimeField(item.LastWriteTime(), LocalTimeField::Month),
            ReadLocalTimeField(item.LastWriteTime(), LocalTimeField::Day),
            ReadLocalTimeField(item.LastWriteTime(), LocalTimeField::Hour),
            ReadLocalTimeField(item.LastWriteTime(), LocalTimeField::Minute)};
        const std::array<std::int32_t, 13> symbols{
            values[0] / 10, values[0] % 10, 11,
            values[1] / 10, values[1] % 10, 11,
            values[2] / 10, values[2] % 10,
            values[3] / 10, values[3] % 10, 10,
            values[4] / 10, values[4] % 10};
        for (std::size_t index = 0; index < symbols.size(); ++index)
        {
            // 50B380 对超出 0..11 的索引返回空引用，随后 SurfaceBlt 不绘制。
            if (symbols[index] < 0 || symbols[index] >= 12)
                continue;
            BlitWhole(
                graphics,
                destination,
                visuals.DateSymbol(symbols[index], saveStyle),
                kTimestampPositions[index]);
        }

        auto preview = services.CreatePreviewSurfaceAt40C2E0();
        const std::string previewName =
            factory.BuildPreviewFileNameAt47AFA0(slotIndex + 1);
        const std::string previewPath =
            services.BuildPreviewPathAt4C7210(previewName);
        const bool previewLoaded =
            services.TryLoadPreviewAt511D90(preview, previewPath);
        const RuntimeGraphicsSurface& previewSource =
            previewLoaded && preview
                ? *preview
                : visuals.PreviewFallback(saveStyle);
        const SIZE* previewSize =
            previewSource.Width() > kPreviewMaximumSize.cx ||
            previewSource.Height() > kPreviewMaximumSize.cy
                ? &kPreviewMaximumSize
                : nullptr;
        static_cast<void>(graphics.BlitSurface(
            destination.Handle(), previewSource.Handle(),
            10, 5, previewSize, nullptr, nullptr, 0));

        services.DrawSceneLabelAt528D90(
            destination,
            TruncateSceneLabelAt4DA8E0(RuntimeStringBorrow(item.labelAt0C).Snapshot()),
            saveStyle);
        if (latest)
        {
            BlitWhole(
                graphics,
                destination,
                visuals.NewMarker(saveStyle),
                POINT{2, 2});
        }
        available = true;
    }
    else
    {
        BlitWhole(
            graphics,
            destination,
            visuals.NoData(saveStyle),
            POINT{10, 5});
    }

    if (const auto deleteButton = ChildByEventValue(slotIndex + 41))
        deleteButton->WriteAtVslot44(available ? 1 : 0);
}

CgOperationChildControlRef SaveLoadPageObjectGraphRuntime::ChildByEventValue(
    const std::int32_t eventValue) const
{
    const auto found = std::ranges::find_if(
        eventChildren_,
        [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    return found == eventChildren_.end() ? nullptr : *found;
}

void SaveLoadPageObjectGraphRuntime::AddChild(
    const CgOperationChildControlRef& child)
{
    if (!child)
        throw std::invalid_argument("SaveLoadPage child is null");
    const RECT rect = child->ReadRectAtVslot3C();
    RECT combined{};
    ::UnionRect(&combined, &combinedRectAt60_, &rect);
    combinedRectAt60_ = combined;
    children_.push_back(child);
}

std::shared_ptr<CgOperationChildControlRuntime>
SaveLoadPageObjectGraphRuntime::CreateButton(
    ICgOperationSurfaceFactory& surfaceFactory,
    ICgOperationButtonInputServices& buttonInputServices,
    const CgOperationChildDescriptor& descriptor)
{
    auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(
        surfaceFactory, descriptor.resources);
    auto child = std::make_shared<CgOperationChildControlRuntime>(
        descriptor.kind,
        descriptor.eventValueAt04,
        descriptor.childIdAt0C,
        descriptor.initialResourceSlotAt24,
        std::move(resources));
    child->BindButtonInputServicesAt4C(buttonInputServices);
    return child;
}

namespace
{
class SaveLoadPageEventTargetRuntime final
    : public ISaveLoadDialogEventTarget
{
public:
    explicit SaveLoadPageEventTargetRuntime(CgOperationChildControlRef child)
        : child_(std::move(child))
    {
        if (!child_)
            throw std::invalid_argument("SaveLoadPage event target is null");
    }

    void InvokeAtVslot08() override { child_->InvokeAtVslot08(); }
    [[nodiscard]] RECT ReadRectAtVslot3C() const override
    {
        return child_->ReadRectAtVslot3C();
    }

private:
    CgOperationChildControlRef child_;
};

[[nodiscard]] std::int32_t InitialPageForKind(
    const SaveLoadPageKind kind,
    const SaveFileCatalog& catalog)
{
    switch (kind)
    {
    case SaveLoadPageKind::Save:
        return catalog.regularSelectedPageAt0C;
    case SaveLoadPageKind::Load:
        return catalog.regularDisplayedPageAt10;
    case SaveLoadPageKind::QuickLoad:
    case SaveLoadPageKind::AutoLoad:
        return 0;
    }
    throw std::invalid_argument("SaveLoadPage kind is invalid");
}
}

SaveLoadPageDialogChildRuntime::SaveLoadPageDialogChildRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    ICgOperationButtonInputServices& buttonInputServices,
    const RECT& dialogBounds,
    const SaveLoadPageKind kind,
    IGraphicsSurfaceCompositionApi& graphics,
    const SaveLoadDialogVisualResourcesRuntime& visuals,
    ISaveLoadPageInheritedServices& inheritedServices)
    : graphics_(&graphics),
      visuals_(&visuals),
      inheritedServices_(&inheritedServices),
      graph_(surfaceFactory, buttonInputServices, dialogBounds, kind,
          InitialPageForKind(
              kind, inheritedServices.SaveFileCatalogAt4180C0()))
{
}

std::int32_t SaveLoadPageDialogChildRuntime::StateAtVslot20() const
{
    return stateAt34_;
}

void SaveLoadPageDialogChildRuntime::WriteEnabledAtVslot24(
    const std::int32_t value)
{
    gateAt38_ = value;
}

std::int32_t SaveLoadPageDialogChildRuntime::GateAtVslot28() const
{
    return gateAt38_;
}

void SaveLoadPageDialogChildRuntime::UpdateAtVslot48()
{
    // 0x52F8DD：每帧先清 +0x5C，再进入 GuiDialog 公共更新。
    eventAt5C_ = -1;
    eventAt5CInitialized_ = true;
    inheritedServices_->UpdateGuiDialogAtVslot54();

    for (;;)
    {
        const std::int32_t clicked =
            graph_.ClickedEventManagerAt70()->PollFirstAt50FDB0(1);
        if (clicked < 0)
            break;

        if (clicked >= 25 && clicked <= 32)
        {
            // Save 页允许选择空槽；Load/Quick/Auto 只有对应 catalog
            // 项真实存在时才发布事件。
            if (graph_.Kind() == SaveLoadPageKind::Save ||
                IsFileSlotAvailable(clicked - 25))
            {
                eventAt5C_ = clicked;
            }
            continue;
        }

        if (clicked >= 41 && clicked <= 48)
        {
            eventAt5C_ = clicked;
            continue;
        }

        switch (clicked)
        {
        case 50:
            graph_.RefreshAtVslot8C(
                graph_.CurrentPageAt7C() - 1,
                true,
                false,
                *graphics_,
                *visuals_,
                *inheritedServices_);
            break;
        case 51:
            graph_.RefreshAtVslot8C(
                graph_.CurrentPageAt7C() + 1,
                true,
                false,
                *graphics_,
                *visuals_,
                *inheritedServices_);
            break;
        case 52:
            eventAt5C_ = 52;
            break;
        default:
            break;
        }
    }
}

void SaveLoadPageDialogChildRuntime::EndAtVslot4C()
{
    switch (graph_.Kind())
    {
    case SaveLoadPageKind::Save:
        inheritedServices_->WriteRegularSelectedPageAt0C(
            graph_.CurrentPageAt7C());
        break;
    case SaveLoadPageKind::Load:
        inheritedServices_->WriteRegularDisplayedPageAt10(
            graph_.CurrentPageAt7C());
        break;
    case SaveLoadPageKind::QuickLoad:
    case SaveLoadPageKind::AutoLoad:
        break;
    }
}

std::int32_t SaveLoadPageDialogChildRuntime::EventAtVslot50() const
{
    if (!eventAt5CInitialized_)
        throw std::logic_error("SaveLoadPage +0x5C has not been initialized");
    return eventAt5C_;
}

SaveLoadDialogEventTargetRef
SaveLoadPageDialogChildRuntime::ResolveEventTargetAtVslot14(
    const std::int32_t event)
{
    const auto& children = graph_.EventChildrenInInsertionOrder();
    const auto found = std::ranges::find_if(
        children,
        [event](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == event;
        });
    return found == children.end()
        ? SaveLoadDialogEventTargetRef{}
        : std::make_shared<SaveLoadPageEventTargetRuntime>(*found);
}

void SaveLoadPageDialogChildRuntime::OpenAtVslot44()
{
    graph_.ClickedEventManagerAt70()->ClearAllAt532200();
    for (const auto& child : graph_.ChildrenInInsertionOrder())
        child->InvokeAtVslot08();
    inheritedServices_->ClearActiveControlAtVslot30();
    inheritedServices_->OpenGuiDialogAtVslot3C();
    graph_.RefreshAtVslot8C(
        graph_.CurrentPageAt7C(),
        false,
        true,
        *graphics_,
        *visuals_,
        *inheritedServices_);
}

void SaveLoadPageDialogChildRuntime::WriteStateAtVslot20(
    const std::int32_t value) noexcept
{
    stateAt34_ = value;
}

void SaveLoadPageDialogChildRuntime::WriteCurrentPageAt7C(
    const std::int32_t value) noexcept
{
    graph_.WriteCurrentPageAt7C(value);
}

void SaveLoadPageDialogChildRuntime::NotifySelectionAtVslot8C(
    const std::int32_t previousValue,
    const std::int32_t argument1,
    const std::int32_t argument2)
{
    graph_.RefreshAtVslot8C(
        previousValue,
        argument1 != 0,
        argument2 != 0,
        *graphics_,
        *visuals_,
        *inheritedServices_);
}

SaveLoadPageObjectGraphRuntime&
SaveLoadPageDialogChildRuntime::Graph() noexcept
{
    return graph_;
}

const SaveLoadPageObjectGraphRuntime&
SaveLoadPageDialogChildRuntime::Graph() const noexcept
{
    return graph_;
}

bool SaveLoadPageDialogChildRuntime::IsFileSlotAvailable(
    const std::int32_t slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= 8)
        throw std::out_of_range("SaveLoadPage file slot is invalid");

    const SaveFileCatalog& catalog =
        inheritedServices_->SaveFileCatalogAt4180C0();
    const SaveFileCatalogPage* page = nullptr;
    switch (graph_.Kind())
    {
    case SaveLoadPageKind::Save:
        return true;
    case SaveLoadPageKind::Load:
    {
        const std::int32_t pageIndex = graph_.CurrentPageAt7C();
        if (pageIndex < 0 ||
            static_cast<std::size_t>(pageIndex) >=
                catalog.RegularPages().size())
        {
            throw std::out_of_range(
                "SaveLoadPage regular page index is invalid");
        }
        page = &catalog.RegularPages()[
            static_cast<std::size_t>(pageIndex)];
        break;
    }
    case SaveLoadPageKind::QuickLoad:
        page = &catalog.QuickPage();
        break;
    case SaveLoadPageKind::AutoLoad:
        page = &catalog.AutoPage();
        break;
    }

    if (!page || page->Items().size() < 8u)
        throw std::out_of_range("SaveLoadPage catalog has fewer than eight slots");
    return page->Items()[static_cast<std::size_t>(slotIndex)].Available();
}
}
