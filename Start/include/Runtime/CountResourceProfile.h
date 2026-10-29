#pragma once
#include <Windows.h>
#include <array>
#include <cstdint>
#include <span>

namespace KaGuYa::Start::Runtime
{
enum class CountResourceKind : std::uint8_t { None, MultiGauge, LikableWeight };
struct CountVariableBinding final
{
    const char* variable;
    std::uint32_t frameIndex;
};
struct CountRowParameters final
{
    const char* selector;
    const char* delta;
    const char* upperLimit;
};
struct CountRowLayout final
{
    POINT gauge;
    POINT singleDigit;
    POINT ones;
    POINT tens;
    POINT up;
    POINT maximum;
};
struct CountMultiResourceData final
{
    std::span<const char* const> gauges;
    std::span<const char* const> icons;
    std::span<const char* const> digits;
    std::span<const CountVariableBinding> variables;
    std::array<CountRowParameters, 3> rows;
    std::array<CountRowLayout, 6> layouts;
    std::array<std::uint32_t, 4> layoutOffsets;
    const char* countVariable;
    const char* centerXVariable;
    const char* centerYVariable;
    const char* initialDelayVariable;
    const char* stepDelayVariable;
    const char* finalDelayVariable;
    const char* countCue;
};
struct CountResourceProfile final
{
    CountResourceKind kind;
    const CountMultiResourceData* multi;
};
const CountResourceProfile& NoCountResourceProfile() noexcept;
void ValidateCountResourceProfile(const CountResourceProfile&);
}
