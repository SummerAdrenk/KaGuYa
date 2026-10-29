#pragma once
#include <Runtime/CgUiButtonNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::CgUiButtonNativeDetail
{
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
inline std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
inline std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
template<class T>T* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base=Read(slot.baseAt00);if(!base)return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    return reinterpret_cast<T*>(base+Read(slot.biasAt04)*stride);
}
inline std::int32_t Count(const GuiControlSurfaceReferences& vector) noexcept
{
    const auto end=reinterpret_cast<std::uintptr_t>(Read(vector.end));
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(vector.begin));
    return std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12;
}
inline std::int32_t StateSlot(CgUiButtonNativeBody6Ch& body,std::int32_t state) noexcept
{
    // 原状态索引未加7上限，地址算术保持DWORD回绕。
    const auto address=reinterpret_cast<std::uintptr_t>(&body)+0x40u+std::uint32_t(state)*4u;
    return Read(*reinterpret_cast<std::int32_t*>(address));
}
CgUiButtonNativeServices& Services();
}
