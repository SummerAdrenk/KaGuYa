#include <Runtime/ToolWindowDialogRuntime.h>

#include <array>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct ControlRecord final
{
    std::int32_t eventValue;
    const char* labelCp932;
    bool checkBox;
};

// CP932 原始字节，对应 604EE0/604F90/605098 等静态记录；不依赖编译器代码页。
constexpr std::array<ControlRecord, 11> NormalRecords{{
    {8, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68", false},
    {7, "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75", false},
    {1, "\x83\x8D\x81\x5B\x83\x68", false},
    {0, "\x83\x5A\x81\x5B\x83\x75", false},
    {4, "\x83\x49\x81\x5B\x83\x67", true},
    {5, "\x83\x58\x83\x4C\x83\x62\x83\x76", true},
    {6, "\x83\x7B\x83\x43\x83\x58", false},
    {9, "\x93\xA7\x96\xBE\x93\x78", false},
    {10, "\x8D\x44\x8A\xB4\x93\x78", false},
    {11, "\x83\x5E\x83\x43\x83\x67\x83\x8B", false},
    {2, "\x94\xF1\x95\x5C\x8E\xA6", false},
}};
constexpr ControlRecord ReplayRecord{
    12, "\x83\x58\x83\x67\x83\x62\x83\x76", false};

std::string ResourceName(const char* state, const char* label)
{
    return std::string("\x83\x52\x83\x7D\x83\x93\x83\x68") + state + label + ".vrm";
}

CgOperationControlResourceDescriptor Resources(const ControlRecord& record,
    const bool checked)
{
    CgOperationControlResourceDescriptor result;
    auto& names = result.resourceNamesAt04;
    names[0] = ResourceName("\x83\x49\x83\x93", record.labelCp932);
    names[1] = ResourceName("\x83\x49\x83\x74", record.labelCp932);
    names[3] = ResourceName("\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67", record.labelCp932);
    names[4] = names[3];
    names[5] = ResourceName("\x83\x4E\x83\x8A\x83\x62\x83\x4E", record.labelCp932);
    names[6] = ResourceName("\x94\xCD\x88\xCD", record.labelCp932);
    if (checked)
    {
        names[0] = ResourceName("\x83\x76\x83\x8C\x83\x43", record.labelCp932);
        names[3] = names[0];
        names[4] = names[0];
        if (record.eventValue == 4)
            names[5] = ResourceName("\x83\x4E\x83\x8A\x83\x62\x83\x4E" "2", record.labelCp932);
        else
        {
            names[1] = ResourceName("\x83\x76\x83\x8C\x83\x43\x83\x49\x83\x74", record.labelCp932);
            names[5] = names[0];
        }
    }
    return result;
}
}

ToolWindowDialogDependencies::ToolWindowDialogDependencies(ICgOperationSurfaceFactory& surfaces,
    IGraphicsSurfaceCompositionApi& graphics, IGraphicsSurfacePixelApi& pixels,
    Platform::MouseView mouse, Platform::KeyboardView keyboard,
    const ToolWindowDialogRelationState& relation) noexcept
    : surfaces(surfaces), graphics(graphics), pixels(pixels), relation(relation),
      mouse_(mouse), keyboard_(keyboard)
{
}

ToolWindowDialogDependencies::ToolWindowDialogDependencies(ICgOperationSurfaceFactory& surfaces,
    IGraphicsSurfaceCompositionApi& graphics, IGraphicsSurfacePixelApi& pixels,
    const Platform::IInputRuntime& input, const ToolWindowDialogRelationState& relation) noexcept
    : surfaces(surfaces), graphics(graphics), pixels(pixels), relation(relation), input_(&input)
{
}

