#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 18号原PE的指针表，不触发std::string/vector构造或按名称猜空槽。
std::span<const GuiStaticDescriptorNative24h> PopupNativeStaticDescriptorsSample18() noexcept;
std::span<const GuiCheckBoxDescriptorNative2Ch> PopupNativeCheckBoxDescriptorsSample18() noexcept;
}
