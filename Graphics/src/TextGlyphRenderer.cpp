#include "TextRendererState.h"
#include "SurfaceTransfer.h"
#include <Logging/Logger.h>

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{
std::uint32_t BgraColor(COLORREF color) noexcept
{ return (GetRValue(color) << 16) | (GetGValue(color) << 8) | GetBValue(color); }

[[noreturn]] void InvalidWorkAccess(const SurfaceInfo& info, std::int64_t word)
{
    Logging::WriteFormat(Logging::Level::Error,
        "文字工作DIB访问越界：word=%lld size=%zu width=%d height=%d pitch=%d；不裁剪或生成替代图。",
        static_cast<long long>(word), info.pixels.size(), info.width, info.height, info.pitch);
    throw std::out_of_range("Text work DIB access outside allocation");
}

struct Pixels
{
    SurfaceInfo& info;
    std::uint8_t* At(std::int64_t word) const
    {
        // 原内核可跨逻辑行；只拒绝真正越出底层分配的读写，不按x/y另行裁剪。
        if (word < 0 || !info.pixels.data() || static_cast<std::uint64_t>(word) >= info.pixels.size() / 4)
            InvalidWorkAccess(info, word);
        return info.pixels.data() + static_cast<std::size_t>(word) * 4;
    }
    std::uint32_t Read(std::int64_t word) const
    { std::uint32_t value; std::memcpy(&value, At(word), sizeof(value)); return value; }
    void Write(std::int64_t word, std::uint32_t value) const
    { std::memcpy(At(word), &value, sizeof(value)); }
    std::int64_t Offset(int x, int y) const noexcept { return std::int64_t(y) * info.width + x; }
};

void Clear(TextWorkDib& work) { (void)ClearTextWorkSurface(work.Info()); }

void BaseGlyph(TextState& state, TextWorkDib& work, const GlyphRecord& glyph, const int x, const int y)
{
    if (!state.font) throw std::logic_error("Text font body absent");
    // 原40BA0内部捕获失败；处理器不据false提前退出或撤销前缀。
    (void)state.font->DrawGlyph(work.Device(), glyph.character, x, y);
}

void MarkedGlyph(TextState& state, TextWorkDib& work, const GlyphRecord& glyph,
    const int x, const int y, const unsigned tag)
{
    if (!state.font) throw std::logic_error("Text font body absent");
    state.font->textColor = RGB(0, 0, tag);
    BaseGlyph(state, work, glyph, x, y);
}

std::uint32_t TagColor(std::uint32_t pixel, const std::array<std::uint32_t, 4>& colors)
{
    const auto tag = pixel & 0xFFFFFFu;
    if (tag >= colors.size())
    {
        Logging::Write(Logging::Level::Error, "文字标签超出原颜色表，拒绝越界读表。");
        throw 6;
    }
    return colors[tag];
}

void Mono(TextState& state, const GlyphRecord& glyph, const bool vertical)
{
    Clear(state.baseWork);
    if (vertical) Clear(state.verticalWork);
    auto& work = vertical ? state.verticalWork : state.baseWork;
    if (state.options.shadowColor != CLR_INVALID)
    {
        const int x = vertical ? state.options.shadowOffsetY : state.options.shadowOffsetX;
        const int y = vertical ? std::bit_cast<int>(0u - std::bit_cast<std::uint32_t>(state.options.shadowOffsetX)) : state.options.shadowOffsetY;
        MarkedGlyph(state, work, glyph, x, y, 2);
    }
    else if (!vertical && state.options.outlineColor != CLR_INVALID)
    {
        if (!state.font) throw std::logic_error("Text font body absent");
        // 原D118只置一次标签；D125/D1DE/D204/D2BF在每次绘字前各自重读半径。
        state.font->textColor = RGB(0, 0, 3);
        BaseGlyph(state, work, glyph, state.options.outlineWidth, 0);
        BaseGlyph(state, work, glyph, -int(state.options.outlineWidth), 0);
        BaseGlyph(state, work, glyph, 0, -int(state.options.outlineWidth));
        BaseGlyph(state, work, glyph, 0, state.options.outlineWidth);
    }
    MarkedGlyph(state, work, glyph, 0, 0, 1);
    state.font->textColor = state.options.textColor; // 原3D392/3F3E1先恢复对象颜色。
    const std::array<std::uint32_t, 4> colors{0,
        0xFF000000u | BgraColor(state.options.textColor),
        0xFF000000u | BgraColor(state.options.shadowColor),
        0xFF000000u | BgraColor(state.options.outlineColor)};
    Pixels source{work.Info()}, target{state.baseWork.Info()};
    const int nx = vertical ? glyph.extent.cy : glyph.extent.cx;
    const int ny = vertical ? glyph.extent.cx : glyph.extent.cy;
    for (int y = 0; y < ny; ++y)
        for (int x = 0; x < nx; ++x)
        {
            const auto word = source.Read(source.Offset(x, y));
            if (vertical)
                target.Write(target.Offset(target.info.width - 1 - y, x), TagColor(word, colors));
            else if (word & 0xFFFFFFu)
                target.Write(target.Offset(x, y), TagColor(word, colors));
        }
}

void EffectOver(const Pixels& foreground, std::int64_t f, const Pixels& background, std::int64_t b)
{
    const auto front = foreground.Read(f), back = background.Read(b);
    const unsigned a = front >> 24, q = (back >> 24) * (255u - a) / 255u;
    const unsigned alpha = a + q;
    if (!alpha) return;
    if (!a) { foreground.Write(f, back); return; }
    if (!q) return;
    std::uint32_t result = alpha << 24;
    for (unsigned shift = 0; shift < 24; shift += 8)
        result |= (((front >> shift & 255u) * a + (back >> shift & 255u) * q) / alpha) << shift;
    foreground.Write(f, result);
}

void Shadow(TextState& state, const GlyphRecord& glyph, const bool vertical)
{
    Clear(state.shadowWork);
    Pixels front{vertical ? state.verticalWork.Info() : state.baseWork.Info()};
    Pixels back{state.shadowWork.Info()};
    const auto color = BgraColor(state.options.shadowColor);
    const int ox = vertical ? state.options.shadowOffsetY : state.options.shadowOffsetX;
    const int oy = vertical ? std::bit_cast<int>(0u - std::bit_cast<std::uint32_t>(state.options.shadowOffsetX)) : state.options.shadowOffsetY;
    const int nx = vertical ? glyph.extent.cy : glyph.extent.cx;
    const int ny = vertical ? glyph.extent.cx : glyph.extent.cy;
    // 原偏移循环仅有上界。负逻辑坐标若仍在真实分配内亦须执行；真正越界由At明确拒绝。
    for (int y = oy; y < ny; ++y)
        for (int x = ox; x < nx; ++x)
        {
            const auto alpha = front.Read(std::int64_t(y - std::int64_t(oy)) * front.info.width + x - std::int64_t(ox)) & 0xFF000000u;
            if (alpha) back.Write(back.Offset(x, y), alpha | color);
        }
    // 原F610/FF80也按cy行、cx列，并让B8跟随A0行距；不可改为普通纵排二维blend。
    for (int y = 0; y < glyph.extent.cy; ++y)
        for (int x = 0; x < glyph.extent.cx; ++x)
        {
            const auto offset = front.Offset(x, y);
            EffectOver(front, offset, back, offset);
        }
}

void Outline(TextState& state, const GlyphRecord& glyph)
{
    Clear(state.outlineWork);
    Pixels front{state.baseWork.Info()}, back{state.outlineWork.Info()};
    const int radius = state.options.outlineWidth;
    const auto color = BgraColor(state.options.outlineColor);
    // 原E002/EEC0按源像素逐个四次覆盖，含r=0；不是max膨胀或四张图整体叠加。
    for (int y = 0; y < glyph.extent.cy; ++y)
        for (int x = 0; x < glyph.extent.cx; ++x)
        {
            const auto source = front.Offset(x, y);
            if (!(front.Read(source) >> 24)) continue;
            back.Write(back.Offset(x, y + radius), (front.Read(source) & 0xFF000000u) | color);
            back.Write(back.Offset(x + 2 * radius, y + radius), (front.Read(source) & 0xFF000000u) | color);
            back.Write(back.Offset(x + radius, y), (front.Read(source) & 0xFF000000u) | color);
            back.Write(back.Offset(x + radius, y + 2 * radius), (front.Read(source) & 0xFF000000u) | color);
        }
    for (int y = 0; y < glyph.extent.cy; ++y)
        for (int x = 0; x < glyph.extent.cx; ++x)
            EffectOver(front, front.Offset(x, y), back, back.Offset(x + radius, y + radius));
}

void Scaled(TextState& state, const GlyphRecord& glyph, const bool vertical, const int scale)
{
    Clear(state.baseWork);
    if (vertical) Clear(state.verticalWork);
    Clear(state.scaledWork);
    if (!state.oversampledFont) throw std::logic_error("Oversampled text font body absent");
    (void)state.oversampledFont->DrawGlyph(state.scaledWork.Device(), glyph.character, 0, 0);
    Pixels source{state.scaledWork.Info()}, target{vertical ? state.verticalWork.Info() : state.baseWork.Info()};
    const auto color = BgraColor(state.options.textColor);
    const int nx = vertical ? glyph.extent.cy : glyph.extent.cx;
    const int ny = vertical ? glyph.extent.cx : glyph.extent.cy;
    for (int y = 0; y < ny; ++y)
        for (int x = 0; x < nx; ++x)
        {
            unsigned sum = 0;
            for (int row = 0; row < scale; ++row)
                for (int column = 0; column < scale; ++column)
                    sum += source.Read((std::int64_t(y) * scale + row) * source.info.width + std::int64_t(x) * scale + column) & 255u;
            target.Write(target.Offset(x, y), color | ((sum / unsigned(scale * scale)) << 24));
        }
    if (state.options.shadowColor != CLR_INVALID) Shadow(state, glyph, vertical);
    else if (!vertical && state.options.outlineColor != CLR_INVALID) Outline(state, glyph);
    if (vertical)
    {
        Pixels from{state.verticalWork.Info()}, to{state.baseWork.Info()};
        for (int y = 0; y < glyph.extent.cx; ++y)
            for (int x = 0; x < glyph.extent.cy; ++x)
                to.Write(to.Offset(to.info.width - 1 - y, x), from.Read(from.Offset(x, y)));
    }
}
}

bool RenderTextGlyphWork(TextState& state, const GlyphRecord& glyph, const unsigned char mode, const bool vertical)
{
    switch (mode)
    {
    case 1: Mono(state, glyph, vertical); return true;
    case 2: Scaled(state, glyph, vertical, 2); return true;
    case 3: Scaled(state, glyph, vertical, 4); return true;
    default: return false;
    }
}
}
