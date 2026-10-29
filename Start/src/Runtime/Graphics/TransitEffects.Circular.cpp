#include "TransitEffects.h"

// x86 CRT的专用XMM0入参/返回ABI；只能由下方汇编调用，不能按普通C函数调用。
extern "C" void __cdecl _libm_sse2_sqrt_precise();

namespace KaGuYa::Start::Runtime::TransitEffects
{
namespace
{
std::int32_t RootToDword(const std::int32_t value, const bool twice)
{
    // 44A72D..44A73E/568DC0：有符号DWORD转double，必要时再倍增，始终调用precise入口。
    // std::sqrt在/O2 /fp:precise仍会将正数改成SQRTSD，跳过MXCSR/x87非默认控制字分支。
    // 直接衔接CVTT保留NaN的0x80000000和异常状态，避免C++转换或x87返回值中转。
    std::int32_t result;
    __asm
    {
        cvtsi2sd xmm0, value
        cmp twice, 0
        je rootReady
        addsd xmm0, xmm0
    rootReady:
        call _libm_sse2_sqrt_precise
        cvttsd2si eax, xmm0
        mov result, eax
    }
    return result;
}
}

std::uint8_t CircularReveal(const TransitOperation& p, IGraphicsTransitApi& graphics, const std::int32_t opcode)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    std::int32_t diameter;
    if (opcode == 54)
        diameter = Mul(p.current, Add(height, width)) >> 8;
    else
    {
        // 50/51与52/53的平方和不同；整数平方先DWORD回绕，再带符号除4。
        const auto squared = opcode <= 51 ? Add(Mul(width, width) / 4, Mul(height, height))
                                          : Add(Mul(height, height) / 4, Mul(width, width));
        diameter = Mul(p.current, RootToDword(squared, true)) >> 8;
    }
    const auto radius = diameter >> 1;
    std::int32_t row = opcode == 50 ? 0 : opcode == 51 ? height : Sub(height >> 1, radius);
    const auto end = opcode == 50 ? radius : opcode == 51 ? Sub(height, radius) : Add(height >> 1, radius);
    const bool descending = opcode == 51;
    if (descending ? row <= end : row >= end) return 1;
    const auto squaredRadius = Mul(diameter, diameter) / 4;
    std::int32_t distance = opcode <= 51 ? 0 : Sub(0, radius);
    do
    {
        const auto extent = RootToDword(Sub(squaredRadius, Mul(distance, distance)), opcode <= 51 || opcode == 54);
        std::int32_t left = 0;
        if (opcode == 53) left = Sub(width, extent);
        else if (opcode != 52) left = Sub(width >> 1, extent >> 1);
        if (left < 0) left = 0;
        const auto top = row >= 0 ? row : 0;
        const auto w = Add(left, extent) <= width ? extent : Sub(width, left);
        const auto h = Add(top, 1) > height ? Sub(height, top) : 1;
        // 原算法保留首行/末行的空矩形、重复clamp到0的行及越界反向矩形。
        DrawRect(p, graphics, RECT{left, top, Add(left, w), Add(top, h)});
        row = Add(row, descending ? -1 : 1);
        distance = Add(distance, 1);
    } while (descending ? row > end : row < end);
    return 1;
}

std::uint8_t EdgeBandsAt44E1B0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    const auto extent = Mul(p.current, height / 2) / 255;
    const auto firstEnd = extent <= height ? extent : height;
    DrawRect(p, graphics, RECT{0, 0, width, firstEnd});
    const auto candidate = Sub(height, extent);
    const auto top = candidate >= 0 ? candidate : 0;
    const auto h = Add(top, extent) <= height ? extent : Sub(height, top);
    DrawRect(p, graphics, RECT{0, top, width, Add(top, h)});
    return 1;
}

std::uint8_t CenterBandAt44DFD0(const TransitOperation& p, IGraphicsTransitApi& graphics)
{
    if (p.current <= 0) return 1;
    if (p.current >= 255 && (p.targetKind == 0 || p.targetKind == 1)) return CopyWhole(p, graphics);
    const auto width = Source(p).widthAt08;
    const auto height = Source(p).heightAt0C;
    const auto half = height / 2;
    const auto extent = Mul(p.current, half) / 255;
    const auto candidate = Sub(half, extent);
    const auto top = candidate >= 0 ? candidate : 0;
    const auto doubled = Mul(2, extent);
    const auto h = Add(top, doubled) <= height ? doubled : Sub(height, top);
    DrawRect(p, graphics, RECT{0, top, width, Add(top, h)});
    return 1;
}
}
