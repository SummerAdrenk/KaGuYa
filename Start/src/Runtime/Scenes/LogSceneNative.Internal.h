#pragma once
#include <Runtime/LogSceneNative.h>

namespace KaGuYa::Start::Runtime::LogSceneNativeDetail
{
using Body=LogSceneNativeBody20h;using Common=GuiDialogNativeBody5Ch;using Slot=YtlReferenceSlotRuntime;
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
template<class T>T* Resolve(const Slot& slot) noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    const auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(slot.biasAt04)*stride);
}
inline std::uint32_t& RegistryValue(void* p) noexcept{return *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(p)+4);}
void RetireTask(Body&) noexcept;
}
