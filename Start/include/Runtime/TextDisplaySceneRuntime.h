#pragma once

#include <Runtime/SceneControllerRuntime.h>

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ITextDisplaySurfaceResource
{
public:
    virtual ~ITextDisplaySurfaceResource() = default;
};

using TextDisplaySurfaceRef = std::shared_ptr<ITextDisplaySurfaceResource>;

enum class TextDisplayTextInitializationResult : std::uint8_t
{
    Success,
    ParseFailed,
    GraphicsInitializationFailed,
};

enum class TextDisplayPhase : std::uint32_t
{
    PrepareLayers = 0,
    BuildTransition = 1,
    AnimateTransition = 2,
    ReleaseTransition = 3,
    WaitForAdvance = 4,
    AdvanceScene = 5,
};

// CTextDisp+0x0C 的 0x30 字节 operation。引用槽使用强引用表达，数值槽保持
// 原 offset 名；业务未闭环的 selector 不被重命名为某种假定效果。
struct TextDisplayTransitionOperation final
{
    std::int32_t valueAt00 = 0;
    TextDisplaySurfaceRef compositeFrameAt04;
    TextDisplaySurfaceRef baseFrameAt08;
    TextDisplaySurfaceRef effectMaskAt0C;
    std::int32_t valueAt10 = 0;
    std::int32_t valueAt14 = 0;
    std::int32_t valueAt18 = 0;
    std::int32_t selectorAt1C = -1;
    std::int32_t valueAt20 = 0;
    std::int32_t progressAt24 = 0;
    std::int32_t targetAt28 = 255;
    std::int32_t auxiliaryAt2C = 0;

    void Reset() noexcept;
};

class ITextDisplaySceneServices
{
public:
    virtual ~ITextDisplaySceneServices() = default;

    // 4A2024..4A20A8：目标尺寸来自 Graphics wrapper，创建参数固定为 (1,1)。
    virtual void ReadGraphicsTargetSize(
        std::int32_t& width,
        std::int32_t& height) const = 0;
    [[nodiscard]] virtual TextDisplaySurfaceRef CreateTextSurface(
        std::int32_t width,
        std::int32_t height,
        bool hasAlpha,
        bool clearOnCreate) = 0;

    [[nodiscard]] virtual std::int32_t ReadNamedValue(
        const char* name) const = 0;
    [[nodiscard]] virtual bool CreateFontFromTextContext(
        std::int32_t contextIndex) = 0;
    [[nodiscard]] virtual TextDisplayTextInitializationResult InitializeText(
        std::int32_t textStringIndex) = 0;
    [[nodiscard]] virtual RECT ReadTextDrawRect() const = 0;
    virtual void SetTextBasePosition(std::int32_t x, std::int32_t y) = 0;
    [[nodiscard]] virtual bool BlitAllText(
        const TextDisplaySurfaceRef& target) = 0;
    [[nodiscard]] virtual TextDisplaySurfaceRef LoadEffectSurface(
        std::int32_t effectIndex) = 0;

    [[nodiscard]] virtual std::uint32_t ReadScriptSlot2DwordAt04() const = 0;
    virtual void WriteScriptSlot2DwordAt04(std::uint32_t value) = 0;
    [[nodiscard]] virtual bool ReadResultLayer5TransitionPredicate() const = 0;
    [[nodiscard]] virtual bool ReadScriptSlot2DwordAt18Bit1() const = 0;
    [[nodiscard]] virtual std::int32_t ReadTextLength() const = 0;
    [[nodiscard]] virtual std::uint32_t ReadScriptSlot1DwordAt3C() const = 0;

