#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/CgOperationSurfaceResourceRuntime.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
[[nodiscard]] GraphicsSurfaceDestination ResolveCompositeDestination(
    const YtlReferenceSlotRuntime& argument)
{
    const auto* body = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(argument);
    if (!body) throw std::invalid_argument("CG composite destination body is null");
    return GraphicsSurfaceDestination::FromNative(*body);
}

void CompositeReferencedChildrenAt5231C0(IGraphicsSurfaceCompositionApi& graphics,
    const std::vector<CgOperationChildControlRef>& children,
    const std::int32_t& modeAt34, const std::int32_t& enabledAt38,
    YtlReferenceSlotRuntime& argument, const RECT* destinationClip)
{
    CgOperationSurfaceArgument parameter(argument);
    if (enabledAt38 != 0)
    {
        for (std::size_t index = 0; index < children.size(); ++index)
        {
            // 523205/52331B：mode 和末端逐轮重读，各次虚调用重新取当前槽。
            if (modeAt34 != 0)
            {
                YtlReferenceSlotRuntime copied;
                CopyGraphicsSurfaceReferenceAt47D740(copied, argument);
                children[index]->CompositeReferenceAtVslot10(graphics, copied, destinationClip);
            }
            else
            {
                const auto saved = children[index]->ReadAtVslot48();
                children[index]->WriteAtVslot44(0);
                YtlReferenceSlotRuntime copied;
                CopyGraphicsSurfaceReferenceAt47D740(copied, argument);
                children[index]->CompositeReferenceAtVslot10(graphics, copied, destinationClip);
                children[index]->WriteAtVslot44(saved);
            }
        }
    }
    parameter.FinishNormally();
}
}

void ICgOperationChildControl::CompositeReferenceAtVslot10(
    IGraphicsSurfaceCompositionApi& graphics, YtlReferenceSlotRuntime& argument,
    const RECT* const destinationClip)
{
    CgOperationSurfaceArgument parameter(argument);
    static_cast<void>(CompositeAtVslot10(graphics,
        ResolveCompositeDestination(argument), destinationClip));
    parameter.FinishNormally();
}

CgOperationChildControlRuntime::CgOperationChildControlRuntime(
    const CgOperationControlKind kind,
    const std::int32_t eventValueAt04,
    const std::int32_t childIdAt0C,
    const std::int32_t initialResourceSlotAt54,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> resources,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> checkedResources)
    : kind_(kind),
      eventValueAt04_(eventValueAt04),
      childIdAt0C_(childIdAt0C),
      resourcesAt28_(std::move(resources)),
      checkedResourcesAt50_(std::move(checkedResources)),
      initialResourceSlotAt54_(initialResourceSlotAt54)
{
    if (!resourcesAt28_)
        throw std::invalid_argument("CGOperation child resource group is null");
    if (kind_ == CgOperationControlKind::CheckBox && !checkedResourcesAt50_)
    {
        throw std::invalid_argument(
            "GuiControl::CheckBox checked resource group is null");
    }
    if (kind_ == CgOperationControlKind::Button ||
        kind_ == CgOperationControlKind::CheckBox)
    {
        if (initialResourceSlotAt54_ < 0 || initialResourceSlotAt54_ > 7)
        {
            throw std::invalid_argument(
                "GuiControl::Button hit-test resource slot is outside 0..7");
        }
        // 0x536150 先取指定槽的 Surface 引用；空槽会把 +0x54 归一为 7。
        if (initialResourceSlotAt54_ < 7 &&
            resourcesAt28_->ResourceIndexForSlot(
                static_cast<std::size_t>(initialResourceSlotAt54_)) < 0)
        {
            initialResourceSlotAt54_ = 7;
        }
    }
    rectAt34_ = resourcesAt28_->CombinedRect();
}

CgOperationControlKind
CgOperationChildControlRuntime::ConstructionKind() const
{
    return kind_;
}

std::int32_t CgOperationChildControlRuntime::EventValueAt04() const
{
    return eventValueAt04_;
}

std::int32_t CgOperationChildControlRuntime::ChildIdAt0C() const
{
    return childIdAt0C_;
}

std::int32_t CgOperationChildControlRuntime::InputModeAt08() const
{
    if (kind_ == CgOperationControlKind::Static)
        return 1;
    if (kind_ == CgOperationControlKind::CheckBox)
        return 2;
    return 0;
}

void CgOperationChildControlRuntime::InvokeAtVslot08()
{
    // Static:531230；Button:531210。Button 还会清 +0x50/+0x58。
    stateAt20_ = 0;
    stateAt1C_ = 0;
    if (kind_ == CgOperationControlKind::Button ||
        kind_ == CgOperationControlKind::CheckBox)
    {
        stateAt50_ = 0;
        stateAt58_ = 0;
    }
}

bool CgOperationChildControlRuntime::DrawAtVslot0C(
    IGraphicsSurfaceCompositionApi& graphics,
    const RECT* const destinationClip)
{
    // Static:531D30 与 Button:5314A0 使用相同的坐标式，差异只在状态槽选择。
    const auto* const selected = ResolveCurrentSurface();
    if (selected == nullptr || !*selected)
        return false;
    const POINT position = DrawPositionAt503700(**selected);
    return graphics.DrawSurface(
        (*selected)->Handle(), position.x, position.y,
        nullptr, nullptr, destinationClip, 0);
}

bool CgOperationChildControlRuntime::CompositeAtVslot10(
    IGraphicsSurfaceCompositionApi& graphics,
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    // Static:531F20 与 Button:531580 均把同一资源和坐标交给 SurfaceBlt。
    const auto* const selected = ResolveCurrentSurface();
    if (selected == nullptr || !*selected)
        return false;
    const POINT position = DrawPositionAt503700(**selected);
    const auto targetHandle = destination.Handle();
    if (targetHandle == 0) return false;
    const auto sourceHandle = (*selected)->Handle();
    if (sourceHandle == 0) return false;
    return graphics.BlitSurface(
        targetHandle, sourceHandle, position.x, position.y,
        nullptr, nullptr, destinationClip, 0);
}

std::int32_t CgOperationChildControlRuntime::ProcessAtVslot14()
{
    // Static+0x14=0x51E640，固定返回 0；Button+0x14=0x51E650，
    // 处理 childId 对应的操作键按住/释放，优先于 +0x18 鼠标路径。
    if (kind_ == CgOperationControlKind::Static)
        return 0;
    if (kind_ != CgOperationControlKind::Button &&
        kind_ != CgOperationControlKind::CheckBox)
        throw std::logic_error("GuiControl child +0x14 reached an invalid kind");
    if (childIdAt0C_ == -1 || stateAt14_ == 0 || stateAt18_ == 0)
        return 0;
    if (buttonInputServicesAt4C_ == nullptr)
    {
        throw std::logic_error(
            "GuiControl::Button parent input services are not bound");
    }

    if (stateAt58_ == 0)
    {
        if (!buttonInputServicesAt4C_->ReadInputAtVslot70(childIdAt0C_))
            return 0;
        stateAt50_ = 1;
        stateAt58_ = 1;
        return 2;
    }

    if (buttonInputServicesAt4C_->ReadInputAtVslot6C(childIdAt0C_))
        return 2;

    stateAt50_ = 0;
    stateAt58_ = 0;
    if (clickedEventEnabledAt48_ != 0)
        InvokeReferencedObjectAt3CAtVslot00();
    return 1;
}

std::int32_t CgOperationChildControlRuntime::ProcessAtVslot18()
{
    // Static vslot +0x18 = 0x5215D0，固定返回 0。
    if (kind_ == CgOperationControlKind::Static)
        return 0;
    if (kind_ != CgOperationControlKind::Button &&
        kind_ != CgOperationControlKind::CheckBox)
        throw std::logic_error("GuiControl child +0x18 reached an invalid kind");
    if (buttonInputServicesAt4C_ == nullptr)
    {
        throw std::logic_error(
            "GuiControl::Button parent input services are not bound");
    }

    // 0x5214D9..0x5214E0：+0x14/+0x18 任一为零时立即返回，
    // 原函数不会顺便清 hover 或 press latch。
    if (stateAt14_ == 0 || stateAt18_ == 0)
        return 0;

    stateAt1C_ = 0;
    const POINT pointer = buttonInputServicesAt4C_->ReadPointerAtVslot74();
    if (HitTestButtonAt502EB0(pointer))
    {
        stateAt1C_ = 1;
        if (stateAt20_ == 0)
            buttonInputServicesAt4C_->SetActiveControlAtVslot30(*this);
    }

    // 0x52151F..0x52152D：移出命中区立即取消当前按压。
    if (stateAt1C_ == 0)
    {
        stateAt50_ = 0;
        return 0;
    }

    if (stateAt50_ == 0)
    {
        if (buttonInputServicesAt4C_->ReadPressAtVslot80())
            stateAt50_ = 1;
        return 0;
    }

    if (!buttonInputServicesAt4C_->ReadReleaseAtVslot84())
        return 0;

    stateAt50_ = 0;
    if (clickedEventEnabledAt48_ != 0)
        InvokeReferencedObjectAt3CAtVslot00();
    return 0;
}

void CgOperationChildControlRuntime::GainFocusAtVslot20()
{
    stateAt20_ = 1;
}

void CgOperationChildControlRuntime::LoseFocusAtVslot24()
{
    stateAt20_ = 0;
}

void CgOperationChildControlRuntime::WriteAtVslot44(
    const std::int32_t value)
{
    stateAt14_ = value;
}

