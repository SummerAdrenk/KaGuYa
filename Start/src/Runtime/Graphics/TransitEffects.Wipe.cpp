#include "TransitEffects.Wipe.h"
#include "TransitEffects.h"
#include "TransitEffects.Random.h"

namespace KaGuYa::Start::Runtime::TransitEffects
{
namespace
{
// Start PE 6012D8..601357：两张 4x4 DWORD 表，不是推定的标准 Bayer 矩阵。
constexpr std::array<std::array<std::int32_t, 16>, 2> ThresholdsAt6012D8{{
    {0, 1, 2, 3, 11, 12, 13, 4, 10, 15, 14, 5, 9, 8, 7, 6},
    {6, 7, 8, 9, 5, 0, 1, 10, 4, 3, 2, 11, 15, 14, 13, 12}
}};

std::uint8_t Wave(const TransitOperation& p, IGraphicsTransitApi& graphics,
    std::uint32_t phase, std::uint32_t increment, bool doubleAmplitude)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);

    // 44F09B/44F22B/44F3DB/44F56A：即使模式不支持也先解引用尺寸，尺寸只读取一次。
    const auto& source = Source(p);
    const auto width = source.widthAt08;
    const auto height = source.heightAt0C;
    for (std::int32_t row = 0; row < height; ++row, phase += increment)
    {
        if (p.targetKind != 0 && p.targetKind != 1) continue;
        const auto cosine = CosineTableAt619AB8()[phase & 511];
        auto offset = Mul(Sub(255, p.current), cosine);
        // 38/39 先 DWORD ADD eax,eax 再 SAR16；40/41 没有该倍增。
        if (doubleAmplitude) offset = Add(offset, offset);
        const auto x = Add(p.x, offset >> 16);
        const auto y = Add(p.y, row);
        const RECT rect{0, row, width, Add(row, 1)};
        // 原每行临调用才重新读 CSurface+4；不能把句柄与尺寸一起快照。
        if (p.targetKind == 0)
            static_cast<void>(graphics.TransitCopyDraw(source.handleAt04, x, y, nullptr, &rect, p.clip, 0));
        else
            static_cast<void>(graphics.TransitCopyBlit(p.destination, source.handleAt04, x, y, nullptr, &rect, p.clip, 0));
    }
    return 1;
}
}

std::uint8_t WaveAt44F000(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    return Wave(p, graphics, 0, 1, true);
}

std::uint8_t WaveAt44F190(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    return Wave(p, graphics, 256, 1, true);
}

std::uint8_t WaveAt44F340(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    // 44F425..44F432：((row & 63) << 5) 是字节偏移，即 DWORD 索引 row*8。
    return Wave(p, graphics, 0, 8, false);
}

std::uint8_t WaveAt44F4D0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    return Wave(p, graphics, 256, 8, false);
}

std::uint8_t OrderedWipeAt44F7F0(const TransitOperation& p, IGraphicsTransitApi& graphics,
    bool firstTable, std::uint32_t shift)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);

    const auto& source = Source(p);
    const auto height = source.heightAt0C;
    const auto width = source.widthAt08;
    if (shift > 3) throw std::out_of_range("CTransit ordered wipe shift outside opcode 42..49 domain");
    const auto step = static_cast<std::int32_t>(1u << shift);
    const auto threshold = p.current >> 4;
    const auto& table = ThresholdsAt6012D8[firstTable ? 0 : 1];
    for (std::int32_t y = 0; y < height; y = Add(y, step))
        for (std::int32_t x = 0; x < width; x = Add(x, step))
        {
            const auto index = static_cast<std::uint32_t>(((y >> shift) & 3) * 4 + ((x >> shift) & 3));
            if (table[index] > threshold) continue;
            // 44F90B..44F948：有符号比较前先 DWORD 加法，不改成 std::min(x+step,width)。
            const auto left = x >= 0 ? x : 0;
            const auto top = y >= 0 ? y : 0;
            const auto w = Add(left, step) <= width ? step : Sub(width, left);
            const auto h = Add(top, step) <= height ? step : Sub(height, top);
            const RECT rect{left, top, Add(left, w), Add(top, h)};
            // 44F956 四分支：0/1 Copy，2/3 Blend；alpha 是原 current，返回 AL 均丢弃。
            DrawRect(p, graphics, rect);
        }
    return 1;
}
}
