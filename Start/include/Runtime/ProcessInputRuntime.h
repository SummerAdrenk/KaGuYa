#pragma once
#include <Runtime/NativeInputRuntime.h>

namespace KaGuYa::Start::Runtime
{
InputRootOwner& ProcessInputRootOwner() noexcept;
InputNativeServices& ProcessInputNativeServices() noexcept;
NativeInputRuntime& ProcessNativeInputRuntime() noexcept;
InputRootNativeBody24h* CaptureProcessInputAt418210();
}
