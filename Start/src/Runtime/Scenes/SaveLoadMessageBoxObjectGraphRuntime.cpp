#include <Runtime/SaveLoadMessageBoxObjectGraphRuntime.h>

#include <algorithm>
#include <array>
#include <map>
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

[[nodiscard]] CgOperationChildDescriptor StaticDescriptor(
    const std::int32_t eventValue,
    const char* const resourceName)
{
    const std::string name = Cp932(resourceName);
    return CgOperationChildDescriptor{
        CgOperationControlKind::Static,
        eventValue,
        {{{name, {}, {}, name, name, {}, {}}}, std::nullopt},
        7,
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
    return CgOperationChildDescriptor{
        CgOperationControlKind::Button,
        eventValue,
        {{{normalName, {}, {}, targetName, targetName, Cp932(clicked), {}}},
         std::nullopt},
        7,
        -1};
}

[[nodiscard]] const std::array<CgOperationChildDescriptor, 7>&
ChildDescriptorsAt606280()
{
    // 0x606280/0x6043E8/0x6044C0：五个 Static 后接“是/否”两个 Button。
    static const std::array<CgOperationChildDescriptor, 7> descriptors{{
        StaticDescriptor(17,
            "\x8A\x6D\x94\x46\x91\xE5\x98\x67\x2E\x76\x72\x6D"),
        StaticDescriptor(18,
            "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x82\xCC"
            "\x83\x5A\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E"
            "\x8D\xED\x8F\x9C\x2E\x76\x72\x6D"),
        StaticDescriptor(19,
            "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x82\xCC"
            "\x83\x5A\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E"
            "\x91\x53\x8D\xED\x8F\x9C\x2E\x76\x72\x6D"),
        StaticDescriptor(20,
            "\x8A\x6D\x94\x46\x83\x4E\x83\x43\x83\x62\x83\x4E"
            "\x83\x5A\x81\x5B\x83\x75\x91\x53\x8D\xED\x8F\x9C"
            "\x2E\x76\x72\x6D"),
        StaticDescriptor(21,
            "\x8A\x6D\x94\x46\x83\x49\x81\x5B\x83\x67\x83\x5A"
            "\x81\x5B\x83\x75\x83\x66\x81\x5B\x83\x5E\x91\x53"
            "\x8D\xED\x8F\x9C\x2E\x76\x72\x6D"),
        ButtonDescriptor(22,
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2"
            "\x2E\x76\x72\x6D",
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2"
            "\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xCD\x82\xA2"
            "\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
        ButtonDescriptor(23,
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2"
            "\x82\xA6\x2E\x76\x72\x6D",
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2"
            "\x82\xA6\x83\x4A\x81\x5B\x83\x5C\x83\x8B\x2E\x76\x72\x6D",
            "\x8A\x6D\x94\x46\x91\x49\x91\xF0\x5F\x82\xA2\x82\xA2"
            "\x82\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D"),
    }};
    return descriptors;
}

[[nodiscard]] std::string DigitResourceName(const std::int32_t digit)
{
    std::string name(
        "\x8A\x6D\x94\x46\x83\x79\x81\x5B\x83\x57\x90\x94\x8E\x9A");
    name.push_back(static_cast<char>('0' + digit));
    name += ".vrm";
    return name;
}
}

SaveLoadMessageBoxObjectGraphRuntime::SaveLoadMessageBoxObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    IGraphicsSurfaceCopyApi& copyGraphics,
    ICgOperationButtonInputServices& buttonInputServices)
    : surfaceFactory_(&surfaceFactory),
      graphics_(&graphics),
      copyGraphics_(&copyGraphics)
{
    children_.reserve(7u);
    clickedEvents_.reserve(2u);
    for (const auto& descriptor : ChildDescriptorsAt606280())
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
        {
            auto event = std::make_shared<CgOperationClickedEventRuntime>(
                descriptor.eventValueAt04);
            child->BindButtonInputServicesAt4C(buttonInputServices);
            child->BindClickedEventAt3C(event);
            child->WriteClickedEventEnabledAt48(1);
            clickedEvents_.push_back(std::move(event));
        }
        children_.push_back(std::move(child));
    }

    // 0x6062B0：动作 3/4 共用 22<->23 的双向表，并保持 3、4 顺序。
    const std::vector<GuiDialogNavigationMapping> mappings{{22, 23}, {23, 22}};
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(3, mappings));
    navigationGroupsAt24_.push_back(
        std::make_shared<GuiDialogNavigationGroupRuntime>(4, mappings));
}

