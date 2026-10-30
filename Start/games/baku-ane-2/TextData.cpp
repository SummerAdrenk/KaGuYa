#include "TextData.h"
#include <array>

namespace KaGuYa::Start::Runtime::BakuAne2TextData
{
namespace
{
// Original 0x4194D0: mess_win_type is 0..1, stride 64; each record read from this game's PE.
// Name ASM 41992E reads +30 as DWORD: 1 = x - trunc(width/2), 2 = x - width.
// Keep wire value 1 (CenterAtAnchor); this is not Baku's bool or internal CenterInRegion (3).
constexpr std::array<TextWindowTextDescriptor, 2> Records{{
    // Original PE VA 0x555010; all 64 bytes retained in text-layout-profiles evidence.
    {{24, 700, 0, 12}, 214, 475, {26, 700, 0, 0}, 111, 477, 1, 1, 1, 10},
    // Original PE VA 0x555050; all 64 bytes retained in text-layout-profiles evidence.
    {{24, 700, 0, 12}, 214, 475, {26, 700, 0, 0}, 111, 477, 1, 1, 1, 10},
}};
}
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept { return Records; }
}
