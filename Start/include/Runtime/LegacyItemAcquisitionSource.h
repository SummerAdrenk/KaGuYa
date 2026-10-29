#pragma once
#include <Runtime/LegacyItemAcquisitionNative.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/ReadScriptInteractionNativeHost.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/SceneManagerView.h>
namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;class RootRuntimePayload;class InputRootOwner;class ISavePreviewCaptureServices;class IScrVmCommandErrorSink;
struct LegacyItemAcquisitionRootDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    GeneralConfirmDialogNativeServices& confirmation;
    IReadScriptInteractionNativeHost& interaction;
    ISavePreviewCaptureServices& preview;
    IScrVmCommandErrorSink& errors;
    const LegacyItemAcquisitionResourceProfile* profile=nullptr;
    ScriptNamedValueNativeTables names=GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
};
class LegacyItemAcquisitionRootSource final:public ILegacyItemAcquisitionSource
{
public:
    explicit LegacyItemAcquisitionRootSource(LegacyItemAcquisitionRootDependencies d) noexcept:d_(d){}
    int Read(const char*) override;void Write(const char*,int) override;
    UnSaveDataRuntime& UnSave() override;SaveDataEnvironmentArchive& SaveData() override;
    SIZE CanvasSize() override;RECT SnapshotBounds(bool) override;void SnapshotLayer(bool,GraphicsSurfaceNativeBody&) override;
    void StartAudio(const char*) override;void SelectionCue(const char*) override;
    int Confirm(int,const YtlReferenceSlotRuntime&) override;
    int TransitionStep() override;bool AccelerateTransition() override;void RenderTransition(const TransitOperation&) override;
    void ResetScriptState() override;void ClearModalResult() override;void Command(const char*) override;
    bool Requested(LegacyItemAcquisitionAction) override;void CapturePreview() override;void Push(LegacyItemAcquisitionAction) override;
    void QuickSave() override;bool QuickLoad() override;bool ConfirmTitle() override;void QueueLoadAfter() override;void PostTitle() override;void Finish() override;
private:
    RootRuntimePayload& Root();SceneManagerView Scenes();void CaptureTransition();
    LegacyItemAcquisitionRootDependencies d_;
};
}
