#include <Runtime/MusicAppreciationModeSoundDialogRuntime.h>

#include <algorithm>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
MusicAppreciationModeSoundCheckboxRuntime::
MusicAppreciationModeSoundCheckboxRuntime(
    const std::int32_t controlIdAt04,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> offResources,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> onResources,
    CgOperationClickedEventRef clickedEvent)
    : controlIdAt04_(controlIdAt04),
      control_(std::make_shared<CgOperationChildControlRuntime>(
          CgOperationControlKind::CheckBox,
          controlIdAt04,
          -1,
          6,
          std::move(offResources),
          std::move(onResources))),
      clickedEventAt3C_(std::move(clickedEvent))
{
    if (!clickedEventAt3C_)
        throw std::invalid_argument("ModeSound CheckBox ClickedEvent is null");
    if (clickedEventAt3C_->EventValueAt08() != controlIdAt04_)
        throw std::invalid_argument("ModeSound CheckBox event value differs from control ID");
    control_->BindClickedEventAt3C(clickedEventAt3C_);
    control_->WriteClickedEventEnabledAt48(1);
}

std::int32_t MusicAppreciationModeSoundCheckboxRuntime::
ControlIdAt04() const noexcept
{
    return controlIdAt04_;
}

std::int32_t MusicAppreciationModeSoundCheckboxRuntime::
CheckedStateAt5C() const noexcept
{
    return control_->CheckBoxStateAt5C();
}

void MusicAppreciationModeSoundCheckboxRuntime::WriteCheckedStateAt5C(
    const std::int32_t value) noexcept
{
    control_->WriteCheckBoxStateAt5037D0(value);
}

RECT MusicAppreciationModeSoundCheckboxRuntime::ReadRectAtVslot3C() const
{
    return control_->ReadRectAtVslot3C();
}

void MusicAppreciationModeSoundCheckboxRuntime::
ClearInteractionStateAt04And20()
{
    clickedEventAt3C_->ClearAtVslot04();
    control_->LoseFocusAtVslot24();
}

const CgOperationControlResourceGroupRuntime&
MusicAppreciationModeSoundCheckboxRuntime::
CurrentResourceGroupAt50B280() const noexcept
{
    return *control_->CurrentResourcesAtVslot5C();
}

const CgOperationClickedEventRef&
MusicAppreciationModeSoundCheckboxRuntime::ClickedEventAt3C() const noexcept
{
    return clickedEventAt3C_;
}

std::int32_t MusicAppreciationModeSoundCheckboxRuntime::
FocusStateAt20() const noexcept
{
    return control_->StateAt20();
}

std::int32_t MusicAppreciationModeSoundCheckboxRuntime::
VisualSlotAt54() const noexcept
{
    return control_->InitialResourceSlotAt54();
}

const std::shared_ptr<CgOperationChildControlRuntime>&
MusicAppreciationModeSoundCheckboxRuntime::Control() const noexcept
{
    return control_;
}

MusicAppreciationModeSoundDialogRuntime::
MusicAppreciationModeSoundDialogRuntime(
    IMusicAppreciationModeSoundDialogServices& services,
    std::vector<MusicAppreciationModeSoundCheckboxRef> checkboxes,
    const std::int32_t firstControlId,
    const RECT& dialogRectAt64)
    : services_(&services),
      checkboxes_(std::move(checkboxes)),
      firstControlId_(firstControlId),
      dialogRectAt64_(dialogRectAt64)
{
    if (checkboxes_.size() >
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::length_error("ModeSoundDialog checkbox count exceeds int32 range");
    }
    for (std::size_t index = 0; index < checkboxes_.size(); ++index)
    {
        if (!checkboxes_[index])
            throw std::invalid_argument("ModeSoundDialog checkbox is null");
        const auto expected = static_cast<std::int64_t>(firstControlId_) +
            static_cast<std::int64_t>(index);
        if (expected < (std::numeric_limits<std::int32_t>::min)() ||
            expected > (std::numeric_limits<std::int32_t>::max)() ||
            checkboxes_[index]->ControlIdAt04() !=
                static_cast<std::int32_t>(expected))
        {
            throw std::invalid_argument(
                "ModeSoundDialog checkbox IDs are not the constructor table order");
        }
    }
}

void MusicAppreciationModeSoundDialogRuntime::InitializeAtVslot44()
{
    // 0x50D090：清空 18 项、自身 enabled、公共初始化、两个状态字段。
    ClearAllSelectionsAt532340(false);
    services_->WriteDialogEnabledAtVslot24(1);
    services_->InitializeChildrenAtVslot3C();
    fieldAt4C_ = 0;
    stateAt5C_ = 3;
}

