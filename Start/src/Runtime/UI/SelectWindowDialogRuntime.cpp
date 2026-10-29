#include <Runtime/SelectWindowDialogRuntime.h>
#include <Runtime/ScrVmMenuHandler.h>

#include <array>
#include <map>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
CgOperationControlResourceDescriptor ResourcesAt603C94()
{
    // 603C94 的七槽 CP932 原字节；只有同一资源组内部去重。
    CgOperationControlResourceDescriptor descriptor;
    auto& names = descriptor.resourceNamesAt04;
    names[0] = "\x91\x49\x91\xF0\x8E\x88.vrm";
    names[3] = "\x91\x49\x91\xF0\x8E\x88\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
    names[4] = names[3];
    names[5] = "\x91\x49\x91\xF0\x8E\x88\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
    return descriptor;
}

constexpr std::array<POINT, 5> PositionsAt607098{{
    {145, 237}, {145, 347}, {145, 200}, {145, 310}, {145, 420}}};
constexpr std::array<std::int32_t, 2> TwoChoices{{0, 1}};
constexpr std::array<std::int32_t, 3> ThreeChoices{{2, 3, 4}};
}

SelectWindowDialogDependencies::SelectWindowDialogDependencies(const ScrVmMenuState& menu,
    ICgOperationSurfaceFactory& surfaces, IGraphicsSurfaceCompositionApi& graphics,
    IGraphicsSurfacePixelApi& pixels, Platform::MouseView mouse,
    Platform::KeyboardView keyboard) noexcept
    : menu(menu), surfaces(surfaces), graphics(graphics), pixels(pixels),
      mouse_(mouse), keyboard_(keyboard)
{
}

SelectWindowDialogDependencies::SelectWindowDialogDependencies(const ScrVmMenuState& menu,
    ICgOperationSurfaceFactory& surfaces, IGraphicsSurfaceCompositionApi& graphics,
    IGraphicsSurfacePixelApi& pixels, const Platform::IInputRuntime& input) noexcept
    : menu(menu), surfaces(surfaces), graphics(graphics), pixels(pixels), input_(&input)
{
}

SelectWindowDialogRuntime::SelectWindowDialogRuntime(SelectWindowDialogDependencies dependencies)
    : dependencies_(dependencies), renderHost_(dependencies.graphics),
      inputAt18_(std::make_unique<NullGuiInputStateRuntime>())
{
    try
    {
        dirtyAt50_ = CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();
        if (!dirtyAt50_) throw std::logic_error("Select dialog dirty owner is null");
        for (std::size_t index = 0; index < PositionsAt607098.size(); ++index)
        {
            auto resources = std::make_shared<CgOperationControlResourceGroupRuntime>(
                dependencies_.surfaces, ResourcesAt603C94());
            auto child = std::make_shared<CgOperationChildControlRuntime>(
                CgOperationControlKind::Button, static_cast<std::int32_t>(index), -1, 7,
                std::move(resources));
            child->WritePositionAt34And38(PositionsAt607098[index]);
            auto event = std::make_shared<CgOperationClickedEventRuntime>(
                static_cast<std::int32_t>(index));
            child->BindClickedEventAt3C(event);
            child->WriteClickedEventEnabledAt48(1);
            child->BindButtonInputServicesAt4C(*this);
            children_.push_back(std::move(child));
            eventsAt7C_.push_back(std::move(event));
        }
        // 4F1B89..4F1BA7：仍在五个控件及事件提交后安装，不提前查询真实输入。
        auto gameInput = dependencies_.input_
            ? std::make_unique<GameGuiInputStateRuntime>(*dependencies_.input_)
            : std::make_unique<GameGuiInputStateRuntime>(*dependencies_.mouse_, *dependencies_.keyboard_);
        inputAt18_.reset();
        inputAt18_ = std::move(gameInput);
        // 构造不执行 Reset，也不写 +60/+64/+68。
    }
    catch (...)
    {
        // 仅复刻析构展开顺序；异常必须继续交给外层 45CC30。
        ReleaseOwnedAt4FE1E0();
        throw;
    }
}

SelectWindowDialogRuntime::~SelectWindowDialogRuntime()
{
    ReleaseOwnedAt4FE1E0();
}

