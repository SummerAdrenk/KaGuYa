#pragma once

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/TextWindowTextRuntime.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <array>
#include <memory>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class ITextWindowConstructionServices;
class IMessageWindowLayerState;

// CTextWin 45DB30/5B2C90 的现代 owner，不是原 0xE8 对象的二进制布局。
// construction 仅在构造期间借用；graphics/state 必须比图层存活更久。
class TextWindowLayerRuntime final : public FrameLayerItem, public ILayerPrepareTarget, public ILayerResetTarget
{
public:
    [[nodiscard]] std::uint8_t SetRequestedEnabledAtVslot10(
        std::uint8_t requested, std::int32_t secondArgument) override
        { return SetRequestedEnabledAt487310(requested, secondArgument); }
    TextWindowLayerRuntime(LayerList* parent, ITextWindowConstructionServices& construction,
        IGraphicsWindowSurfaceApi& graphics, const IMessageWindowLayerState& state);
    ~TextWindowLayerRuntime() override;

    [[nodiscard]] RECT BoundsAt479DB0() const noexcept;
    [[nodiscard]] RECT ContentRectAt30() const noexcept;
    [[nodiscard]] RECT NameRectAt40() const noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface* CanvasAt24() const noexcept;
    [[nodiscard]] const std::uint32_t* ColorOverride(std::uint32_t index) const;
    [[nodiscard]] std::int32_t GlyphCursorAt50() const;
    [[nodiscard]] bool HasNameAt70() const noexcept;
    // 原非虚生产方法共同读写此对象的字段；不给消息框另外复制一份状态。
    [[nodiscard]] TextWindowTextState& TextState() noexcept;
    [[nodiscard]] const TextWindowTextState& TextState() const noexcept;
    [[nodiscard]] bool PrepareTextAt415150(IGraphicsTextApi& text,
        IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services);
    [[nodiscard]] bool BlitRangeAt419090(std::uint32_t begin, std::uint32_t end,
        IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services);

    [[nodiscard]] std::uint8_t SetRequestedEnabledAt487310(
        std::uint8_t requested, std::int32_t unusedSecondArgument);
    // 原 Clear 的 AL 在空 handle 路径是对象地址低字节，不作为现代业务返回值。
    void ClearAt46DC10();
    // 4C291B..4C29A8：清旧内容区域并提交旧 dirty，成功后才清两 RECT。
    void ClearPreviousFormatRegionAt4C291B();
    void ResetAt47EE80();
    // 46D170保留enabled/format/playback等状态，仅清旧区、布局、引用。
    void ResetTextAt46D170();
    [[nodiscard]] YtlReferenceArrayRuntime& ReferencesAt64() noexcept { return referencesAt64_; }
    [[nodiscard]] const YtlReferenceArrayRuntime& ReferencesAt64() const noexcept { return referencesAt64_; }
    void ResetOwnedLayerAtVslot18(IGraphicsWindowSurfaceApi&) override { ResetAt47EE80(); }
    void Draw(const RECT& dirty) override;
    void CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* clip) override;
    void AdvanceOwnedFrame() override;

private:
    [[nodiscard]] RuntimeGraphicsSurface& RequireCanvas() const;
    [[nodiscard]] bool HasContentRect() const noexcept;
    void ReleaseOwnedResources() noexcept;

    RECT bounds_{};
    std::unique_ptr<RuntimeGraphicsSurface> canvas_;
    std::array<std::shared_ptr<const std::uint32_t>, 4> colors_;
    TextWindowTextState textState_;
    YtlReferenceArrayRuntime referencesAt64_;
    IGraphicsWindowSurfaceApi& graphics_;
    const IMessageWindowLayerState& state_;
};
}
