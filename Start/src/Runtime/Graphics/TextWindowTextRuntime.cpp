#include <Runtime/TextWindowTextRuntime.h>

#include <Runtime/LayerRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>

#include <bit>
#include <cstring>
#include <locale.h>
#include <mbstring.h>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) +
        static_cast<std::uint32_t>(b));
}

std::int32_t Sub32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) -
        static_cast<std::uint32_t>(b));
}

std::int32_t Mul32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) *
        static_cast<std::uint32_t>(b));
}

class Cp932Locale final
{
public:
    Cp932Locale() : locale_(_create_locale(LC_CTYPE, ".932"))
    {
        if (!locale_) throw std::runtime_error("CTextWin CP932 locale unavailable");
    }
    ~Cp932Locale() { _free_locale(locale_); }
    Cp932Locale(const Cp932Locale&) = delete;
    Cp932Locale& operator=(const Cp932Locale&) = delete;
    [[nodiscard]] _locale_t Get() const noexcept { return locale_; }
private:
    _locale_t locale_;
};

// 42D6B6..42D829：按 CP932 字符边界查 LF；不是占位符展开，也不去 CR。
std::int32_t PrepareNameNewlines(RuntimeCString& bytes, const TextWindowTextDescriptor& descriptor)
{
    if (bytes.GetLength() <= 0) return 0;
    Cp932Locale locale;
    if (!_mbschr_l(reinterpret_cast<const unsigned char*>(bytes.GetString()), '\n', locale.Get())) return 0;
    auto& strings = DefaultRuntimeStringManagerSource();
    auto flattenedOwner = ArchiveResourceNameAt40B530("", strings);
    auto remainderOwner = ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output)
        {
            auto* const header = reinterpret_cast<ATL::CStringData*>(
                reinterpret_cast<std::uintptr_t>(bytes.GetString()) - sizeof(ATL::CStringData));
            const auto* const cloned = CloneRuntimeCStringDataAt414540(header);
            const char* const characters = reinterpret_cast<const char*>(cloned + 1);
            std::memcpy(output, &characters, sizeof(characters));
        });
    auto& flattened = flattenedOwner.Value();
    auto& remainder = remainderOwner.Value();
    std::int32_t shift = 0;
    const auto* current = reinterpret_cast<const unsigned char*>(remainder.GetString());
    while (_mbscmp_l(current, reinterpret_cast<const unsigned char*>(""), locale.Get()) != 0)
    {
        const auto cachedLength = [&]()
        {
            return reinterpret_cast<const ATL::CStringData*>(current - sizeof(ATL::CStringData))->nDataLength;
        };
        const auto* newline = cachedLength() > 0 ? _mbschr_l(current, '\n', locale.Get()) : nullptr;
        if (newline)
        {
            shift = Add32(shift, Add32(descriptor.nameFont.size, descriptor.nameFont.lineSpacing));
            const auto length = static_cast<int>(newline - current);
            // 与资源名共用原419D00/420C80；保留临时CString的Clone、增持和释放时点。
            {
                auto left = ArchiveResourceNameLeftAt419D00(remainder, length, strings);
                AppendRuntimeCStringAt413550(flattened, left.Value().GetString(), left.Value().GetLength());
                left.ReleaseNormal(); // 42D778..42D794正常出口从当前槽取chars后撤资格。
            }
            auto tail = ArchiveResourceNameMidAt420C80(remainder, length + 1,
                Sub32(cachedLength(), Add32(length, 1)), strings); // 42D797保留跨Left/Append/Free的ESI。
            AssignRuntimeCStringAt410220(remainder, tail.Value());
            tail.ReleaseNormal();
        }
        else
        {
            AppendRuntimeCStringAt413550(flattened, reinterpret_cast<const char*>(current), cachedLength());
            SetRuntimeCStringAt437680(remainder, "", 0);
        }
        current = reinterpret_cast<const unsigned char*>(remainder.GetString());
    }
    if (descriptor.nameNewlineMode == 0) AssignRuntimeCStringAt410220(bytes, flattened);
    if (descriptor.nameNewlineMode == 0 || descriptor.nameNewlineMode == 1) shift = 0;
    // 42D83D使用循环缓存的ESI；随后才从flattened当前槽取chars。展开则均读当前槽。
    remainderOwner.ReleaseNormalCached(reinterpret_cast<const char*>(current));
    flattenedOwner.ReleaseNormal();
    return shift;
}
}

TextWindowTextRuntime::TextWindowTextRuntime(TextWindowTextState& state,
    RuntimeGraphicsSurface* surface, LayerList* parent, IGraphicsTextApi& text,
    IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services) noexcept
    : state_(state), surface_(surface), parent_(parent), text_(text), glyphs_(glyphs), services_(services)
{
}

