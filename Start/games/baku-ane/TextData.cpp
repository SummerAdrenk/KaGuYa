#include "TextData.h"
#include <array>

namespace KaGuYa::Start::Runtime::BakuTextData
{
namespace
{
// 原41C440选择560010/560050；两条原64-byte记录相同，+30的1是区域居中bool。
constexpr TextWindowTextDescriptor Original{
    {24, 700, 0, 12}, 214, 475, {26, 700, 0, 0}, 36, 477, 1, 1, 1, 10};
constexpr auto Adapt()
{
    auto result = Original;
    // 41D30D只读低byte；内部3不是原wire值，不将旧bool误当新版锚点枚举1。
    result.nameAlignment = static_cast<std::int32_t>((Original.nameAlignment & 0xFF)
        ? TextWindowNameAlignment::CenterInRegion : TextWindowNameAlignment::Left);
    return result;
}
constexpr std::array<TextWindowTextDescriptor, 2> Records{Adapt(), Adapt()};
}
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept { return Records; }
}