void SelectWindowDialogRuntime::ReleaseOwnedAt4FE1E0() noexcept
{
    for (auto& event : eventsAt7C_) event.reset();
    eventsAt7C_.clear();
    dirtyAt50_.reset();
    for (auto& group : navigationAt24_) group.reset();
    navigationAt24_.clear();
    inputAt18_.reset();
    for (auto& child : children_) child.reset();
    children_.clear();
    activeAt4C_ = nullptr;
}

void SelectWindowDialogRuntime::AppendNavigation(const std::size_t count)
{
    // 50F2AC：重复 Reset 追加组，不能清空原组或把两方向合并成一个组。
    for (const std::int32_t action : {2, 1})
    {
        std::vector<GuiDialogNavigationMapping> mappings;
        if (count == 2)
            mappings = {{0, 1}, {1, 0}};
        else if (action == 2)
            mappings = {{2, 3}, {3, 4}, {4, 2}};
        else
            mappings = {{2, 4}, {3, 2}, {4, 3}};
        navigationAt24_.push_back(std::make_shared<GuiDialogNavigationGroupRuntime>(
            action, std::move(mappings)));
    }
}

void SelectWindowDialogRuntime::ResetAt50F000()
{
    WriteEnabledAtVslot24(1);
    for (const auto& event : eventsAt7C_) event->ClearAtVslot04();
    for (const auto& child : children_)
    {
        child->InvokeAtVslot08();
        child->WriteAtVslot4C(0);
        child->WriteAtVslot44(0);
    }
    ResetInputAt510820();
    activeAt4C_ = nullptr;
    const auto count = dependencies_.menu.CandidateCount();
    boundsAt6C_ = {};
    // 检查在前面副作用之后；0/1 对应原表空指针，>=4 对应原范围失败。
    if (count != 2 && count != 3) throw 1;
    const std::span<const std::int32_t> ids = count == 2
        ? std::span<const std::int32_t>(TwoChoices)
        : std::span<const std::int32_t>(ThreeChoices);
    for (const auto id : ids)
    {
        const auto child = FindControlAt509D20(id);
        if (!child) continue;
        child->WriteAtVslot4C(1);
        child->WriteAtVslot44(1);
        const RECT rect = child->ReadRectAtVslot3C();
        ::UnionRect(&boundsAt6C_, &boundsAt6C_, &rect);
    }
    AppendNavigation(count);
    phaseAt64_ = 3;
    resultIdAt60_ = -1;
}

std::int32_t SelectWindowDialogRuntime::ProcessAt530270()
{
    if (!phaseAt64_) throw std::logic_error("Select dialog phase has not been initialized");
    switch (*phaseAt64_)
    {
    case 0:
    case 1:
    case 2:
        return 1;
    case 3:
        UpdateAtVslot54();
        for (const auto& event : eventsAt7C_)
        {
            if (event->ClickedFlagAt04() == 0) continue;
            resultIdAt60_ = event->EventValueAt08();
            phaseAt64_ = 4;
            break;
        }
        return 1;
    case 4:
        SubmitRectAtVslot64(boundsAt6C_);
        for (const auto& event : eventsAt7C_) event->ClearAtVslot04();
        for (const auto& child : children_)
            if (std::static_pointer_cast<CgOperationChildControlRuntime>(child)->StateAt18() != 0)
                child->InvokeAtVslot08();
        ResetInputAt510820();
        activeAt4C_ = nullptr;
        phaseAt64_ = -1;
        return 1;
    default:
        return 0;
    }
}

std::int32_t SelectWindowDialogRuntime::ResultAt50ABD0() const
{
    std::int32_t ordinal = 0;
    for (const auto& child : children_)
    {
        const auto control = std::static_pointer_cast<CgOperationChildControlRuntime>(child);
        if (control->StateAt18() == 0) continue;
        if (!resultIdAt60_) throw std::logic_error("Select dialog result has not been initialized");
        if (control->EventValueAt04() == *resultIdAt60_) break;
        ++ordinal;
    }
    return ordinal;
}

RECT SelectWindowDialogRuntime::VisibleBoundsAt50AD40(const std::int32_t ordinal) const
{
    std::int32_t current = 0;
    for (const auto& child : children_)
    {
        const auto control = std::static_pointer_cast<CgOperationChildControlRuntime>(child);
        if (control->StateAt18() == 0) continue;
        if (current == ordinal) return child->ReadRectAtVslot3C();
        ++current;
    }
    return {};
}

