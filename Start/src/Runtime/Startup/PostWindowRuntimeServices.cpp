#include <Runtime/PostWindowRuntimeServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeGraphicsRuntimeLibrary.h>
#include <Runtime/SoundRuntimeApi.h>
#include <Runtime/GuiModalDialogState.h>
#include <Runtime/PostWindowNativeDrawScope.h>

namespace KaGuYa::Start::Runtime
{
PostWindowRuntimeServices::PostWindowRuntimeServices(PostWindowRuntimeDependencies dependencies) noexcept
    : dependencies_(dependencies)
{ Post().BindNativeDrawServices(dependencies_.layers, dependencies_.layerRuntime, dependencies_.windowEffects, dependencies_.gui); }

EnvironmentPostWindowStartupRuntime& PostWindowRuntimeServices::Post() const noexcept
{ return dependencies_.root.PostWindow(); }

void PostWindowRuntimeServices::ConfirmFirstInstance()
{
    ConfirmNativeStartupSettingsWithResourceSlotAt489C90(dependencies_.resourceInstance, Post(), dependencies_.owner.Files());
}
void PostWindowRuntimeServices::CreateFileManager() { Post().CreateFileManager(); }
void PostWindowRuntimeServices::VerifyStartupFileSet() { Post().VerifyStartupFileSet(); }
void PostWindowRuntimeServices::InstallEnvironmentKeyTable() { Post().InstallEnvironmentKeyTable(); }
void PostWindowRuntimeServices::LoadSecretEnvironment() { Post().LoadSecretEnvironment(); }
void PostWindowRuntimeServices::ScanSaveFileCatalog() { Post().ScanSaveFileCatalog(); }
void PostWindowRuntimeServices::LoadGlobalEnvironment() { Post().LoadGlobalEnvironment(); }
void PostWindowRuntimeServices::InitializeGraphicsLibrary() { Post().InitializeGraphicsLibrary(dependencies_.graphics); }
void PostWindowRuntimeServices::CreateDrawManager()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    Post().CreateDrawManager(dependencies_.drawRandom, dependencies_.graphics);
}
void PostWindowRuntimeServices::CreateInformationDisplay()
{ dependencies_.information.Create(Post()); }
void PostWindowRuntimeServices::CreateLayerWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    auto& layers = dependencies_.layers;
    CreateAndAppendLayerWindowNativeAt48A6FF(dependencies_.drawSource,
        layers.layer, layers.surfaces, layers.strings, layers.memory);
}
void PostWindowRuntimeServices::CreateMessageWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendMessageWindowLayerNative(dependencies_.drawSource, dependencies_.layers.message);
}
void PostWindowRuntimeServices::CreateMessageWindowButtons()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendToolWindowNativeAt48A7F7(dependencies_.drawSource, dependencies_.layers.tool);
}
void PostWindowRuntimeServices::CreateTextWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendTextWindowLayerNativeAt48A873(dependencies_.drawSource, dependencies_.layers.text);
}
void PostWindowRuntimeServices::CreateWaitMark()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendWaitMarkNativeAt48A8F2(dependencies_.drawSource, dependencies_.layers.wait);
}
void PostWindowRuntimeServices::CreateSelectionWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendDateWindowNative(dependencies_.drawSource, dependencies_.layers.date, dependencies_.layers.memory);
    CreateAndAppendSelectWindowNativeAt48A96E(dependencies_.drawSource, dependencies_.layers.select);
}
void PostWindowRuntimeServices::CreateOpacityWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    auto& layers = dependencies_.layers;
    CreateAndAppendOpacityWindowAt48A9EC(dependencies_.drawSource,
        layers.opacityResources, layers.surfaces, layers.strings, layers.memory, layers.opacityProfile);
}
void PostWindowRuntimeServices::CreateCountdownWindow()
{
    PostWindowNativeDrawScope scope(dependencies_.layers, dependencies_.layerRuntime, &dependencies_.graphics, &dependencies_.gui);
    CreateAndAppendPrimaryGaugeNative(dependencies_.drawSource, dependencies_.layers.tokimeki,
        dependencies_.layers.loveHolic, dependencies_.layers.memory);
    CreateAndAppendCommandSelectionNative(dependencies_.drawSource, dependencies_.layers.command, dependencies_.layers.memory);
    CreateAndAppendMasoGaugeNative(dependencies_.drawSource, dependencies_.layers.loveHolic, dependencies_.layers.memory);
    CreateAndAppendCountDownWindowNativeAt48AA66(dependencies_.drawSource, dependencies_.layers.countdown);
}
void PostWindowRuntimeServices::InitializeSoundLibrary() { Post().InitializeSoundLibrary(Post().SoundServices().Library()); }
void PostWindowRuntimeServices::CreateSoundManager()
{
    Post().CreateSoundManager();
}
void PostWindowRuntimeServices::CreateInputAggregate()
{ Post().CreateInputAggregate(); }
void PostWindowRuntimeServices::CreateSceneManager()
{ dependencies_.scenes.Create(Post()); }
void PostWindowRuntimeServices::CreateFontManager() { Post().CreateFontManager(); }
void PostWindowRuntimeServices::ConfigureFrameTimer() { Post().ConfigureFrameTimer(dependencies_.root.Frame()); }
void PostWindowRuntimeServices::ResetRuntimeState() { Post().ResetRuntimeState(dependencies_.root.Frame()); }
void PostWindowRuntimeServices::QueueTitleLogoScene()
{
    dependencies_.owner.EnsureCreatedAt413BE0();
    auto* current = dependencies_.owner.ReadPublishedAt6198C4();
    current->PostWindow().QueueTitleLogoScene();
}
void PostWindowRuntimeServices::PostInitialWindowModeCommand()
{ Post().PostInitialWindowModeCommand(dependencies_.windowCommands); }
void PostWindowRuntimeServices::ReleaseInitializationProgress() noexcept { Post().ReleaseInitializationProgress(); }
void PostWindowRuntimeServices::ReleaseSceneManager() noexcept { Post().ReleaseSceneManager(); }
void PostWindowRuntimeServices::ReleaseInputAggregate() noexcept { Post().ReleaseInputAggregate(); }
void PostWindowRuntimeServices::ReleaseSoundManager() noexcept { Post().ReleaseSoundManager(); }
void PostWindowRuntimeServices::ShutdownSoundLibrary() { Post().SoundServices().Library().Shutdown(); }
void PostWindowRuntimeServices::ReleaseDrawManager() { Post().ReleaseDrawManager(); }
void PostWindowRuntimeServices::ReleaseDirectMovie()
{
    // 477769只清电影；不能释放整个root，4777A8还要读同一frame中的写回gate。
    dependencies_.root.ReleaseDirectMovie();
}
void PostWindowRuntimeServices::ShutdownGraphicsLibrary() { dependencies_.graphics.Shutdown(); }
void PostWindowRuntimeServices::PersistEnvironmentState() { Post().PersistEnvironmentState(); }
void PostWindowRuntimeServices::ReleaseFileManager() { Post().ReleaseFileManager(); }
}