bool TextWindowTextRuntime::PrepareNameAt42D5F0()
{
    // 5E1838={0,6,7,1,5E1828}，catch(...) -> 42DA6D/42DA73 返回 false。
    // 只还原 C++ 异常边界；不把原访问违例描述成可恢复的 C++ 异常。
    try
    {
        if (!state_.name) return true;
        const auto* descriptor = services_.ReadDescriptor();
        if (!descriptor) throw 1;
        auto name = state_.name->GetTextAt47D550();
        const auto shift = PrepareNameNewlines(name, *descriptor);

        const auto* colorOverride = services_.ReadNameColorOverride();
        const std::uint32_t* shadowOverride = nullptr;
        std::uint32_t color;
        if (colorOverride)
        {
            shadowOverride = services_.ReadNameShadowOverride();
            color = *colorOverride;
        }
        else
        {
            color = services_.ReadBodyColor();
        }
        static_cast<void>(services_.PrepareFont(descriptor->nameFont, color, shadowOverride));
        const auto x = Add32(state_.nameOffsetX, descriptor->nameX);
        const auto y = Add32(Sub32(state_.nameOffsetY, shift), descriptor->nameY);
        if (!text_.InitializeText(name.GetString(), x, y)) throw 1;

        state_.nameRect.top = 0;
        state_.nameRect.left = 0;
        state_.nameRect.right = Mul32(descriptor->minimumHalfCharacters, descriptor->nameFont.size >> 1);
        state_.nameRect.bottom = descriptor->nameFont.size;
        const auto measured = text_.ReadTextDrawRect();
        const auto width = Sub32(measured.right, measured.left);
        // 原按一个字号递增，没有额外宽度/扫描预算；保留 DWORD 运算，避免有符号溢出 UB。
        if (descriptor->expandName)
            while (width > Sub32(state_.nameRect.right, state_.nameRect.left))
                state_.nameRect.right = Add32(state_.nameRect.right, descriptor->nameFont.size);
        state_.nameRect.bottom = Sub32(measured.bottom, measured.top);
        state_.nameRect.right = Add32(state_.nameRect.right, Sub32(x, state_.nameRect.left));
        state_.nameRect.bottom = Add32(state_.nameRect.bottom, Sub32(y, state_.nameRect.top));
        state_.nameRect.left = x;
        state_.nameRect.top = y;

        // +40 框保持原锚点；只有 Graphics 中的字形基点随对齐变化。
        if (descriptor->nameAlignment == 1)
            text_.SetTextBasePosition(Sub32(x, width / 2), y);
        else if (descriptor->nameAlignment == 2)
            text_.SetTextBasePosition(Sub32(Add32(x, measured.left), measured.right), y);
        else if (descriptor->nameAlignment == static_cast<std::int32_t>(TextWindowNameAlignment::CenterInRegion))
            text_.SetTextBasePosition(Sub32(Add32(x,
                Sub32(state_.nameRect.right, state_.nameRect.left) / 2), width / 2), y);
        state_.contentRect = text_.ReadTextDrawRect();
        const auto length = static_cast<std::uint32_t>(text_.GetTextLength());
        static_cast<void>(BlitRangeAt419090(0, length));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool TextWindowTextRuntime::PrepareTextAt415150()
{
    if (!PrepareNameAt42D5F0()) throw 1;
    const auto* descriptor = services_.ReadDescriptor();
    if (!descriptor) throw 1;
    const auto color = services_.ReadBodyColor();
    static_cast<void>(services_.PrepareFont(descriptor->bodyFont, color, nullptr));
    const auto x = Add32(state_.bodyOffsetX, descriptor->bodyX);
    const auto y = Add32(state_.bodyOffsetY, descriptor->bodyY);
    if (!state_.body) throw 1;
    const auto body = state_.body->GetTextAt47D550();
    if (!text_.InitializeText(body.GetString(), x, y)) throw 1;
    const auto measured = text_.ReadTextDrawRect();
    return ::UnionRect(&state_.contentRect, &state_.contentRect, &measured) != FALSE;
}

bool TextWindowTextRuntime::BlitRangeAt419090(std::uint32_t begin, std::uint32_t end)
{
    RECT drawn{};
    for (auto index = begin; index < end; ++index)
    {
        // 不检查 handle 是否为零，不读长度、不创建/清空画布；RECT 在整段内复用。
        if (!surface_) throw std::logic_error("CTextWin glyph drawing requires Surface object");
        if (!glyphs_.BlitTextGlyphToSurface(surface_->Handle(), index, &drawn)) return false;
        if (!parent_) throw std::logic_error("CTextWin glyph invalidation requires parent LayerList");
        parent_->DirtyRects().Add(drawn);
    }
    return true;
}
}
