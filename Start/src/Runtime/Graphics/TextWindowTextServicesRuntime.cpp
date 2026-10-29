#include <Runtime/TextWindowTextServicesRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/MessageWindowLayerRuntime.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

#include <array>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint8_t RequireByte(const std::optional<std::uint8_t>& value, const char* message)
{
    if (!value) throw std::logic_error(message);
    return *value;
}
}

TextWindowTextServicesRuntime::TextWindowTextServicesRuntime(const TextWindowLayerRuntime& owner,
    IMessageWindowLayerState& selection, const DrawManagerLayerRuntime& layers,
    const SaveDataEnvironmentArchive& save, IGraphicsTextApi& graphics,
    const std::string& fontFaceCp932, std::span<const TextWindowTextDescriptor, 2> descriptors) noexcept
    : owner_(owner), selection_(selection), layers_(layers), save_(save), graphics_(graphics),
      fontFace_(fontFaceCp932), descriptors_(descriptors) {}

const TextWindowTextDescriptor* TextWindowTextServicesRuntime::ReadDescriptor()
{
    const auto index = static_cast<std::uint32_t>(selection_.EvaluateMessWinTypeAt4C73A0());
    return index < descriptors_.size() ? &descriptors_[index] : nullptr;
}

TextWindowTextServicesRuntime TextWindowTextServicesRuntime::ForWindow(const TextWindowLayerRuntime& owner) const noexcept
{
    return {owner, selection_, layers_, save_, graphics_, fontFace_, descriptors_};
}

bool TextWindowTextServicesRuntime::PrepareFont(const TextWindowTextFontDescriptor& record,
    std::uint32_t color, const std::uint32_t* shadowOverride)
{
    GraphicsTextFontParameters font;
    // 40C6E0 的 LOGFONTA/效果默认值，再按 414BD0 的字段写序覆盖。
    font.font.lfHeight = std::bit_cast<std::int32_t>(0u - static_cast<std::uint32_t>(record.size));
    font.font.lfWeight = record.weight;
    font.font.lfCharSet = 128;
    font.font.lfQuality = 2;
    font.font.lfPitchAndFamily = 0x30;
    ::lstrcpynA(font.font.lfFaceName, fontFace_.c_str(), LF_FACESIZE);
    const auto& state = owner_.TextState();
    font.renderMode = static_cast<std::uint8_t>(
        (RequireByte(state.renderAt99, "CTextWin +99 render mode is uninitialized") != 0) + 1);
    font.textColor = color;
    font.shadowColor = 0xFFFFFFFFu;
    font.outlineColor = 0xFFFFFFFFu;
    if (RequireByte(state.shadowAt98, "CTextWin +98 shadow mode is uninitialized") != 0)
    {
        if (shadowOverride) font.shadowColor = *shadowOverride;
        else
        {
            const auto* window = dynamic_cast<const MessageWindowLayerRuntime*>(layers_.LayerAt418C10(1));
            if (!window) throw std::logic_error("CTextWin shadow requires CMesWin in manager slot 1");
            font.shadowColor = window->ReadSelectedStyleColorsAt414BD0().color ^ 0x00FFFFFFu;
        }
        font.shadowX = 1;
        font.shadowY = 2;
    }
    font.lineSpacing = static_cast<std::uint8_t>(record.lineSpacing);
    font.characterSpacing = static_cast<std::uint8_t>(record.characterSpacing);
    return graphics_.CreateTextFont(font);
}

std::uint32_t TextWindowTextServicesRuntime::ReadBodyColor()
{
    const auto unread = RequireByte(owner_.TextState().unreadAt9A, "CTextWin +9A read state is uninitialized");
    const auto index = unread != 0 || save_.valueAt6C == 0 ? 0u : 1u;
    if (const auto* color = owner_.ColorOverride(index)) return *color;
    const std::size_t offset = index == 0 ? 4 : 8;
    return static_cast<std::uint32_t>(save_.bytesAt60[offset]) |
        (static_cast<std::uint32_t>(save_.bytesAt60[offset + 1]) << 8) |
        (static_cast<std::uint32_t>(save_.bytesAt60[offset + 2]) << 16) |
        (static_cast<std::uint32_t>(save_.bytesAt60[offset + 3]) << 24);
}

const std::uint32_t* TextWindowTextServicesRuntime::ReadNameColorOverride() { return owner_.ColorOverride(2); }
const std::uint32_t* TextWindowTextServicesRuntime::ReadNameShadowOverride() { return owner_.ColorOverride(3); }
}
