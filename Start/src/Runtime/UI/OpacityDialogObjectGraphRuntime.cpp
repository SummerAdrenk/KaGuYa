#include <Runtime/OpacityDialogObjectGraphRuntime.h>

#include <algorithm>
#include <map>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
GuiDialogNavigationGroupRuntime::GuiDialogNavigationGroupRuntime(
    const std::int32_t inputValueAt04,
    std::vector<GuiDialogNavigationMapping> mappings)
    : inputValueAt04_(inputValueAt04),
      mappingsAt08_(std::move(mappings))
{
    initialCandidatesAt08_.reserve(mappingsAt08_.size());
    for (const auto& mapping : mappingsAt08_)
        initialCandidatesAt08_.push_back(mapping.sourceEventValue);
}

std::int32_t
GuiDialogNavigationGroupRuntime::InputValueAt04() const noexcept
{
    return inputValueAt04_;
}

std::int32_t GuiDialogNavigationGroupRuntime::NextAtVslot00(
    const std::int32_t currentEventValue) const
{
    const auto iterator = std::ranges::find_if(
        mappingsAt08_,
        [currentEventValue](const GuiDialogNavigationMapping& mapping) {
            return mapping.sourceEventValue == currentEventValue;
        });
    return iterator == mappingsAt08_.end()
        ? -1
        : iterator->targetEventValue;
}

std::span<const std::int32_t>
GuiDialogNavigationGroupRuntime::InitialCandidatesAt08() const noexcept
{
    return initialCandidatesAt08_;
}

std::span<const GuiDialogNavigationMapping>
GuiDialogNavigationGroupRuntime::MappingsAt08() const noexcept
{
    return mappingsAt08_;
}

OpacityDialogObjectGraphRuntime::OpacityDialogObjectGraphRuntime(
    ICgOperationSurfaceFactory& surfaceFactory,
    IGraphicsSurfaceCompositionApi& graphics,
    ICgOperationSliderInputSource& inputServices)
    : renderHost_(graphics),
      inputServices_(&inputServices)
{
    // 4E0D42..4E0DF3：独立 dirty 在 Static/Slider 之前创建。
    dirtyAt50_ = CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();
    if (!dirtyAt50_) throw std::logic_error("OpacityDialog dirty owner is null");
    try
    {
    // 4E4809..4E4871：先由 0x606FE4 创建 Static，再加入 Dialog 子控件表。
    const auto& staticDescriptor =
        OpacityDialogStaticDescriptorSample18At606FE4();
    auto staticResources =
        std::make_shared<CgOperationControlResourceGroupRuntime>(
            surfaceFactory, staticDescriptor.resources);
    backgroundStatic_ = std::make_shared<CgOperationChildControlRuntime>(
        staticDescriptor.kind,
        staticDescriptor.eventValueAt04,
        staticDescriptor.childIdAt0C,
        staticDescriptor.initialResourceSlotAt24,
        std::move(staticResources));
    children_.push_back(backgroundStatic_);
    dialogRectAt5C_ = backgroundStatic_->RectAtVslot3C();

    // 4E48BF..4E497A：随后创建 Slider、加入子控件表，再依次设置范围和
    // 步长（5BE170表+68=5384F0），不是+74的方向键步进开关。
    slider_ = CreateCgOperationSliderAt5052C0(
        surfaceFactory,
        *this,
        OpacityDialogSliderDescriptorSample18At607008());
    children_.push_back(slider_);
    RECT combined{};
    const RECT& sliderRect = slider_->BoundsRect();
    ::UnionRect(&combined, &dialogRectAt5C_, &sliderRect);
    dialogRectAt5C_ = combined;
    slider_->SetRangeAt537680(0, 255);
    slider_->SetStepAt5384F0(1);
    }
    catch (...)
    {
        ReleaseChildrenAndDirty();
        throw;
    }
}

OpacityDialogObjectGraphRuntime::~OpacityDialogObjectGraphRuntime()
{
    ReleaseChildrenAndDirty();
}

void OpacityDialogObjectGraphRuntime::ReleaseChildrenAndDirty() noexcept
{
    // 4F7AC0：dirty -> 导航 -> 输入 -> 正序 children；输入在现代对象中借用。
    dirtyAt50_.reset();
    for (auto& group : navigationGroupsAt24_) group.reset();
    navigationGroupsAt24_.clear();
    activeControlAt4C_ = nullptr;
    backgroundStatic_.reset();
    slider_.reset();
    for (auto& child : children_) child.reset();
    children_.clear();
}

ICgDisplayButtonGroupDirtyRects&
OpacityDialogObjectGraphRuntime::OwnDirtyRectsAtVslot00() noexcept
{
    return *dirtyAt50_;
}

void OpacityDialogObjectGraphRuntime::DrawAt5230B0(const RECT& clip)
{
    renderHost_.DrawChildrenAt5230B0(children_, clip);
}

