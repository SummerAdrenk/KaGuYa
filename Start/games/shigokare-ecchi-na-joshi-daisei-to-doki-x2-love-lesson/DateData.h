#pragma once

#include <Runtime/DateWindowNative.h>

namespace KaGuYa::Start::Runtime::ShigokareDateData
{
const char* Background() noexcept;
std::span<const char* const, 32> Dates() noexcept;
std::span<const char* const, 13> Months() noexcept;
std::span<const char* const, 7> Weekdays() noexcept;
std::span<const char* const, 2> Times() noexcept;
const DateWindowResourceProfile& Profile() noexcept;
}

