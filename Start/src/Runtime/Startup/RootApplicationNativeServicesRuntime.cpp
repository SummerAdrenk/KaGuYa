#include <Runtime/RootApplicationNativeServicesRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SceneCueNativeGlobals.h>
#include <Runtime/ProgramControlSoundNativeGlobals.h>
#include <Runtime/SaveLoadConfirmDialogNativeGlobals.h>
#include <Runtime/SettingTagErrorRuntime.h>
#include <Runtime/ProcessWinHookRuntime.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/StartupResourceProfile.h>
#include "../../../games/baku-ane/ToolData.h"
#include "../../../games/baku-ane/WindowData.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
const ToolDialogNativeProfile& GameToolProfile()
{
    if (CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::Configured)
        return RequireConfiguredStartupResources(CompiledGameProfile().startupProfile).tool;
    return CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::BakuAne
        ? BakuToolData::Profile() : Sample18ToolDialogNativeProfile();
}
const SelectDialogNativeProfile& GameSelectionProfile()
{
    if (CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::Configured)
        return RequireConfiguredStartupResources(CompiledGameProfile().startupProfile).selection;
    return CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::BakuAne
        ? BakuWindowData::Selection() : Sample18SelectDialogNativeProfile();
}
const WaitMarkNativeProfile& GameWaitProfile()
{
    if (CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::Configured)
        return RequireConfiguredStartupResources(CompiledGameProfile().startupProfile).wait;
    return CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::BakuAne
        ? BakuWindowData::WaitMark() : MamatomoWaitMarkNativeProfile();
}
const OpacityDialogNativeProfile& GameOpacityProfile()
{
    if (CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::Configured)
        return RequireConfiguredStartupResources(CompiledGameProfile().startupProfile).opacity;
    return CompiledGameProfile().startupResources == GameBuildProfile::StartupResources::BakuAne
        ? BakuWindowData::Opacity() : MamatomoOpacityDialogNativeProfile();
}
}
RootApplicationNativeServicesRuntime::RootApplicationNativeServicesRuntime(
    RootApplicationNativeServiceDependencies d)
    : graphics_(d.graphics), archives_(d.root), names_(d.params), expressions_(parser_),
      scriptErrors_(d.environment, &::MessageBoxA), resourceErrors_(d.environment.RootSource()),
      pictureServices_{d.graphics, d.strings, d.graphics.RootSource(), d.graphics.Win32Source(),
          d.codecs, {}, ArchiveCodecInputMode::Raw},
      sprites_(d.root, d.archiveFiles, d.root.FileManagerSource(), resourceErrors_, pictureServices_, d.archiveTransaction),
      pictures_(d.root, d.environment, d.archiveFiles, d.root.FileManagerSource(), d.codecs,
          d.graphics, messages_, d.strings, d.archiveTransaction),
      draws_({d.root, d.environment, d.params, d.input, d.graphics, GameScriptNamedValueTables(),
          ProcessSettingTagErrorOwner(), expressions_, scriptErrors_, pictures_, sprites_, names_,
          GameToolProfile(), d.toolRelations, GameSelectionProfile(),
          d.countdownProfile, d.strings, d.memory, d.nativeCountdownProfile, GameWaitProfile(), GameOpacityProfile(),
          CompiledGameProfile().dateWindow, d.fiveFrameCountdownProfile, CompiledGameProfile().commandSelection,
          CompiledGameProfile().tokimekiGauge, CompiledGameProfile().loveHolicGauges}),
      inputUpdate_(d.input, d.environment, d.root, mousePosition_),
      preScene_(d.environment, d.params, d.root, d.input, GameScriptNamedValueTables(),
          &::MessageBoxA, &draws_.ToolInteraction(), d.strings),
      frame_(d.root, d.environment, inputUpdate_, preScene_, d.graphics.PresentCalls()),
      idle_(d.root, frame_),
      modal_(d.root, d.environment, d.input, d.graphics),
      edits_(d.root, d.environment.RootSource(), d.strings),
      auxiliary_({d.root, d.environment, d.params, d.input, d.graphics, pictures_, sprites_,
          d.persistenceFiles, d.environment, d.random, d.saveProfile, modal_.Services(),
          edits_.Source(), edits_.Viewport(), rectangles_, ProcessSaveLoadConfirmMaskNative(),
          ProcessSaveLoadConfirmFontNative(), d.strings, d.memory}),
      execution_({.root=d.root, .environment=d.environment, .params=d.params, .input=d.input,
          .graphics=d.graphics, .draws=draws_.DrawSource(), .effects=draws_.WindowEffects().effects,
          .archives=archives_, .files=d.archiveFiles, .key=d.root.FileManagerSource(), .codecs=d.codecs,
          .readers=readers_, .random=d.random, .errors=scriptErrors_, .transaction=d.archiveTransaction,
          .memory=d.memory, .strings=d.strings}),
      progressSource_(d.environment.RootSource(), d.nativeBase),
      progress_{d.endpoint, DefaultFileInfoProgressNativeApi(), progressSource_, d.strings,
          DefaultFileInfoProgressNativeConfirmation()},
      scenes_({.common={d.root, d.environment, d.input, d.graphics, d.archiveFiles,
          d.root.FileManagerSource(), d.codecs, messages_, d.strings, d.memory, d.archiveTransaction},
          .params=d.params, .sound=d.sound, .draws=draws_, .pictures=pictures_, .sprites=sprites_,
          .archives=archives_, .readers=readers_, .execution=execution_, .resourceNames=names_,
          .errors=scriptErrors_, .persistence=auxiliary_.Persistence(), .pages=auxiliary_.Pages(),
          .deletionConfirmation=auxiliary_.DeletionConfirmation(), .generalConfirmation=auxiliary_.GeneralConfirmation(),
          .nameRegistration=auxiliary_.NameRegistration(), .modal=modal_.Services(), .preview=auxiliary_.Preview(),
          .textFontFace=ProcessSceneFontFaceNativeCharsSlot(), .messageFiles=d.messageFiles,
          .messagePaths=d.environment, .voiceErrors=DefaultFileInfoProgressNativeConfirmation(), .paths=paths_,
          .hooks=ProcessWinHookRootNativeOwner(), .hookServices=ProcessWinHookNativeServices(),
          .titleCues={ProcessTitleBgmNative(), ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Confirm),
              ProcessProgramControlSoundNative(ProgramControlSoundNativeId::Hover)},
          .graffitiCues={ProcessGraffitiLikeCueNative(), ProcessGraffitiWeightCueNative(), ProcessGraffitiTrainingCueNative()}}),
      lifecycle_({d.root, {d.environment.RootSource(), d.params, paths_, d.persistenceFiles,
          d.persistenceTransaction, d.graphics, windows_, d.strings, d.memory, &::MessageBoxA},
          d.graphics, d.random, draws_, auxiliary_, scenes_, commands_, progress_, d.nativeBase.resourceInstanceAt08,
          &d.input.RootSource()})
{
    // 只完成正文外依赖，首次413BE0及4778B0/489C90仍由原应用调用点驱动。
    // 表达式Adapter仍显式复用现代解析核；本装配不扩大其已审计兼容范围。
}
}
