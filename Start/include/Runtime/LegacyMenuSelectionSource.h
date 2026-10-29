#pragma once
#include <Runtime/LegacyMenuSelectionNative.h>
#include <Runtime/GeneralConfirmDialogNative.h>
#include <Runtime/ReadScriptInteractionNativeHost.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/SceneManagerView.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/MessageWindowConfigExpressionParser.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootRuntimePayload;
class ParamsRuntimeRootOwner;
class InputRootOwner;
class IScrVmCommandErrorSink;
class IMessageWindowConfigExpressionParser;
class ISettingTagErrorSource;
class ISavePreviewCaptureServices;
struct LegacyMenuSelectionRootDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    GeneralConfirmDialogNativeServices& confirmation;
    IReadScriptInteractionNativeHost& interaction;
    ISavePreviewCaptureServices& preview;
    IScrVmCommandErrorSink& errors;
    IStartFileApi& messageFiles;
    const IMessageEnvironmentPathSource& messagePaths;
    const char* const& fontFace;
    ISettingTagErrorSource& tagErrors = ProcessSettingTagErrorOwner();
    ScriptNamedValueNativeTables names = GameScriptNamedValueTables();
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
    decltype(&::PostMessageA) postMessage = &::PostMessageA;
    decltype(&::MessageBoxA) messageBox = &::MessageBoxA;
};
// 只借同Root的既有生产服务；每次Params/消息/场景查询均重新取当前正文。
class LegacyMenuSelectionRootSource final : public ILegacyMenuSelectionSource
{
public:
    explicit LegacyMenuSelectionRootSource(LegacyMenuSelectionRootDependencies d) noexcept : d_(d) {}
    int ReadValue(const char*) override;
    void WriteValue(const char*, int) override;
    int ReadLabel(int, MessageFormatSelectionTarget) override;
    void MarkLabelRead(int) override;
    COLORREF ReadColor(const char*) override;
    const char* FontFace() override { return d_.fontFace; }
    UnSaveDataRuntime& UnSave() override;
    SaveDataEnvironmentArchive& SaveData() override;
    void SetCommand(const char*) override;
    bool Requested(LegacyMenuSelectionAction) override;
    void ClearCommand() override;
    void ResetScriptMenuState() override;
    void ClearModalResult() override;
    bool ShouldAutoSave() override;
    void CapturePreview() override;
    void AutoSave() override;
    void PushSystemScene(LegacyMenuSelectionAction) override;
    void QuickSave() override;
    bool QuickLoad() override;
    bool ConfirmTitle() override;
    void QueueLoadAfter() override;
    void PostTitle() override;
    void FinishScene() override;
    bool SuppressBackgroundUpdate() override;
    void DrawBackgroundAndMenu(GuiDialogNativeBody5Ch&) override;
private:
    RootRuntimePayload& Root();
    SceneManagerView Scenes();
    MessageOwnerNative34h& Messages();
    void CaptureTransition();
    LegacyMenuSelectionRootDependencies d_;
    MessageWindowConfigExpressionParser parser_;
};
const char* LegacyMenuSelectionSceneName() noexcept;
const char* LegacyMenuSelectionActionName(LegacyMenuSelectionAction);
}
