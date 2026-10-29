#pragma once
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 原Root+208/+20C/+210；构造跳过index，仅在原时点清尺寸，不添加猜测的默认索引。
struct RootMenuPositionNativeState0Ch final
{
    std::int32_t indexAt208;
    std::uint32_t widthAt20C;
    std::uint32_t heightAt210;
    RootMenuPositionNativeState0Ch()noexcept{}
};
static_assert(sizeof(RootMenuPositionNativeState0Ch)==0x0C&&std::is_trivially_destructible_v<RootMenuPositionNativeState0Ch>);
}
