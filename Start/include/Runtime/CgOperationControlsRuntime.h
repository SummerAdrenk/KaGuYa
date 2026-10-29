#pragma once

#include <Runtime/CgOperationDialogRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <Windows.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class CgOperationControlResourceGroupRuntime;

enum class CgOperationControlKind : std::uint8_t
{
    Static,
    Button,
    CheckBox,
    Slider,
};

class ICgOperationClickedEvent
{
public:
    virtual ~ICgOperationClickedEvent() = default;

    [[nodiscard]] virtual std::int32_t ClickedFlagAt04() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t EventValueAt08() const noexcept = 0;
    virtual void WriteClickedFlagAt04(std::int32_t value) noexcept = 0;
    virtual void ClearAtVslot04() noexcept = 0;
};

using CgOperationClickedEventRef =
    std::shared_ptr<ICgOperationClickedEvent>;

// GuiDialog 子控件在本模块已证明会使用的虚槽。业务名尚未由消费者闭环的
// 字段继续以虚槽和 childId 命名。
class ICgOperationChildControl : public ICgOperationDialogEventTarget
{
public:
    virtual ~ICgOperationChildControl() = default;

    [[nodiscard]] virtual CgOperationControlKind ConstructionKind() const = 0;
    [[nodiscard]] virtual std::int32_t EventValueAt04() const = 0;
    [[nodiscard]] virtual std::int32_t ChildIdAt0C() const = 0;
    [[nodiscard]] virtual std::int32_t InputModeAt08() const = 0;
    virtual void InvokeAtVslot08() = 0;
    [[nodiscard]] virtual bool DrawAtVslot0C(
        IGraphicsSurfaceCompositionApi& graphics,
        const RECT* destinationClip) = 0;
    [[nodiscard]] virtual bool CompositeAtVslot10(
        IGraphicsSurfaceCompositionApi& graphics,
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip) = 0;
    // 原按值参数入口；默认实现消费引用后进入现有单笔合成叶。
    virtual void CompositeReferenceAtVslot10(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip);
    [[nodiscard]] virtual std::int32_t ProcessAtVslot14() = 0;
    [[nodiscard]] virtual std::int32_t ProcessAtVslot18() = 0;
    virtual void WriteAtVslot44(std::int32_t value) = 0;
    [[nodiscard]] virtual std::int32_t ReadAtVslot48() const noexcept = 0;
    virtual void WriteAtVslot4C(std::int32_t value) = 0;
    virtual void WriteAtVslot70(std::int32_t value) = 0;
    virtual void BindClickedEventAt3C(CgOperationClickedEventRef event) = 0;
    virtual void WriteClickedEventEnabledAt48(std::int32_t value) = 0;
    virtual void InvokeReferencedObjectAt3CAtVslot00() = 0;
    [[nodiscard]] virtual std::int32_t VisualStateAtVslot38() const noexcept = 0;
    [[nodiscard]] virtual RECT ReadRectAtVslot3C() const = 0;
    [[nodiscard]] virtual std::int32_t ReadModeFiveChangedAtVslot6C() const = 0;
};

using CgOperationChildControlRef =
    std::shared_ptr<ICgOperationChildControl>;

// GuiControl::Button+0x18(0x5214B0) 只通过父 Dialog 读取这四类输入，
// 并在 alpha 命中时调用 Graphics::GetSurfacePixel。Button 构造早于
// Dialog 完整装配，因此父对象在加入 child vector 后显式绑定。
class ICgOperationButtonInputServices
{
public:
    virtual ~ICgOperationButtonInputServices() = default;

    virtual void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) = 0;
    [[nodiscard]] virtual bool ReadInputAtVslot6C(
        std::int32_t eventValue) = 0;
    [[nodiscard]] virtual bool ReadInputAtVslot70(
        std::int32_t eventValue) = 0;
    [[nodiscard]] virtual POINT ReadPointerAtVslot74() = 0;
    [[nodiscard]] virtual bool ReadPressAtVslot80() = 0;
    [[nodiscard]] virtual bool ReadReleaseAtVslot84() = 0;
    [[nodiscard]] virtual std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) = 0;
};

