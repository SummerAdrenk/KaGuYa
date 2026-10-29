#pragma once

#include <Runtime/ReadScriptSceneRuntime.h>
#include <Runtime/ReadScriptToolServicesRuntime.h>
#include <Runtime/ReadScriptLayerServicesRuntime.h>
#include <Runtime/ReadScriptInteractionStateSource.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class ScrVmValueTable;
class IReadScriptTextServices;

// 4B4164..4B430C 的模态对象必须跨 capture/run 保活；异常只做 C++ 析构，
// 不在析构中恢复 wait-mark 或工具条，因为原函数没有这样的 finally。
class IReadScriptExitDialog
{
public:
    virtual ~IReadScriptExitDialog() = default;
    virtual void CaptureDisplayAt48CA30() = 0;
    [[nodiscard]] virtual std::int32_t RunModalAt5071D0() = 0;
};

// 独立的声音/场景/模态边界，不提供默认成功或按整段子状态执行的回调。
// 生产 owner 必须把这些方法接到同一组环境、图层与 SceneController。
class IReadScriptTextInteractionHost
{
public:
    virtual ~IReadScriptTextInteractionHost() = default;
    virtual void ClearModalResultAt10() = 0;
    virtual void ProcessSoundRequestsAt4A9090() = 0;
    [[nodiscard]] virtual std::string ReadPendingCommand() = 0;
    virtual void WritePendingCommand(std::string_view command) = 0;
    // 历史接口名；原4B2B90的+5C是Keyboard action6区间，具体宿主须读取当前按住态。
    [[nodiscard]] virtual bool ScriptHasWaitMarkerAt5C() = 0;
    virtual void BackupBeforeSaveLoadAt4B01A0() = 0;
    virtual void CaptureQuickOperationAt4C1C60() = 0;
    virtual void QuickSaveAt485FC0(std::int32_t argument) = 0;
    [[nodiscard]] virtual bool QuickLoadAt485CE0() = 0;
    virtual void ClearScenesAt4C1460() = 0;
    virtual void QueueSceneAt47F920(const char* command) = 0;
    virtual void ActivateSceneAt4A9030(const char* command) = 0;
    virtual void MarkRootSaveLoadAt1AF() = 0;
    [[nodiscard]] virtual std::unique_ptr<IReadScriptExitDialog> CreateExitDialogAt4E3330() = 0;
    virtual void PostExitCommand8015() = 0;
    virtual void ShowPublicDialogAt4C1760() = 0;
};

// 4B2730 的全部 0..22 分支；同一 ReadScriptSceneState 由外层 4BA530 持有。
// 返回 true 仅表示外层同帧重调度，不是“本句完成”的统一标志。
class ReadScriptTextInteractionRuntime final
{
public:
    ReadScriptTextInteractionRuntime(IReadScriptTextInteractionHost& host, IReadScriptToolServices& tools,
        IReadScriptLayerServices& layers, IReadScriptInteractionStateSource& stateSource,
        IReadScriptTextServices& text) noexcept;
    ReadScriptTextInteractionRuntime(IReadScriptTextInteractionHost& host, IReadScriptToolServices& tools,
        IReadScriptLayerServices& layers, const ScrVmValueTable& params, SaveDataEnvironmentArchive& save,
        UnSaveDataRuntime& unsave, IReadScriptTextServices& text) noexcept;
    ReadScriptTextInteractionRuntime(const ReadScriptTextInteractionRuntime&) = delete;
    ReadScriptTextInteractionRuntime& operator=(const ReadScriptTextInteractionRuntime&) = delete;
    [[nodiscard]] bool RunAt4B2730(ReadScriptSceneState& state);

private:
    [[nodiscard]] bool RunRevealing(ReadScriptSceneState& state);
    [[nodiscard]] bool RunWaiting(ReadScriptSceneState& state);
    [[nodiscard]] bool RunCommand(ReadScriptSceneState& state);
    [[nodiscard]] bool SuspendForLayers(ReadScriptSceneState& state);
    void EnterWait(ReadScriptSceneState& state);
    void ToggleAuto();
    void EnableSkip();
    void StopForScene();
    [[nodiscard]] bool StartSaveLoad(ReadScriptSceneState& state, std::int32_t substate);
    [[noreturn]] void ThrowTextFailure(ReadScriptSceneState& state);

    IReadScriptTextInteractionHost& host_;
    IReadScriptToolServices& tools_;
    IReadScriptLayerServices& layers_;
    // 旧入口只借用既有正文；Source入口不构造固定适配器或任何dummy状态。
    std::optional<FixedReadScriptInteractionStateSource> fixedStateSource_;
    IReadScriptInteractionStateSource& stateSource_;
    IReadScriptTextServices& text_;
};
}
