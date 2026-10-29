#include <Runtime/ReadScriptInteractionHostRuntime.h>

#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/GeneralConfirmationServicesRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/QuickLoadRuntime.h>
#include <Runtime/ReadScriptInputPredicatesRuntime.h>
#include <Runtime/ReadScriptSoundRequestsRuntime.h>
#include <Runtime/RollingSaveRuntime.h>
#include <Runtime/SaveLoadCaptureCoordinatorRuntime.h>
#include <Runtime/SceneManagerRuntime.h>

#include <utility>

namespace KaGuYa::Start::Runtime
{
ReadScriptInteractionHostRuntime::ReadScriptInteractionHostRuntime(
    ReadScriptInteractionHostDependencies dependencies) noexcept : dependencies_(dependencies)
{
}

void ReadScriptInteractionHostRuntime::ClearModalResultAt10()
{
    // 4B2896/4B11FD取root+3C对象，再写该对象DWORD+10。
    dependencies_.rootServiceDwordAt10 = 0;
}

void ReadScriptInteractionHostRuntime::ProcessSoundRequestsAt4A9090()
{
    dependencies_.soundRequests.ProcessSoundRequestsAt4A9090();
}

std::string ReadScriptInteractionHostRuntime::ReadPendingCommand()
{
    return std::string(dependencies_.scenes.ReadPendingCommandAt1C());
}

void ReadScriptInteractionHostRuntime::WritePendingCommand(std::string_view command)
{
    dependencies_.scenes.WritePendingCommandAt1C(command);
}

bool ReadScriptInteractionHostRuntime::ScriptHasWaitMarkerAt5C()
{
    // 4B2B6A ->418210，4B2B90..4B2BAB遍历action6的全部三byte binding。
    return dependencies_.input.HeldSkipKey();
}

void ReadScriptInteractionHostRuntime::BackupBeforeSaveLoadAt4B01A0()
{
    dependencies_.capture.CaptureBeforeSaveAt4B01A0();
}

void ReadScriptInteractionHostRuntime::CaptureQuickOperationAt4C1C60()
{
    CaptureRootQuickOperationFrame(dependencies_.transitionSource,
        dependencies_.surfaces, dependencies_.sizes);
}

void ReadScriptInteractionHostRuntime::QuickSaveAt485FC0(std::int32_t argument)
{
    dependencies_.rollingSave.QuickSaveAt485FC0(argument);
}

bool ReadScriptInteractionHostRuntime::QuickLoadAt485CE0()
{
    return dependencies_.quickLoad.QuickLoadAt485CE0();
}

void ReadScriptInteractionHostRuntime::ClearScenesAt4C1460()
{
    dependencies_.scenes.ClearScenes();
}

void ReadScriptInteractionHostRuntime::QueueSceneAt47F920(const char* command)
{
    dependencies_.scenes.QueueScene(command);
}

void ReadScriptInteractionHostRuntime::ActivateSceneAt4A9030(const char* command)
{
    dependencies_.scenes.PushActiveScene(command);
}

void ReadScriptInteractionHostRuntime::MarkRootSaveLoadAt1AF()
{
    dependencies_.rootByteAt1AF = 1;
}

std::unique_ptr<IReadScriptExitDialog> ReadScriptInteractionHostRuntime::CreateExitDialogAt4E3330()
{
    // 4B4183(mode1)：专用owner直接合成到dialog，不消费Quick的root+1F0。
    return dependencies_.confirmation.CreateExitDialogAt4E3330(
        { dependencies_.surfaces, dependencies_.sizes, dependencies_.transitionSource });
}

void ReadScriptInteractionHostRuntime::PostExitCommand8015()
{
    // 4B4334..4B4343：忽略BOOL，调用者之后仍写UnSave.Mode=0。
    static_cast<void>(::PostMessageA(dependencies_.window, WM_COMMAND, 0x8015, 0));
}

void ReadScriptInteractionHostRuntime::ShowPublicDialogAt4C1760()
{
    dependencies_.scenes.PopThenActivatePendingAt4C1760();
}

void ReadScriptInteractionHostRuntime::AutoSaveAt46B280(std::uint8_t argument)
{
    // 普通保存失败的正常返回与逃逸异常不同；B8/substate仍由Menu在返回后写入。
    dependencies_.rollingSave.AutoSaveAt46B280(argument);
}
}
