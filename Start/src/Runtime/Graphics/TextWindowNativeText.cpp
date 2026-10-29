#include <Runtime/TextWindowNativeText.h>

#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/LayerListNative.h>

#include <bit>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Add32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
std::int32_t Sub32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
std::int32_t Mul32(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}
void* ResolveWithBase(const YtlReferenceSlotRuntime& input, std::uintptr_t base) noexcept
{
    const volatile auto& slot = input;
    const auto* control = slot.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0u;
    const auto bias = slot.biasAt04;
    return reinterpret_cast<void*>(base + bias * stride);
}
void* Resolve(const YtlReferenceSlotRuntime& input) noexcept
{
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(input).baseAt00;
    return base ? ResolveWithBase(input, base) : nullptr;
}
RuntimeCString* CallTextCommand(void* command, RuntimeCString* output)
{
    // 42D66A/4151EB：真正的+04虚槽与hidden-result；不把空正文改为现代安全异常。
    using Function = RuntimeCString* (__thiscall*)(void*, RuntimeCString*);
    const void* table;
    std::memcpy(&table, command, sizeof(table));
    Function function;
    std::memcpy(&function, static_cast<const std::byte*>(table) + 4, sizeof(function));
    return function(command, output);
}
std::int32_t Length(const char* chars) noexcept
{
    return reinterpret_cast<const ATL::CStringData*>(chars - sizeof(ATL::CStringData))->nDataLength;
}
std::int32_t FindNewline(const char* chars)
{
    if (Length(chars) <= 0) return -1;
    const auto* found = _mbschr(reinterpret_cast<const unsigned char*>(chars), '\n');
    if (!found) return -1;
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(found) - reinterpret_cast<std::uintptr_t>(chars)));
}

std::int32_t PrepareNameNewlines(RuntimeCString& name,
    const TextWindowTextDescriptor& descriptor, IRuntimeStringManagerSource& strings)
{
    if (FindNewline(name.GetString()) < 0) return 0;
    auto flattened = ArchiveResourceNameAt40B530("", strings);
    auto remainder = ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output)
        {
            auto* const header = reinterpret_cast<ATL::CStringData*>(
                reinterpret_cast<std::uintptr_t>(name.GetString()) - sizeof(ATL::CStringData));
            const auto* cloned = CloneRuntimeCStringDataAt414540(header);
            const char* characters = reinterpret_cast<const char*>(cloned + 1);
            std::memcpy(output, &characters, sizeof(characters));
        });
    std::int32_t shift = 0;
    const char* current = remainder.Value().GetString();
    // 原__mbschr/__mbscmp使用进程多字节环境，不在每次排版时创建临时CP932 locale。
    while (_mbscmp(reinterpret_cast<const unsigned char*>(current),
        reinterpret_cast<const unsigned char*>("")) != 0)
    {
        const auto newline = FindNewline(current);
        if (newline >= 0)
        {
            shift = Add32(shift, Add32(descriptor.nameFont.lineSpacing, descriptor.nameFont.size));
            auto left = ArchiveResourceNameLeftAt419D00(remainder.Value(), newline, strings);
            AppendRuntimeCStringAt413550(flattened.Value(), left.Value().GetString(), left.Value().GetLength());
            left.ReleaseNormal();
            // 42D797读跨越Left/Append/Free保存的ESI；不能改读remainder的新header。
            auto tail = ArchiveResourceNameMidAt420C80(remainder.Value(), Add32(newline, 1),
                Sub32(Length(current), Add32(newline, 1)), strings);
            AssignRuntimeCStringAt410220(remainder.Value(), tail.Value());
            tail.ReleaseNormal();
        }
        else
        {
            AppendRuntimeCStringAt413550(flattened.Value(), current, Length(current));
            SetRuntimeCStringAt437680(remainder.Value(), "", 0);
        }
        current = remainder.Value().GetString();
    }
    const auto mode = descriptor.nameNewlineMode;
    if (mode == 0) AssignRuntimeCStringAt410220(name, flattened.Value());
    if (mode == 0 || mode == 1) shift = 0;
    remainder.ReleaseNormalCached(current);
    flattened.ReleaseNormal();
    return shift;
}
}

TextWindowNativeText::TextWindowNativeText(TextWindowLayerNativeBodyE8h& body,
    IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services,
    IRuntimeStringManagerSource& strings) noexcept
    : body_(body), text_(text), glyphs_(glyphs), services_(services), strings_(strings)
{
}

