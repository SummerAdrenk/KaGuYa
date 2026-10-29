#pragma once

#include <Windows.h>

#include <array>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// Sample18只读元数据，不冒充原指针表ABI；每项固定容量包含NUL。
struct CountDownWindowNativeResourceProfile final
{
    std::array<std::array<char, 80>, 60> numberNamesCp932;
    std::array<std::array<char, 80>, 12> suffixNamesCp932;
    std::array<POINT, 6> numberPositions;
    std::array<POINT, 6> suffixPositions;
};
static_assert(std::is_standard_layout_v<CountDownWindowNativeResourceProfile>);
static_assert(std::is_trivially_copyable_v<CountDownWindowNativeResourceProfile>);
static_assert(std::is_trivially_destructible_v<CountDownWindowNativeResourceProfile>);

[[nodiscard]] const CountDownWindowNativeResourceProfile&
    Sample18CountDownWindowNativeResourceProfile() noexcept;
}
