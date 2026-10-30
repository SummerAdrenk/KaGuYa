#include "TextData.h"
#include <array>

namespace KaGuYa::Start::Runtime::NeeChanTextData
{
namespace
{
// Original 0x419E30: mess_win_type is 0..1, stride 64; each record read from this game's PE.
// Name ASM 41A28E reads +30 as DWORD: 1 = x - trunc(width/2), 2 = x - width.
// Keep wire value 1 (CenterAtAnchor); this is not Baku's bool or internal CenterInRegion (3).
constexpr std::array<TextWindowTextDescriptor, 2> Records{{
    // Original PE VA 0x552010; all 64 bytes retained in text-layout-profiles evidence.
    {{26, 700, 0, 18}, 260, 562, {28, 700, 0, 0}, 128, 562, 1, 1, 1, 10},
    // Original PE VA 0x552050; all 64 bytes retained in text-layout-profiles evidence.
    {{26, 700, 0, 18}, 260, 562, {28, 700, 0, 0}, 128, 562, 1, 1, 1, 10},
}};
}
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept { return Records; }
}
