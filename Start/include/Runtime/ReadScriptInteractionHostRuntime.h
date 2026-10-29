#pragma once

#include <Runtime/ReadScriptMenuInteractionRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/RootTransitionFrameRuntime.h>
#include <Runtime/SceneManagerView.h>

#include <Windows.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class GeneralConfirmationServicesRuntime;
class IGraphicsScreenSizeApi;
class IGraphicsSurfaceApi;
class QuickLoadRuntime;
class ReadScriptInputPredicatesRuntime;
class ReadScriptSoundRequestsRuntime;
class RollingSaveRuntime;
class RuntimeGraphicsSurface;
class SaveLoadCaptureCoordinatorRuntime;
class SceneManagerRuntime;

// 全部借用同一根对象图；HWND和根字段必须是实时槽，不是构造时的副本。
// confirmation及rollingSave/quickLoad须来自同一root；Quick捕获和发布各自重新取根。
// 真实模态服务由根注入confirmation，host不另建消息泵或默认确认服务。
struct ReadScriptInteractionHostDependencies final
{
    ReadScriptSoundRequestsRuntime& soundRequests;
    SaveLoadCaptureCoordinatorRuntime& capture;
    RollingSaveRuntime& rollingSave;
    QuickLoadRuntime& quickLoad;
    SceneManagerView scenes;
    GeneralConfirmationServicesRuntime& confirmation;
    const ReadScriptInputPredicatesRuntime& input;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenSizeApi& sizes;
    DrawManagerLayerRuntime& manager;
    const HWND& window;
    std::uint32_t& rootServiceDwordAt10;
    std::uint8_t& rootByteAt1AF;
    IRootTransitionFrameSource& transitionSource;
};

// 4B2730/4B11B0共用的生产宿主。依赖须覆盖所有场景alias及嵌套模态的寿命；
// 不拥有根资源，不补调用者的Tool/WaitMark恢复、scene清理或异常回滚。
class ReadScriptInteractionHostRuntime final : public IReadScriptMenuInteractionHost
{
public:
    explicit ReadScriptInteractionHostRuntime(ReadScriptInteractionHostDependencies dependencies) noexcept;

    void ClearModalResultAt10() override;
    void ProcessSoundRequestsAt4A9090() override;
    [[nodiscard]] std::string ReadPendingCommand() override;
    void WritePendingCommand(std::string_view command) override;
    // 沿用接口名；原+5C属于Keyboard action6，绝非Script或WaitMark字段。
    [[nodiscard]] bool ScriptHasWaitMarkerAt5C() override;
    void BackupBeforeSaveLoadAt4B01A0() override;
    void CaptureQuickOperationAt4C1C60() override;
    void QuickSaveAt485FC0(std::int32_t argument) override;
    [[nodiscard]] bool QuickLoadAt485CE0() override;
    void ClearScenesAt4C1460() override;
    // 原pending保存裸名称指针；调用者须提供覆盖pending寿命的CP932名称。
    void QueueSceneAt47F920(const char* command) override;
    void ActivateSceneAt4A9030(const char* command) override;
    void MarkRootSaveLoadAt1AF() override;
    [[nodiscard]] std::unique_ptr<IReadScriptExitDialog> CreateExitDialogAt4E3330() override;
    // 原8015是异步返回标题，不是退出；窗口接收端由根负责47E180。
    void PostExitCommand8015() override;
    void ShowPublicDialogAt4C1760() override;
    void AutoSaveAt46B280(std::uint8_t argument) override;

private:
    ReadScriptInteractionHostDependencies dependencies_;
};
}
