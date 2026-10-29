#include <Runtime/MessageWindowConfigRuntime.h>

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <array>
#include <bit>
#include <cstddef>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 CP932 字节：テキスト、テキスト２、背景色、透過率；文件编码不参与查找。
constexpr char Type1Section[] = "\x83\x65\x83\x4C\x83\x58\x83\x67";
constexpr char Type2Section[] = "\x83\x65\x83\x4C\x83\x58\x83\x67\x82\x51";
constexpr char ColorKey[] = "\x94\x77\x8C\x69\x90\x46";
constexpr char TransparencyKey[] = "\x93\xA7\x89\xDF\x97\xA6";

class Cp932Locale final
{
public:
    Cp932Locale() : locale_(_create_locale(LC_CTYPE, ".932"))
    {
        if (!locale_) throw std::runtime_error("CMesWin CP932 locale unavailable");
    }
    ~Cp932Locale() { _free_locale(locale_); }
    Cp932Locale(const Cp932Locale&) = delete;
    Cp932Locale& operator=(const Cp932Locale&) = delete;
    [[nodiscard]] _locale_t Get() const noexcept { return locale_; }
private:
    _locale_t locale_;
};

const unsigned char* Bytes(const char* text) noexcept
{
    return reinterpret_cast<const unsigned char*>(text);
}

std::int32_t EvaluateInteger(
    const std::string& text, IMessageWindowConfigExpressionParser& parser)
{
    // 45E770 按字节删除全部 ASCII 空格和 Tab，而非仅 trim 首尾。
    std::string normalized;
    normalized.reserve(text.size());
    for (const char value : text)
        if (value != ' ' && value != '\t') normalized.push_back(value);
    return parser.Evaluate(normalized);
}

void TrimRgbWrapper(std::string& text, _locale_t locale)
{
    const auto* begin = Bytes(text.c_str());
    const auto* cursor = begin;
    while (*cursor && _mbschr_l(Bytes("\t ("), *cursor, locale))
        cursor = _mbsinc_l(cursor, locale);
    text.erase(0, static_cast<std::size_t>(cursor - begin));

    // 479885..4798FA：剥离连续末尾 Tab/空格/')'，不是配平括号。
    begin = Bytes(text.c_str());
    cursor = begin;
    const unsigned char* trailing = nullptr;
    while (static_cast<std::size_t>(cursor - begin) < text.size() && *cursor)
    {
        if (_mbschr_l(Bytes("\t )"), *cursor, locale))
        {
            if (!trailing) trailing = cursor;
        }
        else trailing = nullptr;
        cursor = _mbsinc_l(cursor, locale);
    }
    if (trailing) text.erase(static_cast<std::size_t>(trailing - begin));
}

std::uint32_t ParseRgb(
    const std::string& value, IMessageWindowConfigExpressionParser& parser,
    _locale_t locale)
{
    const auto* begin = Bytes(value.c_str());
    const auto* marker = _mbsstr_l(begin, Bytes("RGB"), locale);
    if (!marker) throw std::runtime_error("CMesWin color has no RGB marker");
    std::string remaining = value.substr(static_cast<std::size_t>(marker - begin) + 3u);
    TrimRgbWrapper(remaining, locale);

    std::array<std::string, 3> components;
    for (std::size_t index = 0; index < 2; ++index)
    {
        begin = Bytes(remaining.c_str());
        const auto* comma = _mbschr_l(begin, ',', locale);
        if (!comma) throw std::runtime_error("CMesWin color component separator missing");
        const auto offset = static_cast<std::size_t>(comma - begin);
        components[index] = remaining.substr(0, offset);
        remaining.erase(0, offset + 1u);
    }
    components[2] = std::move(remaining);

    // 479A21..479B09：先完成三个表达式，再按低字节打包 COLORREF；不钳制分量。
    const auto red = static_cast<std::uint32_t>(EvaluateInteger(components[0], parser));
    const auto green = static_cast<std::uint32_t>(EvaluateInteger(components[1], parser));
    const auto blue = static_cast<std::uint32_t>(EvaluateInteger(components[2], parser));
    return (red & 0xFFu) | ((green & 0xFFu) << 8u) | ((blue & 0xFFu) << 16u);
}

RuntimeCStringResult ReadWindowValue(IMessageWindowParamsSource& source, MessageWindowParamsReadPoint point,
    const char* section, const char* key, ISettingTagErrorSource& errors)
{
    const auto& params = source.ReadGameSystem(point);
    auto* errorCString = errors.CaptureAt47BCF0(); // 459D4A / 459FD3
    const auto tag = CaptureSettingChildAt477E90(errorCString, params.settingTags[2].Native(), section);
    return CaptureSettingValueAt47D470(errorCString, tag.Native(), key);
}

