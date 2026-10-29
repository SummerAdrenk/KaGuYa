#pragma once
#include <Runtime/LegacyMapSelectionSource.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/ReadScriptInteractionNativeHost.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimePayload;
class InputRootOwner;
class ISavePreviewCaptureServices;
class IScrVmCommandErrorSink;
struct LegacyMapSelectionRootDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    GeneralConfirmDialogNativeServices& confirmation;
    IReadScriptInteractionNativeHost& interaction;
    ISavePreviewCaptureServices& preview;
    IArchiveResourceSource& archives;
    IStartFileApi& files;
    IArchiveReaderMemoryApi& readers;
    const IEnvironmentKeyTableSource& key;
    ArchiveCodecServices codecs;
    IScrVmCommandErrorSink& errors;
    const FileTransactionContext* transaction=nullptr;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};
class LegacyMapSelectionRootSource final : public ILegacyMapSelectionSource
{
public:
    explicit LegacyMapSelectionRootSource(LegacyMapSelectionRootDependencies d) noexcept:d_(d){}
    ScriptParamsNativeBody& Params() override;
    ScriptNativeBody& Script() override;
    UnSaveDataRuntime& UnSave() override;
    SaveDataEnvironmentArchive& SaveData() override;
    SIZE ScreenSize() override;
    bool CharacterDisplay(unsigned) override;
    std::uint16_t GlobalWord(unsigned) override;
    void SetCommand(const char*) override;
    void ClearCommand() override;
    bool Requested(LegacyMapSelectionAction) override;
    void CapturePreview() override;
    void PushSystemScene(LegacyMapSelectionAction) override;
    void QuickSave() override;
    bool QuickLoad() override;
    void QueueLoadAfter() override;
    void PostTitle() override;
    int Confirm(int,unsigned) override;
    void PrepareLayers() override;
    void FinishScene() override;
    bool AccelerateTransition() override;
    bool EscapePressed() override;
    bool RightPressed() override;
    void ResetKeyboard() override;
    TransitRuntime& Transition() override;
    void Present() override;
    void PlayBgmAndStopEffects(const char*) override;
    void PlayCue(const LegacyMapSelectionCue&) override;
    void LoadAnimation(SpriteRuntime&,const char*) override;
    void CaptureDate(GraphicsSurfaceReference&) override;
    void CaptureMasoGauge(GraphicsSurfaceReference&) override;
private:
    RootRuntimePayload& Root();
    SceneManagerView Scenes();
    void CaptureTransition();
    void CreateSnapshot(GraphicsSurfaceReference&);
    LegacyMapSelectionRootDependencies d_;
};
}
