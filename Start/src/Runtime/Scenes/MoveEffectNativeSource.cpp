#include <Runtime/MoveEffectNativeConstruction.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
RootMoveEffectNativeConstructionSource::RootMoveEffectNativeConstructionSource(
    RootRuntimeOwner& root, ParamsRuntimeRootOwner& params, EnvironmentRootOwner& environment,
    InputRootOwner& input, IDrawManagerNativeRootSource& draw, IGraphicsScreenSizeApi& screen) noexcept
    : root_(root), params_(params), environment_(environment), input_(input), draw_(draw), screen_(screen) {}
SIZE RootMoveEffectNativeConstructionSource::ReadScreenSizeAt47CC00()
    { return screen_.ReadScreenSizeAt47CC00(); }
std::int32_t RootMoveEffectNativeConstructionSource::ReadLiveGameSystemCountAt49C282()
{
    auto* const params = params_.CaptureAt418480();
    auto* const game = ResolveEnvironmentObjectReference<ParamsGameSystem>(params->gameSystemAt04.Native());
    const auto* const control = static_cast<EnvironmentObjectReferenceControl*>(game->startupConfigBytes.storage.RawSlot().controlAt08);
    return control ? std::bit_cast<std::int32_t>(control->countAt10) : 0;
}
DrawManagerNativeBody2Ch* RootMoveEffectNativeConstructionSource::CaptureDrawManagerAfter413BE0()
    { return draw_.CaptureDrawManagerAfter413BE0(); }
ScriptNativeBody* RootMoveEffectNativeConstructionSource::CaptureScriptAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.Published()->ScriptAt47CD00();
}
EnvironmentRootStorage* RootMoveEffectNativeConstructionSource::CaptureProgramAt4180C0()
    { return environment_.CaptureAt4180C0(); }
std::uint32_t RootMoveEffectNativeConstructionSource::ReadSpeedIndexAt49C86B()
{
    auto* const environment = environment_.CaptureAt4180C0();
    // 49C81F先begin后end；内联slot1要求count>1，不套用inclusive的417990。
    auto* const begin = environment->archivesAt30.begin;
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(environment->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(begin));
    const auto count = std::bit_cast<std::int32_t>(bytes) / 12;
    auto* const save = static_cast<std::uint32_t>(count) > 1 ?
        ResolveEnvironmentObjectReference<SaveDataEnvironmentArchive>(begin[1]) : nullptr;
    return save->valueAt70;
}
bool RootMoveEffectNativeConstructionSource::SuppressSnapshotAt49CDC7()
{
    static_cast<void>(ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 5));
    auto* const keyboard = ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input_.CaptureAt418210()->keyboardAt18);
    const auto& descriptor = keyboard->actionsAt14[6];
    auto at = reinterpret_cast<std::uintptr_t>(descriptor.begin);
    const auto end = reinterpret_cast<std::uintptr_t>(descriptor.end);
    if (at != end)
    {
        const auto buffer = keyboard->bufferAt10;
        do
        {
            if (*reinterpret_cast<const std::uint8_t*>(buffer + at + 1u)) return true;
            at += 3;
        } while (at != end);
    }
    const auto* const unsave = static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
    if ((*reinterpret_cast<const volatile std::uint8_t*>(&unsave->flagsAt14_) & 2) != 0) return true;
    const auto* const save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
    return save->valueAt6F == 0;
}
}