// GuiControl::Static/Button 的公共生产状态。这里只给 IDA 已证明的字段命名；
// +0x0C 与资源槽的业务含义尚未闭环，因此继续保留偏移名。
class CgOperationChildControlRuntime final
    : public ICgOperationChildControl
{
public:
    CgOperationChildControlRuntime(
        CgOperationControlKind kind,
        std::int32_t eventValueAt04,
        std::int32_t childIdAt0C,
        std::int32_t initialResourceSlotAt54,
        std::shared_ptr<CgOperationControlResourceGroupRuntime> resources,
        std::shared_ptr<CgOperationControlResourceGroupRuntime>
            checkedResources = {});

    [[nodiscard]] CgOperationControlKind ConstructionKind() const override;
    [[nodiscard]] std::int32_t EventValueAt04() const override;
    [[nodiscard]] std::int32_t ChildIdAt0C() const override;
    [[nodiscard]] std::int32_t InputModeAt08() const override;
    void InvokeAtVslot08() override;
    [[nodiscard]] bool DrawAtVslot0C(
        IGraphicsSurfaceCompositionApi& graphics,
        const RECT* destinationClip) override;
    [[nodiscard]] bool CompositeAtVslot10(
        IGraphicsSurfaceCompositionApi& graphics,
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip) override;
    [[nodiscard]] std::int32_t ProcessAtVslot14() override;
    [[nodiscard]] std::int32_t ProcessAtVslot18() override;
    void GainFocusAtVslot20() override;
    void LoseFocusAtVslot24() override;
    void WriteAtVslot44(std::int32_t value) override;
    [[nodiscard]] std::int32_t ReadAtVslot48() const noexcept override;
    void WriteAtVslot4C(std::int32_t value) override;
    void WriteAtVslot70(std::int32_t value) override;
    void BindButtonInputServicesAt4C(
        ICgOperationButtonInputServices& inputServices);
    void WritePositionAt34And38(const POINT& position) noexcept;
    void WriteRectAt34ForConstruction(const RECT& rect) noexcept;
    void BindClickedEventAt3C(CgOperationClickedEventRef event) override;
    // 508586/53138A 在 reset 之后取当前绑定引用，调用方复制后保活。
    [[nodiscard]] const CgOperationClickedEventRef& ClickedEventAt3C() const noexcept { return clickedEventAt3C_; }
    void WriteClickedEventEnabledAt48(std::int32_t value) override;
    void InvokeReferencedObjectAt3CAtVslot00() override;
    [[nodiscard]] std::int32_t VisualStateAtVslot38() const noexcept override;
    [[nodiscard]] RECT ReadRectAtVslot3C() const override;
    [[nodiscard]] std::int32_t ReadModeFiveChangedAtVslot6C() const override;

    [[nodiscard]] const RECT& RectAtVslot3C() const noexcept;
    [[nodiscard]] std::int32_t StateAt14() const noexcept;
    [[nodiscard]] std::int32_t StateAt18() const noexcept;
    [[nodiscard]] std::int32_t StateAt20() const noexcept;
    [[nodiscard]] std::int32_t StateAt48() const noexcept;
    [[nodiscard]] std::int32_t StateAt50() const noexcept;
    [[nodiscard]] std::int32_t InitialResourceSlotAt54() const noexcept;
    [[nodiscard]] std::int32_t StateAt58() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationControlResourceGroupRuntime>&
        ResourcesAt28() const noexcept;
    [[nodiscard]] const std::shared_ptr<CgOperationControlResourceGroupRuntime>&
        CurrentResourcesAtVslot5C() const noexcept;
    void WriteCheckBoxStateAt5037D0(std::int32_t value);
    [[nodiscard]] std::int32_t CheckBoxStateAt5C() const noexcept;

private:
    [[nodiscard]] std::int32_t ResourceSlotAtVslot38() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>*
        ResolveCurrentSurface() const;
    [[nodiscard]] POINT DrawPositionAt503700(
        const RuntimeGraphicsSurface& surface) const noexcept;
    [[nodiscard]] bool HitTestButtonAt502EB0(const POINT& point);

    CgOperationControlKind kind_ = CgOperationControlKind::Static;
    std::int32_t eventValueAt04_ = 0;
    std::int32_t childIdAt0C_ = -1;
    std::int32_t stateAt14_ = 1;
    std::int32_t stateAt18_ = 1;
    std::int32_t stateAt1C_ = 0;
    std::int32_t stateAt20_ = 0;
    std::shared_ptr<CgOperationControlResourceGroupRuntime> resourcesAt28_;
    std::shared_ptr<CgOperationControlResourceGroupRuntime> checkedResourcesAt50_;
    RECT rectAt34_{};
    CgOperationClickedEventRef clickedEventAt3C_;
    std::int32_t clickedEventEnabledAt48_ = 1;
    std::int32_t stateAt50_ = 0;
    std::int32_t initialResourceSlotAt54_ = 7;
    std::int32_t stateAt58_ = 0;
    std::int32_t checkBoxStateAt5C_ = 0;
    ICgOperationButtonInputServices* buttonInputServicesAt4C_ = nullptr;
};