std::int32_t CgOperationChildControlRuntime::ReadAtVslot48() const noexcept
{
    return stateAt14_;
}

void CgOperationChildControlRuntime::WriteAtVslot4C(
    const std::int32_t value)
{
    stateAt18_ = value;
}

void CgOperationChildControlRuntime::WriteAtVslot70(
    const std::int32_t value)
{
    if (kind_ != CgOperationControlKind::Button &&
        kind_ != CgOperationControlKind::CheckBox)
    {
        throw std::logic_error(
            "GuiControl::Static has no Button/CheckBox vslot +0x70");
    }
    stateAt50_ = value;
}

void CgOperationChildControlRuntime::BindButtonInputServicesAt4C(
    ICgOperationButtonInputServices& inputServices)
{
    if (kind_ != CgOperationControlKind::Button &&
        kind_ != CgOperationControlKind::CheckBox)
    {
        throw std::logic_error(
            "GuiControl::Static cannot bind Button input services");
    }
    buttonInputServicesAt4C_ = &inputServices;
}

void CgOperationChildControlRuntime::WritePositionAt34And38(
    const POINT& position) noexcept
{
    // 0x53AF9D..0x53AFA6 直接覆盖 GuiControl::Button+0x34/+0x38；
    // +0x3C 返回的命中矩形宽高保持由 CSurfaceGroup 决定。
    const LONG width = rectAt34_.right - rectAt34_.left;
    const LONG height = rectAt34_.bottom - rectAt34_.top;
    rectAt34_ = RECT{
        position.x,
        position.y,
        position.x + width,
        position.y + height};
}

void CgOperationChildControlRuntime::WriteRectAt34ForConstruction(
    const RECT& rect) noexcept
{
    // SaveLoadPopup:4EFFC0..4F000D 将 CheckBox 两个 SurfaceGroup 的并集
    // 左上和尺寸写回 Button+0x34。该写入只发生在构造阶段。
    rectAt34_ = rect;
}

void CgOperationChildControlRuntime::BindClickedEventAt3C(
    CgOperationClickedEventRef event)
{
    clickedEventAt3C_ = std::move(event);
}

void CgOperationChildControlRuntime::WriteClickedEventEnabledAt48(
    const std::int32_t value)
{
    clickedEventEnabledAt48_ = value;
}

void CgOperationChildControlRuntime::InvokeReferencedObjectAt3CAtVslot00()
{
    // ClickedEvent vslot 0 = 509C20，只把 +0x04 写为 1。
    if (clickedEventAt3C_)
        clickedEventAt3C_->WriteClickedFlagAt04(1);
}

std::int32_t
CgOperationChildControlRuntime::VisualStateAtVslot38() const noexcept
{
    return ResourceSlotAtVslot38();
}

RECT CgOperationChildControlRuntime::ReadRectAtVslot3C() const
{
    return rectAt34_;
}

std::int32_t
CgOperationChildControlRuntime::ReadModeFiveChangedAtVslot6C() const
{
    throw std::logic_error(
        "GuiControl vslot +0x6C is only valid for input mode 5");
}

const RECT& CgOperationChildControlRuntime::RectAtVslot3C() const noexcept
{
    return rectAt34_;
}

std::int32_t CgOperationChildControlRuntime::StateAt14() const noexcept
{
    return stateAt14_;
}

std::int32_t CgOperationChildControlRuntime::StateAt18() const noexcept
{
    return stateAt18_;
}

std::int32_t CgOperationChildControlRuntime::StateAt20() const noexcept
{
    return stateAt20_;
}

std::int32_t CgOperationChildControlRuntime::StateAt48() const noexcept
{
    return clickedEventEnabledAt48_;
}

std::int32_t CgOperationChildControlRuntime::StateAt50() const noexcept
{
    return stateAt50_;
}

std::int32_t
CgOperationChildControlRuntime::InitialResourceSlotAt54() const noexcept
{
    return initialResourceSlotAt54_;
}

std::int32_t CgOperationChildControlRuntime::StateAt58() const noexcept
{
    return stateAt58_;
}

const std::shared_ptr<CgOperationControlResourceGroupRuntime>&
CgOperationChildControlRuntime::ResourcesAt28() const noexcept
{
    return resourcesAt28_;
}

const std::shared_ptr<CgOperationControlResourceGroupRuntime>&
CgOperationChildControlRuntime::CurrentResourcesAtVslot5C() const noexcept
{
    if (kind_ == CgOperationControlKind::CheckBox && checkBoxStateAt5C_ != 0)
        return checkedResourcesAt50_;
    return resourcesAt28_;
}

void CgOperationChildControlRuntime::WriteCheckBoxStateAt5037D0(
    const std::int32_t value)
{
    if (kind_ != CgOperationControlKind::CheckBox)
    {
        throw std::logic_error(
            "GuiControl checked-state setter reached a non-CheckBox control");
    }
    checkBoxStateAt5C_ = value;
}

std::int32_t
CgOperationChildControlRuntime::CheckBoxStateAt5C() const noexcept
{
    return checkBoxStateAt5C_;
}

std::int32_t
CgOperationChildControlRuntime::ResourceSlotAtVslot38() const noexcept
{
    // Static:50AEE0 与 Button:50AE60 的前两项和末两项相同；
    // 只有 Button 在其间读取 +0x50 并选择槽 5。判断顺序不可交换。
    if (stateAt18_ == 0)
        return 2;
    if (stateAt14_ == 0)
        return 1;
    if ((kind_ == CgOperationControlKind::Button ||
         kind_ == CgOperationControlKind::CheckBox) &&
        stateAt50_ != 0)
        return 5;
    if (stateAt1C_ != 0)
        return 4;
    if (stateAt20_ != 0)
        return 3;
    return 0;
}

const std::shared_ptr<RuntimeGraphicsSurface>*
CgOperationChildControlRuntime::ResolveCurrentSurface() const
{
    const auto& resources = CurrentResourcesAtVslot5C();
    const std::int32_t slot = ResourceSlotAtVslot38();
    if (slot < 0 || slot >= 7 ||
        resources->ResourceIndexForSlot(static_cast<std::size_t>(slot)) < 0)
    {
        return nullptr;
    }
    return &resources->ResourceForSlot(static_cast<std::size_t>(slot));
}

POINT CgOperationChildControlRuntime::DrawPositionAt503700(
    const RuntimeGraphicsSurface& surface) const noexcept
{
    const RECT& combined = CurrentResourcesAtVslot5C()->CombinedRect();
    const auto wrap = [](const std::int32_t first,
                         const std::int32_t second,
                         const std::int32_t third) noexcept {
        return static_cast<std::int32_t>(
            static_cast<std::uint32_t>(first) +
            static_cast<std::uint32_t>(second) -
            static_cast<std::uint32_t>(third));
    };
    return POINT{
        wrap(rectAt34_.left, surface.OriginX(), combined.left),
        wrap(rectAt34_.top, surface.OriginY(), combined.top)};
}

bool CgOperationChildControlRuntime::HitTestButtonAt502EB0(
    const POINT& point)
{
    // 0x502EDD..0x502F06：外层矩形始终先判定，右/下边界沿用 PtInRect
    // 的排他语义。
    if (!::PtInRect(&rectAt34_, point))
        return false;

    // +0x54==7 表示只做矩形命中。非 7 槽已由 0x536150 语义在构造期
    // 归一化为真实存在的 Surface。
    if (initialResourceSlotAt54_ == 7)
        return true;

    const auto& surface = CurrentResourcesAtVslot5C()->ResourceForSlot(
        static_cast<std::size_t>(initialResourceSlotAt54_));
    if (!surface)
    {
        throw std::logic_error(
            "GuiControl::Button hit-test Surface disappeared after binding");
    }

    // 0x502F54..0x502FC6：只有 Surface+0x18 表示带 alpha 时才读取像素；
    // 坐标必须减去 0x503700 计算出的实际绘制原点，而不是联合矩形原点。
    if (!surface->HasAlpha())
        return true;
    const POINT origin = DrawPositionAt503700(*surface);
    const std::uint32_t pixel =
        buttonInputServicesAt4C_->ReadSurfacePixelAtGetSurfacePixel(
            surface->Handle(), point.x - origin.x, point.y - origin.y);
    return (pixel & 0xFF000000u) != 0;
}

