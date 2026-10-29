#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/TextWindowConstructionRuntime.h>
#include <Runtime/MessageWindowLayerRuntime.h>
#include <Logging/Logger.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
class NullTextCanvas final : public std::logic_error
{
public:
    NullTextCanvas() : std::logic_error("CTextWin construction returned a null Surface object") {}
};
}

TextWindowLayerRuntime::TextWindowLayerRuntime(LayerList* parent,
    ITextWindowConstructionServices& construction, IGraphicsWindowSurfaceApi& graphics,
    const IMessageWindowLayerState& state)
    : FrameLayerItem(parent), graphics_(graphics), state_(state)
{
    // base 注册早于 try；四个颜色 lookup/分配各自捕获，失败不阻止下一项。
    try
    {
        for (std::uint32_t index = 0; index < colors_.size(); ++index)
        {
            try { colors_[index] = construction.ReadColorOverride(index); }
            catch (...) {}
        }
        SetEnabled(false);
        canvas_ = construction.AllocateSurface();
        if (!canvas_) throw NullTextCanvas();
        const auto height = construction.ReadProgramHeightAt1C();
        const auto width = construction.ReadProgramWidthAt18();
        // 45E0D8 不检查 AL。合法失败仍有 Surface 对象，只是 handle 可能为零。
        static_cast<void>(canvas_->Create(width, height, true, true));
        const auto boundsHeight = construction.ReadProgramHeightAt1C();
        const auto boundsWidth = construction.ReadProgramWidthAt18();
        bounds_ = {0, 0, boundsWidth, boundsHeight};
        textState_.contentRect = {};
        textState_.nameRect = {};
        textState_.glyphCursorAt50 = -1;
    }
    catch (const NullTextCanvas&)
    {
        ReleaseOwnedResources();
        throw;
    }
    catch (...)
    {
        // 45E172 只清 bounds，保留颜色、已提交的 Surface 和当前 enabled。
        bounds_ = {};
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "CTextWin 45E172: construction failed; retained owners/state, cleared bounds");
    }
}

TextWindowLayerRuntime::~TextWindowLayerRuntime() { ReleaseOwnedResources(); }

void TextWindowLayerRuntime::ReleaseOwnedResources() noexcept
{
    // 464510：颜色逆序，4100C0 按 body/voices/name 释放，vector存储后才是Surface。
    for (std::size_t index = colors_.size(); index > 0; --index) colors_[index - 1].reset();
    textState_.body.reset();
    textState_.voices.reset();
    textState_.name.reset();
    referencesAt64_.DestroyStorage();
    canvas_.reset();
}

RECT TextWindowLayerRuntime::BoundsAt479DB0() const noexcept { return bounds_; }
RECT TextWindowLayerRuntime::ContentRectAt30() const noexcept { return textState_.contentRect; }
RECT TextWindowLayerRuntime::NameRectAt40() const noexcept { return textState_.nameRect; }
const RuntimeGraphicsSurface* TextWindowLayerRuntime::CanvasAt24() const noexcept { return canvas_.get(); }
const std::uint32_t* TextWindowLayerRuntime::ColorOverride(std::uint32_t index) const
    { return colors_.at(index).get(); }
std::int32_t TextWindowLayerRuntime::GlyphCursorAt50() const
{
    if (!textState_.glyphCursorAt50) throw std::logic_error("CTextWin +50 is uninitialized after construction failure");
    return *textState_.glyphCursorAt50;
}

bool TextWindowLayerRuntime::HasNameAt70() const noexcept { return textState_.name != nullptr; }
TextWindowTextState& TextWindowLayerRuntime::TextState() noexcept { return textState_; }
const TextWindowTextState& TextWindowLayerRuntime::TextState() const noexcept { return textState_; }

bool TextWindowLayerRuntime::PrepareTextAt415150(IGraphicsTextApi& text,
    IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services)
{
    TextWindowTextRuntime layout(textState_, canvas_.get(), RegisteredLayerList(), text, glyphs, services);
    return layout.PrepareTextAt415150();
}

bool TextWindowLayerRuntime::BlitRangeAt419090(std::uint32_t begin, std::uint32_t end,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services)
{
    TextWindowTextRuntime layout(textState_, canvas_.get(), RegisteredLayerList(), text, glyphs, services);
    return layout.BlitRangeAt419090(begin, end);
}

