#pragma once
#include <Platform/InputViews.h>

#include <Runtime/CgOperationControlsRuntime.h>
#include <Runtime/RootTransitionFrameRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/GuiModalNativeAdapters.h>
#include <Runtime/OpacityDialogObjectGraphRuntime.h>

#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Platform { class IInputRuntime; }
namespace KaGuYa::Start::Runtime
{
class ICgOperationSurfaceFactory;

struct GuiModalDialogDependencies final
{
    ICgOperationSurfaceFactory& resources;
    IGraphicsSurfaceApi& surfaces;
    const Platform::IInputRuntime& input;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenUpdateApi& screen;
    IGraphicsSurfacePixelApi& pixels;
    IGuiModalRootSource& rootSource;
    IModalDialogRunnerServices& modal;
};

// 5314A0/531D30 -> 441980：模块级原始提交不附加 Initialized 条件。
class GuiModalRawComposition final : public IGraphicsSurfaceCompositionApi
{
public:
    explicit GuiModalRawComposition(IGraphicsWindowSurfaceApi& api) noexcept : api_(api) {}
    void ClearSurface(GraphicsSurfaceHandle h, std::uint32_t color, const RECT* rect) override
    {
        if (h != 0) api_.SurfaceClearRaw(h, color, rect);
    }
    bool DrawSurface(GraphicsSurfaceHandle h, std::int32_t x, std::int32_t y,
        const SIZE* size, const RECT* source, const RECT* clip, std::int32_t anchor) override
    {
        return h != 0 && static_cast<std::uint8_t>(api_.SurfaceDrawRaw(h, x, y, size, source, clip, anchor)) != 0;
    }
    bool BlitSurface(GraphicsSurfaceHandle destination, GraphicsSurfaceHandle source,
        std::int32_t x, std::int32_t y, const SIZE* size, const RECT* sourceRect,
        const RECT* clip, std::int32_t anchor) override
    {
        return destination != 0 && source != 0 && static_cast<std::uint8_t>(api_.SurfaceBlitRaw(
            destination, source, x, y, size, sourceRect, clip, anchor)) != 0;
    }
private:
    IGraphicsWindowSurfaceApi& api_;
};

// 4E0BE0 共用模态基类状态。不是原 PE 内存布局的二进制覆盖。
// 派生类只提供 +54 输入分支、+90 结果判定和自己的构造/退出动作。
class GuiModalDialogState : public ICgOperationButtonInputServices
{
public:
    explicit GuiModalDialogState(GuiModalDialogDependencies dependencies);
    ~GuiModalDialogState() noexcept override;
    GuiModalDialogState(const GuiModalDialogState&) = delete;
    GuiModalDialogState& operator=(const GuiModalDialogState&) = delete;

    void Release() noexcept;
    void InstallGameInput();
    GuiInputNativeBody& InputBody() const;
    bool ReadPointerMoved();
    void AppendStatic(std::int32_t event, const char* name);
    GraphicsSurfaceNativeBody& Captured() const;
    void AddCapturedRect();
    void SetActive(ICgOperationChildControl* child);
    void SetActiveControlAtVslot30(ICgOperationDialogEventTarget& control) override;
    bool ReadInputAtVslot6C(std::int32_t action) override;
    bool ReadInputAtVslot70(std::int32_t action) override;
    POINT ReadPointerAtVslot74() override;
    bool ReadPressAtVslot80() override;
    bool ReadReleaseAtVslot84() override;
    std::uint32_t ReadSurfacePixelAtGetSurfacePixel(GraphicsSurfaceHandle h,
        std::int32_t x, std::int32_t y) override;
    virtual void ResetInput();
    void ResetChildren();
    bool TryCandidate(std::int32_t event, bool reset);
    virtual std::int32_t ProcessActionFive();
    std::int32_t ProcessUpperInput();
    void ProcessInput();
    void BeginBaseAt50CF50();
    bool PollAt52DBC0();
    void EndBaseAt508E80();
    void FlushDirtyRectsAt5267E0();
    void DrawAt5230B0(const RECT& clip);
    void CompositeAt5231C0(GraphicsSurfaceDestination destination, const RECT* clip);
    virtual void ProcessModalInputAtVslot54() = 0;
    virtual std::int32_t PollModalResultAtVslot90() = 0;

    GuiModalDialogDependencies deps;
    GuiModalRawComposition raw;
    CgOperationDialogRenderHostRuntime renderer;
    ModalDialogRunnerRuntime runner;
    std::vector<CgOperationChildControlRef> children;
    YtlReferenceSlotRuntime inputSlot;
    GuiNavigationReferences navigation{};
    YtlReferenceSlotRuntime dirtySlot;
    GuiModalInputSource inputSource;
    GuiModalDirtyRectView dirty;
    GraphicsSurfaceReference captured;
    std::vector<CgOperationClickedEventRef> events;
    std::optional<std::int32_t> result;
    std::optional<std::int32_t> phase;
    std::optional<std::int32_t> actionFive;
    std::int32_t inputLatch = 0;
    ICgOperationChildControl* active = nullptr;
};
}
