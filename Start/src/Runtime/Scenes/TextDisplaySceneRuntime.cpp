#include <Runtime/TextDisplaySceneRuntime.h>

#include <algorithm>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char kTextContextName[] = "text_context";
constexpr char kTextStringName[] = "text_string";
constexpr char kTextPositionXName[] = "text_pos_x";
constexpr char kTextPositionYName[] = "text_pos_y";
constexpr char kTextPositionCenterName[] = "text_pos_center";
constexpr char kTextEffectName[] = "text_effect";

std::int32_t WrapAdd(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) + std::bit_cast<std::uint32_t>(right));
}

std::int32_t WrapSubtract(const std::int32_t left, const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left) - std::bit_cast<std::uint32_t>(right));
}
}

void TextDisplayTransitionOperation::Reset() noexcept
{
    // 4C57F0：十二个 DWORD 槽全部重置，selector 固定 -1、target 固定 255。
    valueAt00 = 0;
    compositeFrameAt04.reset();
    baseFrameAt08.reset();
    effectMaskAt0C.reset();
    valueAt10 = 0;
    valueAt14 = 0;
    valueAt18 = 0;
    selectorAt1C = -1;
    valueAt20 = 0;
    progressAt24 = 0;
    targetAt28 = 255;
    auxiliaryAt2C = 0;
}

TextDisplaySceneRuntime::TextDisplaySceneRuntime(
    ITextDisplaySceneServices& services,
    const std::uint32_t opaqueStorageAt68)
    : services_(&services), opaqueStorageAt68_(opaqueStorageAt68)
{
    std::int32_t width = 0;
    std::int32_t height = 0;
    services_->ReadGraphicsTargetSize(width, height);
    textSurface_ = services_->CreateTextSurface(width, height, true, true);
    if (!textSurface_)
        throw 1;

    const std::int32_t textContext = services_->ReadNamedValue(kTextContextName);
    if (!services_->CreateFontFromTextContext(textContext))
        throw 1;

    const std::int32_t textString = services_->ReadNamedValue(kTextStringName);
    switch (services_->InitializeText(textString))
    {
    case TextDisplayTextInitializationResult::Success:
        break;
    case TextDisplayTextInitializationResult::ParseFailed:
        throw 2;
    case TextDisplayTextInitializationResult::GraphicsInitializationFailed:
        throw 3;
    }

    std::int32_t x = services_->ReadNamedValue(kTextPositionXName);
    std::int32_t y = services_->ReadNamedValue(kTextPositionYName);
    if (services_->ReadNamedValue(kTextPositionCenterName) != 0)
    {
        const RECT textRect = services_->ReadTextDrawRect();
        // 4A257A..4A25A2：先按 DWORD 回绕求尺寸，再有符号除 2，最后回绕减去半尺寸。
        x = WrapSubtract(x, WrapSubtract(textRect.right, textRect.left) / 2);
        y = WrapSubtract(y, WrapSubtract(textRect.bottom, textRect.top) / 2);
    }
    services_->SetTextBasePosition(x, y);
    if (!services_->BlitAllText(textSurface_))
        throw 3;

    drawTextOverlay_ = false;
    const std::int32_t textEffect = services_->ReadNamedValue(kTextEffectName);
    skipTransition_ = false;
    if (textEffect == -2)
    {
        skipTransition_ = true;
    }
    else if (textEffect >= 0)
    {
        effectSurface_ = services_->LoadEffectSurface(textEffect);
        if (!effectSurface_)
            throw 4;
    }

    savedScriptSlot2DwordAt04_ = services_->ReadScriptSlot2DwordAt04();
    services_->WriteScriptSlot2DwordAt04(0u);
    drawEnabled_ = true;
    phase_ = TextDisplayPhase::PrepareLayers;
    waitCounter_ = 0u;

    if (!services_->ReadResultLayer5TransitionPredicate() &&
        services_->ReadScriptSlot2DwordAt18Bit1())
    {
        std::int32_t textLength = services_->ReadTextLength();
        if (textLength < 30)
            textLength = 30;
        waitCounter_ = static_cast<std::uint32_t>(textLength) *
            services_->ReadScriptSlot1DwordAt3C();
    }
}

void TextDisplaySceneRuntime::Run(const std::int32_t argument)
{
    static_cast<void>(argument);
    Update();
    Draw();
}

TextDisplayPhase TextDisplaySceneRuntime::Phase() const noexcept
{
    return phase_;
}

bool TextDisplaySceneRuntime::DrawEnabled() const noexcept
{
    return drawEnabled_;
}

bool TextDisplaySceneRuntime::DrawTextOverlay() const noexcept
{
    return drawTextOverlay_;
}

std::uint32_t TextDisplaySceneRuntime::WaitCounter() const noexcept
{
    return waitCounter_;
}

std::uint32_t TextDisplaySceneRuntime::OpaqueStorageAt68() const noexcept
{
    return opaqueStorageAt68_;
}

const RECT& TextDisplaySceneRuntime::DirtyTextRect() const noexcept
{
    return dirtyTextRect_;
}

const TextDisplayTransitionOperation& TextDisplaySceneRuntime::Transition() const noexcept
{
    return transition_;
}

void TextDisplaySceneRuntime::Update()
{
    switch (phase_)
    {
    case TextDisplayPhase::PrepareLayers:
        services_->PrepareSevenResultLayersWithFrameContext();
        phase_ = TextDisplayPhase::BuildTransition;
        return;

    case TextDisplayPhase::BuildTransition:
        BuildTransition();
        return;

    case TextDisplayPhase::AnimateTransition:
        AnimateTransition();
        return;

    case TextDisplayPhase::ReleaseTransition:
        ReleaseTransition();
        return;

    case TextDisplayPhase::WaitForAdvance:
        WaitForAdvance();
        return;

    case TextDisplayPhase::AdvanceScene:
        AdvanceScene();
        return;
    }
}

