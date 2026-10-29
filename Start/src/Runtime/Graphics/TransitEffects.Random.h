#pragma once

#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime::TransitEffects
{
const std::array<std::int32_t, 512>& CosineTableAt619AB8() noexcept;
// 449DD0 的七个值参数由 dispatcher 快照；随机表仍引用 CTransit+F8，不重新播种。
std::uint8_t RandomAt44CCD0(const TransitOperation&, IGraphicsTransitApi&,
    const std::array<std::uint8_t, 256>& randomTable);
std::uint8_t RandomAt44D1B0(const TransitOperation&, IGraphicsTransitApi&,
    const std::array<std::uint8_t, 256>& randomTable);
std::uint8_t RandomAt44D690(const TransitOperation&, IGraphicsTransitApi&,
    const std::array<std::uint8_t, 256>& randomTable);
std::uint8_t RandomAt449E10(const TransitOperation&, IGraphicsTransitApi&,
    const std::array<std::uint8_t, 256>& randomTable);
std::uint8_t RandomAt44A230(const TransitOperation&, IGraphicsTransitApi&,
    const std::array<std::uint8_t, 256>& randomTable);
}
