#include <Runtime/SceneManagerOptionCharSelectDialogObjectGraphRuntime.h>

#include <array>
#include <algorithm>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kResource00[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x31\x81\x69\x90\x53\x8D\xD8\x81\x6A\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource01[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x31\x81\x69\x90\x53\x8D\xD8\x81\x6A\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource02[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x31\x81\x69\x90\x53\x8D\xD8\x81\x6A\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource03[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x31\x81\x69\x90\x53\x8D\xD8\x81\x6A\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";
constexpr char kResource04[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x32\x81\x69\x90\xE1\x88\xA4\x81\x6A\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource05[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x32\x81\x69\x90\xE1\x88\xA4\x81\x6A\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource06[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x32\x81\x69\x90\xE1\x88\xA4\x81\x6A\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource07[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x32\x81\x69\x90\xE1\x88\xA4\x81\x6A\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";
constexpr char kResource08[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x33\x81\x69\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x6A\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource09[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x33\x81\x69\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x6A\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource10[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x33\x81\x69\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x6A\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource11[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x33\x81\x69\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x6A\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";
constexpr char kResource12[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x6A\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource13[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x6A\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource14[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x6A\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource15[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x6A\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";
constexpr char kResource16[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x82\xBB\x82\xCC\x91\xBC\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource17[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x82\xBB\x82\xCC\x91\xBC\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource18[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x82\xBB\x82\xCC\x91\xBC\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource19[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x82\xBB\x82\xCC\x91\xBC\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";
constexpr char kResource20[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x40\x96\xA2\x8A\x4A\x95\xFA\x81\x6A\x83\x49\x83\x93\x2E\x61\x6C\x70";
constexpr char kResource21[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x40\x96\xA2\x8A\x4A\x95\xFA\x81\x6A\x83\x49\x83\x74\x2E\x61\x6C\x70";
constexpr char kResource22[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x40\x96\xA2\x8A\x4A\x95\xFA\x81\x6A\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x61\x6C\x70";
constexpr char kResource23[] = "\x76\x72\x6D\x3A\x5C\x8A\xD3\x8F\xDC\x83\x4C\x83\x83\x83\x89\x91\x49\x91\xF0\x5F\x34\x81\x69\x89\xC4\x91\xE3\x8E\x71\x81\x40\x96\xA2\x8A\x4A\x95\xFA\x81\x6A\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x61\x6C\x70";

using ResourceRecord = std::array<const char*, 4>;
constexpr std::array<ResourceRecord, 6> kRecords{{
    {kResource00, kResource01, kResource02, kResource03},
    {kResource04, kResource05, kResource06, kResource07},
    {kResource08, kResource09, kResource10, kResource11},
    {kResource12, kResource13, kResource14, kResource15},
    {kResource16, kResource17, kResource18, kResource19},
    {kResource20, kResource21, kResource22, kResource23},
}};

[[nodiscard]] CgOperationControlResourceDescriptor MakeDescriptor(
    const ResourceRecord& record)
{
    CgOperationControlResourceDescriptor descriptor{};
    descriptor.resourceNamesAt04 = {
        record[0], record[1], {}, record[2], record[2], record[3], {}};
    return descriptor;
}
}

SceneManagerOptionCharSelectDialogObjectGraphRuntime::
SceneManagerOptionCharSelectDialogObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ISceneManagerOptionCharAvailabilitySample18& availability,
    ISceneManagerOptionCharInheritedServices& inheritedServices)
    : inheritedServices_(&inheritedServices),
      renderHost_(graphics)
{
    buttons_.reserve(5u);
    childRefs_.reserve(5u);
    for (std::int32_t index = 0; index < 5; ++index)
    {
        std::size_t recordIndex = static_cast<std::size_t>(index);
        if (index < 4 && !availability.IsAvailableAt4E4C68(index) && index == 3)
            recordIndex = 5u;

        auto resources =
            std::make_shared<CgOperationControlResourceGroupRuntime>(
                surfaceFactory, MakeDescriptor(kRecords[recordIndex]));
        auto button = std::make_shared<CgOperationChildControlRuntime>(
            CgOperationControlKind::Button,
            index,
            -1,
            0,
            std::move(resources));
        button->BindButtonInputServicesAt4C(*this);
        childRefs_.push_back(button);
        buttons_.push_back(std::move(button));
    }
    clickedEvents_ = CgOperationClickedEventManagerRuntime::
        CreateForOptionCharSelectAt4E4A20(childRefs_);
    auto& services =
        static_cast<ISceneManagerOptionCharSelectDialogServices&>(*this);
    dialog_ =
        std::make_shared<SceneManagerOptionCharSelectDialogRuntime>(services);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::InitializeAt50D930()
{
    dialog_->InitializeAt50D930();
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ProcessAtVslot48()
{
    return dialog_->ProcessAtVslot48();
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
SelectionAtVslot50() const
{
    return dialog_->SelectionAtVslot50();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
AttachDirtyRectsAtVslot94(ICgDisplayButtonGroupDirtyRects& dirtyRects)
{
    dialog_->AttachDirtyRectsAtVslot94(dirtyRects);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::ResetAtVslot90()
{
    dialog_->ResetAtVslot90();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::DrawAtVslot60(
    const RECT& clip)
{
    dialog_->DrawAtVslot60(clip);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::CompositeAtVslot5C(
    const CgDisplayTransitionSourceRef& target,
    const RECT* const destinationClip)
{
    dialog_->CompositeAtVslot5C(target, destinationClip);
}

const std::vector<std::shared_ptr<CgOperationChildControlRuntime>>&
SceneManagerOptionCharSelectDialogObjectGraphRuntime::Buttons() const noexcept
{
    return buttons_;
}

const std::shared_ptr<CgOperationClickedEventManagerRuntime>&
SceneManagerOptionCharSelectDialogObjectGraphRuntime::ClickedEvents() const noexcept
{
    return clickedEvents_;
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
PagingInputLatchAt6C() const noexcept
{
    return pagingInputLatchAt6C_;
}

const CgOperationChildControlRef&
SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ActiveControlAt4C() const noexcept
{
    return activeControlAt4C_;
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
StateAt68() const noexcept
{
    return dialog_->StateAt68();
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
FocusedChildAt84() const noexcept
{
    return dialog_->FocusedChildAt84();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::ResetDialogBaseAt510750()
{
    // 0x510750 先通过 +0x30 清空活动控件，再清 +0x30 与本派生类的 +0x6C。
    SubmitActiveControlAt532170({});
    pagingInputLatchAt6C_ = 0;
    inheritedServices_->ResetDialogBaseAt510750();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
WriteDialogEnabledAtVslot24(const std::int32_t value)
{
    renderHost_.WriteEnabledAt38(value);
    inheritedServices_->WriteDialogEnabledAt538660(value);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ProcessGenericDialogAtVslot54()
{
    // 0x518C30：只有 GuiDialog +0x34/+0x38 同时非零才处理子控件。
    if (renderHost_.ModeAt34() == 0 || renderHost_.EnabledAt38() == 0)
        return;

    // 原函数使用 map::insert；事件值重复时必须保留第一份状态快照。
    std::map<std::int32_t, std::int32_t> visualStates;
    for (const auto& control : childRefs_)
    {
        visualStates.insert(
            {control->EventValueAt04(), control->VisualStateAtVslot38()});
    }

    if (ProcessUpperInputAt51F8E0() == 0)
    {
        for (const auto& control : childRefs_)
        {
            if (control->ProcessAtVslot14() == 0)
                static_cast<void>(control->ProcessAtVslot18());
        }
    }

    auto& dirtyRects = inheritedServices_->OwnDirtyRectsAtVslot00();
    for (const auto& control : childRefs_)
    {
        const auto before = visualStates.find(control->EventValueAt04());
        if (before == visualStates.end())
            throw std::logic_error("GuiDialog visual-state snapshot is missing");

        bool changed = before->second != control->VisualStateAtVslot38();
        if (!changed && control->InputModeAt08() == 5)
            changed = control->ReadModeFiveChangedAtVslot6C() != 0;
        if (changed)
            static_cast<void>(dirtyRects.AddAtVslot08(control->ReadRectAtVslot3C()));
    }
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::ClearClickedEventsAt532200()
{
    clickedEvents_->ClearAllAt532200();
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
PollClickedEventAt50FDB0(const std::int32_t clearMatchedEvent)
{
    return clickedEvents_->PollFirstAt50FDB0(clearMatchedEvent);
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::HasChildAtVslot14(
    const std::int32_t childId) const
{
    return childId >= 0 &&
        static_cast<std::size_t>(childId) < buttons_.size();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::ResetChildAtVslot08(
    const std::int32_t childId)
{
    ChildForId(childId).InvokeAtVslot08();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
WriteChildEnabledAtVslot4C(
    const std::int32_t childId,
    const std::int32_t value)
{
    ChildForId(childId).WriteAtVslot4C(value);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
WriteChildPositionAt34And38(
    const std::int32_t childId,
    const POINT& position)
{
    ChildForId(childId).WritePositionAt34And38(position);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
WriteChildStateAtVslot44(
    const std::int32_t childId,
    const std::int32_t value)
{
    ChildForId(childId).WriteAtVslot44(value);
}

ICgDisplayButtonGroupDirtyRects&
SceneManagerOptionCharSelectDialogObjectGraphRuntime::OwnDirtyRectsAtVslot00()
{
    return inheritedServices_->OwnDirtyRectsAtVslot00();
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
DrawChildrenAt5230B0(const RECT& clip)
{
    renderHost_.DrawChildrenAt5230B0(childRefs_, clip);
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
CompositeChildrenAt5231C0(
    const CgDisplayTransitionSourceRef& target,
    const RECT* const destinationClip)
{
    renderHost_.CompositeChildrenAt5231C0(
        childRefs_, inheritedServices_->ResolveTransitionTargetHandle(target),
        destinationClip);
}

CgOperationChildControlRuntime&
SceneManagerOptionCharSelectDialogObjectGraphRuntime::ChildForId(
    const std::int32_t childId) const
{
    if (!HasChildAtVslot14(childId))
        throw std::out_of_range("OptionCharSelectDialog child ID is outside 0..4");
    return *buttons_[static_cast<std::size_t>(childId)];
}

CgOperationChildControlRef
SceneManagerOptionCharSelectDialogObjectGraphRuntime::ResolveChildAt509D20(
    const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(
        childRefs_,
        [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    return it == childRefs_.end() ? CgOperationChildControlRef{} : *it;
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
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

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ProcessUpperInputAt51F8E0()
{
    // 0x51F8E0 是 OptionCharSelectDialog 对通用 +0x34 的覆盖：
    // 动作 9/10 分别选择上一项/下一项，并用 +0x6C 记住待释放方向。
    if (pagingInputLatchAt6C_ != 0)
        return ProcessBaseUpperInputAt51E740();

    std::int32_t eventValue = -1;
    std::int32_t latch = 0;
    if (ReadInputAtVslot70(9))
    {
        // 0x51F920..0x51F958。样本 18 的表尾记录事件值为 3。
        eventValue = dialog_->FocusedChildAt84() - 1;
        if (eventValue < dialog_->FirstVisibleChildAt80())
            eventValue = 3;
        latch = 1;
    }
    else if (ReadInputAtVslot70(10))
    {
        // 0x51F9AE..0x51FA15。可见窗口固定为五项，越界回到表首事件 0。
        eventValue = dialog_->FocusedChildAt84() + 1;
        if (eventValue >= dialog_->FirstVisibleChildAt80() + 5)
            eventValue = 0;
        latch = 2;
    }
    else
    {
        return ProcessBaseUpperInputAt51E740();
    }

    auto target = ResolveChildAt509D20(eventValue);
    if (!target)
        throw std::logic_error("OptionCharSelectDialog paging target is missing");
    SubmitActiveControlAt532170(target);
    pagingInputLatchAt6C_ = latch;
    target->WriteAtVslot70(1);
    return 1;
}

std::int32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ProcessBaseUpperInputAt51E740()
{
    // 本类的导航组 vector 为空，且 +0x40 固定返回 0；保留 0x51E740 在该
    // 派生类中可达的取消、鼠标按下和 +0x38 分支，不引入不存在的导航组。
    if (inheritedServices_->ReadCancelAtVslot7C() || ReadPressAtVslot80())
    {
        ResetDialogBaseAt510750();
        return 0;
    }
    if (activeControlAt4C_ && ProcessPagingReleaseAt510EB0())
        return 1;
    return 0;
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ProcessPagingReleaseAt510EB0()
{
    if (!activeControlAt4C_ || activeControlAt4C_->InputModeAt08() != 0)
        return false;

    const std::int32_t oldLatch = pagingInputLatchAt6C_;
    if (oldLatch == 1 && !ReadInputAtVslot6C(9))
    {
        activeControlAt4C_->InvokeReferencedObjectAt3CAtVslot00();
        pagingInputLatchAt6C_ = 0;
    }
    else if (oldLatch == 2 && !ReadInputAtVslot6C(10))
    {
        activeControlAt4C_->InvokeReferencedObjectAt3CAtVslot00();
        pagingInputLatchAt6C_ = 0;
    }
    return oldLatch != 0;
}

void SceneManagerOptionCharSelectDialogObjectGraphRuntime::
SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    const auto it = std::ranges::find_if(
        childRefs_,
        [&control](const CgOperationChildControlRef& candidate) {
            return candidate.get() == &control;
        });
    if (it == childRefs_.end())
        throw std::logic_error("OptionCharSelectDialog active control is not a child");
    SubmitActiveControlAt532170(*it);
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::ReadInputAtVslot6C(
    const std::int32_t eventValue)
{
    return inheritedServices_->ReadInputHeldAtVslot6C(eventValue);
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::ReadInputAtVslot70(
    const std::int32_t eventValue)
{
    return inheritedServices_->ReadInputPressedAtVslot70(eventValue);
}

POINT SceneManagerOptionCharSelectDialogObjectGraphRuntime::ReadPointerAtVslot74()
{
    return inheritedServices_->ReadPointerAtVslot74();
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::ReadPressAtVslot80()
{
    return inheritedServices_->ReadPointerPressAtVslot80();
}

bool SceneManagerOptionCharSelectDialogObjectGraphRuntime::ReadReleaseAtVslot84()
{
    return inheritedServices_->ReadPointerReleaseAtVslot84();
}

std::uint32_t SceneManagerOptionCharSelectDialogObjectGraphRuntime::
ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle,
    const std::int32_t x,
    const std::int32_t y)
{
    return inheritedServices_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}
}
