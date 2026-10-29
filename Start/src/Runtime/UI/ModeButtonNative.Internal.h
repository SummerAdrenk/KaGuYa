#pragma once
#include <Runtime/ModeButtonNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime::ModeButtonNativeDetail
{
using Body = ModeButtonNativeBody50h;
using Child = CgUiButtonNativeBody6Ch;
using Slot = YtlReferenceSlotRuntime;
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
inline std::int32_t Add(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) + std::uint32_t(b)); }
inline std::int32_t Sub(std::int32_t a, std::int32_t b) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(a) - std::uint32_t(b)); }
template<class T> std::int32_t Distance(T* first, T* last) noexcept
{
    return std::bit_cast<std::int32_t>(std::uint32_t(
        reinterpret_cast<std::uintptr_t>(last) - reinterpret_cast<std::uintptr_t>(first))) /
        static_cast<std::int32_t>(sizeof(T));
}
template<class T> T* Advance(T* value, std::int32_t index) noexcept
{
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(value) + std::uint32_t(index) * sizeof(T));
}
template<class T> T* Resolve(const Slot& slot, std::int32_t index = 0) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + (Read(slot.biasAt04) + std::uint32_t(index)) * stride);
}
inline std::int32_t ResourceCount(Child& child) noexcept
{
    // 原资源计数统一先mov end再sub begin，不依赖C++实参的求值次序。
    auto* last = Read(child.surfacesAt5C.end);
    auto* first = Read(child.surfacesAt5C.begin);
    return Distance(first, last);
}
inline std::int32_t ChildCount(Body& body) noexcept
{
    auto* last = Read(body.childrenAt14.end);
    auto* first = Read(body.childrenAt14.begin);
    return Distance(first, last);
}
RECT ChildRect(Child&);
ModeButtonNativeServices& Services();
MouseManagerNativeBody3Ch* CaptureMouse();
KeyboardManagerNativeBody438h* CaptureKeyboard();
}
