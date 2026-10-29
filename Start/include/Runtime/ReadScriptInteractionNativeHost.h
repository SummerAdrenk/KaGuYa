#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 原生BCh实际调用的Root交互叶。命令CString与raw84确认已由原生caller负责，
// 不暴露旧std::string/unique_ptr兼容口，也不在缺失时改用旧宿主。
class IReadScriptInteractionNativeHost
{
public:
    virtual ~IReadScriptInteractionNativeHost()=default;
    virtual void ClearModalResultAt10()=0;
    virtual void ProcessSoundRequestsAt4A9090()=0;
    virtual bool ScriptHasWaitMarkerAt5C()=0;
    virtual void BackupBeforeSaveLoadAt4B01A0()=0;
    virtual void CaptureQuickOperationAt4C1C60()=0;
    virtual void QuickSaveAt485FC0(std::int32_t argument)=0;
    virtual bool QuickLoadAt485CE0()=0;
    virtual void ClearScenesAt4C1460()=0;
    virtual void QueueSceneAt47F920(const char*)=0;
    virtual void ActivateSceneAt4A9030(const char*)=0;
    virtual void MarkRootSaveLoadAt1AF()=0;
    virtual void ShowPublicDialogAt4C1760()=0;
    virtual void AutoSaveAt46B280(std::uint8_t)=0;
    virtual void CaptureToolAt4C01F0()=0;
    virtual void SetToolCaptureModeAt4C2300(bool)=0;
    virtual void RestoreToolAt4BF4E0()=0;
    virtual std::uint8_t ReadWaitMarkEnabledAt0C()=0;
    virtual void WriteWaitMarkEnabledAt0C(std::uint8_t)=0;
};
}
