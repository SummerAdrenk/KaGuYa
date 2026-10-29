#pragma once

#include <Windows.h>
#include <span>
#include <Runtime/DateWindowNative.h>

namespace KaGuYa::Start::Runtime::EroZemiDateData
{
// 原 CP932 资源名；月份空槽必须保留，不能压缩为三个连续月份。
const char* Background() noexcept;
std::span<const char* const, 10> Digits() noexcept;
std::span<const char* const, 12> Months() noexcept;
std::span<const char* const, 7> Weekdays() noexcept;
std::span<const char* const, 3> Times() noexcept;
// 原表顺序：单个数字、两位日期的个位、两位日期的十位。
std::span<const POINT, 3> DayPositions() noexcept;
const DateWindowResourceProfile& Profile() noexcept;
}