std::uint32_t ReadColorDefinition(IMessageWindowParamsSource& source, MessageWindowParamsReadPoint point,
    const char* colorName, IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
{
    const auto& params = source.ReadGameSystem(point);
    auto* errorCString = errors.CaptureAt47BCF0(); // 459E0C / 45A092
    auto definition = CaptureSettingChildAt477E90(errorCString, params.settingTags[1].Native(), colorName);
    // 479BB0 持有叶 Ref 穿过求值；即使 parser 换根也不复制/保活整个 GameSystem。
    const auto value = ReadSettingNodeRgbColorAt4797C0(errorCString, definition.Native(), "Value", parser);
    definition.ReleaseCached();
    return value;
}

void ReadColorOverride(IMessageWindowParamsSource& source,
    MessageWindowParamsReadPoint namePoint, MessageWindowParamsReadPoint definitionPoint,
    const char* section, IMessageWindowConfigExpressionParser& parser,
    std::optional<std::uint32_t>& target, ISettingTagErrorSource& errors)
{
    // 459D8D..459DBD / 45A013..45A043：窗口子节点在再次取色库前已释放。
    const auto colorName = ReadWindowValue(source, namePoint, section, ColorKey, errors);
    if (colorName.Value().GetString()[0] == '\0') return;
    target = ReadColorDefinition(source, definitionPoint, colorName.Value().GetString(), parser, errors);
    // 459E91 先提交颜色，459E93 才释放名称；Free 失败不回滚已写目标。
}

std::int32_t ReadTransparency(IMessageWindowParamsSource& source, MessageWindowParamsReadPoint point,
    const char* section, IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
{
    const auto& params = source.ReadGameSystem(point);
    auto* errorCString = errors.CaptureAt47BCF0(); // 459EF5 / 45A174
    const auto tag = CaptureSettingChildAt477E90(errorCString, params.settingTags[2].Native(), section);
    return ReadSettingNodeIntegerAt47BBE0(errorCString, tag.Native(), TransparencyKey, parser);
}

class FixedMessageWindowParamsSource final : public IMessageWindowParamsSource
{
public:
    explicit FixedMessageWindowParamsSource(const ParamsGameSystem& params) noexcept : params_(params) {}
    const ParamsGameSystem& ReadGameSystem(MessageWindowParamsReadPoint) override { return params_; }
private:
    const ParamsGameSystem& params_;
};

std::int32_t ConvertTransparency(std::int32_t percentage) noexcept
{
    // SHL 的低 32 位先按有符号数解释，再做 /100；100 得到 256，负溢出仍保留。
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(percentage) << 8u) / 100;
}

std::uint32_t ReadDword(const std::array<std::uint8_t, 8>& bytes, std::size_t offset) noexcept
{
    return static_cast<std::uint32_t>(bytes[offset]) |
        (static_cast<std::uint32_t>(bytes[offset + 1u]) << 8u) |
        (static_cast<std::uint32_t>(bytes[offset + 2u]) << 16u) |
        (static_cast<std::uint32_t>(bytes[offset + 3u]) << 24u);
}
}

std::uint32_t ReadSettingRgbColorAt4797C0(
    RuntimeCString* errorCString, const ParamsGameSystem& params, const char* nameCp932,
    IMessageWindowConfigExpressionParser& parser)
{
    auto definition = CaptureSettingChildAt477E90(errorCString, params.settingTags[1].Native(), nameCp932);
    const auto value = ReadSettingNodeRgbColorAt4797C0(errorCString, definition.Native(), "Value", parser);
    definition.ReleaseCached();
    return value;
}

std::uint32_t ReadSettingNodeRgbColorAt4797C0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& node, const std::string_view keyCp932,
    IMessageWindowConfigExpressionParser& parser)
{
    if (keyCp932.find('\0') != std::string_view::npos)
        throw std::invalid_argument("Setting node key contains NUL");
    const std::string key(keyCp932);
    const Cp932Locale locale;
    // 4797F8 先查当前节点的键，随后沿用同一个 RGB/三次整数求值实现。
    const auto value = CaptureSettingValueAt47D470(errorCString, node, key.c_str());
    return ParseRgb(std::string(RuntimeStringView(value.Value())), parser, locale.Get());
}

std::int32_t ReadSettingNodeIntegerAt47BBE0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& node, const std::string_view keyCp932,
    IMessageWindowConfigExpressionParser& parser)
{
    if (keyCp932.find('\0') != std::string_view::npos)
        throw std::invalid_argument("Setting node key contains NUL");
    const std::string key(keyCp932);
    // 47BC14 -> 45E770 -> 47BC42：空格/Tab 删除、整数结果和失败均沿用既有边界。
    const auto value = CaptureSettingValueAt47D470(errorCString, node, key.c_str());
    return EvaluateInteger(std::string(RuntimeStringView(value.Value())), parser);
}