CgOperationSliderRuntime::CgOperationSliderRuntime(
    ICgOperationSliderInputServices& inputServices,
    const std::int32_t eventValueAt04,
    const std::int32_t childIdAt0C,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> thumbResources,
    std::shared_ptr<CgOperationControlResourceGroupRuntime> trackResources,
    const std::int32_t orientationAt50,
    const std::int32_t reverseAt54,
    const RECT& thumbRectAt64,
    const RECT& travelOffsetAt7C,
    const std::int32_t hitTestResourceSlotAt94)
    : inputServices_(&inputServices),
      eventValueAt04_(eventValueAt04),
      childIdAt0C_(childIdAt0C),
      resourceGroupsAt28_{
          std::move(thumbResources), std::move(trackResources)},
      orientationAt50_(orientationAt50),
      reverseAt54_(reverseAt54),
      thumbRectAt64_(thumbRectAt64),
      travelRectAt7C_(thumbRectAt64)
{
    if (!resourceGroupsAt28_[0] || !resourceGroupsAt28_[1])
        throw std::invalid_argument("GuiControl::Slider resource group is null");

    // 0x4F3E16..0x4F3E31：第二个 RECT 逐分量加到第一个 RECT 上。
    travelRectAt7C_.left += travelOffsetAt7C.left;
    travelRectAt7C_.top += travelOffsetAt7C.top;
    travelRectAt7C_.right += travelOffsetAt7C.right;
    travelRectAt7C_.bottom += travelOffsetAt7C.bottom;

    RECT combined = resourceGroupsAt28_[1]->CombinedRect();
    ::UnionRect(&combined, &combined, &thumbRectAt64_);
    ::UnionRect(&combined, &combined, &travelRectAt7C_);
    boundsRectAt34_ = combined;

    const RECT& trackCombined = resourceGroupsAt28_[1]->CombinedRect();
    trackResourceOffsetAtC4_ = POINT{
        trackCombined.left - combined.left,
        trackCombined.top - combined.top};

    thumbRectOffsetAt5C_ = POINT{
        thumbRectAt64_.left - combined.left,
        thumbRectAt64_.top - combined.top};
    const LONG thumbWidth = thumbRectAt64_.right - thumbRectAt64_.left;
    const LONG thumbHeight = thumbRectAt64_.bottom - thumbRectAt64_.top;
    thumbRectAt64_ = RECT{
        combined.left,
        combined.top,
        combined.left + thumbWidth,
        combined.top + thumbHeight};

    travelOffsetAt74_ = POINT{
        travelRectAt7C_.left - combined.left,
        travelRectAt7C_.top - combined.top};
    const LONG travelWidth = travelRectAt7C_.right - travelRectAt7C_.left;
    const LONG travelHeight = travelRectAt7C_.bottom - travelRectAt7C_.top;
    travelRectAt7C_ = RECT{
        combined.left,
        combined.top,
        combined.left + travelWidth,
        combined.top + travelHeight};

    SetValueAt538520(0);
    SetHitTestResourceSlotAt5361E0(hitTestResourceSlotAt94);
}

CgOperationControlKind CgOperationSliderRuntime::ConstructionKind() const
{
    return CgOperationControlKind::Slider;
}

std::int32_t CgOperationSliderRuntime::EventValueAt04() const
{
    return eventValueAt04_;
}

std::int32_t CgOperationSliderRuntime::ChildIdAt0C() const
{
    return childIdAt0C_;
}

std::int32_t CgOperationSliderRuntime::InputModeAt08() const
{
    return 5;
}

void CgOperationSliderRuntime::InvokeAtVslot08()
{
    // 0x531270。
    hoveredAt90_ = 0;
    draggingAt8C_ = 0;
    changedAtB0_ = 0;
    pressedAtCC_ = 0;
    stateAt20_ = 0;
    stateAt1C_ = 0;
}

bool CgOperationSliderRuntime::DrawAtVslot0C(
    IGraphicsSurfaceCompositionApi& graphics,
    const RECT* const destinationClip)
{
    bool drew = false;
    const std::int32_t baseSlot = BaseResourceSlotAt50AEA0();
    if (const auto* track = ResolveResource(
            *resourceGroupsAt28_[1], baseSlot);
        track != nullptr && *track)
    {
        const POINT position = TrackDrawPosition(**track);
        drew = graphics.DrawSurface(
            (*track)->Handle(), position.x, position.y,
            nullptr, nullptr, destinationClip, 0) || drew;
    }

    if (const auto* thumb = ResolveResource(
            *resourceGroupsAt28_[0], ThumbResourceSlot());
        thumb != nullptr && *thumb)
    {
        const POINT position = ThumbDrawPosition(**thumb);
        drew = graphics.DrawSurface(
            (*thumb)->Handle(), position.x, position.y,
            nullptr, nullptr, destinationClip, 0) || drew;
    }
    return drew;
}

bool CgOperationSliderRuntime::CompositeAtVslot10(
    IGraphicsSurfaceCompositionApi& graphics,
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    return CompositeSelectedSurfaces(graphics, destination, destinationClip, nullptr);
}

void CgOperationSliderRuntime::CompositeReferenceAtVslot10(
    IGraphicsSurfaceCompositionApi& graphics, YtlReferenceSlotRuntime& argument,
    const RECT* const destinationClip)
{
    CgOperationSurfaceArgument parameter(argument);
    static_cast<void>(CompositeSelectedSurfaces(graphics, 0, destinationClip, &argument));
    parameter.FinishNormally();
}

bool CgOperationSliderRuntime::CompositeSelectedSurfaces(
    IGraphicsSurfaceCompositionApi& graphics,
    const GraphicsSurfaceDestination destination, const RECT* const destinationClip,
    const YtlReferenceSlotRuntime* const argument)
{
    bool drew = false;
    const std::int32_t baseSlot = BaseResourceSlotAt50AEA0();
    if (const auto* track = ResolveResource(
            *resourceGroupsAt28_[1], baseSlot);
        track != nullptr && *track)
    {
        const POINT position = TrackDrawPosition(**track);
        const auto targetHandle = (argument ? ResolveCompositeDestination(*argument) : destination).Handle();
        if (targetHandle != 0)
        {
            const auto sourceHandle = (*track)->Handle();
            if (sourceHandle != 0)
                drew = graphics.BlitSurface(targetHandle, sourceHandle, position.x, position.y,
                    nullptr, nullptr, destinationClip, 0) || drew;
        }
    }

    if (const auto* thumb = ResolveResource(
            *resourceGroupsAt28_[0], ThumbResourceSlot());
        thumb != nullptr && *thumb)
    {
        const POINT position = ThumbDrawPosition(**thumb);
        const auto targetHandle = (argument ? ResolveCompositeDestination(*argument) : destination).Handle();
        if (targetHandle != 0)
        {
            const auto sourceHandle = (*thumb)->Handle();
            if (sourceHandle != 0)
                drew = graphics.BlitSurface(targetHandle, sourceHandle, position.x, position.y,
                    nullptr, nullptr, destinationClip, 0) || drew;
        }
    }
    return drew;
}

std::int32_t CgOperationSliderRuntime::ProcessAtVslot14()
{
    return 0;
}

std::int32_t CgOperationSliderRuntime::ProcessAtVslot18()
{
    return ProcessInputAt521820();
}

void CgOperationSliderRuntime::GainFocusAtVslot20()
{
    stateAt20_ = 1;
}

void CgOperationSliderRuntime::LoseFocusAtVslot24()
{
    stateAt20_ = 0;
}

void CgOperationSliderRuntime::WriteAtVslot44(const std::int32_t value)
{
    stateAt14_ = value;
}

std::int32_t CgOperationSliderRuntime::ReadAtVslot48() const noexcept
{
    return stateAt14_;
}

void CgOperationSliderRuntime::WriteAtVslot4C(const std::int32_t value)
{
    stateAt18_ = value;
}

void CgOperationSliderRuntime::WriteAtVslot70(const std::int32_t)
{
    throw std::logic_error(
        "GuiControl::Slider vslot +0x70 is a getter, not a Button setter");
}

void CgOperationSliderRuntime::BindClickedEventAt3C(
    CgOperationClickedEventRef event)
{
    clickedEventAt3C_ = std::move(event);
}

void CgOperationSliderRuntime::WriteClickedEventEnabledAt48(
    const std::int32_t value)
{
    clickedEventEnabledAt48_ = value;
}

void CgOperationSliderRuntime::InvokeReferencedObjectAt3CAtVslot00()
{
    if (clickedEventAt3C_)
        clickedEventAt3C_->WriteClickedFlagAt04(1);
}

std::int32_t CgOperationSliderRuntime::VisualStateAtVslot38() const noexcept
{
    return BaseResourceSlotAt50AEA0();
}

RECT CgOperationSliderRuntime::ReadRectAtVslot3C() const
{
    // 0x50A260：返回值不是简单的 +0x34 静态 bounds。它把完整滑轨行程、
    // 当前滑块和第二资源组的实际矩形重新定位后取并集。
    RECT travel = travelRectAt7C_;
    const LONG travelWidth = travel.right - travel.left;
    const LONG travelHeight = travel.bottom - travel.top;
    travel.left = boundsRectAt34_.left;
    travel.top = boundsRectAt34_.top;
    travel.right = travel.left + travelWidth;
    travel.bottom = travel.top + travelHeight;
    ::OffsetRect(&travel, travelOffsetAt74_.x, travelOffsetAt74_.y);

    const RECT& thumbGroupRect = resourceGroupsAt28_[0]->CombinedRect();
    if (orientationAt50_ != 0)
        travel.bottom += thumbGroupRect.bottom - thumbGroupRect.top;
    else
        travel.right += thumbGroupRect.right - thumbGroupRect.left;

    RECT track = resourceGroupsAt28_[1]->CombinedRect();
    const LONG trackWidth = track.right - track.left;
    const LONG trackHeight = track.bottom - track.top;
    track.left = boundsRectAt34_.left + trackResourceOffsetAtC4_.x;
    track.top = boundsRectAt34_.top + trackResourceOffsetAtC4_.y;
    track.right = track.left + trackWidth;
    track.bottom = track.top + trackHeight;

    RECT thumb = thumbGroupRect;
    const LONG thumbWidth = thumb.right - thumb.left;
    const LONG thumbHeight = thumb.bottom - thumb.top;
    thumb.left = thumbPositionAt98_.x;
    thumb.top = thumbPositionAt98_.y;
    thumb.right = thumb.left + thumbWidth;
    thumb.bottom = thumb.top + thumbHeight;

    RECT combined{};
    ::UnionRect(&combined, &track, &thumb);
    RECT result{};
    ::UnionRect(&result, &combined, &travel);
    return result;
}