void SelectWindowDialogRuntime::ResetInputAt510820()
{
    confirmLatchAt68_ = 0;
    SubmitActiveAt532170(nullptr);
    navigationLatchAt30_ = 0;
}

void SelectWindowDialogRuntime::SubmitActiveAt532170(ICgOperationChildControl* const control)
{
    if (activeAt4C_ == control) return;
    if (activeAt4C_) activeAt4C_->LoseFocusAtVslot24();
    if (control) control->GainFocusAtVslot20();
    activeAt4C_ = control;
}

std::int32_t SelectWindowDialogRuntime::ProcessConfirmAt511420()
{
    if (!activeAt4C_) throw std::logic_error("Select confirmation has no active control");
    if (activeAt4C_->InputModeAt08() != 0) return 0;
    if (!confirmLatchAt68_) throw std::logic_error("Select confirmation latch has not been initialized");
    const auto previous = *confirmLatchAt68_;
    if (previous == 0)
    {
        if (ReadInputAtVslot70(5))
        {
            confirmLatchAt68_ = 1;
            activeAt4C_->WriteAtVslot70(1);
            return 1;
        }
    }
    else if (!ReadInputAtVslot6C(5))
    {
        activeAt4C_->InvokeReferencedObjectAt3CAtVslot00();
        confirmLatchAt68_ = 0;
    }
    return previous;
}

std::int32_t SelectWindowDialogRuntime::ProcessNavigationAt51E740()
{
    if (inputAt18_->ReadPointerMovedAtVslot10() || ReadPressAtVslot80())
    {
        ResetInputAt510820();
        return 0;
    }
    // Select +40 固定返回0；+38 只允许 Button mode0，与 Title 的 mode2 不同。
    if (activeAt4C_ && ProcessConfirmAt511420() != 0) return 1;
    const auto unchanged = navigationLatchAt30_ != 0 ? 2 : 0;
    for (const auto& group : navigationAt24_)
    {
        if (!ReadInputAtVslot70(group->InputValueAt04())) continue;
        if (activeAt4C_)
        {
            const auto initial = activeAt4C_->EventValueAt04();
            auto candidate = initial;
            for (;;)
            {
                candidate = group->NextAtVslot00(candidate);
                if (candidate < 0 || candidate == initial) break;
                const auto child = FindControlAt509D20(candidate);
                if (!child) throw 1;
                const auto state = child->VisualStateAtVslot38();
                if (state == 1 || state == 2) continue;
                ResetChildrenAtVslot68();
                SubmitActiveAt532170(child.get());
                navigationLatchAt30_ = 1;
                return 3;
            }
        }
        else
        {
            for (const auto id : group->InitialCandidatesAt08())
            {
                const auto child = FindControlAt509D20(id);
                if (!child) throw 1;
                const auto state = child->VisualStateAtVslot38();
                if (state == 1 || state == 2) continue;
                SubmitActiveAt532170(child.get());
                navigationLatchAt30_ = 1;
                return 3;
            }
        }
        // 第一组输入命中后结束；不能因旧组无法选中而改试后面的组。
        return unchanged;
    }
    return unchanged;
}

void SelectWindowDialogRuntime::UpdateAtVslot54()
{
    if (EnabledAt38() == 0 || ModeAt34() == 0) return;
    std::map<std::int32_t, std::int32_t> previous;
    for (const auto& child : children_)
        previous.emplace(child->EventValueAt04(), child->VisualStateAtVslot38());
    if (ProcessNavigationAt51E740() == 0)
    {
        for (const auto& child : children_)
            if (child->ProcessAtVslot14() == 0) (void)child->ProcessAtVslot18();
    }
    for (const auto& child : children_)
    {
        bool changed = previous.at(child->EventValueAt04()) != child->VisualStateAtVslot38();
        if (!changed && child->InputModeAt08() == 5)
            changed = child->ReadModeFiveChangedAtVslot6C() != 0;
        if (changed)
        {
            SubmitRectAtVslot64(child->ReadRectAtVslot3C());
            // +88=518A80 只有实参 Ref12 的释放，无声音或额外通知。
        }
    }
}

void SelectWindowDialogRuntime::ResetChildrenAtVslot68()
{
    for (const auto& child : children_) child->InvokeAtVslot08();
}

