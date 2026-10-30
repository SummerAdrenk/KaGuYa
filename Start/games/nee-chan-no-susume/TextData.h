#pragma once
#include <Runtime/TextWindowTextRuntime.h>
#include <span>

namespace KaGuYa::Start::Runtime::NeeChanTextData
{
std::span<const TextWindowTextDescriptor, 2> Descriptors() noexcept;
}