void SaveLoadMessageBoxObjectGraphRuntime::WriteModeAt80(
    const std::int32_t value) noexcept
{
    modeAt80_ = value;
}

void SaveLoadMessageBoxObjectGraphRuntime::ApplyModeChildEnablementAt50EAA0()
{
    for (const auto& child : children_)
    {
        const std::int32_t variant = child->EventValueAt04() - 18;
        child->WriteAtVslot4C(
            variant >= 0 && variant <= 3 ? modeAt80_ == variant : 1);
    }
}

void SaveLoadMessageBoxObjectGraphRuntime::PreparePageAt502950(
    const std::int32_t pageNumber)
{
    if (modeAt80_ != 1)
        return;
    RestoreAndDrawNumber(
        ResolveChildAt509D20(19), pageOnlyBaseAt90_, pageNumber, true);
}

void SaveLoadMessageBoxObjectGraphRuntime::PreparePageAndSlotAt502B10(
    const std::int32_t pageNumber,
    const std::int32_t slotNumber)
{
    if (modeAt80_ != 0)
        return;

    const auto child = ResolveChildAt509D20(18);
    if (!child)
        throw 1;
    const auto& resources = child->ResourcesAt28();
    if (!resources)
        throw 2;
    if (resources->ResourceIndexForSlot(0u) < 0)
        throw 3;
    RuntimeGraphicsSurface& destination = *resources->ResourceForSlot(0u);

    if (!pageAndSlotBaseAt84_)
        pageAndSlotBaseAt84_ = surfaceFactory_->CreateCompatible(destination, true);
    if (!pageAndSlotBaseAt84_)
        throw std::runtime_error("SaveLoadMessageBox page/slot base copy failed");

    graphics_->ClearSurface(
        destination.Handle(), destination.ClearColor(), nullptr);
    static_cast<void>(copyGraphics_->CopyBlitSurface(
        destination.Handle(), pageAndSlotBaseAt84_->Handle(),
        0, 0, nullptr, nullptr, nullptr, 0));
    EnsureDigitResourcesAt529FD0();

    const std::int32_t hundreds = pageNumber / 100;
    const std::int32_t pageRemainder = pageNumber - hundreds * 100;
    const std::int32_t tens = pageRemainder / 10;
    const std::int32_t ones = pageRemainder - tens * 10;
    DrawDigitAt529FD0(destination, hundreds, 455, 284);
    DrawDigitAt529FD0(destination, tens, 471, 284);
    DrawDigitAt529FD0(destination, ones, 487, 284);

    const std::int32_t slotTens = slotNumber / 10;
    DrawDigitAt529FD0(destination, slotTens, 707, 284);
    DrawDigitAt529FD0(
        destination, slotNumber - slotTens * 10, 724, 284);
}

std::int32_t
SaveLoadMessageBoxObjectGraphRuntime::PollClickedResultAt5253D0() noexcept
{
    for (const auto& event : clickedEvents_)
    {
        if (event->ClickedFlagAt04() == 0)
            continue;
        if (event->EventValueAt08() == 22)
        {
            resultAt68_ = 6;
            return 1;
        }
        if (event->EventValueAt08() == 23)
        {
            resultAt68_ = 7;
            return 1;
        }
        return 0;
    }
    return 0;
}

std::int32_t SaveLoadMessageBoxObjectGraphRuntime::ModeAt80() const noexcept
{
    return modeAt80_;
}

std::int32_t SaveLoadMessageBoxObjectGraphRuntime::ResultAt68() const noexcept
{
    return resultAt68_;
}

const std::vector<CgOperationChildControlRef>&
SaveLoadMessageBoxObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const std::vector<CgOperationClickedEventRef>&
SaveLoadMessageBoxObjectGraphRuntime::
ClickedEventsInInsertionOrder() const noexcept
{
    return clickedEvents_;
}

const std::vector<GuiDialogNavigationGroupRef>&
SaveLoadMessageBoxObjectGraphRuntime::NavigationGroupsAt24() const noexcept
{
    return navigationGroupsAt24_;
}

