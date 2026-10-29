#include <Runtime/SaveLoadSlotConfirmationRuntime.h>

#include <algorithm>
#include <array>
#include <map>
#include <ranges>
#include <stdexcept>
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

[[nodiscard]] std::string TrimSpacesAndTabs(std::string value)
{
    const auto isTrimByte = [](const char value) {
        return value == ' ' || value == '\t';
    };
    const auto first = std::find_if_not(value.begin(), value.end(), isTrimByte);
    const auto last = std::find_if_not(value.rbegin(), value.rend(), isTrimByte).base();
    if (first >= last)
        return {};
    return std::string(first, last);
}

[[nodiscard]] CgOperationChildDescriptor StaticDescriptor(
    const std::int32_t eventValue,
    const char* const resourceName)
{
    const std::string name = Cp932(resourceName);
    return {
        CgOperationControlKind::Static,
        eventValue,
        {{{name, {}, {}, name, name, {}, {}}}, std::nullopt},
        0,
        -1};
}

[[nodiscard]] CgOperationChildDescriptor ButtonDescriptor(
    const std::int32_t eventValue,
    const char* const normal,
    const char* const target,
    const char* const clicked)
{
    const std::string normalName = Cp932(normal);
    const std::string targetName = Cp932(target);
    return {
        CgOperationControlKind::Button,
        eventValue,
        {{{normalName, {}, {}, targetName, targetName, Cp932(clicked), {}}},
         std::nullopt},
        0,
        -1};
}