std::int32_t
CgOperationSliderRuntime::ReadModeFiveChangedAtVslot6C() const
{
    return ChangedAtB0();
}

void CgOperationSliderRuntime::SetRangeAt537680(
    const std::int32_t minimum,
    const std::int32_t maximum)
{
    minimumAtB8_ = minimum;
    maximumAtBC_ = maximum;
    SetValueAt538520(currentValueAtB4_);
}

void CgOperationSliderRuntime::SetStepAt5384F0(
    const std::int32_t step) noexcept
{
    const std::int32_t range = maximumAtBC_ - minimumAtB8_;
    stepAtC0_ = range < step ? range : step;
}

void CgOperationSliderRuntime::SetValueAt538520(
    const std::int32_t value)
{
    if (stepAtC0_ == 0 || maximumAtBC_ == minimumAtB8_)
        throw std::domain_error("GuiControl::Slider arithmetic divisor is zero");

    const std::int32_t snapped = value - value % stepAtC0_;
    currentValueAtB4_ = std::min(
        maximumAtBC_, std::max(minimumAtB8_, snapped));
    const std::int32_t effective = reverseAt54_ == 0
        ? currentValueAtB4_
        : minimumAtB8_ + maximumAtBC_ - currentValueAtB4_;
    RecalculateThumbPosition(effective);
}

void CgOperationSliderRuntime::SetStepInputEnabledAt538BA0(
    const std::int32_t value) noexcept
{
    stepInputEnabledAt58_ = value;
}

void CgOperationSliderRuntime::SetHoveredAt521A80() noexcept
{
    hoveredAt90_ = 1;
    stateAt1C_ = 1;
}

void CgOperationSliderRuntime::ClearHoveredAt521AB0() noexcept
{
    hoveredAt90_ = 0;
    stateAt1C_ = 0;
}

void CgOperationSliderRuntime::WriteDraggingAt8C(
    const std::int32_t value) noexcept
{
    draggingAt8C_ = value;
}

void CgOperationSliderRuntime::WritePressedAtCC(
    const std::int32_t value) noexcept
{
    pressedAtCC_ = value;
}

void CgOperationSliderRuntime::SetHitTestResourceSlotAt5361E0(
    const std::int32_t slot)
{
    hitTestResourceSlotAt94_ = 7;
    if (slot < 0 || slot >= 7)
        return;
    if (resourceGroupsAt28_[0]->ResourceIndexForSlot(
            static_cast<std::size_t>(slot)) >= 0)
    {
        hitTestResourceSlotAt94_ = slot;
    }
}

std::int32_t CgOperationSliderRuntime::ProcessInputAt521820()
{
    if (stateAt18_ == 0 || stateAt14_ == 0)
        return 0;

    changedAtB0_ = 0;
    const POINT pointer = inputServices_->ReadPointerAtVslot74();

    if (draggingAt8C_ != 0)
    {
        if (inputServices_->ReadReleaseAtVslot84())
        {
            draggingAt8C_ = 0;
            return 0;
        }

        const std::int32_t coordinate = orientationAt50_ == 0
            ? pointer.x - dragOffsetAtD0_.x
            : pointer.y - dragOffsetAtD0_.y;
        SetValueAt538520(ValueFromCoordinateAt53AE10(coordinate));
        changedAtB0_ = 1;
        return 0;
    }

    ClearHoveredAt521AB0();
    if (HitTestThumbAt502CE0(pointer))
    {
        SetHoveredAt521A80();
    }
    else
    {
        const RECT interaction = BaseInteractionRect();
        if (::PtInRect(&interaction, pointer) != FALSE)
            stateAt1C_ = 1;
    }

    if (stateAt1C_ != 0 && stateAt20_ == 0)
        inputServices_->SetActiveControlAtVslot30(*this);

    if (stateAt1C_ == 0 && hoveredAt90_ == 0)
    {
        pressedAtCC_ = 0;
        return 0;
    }

    if (pressedAtCC_ != 0)
    {
        if (inputServices_->ReadReleaseAtVslot84())
            pressedAtCC_ = 0;
        return 0;
    }

    if (!inputServices_->ReadPressAtVslot80())
    {
        if (stepInputEnabledAt58_ == 0)
            return 0;
        const std::int32_t direction =
            inputServices_->ReadStepDirectionAtVslot78();
        if (direction > 0)
            SetValueAt538520(currentValueAtB4_ - stepAtC0_);
        else if (direction < 0)
            SetValueAt538520(currentValueAtB4_ + stepAtC0_);
        else
            return 0;
        changedAtB0_ = 1;
        return 0;
    }

    if (hoveredAt90_ != 0)
    {
        const RECT thumb = ThumbHitRect();
        dragOffsetAtD0_ = POINT{
            pointer.x - thumb.left,
            pointer.y - thumb.top};
        draggingAt8C_ = 1;
        return 0;
    }

    const std::int32_t coordinate =
        orientationAt50_ == 0 ? pointer.x : pointer.y;
    SetValueAt538520(ValueFromCoordinateAt53AE10(coordinate));
    pressedAtCC_ = 1;
    return 0;
}

const RECT& CgOperationSliderRuntime::BoundsRect() const noexcept
{
    return boundsRectAt34_;
}

POINT CgOperationSliderRuntime::ThumbPositionAt98() const noexcept
{
    return thumbPositionAt98_;
}

std::int32_t CgOperationSliderRuntime::CurrentValueAtB4() const noexcept
{
    return currentValueAtB4_;
}

std::int32_t CgOperationSliderRuntime::MinimumAtB8() const noexcept
{
    return minimumAtB8_;
}

std::int32_t CgOperationSliderRuntime::MaximumAtBC() const noexcept
{
    return maximumAtBC_;
}

std::int32_t CgOperationSliderRuntime::StepAtC0() const noexcept
{
    return stepAtC0_;
}

std::int32_t CgOperationSliderRuntime::ReverseAtVslot70() const noexcept
{
    return reverseAt54_;
}

std::int32_t CgOperationSliderRuntime::ChangedAtB0() const noexcept
{
    return changedAtB0_;
}

std::int32_t CgOperationSliderRuntime::HitTestResourceSlotAt94() const noexcept
{
    return hitTestResourceSlotAt94_;
}

std::int32_t CgOperationSliderRuntime::BaseResourceSlotAt50AEA0() const noexcept
{
    if (stateAt18_ == 0)
        return 2;
    if (stateAt14_ == 0)
        return 1;
    if (pressedAtCC_ != 0)
        return 5;
    if (stateAt1C_ != 0)
        return 4;
    if (stateAt20_ != 0)
        return 3;
    return 0;
}

std::int32_t CgOperationSliderRuntime::ThumbResourceSlot() const noexcept
{
    const std::int32_t base = BaseResourceSlotAt50AEA0();
    if (base == 2 || base == 1)
        return base;
    if (draggingAt8C_ != 0)
        return 5;
    if (hoveredAt90_ != 0)
        return 4;
    return base == 3 ? 3 : 0;
}

const std::shared_ptr<RuntimeGraphicsSurface>*
CgOperationSliderRuntime::ResolveResource(
    const CgOperationControlResourceGroupRuntime& group,
    const std::int32_t slot) const
{
    if (slot < 0 || slot >= 7 ||
        group.ResourceIndexForSlot(static_cast<std::size_t>(slot)) < 0)
    {
        return nullptr;
    }
    return &group.ResourceForSlot(static_cast<std::size_t>(slot));
}

POINT CgOperationSliderRuntime::TrackDrawPosition(
    const RuntimeGraphicsSurface& surface) const noexcept
{
    const RECT& combined = resourceGroupsAt28_[1]->CombinedRect();
    return POINT{
        surface.OriginX() + boundsRectAt34_.left +
            trackResourceOffsetAtC4_.x - combined.left,
        surface.OriginY() + boundsRectAt34_.top +
            trackResourceOffsetAtC4_.y - combined.top};
}

POINT CgOperationSliderRuntime::ThumbDrawPosition(
    const RuntimeGraphicsSurface& surface) const noexcept
{
    const RECT& combined = resourceGroupsAt28_[0]->CombinedRect();
    return POINT{
        surface.OriginX() + thumbPositionAt98_.x - combined.left,
        surface.OriginY() + thumbPositionAt98_.y - combined.top};
}

RECT CgOperationSliderRuntime::TravelRect() const noexcept
{
    RECT result = travelRectAt7C_;
    ::OffsetRect(&result, travelOffsetAt74_.x, travelOffsetAt74_.y);
    return result;
}

RECT CgOperationSliderRuntime::BaseInteractionRect() const noexcept
{
    RECT result = thumbRectAt64_;
    ::OffsetRect(&result, thumbRectOffsetAt5C_.x, thumbRectOffsetAt5C_.y);
    return result;
}

RECT CgOperationSliderRuntime::ThumbHitRect() const noexcept
{
    const RECT& combined = resourceGroupsAt28_[0]->CombinedRect();
    return RECT{
        thumbPositionAt98_.x,
        thumbPositionAt98_.y,
        thumbPositionAt98_.x + combined.right - combined.left,
        thumbPositionAt98_.y + combined.bottom - combined.top};
}