// 外部输入只负责查询；当前控件的切换由真实父 Dialog 持有并处理。
class ICgOperationSliderInputSource
{
public:
    virtual ~ICgOperationSliderInputSource() = default;

    [[nodiscard]] virtual bool ReadInputAtVslot70(
        std::int32_t eventValue) = 0;
    [[nodiscard]] virtual POINT ReadPointerAtVslot74() = 0;
    [[nodiscard]] virtual std::int32_t ReadStepDirectionAtVslot78() = 0;
    [[nodiscard]] virtual bool ReadInputAtVslot7C() = 0;
    [[nodiscard]] virtual bool ReadPressAtVslot80() = 0;
    [[nodiscard]] virtual bool ReadReleaseAtVslot84() = 0;
    [[nodiscard]] virtual std::uint32_t ReadSurfacePixelAtGetSurfacePixel(
        GraphicsSurfaceHandle handle,
        std::int32_t x,
        std::int32_t y) = 0;
};

class ICgOperationSliderInputServices : public ICgOperationSliderInputSource
{
public:
    ~ICgOperationSliderInputServices() override = default;

    virtual void SetActiveControlAtVslot30(
        ICgOperationDialogEventTarget& control) = 0;
};

// GuiControl::Slider(0x4F3C00) 的生产状态。Slider 的 +0x70 虚槽是
// reverse getter，并非 Button 的状态写入槽，因此不能与 Static/Button 共用实现类。
class CgOperationSliderRuntime final
    : public ICgOperationChildControl
{
public:
    CgOperationSliderRuntime(
        ICgOperationSliderInputServices& inputServices,
        std::int32_t eventValueAt04,
        std::int32_t childIdAt0C,
        std::shared_ptr<CgOperationControlResourceGroupRuntime> thumbResources,
        std::shared_ptr<CgOperationControlResourceGroupRuntime> trackResources,
        std::int32_t orientationAt50,
        std::int32_t reverseAt54,
        const RECT& thumbRectAt64,
        const RECT& travelOffsetAt7C,
        std::int32_t hitTestResourceSlotAt94);

    [[nodiscard]] CgOperationControlKind ConstructionKind() const override;
    [[nodiscard]] std::int32_t EventValueAt04() const override;
    [[nodiscard]] std::int32_t ChildIdAt0C() const override;
    [[nodiscard]] std::int32_t InputModeAt08() const override;
    void InvokeAtVslot08() override;
    [[nodiscard]] bool DrawAtVslot0C(
        IGraphicsSurfaceCompositionApi& graphics,
        const RECT* destinationClip) override;
    [[nodiscard]] bool CompositeAtVslot10(
        IGraphicsSurfaceCompositionApi& graphics,
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip) override;
    void CompositeReferenceAtVslot10(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip) override;
    [[nodiscard]] std::int32_t ProcessAtVslot14() override;
    [[nodiscard]] std::int32_t ProcessAtVslot18() override;
    void GainFocusAtVslot20() override;
    void LoseFocusAtVslot24() override;
    void WriteAtVslot44(std::int32_t value) override;
    [[nodiscard]] std::int32_t ReadAtVslot48() const noexcept override;
    void WriteAtVslot4C(std::int32_t value) override;
    void WriteAtVslot70(std::int32_t value) override;
    void BindClickedEventAt3C(CgOperationClickedEventRef event) override;
    void WriteClickedEventEnabledAt48(std::int32_t value) override;
    void InvokeReferencedObjectAt3CAtVslot00() override;
    [[nodiscard]] std::int32_t VisualStateAtVslot38() const noexcept override;
    [[nodiscard]] RECT ReadRectAtVslot3C() const override;
    [[nodiscard]] std::int32_t ReadModeFiveChangedAtVslot6C() const override;

    void SetRangeAt537680(std::int32_t minimum, std::int32_t maximum);
    void SetStepAt5384F0(std::int32_t step) noexcept;
    void SetValueAt538520(std::int32_t value);
    void SetStepInputEnabledAt538BA0(std::int32_t value) noexcept;
    void SetHoveredAt521A80() noexcept;
    void ClearHoveredAt521AB0() noexcept;
    void WriteDraggingAt8C(std::int32_t value) noexcept;
    void WritePressedAtCC(std::int32_t value) noexcept;
    void SetHitTestResourceSlotAt5361E0(std::int32_t slot);
    [[nodiscard]] std::int32_t ProcessInputAt521820();

    [[nodiscard]] const RECT& BoundsRect() const noexcept;
    [[nodiscard]] POINT ThumbPositionAt98() const noexcept;
    [[nodiscard]] std::int32_t CurrentValueAtB4() const noexcept;
    [[nodiscard]] std::int32_t MinimumAtB8() const noexcept;
    [[nodiscard]] std::int32_t MaximumAtBC() const noexcept;
    [[nodiscard]] std::int32_t StepAtC0() const noexcept;
    [[nodiscard]] std::int32_t ReverseAtVslot70() const noexcept;
    [[nodiscard]] std::int32_t ChangedAtB0() const noexcept;
    [[nodiscard]] std::int32_t HitTestResourceSlotAt94() const noexcept;

private:
    [[nodiscard]] bool CompositeSelectedSurfaces(IGraphicsSurfaceCompositionApi& graphics,
        GraphicsSurfaceDestination destination, const RECT* destinationClip,
        const YtlReferenceSlotRuntime* argument);
    [[nodiscard]] std::int32_t BaseResourceSlotAt50AEA0() const noexcept;
    [[nodiscard]] std::int32_t ThumbResourceSlot() const noexcept;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>*
        ResolveResource(
            const CgOperationControlResourceGroupRuntime& group,
            std::int32_t slot) const;
    [[nodiscard]] POINT TrackDrawPosition(
        const RuntimeGraphicsSurface& surface) const noexcept;
    [[nodiscard]] POINT ThumbDrawPosition(
        const RuntimeGraphicsSurface& surface) const noexcept;
    [[nodiscard]] RECT TravelRect() const noexcept;
    [[nodiscard]] RECT BaseInteractionRect() const noexcept;
    [[nodiscard]] RECT ThumbHitRect() const noexcept;
    [[nodiscard]] bool HitTestThumbAt502CE0(const POINT& point);
    [[nodiscard]] std::int32_t ValueFromCoordinateAt53AE10(
        std::int32_t coordinate) const;
    void RecalculateThumbPosition(std::int32_t effectiveValue);

    ICgOperationSliderInputServices* inputServices_ = nullptr;
    std::int32_t eventValueAt04_ = 0;
    std::int32_t childIdAt0C_ = -1;
    std::int32_t stateAt14_ = 1;
    std::int32_t stateAt18_ = 1;
    std::int32_t stateAt1C_ = 0;
    std::int32_t stateAt20_ = 0;
    std::array<std::shared_ptr<CgOperationControlResourceGroupRuntime>, 2>
        resourceGroupsAt28_;
    RECT boundsRectAt34_{};
    CgOperationClickedEventRef clickedEventAt3C_;
    std::int32_t clickedEventEnabledAt48_ = 1;
    std::int32_t orientationAt50_ = 0;
    std::int32_t reverseAt54_ = 0;
    std::int32_t stepInputEnabledAt58_ = 1;
    POINT thumbRectOffsetAt5C_{};
    RECT thumbRectAt64_{};
    POINT travelOffsetAt74_{};
    RECT travelRectAt7C_{};
    std::int32_t draggingAt8C_ = 0;
    std::int32_t hoveredAt90_ = 0;
    std::int32_t hitTestResourceSlotAt94_ = 7;
    POINT thumbPositionAt98_{};
    std::int32_t changedAtB0_ = 0;
    std::int32_t currentValueAtB4_ = 0;
    std::int32_t minimumAtB8_ = 0;
    std::int32_t maximumAtBC_ = 100;
    std::int32_t stepAtC0_ = 1;
    POINT trackResourceOffsetAtC4_{};
    std::int32_t pressedAtCC_ = 0;
    POINT dragOffsetAtD0_{};
};