std::span<const std::shared_ptr<RuntimeGraphicsSurface>>
SaveLoadMessageBoxObjectGraphRuntime::DigitResourcesAt9C() const noexcept
{
    return digitResourcesAt9C_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadMessageBoxObjectGraphRuntime::PageAndSlotBaseAt84() const noexcept
{
    return pageAndSlotBaseAt84_;
}

const std::shared_ptr<RuntimeGraphicsSurface>&
SaveLoadMessageBoxObjectGraphRuntime::PageOnlyBaseAt90() const noexcept
{
    return pageOnlyBaseAt90_;
}

std::shared_ptr<CgOperationChildControlRuntime>
SaveLoadMessageBoxObjectGraphRuntime::ResolveChildAt509D20(
    const std::int32_t eventValue) const
{
    const auto iterator = std::ranges::find_if(
        children_,
        [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    if (iterator == children_.end())
        return {};
    auto child = std::dynamic_pointer_cast<CgOperationChildControlRuntime>(
        *iterator);
    if (!child)
    {
        throw std::logic_error(
            "SaveLoadMessageBox child has an unexpected runtime type");
    }
    return child;
}

void SaveLoadMessageBoxObjectGraphRuntime::EnsureDigitResourcesAt529FD0()
{
    if (!digitResourcesAt9C_.empty())
        return;

    digitResourcesAt9C_.resize(10u);
    for (std::int32_t digit = 0; digit < 10; ++digit)
    {
        digitResourcesAt9C_[static_cast<std::size_t>(digit)] =
            surfaceFactory_->LoadStatic(DigitResourceName(digit));
        if (!digitResourcesAt9C_[static_cast<std::size_t>(digit)])
            throw 1;
    }
}

void SaveLoadMessageBoxObjectGraphRuntime::RestoreAndDrawNumber(
    const std::shared_ptr<CgOperationChildControlRuntime>& child,
    std::shared_ptr<RuntimeGraphicsSurface>& cachedBase,
    const std::int32_t number,
    const bool threeDigits)
{
    if (!child)
        throw 1;
    const auto& resources = child->ResourcesAt28();
    if (!resources)
        throw 2;
    if (resources->ResourceIndexForSlot(0u) < 0)
        throw 3;
    RuntimeGraphicsSurface& destination = *resources->ResourceForSlot(0u);

    if (!cachedBase)
        cachedBase = surfaceFactory_->CreateCompatible(destination, true);
    if (!cachedBase)
        throw std::runtime_error("SaveLoadMessageBox base copy failed");

    graphics_->ClearSurface(
        destination.Handle(), destination.ClearColor(), nullptr);
    static_cast<void>(copyGraphics_->CopyBlitSurface(
        destination.Handle(), cachedBase->Handle(),
        0, 0, nullptr, nullptr, nullptr, 0));
    EnsureDigitResourcesAt529FD0();

    if (threeDigits)
    {
        const std::int32_t hundreds = number / 100;
        const std::int32_t remainder = number - hundreds * 100;
        const std::int32_t tens = remainder / 10;
        DrawDigitAt529FD0(destination, hundreds, 455, 284);
        DrawDigitAt529FD0(destination, tens, 471, 284);
        DrawDigitAt529FD0(destination, remainder - tens * 10, 487, 284);
        return;
    }

    const std::int32_t tens = number / 10;
    DrawDigitAt529FD0(destination, tens, 707, 284);
    DrawDigitAt529FD0(destination, number - tens * 10, 724, 284);
}

void SaveLoadMessageBoxObjectGraphRuntime::DrawDigitAt529FD0(
    RuntimeGraphicsSurface& destination,
    const std::int32_t digit,
    const std::int32_t absoluteX,
    const std::int32_t absoluteY)
{
    const auto& source = digitResourcesAt9C_.at(
        static_cast<std::size_t>(digit));
    static_cast<void>(graphics_->BlitSurface(
        destination.Handle(), source->Handle(),
        absoluteX - destination.OriginX(),
        absoluteY - destination.OriginY(),
        nullptr, nullptr, nullptr, 0));
}

SaveLoadMessageBoxModalRuntime::SaveLoadMessageBoxModalRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    IGraphicsSurfaceCopyApi& copyGraphics,
    ISaveLoadMessageBoxInputServices& inputServices,
    ISaveLoadMessageBoxModalServices& modalServices,
    ModalDialogRunnerRuntime& runner)
    : renderHost_(graphics),
      inputServices_(&inputServices),
      modalServices_(&modalServices),
      runner_(&runner),
      objectGraph_(surfaceFactory, graphics, copyGraphics, *this)
{
}

void SaveLoadMessageBoxModalRuntime::WriteModeAt80(
    const std::int32_t value)
{
    objectGraph_.WriteModeAt80(value);
}

void SaveLoadMessageBoxModalRuntime::PreparePageAt502950(
    const std::int32_t pageNumber)
{
    objectGraph_.PreparePageAt502950(pageNumber);
}

void SaveLoadMessageBoxModalRuntime::PreparePageAndSlotAt502B10(
    const std::int32_t pageNumber,
    const std::int32_t slotNumber)
{
    objectGraph_.PreparePageAndSlotAt502B10(pageNumber, slotNumber);
}

std::int32_t SaveLoadMessageBoxModalRuntime::RunAt5071D0()
{
    return runner_->RunAt5071D0(*this);
}

void SaveLoadMessageBoxModalRuntime::BeginAtVslot44()
{
    // 0x50EAA0 的派生门控严格先于 0x50CF50 公共 Begin。
    objectGraph_.ApplyModeChildEnablementAt50EAA0();
    if (!capturedFrameAt5C_)
    {
        modalServices_->SynchronizeRuntimeAt413BE0();
        capturedFrameAt5C_ = modalServices_->CapturePrimaryFrameAt4ACDD0();
        if (!capturedFrameAt5C_)
        {
            // 原版会在 0x52DBC0 解引用空帧；这里在同一所有权边界明确失败。
            throw std::runtime_error(
                "SaveLoadMessageBox primary frame capture returned null");
        }
    }

    renderHost_.WriteEnabledAt38(1);
    for (const auto& event : objectGraph_.ClickedEventsInInsertionOrder())
        event->ClearAtVslot04();
    phaseAt6C_ = 0;
    ResetInputAt5106D0();
    ResetChildrenAt522E90();
    activeControlAt4C_.reset();
    // 派生虚槽 +0x8C 在样本 18 中是 0x5086B0（nullsub）。
    modalServices_->SynchronizeRuntimeAt413BE0();
    modalServices_->WriteRootModalGateAt1B8(1);
}

bool SaveLoadMessageBoxModalRuntime::PollAtVslot48()
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
        if (objectGraph_.PollClickedResultAt5253D0() != 0)
            phaseAt6C_ = 4;
        modalServices_->FlushDirtyRectsAt5267E0();
        return true;
    case 4:
    {
        const RECT bounds = capturedFrameAt5C_->ReadBoundsAt08();
        modalServices_->CompositeCapturedFrameAtVslot10(
            capturedFrameAt5C_, bounds);
        renderHost_.WriteEnabledAt38(0);
        // 派生虚槽 +0x94 在样本 18 中是 0x51C100（nullsub）。
        phaseAt6C_ = -1;
        modalServices_->FlushDirtyRectsAt5267E0();
        return true;
    }
    default:
        return false;
    }
}

