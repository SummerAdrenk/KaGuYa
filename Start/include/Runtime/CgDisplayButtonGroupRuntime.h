#pragma once

#include <Runtime/CgDisplayButtonGroupStateRuntime.h>
#include <Runtime/CgDisplayPublicControlsRuntime.h>
#include <Runtime/CgDisplaySceneRuntime.h>
#include <Runtime/ScenePlaybackRuntime.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class CgDisplayButtonSoundAt4BCD70 : std::uint8_t
{
    ConfirmOrPageAt61A564,
    HoverOrInvalidAt61A568,
    ExitOrModeAt61A56C,
};

class ICgDisplayButtonGroupDirtyRects
{
public:
    virtual ~ICgDisplayButtonGroupDirtyRects() = default;

    virtual void PrepareAtVslot04() = 0;
    [[nodiscard]] virtual std::size_t CountAtVslot18() const noexcept = 0;
    [[nodiscard]] virtual std::span<const RECT> RectsAtVslot1C() const noexcept = 0;
    [[nodiscard]] virtual std::int32_t AddAtVslot08(const RECT& rect) = 0;
    virtual void ClearAtVslot00() noexcept = 0;
    // 受控测试或现代DIR没有原生Ref，不能由Rects()的矩形存储拼造引用。
    [[nodiscard]] virtual const YtlReferenceSlotRuntime* BorrowNativeReference() const noexcept
        { return nullptr; }
};

class ICgDisplayButtonGroupSelector
{
public:
    virtual ~ICgDisplayButtonGroupSelector() = default;

    [[nodiscard]] virtual std::int32_t ProcessAtVslot48() = 0;
    [[nodiscard]] virtual std::int32_t SelectionAtVslot50() const = 0;
    virtual void AttachDirtyRectsAtVslot94(
        ICgDisplayButtonGroupDirtyRects& dirtyRects) = 0;
    virtual void ResetAtVslot90() = 0;
    virtual void DrawAtVslot60(const RECT& clip) = 0;
    virtual void CompositeAtVslot5C(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) = 0;
};

[[nodiscard]] std::unique_ptr<ICgDisplayButtonGroupDirtyRects>
CreateCgDisplayButtonGroupDirtyRectsAt4A2E60();

class ICgDisplayButtonGroupRuntimeServices
{
public:
    virtual ~ICgDisplayButtonGroupRuntimeServices() = default;

    [[nodiscard]] virtual std::int32_t PollBaseInputAt4AA200() = 0;
    [[nodiscard]] virtual bool ReadInputEdgeAt47F4E0(std::int32_t action) = 0;
    [[nodiscard]] virtual bool ReadInputHeldAt4198D0(std::int32_t action) = 0;
    [[nodiscard]] virtual bool ReadAggregateInputByteAt11() const = 0;
    [[nodiscard]] virtual std::int32_t ReadAggregateInputDwordAt28() const = 0;
    [[nodiscard]] virtual bool ReadAggregateInputByteAt31() const = 0;
    [[nodiscard]] virtual bool PageAxisModeAt2C() const = 0;
    [[nodiscard]] virtual std::int32_t PageAxisDeltaAt34() const = 0;
    virtual void EmitSoundAt4BCD70(
        CgDisplayButtonSoundAt4BCD70 sound,
        std::int32_t channelFlag) = 0;
    virtual void SubmitInvalidatedRect(const RECT& rect) = 0;
    [[nodiscard]] virtual RECT ReadScreenRectAt4180C0() const = 0;
    [[nodiscard]] virtual GraphicsSurfaceHandle ResolveTransitionTargetHandle(
        const CgDisplayTransitionSourceRef& target) const = 0;
    // 默认解析既有GraphicsTransitionSourceRuntime::Surface().NativeBody()。
    // 其它目标适配器返回自己的真实正文；原生控件不退回纯句柄入口。
    [[nodiscard]] virtual GraphicsSurfaceNativeBody* ResolveTransitionTargetNativeBody(
        const CgDisplayTransitionSourceRef& target) const;
};

