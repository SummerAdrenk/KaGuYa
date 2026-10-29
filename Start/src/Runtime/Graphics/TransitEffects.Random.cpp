#include "TransitEffects.Random.h"
#include "TransitEffects.h"

#include <cmath>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime::TransitEffects
{
namespace
{
// 402B10/5B1C70..90：逐项 trunc(cos((i*pi)*2^-8)*65536)。
// 固定初始化结果，避免运行库 cos 的微小误差影响整数查表。
constexpr std::array<std::int32_t, 512> CosineAt619AB8{
    65536, 65531, 65516, 65491, 65457, 65412, 65358, 65294, 65220, 65136, 65043, 64939, 64826, 64703, 64571, 64428,
    64276, 64115, 63943, 63762, 63571, 63371, 63162, 62942, 62714, 62475, 62228, 61971, 61705, 61429, 61144, 60850,
    60547, 60235, 59913, 59583, 59243, 58895, 58538, 58172, 57797, 57414, 57022, 56621, 56212, 55794, 55368, 54933,
    54491, 54040, 53581, 53114, 52639, 52155, 51665, 51166, 50660, 50146, 49624, 49095, 48558, 48015, 47464, 46906,
    46340, 45768, 45189, 44603, 44011, 43412, 42806, 42194, 41575, 40950, 40319, 39682, 39039, 38390, 37736, 37075,
    36409, 35738, 35061, 34379, 33692, 32999, 32302, 31600, 30893, 30181, 29465, 28745, 28020, 27291, 26557, 25820,
    25079, 24334, 23586, 22833, 22078, 21319, 20557, 19792, 19024, 18253, 17479, 16702, 15923, 15142, 14359, 13573,
    12785, 11995, 11204, 10410, 9616, 8819, 8022, 7223, 6423, 5622, 4821, 4018, 3215, 2412, 1608, 804,
    0, -804, -1608, -2412, -3215, -4018, -4821, -5622, -6423, -7223, -8022, -8819, -9616, -10410, -11204, -11995,
    -12785, -13573, -14359, -15142, -15923, -16702, -17479, -18253, -19024, -19792, -20557, -21319, -22078, -22833, -23586, -24334,
    -25079, -25820, -26557, -27291, -28020, -28745, -29465, -30181, -30893, -31600, -32302, -32999, -33692, -34379, -35061, -35738,
    -36409, -37075, -37736, -38390, -39039, -39682, -40319, -40950, -41575, -42194, -42806, -43412, -44011, -44603, -45189, -45768,
    -46340, -46906, -47464, -48015, -48558, -49095, -49624, -50146, -50660, -51166, -51665, -52155, -52639, -53114, -53581, -54040,
    -54491, -54933, -55368, -55794, -56212, -56621, -57022, -57414, -57797, -58172, -58538, -58895, -59243, -59583, -59913, -60235,
    -60547, -60850, -61144, -61429, -61705, -61971, -62228, -62475, -62714, -62942, -63162, -63371, -63571, -63762, -63943, -64115,
    -64276, -64428, -64571, -64703, -64826, -64939, -65043, -65136, -65220, -65294, -65358, -65412, -65457, -65491, -65516, -65531,
    -65536, -65531, -65516, -65491, -65457, -65412, -65358, -65294, -65220, -65136, -65043, -64939, -64826, -64703, -64571, -64428,
    -64276, -64115, -63943, -63762, -63571, -63371, -63162, -62942, -62714, -62475, -62228, -61971, -61705, -61429, -61144, -60850,
    -60547, -60235, -59913, -59583, -59243, -58895, -58538, -58172, -57797, -57414, -57022, -56621, -56212, -55794, -55368, -54933,
    -54491, -54040, -53581, -53114, -52639, -52155, -51665, -51166, -50660, -50146, -49624, -49095, -48558, -48015, -47464, -46906,
    -46340, -45768, -45189, -44603, -44011, -43412, -42806, -42194, -41575, -40950, -40319, -39682, -39039, -38390, -37736, -37075,
    -36409, -35738, -35061, -34379, -33692, -32999, -32302, -31600, -30893, -30181, -29465, -28745, -28020, -27291, -26557, -25820,
    -25079, -24334, -23586, -22833, -22078, -21319, -20557, -19792, -19024, -18253, -17479, -16702, -15923, -15142, -14359, -13573,
    -12785, -11995, -11204, -10410, -9616, -8819, -8022, -7223, -6423, -5622, -4821, -4018, -3215, -2412, -1608, -804,
    0, 804, 1608, 2412, 3215, 4018, 4821, 5622, 6423, 7223, 8022, 8819, 9616, 10410, 11204, 11995,
    12785, 13573, 14359, 15142, 15923, 16702, 17479, 18253, 19024, 19792, 20557, 21319, 22078, 22833, 23586, 24334,
    25079, 25820, 26557, 27291, 28020, 28745, 29465, 30181, 30893, 31600, 32302, 32999, 33692, 34379, 35061, 35738,
    36409, 37075, 37736, 38390, 39039, 39682, 40319, 40950, 41575, 42194, 42806, 43412, 44011, 44603, 45189, 45768,
    46340, 46906, 47464, 48015, 48558, 49095, 49624, 50146, 50660, 51166, 51665, 52155, 52639, 53114, 53581, 54040,
    54491, 54933, 55368, 55794, 56212, 56621, 57022, 57414, 57797, 58172, 58538, 58895, 59243, 59583, 59913, 60235,
    60547, 60850, 61144, 61429, 61705, 61971, 62228, 62475, 62714, 62942, 63162, 63371, 63571, 63762, 63943, 64115,
    64276, 64428, 64571, 64703, 64826, 64939, 65043, 65136, 65220, 65294, 65358, 65412, 65457, 65491, 65516, 65531
};

RECT BlockRect(std::int32_t x, std::int32_t y, std::int32_t width,
    std::int32_t height, std::int32_t stepX, std::int32_t stepY)
{
    const auto left = x >= 0 ? x : 0;
    const auto top = y >= 0 ? y : 0;
    const auto w = Add(left, stepX) <= width ? stepX : Sub(width, left);
    const auto h = Add(top, stepY) <= height ? stepY : Sub(height, top);
    return {left, top, Add(left, w), Add(top, h)};
}

void CopyBlock(const TransitOperation& p, IGraphicsTransitApi& graphics,
    GraphicsSurfaceHandle handle, const RECT& rect, std::int32_t x, std::int32_t y)
{
    if (p.targetKind == 0)
        static_cast<void>(graphics.TransitCopyDraw(handle, x, y, nullptr, &rect, p.clip, 0));
    else if (p.targetKind == 1)
        static_cast<void>(graphics.TransitCopyBlit(p.destination, handle, x, y, nullptr, &rect, p.clip, 0));
}

std::uint8_t Radial(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable, std::int32_t stepX, bool modulated)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);

    const auto& source = Source(p);
    const auto height = source.heightAt0C;
    const auto width = source.widthAt08;
    const auto sum = Add(height, width);
    const auto remaining = Sub(sum, Mul(sum, p.current) >> 8);
    const auto phase = Mul(Sub(256, p.current), 2);
    for (std::int32_t y = 0; y < height; y = Add(y, 2))
        for (std::int32_t x = 0; x < width; x = Add(x, stepX))
        {
            auto dx = Sub(x, width / 2);
            auto dy = Sub(y, height / 2);
            // 44CDE4..44CE17（另两函数同序）：先 DWORD 平方和，再有符号转 double。
            // 负平方和的 sqrt 为 NaN，CVTTSD2SI 返回 80000000h；不能作 C++ 浮点强转。
            const auto squared = Add(Mul(dy, dy), Mul(dx, dx));
            // 568DC0 -> 574FA0(case49) 还调用 CRT matherr/设置 EDOM，不能仅用 SQRTSD。
            const auto root = std::sqrt(static_cast<double>(squared));
            const auto distance = _mm_cvttsd_si32(_mm_set_sd(root));
            if (distance != 0)
            {
                dx = Mul(dx, 256) / distance;
                dy = Mul(dy, 256) / distance;
            }
            const auto rect = BlockRect(x, y, width, height, stepX, 2);
            // 未知 targetKind 仍经过原几何计算，但不读句柄、不扩展为 Blend。
            if (p.targetKind != 0 && p.targetKind != 1) continue;
            const auto cosineY = modulated ? CosineAt619AB8[Sub(phase, 128) & 511] : 0;
            const auto cosineX = modulated ? CosineAt619AB8[phase & 511] : 0;
            const auto handle = source.handleAt04;
            // 44CF16/44CF32：索引用像素坐标；先 Y 后 X，两个乘法各自保留低 DWORD。
            auto offsetY = Mul(Mul(randomTable[static_cast<std::uint8_t>(Add(Add(Mul(y, 19), 1), x))], dy), remaining) >> 16;
            auto offsetX = Mul(Mul(randomTable[static_cast<std::uint8_t>(Add(Mul(y, 13), x))], dx), remaining) >> 16;
            if (modulated)
            {
                // 44D933..44D962：各轴单独乘表，不是二维旋转，不能合并两次 SAR16。
                offsetY = Mul(offsetY, cosineY) >> 16;
                offsetX = Mul(offsetX, cosineX) >> 16;
            }
            const auto destinationY = Add(Add(offsetY, rect.top), p.y);
            const auto destinationX = Add(Add(offsetX, rect.left), p.x);
            CopyBlock(p, graphics, handle, rect, destinationX, destinationY);
        }
    return 1;
}