// GuiDialog::5230B0/5231C0 的公共子控件绘制主机。+0x34 和 +0x38
// 保留 offset 名称，因为现有消费者只证明了它们分别控制临时状态切换和总绘制开关。
class CgOperationDialogRenderHostRuntime final
{
public:
    explicit CgOperationDialogRenderHostRuntime(
        IGraphicsSurfaceCompositionApi& graphics) noexcept;

    void WriteModeAt34(std::int32_t value) noexcept;
    void WriteEnabledAt38(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t ModeAt34() const noexcept;
    [[nodiscard]] std::int32_t EnabledAt38() const noexcept;

    void DrawChildrenAt5230B0(
        std::span<const CgOperationChildControlRef> children,
        const RECT& clip);
    // 原按值 Ref 入口：consumed 已增持，禁用或异常路径也由本次调用消费。
    void CompositeChildrenAt5231C0(
        const std::vector<CgOperationChildControlRef>& children,
        YtlReferenceSlotRuntime& consumed,
        const RECT* destinationClip);
    // 未迁移消费者的显式现代入口；不创建或消费原生 Ref 控制块。
    void CompositeChildrenAt5231C0(
        std::span<const CgOperationChildControlRef> children,
        GraphicsSurfaceDestination destination,
        const RECT* destinationClip);

private:
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    std::int32_t modeAt34_ = 1;
    std::int32_t enabledAt38_ = 1;
};

class CgOperationClickedEventRuntime final
    : public ICgOperationClickedEvent
{
public:
    explicit CgOperationClickedEventRuntime(std::int32_t eventValue) noexcept;

    [[nodiscard]] std::int32_t ClickedFlagAt04() const noexcept override;
    [[nodiscard]] std::int32_t EventValueAt08() const noexcept override;
    void WriteClickedFlagAt04(std::int32_t value) noexcept override;
    void ClearAtVslot04() noexcept override;

private:
    std::int32_t clickedFlagAt04_ = 0;
    std::int32_t eventValueAt08_ = 0;
};

// ClickedEventManager 的三指针 vector 行为。两个工厂分别固定
// 4DAC80 与 4DB860 的控件类型、ID 和事件插入顺序。
class CgOperationClickedEventManagerRuntime final
{
public:
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateEmptyAt4D7B90();
    void AppendBoundEventAt4D9620(CgOperationClickedEventRef event);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForButtonsAt4DAC80(
            const std::vector<CgOperationChildControlRef>& children);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForPopupAt4DB860(
            const std::vector<CgOperationChildControlRef>& children);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForOptionCharSelectAt4E4A20(
            const std::vector<CgOperationChildControlRef>& children);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForProgramSelectionSystemAt4DF420(
            const std::vector<CgOperationChildControlRef>& children,
            bool mode01);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForProgramSelectionMainAt4DCEB0(
            const std::vector<CgOperationChildControlRef>& children,
            bool mode01);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForSaveLoadSystemAt4F0C80(
            const std::vector<CgOperationChildControlRef>& children);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForSaveLoadPageAt4ED190(
            const std::vector<CgOperationChildControlRef>& eventChildren);
    [[nodiscard]] static std::shared_ptr<CgOperationClickedEventManagerRuntime>
        CreateForSaveLoadPopupAt4EF660(
            const std::vector<CgOperationChildControlRef>& eventChildren);