void TextDisplaySceneRuntime::BuildTransition()
{
    if (skipTransition_ || services_->ReadResultLayer5TransitionPredicate())
    {
        drawTextOverlay_ = true;
        phase_ = TextDisplayPhase::WaitForAdvance;
        services_->CommitTextDisplayState();
        return;
    }

    baseFrame_ = services_->CaptureCurrentFrame();
    transition_.Reset();
    if (effectSurface_)
    {
        compositeFrame_ = services_->CaptureDisplayLayersWithAlpha();
        transition_.selectorAt1C = 1;
        // 4BAA67..4BAA79：CSurface+4 的遮罩句柄写入 CTextDisp+18，即 operation+0C。
        transition_.effectMaskAt0C = effectSurface_;
        transition_.progressAt24 = 255;
        transition_.auxiliaryAt2C = 128;
        transition_.targetAt28 = -128;
    }
    else
    {
        compositeFrame_ = services_->CreateCompositeFromBase(baseFrame_);
        transition_.selectorAt1C = 0;
    }

    services_->BlitTextSurfaceIntoComposite(compositeFrame_, textSurface_);
    // 4BAAF5..4BAB0E：+04 是 composite 的 CSurface 对象，+08 才是 base 的 DLL 句柄。
    // 这里保活两者，生产绘制边界再分别解析对象和句柄，不能把两槽当作相同 ABI。
    transition_.compositeFrameAt04 = compositeFrame_;
    transition_.baseFrameAt08 = baseFrame_;
    transitionStep_ = services_->ReadTransitionStepFromSlot1At70();
    phase_ = TextDisplayPhase::AnimateTransition;
    drawEnabled_ = false;

    // 4BAB40 直接落入 phase 2；建立转场的同一帧必须渲染第一步。
    AnimateTransition();
}

void TextDisplaySceneRuntime::AnimateTransition()
{
    if (transition_.selectorAt1C == 1)
    {
        if (services_->ShouldAccelerateTransition())
            transition_.progressAt24 = transition_.targetAt28;

        if (transition_.progressAt24 < transition_.targetAt28)
            phase_ = TextDisplayPhase::ReleaseTransition;

        services_->RenderTransition(transition_);
        transition_.progressAt24 = WrapSubtract(
            transition_.progressAt24, transitionStep_);
        services_->InvalidateScreenRect(nullptr);
        return;
    }

    if (services_->ShouldAccelerateTransition())
    {
        transition_.progressAt24 = WrapAdd(transition_.progressAt24, 256);
        if (transition_.progressAt24 >= transition_.targetAt28)
            transition_.progressAt24 = transition_.targetAt28;
    }

    if (transition_.selectorAt1C == -1 ||
        transition_.progressAt24 >= transition_.targetAt28)
    {
        phase_ = TextDisplayPhase::ReleaseTransition;
    }

    services_->RenderTransition(transition_);
    transition_.progressAt24 = WrapAdd(
        transition_.progressAt24, transitionStep_);
    if (transition_.progressAt24 >= transition_.targetAt28)
        transition_.progressAt24 = transition_.targetAt28;
    services_->InvalidateScreenRect(nullptr);
}

void TextDisplaySceneRuntime::ReleaseTransition()
{
    drawEnabled_ = true;
    transition_.compositeFrameAt04.reset();
    transition_.baseFrameAt08.reset();
    transition_.effectMaskAt0C.reset();
    baseFrame_.reset();
    compositeFrame_.reset();
    effectSurface_.reset();
    phase_ = TextDisplayPhase::WaitForAdvance;
    drawTextOverlay_ = true;

    // 4BAC8E 直接落入 phase 4，等待条件必须在同一帧求值。
    WaitForAdvance();
}

void TextDisplaySceneRuntime::WaitForAdvance()
{
    bool advance = services_->ShouldAdvanceFromResultLayer5();
    if (!advance)
        advance = services_->GlobalWaitConditionAt2C34();
    if (!advance)
        advance = services_->PollInputGroup6();
    if (!advance)
        advance = services_->ReadScriptSlot2DwordAt14Bit1();

    if (!advance && services_->ReadScriptSlot2DwordAt18Bit1())
    {
        if (waitCounter_ != 0u)
        {
            --waitCounter_;
            return;
        }
        advance = true;
    }

    if (!advance)
        return;

    phase_ = TextDisplayPhase::AdvanceScene;
    drawTextOverlay_ = false;
    services_->CommitTextDisplayState();
}

void TextDisplaySceneRuntime::AdvanceScene()
{
    services_->WriteScriptSlot2DwordAt04(savedScriptSlot2DwordAt04_);
    services_->AdvanceActiveSceneFromPending();
}

void TextDisplaySceneRuntime::Draw()
{
    if (!drawEnabled_)
        return;

    if (!services_->ReadResultLayer5DrawPredicate())
        services_->DrawGlobalDisplayLayers();

    services_->PrepareResultLayer0TextRectCollection();
    SetRectEmpty(&dirtyTextRect_);
    for (const RECT& rect : services_->ReadResultLayer0TextRects())
        UnionRect(&dirtyTextRect_, &dirtyTextRect_, &rect);
    services_->CompleteResultLayer0TextRectCollection();

    if (drawTextOverlay_ && !IsRectEmpty(&dirtyTextRect_))
        services_->DrawTextSurface(textSurface_, dirtyTextRect_);
}
}
