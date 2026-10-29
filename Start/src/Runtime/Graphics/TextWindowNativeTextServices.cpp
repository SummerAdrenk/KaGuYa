#include <Runtime/TextWindowNativeTextServices.h>

#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/GameBuildProfile.h>

#include <array>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr TextWindowTextDescriptor Descriptor{
    {26, 700, 0, 18}, 260, 562, {28, 700, 0, 0}, 128, 562, 1, 1, 1, 10};
constexpr std::array<TextWindowTextDescriptor, 2> Descriptors{Descriptor, Descriptor};
static_assert(sizeof(TextWindowTextDescriptor) == 0x40);
static_assert(offsetof(TextWindowTextDescriptor, nameFont) == 0x18);
static_assert(offsetof(TextWindowTextDescriptor, expandName) == 0x38);
static_assert(offsetof(TextWindowTextDescriptor, minimumHalfCharacters) == 0x3C);

const std::uint32_t* ResolveColor(const YtlReferenceSlotRuntime& input) noexcept
{
    const volatile auto& slot = input;
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = slot.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0u;
    const auto bias = slot.biasAt04;
    return reinterpret_cast<const std::uint32_t*>(base + bias * stride);
}
SaveDataEnvironmentArchive* CaptureSave(EnvironmentRootOwner& environment)
{
    return static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 1));
}
std::uint32_t ReadFallbackColor(EnvironmentRootOwner& environment, std::size_t offset)
{
    auto* const save = CaptureSave(environment);
    std::uint32_t color;
    std::memcpy(&color, save->bytesAt60.data() + offset, sizeof(color));
    return color;
}
}

std::span<const TextWindowTextDescriptor, 2> MamatomoTextWindowNativeDescriptors() noexcept
{
    return Descriptors;
}

std::span<const TextWindowTextDescriptor, 2> GameTextWindowNativeDescriptors() noexcept
{
    return CompiledGameProfile().textLayout();
}

TextWindowNativeTextServices::TextWindowNativeTextServices(TextWindowLayerNativeBodyE8h& body,
    IMessageWindowNativeStateSource& state, EnvironmentRootOwner& environment,
    GraphicsTextNativeApi& graphics, const char* const& fontFace, IScrVmCommandErrorSink& errors,
    std::span<const TextWindowTextDescriptor, 2> descriptors) noexcept
    : body_(body), state_(state), environment_(environment), graphics_(graphics),
      fontFace_(fontFace), errors_(errors), descriptors_(descriptors)
{
}

const TextWindowTextDescriptor* TextWindowNativeTextServices::ReadDescriptor()
{
    const auto* const slot0 = state_.CaptureSlot0At4180C0();
    const auto root = state_.CaptureAfterEnsureAt413BE0();
    const auto index = state_.ResolveAt498020(slot0, "mess_win_type");
    auto* const params = root.paramsAt47BDB0(root.object);
    std::int32_t value;
    if (index == -1)
        value = ReadScriptValueAt4C73A0(*params, index, errors_);
    else
    {
        // 4C73AB不做index/表长检查；只在-1时沿原MessageBox/throw路径。
        const auto* values = ResolveEnvironmentObjectReference<std::byte>(params->saveParams2At30);
        const auto address = reinterpret_cast<std::uintptr_t>(values) +
            static_cast<std::uint32_t>(index) * 4u + 0x0Cu;
        std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
    }
    const auto selected = static_cast<std::uint32_t>(value);
    return selected < 2u ? &descriptors_[selected] : nullptr;
}

bool TextWindowNativeTextServices::PrepareFont(const TextWindowTextFontDescriptor& record,
    std::uint32_t color, const std::uint32_t* shadowOverride)
{
    GraphicsTextNativeFontParameters font;
    InitializeGraphicsTextFontNativeAt40C6E0(font);
    font.fontAt00.lfHeight = std::bit_cast<std::int32_t>(0u - static_cast<std::uint32_t>(record.size));
    font.fontAt00.lfWeight = record.weight;
    ::lstrcpynA(font.fontAt00.lfFaceName, fontFace_, LF_FACESIZE);
    font.renderModeAt3E = static_cast<std::uint8_t>((body_.renderAt99 != 0) + 1);
    font.textColorAt40 = color;
    if (body_.shadowAt98 != 0)
    {
        if (shadowOverride)
            font.shadowColorAt44 = *shadowOverride;
        else
        {
            const auto root = state_.CaptureAfterEnsureAt413BE0();
            auto* const manager = root.drawManagerAt4175A0(root.object);
            auto* const window = reinterpret_cast<MessageWindowLayerNativeBody50h*>(
                DrawLayerAt418C10(*manager, 1));
            auto* const config = ResolveMessageWindowTextConfigNativeAt414C3C(*window);
            MessageWindowColors colors;
            const MessageWindowNativeConfigScope scope(environment_);
            // 414CF3消费栈上输出，不消费getter返回指针；异常不改为默认阴影。
            static_cast<void>(config->vptrAt00->readAt00(config, nullptr, &colors));
            font.shadowColorAt44 = colors.color ^ 0x00FFFFFFu;
        }
        font.shadowXAt4C = 1;
        font.shadowYAt50 = 2;
    }
    else
    {
        font.shadowColorAt44 = 0xFFFFFFFFu;
        font.shadowXAt4C = 0;
        font.shadowYAt50 = 0;
    }
    // 414D27/414D2D：shadow getter之后才读取原记录的低byte，不提前复制整条descriptor。
    font.lineSpacingAt3D = *reinterpret_cast<const volatile std::uint8_t*>(&record.lineSpacing);
    font.characterSpacingAt3C = *reinterpret_cast<const volatile std::uint8_t*>(&record.characterSpacing);
    font.verticalAt55 = 0;
    return graphics_.CreateTextFontNativeAt4388B0(&font);
}

std::uint32_t TextWindowNativeTextServices::ReadBodyColor()
{
    // 418980：高亮检查与缺override时的fallback各自Capture，允许期间根/Save发生变化。
    if (body_.unreadAt9A == 0 && CaptureSave(environment_)->valueAt6C != 0)
    {
        if (body_.readBodyColorAtA8.baseAt00)
            return *ResolveColor(body_.readBodyColorAtA8);
        return ReadFallbackColor(environment_, 8);
    }
    if (body_.normalBodyColorAt9C.baseAt00)
        return *ResolveColor(body_.normalBodyColorAt9C);
    return ReadFallbackColor(environment_, 4);
}

const std::uint32_t* TextWindowNativeTextServices::ReadNameColorOverride()
{
    return ResolveColor(body_.nameColorAtB4);
}

const std::uint32_t* TextWindowNativeTextServices::ReadNameShadowOverride()
{
    return ResolveColor(body_.nameShadowColorAtC0);
}
}