    void ClearAllAt532200() noexcept;
    [[nodiscard]] std::int32_t PollFirstAt50FDB0(
        std::int32_t clearMatchedEvent) noexcept;
    [[nodiscard]] const std::vector<CgOperationClickedEventRef>&
        EventsInInsertionOrder() const noexcept;
    [[nodiscard]] ICgOperationClickedEvent& EventForValue(
        std::int32_t eventValue) const;

private:
    explicit CgOperationClickedEventManagerRuntime(
        std::vector<CgOperationClickedEventRef> events) noexcept;

    std::vector<CgOperationClickedEventRef> events_;
};

class ICgOperationPopupPageResource
{
public:
    virtual ~ICgOperationPopupPageResource() = default;

    virtual void ClearAt4143A0(const RECT* rect) = 0;
};

using CgOperationPopupPageResourceRef =
    std::shared_ptr<ICgOperationPopupPageResource>;

class ICgOperationPopupPageProducer
{
public:
    virtual ~ICgOperationPopupPageProducer() = default;

    virtual void DrawSelectionAt4FEAE0(
        std::vector<CgOperationPopupPageResourceRef>& pageResources,
        const std::shared_ptr<RuntimeGraphicsSurface>& independentSurfaceAt7C,
        std::int32_t itemIndex,
        std::int32_t available) = 0;
};

using CgOperationPopupPageProducerRef =
    std::shared_ptr<ICgOperationPopupPageProducer>;

class ICgOperationButtonsServices
{
public:
    virtual ~ICgOperationButtonsServices() = default;