void SelectWindowDialogRuntime::SubmitRectAtVslot64(const RECT& rect)
{
    (void)OwnDirtyRectsAtVslot00().AddAtVslot08(rect);
}

ICgDisplayButtonGroupDirtyRects& SelectWindowDialogRuntime::OwnDirtyRectsAtVslot00()
{
    return *dirtyAt50_;
}

void SelectWindowDialogRuntime::DrawAtVslot60(const RECT& dirty)
{
    renderHost_.DrawChildrenAt5230B0(children_, dirty);
}

void SelectWindowDialogRuntime::CompositeAtVslot5C(
    YtlReferenceSlotRuntime& consumed, const RECT* clip)
{
    renderHost_.CompositeChildrenAt5231C0(children_, consumed, clip);
}

void SelectWindowDialogRuntime::WriteModeAtVslot1C(const std::int32_t value) noexcept
{
    renderHost_.WriteModeAt34(value);
}

void SelectWindowDialogRuntime::WriteEnabledAtVslot24(const std::int32_t value) noexcept
{
    renderHost_.WriteEnabledAt38(value);
}

std::int32_t SelectWindowDialogRuntime::CloseAtVslot4C() noexcept
{
    WriteEnabledAtVslot24(0);
    return 1;
}

std::int32_t SelectWindowDialogRuntime::ModeAt34() const noexcept { return renderHost_.ModeAt34(); }
std::int32_t SelectWindowDialogRuntime::EnabledAt38() const noexcept { return renderHost_.EnabledAt38(); }

std::shared_ptr<CgOperationChildControlRuntime>
SelectWindowDialogRuntime::FindControlAt509D20(const std::int32_t id) const
{
    for (const auto& child : children_)
        if (child->EventValueAt04() == id)
            return std::static_pointer_cast<CgOperationChildControlRuntime>(child);
    return {};
}

std::span<const CgOperationChildControlRef> SelectWindowDialogRuntime::Children() const noexcept { return children_; }
std::span<const CgOperationClickedEventRef> SelectWindowDialogRuntime::EventsAt7C() const noexcept { return eventsAt7C_; }
std::span<const GuiDialogNavigationGroupRef> SelectWindowDialogRuntime::NavigationAt24() const noexcept { return navigationAt24_; }
const ICgOperationChildControl* SelectWindowDialogRuntime::ActiveAt4C() const noexcept { return activeAt4C_; }
const RECT& SelectWindowDialogRuntime::BoundsAt6C() const noexcept { return boundsAt6C_; }
const std::optional<std::int32_t>& SelectWindowDialogRuntime::ResultIdAt60() const noexcept { return resultIdAt60_; }
const std::optional<std::int32_t>& SelectWindowDialogRuntime::PhaseAt64() const noexcept { return phaseAt64_; }
const std::optional<std::int32_t>& SelectWindowDialogRuntime::ConfirmLatchAt68() const noexcept { return confirmLatchAt68_; }
std::int32_t SelectWindowDialogRuntime::NavigationLatchAt30() const noexcept { return navigationLatchAt30_; }

void SelectWindowDialogRuntime::SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    auto* const child = dynamic_cast<ICgOperationChildControl*>(&control);
    if (!child) throw std::invalid_argument("Select active object is not a child control");
    SubmitActiveAt532170(child);
}

bool SelectWindowDialogRuntime::ReadInputAtVslot6C(const std::int32_t action) { return inputAt18_->ReadHeldAtVslot00(action); }
bool SelectWindowDialogRuntime::ReadInputAtVslot70(const std::int32_t action) { return inputAt18_->ReadPressedAtVslot04(action); }
POINT SelectWindowDialogRuntime::ReadPointerAtVslot74() { return inputAt18_->ReadPointerAtVslot08(); }
bool SelectWindowDialogRuntime::ReadPressAtVslot80() { return inputAt18_->ReadMousePressAtVslot14(); }
bool SelectWindowDialogRuntime::ReadReleaseAtVslot84() { return inputAt18_->ReadMouseReleaseAtVslot18(); }
std::uint32_t SelectWindowDialogRuntime::ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle, const std::int32_t x, const std::int32_t y)
{
    return dependencies_.pixels.GetSurfacePixel(handle, x, y);
}
}