std::int32_t MusicAppreciationModeSoundDialogRuntime::UpdateAtVslot48()
{
    // 0x52DD40 对调用方只使用副作用；返回值 0/1 不穿过现有 CSoundBtn 接口。
    switch (stateAt5C_)
    {
    case 0:
    {
        services_->InvalidateRootRect(dialogRectAt64_);
        stateAt5C_ = 3;
        [[fallthrough]];
    }
    case 3:
    {
        selectionAt80_ = -1;
        services_->UpdateGuiDialogAtVslot54();
        const std::int32_t event = services_->PollClickedEventAt50FDB0(1);
        if (event >= 0)
        {
            auto& checkbox = CheckboxForControlId(event);
            if (checkbox.CheckedStateAt5C() == 0)
            {
                ClearAllSelectionsAt532340(true);
                checkbox.WriteCheckedStateAt5C(1);
            }
            else
            {
                checkbox.WriteCheckedStateAt5C(0);
            }
            selectionAt80_ = event - firstControlId_;
        }
        services_->SubmitOwnDirtyRectsAtVslot58();
        return 1;
    }
    case 4:
    {
        services_->InvalidateRootRect(dialogRectAt64_);
        services_->WriteDialogEnabledAtVslot24(0);
        stateAt5C_ = -1;
        services_->SubmitOwnDirtyRectsAtVslot58();
        return 1;
    }
    default:
        return 0;
    }
}

std::int32_t MusicAppreciationModeSoundDialogRuntime::
SelectionAtVslot50()
{
    return selectionAt80_;
}

std::span<const RECT> MusicAppreciationModeSoundDialogRuntime::
EventDirtyRectsAtVslots00_18_1C() const
{
    return services_->EventDirtyRectsAtVslots00_18_1C();
}

void MusicAppreciationModeSoundDialogRuntime::DrawAtVslot60(
    const RECT* const clip)
{
    if (!clip)
        throw std::invalid_argument("ModeSoundDialog +0x60 clip is null");
    services_->DrawChildrenAtVslot60(*clip);
}

std::int32_t MusicAppreciationModeSoundDialogRuntime::ItemCount() const noexcept
{
    return static_cast<std::int32_t>(checkboxes_.size());
}

void MusicAppreciationModeSoundDialogRuntime::ClearItemStateAt04And20(
    const std::int32_t itemIndex)
{
    CheckboxForItemIndex(itemIndex).ClearInteractionStateAt04And20();
}

std::int32_t MusicAppreciationModeSoundDialogRuntime::StateAt5C() const noexcept
{
    return stateAt5C_;
}

void MusicAppreciationModeSoundDialogRuntime::WriteStateAt5C(
    const std::int32_t value) noexcept
{
    stateAt5C_ = value;
}

std::int32_t MusicAppreciationModeSoundDialogRuntime::FieldAt4C() const noexcept
{
    return fieldAt4C_;
}

const RECT& MusicAppreciationModeSoundDialogRuntime::DialogRectAt64() const noexcept
{
    return dialogRectAt64_;
}

IMusicAppreciationModeSoundCheckbox&
MusicAppreciationModeSoundDialogRuntime::CheckboxForControlId(
    const std::int32_t controlId) const
{
    const auto iterator = std::find_if(
        checkboxes_.begin(), checkboxes_.end(),
        [controlId](const MusicAppreciationModeSoundCheckboxRef& checkbox)
        {
            return checkbox->ControlIdAt04() == controlId;
        });
    if (iterator == checkboxes_.end())
    {
        // 原版随后会解引用空 ref_object；重写版不复制未定义访问，但也不兜底忽略。
        throw std::out_of_range("ModeSoundDialog event does not identify a checkbox");
    }
    return **iterator;
}

IMusicAppreciationModeSoundCheckbox&
MusicAppreciationModeSoundDialogRuntime::CheckboxForItemIndex(
    const std::int32_t itemIndex) const
{
    if (itemIndex < 0 ||
        static_cast<std::size_t>(itemIndex) >= checkboxes_.size())
    {
        throw std::out_of_range("ModeSoundDialog item index is outside checkbox table");
    }
    return *checkboxes_[static_cast<std::size_t>(itemIndex)];
}