    virtual void UpdateInheritedAtVslot54() = 0;
};

struct CgOperationButtonsVisualLayout final
{
    std::array<POINT, 3> digitPositions{};
};

class ICgOperationButtonsVisuals
{
public:
    virtual ~ICgOperationButtonsVisuals() = default;
    virtual void ApplySelectionAt537010(std::int32_t selection) = 0;
    [[nodiscard]] virtual const std::shared_ptr<RuntimeGraphicsSurface>&
        TargetAt522F80(std::int32_t modeAt34) const noexcept = 0;
};

// 0x537010 的两目标 Surface 合成器。资源必须保持原 Dialog+0x8C 的顺序。
class CgOperationButtonsVisualRuntime final
    : public ICgOperationButtonsVisuals
{
public:
    CgOperationButtonsVisualRuntime(
        IGraphicsSurfaceCompositionApi& graphics,
        std::shared_ptr<RuntimeGraphicsSurface> target0,
        std::shared_ptr<RuntimeGraphicsSurface> target1,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dialogResources,
        CgOperationButtonsVisualLayout layout);

    void ApplySelectionAt537010(std::int32_t selection) override;
    [[nodiscard]] const std::shared_ptr<RuntimeGraphicsSurface>&
        TargetAt522F80(std::int32_t modeAt34) const noexcept override;

private:
    [[nodiscard]] const RuntimeGraphicsSurface& ResourceAt(
        std::int32_t index) const;
    void BlitPair(
        std::int32_t digit,
        const POINT& absolutePosition);

    static constexpr std::size_t kRequiredResourceCount = 20u;

    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    std::shared_ptr<RuntimeGraphicsSurface> target0_;
    std::shared_ptr<RuntimeGraphicsSurface> target1_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> dialogResources_;
    CgOperationButtonsVisualLayout layout_{};
};

// CGOperationButtons 的 Dialog 可见生产状态。对象布局依据 0x4DAC80，
// 覆盖虚槽依据原虚表 0x5C0000。
class CgOperationButtonsRuntime final
    : public ICgOperationDialogPrimaryControl
{
public:
    CgOperationButtonsRuntime(
        ICgOperationButtonsServices& services,
        std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager,
        std::shared_ptr<ICgOperationButtonsVisuals> visuals,
        std::vector<CgOperationChildControlRef> children,
        const RECT& rectAt80);

    void OpenAtVslot44() override;
    [[nodiscard]] std::int32_t StateAtVslot20() const override;
    [[nodiscard]] std::int32_t UpdateAtVslot48() override;
    [[nodiscard]] std::int32_t EventAtVslot50() const override;
    [[nodiscard]] std::int32_t WriteAtVslot1C(
        std::int32_t value) override;
    void InvokeAtVslot68() override;
    void ApplySelectionAt537010(std::int32_t selection) override;
    [[nodiscard]] RECT* ReadRectAtVslot8C(RECT* output) const override;

    void CompositeAtVslot5C(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip) override;
    [[nodiscard]] std::int32_t WriteAtVslot24(std::int32_t value) noexcept;
    [[nodiscard]] std::int32_t StateAt38() const noexcept;

private:
    static constexpr std::size_t kRequiredControlCount = 6u;

    ICgOperationButtonsServices* services_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager_;
    std::shared_ptr<ICgOperationButtonsVisuals> visuals_;
    RECT rectAt80_{};
    std::int32_t stateAt34_ = 1;
    std::int32_t stateAt38_ = 1;
    bool eventAt90Initialized_ = false;
    std::int32_t eventAt90_ = -1;
};

class ICgOperationPopupServices
{
public:
    virtual ~ICgOperationPopupServices() = default;

