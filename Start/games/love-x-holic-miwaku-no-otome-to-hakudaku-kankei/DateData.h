#pragma once

#include <Runtime/DateWindowNative.h>

namespace KaGuYa::Start::Runtime::LoveHolicDateData
{
const char* Background() noexcept;
std::span<const char* const, 60> Days() noexcept;
std::span<const char* const, 2> Times() noexcept;
const DateWindowResourceProfile& Profile() noexcept;
}