MessageWindowConfigRuntime::MessageWindowConfigRuntime(
    const SaveDataEnvironmentArchive& save, MessageWindowConfigType type,
    std::optional<std::uint32_t> colorOverride, std::int32_t transparencyOverride)
    : save_(save), type_(type), colorOverride_(colorOverride),
      transparencyOverride_(transparencyOverride)
{
    if (type != MessageWindowConfigType::Type1 && type != MessageWindowConfigType::Type2)
        throw std::invalid_argument("CMesWin config type is invalid");
}

MessageWindowColors MessageWindowConfigRuntime::Read() const noexcept
{
    const auto& bytes = type_ == MessageWindowConfigType::Type1 ? save_.bytesAt40 : save_.bytesAt48;
    // 分别短路两个 fallback；透明度 DWORD 按原寄存器位型转成 int32，而非钳制。
    const auto color = colorOverride_ ? *colorOverride_ : ReadDword(bytes, 0);
    const auto transparency = transparencyOverride_ >= 0 ? transparencyOverride_ :
        std::bit_cast<std::int32_t>(ReadDword(bytes, 4));
    return {color, transparency};
}

std::optional<std::uint32_t> MessageWindowConfigRuntime::ColorOverride() const noexcept
{
    return colorOverride_;
}

std::int32_t MessageWindowConfigRuntime::TransparencyOverride() const noexcept
{
    return transparencyOverride_;
}

MessageWindowConfigPairRuntime::MessageWindowConfigPairRuntime(
    const ParamsGameSystem& params, const SaveDataEnvironmentArchive& save,
    IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
    : type1_(save, MessageWindowConfigType::Type1),
      type2_(save, MessageWindowConfigType::Type2)
{
    ApplyMessageWindowConfigOverrides(params, type1_, type2_, parser, errors);
}

void ApplyMessageWindowConfigOverrides(const ParamsGameSystem& params,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
{
    FixedMessageWindowParamsSource source(params);
    ApplyMessageWindowConfigOverrides(source, type1, type2, parser, errors);
}

const ParamsGameSystem& MessageWindowParamsRootSource::ReadGameSystem(MessageWindowParamsReadPoint)
{
    // 每个 418480 读点只借当前 child，绝不复制 shared_ptr 或缓存第一次得到的 GameSystem。
    const auto& game = root_.GetOrCreateAt418480().GameSystem();
    if (!game) throw std::logic_error("CMesWin requires the current params GameSystem");
    return *game;
}

void ApplyMessageWindowConfigOverrides(ParamsRuntimeRootOwner& root,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
{
    MessageWindowParamsRootSource source(root);
    ApplyMessageWindowConfigOverrides(source, type1, type2, parser, errors);
}

void ApplyMessageWindowConfigOverrides(IMessageWindowParamsSource& source,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser, ISettingTagErrorSource& errors)
{
    using Point = MessageWindowParamsReadPoint;
    // 459CED/459D13 在四个局部 try 外；失败由 CMesWin state 4 的 45A46C 处理。
    if (!source.ReadGameSystem(Point::WindowSettings).settingTags[2].Native().baseAt00) return;

    // EH (5..8) -> 459EBB；失败不阻止后续三个独立尝试。
    try { ReadColorOverride(source,
        Point::Type1ColorName, Point::Type1ColorDefinition, Type1Section, parser, type1.colorOverride_, errors); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}

    // EH (10..12) -> 459F99；成功后的换算/赋值在原局部 try 之外。
    std::optional<std::int32_t> percentage;
    try { percentage = ReadTransparency(source, Point::Type1Transparency, Type1Section, parser, errors); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}
    if (percentage && *percentage >= 0)
        type1.transparencyOverride_ = ConvertTransparency(*percentage);

    // EH (14..17) -> 45A13A；不回滚 type1 或已成功的颜色值。
    try { ReadColorOverride(source,
        Point::Type2ColorName, Point::Type2ColorDefinition, Type2Section, parser, type2.colorOverride_, errors); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}

    // EH (19..21) -> 45A218。
    percentage.reset();
    try { percentage = ReadTransparency(source, Point::Type2Transparency, Type2Section, parser, errors); }
    catch (const MissingSettingsRoot&) { throw; }
    catch (...) {}
    if (percentage && *percentage >= 0)
        type2.transparencyOverride_ = ConvertTransparency(*percentage);
}

const MessageWindowConfigRuntime& MessageWindowConfigPairRuntime::Type1() const noexcept
{
    return type1_;
}

const MessageWindowConfigRuntime& MessageWindowConfigPairRuntime::Type2() const noexcept
{
    return type2_;
}
}
