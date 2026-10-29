#pragma once
#include <Runtime/EditWindowNativeViewport.h>
#include <Runtime/WindowModeNative.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::EditWindowNativeViewportDetail
{
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }

template<class T> T* Resolve(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(slot.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
inline LONG Add(LONG a, LONG b) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(a) + std::uint32_t(b)); }
inline LONG Sub(LONG a, LONG b) noexcept
{ return std::bit_cast<LONG>(std::uint32_t(a) - std::uint32_t(b)); }

// 4878AE等：IMUL仅取低DWORD；EDX=0的DIV不是signed除法，也不是64位乘积除法。
// 不加noexcept：真实除零仍为硬件故障，不转换成C++异常。
inline LONG Scale(LONG factor, LONG value, std::uint32_t denominator)
{
    LONG result;
    __asm
    {
        mov eax, factor
        imul eax, value
        xor edx, edx
        div denominator
        mov result, eax
    }
    return result;
}

// 48806D已进入state0Fh，5E6DE0指向terminate；固定EDI而非回读slot.control。
inline void ReleaseNormally(YtlReferenceSlotRuntime& slot, EnvironmentObjectReferenceControl* edi) noexcept
{
    if (!edi) return;
    if (--edi->referenceCountAt04 == 0)
    {
        // 两次调用均读取固定EDI的当前表，不能因新建的final control而去虚拟化。
        const std::uintptr_t* table;
        if (edi->owningAt08)
        {
            using Destroy = void (__thiscall*)(YtlReferenceControlRuntime*);
            std::memcpy(&table, edi, sizeof(table));
            reinterpret_cast<Destroy>(table[0])(edi);
        }
        using Delete = void (__thiscall*)(YtlReferenceControlRuntime*, std::uint32_t);
        std::memcpy(&table, edi, sizeof(table));
        reinterpret_cast<Delete>(table[1])(edi, 1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}
struct TemporaryMargins final
{
    YtlReferenceSlotRuntime slot{};
    bool armed = true;
    // state0 -> 59A840 -> 4601F0：EH回读当前control；释放自身失败必须终止。
    ~TemporaryMargins() noexcept { if (armed) ReleaseYtlReference(slot); }
    TemporaryMargins() noexcept = default;
    TemporaryMargins(const TemporaryMargins&) = delete;
    TemporaryMargins& operator=(const TemporaryMargins&) = delete;
    void Finish(EnvironmentObjectReferenceControl* edi) noexcept
    { armed = false; ReleaseNormally(slot, edi); }
};
}
