#include <Runtime/SelectWindowLayerRuntime.h>
#include <Runtime/SelectWindowDialogRuntime.h>
#include <Logging/Logger.h>

#include <bit>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
LONG CenterCoordinate(LONG begin, LONG end, LONG textBegin, LONG textEnd)
{
    // 437135..437170：先执行32位环绕的 SUB/ADD，再 SAR；负奇数不可用 /2。
    const auto delta = std::uint32_t(end) + std::uint32_t(textBegin) -
        std::uint32_t(textEnd) - std::uint32_t(begin);
    const auto half = (delta >> 1u) | (delta & 0x80000000u);
    return std::bit_cast<std::int32_t>(std::uint32_t(begin) + half);
}
}

SelectWindowLayerRuntime::SelectWindowLayerRuntime(LayerList* parent,
    ISelectWindowConstructionServices& construction, IGraphicsWindowSurfaceApi& graphics)
    : FrameLayerItem(parent), graphics_(graphics)
{
    try
    {
        for (std::uint32_t index = 0; index < colors_.size(); ++index)
        {
            try { colors_[index] = construction.ReadColorOverride(index); }
            catch (...) {} // 45CDC7/45CEA4/45CF81：三项独立的原局部 catch。
        }
        SetEnabled(false);
        dialog_ = construction.CreateDialogAt4F17E0();
        canvas_ = construction.AllocateSurface();
        const auto height = construction.ReadProgramHeightAt1C();
        const auto width = construction.ReadProgramWidthAt18();
        static_cast<void>(RequireCanvas().Create(width, height, true, true));
        const auto boundsHeight = construction.ReadProgramHeightAt1C();
        const auto boundsWidth = construction.ReadProgramWidthAt18();
        bounds_ = {0, 0, boundsWidth, boundsHeight};
    }
    catch (...)
    {
        // 45D0EE 只清 bounds，已发布的 child/Surface/颜色仍由本对象持有。
        bounds_ = {};
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "SelectWin 45D0EE: construction failed; retained partial owners, cleared bounds");
    }
}
SelectWindowLayerRuntime::~SelectWindowLayerRuntime() { ReleaseOwnedResources(); }
void SelectWindowLayerRuntime::ReleaseOwnedResources() noexcept
{
    for (std::size_t index = colors_.size(); index != 0; --index) colors_[index - 1].reset();
    dialog_.reset();
    canvas_.reset();
}
RuntimeGraphicsSurface& SelectWindowLayerRuntime::RequireCanvas() const
{
    if (!canvas_) throw std::logic_error("SelectWin +2C Surface object is null");
    return *canvas_;
}
ISelectWindowDialog& SelectWindowLayerRuntime::RequireDialog() const
{
    if (!dialog_) throw std::logic_error("SelectWin +38 Dialog object is null");
    return *dialog_;
}
LayerList& SelectWindowLayerRuntime::RequireParent() const
{
    if (!RegisteredLayerList()) throw std::logic_error("SelectWin requires its parent LayerList");
    return *RegisteredLayerList();
}
std::uint8_t SelectWindowLayerRuntime::SetRequestedEnabledAtVslot10(std::uint8_t value, std::int32_t)
{
    if ((value != 0) == Enabled()) return 0;
    SetEnabled(value != 0);
    RequireParent().DirtyRects().Add(bounds_);
    return 1;
}
void SelectWindowLayerRuntime::ResetAt47EE70() noexcept { SetEnabled(false); }
void SelectWindowLayerRuntime::AdvanceOwnedFrame() {} // 46B230: RET。
void SelectWindowLayerRuntime::Draw(const RECT& dirty)
{
    RequireDialog().DrawAtVslot60(dirty);
    const auto handle = RequireCanvas().Handle();
    if (handle) static_cast<void>(graphics_.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, &dirty, 0));
}
void SelectWindowLayerRuntime::CompositeToSurface(GraphicsSurfaceDestination target, const RECT* clip)
{
    if (!target.Body())
        throw std::invalid_argument("SelectWin composite requires a native destination body");
    // 481C48..481C7F：先捕获 Dialog，不增持；451500 owning=0 仍分配18h控制块。
    auto& dialog = RequireDialog();
    YtlReferenceSlotRuntime argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument,
        const_cast<GraphicsSurfaceNativeBody*>(target.Body()), 0);
    dialog.CompositeAtVslot5C(argument, clip);
    // 原 +5C 返回后才解析本窗文字画布，4425C0 先判断目标再读取源句柄。
    auto& canvas = RequireCanvas();
    const auto targetHandle = target.Handle();
    if (!targetHandle) return;
    const auto handle = canvas.Handle();
    if (handle) static_cast<void>(graphics_.SurfaceBlitRaw(targetHandle, handle, 0, 0,
        nullptr, nullptr, clip, 0));
}
std::uint32_t SelectWindowLayerRuntime::NormalColor(ISelectWindowStateServices& services) const
{
    return colors_[0] ? *colors_[0] : services.ReadSaveColorAt64();
}
void SelectWindowLayerRuntime::RebuildAt4369E0(ISelectWindowStateServices& services,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs)
{
    try
    {
        RequireDialog().ResetAt50F000();
        auto& canvas = RequireCanvas();
        if (canvas.Handle()) graphics_.SurfaceClearRaw(canvas.Handle(), canvas.ClearColor(), nullptr);
        GraphicsTextFontParameters font;
        font.font.lfHeight = -24;
        font.font.lfWeight = 700;
        font.font.lfCharSet = 128;
        font.font.lfQuality = 2;
        font.font.lfPitchAndFamily = 0x30;
        const auto face = services.ReadFontFaceAt619824();
        ::lstrcpynA(font.font.lfFaceName, face.c_str(), LF_FACESIZE);
        font.renderMode = 2;
        font.textColor = NormalColor(services);
        font.shadowColor = font.outlineColor = 0xFFFFFFFFu;
        if (colors_[2])
        {
            // 436B0E 的 ebp-48 相对 font(ebp-8C) 是 +44，即阴影色。
            font.shadowColor = *colors_[2];
            font.shadowX = 1;
            font.shadowY = 2;
        }
        if (services.ReadMessageVersion() < 4.0f) static_cast<void>(text.CreateTextFont(font));
        MessageTextCommandReference legacyName;
        // 593B30 -> 4100C0 异常释放 body/voices/name；声明顺序不能反过来。
        MessageTextSelection choice;
        const auto count = services.MenuCountAt64();
        if (count > std::size_t(std::numeric_limits<std::int32_t>::max()))
            throw std::length_error("SelectWin menu length exceeds original signed DWORD range");
        for (std::int32_t ordinal = 0; ordinal < static_cast<std::int32_t>(count); ++ordinal)
        {
            const auto id = services.MenuChoiceAt64(static_cast<std::size_t>(ordinal));
            if (id == -1) continue;
            // 原三引用的清理顺序是 name/body/voices，不是声明的析构逆序。
            legacyName.reset();
            choice.storageAt00.reset();
            choice.textAt18.reset();
            choice.voicesAt0C.reset();
            MessageTextCommandReference command;
            // 436D95 COMISS(version,4)+JB：unordered也走旧reader；前面的字体比较方向不同。
            if (!(services.ReadMessageVersion() >= 4.0f))
            {
                if (services.ReadLegacyAt417C40(id, {legacyName, choice.voicesAt0C, choice.textAt18}) < 0 ||
                    !legacyName) throw 1;
                command = legacyName;
            }
            else
            {
                auto selected = services.ReadChoiceAt418700(id, choice);
                if (selected < 0) throw 1;
                if (services.HasProgramChoiceFlagsAt28())
                    selected = ordinal < 4 && services.ReadProgramChoiceFlag(ordinal) ? 1 : 0;
                font.textColor = selected != 0 && services.HighlightChosenAt27() && colors_[1]
                    ? *colors_[1] : NormalColor(services);
                static_cast<void>(text.CreateTextFont(font));
                command = choice.textAt18;
            }
            if (!command) throw std::logic_error("SelectWin resolved a null text command");
            const auto commandText = command->GetTextAt47D550();
            if (!text.InitializeText(commandText.GetString(), 0, 0)) throw 1;
            const auto textRect = text.ReadTextDrawRect();
            const auto rect = RequireDialog().VisibleBoundsAt50AD40(ordinal);
            text.SetTextBasePosition(CenterCoordinate(rect.left, rect.right, textRect.left, textRect.right),
                CenterCoordinate(rect.top, rect.bottom, textRect.top, textRect.bottom));
            // 原每一轮都重新问长度，字形 false 被忽略，异常交本函数原 catch。
            for (std::int32_t index = 0; index < text.GetTextLength(); ++index)
                static_cast<void>(glyphs.BlitTextGlyphToSurface(RequireCanvas().Handle(),
                    static_cast<std::uint32_t>(index), nullptr));
        }
        // 437477..437501 在 parent dirty 前释放所有临时文字引用。
        choice.textAt18.reset();
        choice.voicesAt0C.reset();
        legacyName.reset();
        choice.storageAt00.reset();
        RequireParent().DirtyRects().Add(bounds_);
        savedByteAt44_ = services.ReadSaveByteAt60();
        savedByteAt45_ = services.ReadSaveByteAt61();
    }
    catch (...)
    {
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "SelectWin 4375BA: rebuild failed; retained partial child/text/dirty state");
    }
}
std::int32_t SelectWindowLayerRuntime::TickAt4BBFD0(std::uint8_t blocked,
    ISelectWindowStateServices& services)
{
    std::int32_t result = -1;
    if (blocked == 0)
    {
        if (services.MouseSecondaryCountdownAt2C() == 0 && services.MouseWheelAt34() > 0)
            services.SetUnSaveFlagAt1C();
        else
        {
            if (RequireDialog().ProcessAt530270() == 0)
            {
                result = RequireDialog().ResultAt50ABD0();
                if (result < 0) throw std::out_of_range("SelectWin negative ordinal cannot index menu");
                services.MarkChoiceAt4C29D0(services.MenuChoiceAt64(static_cast<std::size_t>(result)));
            }
            RequireDialog().OwnDirtyRectsAtVslot00().PrepareAtVslot04();
            const auto count = RequireDialog().OwnDirtyRectsAtVslot00().CountAtVslot18();
            if (count != 0)
            {
                const auto rects = RequireDialog().OwnDirtyRectsAtVslot00().RectsAtVslot1C();
                if (count > rects.size()) throw std::logic_error("SelectWin dirty count/data mismatch");
                for (std::size_t index = 0; index < count; ++index)
                    RequireParent().DirtyRects().Add(rects[index]);
                RequireDialog().OwnDirtyRectsAtVslot00().ClearAtVslot00();
            }
        }
    }
    services.RefreshEffectsAt433F60();
    return result;
}
}
