#include <Runtime/TextWindowNativeMaintenance.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/TextWindowNativePlayback.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uintptr_t ResolveInlineReference(const volatile YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base = slot.baseAt00;
    if (!base) return 0;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    // 各内联Ref均先control/stride、后bias；control空也必须读bias，不检查count。
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot.biasAt04;
    return base + bias * stride;
}

template<class T, bool EndFirst = false>
T* ResolveInlineArchive(EnvironmentRootStorage* root, std::uint32_t index) noexcept
{
    const volatile auto& slots = root->archivesAt30;
    std::uintptr_t begin, end;
    if constexpr (EndFirst)
    {
        end = reinterpret_cast<std::uintptr_t>(slots.end);
        begin = reinterpret_cast<std::uintptr_t>(slots.begin);
    }
    else
    {
        begin = reinterpret_cast<std::uintptr_t>(slots.begin);
        end = reinterpret_cast<std::uintptr_t>(slots.end);
    }
    // 原IMUL/SAR修正是signed差除12，再作unsigned比较；不校验capacity或指针关系。
    const auto count = static_cast<std::uint32_t>(
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(end - begin)) / 12);
    if (index >= count) return nullptr;
    const auto* slot = reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin + index * 12u);
    return reinterpret_cast<T*>(ResolveInlineReference(*slot));
}

bool ShouldPrepare(ITextWindowNativeMaintenanceSource& source)
{
    // 首段4C0B98先end再begin；其余四段4C0CEF/4C0E3E/4C0FA4/4C10FE先begin再end。
    auto* const save = ResolveInlineArchive<SaveDataEnvironmentArchive, true>(
        source.CaptureEnvironmentInline(0x4C0A9A), 1);
    if (static_cast<const volatile SaveDataEnvironmentArchive*>(save)->valueAt81 == 0) return false;

    auto* const present = ResolveInlineArchive<UnSaveDataRuntime>(
        source.CaptureEnvironmentInline(0x4C0BEB), 2);
    const auto* presentSlot = reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(present) + 0x3Cu);
    if (presentSlot->baseAt00 == 0) return false;

    auto* const parameterSave = ResolveInlineArchive<SaveDataEnvironmentArchive>(
        source.CaptureEnvironmentInline(0x4C0D3A), 1);
    const auto parameters = ResolveInlineReference(parameterSave->soundParamsAt1C);

    auto* const voices = ResolveInlineArchive<UnSaveDataRuntime>(
        source.CaptureEnvironmentInline(0x4C0EA0), 2);
    // 4C0FE5在voices捕获之后才读固定参数正文的当前vptr，不复用4BC646的提前缓存receiver。
    const auto gate = TextSoundVoiceGateTableReceiver::Native(
        reinterpret_cast<EnvironmentSoundParamsArchive*>(parameters));
    const auto* names = reinterpret_cast<const YtlReferenceSlotRuntime*>(
        reinterpret_cast<std::uintptr_t>(voices) + 0x3Cu);
    if (!gate.Allowed(names)) return false;

    auto* const flags = ResolveInlineArchive<UnSaveDataRuntime>(
        source.CaptureEnvironmentInline(0x4C0FFA), 2);
    // 4C113F只测+18的低BYTE bit1，不能扩大为DWORD或把非零都当作禁止。
    if ((*reinterpret_cast<const volatile std::uint8_t*>(
        reinterpret_cast<std::uintptr_t>(flags) + 0x18u) & 2u) != 0) return false;

    const auto root = source.CaptureSoundRootAfter413BE0(0x4C114A);
    const auto manager = root.Resolve();
    return !manager.effects.AnyVoiceSlotPlaying();
}
}

RootTextWindowNativeMaintenanceSource::RootTextWindowNativeMaintenanceSource(
    EnvironmentRootOwner& environment, ITextWindowSoundSource& sound,
    ITextWindowNativePlaybackServices& playback) noexcept
    : environment_(environment), sound_(sound), playback_(playback) {}

EnvironmentRootStorage* RootTextWindowNativeMaintenanceSource::CaptureEnvironmentInline(std::uint32_t)
{
    // 五组new60/两次GetNil/发布及EH资格与4180C0对应，热路径同样Enter/Leave后重读发布槽。
    return environment_.CaptureAt4180C0();
}

TextSoundRootReceiver RootTextWindowNativeMaintenanceSource::CaptureSoundRootAfter413BE0(std::uint32_t)
{
    return sound_.CaptureRootAfter413BE0();
}

bool RootTextWindowNativeMaintenanceSource::PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h& body)
{
    return playback_.PreparePlaybackAt4C2A40(body);
}

void MaintainTextWindowVoiceNativeAt4C0A70(
    TextWindowLayerNativeBodyE8h& body, ITextWindowNativeMaintenanceSource& source)
{
    if (ShouldPrepare(source)) static_cast<void>(source.PreparePlaybackAt4C2A40(body));
    // 4C1186为普通尾段，不是异常清理；Prepare两种正常返回都重取root/manager并Refresh。
    const auto root = source.CaptureSoundRootAfter413BE0(0x4C118B);
    const auto manager = root.Resolve();
    manager.volume.Refresh();
}
}