bool CgOperationSliderRuntime::HitTestThumbAt502CE0(const POINT& point)
{
    const RECT thumb = ThumbHitRect();
    if (::PtInRect(&thumb, point) == FALSE)
        return false;
    if (hitTestResourceSlotAt94_ == 7)
        return true;

    const auto* const surface = ResolveResource(
        *resourceGroupsAt28_[0], hitTestResourceSlotAt94_);
    if (surface == nullptr || !*surface)
        throw std::runtime_error(
            "GuiControl::Slider hit-test resource is unavailable");
    if (!(*surface)->HasAlpha())
        return true;

    const POINT position = ThumbDrawPosition(**surface);
    return (inputServices_->ReadSurfacePixelAtGetSurfacePixel(
        (*surface)->Handle(),
        point.x - position.x,
        point.y - position.y) & 0xFF000000u) != 0u;
}

std::int32_t CgOperationSliderRuntime::ValueFromCoordinateAt53AE10(
    const std::int32_t coordinate) const
{
    const RECT travel = TravelRect();
    const std::int32_t extent = orientationAt50_ == 0
        ? travel.right - travel.left
        : travel.bottom - travel.top;
    if (extent == 0)
        throw std::domain_error("GuiControl::Slider travel extent is zero");

    const std::int32_t relative = orientationAt50_ == 0
        ? coordinate - travel.left
        : coordinate - travel.top;
    const float value =
        static_cast<float>(relative) *
        (static_cast<float>(maximumAtBC_ - minimumAtB8_) /
         static_cast<float>(extent)) +
        (static_cast<float>(minimumAtB8_) + 0.5F);
    const std::int32_t converted = static_cast<std::int32_t>(value);
    return reverseAt54_ == 0
        ? converted
        : maximumAtBC_ + minimumAtB8_ - converted;
}

void CgOperationSliderRuntime::RecalculateThumbPosition(
    const std::int32_t effectiveValue)
{
    const RECT travel = TravelRect();
    const std::int32_t extent = orientationAt50_ == 0
        ? travel.right - travel.left
        : travel.bottom - travel.top;
    const float offset =
        static_cast<float>(effectiveValue - minimumAtB8_) *
        static_cast<float>(extent) /
        static_cast<float>(maximumAtBC_ - minimumAtB8_);
    thumbPositionAt98_ = POINT{travel.left, travel.top};
    if (orientationAt50_ == 0)
        thumbPositionAt98_.x += static_cast<std::int32_t>(offset);
    else
        thumbPositionAt98_.y += static_cast<std::int32_t>(offset);
}

CgOperationDialogRenderHostRuntime::CgOperationDialogRenderHostRuntime(
    IGraphicsSurfaceCompositionApi& graphics) noexcept
    : graphics_(&graphics)
{
}

void CgOperationDialogRenderHostRuntime::WriteModeAt34(
    const std::int32_t value) noexcept
{
    modeAt34_ = value;
}

void CgOperationDialogRenderHostRuntime::WriteEnabledAt38(
    const std::int32_t value) noexcept
{
    enabledAt38_ = value;
}

std::int32_t CgOperationDialogRenderHostRuntime::ModeAt34() const noexcept
{
    return modeAt34_;
}

std::int32_t CgOperationDialogRenderHostRuntime::EnabledAt38() const noexcept
{
    return enabledAt38_;
}

void CgOperationDialogRenderHostRuntime::DrawChildrenAt5230B0(
    const std::span<const CgOperationChildControlRef> children,
    const RECT& clip)
{
    if (enabledAt38_ == 0)
        return;
    for (const auto& child : children)
    {
        if (!child)
            throw std::invalid_argument("GuiDialog child reference is null");
        if (modeAt34_ != 0)
        {
            static_cast<void>(child->DrawAtVslot0C(*graphics_, &clip));
            continue;
        }

        const std::int32_t previous = child->ReadAtVslot48();
        child->WriteAtVslot44(0);
        static_cast<void>(child->DrawAtVslot0C(*graphics_, &clip));
        child->WriteAtVslot44(previous);
    }
}

void CgOperationDialogRenderHostRuntime::CompositeChildrenAt5231C0(
    const std::vector<CgOperationChildControlRef>& children,
    YtlReferenceSlotRuntime& consumed,
    const RECT* const destinationClip)
{
    CompositeReferencedChildrenAt5231C0(
        *graphics_, children, modeAt34_, enabledAt38_, consumed, destinationClip);
}

void CgOperationDialogRenderHostRuntime::CompositeChildrenAt5231C0(
    const std::span<const CgOperationChildControlRef> children,
    const GraphicsSurfaceDestination destination,
    const RECT* const destinationClip)
{
    if (enabledAt38_ == 0)
        return;
    for (const auto& child : children)
    {
        if (!child)
            throw std::invalid_argument("GuiDialog child reference is null");
        if (modeAt34_ != 0)
        {
            static_cast<void>(child->CompositeAtVslot10(
                *graphics_, destination, destinationClip));
            continue;
        }

        const std::int32_t previous = child->ReadAtVslot48();
        child->WriteAtVslot44(0);
        static_cast<void>(child->CompositeAtVslot10(
            *graphics_, destination, destinationClip));
        child->WriteAtVslot44(previous);
    }
}

CgOperationClickedEventRuntime::CgOperationClickedEventRuntime(
    const std::int32_t eventValue) noexcept
    : eventValueAt08_(eventValue)
{
}

std::int32_t
CgOperationClickedEventRuntime::ClickedFlagAt04() const noexcept
{
    return clickedFlagAt04_;
}

std::int32_t
CgOperationClickedEventRuntime::EventValueAt08() const noexcept
{
    return eventValueAt08_;
}

void CgOperationClickedEventRuntime::WriteClickedFlagAt04(
    const std::int32_t value) noexcept
{
    clickedFlagAt04_ = value;
}

void CgOperationClickedEventRuntime::ClearAtVslot04() noexcept
{
    // 5321F0 只清除 ClickedEvent+4，不执行控件行为。
    clickedFlagAt04_ = 0;
}

namespace
{
[[nodiscard]] std::vector<CgOperationClickedEventRef>
CreateClickedEvents(
    const std::vector<CgOperationChildControlRef>& children,
    const std::vector<std::pair<CgOperationControlKind, std::int32_t>>& expected)
{
    if (children.size() != expected.size())
        throw std::invalid_argument("CGOperation clicked-event control count is invalid");

    std::vector<CgOperationClickedEventRef> events;
    events.reserve(expected.size());
    for (std::size_t index = 0; index < expected.size(); ++index)
    {
        const auto& child = children[index];
        if (!child || child->ConstructionKind() != expected[index].first ||
            child->EventValueAt04() != expected[index].second)
        {
            throw std::invalid_argument("CGOperation clicked-event control order is invalid");
        }

        auto event = std::make_shared<CgOperationClickedEventRuntime>(
            expected[index].second);
        // 原构造逐项先绑定事件并写 +0x48=1，随后才插入 manager vector。
        child->BindClickedEventAt3C(event);
        child->WriteClickedEventEnabledAt48(1);
        events.push_back(std::move(event));
    }
    return events;
}
}

CgOperationClickedEventManagerRuntime::
CgOperationClickedEventManagerRuntime(
    std::vector<CgOperationClickedEventRef> events) noexcept
    : events_(std::move(events))
{
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateEmptyAt4D7B90()
{
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime({}));
}

void CgOperationClickedEventManagerRuntime::AppendBoundEventAt4D9620(
    CgOperationClickedEventRef event)
{
    if (!event) throw std::invalid_argument("Title clicked event is null");
    events_.push_back(std::move(event));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForButtonsAt4DAC80(
    const std::vector<CgOperationChildControlRef>& children)
{
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(children, {
                {CgOperationControlKind::Static, 2},
                {CgOperationControlKind::Button, 3},
                {CgOperationControlKind::Button, 4},
                {CgOperationControlKind::Button, 5},
                {CgOperationControlKind::Button, 6},
                {CgOperationControlKind::Button, 7},
            })));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForPopupAt4DB860(
    const std::vector<CgOperationChildControlRef>& children)
{
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    expected.reserve(22u);
    for (std::int32_t childId = 12; childId <= 31; ++childId)
        expected.emplace_back(CgOperationControlKind::Button, childId);
    expected.emplace_back(CgOperationControlKind::Button, 10);
    expected.emplace_back(CgOperationControlKind::Button, 11);

    std::vector<CgOperationChildControlRef> eventChildren;
    eventChildren.reserve(22u);
    for (std::size_t index = 2; index < children.size(); ++index)
        eventChildren.push_back(children[index]);
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(eventChildren, expected)));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForOptionCharSelectAt4E4A20(
    const std::vector<CgOperationChildControlRef>& children)
{
    // 0x4E4AF0..0x4E4FD2：五个 Button 的事件值严格为 0..4，
    // 替换的未开放角色资源仍沿用事件值 3。
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(children, {
                {CgOperationControlKind::Button, 0},
                {CgOperationControlKind::Button, 1},
                {CgOperationControlKind::Button, 2},
                {CgOperationControlKind::Button, 3},
                {CgOperationControlKind::Button, 4},
            })));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::