    virtual void UpdateInheritedAtVslot54() = 0;
    virtual void AssignPageResourceToChild9At527500(
        const CgOperationChildControlRef& child9,
        const CgOperationPopupPageResourceRef& resource) = 0;
    [[nodiscard]] virtual std::size_t
        ParentSelectionCountAt80And84() const = 0;
    [[nodiscard]] virtual bool ReadInputAtVslot6C(
        std::int32_t action) = 0;
    [[nodiscard]] virtual bool ReadInputAtVslot70(
        std::int32_t action) = 0;
};

struct CgOperationKeyMapping final
{
    std::int32_t sourceControlId = 0;
    std::int32_t targetControlId = 0;
};

// 0x504CE0 创建的 GuiOperationKey 行为数据。事件值和映射顺序均保持原表顺序。
class CgOperationPopupKeyRuntime final
{
public:
    CgOperationPopupKeyRuntime(
        std::int32_t eventValue,
        std::array<CgOperationKeyMapping, 20> mappings);

    [[nodiscard]] std::int32_t EventValueAt04() const noexcept;
    [[nodiscard]] const std::array<CgOperationKeyMapping, 20>&
        MappingsAt0C() const noexcept;
    [[nodiscard]] std::int32_t TargetForSource(
        std::int32_t sourceControlId) const;

private:
    std::int32_t eventValueAt04_ = 0;
    std::array<CgOperationKeyMapping, 20> mappings_{};
};

// 0x4DB860 将四个 GuiOperationKey 按 3、4、1、2 的固定顺序写入 GuiDialog。
class CgOperationPopupKeySetRuntime final
{
public:
    CgOperationPopupKeySetRuntime();

