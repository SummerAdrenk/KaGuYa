#pragma once
#include <Runtime/MainFrameSettingsNative.h>
#include <Runtime/RuntimeCStringVector.h>
#include <Runtime/SettingsLayerRefreshNative.h>
#include "SettingDialogNativeApply.Internal.h"

namespace KaGuYa::Start::Runtime::MainFrameSettingsNativeDetail
{
using Slot = YtlReferenceSlotRuntime;
using Vector = EnvironmentVectorStorage<RuntimeCString>;
using SettingDialogNativeApplyDetail::Read;
using SettingDialogNativeApplyDetail::Field;
using SettingDialogNativeApplyDetail::RootSound;
using SettingDialogNativeApplyDetail::RootDraw;
using SettingDialogNativeApplyDetail::GameSystem;

enum class RegistryOrder { InlineStrideFirst, InlineBiasFirst, Function417990 };
void* Registry(MainFrameSettingsNativeServices&, unsigned index, unsigned site,
    RegistryOrder = RegistryOrder::InlineStrideFirst, bool lockedInline = false);
void* Root(MainFrameSettingsNativeServices&, unsigned site);
template<class T> T* Resolve(const Slot& slot, bool biasFirst = false) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* const control = Read(slot.controlAt08);
    std::uint32_t bias, stride;
    if (biasFirst)
    {
        bias = Read(slot.biasAt04);
        stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 0x0C) : 0u;
    }
    else
    {
        stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 0x0C) : 0u;
        bias = Read(slot.biasAt04);
    }
    return reinterpret_cast<T*>(base + bias * stride);
}
inline RuntimeCString* At(RuntimeCString* begin, unsigned index) noexcept
{ return reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(begin) + index * 4u); }
inline std::uint32_t Count(const Vector& vector) noexcept
{
    const auto end = reinterpret_cast<std::uintptr_t>(Read(vector.end));
    const auto begin = reinterpret_cast<std::uintptr_t>(Read(vector.begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(end - begin) >> 2);
}
// 原栈拥有对象，无自动CString/33Ch析构；qualification在caller中逐点发布。
struct Locals final
{
    Vector mappings;
    union { RuntimeCString first; };
    union { RuntimeCString second; };
    DWORD color44, color40, text64, text68;
    BYTE text60, text61;
    union { GamePropertyNativeBody33Ch property; };
    Locals() noexcept {}
    ~Locals() noexcept {}
};
void CaptureSnapshot(Locals&, MainFrameSettingsNativeServices&);
void RestoreSounds(Locals&, MainFrameSettingsNativeServices&);
void ApplyPolicyAndLayers(Locals&, MainFrameSettingsNativeServices&);
}