void OpacityDialogObjectGraphRuntime::CompositeAt5231C0(
    YtlReferenceSlotRuntime& consumed,
    const RECT* const destinationClip)
{
    renderHost_.CompositeChildrenAt5231C0(
        children_, consumed, destinationClip);
}

std::int32_t OpacityDialogObjectGraphRuntime::ProcessSliderInputAt521820()
{
    return slider_->ProcessInputAt521820();
}

std::int32_t OpacityDialogObjectGraphRuntime::ProcessUpperInputPathAt51E740()
{
    // 51E76A..51E78F：+0x7C/+0x80 任一成立时先执行 +0x3C，
    // 即清除 active control 和 +0x30 latch，再以 0 返回。
    if (ReadInputAtVslot7C() || ReadPressAtVslot80())
    {
        ResetInputAt5106B0();
        return 0;
    }

    // OpacityDialog 的 +0x40=0x511610、+0x38=0x510B90，二者均固定返回 0。
    // 因而该派生类不会走 51E78F/51E7B8 的两个 return 1 分支。
    for (const auto& group : navigationGroupsAt24_)
    {
        if (!group)
            throw std::logic_error("GuiDialog +0x24 navigation group is null");
        if (!ReadInputAtVslot70(group->InputValueAt04()))
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

    // 51E7CA/51E84A：没有切换焦点时，+0x30 非零返回 2，否则返回 0。
    return inputLatchAt30_ != 0 ? 2 : 0;
}

void OpacityDialogObjectGraphRuntime::ProcessGenericDialogInputAt518C30()
{
    // GuiDialog +0x2C = 0x500800，只在 +0x34/+0x38 同时非零时许可。
    if (renderHost_.ModeAt34() == 0 || renderHost_.EnabledAt38() == 0)
        return;

    // 518C87..518D0B 使用 std::map::insert；重复 field +0x04 时保留
    // 首个快照，不能用 operator[] 覆盖。
    std::map<std::int32_t, std::int32_t> visualStates;
    for (const auto& control : children_)
    {
        visualStates.insert(
            {control->EventValueAt04(), control->VisualStateAtVslot38()});
    }

    // +0x34 是独立的 0x51E740 上层输入/导航状态机。非零只跳过普通
    // 控件轮询，后续视觉差异与模式 5 changed 检查仍必须执行。
    if (ProcessUpperInputPathAt51E740() == 0)
    {
        for (const auto& control : children_)
        {
            if (control->ProcessAtVslot14() == 0)
                static_cast<void>(control->ProcessAtVslot18());
        }
    }

    for (const auto& control : children_)
    {
        const std::int32_t before = visualStates[control->EventValueAt04()];
        const std::int32_t after = control->VisualStateAtVslot38();
        bool notify = before != after;
        if (!notify && control->InputModeAt08() == 5)
            notify = control->ReadModeFiveChangedAtVslot6C() != 0;
        if (!notify)
            continue;

        // 50FC80：此根 Dialog 的 parent 固定为空，先登记自身 dirty。
        (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());

        // OpacityDialog +0x88 = 0x518A80；函数体只销毁按值传入的 Ref12。
        // shared_ptr 的局部副本保留同样的增减引用时序，不添加额外回调。
        const CgOperationChildControlRef changedControl = control;
        static_cast<void>(changedControl);
    }
}

std::int32_t OpacityDialogObjectGraphRuntime::UpdateAtVslot48At52DF20(
    IOpacityDialogUpdateServices& services)
{
    // 52DF4A..52DFAF：child ID 1 是 Slider；通用 Dialog 输入必须位于
    // 两次取值之间，不能提前生成结果或只依据 Slider 的 changed 标志。
    const std::int32_t valueBefore = slider_->CurrentValueAtB4();
    ProcessGenericDialogInputAt518C30();
    const std::int32_t valueAfter = slider_->CurrentValueAtB4();
    if (valueBefore == valueAfter)
        return 1;

    // 52DFC3..52E0AF：这里读取 Layer 1 CMesWin 的 selector。负数或越界
    // 已由原 CMesWin 访问边界归入零类；服务只暴露归一化后的二分类结果。
    const bool selectorNonZero =
        services.SelectMessageWindowOpacitySelectorNonZeroAt52DF20();
    services.WriteSaveSlot1OpacitySelectorZeroOrNonZero(
        selectorNonZero, valueAfter);
    services.RefreshMessageWindowStyleAt5325F0(
        selectorNonZero ? 1 : 0);
    return 1;
}

void OpacityDialogObjectGraphRuntime::WriteEnabledAtVslot24(
    const std::int32_t value) noexcept
{
    // OpacityDialog vslot +0x24 = 0x538660，只写 GuiDialog+0x38。
    renderHost_.WriteEnabledAt38(value);
}

void OpacityDialogObjectGraphRuntime::ResetAtVslot44At50D730(
    const IOpacityDialogResetStateServices& stateServices)
{
    // 50D759..50D805：先启用 Dialog，再按 child ID 0、1 重置两项。
    WriteEnabledAtVslot24(1);
    backgroundStatic_->InvokeAtVslot08();
    slider_->InvokeAtVslot08();

    // 50D857..50D8E2：owning[1] CMesWin 的样式决定 Save+44/+4C。
    const bool selectorNonZero =
        stateServices.SelectMessageWindowOpacitySelectorNonZeroAt50D730();
    slider_->SetValueAt538520(
        stateServices.ReadSaveSlot1OpacitySelectorZeroOrNonZero(
            selectorNonZero));
    // 50D911：直接清借用指针，不调用 LoseFocus，也不清 inputLatchAt30。
    activeControlAt4C_ = nullptr;
}

const std::shared_ptr<CgOperationChildControlRuntime>&
OpacityDialogObjectGraphRuntime::BackgroundStatic() const noexcept
{
    return backgroundStatic_;
}

const std::shared_ptr<CgOperationSliderRuntime>&
OpacityDialogObjectGraphRuntime::Slider() const noexcept
{
    return slider_;
}

const std::vector<CgOperationChildControlRef>&
OpacityDialogObjectGraphRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const RECT& OpacityDialogObjectGraphRuntime::DialogRectAt5C() const noexcept
{
    return dialogRectAt5C_;
}

std::int32_t OpacityDialogObjectGraphRuntime::EnabledAt38() const noexcept
{
    return renderHost_.EnabledAt38();
}

std::int32_t OpacityDialogObjectGraphRuntime::InputLatchAt30() const noexcept
{
    return inputLatchAt30_;
}

ICgOperationChildControl*
OpacityDialogObjectGraphRuntime::ActiveControlAt4C() const noexcept
{
    return activeControlAt4C_;
}

void OpacityDialogObjectGraphRuntime::SetNavigationGroupsAt24ForConstruction(
    std::vector<GuiDialogNavigationGroupRef> navigationGroups)
{
    for (const auto& group : navigationGroups)
    {
        if (!group)
            throw std::invalid_argument("GuiDialog navigation group is null");
    }
    navigationGroupsAt24_ = std::move(navigationGroups);
}

void OpacityDialogObjectGraphRuntime::SetActiveControlAtVslot30(
    ICgOperationDialogEventTarget& control)
{
    const auto it = std::ranges::find_if(
        children_,
        [&control](const CgOperationChildControlRef& candidate) {
            return candidate.get() == &control;
        });
    if (it == children_.end())
        throw std::logic_error("GuiDialog active control is not a child");
    SubmitActiveControlAt532170(it->get());
}

bool OpacityDialogObjectGraphRuntime::ReadInputAtVslot70(
    const std::int32_t eventValue)
{
    return inputServices_->ReadInputAtVslot70(eventValue);
}

POINT OpacityDialogObjectGraphRuntime::ReadPointerAtVslot74()
{
    return inputServices_->ReadPointerAtVslot74();
}

std::int32_t OpacityDialogObjectGraphRuntime::ReadStepDirectionAtVslot78()
{
    return inputServices_->ReadStepDirectionAtVslot78();
}

bool OpacityDialogObjectGraphRuntime::ReadInputAtVslot7C()
{
    return inputServices_->ReadInputAtVslot7C();
}

bool OpacityDialogObjectGraphRuntime::ReadPressAtVslot80()
{
    return inputServices_->ReadPressAtVslot80();
}

bool OpacityDialogObjectGraphRuntime::ReadReleaseAtVslot84()
{
    return inputServices_->ReadReleaseAtVslot84();
}

std::uint32_t
OpacityDialogObjectGraphRuntime::ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle,
    const std::int32_t x,
    const std::int32_t y)
{
    return inputServices_->ReadSurfacePixelAtGetSurfacePixel(handle, x, y);
}

