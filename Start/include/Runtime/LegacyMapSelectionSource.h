#pragma once

#include <Runtime/LegacyMapSelectionProfile.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/SpriteRuntime.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;
enum class LegacyMapSelectionAction { Save, Load, QuickSave, QuickLoad, Settings, Title, Likability };
class ILegacyMapSelectionSource
{
public:
    virtual ~ILegacyMapSelectionSource() = default;
    virtual ScriptParamsNativeBody& Params() = 0;
    virtual ScriptNativeBody& Script() = 0;
    virtual UnSaveDataRuntime& UnSave() = 0;
    virtual SaveDataEnvironmentArchive& SaveData() = 0;
    virtual SIZE ScreenSize() = 0;
    virtual bool CharacterDisplay(unsigned index) = 0;
    virtual std::uint16_t GlobalWord(unsigned index) = 0;
    virtual void SetCommand(const char*) = 0;
    virtual void ClearCommand() = 0;
    virtual bool Requested(LegacyMapSelectionAction) = 0;
    virtual void CapturePreview() = 0;
    virtual void PushSystemScene(LegacyMapSelectionAction) = 0;
    virtual void QuickSave() = 0;
    virtual bool QuickLoad() = 0;
    virtual void QueueLoadAfter() = 0;
    virtual void PostTitle() = 0;
    virtual int Confirm(int mode, unsigned flags) = 0;
    virtual void PrepareLayers() = 0;
    virtual void FinishScene() = 0;
    virtual bool AccelerateTransition() = 0;
    virtual bool EscapePressed() = 0;
    virtual bool RightPressed() = 0;
    virtual void ResetKeyboard() = 0;
    virtual TransitRuntime& Transition() = 0;
    virtual void Present() = 0;
    virtual void PlayBgmAndStopEffects(const char*) = 0;
    virtual void PlayCue(const LegacyMapSelectionCue&) = 0;
    // 04:4F3A10、05:4FBF90借动态archive source，主成员0/init1，不捕获catalog。
    virtual void LoadAnimation(SpriteRuntime&, const char* name) = 0;
    virtual void CaptureDate(GraphicsSurfaceReference&) = 0;
    virtual void CaptureMasoGauge(GraphicsSurfaceReference&) = 0;
};
const char* LegacyMapSelectionActionName(LegacyMapSelectionAction);
}