bool TextWindowNativeText::PrepareNameAt42D5F0(const YtlReferenceSlotRuntime& nameReference)
{
    try
    {
        if (!nameReference.baseAt00) return true;
        const auto* const descriptor = services_.ReadDescriptor();
        if (!descriptor) throw 1;
        auto name = ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
            [&](RuntimeCString* output) { static_cast<void>(CallTextCommand(Resolve(nameReference), output)); });
        const auto shift = PrepareNameNewlines(name.Value(), *descriptor, strings_);
        const auto nameColorBase = body_.nameColorAtB4.baseAt00;
        const std::uint32_t* shadow = nullptr;
        std::uint32_t color;
        if (!nameColorBase)
            color = services_.ReadBodyColor();
        else
        {
            // 42D892先解析C0，再用已固定的B4.base和晚读control/bias读取姓名色。
            shadow = static_cast<const std::uint32_t*>(Resolve(body_.nameShadowColorAtC0));
            color = *static_cast<const std::uint32_t*>(ResolveWithBase(body_.nameColorAtB4, nameColorBase));
        }
        static_cast<void>(services_.PrepareFont(descriptor->nameFont, color, shadow));
        const auto x = Add32(descriptor->nameX, body_.nameOffsetXAtE0);
        const auto y = Add32(descriptor->nameY, Sub32(body_.nameOffsetYAtE4, shift));
        if (!text_.InitializeText(name.Value().GetString(), x, y)) throw 1;
        auto& bounds = body_.nameBoundsAt40;
        bounds.top = 0;
        bounds.left = 0;
        bounds.right = Mul32(descriptor->nameFont.size >> 1, descriptor->minimumHalfCharacters);
        bounds.bottom = descriptor->nameFont.size;
        const auto measured = text_.ReadTextDrawRect();
        const auto width = Sub32(measured.right, measured.left);
        if (descriptor->expandName)
            while (width > Sub32(bounds.right, bounds.left))
                bounds.right = Add32(bounds.right, descriptor->nameFont.size);
        bounds.bottom = Sub32(measured.bottom, measured.top);
        bounds.right = Add32(bounds.right, Sub32(x, bounds.left));
        bounds.bottom = Add32(bounds.bottom, Sub32(y, bounds.top));
        bounds.left = x;
        bounds.top = y;
        if (descriptor->nameAlignment == 1)
            text_.SetTextBasePosition(Sub32(bounds.left, width / 2), y);
        else if (descriptor->nameAlignment == 2)
            text_.SetTextBasePosition(Add32(Sub32(x, measured.right), measured.left), y);
        else if (descriptor->nameAlignment == static_cast<std::int32_t>(TextWindowNameAlignment::CenterInRegion))
            // Baku 41D36E..41D3D3：使用已扩展区域宽度；两个有符号/2不可合并。
            text_.SetTextBasePosition(Sub32(Add32(x, Sub32(bounds.right, bounds.left) / 2), width / 2), y);
        const auto content = text_.ReadTextDrawRect();
        ::CopyRect(&body_.contentAt30, &content);
        const auto count = static_cast<std::uint32_t>(text_.GetTextLength());
        static_cast<void>(BlitRangeAt419090(0, count));
        name.ReleaseNormal();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

BOOL TextWindowNativeText::PrepareTextAt415150()
{
    if (!PrepareNameAt42D5F0(body_.nameAt70)) throw 1;
    const auto* const descriptor = services_.ReadDescriptor();
    if (!descriptor) throw 1;
    const auto color = services_.ReadBodyColor();
    static_cast<void>(services_.PrepareFont(descriptor->bodyFont, color, nullptr));
    const auto x = Add32(descriptor->bodyX, body_.bodyOffsetXAtD8);
    const auto y = Add32(descriptor->bodyY, body_.bodyOffsetYAtDC);
    const auto base = body_.bodyAt88.baseAt00;
    if (!base) throw 1;
    RuntimeCString* returned;
    auto text = ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output) { returned = CallTextCommand(ResolveWithBase(body_.bodyAt88, base), output); });
    // 4151F9从虚槽返回指针取chars，但正常释放的是hidden-result局部槽。
    const auto initialized = text_.InitializeText(returned->GetString(), x, y);
    text.ReleaseNormal();
    if (!initialized) throw 1;
    const auto measured = text_.ReadTextDrawRect();
    return ::UnionRect(&body_.contentAt30, &body_.contentAt30, &measured);
}

bool TextWindowNativeText::BlitRangeAt419090(std::uint32_t begin, std::uint32_t end)
{
    RECT drawn{};
    for (auto index = begin; index < end; ++index)
    {
        auto* const surface = static_cast<GraphicsSurfaceNativeBody*>(Resolve(body_.canvasAt24));
        const auto handle = surface->handleAt04;
        if (!glyphs_.BlitTextGlyphToSurface(handle, index, &drawn)) return false;
        auto* const parent = body_.layer.parentAt10;
        parent->vptrAt00->addDirtyAt20(parent, &drawn);
    }
    return true;
}
}