void MusicAppreciationModeSoundDialogRuntime::ClearAllSelectionsAt532340(
    const bool invalidate)
{
    // 0x532370..0x532458 固定遍历控件 2..19；仅已选项被清除。
    for (const auto& checkbox : checkboxes_)
    {
        if (checkbox->CheckedStateAt5C() == 0)
            continue;
        checkbox->WriteCheckedStateAt5C(0);
        if (invalidate)
            services_->AddOwnDirtyRectAtVslot64(checkbox->ReadRectAtVslot3C());
    }
}

namespace
{
[[nodiscard]] CgOperationControlResourceDescriptor MakeCheckboxResources(
    const MusicAppreciationCheckboxStaticData& data,
    const MusicAppreciationItemStaticData& item,
    const bool checked)
{
    CgOperationControlResourceDescriptor descriptor{};
    if (checked)
    {
        descriptor.resourceNamesAt04 = {
            std::string(data.on), {}, {},
            std::string(data.onTargetFirst),
            std::string(data.onTargetSecond),
            std::string(data.onClick),
            std::string(data.selectionRange)};
    }
    else
    {
        descriptor.resourceNamesAt04 = {
            std::string(data.off), {}, {},
            std::string(data.offTargetFirst),
            std::string(data.offTargetSecond),
            std::string(data.offClick),
            std::string(data.selectionRange)};
    }
    descriptor.absoluteOriginAt20 = item.position;
    return descriptor;
}
}

