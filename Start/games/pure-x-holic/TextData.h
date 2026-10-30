#pragma once
#include <Runtime/TextWindowTextRuntime.h>
#include <span>

namespace KaGuYa::Start::Runtime::PureHolicTextData
{
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept;
}
