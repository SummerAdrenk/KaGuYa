#include <Runtime/TextWindowNativePlayback.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Dword(const std::uint8_t* bytes) noexcept
{
    std::uint32_t value;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}
SaveDataEnvironmentArchive* CaptureSave(EnvironmentRootOwner& environment)
{
    return static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 1));
}
}

TextWindowNativePlayback::TextWindowNativePlayback(TextWindowLayerNativeBodyE8h& body,
    EnvironmentRootOwner& environment, IGraphicsTextApi& text, TextWindowNativeText& layout,
    ITextWindowNativePlaybackServices& services) noexcept
    : body_(body), environment_(environment), text_(text), layout_(layout), services_(services) {}

std::int32_t TextWindowNativePlayback::FinishAt4BB697()
{
    body_.glyphCursorAt50 = -1;
    const auto skip = services_.ReadSkipAt4AA540(body_);
    body_.skipAt60 = static_cast<std::uint8_t>(skip);
    if (skip) services_.ClearRootUpdateEnabledAt1B0After413BE0();
    // 4BB6C2长度先于4BB6DA的独立Save捕获；signed CMOVL后DWORD乘法回绕。
    const auto length = text_.GetTextLength();
    const auto factor = static_cast<std::uint32_t>(length < 30 ? 30 : length);
    const auto speed = Dword(CaptureSave(environment_)->bytesAt34.data() + 8);
    body_.autoWaitAt5C = std::bit_cast<std::int32_t>(factor * speed);
    return 0;
}

std::int32_t TextWindowNativePlayback::TickAt4BB530(std::uint32_t suppressAdvanceInput)
{
    try
    {
        if (body_.playbackStartedAt54 == 0)
        {
            if (!services_.PreparePlaybackAt4C2A40(body_)) throw 1;
            body_.playbackStartedAt54 = 1;
            body_.waitAt58 = 0;
        }
        if (body_.layer.enabledAt0C == 0 || body_.glyphCursorAt50 < 0)
            return FinishAt4BB697();

        auto step = text_.GetTextLength();
        bool paced = body_.layer.enabledAt0C != 0;
        if (paced && suppressAdvanceInput == 0)
        {
            auto* const click = static_cast<ClickInfoArchive*>(
                ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 5));
            paced = !services_.ReadAdvanceInputAt4B0550(*click);
        }
        if (paced)
        {
            paced = services_.IsPacedMouseAt4BB5D1();
        }
        if (paced) paced = !services_.ReadSkipAt4AA540(body_);
        if (paced)
        {
            const auto wait = body_.waitAt58;
            if (wait != 0)
            {
                body_.waitAt58 = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(wait) - 1u);
                return 1;
            }
            switch (Dword(CaptureSave(environment_)->bytesAt34.data()))
            {
            case 1: step = 2; break;
            case 2: step = 1; break;
            case 3: step = 1; body_.waitAt58 = 2; break;
            case 4: step = 1; body_.waitAt58 = 4; break;
            default: break;
            }
        }
        else body_.waitAt58 = 0;

        const auto begin = static_cast<std::uint32_t>(body_.glyphCursorAt50);
        const auto increment = static_cast<std::uint32_t>(step);
        if (layout_.BlitRangeAt419090(begin, begin + increment))
        {
            // 4BB67B是ADD当前字段；字形/dirty回调后的值不能被旧begin覆盖。
            body_.glyphCursorAt50 = std::bit_cast<std::int32_t>(
                static_cast<std::uint32_t>(body_.glyphCursorAt50) + increment);
            return 1;
        }
        return FinishAt4BB697();
    }
    catch (...)
    {
        // 4BB707..724仅C++catch返回-1，不补Clear/dirty或撤销状态前缀。
        return -1;
    }
}
}