ToolWindowDialogRuntime::ToolWindowDialogRuntime(const std::int32_t mode,
    ToolWindowDialogDependencies dependencies)
    : dependencies_(dependencies), renderHost_(dependencies.graphics),
      inputAt18_(std::make_unique<NullGuiInputStateRuntime>())
{
    try
    {
        dirtyAt50_ = CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();
        if (!dirtyAt50_)
            throw std::logic_error("MessWinDialog dirty owner was not constructed");
        RECT bounds{};
        for (const auto& normal : NormalRecords)
        {
            // 4E287C 比较完整 DWORD；只有第四组的一条记录随非零 mode 改变。
            const auto& record = mode != 0 && normal.eventValue == 11 ? ReplayRecord : normal;
            auto off = std::make_shared<CgOperationControlResourceGroupRuntime>(
                dependencies_.surfaces, Resources(record, false));
            std::shared_ptr<CgOperationControlResourceGroupRuntime> on;
            if (record.checkBox)
            {
                on = std::make_shared<CgOperationControlResourceGroupRuntime>(
                    dependencies_.surfaces, Resources(record, true));
                RECT combined{};
                ::UnionRect(&combined, &off->CombinedRect(), &on->CombinedRect());
                // 4E0520 同时回写两组 RECT，但不移动任何 Surface 的原点。
                off->WriteCombinedRectForConstruction(combined);
                on->WriteCombinedRectForConstruction(combined);
            }
            auto control = std::make_shared<CgOperationChildControlRuntime>(
                record.checkBox ? CgOperationControlKind::CheckBox : CgOperationControlKind::Button,
                record.eventValue, record.eventValue == 4 ? 26 : -1, 6, off, on);
            const RECT rect = control->ReadRectAtVslot3C();
            RECT combined{};
            ::UnionRect(&combined, &bounds, &rect);
            bounds = combined;
            auto event = std::make_shared<CgOperationClickedEventRuntime>(record.eventValue);
            control->BindClickedEventAt3C(event);
            control->WriteClickedEventEnabledAt48(1);
            control->BindButtonInputServicesAt4C(*this);
            // 4E30F5..4E31A8：先 append 控件，再 append 对应的事件引用。
            children_.push_back(std::move(control));
            eventsAt70_.push_back(std::move(event));
        }
        boundsAt60_ = bounds;
        // 4E3250..4E326F：仍在全部控件构造后安装，稳定来源交给查询对象晚取。
        auto gameInput = dependencies_.input_
            ? std::make_unique<GameGuiInputStateRuntime>(*dependencies_.input_)
            : std::make_unique<GameGuiInputStateRuntime>(*dependencies_.mouse_, *dependencies_.keyboard_);
        inputAt18_.reset();
        inputAt18_ = std::move(gameInput);
        // 4E27F0 未写 event+5C，也没有调用 50CEF0。
    }
    catch (...)
    {
        ReleaseOwnedAt4FCA80();
        throw;
    }
}

ToolWindowDialogRuntime::~ToolWindowDialogRuntime()
{
    ReleaseOwnedAt4FCA80();
}

void ToolWindowDialogRuntime::ReleaseOwnedAt4FCA80() noexcept
{
    // 先减 derived 事件引用，再释放 base dirty/input/children；控件仍可持有事件。
    for (auto& event : eventsAt70_) event.reset();
    eventsAt70_.clear();
    dirtyAt50_.reset();
    inputAt18_.reset();
    for (auto& child : children_) child.reset();
    children_.clear();
    activeAt4C_ = nullptr;
}

void ToolWindowDialogRuntime::ResetControlsAt532480()
{
    for (const auto& control : children_)
    {
        control->InvokeAtVslot08();
        if (control->InputModeAt08() == 2)
            std::static_pointer_cast<CgOperationChildControlRuntime>(control)->WriteCheckBoxStateAt5037D0(0);
    }
}

void ToolWindowDialogRuntime::SetEnabledAtVslot24(const std::uint8_t value)
{
    renderHost_.WriteEnabledAt38(value);
}

void ToolWindowDialogRuntime::ResetChildrenForCaptureAt5325A0()
{
    // 5325A5虚槽+28=510630，仅读DWORD enabled+38，不检查mode或Tool图层enabled。
    if (EnabledAt38() == 0) return;
    for (const auto& control : children_)
    {
        if (!control) throw std::logic_error("5325A0 child reference is null");
        control->InvokeAtVslot08(); // 5325D7：不按child的enabled/type增加分支。
    }
}

const RECT& ToolWindowDialogRuntime::BoundsAt60() const
{
    return boundsAt60_;
}

void ToolWindowDialogRuntime::ResetAtVslot44()
{
    SetEnabledAtVslot24(1);
    for (const auto& event : eventsAt70_) event->ClearAtVslot04();
    ResetControlsAt532480();
    eventAt5C_ = -1;
    activeAt4C_ = nullptr;
}

void ToolWindowDialogRuntime::DrawAtVslot60(const RECT& dirty)
{
    renderHost_.DrawChildrenAt5230B0(children_, dirty);
}