void OpacityDialogObjectGraphRuntime::SubmitActiveControlAt532170(
    ICgOperationChildControl* const control)
{
    // 532170：同一对象不重复失焦/获焦；切换时严格先旧对象 +0x24，
    // 再新对象 +0x20，最后提交 +0x4C。
    if (activeControlAt4C_ == control)
        return;
    if (activeControlAt4C_)
        activeControlAt4C_->LoseFocusAtVslot24();
    if (control)
        control->GainFocusAtVslot20();
    activeControlAt4C_ = control;
}

void OpacityDialogObjectGraphRuntime::ResetInputAt5106B0()
{
    SubmitActiveControlAt532170({});
    inputLatchAt30_ = 0;
}

void OpacityDialogObjectGraphRuntime::ResetChildrenAt522E90()
{
    for (const auto& child : children_)
        child->InvokeAtVslot08();
}

CgOperationChildControlRef
OpacityDialogObjectGraphRuntime::ResolveChildAt509D20(
    const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(
        children_,
        [eventValue](const CgOperationChildControlRef& child) {
            return child && child->EventValueAt04() == eventValue;
        });
    if (it == children_.end())
        return {};
    return *it;
}

bool OpacityDialogObjectGraphRuntime::TryActivateCandidateAt51E740(
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
    SubmitActiveControlAt532170(candidate.get());
    inputLatchAt30_ = 1;
    return true;
}
}