bool TextWindowLayerRuntime::HasContentRect() const noexcept
{
    // 原检查四个 DWORD，不是 IsRectEmpty；退化/反向的非零 RECT 也会进入绘制。
    return textState_.contentRect.left || textState_.contentRect.right ||
        textState_.contentRect.top || textState_.contentRect.bottom;
}

RuntimeGraphicsSurface& TextWindowLayerRuntime::RequireCanvas() const
{
    if (!canvas_) throw std::logic_error("CTextWin +24 Surface object is null");
    return *canvas_;
}

std::uint8_t TextWindowLayerRuntime::SetRequestedEnabledAt487310(
    std::uint8_t requested, std::int32_t)
{
    if (state_.ActiveScriptField60() == -1 || !state_.MessageWindowAllowedAt419290()) requested = 0;
    if ((requested != 0) == Enabled()) return 0;
    SetEnabled(requested != 0);
    if (HasContentRect())
    {
        auto* parent = RegisteredLayerList();
        if (!parent) throw std::logic_error("CTextWin invalidation requires its parent LayerList");
        parent->DirtyRects().Add(textState_.contentRect);
    }
    // 4873AF 无论是否改标志都返回 0，不能复用 CMesWin 的返回值规则。
    return 0;
}

void TextWindowLayerRuntime::ClearAt46DC10()
{
    textState_.contentRect = {};
    textState_.nameRect = {};
    auto& surface = RequireCanvas();
    if (surface.Handle()) graphics_.SurfaceClearRaw(surface.Handle(), surface.ClearColor(), nullptr);
}

void TextWindowLayerRuntime::ResetAt47EE80()
{
    ClearAt46DC10();
    SetEnabled(false);
}

void TextWindowLayerRuntime::ResetTextAt46D170()
{
    if (HasContentRect())
    {
        auto& surface = RequireCanvas();
        if (surface.Handle())
            graphics_.SurfaceClearRaw(surface.Handle(), surface.ClearColor(), &textState_.contentRect);
        auto* parent = RegisteredLayerList();
        if (!parent) throw std::logic_error("CTextWin reset requires its parent LayerList");
        parent->DirtyRects().Add(textState_.contentRect);
    }
    // 46D1F3..46D252：保留原字段写入顺序，不改enabled/format或播放计时。
    auto& content = textState_.contentRect;
    content.bottom = 0; content.top = 0; content.right = 0; content.left = 0;
    auto& name = textState_.nameRect;
    name.bottom = 0; name.top = 0; name.right = 0; name.left = 0;
    textState_.nameOffsetY = 0; textState_.nameOffsetX = 0;
    textState_.bodyOffsetY = 0; textState_.bodyOffsetX = 0;
    textState_.glyphCursorAt50 = -1;
    referencesAt64_.ClearPreservingStorage();
    // 此处与4100C0析构次序不同；原46D274后为name/body/voices。
    textState_.name.reset();
    textState_.body.reset();
    textState_.voices.reset();
}

void TextWindowLayerRuntime::ClearPreviousFormatRegionAt4C291B()
{
    if (HasContentRect())
    {
        auto& surface = RequireCanvas();
        if (surface.Handle())
            graphics_.SurfaceClearRaw(surface.Handle(), surface.ClearColor(), &textState_.contentRect);
        auto* parent = RegisteredLayerList();
        if (!parent) throw std::logic_error("CTextWin old format invalidation requires parent LayerList");
        parent->DirtyRects().Add(textState_.contentRect);
    }
    textState_.contentRect = {};
    textState_.nameRect = {};
}

void TextWindowLayerRuntime::Draw(const RECT& dirty)
{
    if (!HasContentRect()) return;
    const auto handle = RequireCanvas().Handle();
    if (handle) static_cast<void>(graphics_.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, &dirty, 0));
}

void TextWindowLayerRuntime::CompositeToSurface(GraphicsSurfaceDestination destination, const RECT* clip)
{
    if (!HasContentRect()) return;
    const auto targetHandle = destination.Handle();
    if (!targetHandle) return;
    const auto handle = RequireCanvas().Handle();
    if (handle) static_cast<void>(graphics_.SurfaceBlitRaw(targetHandle, handle, 0, 0, nullptr, nullptr, clip, 0));
}

void TextWindowLayerRuntime::AdvanceOwnedFrame()
{
    // 46B230 是 RETN；文字推进由非虚的消息播放链触发。
}
}
