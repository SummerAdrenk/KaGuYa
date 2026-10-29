#include <Runtime/CommandSelectionNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/SoundManagerNativeRuntime.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ProgramControlSoundNativeGlobals.h>
#include <Runtime/GameBuildProfile.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
RootCommandSelectionSource::RootCommandSelectionSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment,
    ScriptNamedValueNativeTables names, IScrVmCommandErrorSink& errors, IRuntimeStringManagerSource& strings) noexcept
    : root_(root), environment_(environment), names_(names), errors_(errors), strings_(strings) {}

SIZE RootCommandSelectionSource::CanvasSize()
{
    const auto* environment = environment_.CaptureAt4180C0();
    return {std::bit_cast<std::int32_t>(environment->widthAt18), std::bit_cast<std::int32_t>(environment->heightAt1C)};
}
std::int32_t RootCommandSelectionSource::ReadAvailabilityParameter()
{
    root_.EnsureCreatedAt413BE0();
    auto& captured = root_.RequirePublished();
    const auto index = ResolveScriptNamedValueNativeAt498020("prog_param00", names_, strings_);
    return ReadScriptValueAt4C73A0(captured.ParamsAt47BDB0(), index, errors_);
}
void RootCommandSelectionSource::PlayCommandCue(bool confirm)
{
    auto* save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
    const auto volume = std::bit_cast<std::int32_t>(*save->BorrowSoundParamsAt1C().NormalEffectVolumeAt24());
    const auto* profile = CompiledGameProfile().commandSelection;
    const auto* name = profile ? (confirm ? profile->confirmCue : profile->hoverCue) : nullptr;
    if (!name) name = ProcessProgramControlSoundNative(confirm ? ProgramControlSoundNativeId::Confirm
        : ProgramControlSoundNativeId::Hover).GetString();
    root_.EnsureCreatedAt413BE0();
    auto* sound = root_.RequirePublished().PostWindow().SoundManagerAt24();
    (void)root_.NativeSoundServices().PlayCueAt4BCD70(sound, name, confirm ? 0u : 1u, volume, 1);
}
}
