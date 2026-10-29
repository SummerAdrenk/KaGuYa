#include <Runtime/ReadScriptAdvanceCommandGateNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootRuntimeOwner.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(void*) == 4);
constexpr char kSaveAt5AE218[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA";
constexpr char kLoadAt5AE224[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA";

std::uintptr_t InlineArchiveTwoAt4AA3E8(const EnvironmentRootStorage& environment)
{
    const volatile auto& array = environment.archivesAt30;
    // 此inline先end后begin，与417990相反；负差除12后仍按DWORD无符号比较。
    const auto end = reinterpret_cast<std::uintptr_t>(array.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(array.begin);
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end - begin)) / 12);
    if (count <= 2u) return 0;
    const auto* slot = reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin + 0x18u);
    const auto base = slot->baseAt00;
    if (!base) return 0;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot->controlAt08);
    // 4AA413先读stride，4AA416后读bias；空control也须读bias，DWORD乘加自然回绕。
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot->biasAt04;
    return base + bias * stride;
}

std::uint8_t ByteAt(std::uintptr_t body, std::uint32_t offset)
{
    // 空/非法Ref保留原读取故障，不以false或默认归档替代。
    return *reinterpret_cast<const volatile std::uint8_t*>(body + offset);
}
}

bool ReadScriptAdvanceCommandAllowedNativeAt4AA3E0(IReadScriptAdvanceCommandGateNativeSource& source)
{
    auto* environment = source.CaptureEnvironmentAt4180C0(0x4AA3E3);
    if (ByteAt(InlineArchiveTwoAt4AA3E8(*environment), 0x1C) & 2u) return false;
    environment = source.CaptureEnvironmentAt4180C0(0x4AA43A);
    const auto archive = reinterpret_cast<std::uintptr_t>(ResolveEnvironmentArchiveAt417990(*environment, 2));
    if (ByteAt(archive, 0x20) & 2u) return false;

    auto* root = CaptureReadScriptCommandRoot(source, 0x4AA455, 0x4AA45A);
    // managerCall标记原inline的首个Ref读，不声称原caller实际call了4186D0。
    if (CompareReadScriptCommandNative(source, root, 0x4AA45F, kSaveAt5AE218,
        ReadScriptCommandRelease::CachedHeader)) return false;
    root = CaptureReadScriptCommandRoot(source, 0x4AA4C5, 0x4AA4CA);
    return !CompareReadScriptCommandNative(source, root, 0x4AA4CF, kLoadAt5AE224,
        ReadScriptCommandRelease::CachedHeader);
}

RootReadScriptAdvanceCommandGateNativeSource::RootReadScriptAdvanceCommandGateNativeSource(
    RootRuntimeOwner& root, EnvironmentRootOwner& environment) noexcept
    : root_(root), environment_(environment) {}

EnvironmentRootStorage* RootReadScriptAdvanceCommandGateNativeSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{ return environment_.CaptureAt4180C0(); }

void RootReadScriptAdvanceCommandGateNativeSource::EnsureCommandRootAt413BE0(std::uint32_t)
{ root_.EnsureCreatedAt413BE0(); }

void* RootReadScriptAdvanceCommandGateNativeSource::CommandRootAt6198C4(std::uint32_t)
{ return root_.ReadPublishedAt6198C4(); }

SceneManagerNativeBody48h* RootReadScriptAdvanceCommandGateNativeSource::CommandManagerAt4186D0(
    void* capturedRoot, std::uint32_t)
{
    const volatile auto& slot = static_cast<RootRuntimePayload*>(capturedRoot)->SceneManagerSlotAt30();
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot.biasAt04;
    return reinterpret_cast<SceneManagerNativeBody48h*>(base + bias * stride);
}
}
