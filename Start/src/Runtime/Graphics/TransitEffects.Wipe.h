#pragma once

#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime::TransitEffects
{
// 与其他 >2 回调一致：dispatcher 先快照七个值参数，source 对象内容仍为借用。
std::uint8_t WaveAt44F000(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t WaveAt44F190(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t WaveAt44F340(const TransitOperation&, IGraphicsTransitApi&);
std::uint8_t WaveAt44F4D0(const TransitOperation&, IGraphicsTransitApi&);

// 42..49：firstTable=true/false 交替，每对 shift=0/1/2/3。
// 私有现代入口只承接这八个包装的参数域；不对域外 shift 伪造成功。
std::uint8_t OrderedWipeAt44F7F0(const TransitOperation&, IGraphicsTransitApi&,
    bool firstTable, std::uint32_t shift);
}