bool SaveLoadMessageBoxModalRuntime::EndAtVslot4C()
{
    capturedFrameAt5C_.reset();
    modalServices_->SynchronizeRuntimeAt413BE0();
    modalServices_->WriteRootModalGateAt1B8(0);
    return true;
}

std::int32_t SaveLoadMessageBoxModalRuntime::ResultAtVslot50() const
{
    return objectGraph_.ResultAt68();
}

IModalDialogRuntimeTarget*
SaveLoadMessageBoxModalRuntime::ResolveAt4A5330()
{
    return this;
}

void SaveLoadMessageBoxModalRuntime::DrawAt5230B0(const RECT& clip)
{
    renderHost_.DrawChildrenAt5230B0(
        objectGraph_.ChildrenInInsertionOrder(), clip);
}

void SaveLoadMessageBoxModalRuntime::CompositeAt5231C0(
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    renderHost_.CompositeChildrenAt5231C0(
        objectGraph_.ChildrenInInsertionOrder(),
        destination,
        destinationClip);
}

const SaveLoadMessageBoxObjectGraphRuntime&
SaveLoadMessageBoxModalRuntime::ObjectGraph() const noexcept
{
    return objectGraph_;
}

std::int32_t SaveLoadMessageBoxModalRuntime::PhaseAt6C() const noexcept
{
    return phaseAt6C_;
}

