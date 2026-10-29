#pragma once
#include <Runtime/SettingSceneNative.h>

namespace KaGuYa::Start::Runtime::SettingSceneNativeDetail
{
using Body = SettingSceneNativeBody20h;
using Table = SettingSceneNativeVtable4;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    const auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    // 4BA8A9..B6：先 stride 再 bias；control 空时仍读 bias，stride=0。
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
inline std::uint32_t& RegistryWord(void* registry, std::uint32_t offset) noexcept
{ return *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(registry) + offset); }
void RetireTask(Body&) noexcept;
}