CreateForProgramSelectionSystemAt4DF420(
    const std::vector<CgOperationChildControlRef>& children,
    const bool mode01)
{
    // 0x4DF420/0x4DFCA0：Static 的 +0x08 为 1，不创建 ClickedEvent；
    // 其余十项按四组描述表的原始顺序进入 +0x5C manager。
    if (children.size() != 11u)
    {
        throw std::invalid_argument(
            "ProgramSelection system control count is invalid");
    }
    if (!children.front() ||
        children.front()->ConstructionKind() != CgOperationControlKind::Static)
    {
        throw std::invalid_argument(
            "ProgramSelection system first control is not Static");
    }

    const std::array<std::int32_t, 10> ids = mode01 ?
        std::array<std::int32_t, 10>{28, 27, 33, 32, 29, 30, 24, 26, 31, 34} :
        std::array<std::int32_t, 10>{53, 52, 58, 57, 54, 55, 49, 51, 56, 59};
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    expected.reserve(ids.size());
    for (std::size_t index = 0; index < ids.size(); ++index)
    {
        const CgOperationControlKind kind = index >= 4u && index < 6u ?
            CgOperationControlKind::CheckBox : CgOperationControlKind::Button;
        expected.emplace_back(kind, ids[index]);
    }

    std::vector<CgOperationChildControlRef> eventChildren(
        children.begin() + 1, children.end());
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(eventChildren, expected)));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForProgramSelectionMainAt4DCEB0(
    const std::vector<CgOperationChildControlRef>& children,
    const bool mode01)
{
    // 4DCEB0/4DE520 仅把 Button 分组加入 ClickedEvent manager；
    // 构造在它们之前和之后的 Static/CElement 均不参与事件表。
    const std::array<std::int32_t, 8> mode00Ids{
        40, 41, 42, 43, 44, 45, 46, 47};
    const std::array<std::int32_t, 6> mode01Ids{
        17, 18, 19, 20, 21, 22};

    std::vector<CgOperationChildControlRef> eventChildren;
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    const auto append = [&](const std::int32_t id) {
        const auto found = std::ranges::find_if(
            children, [id](const auto& child) {
                return child &&
                    child->ConstructionKind() == CgOperationControlKind::Button &&
                    child->EventValueAt04() == id;
            });
        if (found == children.end())
        {
            throw std::invalid_argument(
                "ProgramSelection main Button table is incomplete");
        }
        eventChildren.push_back(*found);
        expected.emplace_back(CgOperationControlKind::Button, id);
    };

    if (mode01)
    {
        for (const std::int32_t id : mode01Ids)
            append(id);
    }
    else
    {
        for (const std::int32_t id : mode00Ids)
            append(id);
    }

    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(eventChildren, expected)));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForSaveLoadSystemAt4F0C80(
    const std::vector<CgOperationChildControlRef>& children)
{
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    expected.reserve(children.size());
    for (const auto& child : children)
    {
        if (!child)
            throw std::invalid_argument("SaveLoadSystem control is null");
        expected.emplace_back(
            CgOperationControlKind::Button,
            child->EventValueAt04());
    }
    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(children, expected)));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForSaveLoadPageAt4ED190(
    const std::vector<CgOperationChildControlRef>& eventChildren)
{
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    if (eventChildren.size() == 19u)
    {
        // Page00 以及 Page01(type 3)：顶部 50..52、文件 25..32、删除 41..48。
        expected.reserve(19u);
        for (std::int32_t eventValue = 50; eventValue <= 52; ++eventValue)
            expected.emplace_back(CgOperationControlKind::Button, eventValue);
        for (std::int32_t eventValue = 25; eventValue <= 32; ++eventValue)
            expected.emplace_back(CgOperationControlKind::Button, eventValue);
        for (std::int32_t eventValue = 41; eventValue <= 48; ++eventValue)
            expected.emplace_back(CgOperationControlKind::Button, eventValue);
    }
    else if (eventChildren.size() == 8u)
    {
        // Page01(type 4/5) 只有八个文件按钮进入 ClickedEvent manager。
        expected.reserve(8u);
        for (std::int32_t eventValue = 25; eventValue <= 32; ++eventValue)
            expected.emplace_back(CgOperationControlKind::Button, eventValue);
    }
    else
    {
        throw std::invalid_argument(
            "SaveLoadPage clicked-event control count is invalid");
    }

    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(eventChildren, expected)));
}

std::shared_ptr<CgOperationClickedEventManagerRuntime>
CgOperationClickedEventManagerRuntime::CreateForSaveLoadPopupAt4EF660(
    const std::vector<CgOperationChildControlRef>& eventChildren)
{
    // 0x4EFAB4..0x4F083C：上翻、11 个页选择框、下翻依次绑定
    // ClickedEvent；Static 与 Slider 不进入该 manager。
    std::vector<std::pair<CgOperationControlKind, std::int32_t>> expected;
    expected.reserve(13u);
    expected.emplace_back(CgOperationControlKind::Button, 58);
    for (std::int32_t eventValue = 60; eventValue <= 70; ++eventValue)
        expected.emplace_back(CgOperationControlKind::CheckBox, eventValue);
    expected.emplace_back(CgOperationControlKind::Button, 59);

    return std::shared_ptr<CgOperationClickedEventManagerRuntime>(
        new CgOperationClickedEventManagerRuntime(
            CreateClickedEvents(eventChildren, expected)));
}

void CgOperationClickedEventManagerRuntime::ClearAllAt532200() noexcept
{
    for (const auto& event : events_)
        event->ClearAtVslot04();
}

std::int32_t CgOperationClickedEventManagerRuntime::PollFirstAt50FDB0(
    const std::int32_t clearMatchedEvent) noexcept
{
    for (const auto& event : events_)
    {
        if (event->ClickedFlagAt04() == 0)
            continue;
        const std::int32_t result = event->EventValueAt08();
        if (clearMatchedEvent != 0)
            event->ClearAtVslot04();
        return result;
    }
    return -1;
}

const std::vector<CgOperationClickedEventRef>&
CgOperationClickedEventManagerRuntime::EventsInInsertionOrder() const noexcept
{
    return events_;
}

ICgOperationClickedEvent&
CgOperationClickedEventManagerRuntime::EventForValue(
    const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(
        events_, [eventValue](const auto& event) {
            return event->EventValueAt08() == eventValue;
        });
    if (it == events_.end())
        throw std::out_of_range("CGOperation clicked event is not owned");
    return **it;
}

CgOperationButtonsVisualRuntime::CgOperationButtonsVisualRuntime(
    IGraphicsSurfaceCompositionApi& graphics,
    std::shared_ptr<RuntimeGraphicsSurface> target0,
    std::shared_ptr<RuntimeGraphicsSurface> target1,
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dialogResources,
    const CgOperationButtonsVisualLayout layout)
    : graphics_(&graphics),
      target0_(std::move(target0)),
      target1_(std::move(target1)),
      dialogResources_(std::move(dialogResources)),
      layout_(layout)
{
    if (!target0_ || !target1_ || target0_->Handle() == 0 ||
        target1_->Handle() == 0)
    {
        throw std::invalid_argument("CGOperationButtons target surface is invalid");
    }
    if (dialogResources_.size() != kRequiredResourceCount)
        throw std::invalid_argument("CGOperationDialog requires twenty visual resources");
    if (std::ranges::any_of(dialogResources_, [](const auto& resource) {
            return !resource || resource->Handle() == 0;
        }))
    {
        throw std::invalid_argument("CGOperationDialog visual resource is invalid");
    }
}

void CgOperationButtonsVisualRuntime::ApplySelectionAt537010(
    const std::int32_t selection)
{
    // 原函数无条件先清两张目标；后续索引或绘制失败不得撤销该副作用。
    graphics_->ClearSurface(
        target0_->Handle(), target0_->ClearColor(), nullptr);
    graphics_->ClearSurface(
        target1_->Handle(), target1_->ClearColor(), nullptr);
    if (selection >= 1000)
        return;

    const std::int32_t value = selection + 1;
    const std::int32_t hundreds = value / 100;
    const std::int32_t remainder = value - hundreds * 100;
    const std::int32_t tens = remainder / 10;
    const std::int32_t ones = remainder - tens * 10;

    BlitPair(hundreds, layout_.digitPositions[0]);
    BlitPair(tens, layout_.digitPositions[1]);
    BlitPair(ones, layout_.digitPositions[2]);
}

const std::shared_ptr<RuntimeGraphicsSurface>& CgOperationButtonsVisualRuntime::TargetAt522F80(
    const std::int32_t modeAt34) const noexcept
{
    return modeAt34 != 0 ? target0_ : target1_;
}

const RuntimeGraphicsSurface& CgOperationButtonsVisualRuntime::ResourceAt(
    const std::int32_t index) const
{
    // 原二进制对非法负索引会越界访问；重写版明确中止，不能用空资源兜底。
    if (index < 0 || static_cast<std::size_t>(index) >= dialogResources_.size())
        throw std::out_of_range("CGOperationDialog visual resource index is invalid");
    return *dialogResources_[static_cast<std::size_t>(index)];
}

void CgOperationButtonsVisualRuntime::BlitPair(
    const std::int32_t digit,
    const POINT& absolutePosition)
{
    const std::int32_t firstIndex = digit * 2;
    const auto& first = ResourceAt(firstIndex);
    const auto& second = ResourceAt(firstIndex + 1);
    const std::int32_t destinationX =
        absolutePosition.x - target0_->OriginX();
    const std::int32_t destinationY =
        absolutePosition.y - target0_->OriginY();

    // 537010 六次都把后四个 ABI 槽置零，并忽略 SurfaceBlt 返回值。
    static_cast<void>(graphics_->BlitSurface(
        target0_->Handle(), first.Handle(), destinationX, destinationY,
        nullptr, nullptr, nullptr, 0));
    static_cast<void>(graphics_->BlitSurface(
        target1_->Handle(), second.Handle(), destinationX, destinationY,
        nullptr, nullptr, nullptr, 0));
}

