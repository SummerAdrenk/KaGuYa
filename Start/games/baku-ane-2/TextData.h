#pragma once
#include <Runtime/TextWindowTextRuntime.h>
#include <span>

namespace KaGuYa::Start::Runtime::BakuAne2TextData
{
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept;
}