void ToolWindowDialogRuntime::CompositeAtVslot5C(YtlReferenceSlotRuntime& consumed,
    const RECT* const clip)
{
    renderHost_.CompositeChildrenAt5231C0(children_, consumed, clip);
}

void ToolWindowDialogRuntime::SubmitActiveControlAt532170(ICgOperationDialogEventTarget* control)
{
    if (activeAt4C_ != nullptr)
    {
        if (activeAt4C_ == control) return;
        activeAt4C_->LoseFocusAtVslot24();
    }
    if (control != nullptr) control->GainFocusAtVslot20();
    activeAt4C_ = control;
}

void ToolWindowDialogRuntime::ProcessGenericInputAt518C30()
{
    if (EnabledAt38() == 0 || ModeAt34() == 0) return;
    std::map<std::int32_t, std::int32_t> previous;
    for (const auto& control : children_)
        previous.emplace(control->EventValueAt04(), control->VisualStateAtVslot38());

    // 51E740：先检测指针移动/按下并清 active；本类 +38/+40 确定返回 0，
    // 4E27F0 的导航 vector 始终为空，+30 latch 仅由 base 初始化为 0。
    if (inputAt18_->ReadPointerMovedAtVslot10() || inputAt18_->ReadMousePressAtVslot14())
        SubmitActiveControlAt532170(nullptr);
    for (const auto& control : children_)
    {
        if (control->ProcessAtVslot14() == 0)
            (void)control->ProcessAtVslot18();
    }
    for (const auto& control : children_)
    {
        if (previous.at(control->EventValueAt04()) != control->VisualStateAtVslot38())
        {
            (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());
            // 本类 +88=518A80 只释放实参 Ref12，不调用 Title 的声音/通知。
        }
    }
}

void ToolWindowDialogRuntime::ReconcileControlsAt527720()
{
    const auto control100 = FindControlAt509D20(dependencies_.relation.controlIdAt606FE0);
    if (!control100 || control100->ReadAtVslot48() == 0) return;
    const auto control10 = FindControlAt509D20(dependencies_.relation.relatedIdAt606FDC);
    const std::int32_t value = control10 && control10->StateAt14() != 0 &&
        control10->StateAt18() != 0 && activeAt4C_ == control10.get();
    if (control100->StateAt18() == value) return;
    control100->WriteAtVslot4C(value);
    (void)dirtyAt50_->AddAtVslot08(control100->ReadRectAtVslot3C());
}

void ToolWindowDialogRuntime::UpdateAtVslot54()
{
    eventAt5C_ = -1;
    ProcessGenericInputAt518C30();
    ReconcileControlsAt527720();
    for (const auto& event : eventsAt70_)
    {
        if (event->ClickedFlagAt04() != 0)
        {
            eventAt5C_ = event->EventValueAt08();
            event->ClearAtVslot04();
            return;
        }
    }
}

std::int32_t ToolWindowDialogRuntime::ResultAtVslot50()
{
    if (!eventAt5C_)
        throw std::logic_error("MessWinDialog event +5C has not been written by reset/update");
    return *eventAt5C_;
}

const std::optional<std::int32_t>& ToolWindowDialogRuntime::EventAt5C() const noexcept
{
    return eventAt5C_;
}

ICgDisplayButtonGroupDirtyRects& ToolWindowDialogRuntime::OwnDirtyRectsAtVslot00()
{
    return *dirtyAt50_;
}

void ToolWindowDialogRuntime::ResetControlAndBoundEvent(CgOperationChildControlRuntime& control)
{
    control.InvokeAtVslot08();
    const auto event = control.ClickedEventAt3C();
    if (!event)
        throw std::logic_error("MessWinDialog control +3C event reference is null");
    event->ClearAtVslot04();
}

void ToolWindowDialogRuntime::SetControlEnabledAt508470(const std::int32_t id,
    const std::int32_t enabled)
{
    const auto control = FindControlAt509D20(id);
    if (!control || control->StateAt18() == 0) return;
    const auto previous = control->ReadAtVslot48();
    if (enabled != 0) ResetControlAndBoundEvent(*control);
    control->WriteAtVslot44(enabled != 0 ? 1 : 0);
    if (control->ReadAtVslot48() != previous)
        (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());
}

