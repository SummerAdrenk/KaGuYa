#include <Runtime/TextWindowPlaybackRuntime.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Logging/Logger.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t ReadLe32(const std::uint8_t* bytes) noexcept
{
    return std::uint32_t{bytes[0]} | (std::uint32_t{bytes[1]} << 8) |
        (std::uint32_t{bytes[2]} << 16) | (std::uint32_t{bytes[3]} << 24);
}

std::int32_t Signed32(std::uint32_t bits) noexcept
{
    return std::bit_cast<std::int32_t>(bits);
}
}

TextWindowPlaybackRuntime::TextWindowPlaybackRuntime(TextWindowLayerRuntime& window,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs,
    ITextWindowTextServices& layout, const SaveDataEnvironmentArchive& save,
    const UnSaveDataRuntime& unsave, const ClickInfoArchive& click,
    Platform::MouseView mouse, Platform::KeyboardView keyboard,
    ITextWindowPlaybackServices& services) noexcept
    : window_(window), text_(text), glyphs_(glyphs), layout_(layout), save_(save),
      unsave_(unsave), click_(click), mouse_(mouse), input_(mouse, keyboard), services_(services) {}

bool TextWindowPlaybackRuntime::ReadAdvanceInputAt4B0550() const
{
    switch (ReadLe32(click_.bytesAt04.data() + 4))
    {
    case 1: if (input_.ReadPressedAtVslot04(5)) return true; break;
    case 2: if (input_.ReadPressedAtVslot04(0)) return true; break;
    case 3: if (input_.ReadPressedAtVslot04(8)) return true; break;
    default: break;
    }
    // 4B0630 读取原始按下锁存 byte，不经过 GUI 的边沿/倒计时过滤。
    switch (ReadLe32(click_.bytesAt04.data()))
    {
    case 1: return mouse_.DownLatched(1) != 0;
    case 2: return mouse_.DownLatched(0) != 0;
    case 3: return mouse_.DownLatched(2) != 0;
    default: return false;
    }
}

bool TextWindowPlaybackRuntime::ReadSkipAt4AA540() const
{
    if (input_.ReadHeldAtVslot00(6)) return true;
    if ((unsave_.FlagsAt14() & 3u) != 3u) return false;
    if ((save_.bytesAt34[4] & 1u) == 0) return true;
    return window_.TextState().unreadAt9A.value() == 0;
}

std::int32_t TextWindowPlaybackRuntime::FinishAt4BB697()
{
    auto& state = window_.TextState();
    state.glyphCursorAt50 = -1;
    const auto skip = static_cast<std::uint8_t>(ReadSkipAt4AA540());
    state.skipAt60 = skip;
    if (skip) services_.ClearRootUpdateEnabledAt1B0After413BE0();
    const auto length = text_.GetTextLength();
    const auto factor = static_cast<std::uint32_t>(length < 30 ? 30 : length);
    state.autoWaitFramesAt5C = Signed32(factor * ReadLe32(save_.bytesAt34.data() + 8));
    return 0;
}

std::int32_t TextWindowPlaybackRuntime::TickAt4BB530(bool suppressAdvanceInput)
{
    try
    {
        auto& state = window_.TextState();
        if (state.playbackStartedAt54.value() == 0)
        {
            if (!services_.PreparePlaybackAt4C2A40()) throw 1;
            state.playbackStartedAt54 = std::uint8_t{1};
            state.waitFramesAt58 = 0;
        }
        if (!window_.Enabled() || state.glyphCursorAt50.value() < 0)
            return FinishAt4BB697();

        auto step = text_.GetTextLength();
        if (window_.Enabled() &&
            (suppressAdvanceInput || !ReadAdvanceInputAt4B0550()) &&
            (mouse_.SecondaryCountdown() != 0 || mouse_.WheelSteps() >= 0) &&
            !ReadSkipAt4AA540())
        {
            const auto wait = state.waitFramesAt58.value();
            if (wait != 0)
            {
                state.waitFramesAt58 = Signed32(static_cast<std::uint32_t>(wait) - 1u);
                return 1;
            }
            switch (ReadLe32(save_.bytesAt34.data()))
            {
            case 1: step = 2; break;
            case 2: step = 1; break;
            case 3: step = 1; state.waitFramesAt58 = 2; break;
            case 4: step = 1; state.waitFramesAt58 = 4; break;
            default: break;
            }
        }
        else
        {
            state.waitFramesAt58 = 0;
        }

        const auto begin = static_cast<std::uint32_t>(state.glyphCursorAt50.value());
        const auto increment = static_cast<std::uint32_t>(step);
        if (window_.BlitRangeAt419090(begin, begin + increment, text_, glyphs_, layout_))
        {
            // ADD [this+50],edi 在外部字形调用后重新读字段，不能赋回旧 begin+step。
            state.glyphCursorAt50 = Signed32(
                static_cast<std::uint32_t>(state.glyphCursorAt50.value()) + increment);
            return 1;
        }
        return FinishAt4BB697();
    }
    catch (...)
    {
        // 5EE3F8 -> 4BB707：catch-all，不回滚字段或外部副作用。
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "CTextWin 4BB530 tick failed; partial state retained");
        return -1;
    }
}
}
