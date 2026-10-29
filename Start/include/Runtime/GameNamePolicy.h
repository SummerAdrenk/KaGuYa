#pragma once
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
struct GameNameByteLimits final
{
    std::size_t given = 6;
    std::size_t family = 6;
};
}
