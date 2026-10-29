#include <Runtime/MessageWindowNativeStateSource.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<bool StrideFirst, class T>
T* ResolveInlineReference(const YtlReferenceSlotRuntime& input) noexcept
{
    const volatile auto& slot = input;
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    std::uint32_t stride, bias;
    // 4192C1/419317先stride；4870CD先bias。volatile只约束原始字段读序，不增加校验。
    if constexpr (StrideFirst)
    {
        stride = control ? control->strideAt0C : 0u;
        bias = slot.biasAt04;
    }
    else
    {
        bias = slot.biasAt04;
        stride = control ? control->strideAt0C : 0u;
    }
    return reinterpret_cast<T*>(base + bias * stride);
}
template<bool StrideFirst>
IEnvironmentArchiveRuntime* ResolveInlineArchive(EnvironmentRootStorage* root, std::uint32_t index) noexcept
{
    // 419296/4870A6的内联版本先end后begin；不能与417994首读begin合并。
    const volatile auto& storage = root->archivesAt30;
    const auto end = reinterpret_cast<std::uintptr_t>(storage.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(storage.begin);
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(end - begin)) / 12);
    if (index >= count) return nullptr;
    const auto* reference = reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + index * 12u);
    return ResolveInlineReference<StrideFirst, IEnvironmentArchiveRuntime>(*reference);
}
}
MessageWindowNativeStateSource::MessageWindowNativeStateSource(IMessageWindowNativeRootSource& roots,
    EnvironmentRootOwner& environment, ScriptNamedValueNativeTables names,
    IRuntimeStringManagerSource& strings) noexcept
    : roots_(roots), environment_(environment), names_(names), strings_(strings) {}

MessageWindowNativeRootReceiver MessageWindowNativeStateSource::CaptureAfterEnsureAt413BE0()
    { return roots_.CaptureAfterEnsureAt413BE0(); }

std::uint8_t MessageWindowNativeStateSource::MessageWindowAllowedAt419290(DrawManagerNativeBody2Ch*)
{
    // 419290不读取传入ECX，但调用方取manager的副作用必须已经执行。
    auto* save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveInlineArchive<true>(environment_.CaptureAt4180C0(), 1));
    if ((save->bytesAt34[4] & 0x10u) == 0) return 1;
    auto* unsave = static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    if (!unsave->ArrayReferenceAt3C().StorageAt3C()) return 1;
    save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
    auto* const sound = ResolveInlineReference<true, EnvironmentSoundParamsArchive>(save->soundParamsAt1C);
    // 419326在最后一次环境Capture前固定vtable地址；+60函数槽在Capture后才读取。
    const void* table;
    std::memcpy(&table, sound, sizeof(table));
    unsave = static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    using VoiceAllowed = bool (__thiscall*)(EnvironmentSoundParamsArchive*, const UnSaveDataStringArrayReference&);
    VoiceAllowed callback;
    std::memcpy(&callback, static_cast<const std::byte*>(table) + 0x60, sizeof(callback));
    return callback(sound, unsave->ArrayReferenceAt3C()) ? 0u : 1u;
}

const IEnvironmentArchiveRuntime* MessageWindowNativeStateSource::CaptureSlot0At4180C0()
{
    return ResolveInlineArchive<false>(environment_.CaptureAt4180C0(), 0);
}
std::int32_t MessageWindowNativeStateSource::ResolveAt498020(
    const IEnvironmentArchiveRuntime*, const char* name)
{
    // 498020是固定名称表查询，不读取传入receiver；不为空receiver发明错误。
    return ResolveScriptNamedValueNativeAt498020(name, names_, strings_);
}
LayerItemNativeBody14h* MessageWindowNativeStateSource::CaptureTextWindow()
{
    const auto root = roots_.CaptureAfterEnsureAt413BE0();
    auto* const manager = root.drawManagerAt4175A0(root.object);
    const auto count = static_cast<std::uint32_t>(DrawLayerReferenceCount(manager->ownedLayersAt14));
    // 原418C10只拒绝index>count，==继续读取；不是现代checked LayerAt。
    if (3u > count) return nullptr;
    const auto address = reinterpret_cast<std::uintptr_t>(manager->ownedLayersAt14.begin) + 3u * 12u;
    return ResolveDrawLayerReference(*reinterpret_cast<const YtlReferenceSlotRuntime*>(address));
}
std::uintptr_t MessageWindowNativeStateSource::CaptureTextWindowNameBaseAt70()
{
    const auto address = reinterpret_cast<std::uintptr_t>(CaptureTextWindow()) + 0x70u;
    std::uintptr_t base;
    std::memcpy(&base, reinterpret_cast<const void*>(address), sizeof(base));
    return base;
}
const RECT* MessageWindowNativeStateSource::CaptureTextWindowNameBoundsAt46B558()
{
    return reinterpret_cast<const RECT*>(reinterpret_cast<std::uintptr_t>(CaptureTextWindow()) + 0x40u);
}
}