[[nodiscard]] const std::array<CgOperationChildDescriptor, 7>&
ChildDescriptorsAt603C28And603CC0()
{
    static const std::array<CgOperationChildDescriptor, 7> descriptors{{
        StaticDescriptor(0,
            "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x2E\x76\x72\x6D"),
        StaticDescriptor(1,
            "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x2E\x76\x72\x6D"),
        StaticDescriptor(2,
            "\x8F\xE3\x8F\x91\x82\xAB\x8A\x6D\x94\x46\x2E\x76\x72\x6D"),
        ButtonDescriptor(3,
            "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
        ButtonDescriptor(4,
            "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
        ButtonDescriptor(5,
            "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
        ButtonDescriptor(6,
            "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D",
            "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
    }};
    return descriptors;
}

[[nodiscard]] std::vector<std::string> SlotResourceNamesAt605E48()
{
    std::vector<std::string> names(20u);
    for (std::int32_t digit = 1; digit <= 8; ++digit)
    {
        names[static_cast<std::size_t>(digit)] =
            "file_" + std::to_string(digit) + ".vrm";
        names[static_cast<std::size_t>(digit + 10)] =
            "Lfile_" + std::to_string(digit) + ".vrm";
    }
    return names;
}

[[nodiscard]] std::vector<std::string> DecimalResourceNames(
    const char family,
    const bool includeSeparator,
    const char* const separator,
    const char* const largeSeparator)
{
    const std::size_t groupSize = includeSeparator ? 11u : 10u;
    std::vector<std::string> names(groupSize * 2u);
    for (std::int32_t digit = 0; digit < 10; ++digit)
    {
        const std::string suffix =
            std::string("file_") + family + "_" +
            std::to_string(digit) + ".vrm";
        names[static_cast<std::size_t>(digit)] = suffix;
        names[groupSize + static_cast<std::size_t>(digit)] = "L" + suffix;
    }
    if (includeSeparator)
    {
        names[10] = Cp932(separator);
        names[groupSize + 10u] = Cp932(largeSeparator);
    }
    return names;
}

[[nodiscard]] std::vector<std::shared_ptr<RuntimeGraphicsSurface>>
LoadResourceTable(
    ICgOperationSurfaceFactory& factory,
    const std::vector<std::string>& names)
{
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> result;
    result.reserve(names.size());
    for (const auto& name : names)
    {
        if (name.empty())
        {
            result.push_back({});
            continue;
        }
        auto resource = factory.LoadStatic(name);
        if (!resource)
            throw 1;
        result.push_back(std::move(resource));
    }
    return result;
}

[[nodiscard]] std::int32_t DigitIndex(const char value) noexcept
{
    return value >= '0' && value <= '9' ? value - '0' : -1;
}

[[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> SelectResource(
    const std::vector<std::shared_ptr<RuntimeGraphicsSurface>>& resources,
    const std::size_t localIndex,
    const std::size_t groupSize,
    const bool saveMode)
{
    const std::size_t index = localIndex + (saveMode ? 0u : groupSize);
    return resources.at(index);
}

constexpr std::array<std::array<std::int32_t, 4>, 3> kDisabledChildrenByMode{{
    {{1, 2, 5, 6}},
    {{0, 2, 3, 4}},
    {{0, 1, 3, 4}},
}};
constexpr std::array<POINT, 3> kPageCoordinates{{
    {188, 77}, {161, 77}, {134, 77}}};
constexpr std::array<POINT, 8> kDateCoordinates{{
    {113, 139}, {129, 139}, {145, 137}, {158, 139},
    {174, 139}, {190, 137}, {203, 139}, {219, 139}}};
constexpr std::array<POINT, 5> kTimeCoordinates{{
    {118, 178}, {141, 178}, {170, 182}, {185, 178}, {208, 178}}};
}

SaveLoadSlotConfirmationObjectGraphRuntime::
SaveLoadSlotConfirmationObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationButtonInputServices& buttonInputServices,
    ISaveLoadSlotConfirmationRenderServices& renderServices)
    : surfaceFactory_(&surfaceFactory),
      graphics_(&graphics),
      renderServices_(&renderServices)
{
    RECT combined{};
    children_.reserve(7u);
    clickedEventsAt74_.reserve(7u);
    for (const auto& descriptor : ChildDescriptorsAt603C28And603CC0())
    {
        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, descriptor.resources);
        auto child = std::make_shared<CgOperationChildControlRuntime>(
            descriptor.kind,
            descriptor.eventValueAt04,
            descriptor.childIdAt0C,
            descriptor.initialResourceSlotAt24,
            std::move(resources));
        if (descriptor.kind == CgOperationControlKind::Button)
            child->BindButtonInputServicesAt4C(buttonInputServices);

        const RECT rect = child->ReadRectAtVslot3C();
        ::UnionRect(&combined, &combined, &rect);
        auto event = std::make_shared<CgOperationClickedEventRuntime>(
            descriptor.eventValueAt04);
        child->BindClickedEventAt3C(event);
        child->WriteClickedEventEnabledAt48(1);
        clickedEventsAt74_.push_back(event);
        children_.push_back(std::move(child));
    }

    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(3,
            std::vector<GuiDialogNavigationMapping>{
                {3, 4}, {4, 3}, {5, 6}, {6, 5}}));
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(4,
            std::vector<GuiDialogNavigationMapping>{
                {4, 3}, {3, 4}, {6, 5}, {5, 6}}));

    compositeSurfaceAt90_ = surfaceFactory.CreateBlank(
        combined.right - combined.left,
        combined.bottom - combined.top,
        true,
        true);
    if (!compositeSurfaceAt90_)
        throw 1;
    compositeSurfaceAt90_->SetOrigin(combined.left, combined.top);

    slotDigitsAtA8_ = LoadResourceTable(
        surfaceFactory, SlotResourceNamesAt605E48());
    pageDigitsAtB4_ = LoadResourceTable(
        surfaceFactory, DecimalResourceNames('p', false, "", ""));
    dateResourcesAtC0_ = LoadResourceTable(
        surfaceFactory,
        DecimalResourceNames('y', true,
            "\x8A\x6D\x94\x46\x8E\xCE\x90\xFC\x2E\x76\x72\x6D",
            "\x4C\x8A\x6D\x94\x46\x8E\xCE\x90\xFC\x2E\x76\x72\x6D"));
    timeResourcesAtCC_ = LoadResourceTable(
        surfaceFactory,
        DecimalResourceNames('t', true,
            "\x8A\x6D\x94\x46\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D",
            "\x4C\x8A\x6D\x94\x46\x83\x52\x83\x8D\x83\x93\x2E\x76\x72\x6D"));
    quickAutoAtD8_ = LoadResourceTable(surfaceFactory, {
        Cp932("\x8A\x6D\x94\x46\x83\x4E\x83\x43\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
        Cp932("\x8A\x6D\x94\x46\x83\x49\x81\x5B\x83\x67\x2E\x76\x72\x6D")});
    overlaySurfaceAt84_ = surfaceFactory.LoadStatic(
        Cp932("\x8A\x6D\x94\x46\x89\xE6\x96\xCA\x97\x70\x94\xBC\x93\xA7\x96\xBE\x8D\x95\x2E\x76\x72\x6D"));
    if (!overlaySurfaceAt84_)
        throw 1;
}

void SaveLoadSlotConfirmationObjectGraphRuntime::WriteModeAt80(
    const std::int32_t value) noexcept
{
    modeAt80_ = value;
}

void SaveLoadSlotConfirmationObjectGraphRuntime::
ApplyModeChildEnablementAt50E7E0()
{
    for (const auto& child : children_)
        child->WriteAtVslot4C(1);

    if (modeAt80_ < 0 ||
        modeAt80_ >= static_cast<std::int32_t>(kDisabledChildrenByMode.size()))
    {
        throw std::out_of_range(
            "SaveLoadConfirm mode is outside the IDA table");
    }
    for (const std::int32_t eventValue :
         kDisabledChildrenByMode[static_cast<std::size_t>(modeAt80_)])
    {
        children_.at(static_cast<std::size_t>(eventValue))->WriteAtVslot4C(0);
    }
}

void SaveLoadSlotConfirmationObjectGraphRuntime::ConfigureAt535FF0(
    const std::string& pageLabel,
    const std::string& slotLabel,
    const std::string& timestampLabel,
    const std::string& sceneLabel,
    const bool isSaveMode)
{
    graphics_->ClearSurface(
        compositeSurfaceAt90_->Handle(),
        compositeSurfaceAt90_->ClearColor(),
        nullptr);
    renderServices_->DrawSceneLabelAt53ACA0(
        compositeSurfaceAt90_->Handle(), sceneLabel,
        57, 277, 21, 500, 0, 4,
        isSaveMode ? 0x006E10E5u : 0x00E5106Eu);
    DrawPageLabelAt508110(pageLabel, isSaveMode);
    DrawSlotLabelAt507EB0(slotLabel, isSaveMode);
    DrawTimestampAt5075C0(timestampLabel, isSaveMode);
}

void SaveLoadSlotConfirmationObjectGraphRuntime::AssignPreviewFrameAt538430(
    const SaveLoadDialogFrameRef& frame)
{
    previewFrameAt9C_ = frame;
}

std::int32_t
SaveLoadSlotConfirmationObjectGraphRuntime::PollClickedResultAt525300() noexcept
{
    for (const auto& event : clickedEventsAt74_)
    {
        if (event->ClickedFlagAt04() == 0)
            continue;
        const std::int32_t eventValue = event->EventValueAt08();
        event->ClearAtVslot04();
        if (eventValue == 3 || eventValue == 5)
        {
            resultAt68_ = 6;
            return 1;
        }
        if (eventValue == 4 || eventValue == 6)
        {
            resultAt68_ = 7;
            return 1;
        }
        return 0;
    }
    return 0;
}

std::int32_t SaveLoadSlotConfirmationObjectGraphRuntime::ModeAt80() const noexcept
{
    return modeAt80_;
}

std::int32_t SaveLoadSlotConfirmationObjectGraphRuntime::ResultAt68() const noexcept
{
    return resultAt68_;
}

const std::vector<CgOperationChildControlRef>&
SaveLoadSlotConfirmationObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::vector<CgOperationClickedEventRef>&
SaveLoadSlotConfirmationObjectGraphRuntime::ClickedEventsInInsertionOrder() const noexcept
{
    return clickedEventsAt74_;
}

const std::vector<GuiDialogNavigationGroupRef>&
SaveLoadSlotConfirmationObjectGraphRuntime::NavigationGroupsAt24() const noexcept
{
    return navigationGroupsAt24_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadSlotConfirmationObjectGraphRuntime::OverlaySurfaceAt84() const noexcept
{
    return overlaySurfaceAt84_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadSlotConfirmationObjectGraphRuntime::CompositeSurfaceAt90() const noexcept
{
    return compositeSurfaceAt90_;
}

const SaveLoadDialogFrameRef&
SaveLoadSlotConfirmationObjectGraphRuntime::PreviewFrameAt9C() const noexcept
{
    return previewFrameAt9C_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadSlotConfirmationObjectGraphRuntime::SlotDigitResourcesAtA8() const noexcept
{
    return slotDigitsAtA8_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadSlotConfirmationObjectGraphRuntime::PageDigitResourcesAtB4() const noexcept
{
    return pageDigitsAtB4_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadSlotConfirmationObjectGraphRuntime::DateResourcesAtC0() const noexcept
{
    return dateResourcesAtC0_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadSlotConfirmationObjectGraphRuntime::TimeResourcesAtCC() const noexcept
{
    return timeResourcesAtCC_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadSlotConfirmationObjectGraphRuntime::QuickAutoResourcesAtD8() const noexcept
{
    return quickAutoAtD8_;
}

void SaveLoadSlotConfirmationObjectGraphRuntime::DrawPageLabelAt508110(
    const std::string& value,
    const bool isSaveMode)
{
    const std::string trimmed = TrimSpacesAndTabs(value);
    if (trimmed == "Quick")
    {
        CopyResourceAt(quickAutoAtD8_.at(0), 123, 73);
        return;
    }
    if (trimmed == "Auto")
    {
        CopyResourceAt(quickAutoAtD8_.at(1), 123, 73);
        return;
    }

    std::size_t coordinate = 0;
    for (auto iterator = trimmed.rbegin();
         iterator != trimmed.rend() && coordinate < kPageCoordinates.size();
         ++iterator, ++coordinate)
    {
        const std::int32_t digit = DigitIndex(*iterator);
        if (digit < 0)
            continue;
        const POINT point = kPageCoordinates[coordinate];
        CopyResourceAt(
            SelectResource(pageDigitsAtB4_,
                static_cast<std::size_t>(digit), 10u, isSaveMode),
            point.x, point.y);
    }
}

void SaveLoadSlotConfirmationObjectGraphRuntime::DrawSlotLabelAt507EB0(
    const std::string& value,
    const bool isSaveMode)
{
    const std::string trimmed = TrimSpacesAndTabs(value);
    if (trimmed.empty())
        throw std::out_of_range("SaveLoadConfirm slot label is empty");
    const std::int32_t digit = DigitIndex(trimmed.back());
    if (digit < 0)
        return;
    CopyResourceAt(
        SelectResource(slotDigitsAtA8_,
            static_cast<std::size_t>(digit), 10u, isSaveMode),
        160, 226);
}

void SaveLoadSlotConfirmationObjectGraphRuntime::DrawTimestampAt5075C0(
    const std::string& value,
    const bool isSaveMode)
{
    const std::string trimmed = TrimSpacesAndTabs(value);
    const std::size_t separator = trimmed.find("\r\n");
    if (separator == std::string::npos)
        return;

    const std::string date = TrimSpacesAndTabs(trimmed.substr(0, separator));
    for (std::size_t index = 0;
         index < date.size() && index < kDateCoordinates.size(); ++index)
    {
        std::int32_t resourceIndex = DigitIndex(date[index]);
        if (date[index] == '/')
            resourceIndex = 10;
        if (resourceIndex < 0)
            continue;
        const POINT point = kDateCoordinates[index];
        CopyResourceAt(
            SelectResource(dateResourcesAtC0_,
                static_cast<std::size_t>(resourceIndex), 11u, isSaveMode),
            point.x, point.y);
    }

    const std::string time = TrimSpacesAndTabs(trimmed.substr(separator + 2u));
    for (std::size_t index = 0;
         index < time.size() && index < kTimeCoordinates.size(); ++index)
    {
        std::int32_t resourceIndex = DigitIndex(time[index]);
        if (time[index] == ':')
            resourceIndex = 10;
        if (resourceIndex < 0)
            continue;
        const POINT point = kTimeCoordinates[index];
        CopyResourceAt(
            SelectResource(timeResourcesAtCC_,
                static_cast<std::size_t>(resourceIndex), 11u, isSaveMode),
            point.x, point.y);
    }
}

void SaveLoadSlotConfirmationObjectGraphRuntime::CopyResourceAt(
    const std::shared_ptr<RuntimeGraphicsSurface>& source,
    const std::int32_t x,
    const std::int32_t y)
{
    if (!source)
        return;
    static_cast<void>(graphics_->BlitSurface(
        compositeSurfaceAt90_->Handle(), source->Handle(),
        x, y, nullptr, nullptr, nullptr, 0));
}

SaveLoadSlotConfirmationModalRuntime::SaveLoadSlotConfirmationModalRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ISaveLoadMessageBoxInputServices& inputServices,
    ISaveLoadMessageBoxModalServices& modalServices,
    ISaveLoadSlotConfirmationRenderServices& renderServices,
    ModalDialogRunnerRuntime& runner)
    : renderHost_(graphics),
      graphics_(&graphics),
      inputServices_(&inputServices),
      modalServices_(&modalServices),
      renderServices_(&renderServices),
      runner_(&runner),
      objectGraph_(surfaceFactory, graphics, *this, renderServices)
{
}

void SaveLoadSlotConfirmationModalRuntime::WriteModeAtVslot98(
    const std::int32_t value)
{
    objectGraph_.WriteModeAt80(value);
}

void SaveLoadSlotConfirmationModalRuntime::ConfigureAt535FF0(
    const std::string& pageLabel,
    const std::string& slotLabel,
    const std::string& timestampLabel,
    const std::string& sceneLabel,
    const bool isSaveMode)
{
    objectGraph_.ConfigureAt535FF0(
        pageLabel, slotLabel, timestampLabel, sceneLabel, isSaveMode);
}

void SaveLoadSlotConfirmationModalRuntime::AssignPreviewFrameAt538430(
    const SaveLoadDialogFrameRef& frame)
{
    objectGraph_.AssignPreviewFrameAt538430(frame);
}

std::int32_t SaveLoadSlotConfirmationModalRuntime::RunAt5071D0()
{
    return runner_->RunAt5071D0(*this);
}

void SaveLoadSlotConfirmationModalRuntime::BeginAtVslot44()
{
    objectGraph_.ApplyModeChildEnablementAt50E7E0();
    if (!capturedFrameAt5C_)
    {
        modalServices_->SynchronizeRuntimeAt413BE0();
        capturedFrameAt5C_ = modalServices_->CapturePrimaryFrameAt4ACDD0();
        if (!capturedFrameAt5C_)
        {
            throw std::runtime_error(
                "SaveLoadConfirm primary frame capture returned null");
        }
    }

    renderHost_.WriteEnabledAt38(1);
    for (const auto& event : objectGraph_.ClickedEventsInInsertionOrder())
        event->ClearAtVslot04();
    phaseAt6C_ = 0;
    ResetInputAt5106D0();
    ResetChildrenAt522E90();
    activeControlAt4C_.reset();
    modalServices_->SynchronizeRuntimeAt413BE0();
    modalServices_->WriteRootModalGateAt1B8(1);
}

bool SaveLoadSlotConfirmationModalRuntime::PollAtVslot48()
{
    switch (phaseAt6C_)
    {
    case 0:
    {
        const RECT bounds = capturedFrameAt5C_->ReadBoundsAt08();
        modalServices_->CompositeCapturedFrameAtVslot10(
            capturedFrameAt5C_, bounds);
        phaseAt6C_ = 3;
        modalServices_->FlushDirtyRectsAt5267E0();
        return true;
    }
    case 3:
        ProcessGenericDialogInputAt518C30();
        if (objectGraph_.PollClickedResultAt525300() != 0)
            phaseAt6C_ = 4;
        modalServices_->FlushDirtyRectsAt5267E0();
        return true;
    case 4:
    {
        const RECT bounds = capturedFrameAt5C_->ReadBoundsAt08();
        modalServices_->CompositeCapturedFrameAtVslot10(
            capturedFrameAt5C_, bounds);
        renderHost_.WriteEnabledAt38(0);
        phaseAt6C_ = -1;
        modalServices_->FlushDirtyRectsAt5267E0();
        return true;
    }
    default:
        return false;
    }
}

bool SaveLoadSlotConfirmationModalRuntime::EndAtVslot4C()
{
    objectGraph_.AssignPreviewFrameAt538430({});
    capturedFrameAt5C_.reset();
    modalServices_->SynchronizeRuntimeAt413BE0();
    modalServices_->WriteRootModalGateAt1B8(0);
    return true;
}

std::int32_t SaveLoadSlotConfirmationModalRuntime::ResultAtVslot50() const
{
    return objectGraph_.ResultAt68();
}

IModalDialogRuntimeTarget*
SaveLoadSlotConfirmationModalRuntime::ResolveAt4A5330()
{
    return this;
}

void SaveLoadSlotConfirmationModalRuntime::DrawAt526DA0()
{
    const std::span<const RECT> dirtyRects =
        renderServices_->ReadDirtyRectsAtVslot00Then18And1C();
    if (dirtyRects.empty())
        return;

    for (const RECT& clip : dirtyRects)
    {
        renderServices_->DrawCapturedFrameAtSurfaceDraw(
            capturedFrameAt5C_, clip);
        if (renderHost_.EnabledAt38() != 0)
        {
            const auto& overlay = objectGraph_.OverlaySurfaceAt84();
            static_cast<void>(graphics_->DrawSurface(
                overlay->Handle(), 0, 0,
                nullptr, nullptr, &clip, 0));
        }
        renderHost_.DrawChildrenAt5230B0(
            objectGraph_.ChildrenInInsertionOrder(), clip);
        if (renderHost_.EnabledAt38() != 0)
        {
            const auto& composite = objectGraph_.CompositeSurfaceAt90();
            static_cast<void>(graphics_->DrawSurface(
                composite->Handle(),
                composite->OriginX(), composite->OriginY(),
                nullptr, nullptr, &clip, 0));
            renderServices_->DrawPreviewFrameAt441980(
                objectGraph_.PreviewFrameAt9C(), 615, 200, clip);
        }
        renderServices_->InvalidatedRectAt526DA0(clip);
    }
    renderServices_->FinishDirtyRectsAtVslot00();
}

const SaveLoadSlotConfirmationObjectGraphRuntime&
SaveLoadSlotConfirmationModalRuntime::ObjectGraph() const noexcept
{
    return objectGraph_;
}

std::int32_t SaveLoadSlotConfirmationModalRuntime::PhaseAt6C() const noexcept
{
    return phaseAt6C_;
}

const SaveLoadMessageBoxCapturedFrameRef&
SaveLoadSlotConfirmationModalRuntime::CapturedFrameAt5C() const noexcept
{
    return capturedFrameAt5C_;
}

void SaveLoadSlotConfirmationModalRuntime::SetActiveControlAtVslot30(
    ICgOperationDialogEventTarget& control)
{
    const auto iterator = std::ranges::find_if(
        objectGraph_.ChildrenInInsertionOrder(),
        [&control](const CgOperationChildControlRef& child) {
            return child.get() == &control;
        });
    if (iterator == objectGraph_.ChildrenInInsertionOrder().end())
        throw std::logic_error("SaveLoadConfirm active control is not a child");
    SubmitActiveControlAt532170(*iterator);
}

bool SaveLoadSlotConfirmationModalRuntime::ReadInputAtVslot6C(
    const std::int32_t eventValue)
{
    return inputServices_->ReadInputAtVslot6C(eventValue);
}

bool SaveLoadSlotConfirmationModalRuntime::ReadInputAtVslot70(
    const std::int32_t eventValue)
{
    return inputServices_->ReadInputAtVslot70(eventValue);
}

POINT SaveLoadSlotConfirmationModalRuntime::ReadPointerAtVslot74()
{
    return inputServices_->ReadPointerAtVslot74();
}

bool SaveLoadSlotConfirmationModalRuntime::ReadPressAtVslot80()
{
    return inputServices_->ReadPressAtVslot80();
}

bool SaveLoadSlotConfirmationModalRuntime::ReadReleaseAtVslot84()
{
    return inputServices_->ReadReleaseAtVslot84();
}

std::uint32_t
SaveLoadSlotConfirmationModalRuntime::ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle,
    const std::int32_t x,
    const std::int32_t y)
{
    return inputServices_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}

void SaveLoadSlotConfirmationModalRuntime::SubmitActiveControlAt532170(
    CgOperationChildControlRef control)
{
    if (activeControlAt4C_ == control)
        return;
    if (activeControlAt4C_)
        activeControlAt4C_->LoseFocusAtVslot24();
    if (control)
        control->GainFocusAtVslot20();
    activeControlAt4C_ = std::move(control);
}

void SaveLoadSlotConfirmationModalRuntime::ResetInputAt5106D0()
{
    actionFiveLatchAt70_ = 0;
    SubmitActiveControlAt532170({});
    inputLatchAt30_ = 0;
}

void SaveLoadSlotConfirmationModalRuntime::ResetChildrenAt522E90()
{
    for (const auto& child : objectGraph_.ChildrenInInsertionOrder())
        child->InvokeAtVslot08();
}

CgOperationChildControlRef
SaveLoadSlotConfirmationModalRuntime::ResolveChildAt509D20(
    const std::int32_t eventValue) const
{
    const auto iterator = std::ranges::find_if(
        objectGraph_.ChildrenInInsertionOrder(),
        [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    return iterator == objectGraph_.ChildrenInInsertionOrder().end()
        ? CgOperationChildControlRef{}
        : *iterator;
}

bool SaveLoadSlotConfirmationModalRuntime::TryActivateCandidateAt51E740(
    const std::int32_t eventValue,
    const bool resetChildrenFirst)
{
    auto candidate = ResolveChildAt509D20(eventValue);
    if (!candidate)
        throw 1;
    const std::int32_t visualState = candidate->VisualStateAtVslot38();
    if (visualState == 1 || visualState == 2)
        return false;
    if (resetChildrenFirst)
        ResetChildrenAt522E90();
    SubmitActiveControlAt532170(std::move(candidate));
    inputLatchAt30_ = 1;
    return true;
}

std::int32_t SaveLoadSlotConfirmationModalRuntime::ProcessActionFiveAt510BA0()
{
    if (!activeControlAt4C_ || activeControlAt4C_->InputModeAt08() != 0)
        return 0;

    const std::int32_t previousLatch = actionFiveLatchAt70_;
    if (previousLatch != 0)
    {
        if (!inputServices_->ReadInputAtVslot6C(5))
        {
            activeControlAt4C_->InvokeReferencedObjectAt3CAtVslot00();
            actionFiveLatchAt70_ = 0;
        }
        return previousLatch;
    }
    if (!inputServices_->ReadInputAtVslot70(5))
        return 0;
    actionFiveLatchAt70_ = 1;
    activeControlAt4C_->WriteAtVslot70(1);
    return 1;
}

std::int32_t SaveLoadSlotConfirmationModalRuntime::ProcessUpperInputPathAt51E740()
{
    if (inputServices_->ReadInputAtVslot7C() ||
        inputServices_->ReadPressAtVslot80())
    {
        ResetInputAt5106D0();
        return 0;
    }
    if (activeControlAt4C_ && ProcessActionFiveAt510BA0() != 0)
        return 1;

    for (const auto& group : objectGraph_.NavigationGroupsAt24())
    {
        if (!group)
            throw std::logic_error("SaveLoadConfirm navigation group is null");
        if (!inputServices_->ReadInputAtVslot70(group->InputValueAt04()))
            continue;
        if (activeControlAt4C_)
        {
            const std::int32_t initial = activeControlAt4C_->EventValueAt04();
            std::int32_t candidate = initial;
            for (;;)
            {
                candidate = group->NextAtVslot00(candidate);
                if (candidate < 0 || candidate == initial)
                    break;
                if (TryActivateCandidateAt51E740(candidate, true))
                    return 3;
            }
        }
        else
        {
            for (const std::int32_t candidate : group->InitialCandidatesAt08())
            {
                if (TryActivateCandidateAt51E740(candidate, false))
                    return 3;
            }
        }
    }
    return inputLatchAt30_ != 0 ? 2 : 0;
}

void SaveLoadSlotConfirmationModalRuntime::ProcessGenericDialogInputAt518C30()
{
    if (renderHost_.ModeAt34() == 0 || renderHost_.EnabledAt38() == 0)
        return;

    std::map<std::int32_t, std::int32_t> visualStates;
    for (const auto& child : objectGraph_.ChildrenInInsertionOrder())
    {
        visualStates.insert(
            {child->EventValueAt04(), child->VisualStateAtVslot38()});
    }
    if (ProcessUpperInputPathAt51E740() == 0)
    {
        for (const auto& child : objectGraph_.ChildrenInInsertionOrder())
        {
            if (child->ProcessAtVslot14() == 0)
                static_cast<void>(child->ProcessAtVslot18());
        }
    }
    for (const auto& child : objectGraph_.ChildrenInInsertionOrder())
    {
        const std::int32_t before = visualStates[child->EventValueAt04()];
        const std::int32_t after = child->VisualStateAtVslot38();
        bool changed = before != after;
        if (!changed && child->InputModeAt08() == 5)
            changed = child->ReadModeFiveChangedAtVslot6C() != 0;
        if (changed)
            modalServices_->SubmitControlRectAt50FC80(
                child->ReadRectAtVslot3C());
    }
}
}