// CCGdispBtn / CModeReplayBtn 的共用行为 owner：原虚表
// 5B5190/5B529C 的 +04..+38 相同，析构均进入 4A4C60；构造器分别实现。
// 公共模式数组与选择器使用互斥原生Ref或显式受控域；其它页面与按钮
// 仍为现代模型。释放次序与三条绘制路径保持独立。
class CgDisplayButtonGroupRuntime final
    : public ICgDisplayButton,
      public IScenePlaybackButton,
      private ICgDisplayButtonGroupStateServices
{
public:
    CgDisplayButtonGroupRuntime(
        ICgDisplayButtonGroupRuntimeServices& services,
        IGraphicsSurfaceCompositionApi& graphics,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfaces,
        std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtons,
        std::vector<std::array<std::int32_t, 4>> navigationTable,
        CgDisplayPublicModeButtonsRuntime modeButtons,
        std::int32_t currentModeIndex,
        std::int32_t categoryIndex,
        std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons,
        CgDisplayPublicSelectorRuntime selector);
    CgDisplayButtonGroupRuntime(
        ICgDisplayButtonGroupRuntimeServices& services,
        IGraphicsSurfaceCompositionApi& graphics,
        std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyRects,
        std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfaces,
        std::vector<std::shared_ptr<CgUiButtonRuntime>> mainButtons,
        std::vector<std::array<std::int32_t, 4>> navigationTable,
        CgDisplayPublicModeButtonsRuntime modeButtons,
        std::int32_t currentModeIndex,
        std::int32_t categoryIndex,
        std::vector<std::shared_ptr<CgUiButtonRuntime>> pageButtons,
        CgDisplayPublicSelectorRuntime selector);
    ~CgDisplayButtonGroupRuntime() override;

    [[nodiscard]] std::int32_t ProcessSelectionAtVslot04() override;
    [[nodiscard]] bool PollInputReleaseGateAtVslot0C();
    [[nodiscard]] std::int32_t MoveSelectionAtVslot30();
    [[nodiscard]] std::int32_t ItemCountAt3C() const override;
    void RenderDirtyRegionsAtVslot34() override;
    void CompositeIntoAtVslot38(
        const CgDisplayTransitionSourceRef& target,
        const RECT* destinationClip) override;
    void ResetGroupAt4C1260(std::int32_t invalidateFullScreen) override;

    [[nodiscard]] CgDisplayButtonGroupStateRuntime& State() noexcept;
    [[nodiscard]] const CgDisplayButtonGroupStateRuntime& State() const noexcept;
    [[nodiscard]] std::size_t PageCountAt50() const noexcept;

private:
    [[nodiscard]] std::int32_t ProcessPageKeysAt4A9FC0();
    [[nodiscard]] bool PageButtonVisible(std::size_t index) const noexcept;
    void DrawButtonsToScreen(
        const RECT& clip);
    void DrawButtonsInto(
        GraphicsSurfaceDestination destination,
        const RECT* clip);
    void EmitPageSound();

    [[nodiscard]] std::int32_t SubmitDirtyRectAtVslot08(
        const RECT& rect) override;
    void ResetCategorySelectorAtVslot90() override;
    [[nodiscard]] RECT ReadScreenRectAt4180C0() const override;

    ICgDisplayButtonGroupRuntimeServices* services_ = nullptr;
    IGraphicsSurfaceCompositionApi* graphics_ = nullptr;
    // 0x4A2E60 在基类 +0x24 构造独占的 CDirtyRect ref_object；必须晚于
    // selector 和按钮销毁，避免它们在析构期间看到失效的 dirty-rect。
    std::unique_ptr<ICgDisplayButtonGroupDirtyRects> dirtyRects_;
    CgDisplayButtonGroupStateRuntime state_;
    std::vector<std::shared_ptr<RuntimeGraphicsSurface>> pageSurfacesAt50_;
    CgDisplayPublicModeButtonsRuntime modeButtonsAt44_;
    std::int32_t currentModeIndexAt40_ = 0;
    CgDisplayPublicSelectorRuntime selectorAt78_;
};
}
