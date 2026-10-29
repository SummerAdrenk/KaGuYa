#pragma once

#include <Runtime/RootOptionCharSelectNativeSource.h>

namespace KaGuYa::Start::Runtime::RootOptionCharSelectNativeDetail
{
// 仅用于原指令逐点读字段。DWORD 运算回绕；空正文不补默认值或可恢复异常。
template<class T> T ReadAt(const void* body, std::uint32_t offset) noexcept
{
    const auto address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(body)) + offset;
    return *reinterpret_cast<const volatile T*>(address);
}
inline std::uintptr_t ResolveWithCapturedBase(const YtlReferenceSlotRuntime* slot,
    std::uintptr_t base) noexcept
{
    if (!base) return 0;
    const auto* control = ReadAt<YtlReferenceControlRuntime*>(slot, 8);
    const auto stride = control ? ReadAt<std::uint32_t>(control, 0x0C) : 0u;
    const auto bias = ReadAt<std::uint32_t>(slot, 4);
    return base + bias * stride;
}
GuiClickedEventReferences* NewManager(IEnvironmentRootMemory& memory);
void AdoptManager(YtlReferenceSlotRuntime& destination, GuiClickedEventReferences* owned,
    IEnvironmentRootMemory& memory);
std::int32_t PollManager(GuiClickedEventReferences* manager, std::int32_t clearMatched);
}