std::int32_t SaveLoadMessageBoxModalRuntime::InputLatchAt30() const noexcept
{
    return inputLatchAt30_;
}

std::int32_t SaveLoadMessageBoxModalRuntime::ActionFiveLatchAt70() const noexcept
{
    return actionFiveLatchAt70_;
}

const CgOperationChildControlRef&
SaveLoadMessageBoxModalRuntime::ActiveControlAt4C() const noexcept
{
    return activeControlAt4C_;
}

const SaveLoadMessageBoxCapturedFrameRef&
SaveLoadMessageBoxModalRuntime::CapturedFrameAt5C() const noexcept
{
    return capturedFrameAt5C_;
}

void SaveLoadMessageBoxModalRuntime::SetActiveControlAtVslot30(
    ICgOperationDialogEventTarget& control)
{
    const auto iterator = std::ranges::find_if(
        objectGraph_.ChildrenInInsertionOrder(),
        [&control](const CgOperationChildControlRef& child) {
            return child.get() == &control;
        });
    if (iterator == objectGraph_.ChildrenInInsertionOrder().end())
        throw std::logic_error("SaveLoadMessageBox active control is not a child");
    SubmitActiveControlAt532170(*iterator);
}

bool SaveLoadMessageBoxModalRuntime::ReadInputAtVslot6C(
    const std::int32_t eventValue)
{
    return inputServices_->ReadInputAtVslot6C(eventValue);
}

bool SaveLoadMessageBoxModalRuntime::ReadInputAtVslot70(
    const std::int32_t eventValue)
{
    return inputServices_->ReadInputAtVslot70(eventValue);
}

POINT SaveLoadMessageBoxModalRuntime::ReadPointerAtVslot74()
{
    return inputServices_->ReadPointerAtVslot74();
}

bool SaveLoadMessageBoxModalRuntime::ReadPressAtVslot80()
{
    return inputServices_->ReadPressAtVslot80();
}

bool SaveLoadMessageBoxModalRuntime::ReadReleaseAtVslot84()
{
    return inputServices_->ReadReleaseAtVslot84();
}

std::uint32_t
SaveLoadMessageBoxModalRuntime::ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle,
    const std::int32_t x,
    const std::int32_t y)
{
    return inputServices_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}

void SaveLoadMessageBoxModalRuntime::SubmitActiveControlAt532170(
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

void SaveLoadMessageBoxModalRuntime::ResetInputAt5106D0()
{
    actionFiveLatchAt70_ = 0;
    SubmitActiveControlAt532170({});
    inputLatchAt30_ = 0;
}

void SaveLoadMessageBoxModalRuntime::ResetChildrenAt522E90()
{
    for (const auto& child : objectGraph_.ChildrenInInsertionOrder())
        child->InvokeAtVslot08();
}

CgOperationChildControlRef
SaveLoadMessageBoxModalRuntime::ResolveChildAt509D20(
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

bool SaveLoadMessageBoxModalRuntime::TryActivateCandidateAt51E740(
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

std::int32_t SaveLoadMessageBoxModalRuntime::ProcessActionFiveAt510BA0()
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

std::int32_t SaveLoadMessageBoxModalRuntime::ProcessUpperInputPathAt51E740()
{
    if (inputServices_->ReadInputAtVslot7C() ||
        inputServices_->ReadPressAtVslot80())
    {
        ResetInputAt5106D0();
        return 0;
    }

    // +0x40=0x511610 固定返回 0；有活动控件时才进入 +0x38=0x510BA0。
    if (activeControlAt4C_ && ProcessActionFiveAt510BA0() != 0)
        return 1;

    for (const auto& group : objectGraph_.NavigationGroupsAt24())
    {
        if (!group)
            throw std::logic_error("SaveLoadMessageBox navigation group is null");
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

void SaveLoadMessageBoxModalRuntime::ProcessGenericDialogInputAt518C30()
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
        // +0x88=0x518A80 只销毁按值传入的引用，不产生额外业务副作用。
    }
}
}
