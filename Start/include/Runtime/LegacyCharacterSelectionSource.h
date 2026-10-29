#pragma once
#include <Runtime/LegacyCharacterSelectionProfile.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SpriteRuntime.h>
#include <Runtime/TransitRuntime.h>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class LegacyCharacterSelectionAction { Save, Load, QuickSave, QuickLoad, Settings, Title, Likability };
class ILegacyCharacterSelectionSource
{
public:
    virtual ~ILegacyCharacterSelectionSource() = default;
    virtual ScriptParamsNativeBody& Params() = 0;
    virtual ScriptNativeBody& Script() = 0;
    virtual UnSaveDataRuntime& UnSave() = 0;
    virtual SaveDataEnvironmentArchive& SaveData() = 0;
    virtual SIZE ScreenSize() = 0;
    virtual void SetCommand(const char*) = 0;
    virtual void ClearCommand() = 0;
    virtual bool Requested(LegacyCharacterSelectionAction) = 0;
    virtual void CapturePreview() = 0;
    virtual void PushSystemScene(LegacyCharacterSelectionAction) = 0;
    virtual void QuickSave() = 0;
    virtual bool QuickLoad() = 0;
    virtual void QueueLoadAfter() = 0;
    virtual void PostTitle() = 0;
    // 使用既有GeneralConfirm；返回原结果6/7，不把失败当取消或接受。
    virtual int Confirm(int mode, const char* fallbackCp932, unsigned fallbackFlags) = 0;
    virtual void PrepareLayers() = 0;
    virtual void FinishScene() = 0;
    virtual void DrawBackgroundAndDialog(GuiDialogNativeBody5Ch&) = 0;
    virtual bool AccelerateTransition() = 0;
    virtual bool CancelPressed() = 0;
    virtual TransitRuntime& Transition() = 0;
    virtual void Present() = 0;
    virtual void PlayBgmAndStopEffects(const char*) = 0;
    virtual void PlaySelectionCue(const char*) = 0;
    // 资源叶保留AN/PIC解析器与现有魔素层，不接管选择/扣费业务。
    virtual void LoadAnimation(SpriteRuntime&, const char* commands, const char* pictures) = 0;
    virtual void CaptureMasoGauge(GraphicsSurfaceReference&) = 0;
};
const char* LegacyCharacterSelectionActionName(LegacyCharacterSelectionAction);
}