MusicAppreciationModeSoundDialogObjectGraphRuntime::
MusicAppreciationModeSoundDialogObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    IMusicAppreciationModeSoundDialogInheritedServices& inheritedServices,
    const MusicAppreciationButtonStaticData& staticData)
    : inheritedServices_(&inheritedServices),
      renderHost_(graphics),
      operationKeys_(staticData.operationKeys)
{
    if (staticData.dialogBaseResources.size() != baseSurfaces_.size())
        throw std::invalid_argument("ModeSoundDialog requires three base resources");
    if (staticData.items.size() != 18u)
        throw std::invalid_argument("ModeSoundDialog requires eighteen CheckBoxes");
    if (staticData.operationKeys.size() != 4u)
        throw std::invalid_argument("ModeSoundDialog requires four operation keys");

    // 0x4E39B5..0x4E3AAF：三张资源严格按表顺序装载，失败抛出整数 1。
    for (std::size_t index = 0; index < baseSurfaces_.size(); ++index)
    {
        baseSurfaces_[index] = surfaceFactory.LoadStatic(
            std::string(staticData.dialogBaseResources[index]));
        if (!baseSurfaces_[index])
            throw 1;
    }

    // 0x4E3B00..0x4E3C34：第二张边框按 18 个固定坐标合入第一张。
    for (const auto& item : staticData.items)
    {
        (void)graphics.BlitSurface(
            baseSurfaces_[0]->Handle(),
            baseSurfaces_[1]->Handle(),
            item.position.x,
            item.position.y,
            nullptr,
            nullptr,
            nullptr,
            0);
    }
    // 0x4E3C40..0x4E3CBA：标题使用自身 AP 原点合入第一张。
    (void)graphics.BlitSurface(
        baseSurfaces_[0]->Handle(),
        baseSurfaces_[2]->Handle(),
        baseSurfaces_[2]->OriginX(),
        baseSurfaces_[2]->OriginY(),
        nullptr,
        nullptr,
        nullptr,
        0);

    auto backgroundResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            baseSurfaces_[0], 0u);
    staticBackgroundChild_ = std::make_shared<CgOperationChildControlRuntime>(
        CgOperationControlKind::Static,
        1,
        -1,
        0,
        std::move(backgroundResources));
    childRefs_.push_back(staticBackgroundChild_);

    checkboxes_.reserve(staticData.items.size());
    clickedEvents_.reserve(staticData.items.size());
    childRefs_.reserve(staticData.items.size() + 1u);
    std::vector<MusicAppreciationModeSoundCheckboxRef> dialogCheckboxes;
    dialogCheckboxes.reserve(staticData.items.size());
    for (std::size_t index = 0; index < staticData.items.size(); ++index)
    {
        const auto& item = staticData.items[index];
        const std::int32_t expectedControlId =
            staticData.checkbox.firstControlId + static_cast<std::int32_t>(index);
        if (item.controlId != expectedControlId)
            throw std::invalid_argument("ModeSoundDialog CheckBox ID table is not contiguous");

        auto offResources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory,
                MakeCheckboxResources(staticData.checkbox, item, false));
        auto onResources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory,
                MakeCheckboxResources(staticData.checkbox, item, true));
        auto event = std::make_shared<CgOperationClickedEventRuntime>(
            item.controlId);
        auto checkbox =
            std::make_shared<MusicAppreciationModeSoundCheckboxRuntime>(
                item.controlId,
                std::move(offResources),
                std::move(onResources),
                event);
        checkbox->Control()->BindButtonInputServicesAt4C(*this);
        clickedEvents_.push_back(std::move(event));
        childRefs_.push_back(checkbox->Control());
        checkboxes_.push_back(checkbox);
        dialogCheckboxes.push_back(std::move(checkbox));
    }

    operationKeyStates_.reserve(operationKeys_.size());
    for (const auto& key : operationKeys_)
        operationKeyStates_.push_back(OperationKeyRuntimeState{&key, 0, 0});

    dialog_ = std::make_shared<MusicAppreciationModeSoundDialogRuntime>(
        static_cast<IMusicAppreciationModeSoundDialogServices&>(*this),
        std::move(dialogCheckboxes),
        staticData.checkbox.firstControlId,
        staticBackgroundChild_->RectAtVslot3C());
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
InitializeAtVslot44()
{
    dialog_->InitializeAtVslot44();
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
UpdateAtVslot48()
{
    return dialog_->UpdateAtVslot48();
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
SelectionAtVslot50()
{
    return dialog_->SelectionAtVslot50();
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::DrawAtVslot60(
    const RECT* const clip)
{
    dialog_->DrawAtVslot60(clip);
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
ItemCount() const noexcept
{
    return dialog_->ItemCount();
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
ClearItemStateAt04And20(const std::int32_t itemIndex)
{
    dialog_->ClearItemStateAt04And20(itemIndex);
}

const std::shared_ptr<MusicAppreciationModeSoundDialogRuntime>&
MusicAppreciationModeSoundDialogObjectGraphRuntime::Dialog() const noexcept
{
    return dialog_;
}

const std::array<std::shared_ptr<RuntimeGraphicsSurface>, 3>&
MusicAppreciationModeSoundDialogObjectGraphRuntime::
BaseSurfacesInLoadOrder() const noexcept
{
    return baseSurfaces_;
}

const std::shared_ptr<CgOperationChildControlRuntime>&
MusicAppreciationModeSoundDialogObjectGraphRuntime::
StaticBackgroundChild() const noexcept
{
    return staticBackgroundChild_;
}

const std::vector<std::shared_ptr<
    MusicAppreciationModeSoundCheckboxRuntime>>&
MusicAppreciationModeSoundDialogObjectGraphRuntime::Checkboxes() const noexcept
{
    return checkboxes_;
}

const std::vector<CgOperationClickedEventRef>&
MusicAppreciationModeSoundDialogObjectGraphRuntime::
ClickedEvents() const noexcept
{
    return clickedEvents_;
}

std::span<const MusicAppreciationOperationKeyStaticData>
MusicAppreciationModeSoundDialogObjectGraphRuntime::OperationKeys() const noexcept
{
    return operationKeys_;
}

const CgOperationChildControlRef&
MusicAppreciationModeSoundDialogObjectGraphRuntime::
ActiveControlAt4C() const noexcept
{
    return activeControlAt4C_;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
NavigationStateAt30() const noexcept
{
    return navigationStateAt30_;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
ActionLatchAt60() const noexcept
{
    return actionLatchAt60_;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
OperationTimer(const std::size_t index) const
{
    return operationKeyStates_.at(index).timerAt14;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
OperationRepeatPhase(const std::size_t index) const
{
    return operationKeyStates_.at(index).repeatPhaseAt18;
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
WriteDialogEnabledAtVslot24(const std::int32_t value)
{
    renderHost_.WriteEnabledAt38(value);
    inheritedServices_->WriteDialogEnabledAt538660(value);
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
InitializeChildrenAtVslot3C()
{
    // GuiModeSoundDialog vslot +0x3C = 0x5106F0。
    ResetInputAt5106F0();
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
UpdateGuiDialogAtVslot54()
{
    // 0x518C30：+0x34 与 +0x38 均非零时才进入输入协调。
    if (renderHost_.ModeAt34() == 0 || renderHost_.EnabledAt38() == 0)
        return;

    std::map<std::int32_t, std::int32_t> visualStates;
    for (const auto& child : childRefs_)
    {
        visualStates.insert(
            {child->EventValueAt04(), child->VisualStateAtVslot38()});
    }

    if (ProcessUpperInputAt51EB70() == 0)
    {
        for (const auto& child : childRefs_)
        {
            if (child->ProcessAtVslot14() == 0)
                static_cast<void>(child->ProcessAtVslot18());
        }
    }

    auto& dirtyRects = inheritedServices_->OwnDirtyRectsAtVslot00();
    for (const auto& child : childRefs_)
    {
        const auto before = visualStates.find(child->EventValueAt04());
        if (before == visualStates.end())
            throw std::logic_error("ModeSoundDialog visual-state snapshot is missing");
        bool changed = before->second != child->VisualStateAtVslot38();
        if (!changed && child->InputModeAt08() == 5)
            changed = child->ReadModeFiveChangedAtVslot6C() != 0;
        if (changed)
            static_cast<void>(dirtyRects.AddAtVslot08(child->ReadRectAtVslot3C()));
    }
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
PollClickedEventAt50FDB0(const std::int32_t clear)
{
    for (const auto& event : clickedEvents_)
    {
        if (event->ClickedFlagAt04() == 0)
            continue;
        const std::int32_t value = event->EventValueAt08();
        if (clear != 0)
            event->ClearAtVslot04();
        return value;
    }
    return -1;
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
InvalidateRootRect(const RECT& rect)
{
    inheritedServices_->InvalidateRootRect(rect);
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
AddOwnDirtyRectAtVslot64(const RECT& rect)
{
    static_cast<void>(
        inheritedServices_->OwnDirtyRectsAtVslot00().AddAtVslot08(rect));
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
SubmitOwnDirtyRectsAtVslot58()
{
    // 0x525FB0：先整理自身脏矩形，逐项提交根失效，最后清空。
    auto& dirtyRects = inheritedServices_->OwnDirtyRectsAtVslot00();
    dirtyRects.PrepareAtVslot04();
    const std::size_t count = dirtyRects.CountAtVslot18();
    if (count == 0u)
        return;
    const auto rects = dirtyRects.RectsAtVslot1C();
    if (rects.size() < count)
        throw 1;
    for (std::size_t index = 0; index < count; ++index)
        inheritedServices_->InvalidateRootRect(rects[index]);
    dirtyRects.ClearAtVslot00();
}

std::span<const RECT>
MusicAppreciationModeSoundDialogObjectGraphRuntime::
EventDirtyRectsAtVslots00_18_1C() const
{
    auto& dirtyRects = inheritedServices_->OwnDirtyRectsAtVslot00();
    dirtyRects.PrepareAtVslot04();
    return dirtyRects.RectsAtVslot1C();
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
DrawChildrenAtVslot60(const RECT& clip)
{
    renderHost_.DrawChildrenAt5230B0(childRefs_, clip);
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    const auto iterator = std::ranges::find_if(
        childRefs_, [&control](const CgOperationChildControlRef& candidate) {
            return candidate.get() == &control;
        });
    if (iterator == childRefs_.end())
        throw std::logic_error("ModeSoundDialog active control is not a child");
    SubmitActiveControlAt532170(*iterator);
}

bool MusicAppreciationModeSoundDialogObjectGraphRuntime::
ReadInputAtVslot6C(const std::int32_t action)
{
    return inheritedServices_->ReadInputHeldAtVslot6C(action);
}

bool MusicAppreciationModeSoundDialogObjectGraphRuntime::
ReadInputAtVslot70(const std::int32_t action)
{
    return inheritedServices_->ReadInputPressedAtVslot70(action);
}

POINT MusicAppreciationModeSoundDialogObjectGraphRuntime::ReadPointerAtVslot74()
{
    return inheritedServices_->ReadPointerAtVslot74();
}

bool MusicAppreciationModeSoundDialogObjectGraphRuntime::ReadPressAtVslot80()
{
    return inheritedServices_->ReadPointerPressAtVslot80();
}

bool MusicAppreciationModeSoundDialogObjectGraphRuntime::ReadReleaseAtVslot84()
{
    return inheritedServices_->ReadPointerReleaseAtVslot84();
}

std::uint32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle,
    const std::int32_t x,
    const std::int32_t y)
{
    return inheritedServices_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::
SubmitActiveControlAt532170(CgOperationChildControlRef control)
{
    if (activeControlAt4C_ == control)
        return;
    if (activeControlAt4C_)
        activeControlAt4C_->LoseFocusAtVslot24();
    if (control)
        control->GainFocusAtVslot20();
    activeControlAt4C_ = std::move(control);
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::ResetInputAt5106F0()
{
    SubmitActiveControlAt532170({});
    navigationStateAt30_ = 0;
    actionLatchAt60_ = 0;
    for (auto& key : operationKeyStates_)
    {
        key.timerAt14 = 0;
        key.repeatPhaseAt18 = 0;
    }
}

void MusicAppreciationModeSoundDialogObjectGraphRuntime::ResetChildrenAt522E90()
{
    for (const auto& child : childRefs_)
        child->InvokeAtVslot08();
}

CgOperationChildControlRef
MusicAppreciationModeSoundDialogObjectGraphRuntime::ResolveChildAt509D20(
    const std::int32_t eventValue) const
{
    const auto iterator = std::ranges::find_if(
        childRefs_, [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    return iterator == childRefs_.end() ? CgOperationChildControlRef{} : *iterator;
}

bool MusicAppreciationModeSoundDialogObjectGraphRuntime::
TryActivateCandidateAt51EB70(
    const std::int32_t eventValue,
    const bool resetChildrenFirst)
{
    auto candidate = ResolveChildAt509D20(eventValue);
    if (!candidate)
        throw std::logic_error("ModeSoundDialog navigation target does not exist");
    const std::int32_t visualState = candidate->VisualStateAtVslot38();
    if (visualState == 1 || visualState == 2)
        return false;
    if (resetChildrenFirst)
        ResetChildrenAt522E90();
    SubmitActiveControlAt532170(std::move(candidate));
    navigationStateAt30_ = 1;
    return true;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
ProcessActiveCheckBoxAt510C80()
{
    if (activeControlAt4C_ && activeControlAt4C_->InputModeAt08() != 2)
        return 0;

    const std::int32_t oldLatch = actionLatchAt60_;
    if (oldLatch != 0)
    {
        if (!ReadInputAtVslot6C(5))
        {
            if (!activeControlAt4C_)
                throw std::logic_error("ModeSoundDialog action latch has no active CheckBox");
            activeControlAt4C_->InvokeReferencedObjectAt3CAtVslot00();
            activeControlAt4C_->WriteAtVslot70(0);
            actionLatchAt60_ = 0;
        }
        return oldLatch;
    }

    if (!ReadInputAtVslot70(5))
        return 0;
    if (!activeControlAt4C_)
        throw std::logic_error("ModeSoundDialog action 5 has no active CheckBox");
    actionLatchAt60_ = 1;
    activeControlAt4C_->WriteAtVslot70(1);
    return 1;
}

std::int32_t MusicAppreciationModeSoundDialogObjectGraphRuntime::
ProcessUpperInputAt51EB70()
{
    if (inheritedServices_->ReadCancelAtVslot7C() || ReadPressAtVslot80())
    {
        ResetInputAt5106F0();
        return 0;
    }
    if (activeControlAt4C_ && ProcessActiveCheckBoxAt510C80() != 0)
        return 1;

    std::int32_t result = navigationStateAt30_ != 0 ? 2 : 0;
    for (auto& key : operationKeyStates_)
    {
        if (!key.data)
            throw std::logic_error("ModeSoundDialog operation key is null");
        if (!ReadInputAtVslot6C(key.data->actionAt04))
        {
            key.timerAt14 = 0;
            key.repeatPhaseAt18 = 0;
            continue;
        }
        if (key.timerAt14 != 0)
        {
            if (key.timerAt14 > 0)
                --key.timerAt14;
            continue;
        }

        if (activeControlAt4C_)
        {
            const std::int32_t initial = activeControlAt4C_->EventValueAt04();
            std::int32_t current = initial;
            for (;;)
            {
                const auto pair = std::ranges::find_if(
                    key.data->navigationPairs,
                    [current](const MusicAppreciationNavigationPair& value) {
                        return value.sourceControlId == current;
                    });
                if (pair == key.data->navigationPairs.end())
                    throw std::logic_error("ModeSoundDialog navigation source is absent");
                current = pair->destinationControlId;
                if (current < 0 || current == initial)
                    break;
                if (TryActivateCandidateAt51EB70(current, true))
                {
                    result = 3;
                    break;
                }
            }
        }
        else
        {
            for (const auto& pair : key.data->navigationPairs)
            {
                if (TryActivateCandidateAt51EB70(pair.sourceControlId, false))
                {
                    result = 3;
                    break;
                }
            }
        }

        key.timerAt14 = key.repeatPhaseAt18 != 0
            ? key.data->extendedValueAt20
            : key.data->extendedValueAt1C;
        key.repeatPhaseAt18 = 1;
        return result;
    }
    return result;
}
} 
