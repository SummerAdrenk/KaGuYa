#pragma once
#include <Runtime/SaveLoadPageSlotNative.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <bit>
#include <cstring>
#include <exception>

namespace KaGuYa::Start::Runtime::SaveLoadPageSlotNativeDetail
{
using Slot = YtlReferenceSlotRuntime;
union TextCell { RuntimeCString value; TextCell() noexcept {} ~TextCell() noexcept {} };
inline void ReleaseCached(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
struct RefLifetime final
{
    Slot& value;
    bool armed = true;
    ~RefLifetime() noexcept { if (armed) ReleaseYtlReference(value); }
    void FinishCached(YtlReferenceControlRuntime* control) noexcept
    { armed = false; ReleaseCached(control); }
    void FinishClearing() noexcept { armed = false; ReleaseYtlReference(value); }
    void FinishUncleared() noexcept
    {
        armed = false;
        auto* control = value.controlAt08;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            if (value.controlAt08) value.controlAt08->DeleteControlAtVslot04(1);
        }
    }
};
struct TextLifetime final
{
    RuntimeCString& value;
    bool armed = true;
    ~TextLifetime() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    void Finish() { armed = false; DestroyRuntimeCStringAt40CD40(&value); }
};
template<class T> T* Required(T* pointer) noexcept
{
    // 原空正文的首次解引用没有合法返回值；显式终止，不伪造空存档/成功。
    if (!pointer) std::terminate();
    return pointer;
}
template<class T> T* Resolve(const Slot& slot) noexcept
{
    const volatile auto& raw = slot;
    const auto base = raw.baseAt00;
    if (!base) return nullptr;
    const auto* control = raw.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0u;
    const auto bias = raw.biasAt04;
    return reinterpret_cast<T*>(base + bias * stride);
}
inline const Slot& At(const Slot* begin, std::uint32_t index) noexcept
{ return *reinterpret_cast<const Slot*>(reinterpret_cast<std::uintptr_t>(begin) + index * 12u); }
inline std::uint32_t Count(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = std::bit_cast<std::int32_t>(std::uint32_t(
        reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin)));
    return std::uint32_t(bytes / 12);
}
inline Slot Copy(const Slot& source) noexcept
{
    Slot out;
    CopyGraphicsSurfaceReferenceAt47D740(out, source);
    return out;
}
inline Slot CopyViaTemporary(const Slot& source) noexcept
{
    auto temporary = Copy(source);
    auto result = Copy(temporary);
    ReleaseCached(temporary.controlAt08);
    return result;
}
inline std::int32_t Signed(std::uint32_t value) noexcept { return std::bit_cast<std::int32_t>(value); }
inline SaveLoadDialogNativeBody184h& Parent(SaveLoadPageSecondNativeBodyACh& page) noexcept
{ return *Required(reinterpret_cast<SaveLoadDialogNativeBody184h*>(page.common.parentAt04)); }
inline constexpr POINT datePositions[13] = {
    {52,166},{64,166},{80,166},{94,166},{106,166},{122,166},{136,166},
    {148,166},{177,166},{189,166},{202,168},{209,166},{221,166}
};
inline constexpr SIZE previewSize{236,133}; // 原PE 5B6B58/5B6B5C。
void Blit(GraphicsSurfaceNativeBody*, GraphicsSurfaceNativeBody*, std::int32_t x,
    std::int32_t y, const SIZE*, SaveLoadPageSlotNativeServices&);
void DrawDateDigit(SaveLoadPageSecondNativeBodyACh&, const Slot& target,
    std::int32_t digit, std::int32_t saveMode, unsigned position, SaveLoadPageSlotNativeServices&);
}