std::uint8_t Jitter(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable, bool falling)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1))
        return CopyWhole(p, graphics);

    const auto& source = Source(p);
    const auto height = source.heightAt0C;
    const auto width = source.widthAt08;
    const auto divisor = Add(p.current >> 1, 1);
    const auto remaining = Sub(256, p.current);
    for (std::int32_t y = 0; y < height; y = Add(y, 4))
        for (std::int32_t x = 0; x < width; x = Add(x, 4))
        {
            // 449EF8/44A326：这里索引改用 4x4 块坐标，Y 索引没有径向分支的 +1。
            const auto randomX = randomTable[static_cast<std::uint8_t>(Add(Mul(y >> 2, 13), x >> 2))];
            const auto randomY = randomTable[static_cast<std::uint8_t>(Add(Mul(y >> 2, 19), x >> 2))];
            const auto offsetX = falling ? Mul(Sub(randomX, 128), remaining) >> 11
                                        : Sub(randomX, 128) / divisor;
            // 44A34D..44A361：只有 X 减 128，Y 使用零扩展随机字节。
            const auto offsetY = falling ? Mul(randomY, remaining) >> 11
                                        : Sub(randomY, 128) / divisor;
            const auto rect = BlockRect(x, y, width, height, 4, 4);
            if (p.targetKind != 0 && p.targetKind != 1) continue;
            const auto handle = source.handleAt04;
            const auto destinationY = Add(Add(offsetY, rect.top), p.y);
            const auto destinationX = Add(Add(offsetX, rect.left), p.x);
            CopyBlock(p, graphics, handle, rect, destinationX, destinationY);
        }
    return 1;
}
}

const std::array<std::int32_t, 512>& CosineTableAt619AB8() noexcept
{
    return CosineAt619AB8;
}

std::uint8_t RandomAt44CCD0(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable)
{
    return Radial(p, graphics, randomTable, 4, false);
}

std::uint8_t RandomAt44D1B0(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable)
{
    return Radial(p, graphics, randomTable, 2, false);
}

std::uint8_t RandomAt44D690(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable)
{
    return Radial(p, graphics, randomTable, 4, true);
}

std::uint8_t RandomAt449E10(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable)
{
    return Jitter(p, graphics, randomTable, false);
}

std::uint8_t RandomAt44A230(const TransitOperation& p, IGraphicsTransitApi& graphics,
    const std::array<std::uint8_t, 256>& randomTable)
{
    return Jitter(p, graphics, randomTable, true);
}
}
