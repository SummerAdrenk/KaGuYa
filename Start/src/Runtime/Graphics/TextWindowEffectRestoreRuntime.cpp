#include <Runtime/TextWindowEffectRestoreRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
RuntimeCString* ReadMappingSlot(EnvironmentRootStorage& root, std::uint32_t port)
{
    auto* body = static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(root, 2));
    const auto begin = reinterpret_cast<std::uintptr_t>(static_cast<volatile UnSaveDataRuntime&>(*body).mappingsAt30_.begin);
    return reinterpret_cast<RuntimeCString*>(begin + port * 4U);
}
SoundMappingParameterReceiver ReadParameters(EnvironmentRootStorage& root)
{
    auto* body = static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(root, 1));
    return SoundMappingParameterReceiver::RawNative(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(body->soundParamsAt1C));
}
std::uint32_t FirstCount(const ParamsGameSystem* game)
{ return *static_cast<const volatile std::uint32_t*>(&game->soundSlotCounts[3]); }
}
TextEffectEnvironmentReceiver TextEffectEnvironmentReceiver::Native(EnvironmentRootStorage* root) noexcept
{
    return {root,
        [](void* p, std::uint32_t i) { return ReadMappingSlot(*static_cast<EnvironmentRootStorage*>(p), i); },
        [](void* p) { return ReadParameters(*static_cast<EnvironmentRootStorage*>(p)); }};
}
EnvironmentTextWindowEffectRestoreSource::EnvironmentTextWindowEffectRestoreSource(EnvironmentRootOwner& environment,
    ParamsRuntimeRootOwner& params, ITextWindowSoundSource& sound) noexcept
    : environment_(environment), params_(params), sound_(sound) {}
const ParamsGameSystem* EnvironmentTextWindowEffectRestoreSource::CaptureGameInlineAt4BC350()
{
    auto* root = params_.CaptureAt418480();
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(root->gameSystemAt04.Native());
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    const auto bias = ref.biasAt04;
    return reinterpret_cast<ParamsGameSystem*>(base + bias * stride);
}
TextEffectEnvironmentReceiver EnvironmentTextWindowEffectRestoreSource::CaptureEnvironmentAt4BC379()
{ return TextEffectEnvironmentReceiver::Native(environment_.CaptureAt4180C0()); }
TextSoundRootReceiver EnvironmentTextWindowEffectRestoreSource::CaptureRootAfter413BE0()
{ return sound_.CaptureRootAfter413BE0(); }
const char* EnvironmentTextWindowEffectRestoreSource::CaptureMappingNameVia417990(std::uint32_t port)
{ return ReadMappingSlot(*environment_.CaptureAt4180C0(), port)->GetString(); }
SoundMappingParameterReceiver EnvironmentTextWindowEffectRestoreSource::CaptureParametersVia417990()
{ return ReadParameters(*environment_.CaptureAt4180C0()); }
std::int32_t EnvironmentTextWindowEffectRestoreSource::MappingCountVia417990()
{
    auto* body = static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    const auto& values = static_cast<const volatile UnSaveDataRuntime&>(*body).mappingsAt30_;
    const auto end = reinterpret_cast<std::uintptr_t>(values.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(values.begin);
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(end - begin)) >> 2;
}
bool EnvironmentTextWindowEffectRestoreSource::MappingEmptyVia417990(std::uint32_t port)
{ return ReadMappingSlot(*environment_.CaptureAt4180C0(), port)->GetLength() == 0; }
const ParamsGameSystem* EnvironmentTextWindowEffectRestoreSource::CaptureGameGetterAt4BC4AE()
{
    auto* const root = params_.CaptureAt418480();
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(root->gameSystemAt04.Native());
    if (!ref.baseAt00) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    const auto bias = ref.biasAt04;
    const auto base = ref.baseAt00; // 4664D8/4664E3在算完bias后重新读base，不复用资格检查值。
    return reinterpret_cast<ParamsGameSystem*>(base + bias * stride);
}
void RestoreTextWindowEffectsAt4BC34B(ITextWindowEffectRestoreSource& source)
{
    std::uint32_t port = 0;
    const auto gate = [&]() -> SoundMappingParameterReceiver {
        for (;; ++port)
        {
            const auto count = FirstCount(source.CaptureGameInlineAt4BC350());
            const auto environment = source.CaptureEnvironmentAt4BC379();
            // 4BC379捕获早于比较；结束分支从这一固定Environment取slot1，而不是再捕获。
            if (std::bit_cast<std::int32_t>(port) >= std::bit_cast<std::int32_t>(count)) return environment.Parameters();
            if (environment.Mapping(port)->GetLength() == 0) continue;
            const auto voiceRoot = source.CaptureRootAfter413BE0();
            const auto* name = source.CaptureMappingNameVia417990(port);
            const auto restoreRoot = source.CaptureRootAfter413BE0();
            const bool any = voiceRoot.Resolve().effects.AnyVoiceSlotPlaying();
            restoreRoot.Resolve().effects.RestoreFirstMappedEffectRaw(port, name, any);
        }
    }();
    const bool loop = gate.HasLoopEffectType();
    const auto parameters = source.CaptureParametersVia417990();
    // 原fallback仅此一次；无second项也要执行getter并直接读取返回值。
    const auto fallback = loop ? parameters.LoopEffectValueOrThrow() : parameters.NormalEffectValueOrThrow();
    for (; std::bit_cast<std::int32_t>(port) < source.MappingCountVia417990(); ++port)
    {
        if (source.MappingEmptyVia417990(port)) continue;
        const auto* name = source.CaptureMappingNameVia417990(port);
        const auto root = source.CaptureRootAfter413BE0();
        const auto groupIndex = port - FirstCount(source.CaptureGameGetterAt4BC4AE());
        root.Resolve().effects.RestoreSecondMappedEffectRaw(groupIndex, name, fallback);
    }
}
}
