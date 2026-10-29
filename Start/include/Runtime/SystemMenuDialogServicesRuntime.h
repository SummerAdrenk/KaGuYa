#pragma once

#include <Runtime/SystemMenuDialogRuntime.h>
#include <Runtime/RootLogicalRectangleRuntime.h>

namespace KaGuYa::Start::Runtime
{
class SceneManagerRuntime;
class IWindowModeCommandPlatformApi;
class EnvironmentPostWindowStartupRuntime;
class RootRuntimePayload;

// 将菜单接到同源环境/窗口模式/场景槽；不复制根字段，也不提前缓存待解析对象。
class SystemMenuDialogServicesRuntime final : public ISystemMenuDialogServices,
    private IRootLogicalRectangleSynchronization
{
public:
    SystemMenuDialogServicesRuntime(const WindowEnvironmentRuntime& environment,
        const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
        const std::shared_ptr<SceneManagerRuntime>& sceneSlot,
        const std::uint8_t& settingsEnabledAtRoot219, IModalDialogRunnerServices& modal,
        IRootLogicalRectanglePlatformApi& windowApi, IWindowModeCommandPlatformApi& messages) noexcept;

    // 50F990/509550：同根unique mode、当前发布SceneManager和真实+219 byte；不复制旧槽。
    SystemMenuDialogServicesRuntime(const EnvironmentStartupRuntime& environment,
        const std::unique_ptr<WindowModeManagerRuntime>& modeSlot,
        const EnvironmentPostWindowStartupRuntime& postWindow,
        const std::uint8_t& settingsEnabledAtRoot219, IModalDialogRunnerServices& modal,
        IRootLogicalRectanglePlatformApi& windowApi, IWindowModeCommandPlatformApi& messages) noexcept;

    // 每个原解根位置重新选当前根，不把构造时的payload借用跨换根缓存。
    SystemMenuDialogServicesRuntime(const EnvironmentStartupRuntime& environment,
        RootRuntimeOwner& root, IModalDialogRunnerServices& modal,
        IRootLogicalRectanglePlatformApi& windowApi, IWindowModeCommandPlatformApi& messages) noexcept;
    std::uint8_t ReadCurrentWindowModeAt04() override;
    std::uint8_t ReadSettingsEnabledAtRoot219() override;
    bool IsGameWindowIconic() override;
    RECT ReadRootLogicalRectangleAt478E50() override;
    void PostGameWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) override;
    void WriteSceneManagerPendingCommandAt1C(std::string_view nameCp932) override;

private:
    void SynchronizeRuntimeAt413BE0() override;
    [[nodiscard]] HWND CurrentWindow() const;
    const WindowEnvironmentRuntime* environment_ = nullptr;
    const std::shared_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    const std::shared_ptr<SceneManagerRuntime>* sceneSlot_ = nullptr;
    const EnvironmentStartupRuntime* startupEnvironment_ = nullptr;
    const std::unique_ptr<WindowModeManagerRuntime>* uniqueModeSlot_ = nullptr;
    const EnvironmentPostWindowStartupRuntime* postWindow_ = nullptr;
    const std::uint8_t* settingsEnabledAtRoot219_ = nullptr;
    RootRuntimeOwner* root_ = nullptr;
    IModalDialogRunnerServices& modal_;
    IRootLogicalRectanglePlatformApi& windowApi_;
    IWindowModeCommandPlatformApi& messages_;
    RootLogicalRectangleRuntime rectangle_;
};
}
