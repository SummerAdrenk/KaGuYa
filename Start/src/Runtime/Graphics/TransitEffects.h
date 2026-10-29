#pragma once

#include <Runtime/TransitRuntime.h>

#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::TransitEffects
{
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) - static_cast<std::uint32_t>(b));
}
inline std::int32_t Mul(std::int32_t a, std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
}
// 原生入口直接读24h正文；现代checked入口显式保留其空source异常，不在失败后切换。
class SourceAccessScope final
{
public:
    explicit SourceAccessScope(bool checked) noexcept;
    ~SourceAccessScope() noexcept;
    SourceAccessScope(const SourceAccessScope&) = delete;
    SourceAccessScope& operator=(const SourceAccessScope&) = delete;
private:
    bool previous_;
};
bool CheckedSourceAccess() noexcept;
inline const volatile GraphicsSurfaceNativeBody& Source(const TransitOperation& operation)
{
    // 在原分支实际读取 source 时才检查，不抢在 previous 复制等副作用之前。
    if (CheckedSourceAccess() && !operation.source) throw std::logic_error("CTransit source object is missing");
    return *operation.source;
}

std::uint8_t BlendAt448CB0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t UniversalAt44ED80(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t MosaicAt44DBF0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t CopyWhole(const TransitOperation&, IGraphicsTransitApi&);
void DrawRect(const TransitOperation&, IGraphicsTransitApi&, const RECT&);
std::uint8_t TileRevealAt44DDA0(const TransitOperation&, IGraphicsTransitApi&, std::int32_t step, std::int32_t size);
std::uint8_t SlidingCopy(const TransitOperation&, IGraphicsTransitApi&, bool vertical, bool negative);
std::uint8_t StaggeredBands(const TransitOperation&, IGraphicsTransitApi&, bool vertical, bool forward, std::int32_t size);
std::uint8_t CircularReveal(const TransitOperation&, IGraphicsTransitApi&, std::int32_t opcode);
std::uint8_t EdgeBandsAt44E1B0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t CenterBandAt44DFD0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t RevealAt44B3C0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t InterlaceAt44B5A0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t InterlaceAt44B8C0(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t OpposingLines(const TransitOperation&, IGraphicsTransitApi&, bool columns);
std::uint8_t TwoPhaseInterlace(const TransitOperation&, IGraphicsTransitApi&, bool columns);
std::uint8_t StripedReveal(const TransitOperation&, IGraphicsTransitApi&,
    bool columns, bool reverse, std::int32_t phaseMask, std::int32_t thresholdShift);
}