    // 46A980 不读取业务参数；EBX/ESI 仅为保存寄存器，并非 FrameContext。
    // 旧名称为源兼容保留；生产接 manager.PrepareLayersAt46A980()，调用者忽略 AL。
    virtual void PrepareSevenResultLayersWithFrameContext() = 0;
    [[nodiscard]] virtual TextDisplaySurfaceRef CaptureCurrentFrame() = 0;
    // 4BAA37..4BAA41 -> 4A8340：新建 alpha 画布并合成图层，不从遮罩复制颜色。
    [[nodiscard]] virtual TextDisplaySurfaceRef CaptureDisplayLayersWithAlpha() = 0;
    [[nodiscard]] virtual TextDisplaySurfaceRef CreateCompositeFromBase(
        const TextDisplaySurfaceRef& baseFrame) = 0;
    virtual void BlitTextSurfaceIntoComposite(
        const TextDisplaySurfaceRef& composite,
        const TextDisplaySurfaceRef& textSurface) = 0;
    [[nodiscard]] virtual std::int32_t ReadTransitionStepFromSlot1At70() const = 0;
    [[nodiscard]] virtual bool ShouldAccelerateTransition() const = 0;
    virtual void RenderTransition(TextDisplayTransitionOperation& operation) = 0;
    virtual void InvalidateScreenRect(const void* rect) = 0;
    virtual void CommitTextDisplayState() = 0;

    [[nodiscard]] virtual bool ShouldAdvanceFromResultLayer5() const = 0;
    [[nodiscard]] virtual bool GlobalWaitConditionAt2C34() const = 0;
    [[nodiscard]] virtual bool PollInputGroup6() = 0;
    [[nodiscard]] virtual bool ReadScriptSlot2DwordAt14Bit1() const = 0;

    [[nodiscard]] virtual bool ReadResultLayer5DrawPredicate() const = 0;
    virtual void DrawGlobalDisplayLayers() = 0;
    virtual void PrepareResultLayer0TextRectCollection() = 0;
    [[nodiscard]] virtual std::vector<RECT> ReadResultLayer0TextRects() const = 0;
    virtual void CompleteResultLayer0TextRectCollection() = 0;
    virtual void DrawTextSurface(
        const TextDisplaySurfaceRef& textSurface,
        const RECT& dirtyRect) = 0;

    virtual void AdvanceActiveSceneFromPending() = 0;
};

// TextDispScene::CTextDisp（4A1F30/4BA940/4B5BF0）的六阶段生产状态机。
class TextDisplaySceneRuntime final : public ISceneTask
{
public:
    TextDisplaySceneRuntime(
        ITextDisplaySceneServices& services,
        std::uint32_t opaqueStorageAt68);

    // 4C3630：严格执行 Update 后 Draw；argument 未被场景使用。
    void Run(std::int32_t argument) override;

    [[nodiscard]] TextDisplayPhase Phase() const noexcept;
    [[nodiscard]] bool DrawEnabled() const noexcept;
    [[nodiscard]] bool DrawTextOverlay() const noexcept;
    [[nodiscard]] std::uint32_t WaitCounter() const noexcept;
    [[nodiscard]] std::uint32_t OpaqueStorageAt68() const noexcept;
    [[nodiscard]] const RECT& DirtyTextRect() const noexcept;
    [[nodiscard]] const TextDisplayTransitionOperation& Transition() const noexcept;

private:
    void Update();
    void BuildTransition();
    void AnimateTransition();
    void ReleaseTransition();
    void WaitForAdvance();
    void AdvanceScene();
    void Draw();

    ITextDisplaySceneServices* services_ = nullptr;
    TextDisplayPhase phase_ = TextDisplayPhase::PrepareLayers;
    std::uint32_t savedScriptSlot2DwordAt04_ = 0;
    TextDisplayTransitionOperation transition_;
    std::int32_t transitionStep_ = 0;
    TextDisplaySurfaceRef baseFrame_;
    TextDisplaySurfaceRef compositeFrame_;
    bool skipTransition_ = false;
    TextDisplaySurfaceRef effectSurface_;
    std::uint32_t opaqueStorageAt68_ = 0;
    bool drawEnabled_ = false;
    bool drawTextOverlay_ = false;
    std::uint32_t waitCounter_ = 0;
    TextDisplaySurfaceRef textSurface_;
    RECT dirtyTextRect_{};
};
}