    [[nodiscard]] const std::array<CgOperationPopupKeyRuntime, 4>&
        KeysInInsertionOrder() const noexcept;
    [[nodiscard]] const CgOperationPopupKeyRuntime& KeyForEvent(
        std::int32_t eventValue) const;

private:
    std::array<CgOperationPopupKeyRuntime, 4> keys_;
};

// CGOperationPopup 的 Dialog 可见生产状态。固定 childId 为 1、对象尺寸
// 0x9C，覆盖虚槽依据原虚表 0x5C0094。
class CgOperationPopupRuntime final
    : public ICgOperationDialogPopupControl
{
public:
    CgOperationPopupRuntime(
        ICgOperationPopupServices& services,
        std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager,
        std::vector<CgOperationChildControlRef> children,
        std::vector<CgOperationPopupPageResourceRef> pageResources,
        const RECT& rectAt68,
        std::shared_ptr<RuntimeGraphicsSurface> independentSurfaceAt7C,
        CgOperationPopupPageProducerRef pageProducer);

    void OpenAtVslot44() override;
    [[nodiscard]] std::int32_t StateAtVslot20() const override;
    [[nodiscard]] std::int32_t WriteAtVslot1C(std::int32_t value) override;
    void CompositeAtVslot5C(IGraphicsSurfaceCompositionApi& graphics,
        YtlReferenceSlotRuntime& argument, const RECT* destinationClip) override;
    [[nodiscard]] std::int32_t UpdateAtVslot48() override;
    [[nodiscard]] std::int32_t EventAtVslot50() const override;
    [[nodiscard]] std::int32_t WriteAtVslot24(
        std::int32_t value) override;
    [[nodiscard]] std::int32_t CurrentPageAt94() const override;
    [[nodiscard]] std::int32_t PageCountAt88And8C() const override;
    void SetPageAt527500(std::int32_t page) override;
    void ResetPagesAt5021C0() override;
    void DrawSelectionAt4FEAE0(
        std::int32_t itemIndex,
        std::int32_t available) override;
    [[nodiscard]] CgOperationDialogEventTargetRef
        ResolveEventTargetAtVslot14(std::int32_t event) override;
    void InvokeAtVslot68() override;
    void SubmitAtVslot30(CgOperationDialogEventTargetRef value) override;
    [[nodiscard]] RECT* ReadRectAtVslot8C(RECT* output) const override;

    [[nodiscard]] std::int32_t UpdateInputAt510860();
    void ResetInputAt510650();
    [[nodiscard]] std::int32_t StateAt38() const noexcept;
    [[nodiscard]] std::int32_t InputLatchAt98() const noexcept;
    [[nodiscard]] const CgOperationPopupKeySetRuntime&
        OperationKeysAt24() const noexcept;

private:
    [[nodiscard]] CgOperationChildControlRef ChildByEventValueAt04(
        std::int32_t eventValue) const;

    ICgOperationPopupServices* services_ = nullptr;
    std::vector<CgOperationChildControlRef> children_;
    std::shared_ptr<CgOperationClickedEventManagerRuntime> eventManager_;
    std::shared_ptr<RuntimeGraphicsSurface> independentSurfaceAt7C_;
    CgOperationPopupPageProducerRef pageProducer_;
    std::vector<CgOperationPopupPageResourceRef> pageResources_;
    RECT rectAt68_{};
    std::int32_t stateAt34_ = 1;
    std::int32_t stateAt38_ = 1;
    bool eventAt78Initialized_ = false;
    std::int32_t eventAt78_ = -1;
    bool currentPageAt94Initialized_ = false;
    std::int32_t currentPageAt94_ = -1;
    std::int32_t inputLatchAt98_ = 0;
    std::int32_t opaqueAt30_ = 0;
    CgOperationDialogEventTargetRef activeControlAt4C_;
    CgOperationPopupKeySetRuntime operationKeysAt24_;
};
}
#include <array>