CgOperationButtonsRuntime::CgOperationButtonsRuntime(
    ICgOperationButtonsServices& services,
    std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager,
    std::shared_ptr<ICgOperationButtonsVisuals> visuals,
    std::vector<CgOperationChildControlRef> children,
    const RECT& rectAt80)
    : services_(&services),
      children_(std::move(children)),
      eventManager_(std::move(eventManager)),
      visuals_(std::move(visuals)),
      rectAt80_(rectAt80)
{
    if (!eventManager_)
        throw std::invalid_argument("CGOperationButtons event manager is null");
    if (!visuals_)
        throw std::invalid_argument("CGOperationButtons visual owner is null");
    // 0x608440 固定给出 Static 2 与 Button 3..7；这些值进入控件 +0x04，
    // 与 Button descriptor 另写入 +0x0C 的值不是同一字段。
    if (children_.size() != kRequiredControlCount)
        throw std::invalid_argument("CGOperationButtons requires six controls");
    for (std::size_t index = 0; index < children_.size(); ++index)
    {
        const auto expectedKind = index == 0u
            ? CgOperationControlKind::Static
            : CgOperationControlKind::Button;
        const std::int32_t expectedEvent = static_cast<std::int32_t>(index) + 2;
        if (!children_[index] ||
            children_[index]->ConstructionKind() != expectedKind ||
            children_[index]->EventValueAt04() != expectedEvent)
        {
            throw std::invalid_argument("CGOperationButtons control order is invalid");
        }
    }
}

void CgOperationButtonsRuntime::OpenAtVslot44()
{
    // 0x50BF90：先遍历 +0x5C 管理器中的全部事件，再写自身 +0x38，最后遍历子控件。
    eventManager_->ClearAllAt532200();
    stateAt38_ = 1;
    InvokeAtVslot68();
}

std::int32_t CgOperationButtonsRuntime::StateAtVslot20() const
{
    return stateAt34_;
}

std::int32_t CgOperationButtonsRuntime::UpdateAtVslot48()
{
    // 0x52C270 在公共更新和子对象轮询之前清除 +0x90。
    eventAt90_ = -1;
    eventAt90Initialized_ = true;
    services_->UpdateInheritedAtVslot54();
    eventAt90_ = eventManager_->PollFirstAt50FDB0(1);
    return 1;
}

std::int32_t CgOperationButtonsRuntime::EventAtVslot50() const
{
    if (!eventAt90Initialized_)
        throw std::logic_error("CGOperationButtons +0x90 has not been initialized by Update");
    return eventAt90_;
}

std::int32_t CgOperationButtonsRuntime::WriteAtVslot1C(
    const std::int32_t value)
{
    stateAt34_ = value;
    return value;
}

void CgOperationButtonsRuntime::InvokeAtVslot68()
{
    // 0x522E90 按插入顺序调用每个子控件的虚槽 +0x08。
    for (const auto& child : children_)
        child->InvokeAtVslot08();
}

void CgOperationButtonsRuntime::ApplySelectionAt537010(
    const std::int32_t selection)
{
    visuals_->ApplySelectionAt537010(selection);
}

RECT* CgOperationButtonsRuntime::ReadRectAtVslot8C(RECT* const output) const
{
    // 0x50A0D0 是原始 out-parameter ABI；不接受空输出指针。
    *output = rectAt80_;
    return output;
}

std::int32_t CgOperationButtonsRuntime::StateAt38() const noexcept
{
    return stateAt38_;
}

std::int32_t CgOperationButtonsRuntime::WriteAtVslot24(const std::int32_t value) noexcept
{
    stateAt38_ = value;
    return value;
}

void CgOperationButtonsRuntime::CompositeAtVslot5C(
    IGraphicsSurfaceCompositionApi& graphics, YtlReferenceSlotRuntime& argument,
    const RECT* const destinationClip)
{
    CgOperationSurfaceArgument parameter(argument);
    YtlReferenceSlotRuntime copied;
    CopyGraphicsSurfaceReferenceAt47D740(copied, argument);
    CompositeReferencedChildrenAt5231C0(graphics, children_, stateAt34_, stateAt38_, copied, destinationClip);
    // 522FD6..523024：子控件可改写 enabled/mode；选中面只能在此时决定。
    if (stateAt38_ != 0)
    {
        const auto& source = visuals_->TargetAt522F80(stateAt34_);
        if (source)
        {
            const auto x = source->OriginX();
            const auto y = source->OriginY();
            const auto targetHandle = ResolveCompositeDestination(argument).Handle();
            if (targetHandle != 0)
            {
                const auto sourceHandle = source->Handle();
                if (sourceHandle != 0)
                    static_cast<void>(graphics.BlitSurface(targetHandle, sourceHandle,
                        x, y, nullptr, nullptr, destinationClip, 0));
            }
        }
    }
    parameter.FinishNormally();
}

CgOperationPopupKeyRuntime::CgOperationPopupKeyRuntime(
    const std::int32_t eventValue,
    std::array<CgOperationKeyMapping, 20> mappings)
    : eventValueAt04_(eventValue),
      mappings_(std::move(mappings))
{
    std::array<bool, 20> observedSources{};
    for (const auto& mapping : mappings_)
    {
        if (mapping.sourceControlId < 12 || mapping.sourceControlId > 31 ||
            mapping.targetControlId < 12 || mapping.targetControlId > 31)
        {
            throw std::invalid_argument("CGOperationPopup key mapping is outside control range");
        }
        const auto sourceIndex =
            static_cast<std::size_t>(mapping.sourceControlId - 12);
        if (observedSources[sourceIndex])
            throw std::invalid_argument("CGOperationPopup key mapping source is duplicated");
        observedSources[sourceIndex] = true;
    }
}

std::int32_t CgOperationPopupKeyRuntime::EventValueAt04() const noexcept
{
    return eventValueAt04_;
}

const std::array<CgOperationKeyMapping, 20>&
CgOperationPopupKeyRuntime::MappingsAt0C() const noexcept
{
    return mappings_;
}

std::int32_t CgOperationPopupKeyRuntime::TargetForSource(
    const std::int32_t sourceControlId) const
{
    const auto it = std::ranges::find_if(
        mappings_,
        [sourceControlId](const CgOperationKeyMapping& mapping) {
            return mapping.sourceControlId == sourceControlId;
        });
    if (it == mappings_.end())
        throw std::out_of_range("CGOperationPopup key source is not present in the original table");
    return it->targetControlId;
}

namespace
{
[[nodiscard]] std::array<CgOperationKeyMapping, 20>
BuildPopupPairSwapMappingsAt6086C8()
{
    std::array<CgOperationKeyMapping, 20> mappings{};
    for (std::int32_t source = 12; source <= 31; ++source)
    {
        const std::int32_t target = (source & 1) == 0
            ? source + 1
            : source - 1;
        mappings[static_cast<std::size_t>(source - 12)] = {source, target};
    }
    return mappings;
}

[[nodiscard]] std::array<CgOperationKeyMapping, 20>
BuildPopupPreviousRowMappingsAt608770()
{
    std::array<CgOperationKeyMapping, 20> mappings{};
    for (std::int32_t source = 12; source <= 31; ++source)
    {
        const std::int32_t target = source < 14
            ? source + 18
            : source - 2;
        mappings[static_cast<std::size_t>(source - 12)] = {source, target};
    }
    return mappings;
}

[[nodiscard]] std::array<CgOperationKeyMapping, 20>
BuildPopupNextRowMappingsAt608818()
{
    std::array<CgOperationKeyMapping, 20> mappings{};
    for (std::int32_t source = 12; source <= 31; ++source)
    {
        const std::int32_t target = source > 29
            ? source - 18
            : source + 2;
        mappings[static_cast<std::size_t>(source - 12)] = {source, target};
    }
    return mappings;
}
}

CgOperationPopupKeySetRuntime::CgOperationPopupKeySetRuntime()
    : keys_{
          CgOperationPopupKeyRuntime(3, BuildPopupPairSwapMappingsAt6086C8()),
          CgOperationPopupKeyRuntime(4, BuildPopupPairSwapMappingsAt6086C8()),
          CgOperationPopupKeyRuntime(1, BuildPopupPreviousRowMappingsAt608770()),
          CgOperationPopupKeyRuntime(2, BuildPopupNextRowMappingsAt608818())}
{
}

const std::array<CgOperationPopupKeyRuntime, 4>&
CgOperationPopupKeySetRuntime::KeysInInsertionOrder() const noexcept
{
    return keys_;
}

const CgOperationPopupKeyRuntime&
CgOperationPopupKeySetRuntime::KeyForEvent(const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(
        keys_,
        [eventValue](const CgOperationPopupKeyRuntime& key) {
            return key.EventValueAt04() == eventValue;
        });
    if (it == keys_.end())
        throw std::out_of_range("CGOperationPopup key event is not present in the original table");
    return *it;
}