void ToolWindowDialogRuntime::SetControlCheckedAt501FA0(const std::int32_t id,
    const std::int32_t checked)
{
    const auto control = FindControlAt509D20(id);
    if (!control || control->StateAt18() == 0 || control->InputModeAt08() != 2) return;
    const auto previous = control->CheckBoxStateAt5C();
    control->WriteCheckBoxStateAt5037D0(checked != 0 ? 1 : 0);
    if (control->CheckBoxStateAt5C() != previous)
        (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());
}

void ToolWindowDialogRuntime::ResetControlAt5312B0(const std::int32_t id)
{
    const auto control = FindControlAt509D20(id);
    if (!control || control->StateAt18() == 0) return;
    const auto previous = control->VisualStateAtVslot38();
    ResetControlAndBoundEvent(*control);
    if (control->VisualStateAtVslot38() != previous)
        (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());
}

void ToolWindowDialogRuntime::InvalidateAllControlsAt530DC0()
{
    for (const auto& control : children_)
        (void)dirtyAt50_->AddAtVslot08(control->ReadRectAtVslot3C());
}

void ToolWindowDialogRuntime::ClearActiveControlAtVslot30()
{
    SubmitActiveControlAt532170(nullptr);
}

void ToolWindowDialogRuntime::SetInputByteAtVslot1C(const std::uint8_t value)
{
    WriteModeAtVslot1C(value);
}

std::int32_t ToolWindowDialogRuntime::ScanControlStateAt50FD20()
{
    for (const auto& child : children_)
    {
        const auto control = std::static_pointer_cast<CgOperationChildControlRuntime>(child);
        if (control->StateAt14() == 0 || control->StateAt18() == 0) continue;
        const auto state = control->VisualStateAtVslot38();
        if (state == 4) return 1;
        if (state == 5) return 2;
    }
    return 0;
}

void ToolWindowDialogRuntime::WriteModeAtVslot1C(const std::int32_t value) noexcept
{
    renderHost_.WriteModeAt34(value);
}

std::int32_t ToolWindowDialogRuntime::ModeAt34() const noexcept
{
    return renderHost_.ModeAt34();
}

std::int32_t ToolWindowDialogRuntime::EnabledAt38() const noexcept
{
    return renderHost_.EnabledAt38();
}

std::shared_ptr<CgOperationChildControlRuntime>
ToolWindowDialogRuntime::FindControlAt509D20(const std::int32_t eventValue) const
{
    for (const auto& control : children_)
        if (control->EventValueAt04() == eventValue)
            return std::static_pointer_cast<CgOperationChildControlRuntime>(control);
    return {};
}

std::span<const CgOperationChildControlRef>
ToolWindowDialogRuntime::ChildrenInInsertionOrder() const noexcept
{
    return children_;
}

const ICgOperationDialogEventTarget* ToolWindowDialogRuntime::ActiveControlAt4C() const noexcept
{
    return activeAt4C_;
}

void ToolWindowDialogRuntime::SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control)
{
    SubmitActiveControlAt532170(&control);
}

bool ToolWindowDialogRuntime::ReadInputAtVslot6C(const std::int32_t action)
{
    return inputAt18_->ReadHeldAtVslot00(action);
}

bool ToolWindowDialogRuntime::ReadInputAtVslot70(const std::int32_t action)
{
    return inputAt18_->ReadPressedAtVslot04(action);
}

POINT ToolWindowDialogRuntime::ReadPointerAtVslot74()
{
    return inputAt18_->ReadPointerAtVslot08();
}

bool ToolWindowDialogRuntime::ReadPressAtVslot80()
{
    return inputAt18_->ReadMousePressAtVslot14();
}

bool ToolWindowDialogRuntime::ReadReleaseAtVslot84()
{
    return inputAt18_->ReadMouseReleaseAtVslot18();
}

std::uint32_t ToolWindowDialogRuntime::ReadSurfacePixelAtGetSurfacePixel(
    const GraphicsSurfaceHandle handle, const std::int32_t x, const std::int32_t y)
{
    return dependencies_.pixels.GetSurfacePixel(handle, x, y);
}

ToolWindowDialogFactoryRuntime::ToolWindowDialogFactoryRuntime(
    ToolWindowDialogDependencies dependencies) : dependencies_(dependencies)
{
}

std::shared_ptr<IToolWindowDialog> ToolWindowDialogFactoryRuntime::CreateDialogAt4E27F0(
    const std::int32_t mode)
{
    return std::make_shared<ToolWindowDialogRuntime>(mode, dependencies_);
}
}
