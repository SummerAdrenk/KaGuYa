#pragma once
#include <Runtime/LegacyCharacterSelectionSource.h>
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
struct LegacyCharacterSelectionRootDependencies final
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
class LegacyCharacterSelectionRootSource final : public ILegacyCharacterSelectionSource
{
public:
    explicit LegacyCharacterSelectionRootSource(LegacyCharacterSelectionRootDependencies d) noexcept:d_(d){}
    ScriptParamsNativeBody& Params() override;
    ScriptNativeBody& Script() override;
    UnSaveDataRuntime& UnSave() override;
    SaveDataEnvironmentArchive& SaveData() override;
    SIZE ScreenSize() override;
    void SetCommand(const char*) override;
    void ClearCommand() override;
    bool Requested(LegacyCharacterSelectionAction) override;
    void CapturePreview() override;
    void PushSystemScene(LegacyCharacterSelectionAction) override;
    void QuickSave() override;
    bool QuickLoad() override;
    void QueueLoadAfter() override;
    void PostTitle() override;
    int Confirm(int,const char*,unsigned) override;
    void PrepareLayers() override;
    void FinishScene() override;
    void DrawBackgroundAndDialog(GuiDialogNativeBody5Ch&) override;
    bool AccelerateTransition() override;
    bool CancelPressed() override;
    TransitRuntime& Transition() override;
    void Present() override;
    void PlayBgmAndStopEffects(const char*) override;
    void PlaySelectionCue(const char*) override;
    void LoadAnimation(SpriteRuntime&,const char*,const char*) override;
    void CaptureMasoGauge(GraphicsSurfaceReference&) override;
private:
    RootRuntimePayload& Root();
    SceneManagerView Scenes();
    void CaptureTransition();
    LegacyCharacterSelectionRootDependencies d_;
};
}