CgOperationPopupRuntime::CgOperationPopupRuntime(
    ICgOperationPopupServices& services,
    std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager,
    std::vector<CgOperationChildControlRef> children,
    std::vector<CgOperationPopupPageResourceRef> pageResources,
    const RECT& rectAt68,
    std::shared_ptr<RuntimeGraphicsSurface> independentSurfaceAt7C,
    CgOperationPopupPageProducerRef pageProducer)
    : services_(&services),
      children_(std::move(children)),
      eventManager_(std::move(eventManager)),
      independentSurfaceAt7C_(std::move(independentSurfaceAt7C)),
      pageProducer_(std::move(pageProducer)),
      pageResources_(std::move(pageResources)),
      rectAt68_(rectAt68)
{
    if (!eventManager_)
        throw std::invalid_argument("CGOperationPopup event manager is null");
    if (!independentSurfaceAt7C_)
        throw std::invalid_argument("CGOperationPopup +0x7C surface is null");
    if (!pageProducer_)
        throw std::invalid_argument("CGOperationPopup page producer is null");
    if (std::ranges::any_of(children_, [](const auto& child) {
            return !child;
        }))
    {
        throw std::invalid_argument("CGOperationPopup child is null");
    }
    if (std::ranges::any_of(pageResources_, [](const auto& resource) {
            return !resource;
        }))
    {
        throw std::invalid_argument("CGOperationPopup page resource is null");
    }
    if (pageResources_.size() >
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::length_error("CGOperationPopup +0x88 vector exceeds int32 range");
    }

    // 0x4DB860 的插入顺序固定为 Static 8/9、Button 12..31、Button 10/11。
    // 0x509D20 明确按控件 +0x04 查找；动态 Button 的 +0x0C 实际均为 -1。
    std::vector<std::int32_t> expectedEvents{8, 9};
    for (std::int32_t eventValue = 12; eventValue <= 31; ++eventValue)
        expectedEvents.push_back(eventValue);
    expectedEvents.push_back(10);
    expectedEvents.push_back(11);
    if (children_.size() != expectedEvents.size())
        throw std::invalid_argument("CGOperationPopup requires twenty-four controls");
    for (std::size_t index = 0; index < expectedEvents.size(); ++index)
    {
        const auto expectedKind = index < 2u
            ? CgOperationControlKind::Static
            : CgOperationControlKind::Button;
        if (children_[index]->ConstructionKind() != expectedKind ||
            children_[index]->EventValueAt04() != expectedEvents[index])
            throw std::invalid_argument("CGOperationPopup control order is invalid");
    }
}

std::int32_t CgOperationPopupRuntime::StateAtVslot20() const { return stateAt34_; }

std::int32_t CgOperationPopupRuntime::WriteAtVslot1C(const std::int32_t value)
{
    stateAt34_ = value;
    return value;
}

void CgOperationPopupRuntime::CompositeAtVslot5C(
    IGraphicsSurfaceCompositionApi& graphics, YtlReferenceSlotRuntime& argument,
    const RECT* const destinationClip)
{
    // 原 Popup+5C 直接指向 5231C0，不额外复制一层参数。
    CompositeReferencedChildrenAt5231C0(graphics, children_, stateAt34_, stateAt38_, argument, destinationClip);
}

void CgOperationPopupRuntime::OpenAtVslot44()
{
    // 0x50C0B0 的调用顺序不可交换；+0x94 在两个翻页控件之前写 -1。
    (void)WriteAtVslot24(0);
    ResetInputAt510650();
    InvokeAtVslot68();
    currentPageAt94_ = -1;
    currentPageAt94Initialized_ = true;

    const std::int32_t hasMultiplePages = pageResources_.size() > 1 ? 1 : 0;
    ChildByEventValueAt04(10)->WriteAtVslot44(hasMultiplePages);
    ChildByEventValueAt04(11)->WriteAtVslot44(hasMultiplePages);
}

std::int32_t CgOperationPopupRuntime::UpdateAtVslot48()
{
    // 0x52C940 与 Primary 一样，先清事件，再执行公共更新与首对象轮询。
    eventAt78_ = -1;
    eventAt78Initialized_ = true;
    services_->UpdateInheritedAtVslot54();
    eventAt78_ = eventManager_->PollFirstAt50FDB0(1);
    return 1;
}

std::int32_t CgOperationPopupRuntime::EventAtVslot50() const
{
    if (!eventAt78Initialized_)
        throw std::logic_error("CGOperationPopup +0x78 has not been initialized by Update");
    return eventAt78_;
}

std::int32_t CgOperationPopupRuntime::WriteAtVslot24(
    const std::int32_t value)
{
    stateAt38_ = value;
    return value;
}

std::int32_t CgOperationPopupRuntime::CurrentPageAt94() const
{
    if (!currentPageAt94Initialized_)
        throw std::logic_error("CGOperationPopup +0x94 has not been initialized by Open");
    return currentPageAt94_;
}

std::int32_t CgOperationPopupRuntime::PageCountAt88And8C() const
{
    return static_cast<std::int32_t>(pageResources_.size());
}

void CgOperationPopupRuntime::SetPageAt527500(const std::int32_t page)
{
    if (!currentPageAt94Initialized_)
        throw std::logic_error("CGOperationPopup SetPage called before Open");
    if (page < 0)
        throw std::out_of_range("CGOperationPopup page is negative");
    if (currentPageAt94_ == page)
        return;

    const auto pageIndex = static_cast<std::size_t>(page);
    if (pageIndex < pageResources_.size())
    {
        services_->AssignPageResourceToChild9At527500(
            ChildByEventValueAt04(9),
            pageResources_[pageIndex]);
    }

    const std::size_t selectionCount =
        services_->ParentSelectionCountAt80And84();
    if (selectionCount >
        static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
    {
        throw std::length_error("CGOperationDialog +0x80 vector exceeds int32 range");
    }

    const std::int64_t first = static_cast<std::int64_t>(page) * 20;
    const std::int64_t total = static_cast<std::int64_t>(selectionCount);
    const std::int64_t visibleCount =
        (std::min)(total, first + 20) - first;
    for (std::int32_t index = 0; index < 20; ++index)
    {
        ChildByEventValueAt04(index + 12)->WriteAtVslot4C(
            static_cast<std::int64_t>(index) < visibleCount ? 1 : 0);
    }

    // 0x5276F9：页号先提交，随后才调用虚槽 +0x3C。
    currentPageAt94_ = page;
    ResetInputAt510650();
}

void CgOperationPopupRuntime::ResetPagesAt5021C0()
{
    if (pageResources_.empty())
        throw std::out_of_range("CGOperationPopup +0x88 vector is empty");

    // 0x5021C0：页切换先发生；随后首资源被写 0，最后才裁去其余引用。
    SetPageAt527500(0);
    pageResources_.front()->ClearAt4143A0(nullptr);
    if (pageResources_.size() > 1)
        pageResources_.erase(pageResources_.begin() + 1, pageResources_.end());
}

void CgOperationPopupRuntime::DrawSelectionAt4FEAE0(
    const std::int32_t itemIndex,
    const std::int32_t available)
{
    pageProducer_->DrawSelectionAt4FEAE0(
        pageResources_, independentSurfaceAt7C_, itemIndex, available);
}

CgOperationDialogEventTargetRef
CgOperationPopupRuntime::ResolveEventTargetAtVslot14(
    const std::int32_t event)
{
    return ChildByEventValueAt04(event);
}

void CgOperationPopupRuntime::InvokeAtVslot68()
{
    for (const auto& child : children_)
        child->InvokeAtVslot08();
}

void CgOperationPopupRuntime::SubmitAtVslot30(
    CgOperationDialogEventTargetRef value)
{
    // 0x532170：相同对象不重复失焦/获焦。
    if (activeControlAt4C_ == value)
        return;
    if (activeControlAt4C_)
        activeControlAt4C_->LoseFocusAtVslot24();
    if (value)
        value->GainFocusAtVslot20();
    activeControlAt4C_ = std::move(value);
}

RECT* CgOperationPopupRuntime::ReadRectAtVslot8C(RECT* const output) const
{
    *output = rectAt68_;
    return output;
}

std::int32_t CgOperationPopupRuntime::UpdateInputAt510860()
{
    if (!activeControlAt4C_)
        throw std::logic_error("CGOperationPopup +0x4C active control is null");

    const auto activeChild =
        std::dynamic_pointer_cast<ICgOperationChildControl>(activeControlAt4C_);
    if (!activeChild)
        throw std::logic_error("CGOperationPopup active control has no child state");

    const std::int32_t mode = activeChild->InputModeAt08();
    if (mode != 0 && mode != 2)
        return 0;

    const std::int32_t oldLatch = inputLatchAt98_;
    if (oldLatch != 0)
    {
        if (!services_->ReadInputAtVslot6C(5))
        {
            // 0x5108F6：释放关联对象发生在 +0x98 清零之前。
            activeChild->InvokeReferencedObjectAt3CAtVslot00();
            inputLatchAt98_ = 0;
        }
        return oldLatch;
    }

    if (!services_->ReadInputAtVslot70(5))
        return 0;

    inputLatchAt98_ = 1;
    activeChild->WriteAtVslot70(1);
    return 1;
}

void CgOperationPopupRuntime::ResetInputAt510650()
{
    inputLatchAt98_ = 0;
    SubmitAtVslot30({});
    opaqueAt30_ = 0;
}

std::int32_t CgOperationPopupRuntime::StateAt38() const noexcept
{
    return stateAt38_;
}

std::int32_t CgOperationPopupRuntime::InputLatchAt98() const noexcept
{
    return inputLatchAt98_;
}

const CgOperationPopupKeySetRuntime&
CgOperationPopupRuntime::OperationKeysAt24() const noexcept
{
    return operationKeysAt24_;
}

CgOperationChildControlRef CgOperationPopupRuntime::ChildByEventValueAt04(
    const std::int32_t eventValue) const
{
    const auto it = std::ranges::find_if(children_, [eventValue](const auto& child) {
        return child && child->EventValueAt04() == eventValue;
    });
    if (it == children_.end())
        return {};
    return *it;
}
}
