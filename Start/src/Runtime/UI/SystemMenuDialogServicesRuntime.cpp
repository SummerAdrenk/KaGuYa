#include <Runtime/SystemMenuDialogServicesRuntime.h>
#include <Runtime/WindowEnvironmentRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>
#include <Runtime/WindowModeCommandsRuntime.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentPostWindowStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SystemMenuDialogServicesRuntime::SystemMenuDialogServicesRuntime(const WindowEnvironmentRuntime& environment,
    const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
    const std::shared_ptr<SceneManagerRuntime>& sceneSlot, const std::uint8_t& settingsEnabledAtRoot219,
    IModalDialogRunnerServices& modal, IRootLogicalRectanglePlatformApi& windowApi,
    IWindowModeCommandPlatformApi& messages) noexcept
    : environment_(&environment), modeSlot_(&modeSlot), sceneSlot_(&sceneSlot),
      settingsEnabledAtRoot219_(&settingsEnabledAtRoot219), modal_(modal), windowApi_(windowApi),
      messages_(messages), rectangle_(environment, modeSlot, *this, windowApi) {}

SystemMenuDialogServicesRuntime::SystemMenuDialogServicesRuntime(const EnvironmentStartupRuntime& environment,
    const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
    const EnvironmentPostWindowStartupRuntime& postWindow, const std::uint8_t& settingsEnabledAtRoot219,
    IModalDialogRunnerServices& modal, IRootLogicalRectanglePlatformApi& windowApi,
    IWindowModeCommandPlatformApi& messages) noexcept
    : startupEnvironment_(&environment), uniqueModeSlot_(&modeSlot), postWindow_(&postWindow),
      settingsEnabledAtRoot219_(&settingsEnabledAtRoot219), modal_(modal), windowApi_(windowApi),
      messages_(messages), rectangle_(environment, modeSlot, *this, windowApi) {}

SystemMenuDialogServicesRuntime::SystemMenuDialogServicesRuntime(const EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, IModalDialogRunnerServices& modal,
    IRootLogicalRectanglePlatformApi& windowApi, IWindowModeCommandPlatformApi& messages) noexcept
    : startupEnvironment_(&environment), root_(&root), modal_(modal), windowApi_(windowApi),
      messages_(messages), rectangle_(environment, root, *this, windowApi) {}

HWND SystemMenuDialogServicesRuntime::CurrentWindow() const
{
    // 50956C/50958A/5095DA各自晚取Environment+2C；只有478E50内部会保存本次HWND。
    return startupEnvironment_ ? reinterpret_cast<HWND>(startupEnvironment_->CaptureAt4180C0().windowAt2C) : environment_->window;
}

void SystemMenuDialogServicesRuntime::SynchronizeRuntimeAt413BE0() { modal_.SynchronizeRuntimeAt413BE0(); }
std::uint8_t SystemMenuDialogServicesRuntime::ReadCurrentWindowModeAt04()
{
    if (root_) return root_->RequirePublished().WindowModeAt0C().ModeAt04();
    const auto* mode = uniqueModeSlot_ ? uniqueModeSlot_->get() : modeSlot_->get();
    if (!mode) throw std::logic_error("50F990 dereferenced an empty window mode slot");
    return mode->ModeAt04();
}
std::uint8_t SystemMenuDialogServicesRuntime::ReadSettingsEnabledAtRoot219()
{
    return root_ ? root_->RequirePublished().ReadByteAt219() : *settingsEnabledAtRoot219_;
}
bool SystemMenuDialogServicesRuntime::IsGameWindowIconic() { return windowApi_.IsIconic(CurrentWindow()) != FALSE; }
RECT SystemMenuDialogServicesRuntime::ReadRootLogicalRectangleAt478E50()
{
    RECT result;
    return rectangle_.QueryAt478E50(result);
}
void SystemMenuDialogServicesRuntime::PostGameWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    // 509550 为异步 PostMessageA，失败返回未分支，不能改为同步 Dispatch 或业务异常。
    static_cast<void>(messages_.PostWindowMessage(CurrentWindow(), message, wParam, lParam));
}
void SystemMenuDialogServicesRuntime::WriteSceneManagerPendingCommandAt1C(std::string_view nameCp932)
{
    // 5095B8->4186D0解析root+30，5095C7只写manager+1C，不触发SceneController入栈/切场景。
    const auto scene = root_ ? root_->RequirePublished().PostWindow().BorrowSceneManagerAt30()
        : postWindow_ ? postWindow_->BorrowSceneManagerAt30() : SceneManagerView(sceneSlot_->get());
    if (!scene.IsNative()&&!scene) throw std::logic_error("5095AD dereferenced an empty controlled scene manager slot");
    scene.WritePendingCommandAt1C(nameCp932);
}
}
